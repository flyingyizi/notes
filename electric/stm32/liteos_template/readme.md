

https://seisman.github.io/how-to-write-makefile/overview.html （跟我一起写Makefile 陈皓）

GNU-LD 在线文档https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_mono/ld.html

# 1.liteOS移植

从实践来看， liteOS的版本比较多，它们的移植实践有比较多的区别。 下面对liteOS-master， liteOS-develop，liteOS-lab三种移植过程进行记录。

当前官方推荐的是liteOS-lab。

LiteOS相关的主要代码仓如下：

- 码云gitee的[master分支](https://gitee.com/LiteOS/LiteOS)：官方最新的LiteOS代码仓。

- 码云gitee的LiteOS Studio官方仓库（包括使用手册）。

- github上的[LiteOS代码仓](https://github.com/LiteOS/LiteOS)：跟随码云的master分支，会定期更新同步。

- github上的[LiteOS_Lab代码仓](https://github.com/LiteOS/LiteOS_Lab)：LiteOS_Lab是LiteOS发展过程中衍生出来的一个分支，其代码仓的最新分支为iot_link分支。LiteOS_Lab在官方LiteOS 18年的版本上持续演进，新增了更多的组件和开发板支持。


## 1.1.liteOS-lab版本(iot_link分支)移植

源码下载路径是“https://github.com/LiteOS/LiteOS_Lab”。与该版本配套有个IoT link vscode扩展工具，通过该扩展工具可以方便的对移植进行模块裁剪。

由于安装IoT link会自动下载liteOS-lab,所以我们这就不单独下载“https://github.com/LiteOS/LiteOS_Lab”了，如果单独下载，注意记得设置变量SDK_DIR指向下载的路径。

下面以nucleo stm32F411板子，分步骤介绍下from scratch 从头开始进行liteOS-LAB的移植

### step0 安装IoT link studio

在vscode中搜索IoT link扩展并安装，待安装完成后将会自动安装arm gcc工具以及liteOS-lab SDK。

例如在本机上，该扩展安装的工具与SDK位于：“C:\Users\XX\.iotlink”，其中SDK位于“C:\Users\XX\.iotlink\sdk\IoT_LINK”，后面简称“$(SDK_DIR)”. arm gcc工具位于“C:\Users\XX\.iotlink\tools\gcc-arm-none-eabi”

注：在IoT link环境下，它会自动设置SDK_DIR环境变量。例如它可以正确认识Kconfig文件中出现的该变量。

### step1 构建裸机项目

通过stm32cubeMX 构建一个初始项目，

在“project manager/ project” 中，对“toolchain/IDE”选择：

- “Makefile”

在“project manager/ code generator ”中选择:

- “generate peripheral initialization as pair of .c/.h files per peripheral”
- keep user code when re-generating
- delete previously generated files when not re-generated
- copy all used libraries into the project folder
- 

注意1：如果使用的是SWD接口， 在“pingout &  configuration” 中“sys mode and configuration”里面注意设置debug为“serial wire”使能串行调试功能。否则无法下载和调试代码



假设构建的裸机项目的目录为$(MYPROJECT),后面以该目录进行相关描述

### step2-1 移植准备：Kconfig文件

从$(SDK_DIR)/targets下任意一个项目中拷贝一份Kconfig文件到$(MYPROJECT)根目录，或直接手写一份文件名为Kconfig的文件放在$(MYPROJECT)根目录下，手写文件内如下：

```kconfig
menu "LiteOS for STM32F411 Project configuration For IoT Link"
comment "This is a demo project and hope you enjoy it"
source "$(SDK_DIR)/iot_link/Kconfig"
endmenu
```

Kconfig模板将用于后面生成“iot_config.h/.config”

### step2-2 移植准备： IoT link项目生成

在vscode中安装完成IoT link扩展后，在vscode最下面任务栏会出现“Home”的图标，点击它进入IoT link studio，进入后选择“导入GCC项目”将$(MYPROJECT)导入。

稍等片刻，看到vscode最下面任务栏会出现增加的“build”，“rebuild”的图标，说明IoT link项目已经生成了。同时也可以看到$(MYPROJECT)目录中出现了“.iotlink”子目录。

### step2-3 移植准备： 生成iot_config.h/.config

在vscode中，点击最下面任务栏中的Home图标进入IoT Link studio，点击“IoT link 设置”进入设置界面，在“工程设置/SDK配置”中就可以以Kconfig文件为模板来生成“iot_config.h/.config”了



### step2-4 移植准备： 准备link script

stm32cubeMX会自动生成link script，但它并不适用，因为liteOS需要在link script中增加额外的变量，比如“__bss_end”定义，比如“`__los_heap_addr_start__`, `__los_heap_addr_end__`,`_si_liteos_vector_data`”定义，虽然增加的内容很少,但没有这些定义，是无法编译的。从实践来看，建议采用下面的方式来得到link script：

从$(SDK_DIR)/targets下找一个与自己移植板子MCU接近的项目，以该项目的linkscript 与stm32cubeMX为我们目标板子生成的link script进行compare， 将liteOS 增加的一些定义合并到目标link script。

上面的描述看起来比较麻烦，但其实是比较容易的，因为liteOS额外新增的定义比较少。 

假设通过上面的动作我们已经得到了新的link script ： “$(MYPROJECT)/cortex-m4.ld”

总结：

对stm32cubeMX自动生成的下面端信息,以自动生成的信息为准，其他的以liteOS范例中的为准

```ld
/* Highest address of the user mode stack */
_estack = 0x20018000;    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 96K
FLASH (rx)      : ORIGIN = 0x8000000, LENGTH = 512K
}
```

### step2-5 移植准备： 准备OS_CONFIG

从`$(SDK_DIR)/targets`下找一个与自己移植板子MCU接近的项目，拷贝该项目中的`OS_CONFIG`目录到“`$(MYPROJECT)`”目录下，
即生成了“`$(MYPROJECT)/OS_CONFIG`”。

对“`$(MYPROJECT)/OS_CONFIG/target_config.h`”文件中的mcu头文件替换为与自己MCU配套的头文件，例如在本次移植，正确的头文件是“#include "stm32f4xx.h"”

### step2-6 移植准备： 启用liteOS PendSV_Handler、SysTick_Handler

stm32cubeMX生成的代码中会定义`void PendSV_Handler(void)`, `void SysTick_Handler(void)`， 但在移植liteOS的情况下，它们需要使用liteOS的实现。因此需要对stm32cubeMX生成的“`$(MYPROJECT)\Src\stm32f4xx_it.c`”文件,进行如下修改：

- 在该文件中删除这两个函数的定义
- 或 `__weak void PendSV_Handler(void)`,`__weak void SysTick_Handler(void)`方式删除

显然一旦stm32cubeMX重新生成代码，上面的删除步骤又要做一次。 不记得该动作也影响不大，因为编译会报错，从而提醒去做这个删除动作。

如果不希望在自动生成裸机代码后每次手工删除它们，也可以在stm32cubeMX中进行配置：

- “pingout& configuration”/ NVIC(categories)/code generation ，去掉"time base: system tick timer"勾选，对应去掉"SysTick_Handler"
- “pingout& configuration”/ NVIC(categories)/code generation ，去掉"pendable equest for system service"勾选，对应去掉"PendSV_Handler"

### step2-7 移植准备： 增加内核需要的system_phys_mem

liteOS内核需要引用一个名称为`system_phys_mem`的结构体数组。 在移植时，需要移植程序员定义它。从liteOS-lab给的各targets demo可以看到，它们的内容都是一样的，因此，“`$(MYPROJECT)\Src\main.c`”文件中增加下面的内容：

```c++
/* USER CODE BEGIN Includes */
...
#include "mem.h"
...
/* USER CODE END Includes */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#if defined (__CC_ARM)
extern char Image$$RW_IRAM1$$ZI$$Limit [];
extern char Image$$ARM_LIB_STACKHEAP$$Base [];
#elif defined (__GNUC__)
extern char __los_heap_addr_start__ [];
extern char __los_heap_addr_end__ [];
#else
#error "fix me"
#endif

const struct phys_mem system_phys_mem [] =
    {
#if defined (__CC_ARM)
        { Image$$RW_IRAM1$$ZI$$Limit, Image$$ARM_LIB_STACKHEAP$$Base },
#elif defined (__GNUC__)
        {(unsigned long) __los_heap_addr_start__,(unsigned long) __los_heap_addr_end__ },
#else
#error "fix me"
#endif
        { 0, 0 }
    };
/* USER CODE END 0 */

```

以下代码就将一段物理内存开始地址和结束地址映射为一个名为system_phys_mem的结构体数组
  * 这段内存将来给LiteOS分配使用


### step2-8 移植准备： 避免printf编译错误

在liteOS中，debug、log中有使用print，因此需要实现对应的fputc，_write.

它们的实现，可以直接从`$(SDK_DIR)/targets`下找一个与自己移植板子MCU接近的项目，拷贝fputc，_write实现到自己的项目，或者使用下面的简单实现：

“`$(MYPROJECT)\Src\usart.c`”文件中增加下面的内容，好主意要写到user code中，避免被stm32cubeMX删除

```c++
...
/* USER CODE BEGIN 0 */
#include <stdio.h>
/* USER CODE END 0 */
...
/* USER CODE BEGIN 1 */
#if 1
#if defined ( __CC_ARM ) || defined ( __ICCARM__ )  /* KEIL and IAR: printf will call fputc to print */
int fputc(int ch, FILE *f)
{
    (void)HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}
#elif defined ( __GNUC__ )  /* GCC: printf will call _write to print */
__attribute__((used)) int _write(int fd, char *ptr, int len)
{
    (void)HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, 0xFFFF);
    return len;
}
#endif
 
#endif

/* USER CODE END 1 */
```



### step3 移植makefile书写

在前面的步骤中，已经通过stem32cubeMX生成了裸机项目，该项目带有`$(MYPROJECT)/Makefile`文件。由于当stm32cubeMX regenerate code时，该文件也是会更新的。因此我们期望由于移植改动引起对该文件的变化最小，并且与stm32cubeMX自动更新不冲突，从而不用去再次修改它。基于该考虑对该文件的改动如下, 仅仅增加一行`include **.mk`：

```makefile
...
# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin
#移植liteOS引入，注意它的位置

#add
# must be the first item in addtional including
# because we want to use CONFIG_XX_ENABLE in other *.mk
include .config
include **.mk

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
...
```

下面开始写移植liteOS的变更Makefile部分，假设文件命名为`$(MYPROJECT)/liteos_lab.mk`文件, 它的内容如下， 其中：

```makefile
# 覆盖 stm32cubleMX 自动生成的link script，文件名根据实际名字替换
LDSCRIPT = ./cortex-m4.ld

# 根据liteOS_lab的实际路径替换， 例如下面例子是git clone liteOS_lab 到“C:/xxx/LiteOS_Lab”路径
#we should export the SDK_DIR FOR THE iot.mk  use
ifndef SDK_DIR
    SDK_DIR :=C:/xxx/LiteOS_Lab
endif

DRIVERLIB_DIR := $(SDK_DIR)/drivers
IOTLINK_DIR := $(SDK_DIR)/iot_link

OS_CONFIG_INC = \
        -I ./OS_CONFIG 
        C_INCLUDES += $(OS_CONFIG_INC)       

# for iot_config.h file
IOT_CONFIG_INC = -I . 
        C_INCLUDES += $(IOT_CONFIG_INC)       


##########################LOAD THE SOURCES INCLUDES AND DEFINES#################
###include .config
include $(SDK_DIR)/iot_link/iot.mk
```

#### 附加

stm32cubmx自动生成的makefile 输出非常详细，过于复杂，建议使用`@`屏蔽当前指令输出，并增加"`@echo $(notdir $(<:.c=.o))`"输出文件名字。从而简化输出，例如

```makefile
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	@$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@
	@echo $(notdir $(<:.c=.o))
```

#### c++支持

需要对stl支持时，需要注意添加"`-lsupc++ -lstdc++ `"

参考[Using-std-vector](https://forum.pjrc.com/threads/23467-Using-std-vector)



### step4 移植完成与验证

到此时， 点击vscode 最下面任务栏中的rebuild图标，开始编译，编译结果显示编译成功，这意味着移植已经成功。 下面我们尝试写个listeOS task，

在`$(MYPROJECT)/Src/main.c`文件中增加下面的内容,重新编译，编译显示成功，从而验证完成：

```C++
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "los_base.h"
#include "los_task.ph"
#include "los_typedef.h"
#include "los_sys.h"
#include "los_memory.h"
#include "mem.h"
/* USER CODE END Includes */
...
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void task1(void)
{
  int count = 1;
  while (1)
  {
    printf("This is task1,count is %d\r\n", count++);
    LOS_TaskDelay(1000);
  }
}

/* task pid */
static UINT32 g_TskHandle;

UINT32 creat_task1()
{
  UINT32 uwRet = LOS_OK;
  TSK_INIT_PARAM_S task_init_param;

  task_init_param.usTaskPrio = 0;
  task_init_param.pcName = "task1";
  task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)task1;
  task_init_param.uwStackSize = 0x200;

  uwRet = LOS_TaskCreate(&g_TskHandle, &task_init_param);
  if (LOS_OK != uwRet)
  {
    return uwRet;
  }
  return uwRet;
}

void task2(void)
{
  int count = 1;
  while (1)
  {
    printf("This is task2,count is %d\r\n", count++);
    LOS_TaskDelay(2000);
  }
}
UINT32 creat_task2()
{
  UINT32 uwRet = LOS_OK;
  TSK_INIT_PARAM_S task_init_param;

  task_init_param.usTaskPrio = 1;
  task_init_param.pcName = "task2";
  task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)task2;
  task_init_param.uwStackSize = 0x200;

  uwRet = LOS_TaskCreate(&g_TskHandle, &task_init_param);
  if (LOS_OK != uwRet)
  {
    return uwRet;
  }
  return uwRet;
}

/* USER CODE END 0 */
...
int main(void)
{
...
  /* USER CODE BEGIN 2 */
  UINT32 uwRet = LOS_OK;
  uwRet = LOS_KernelInit();
  if (uwRet != LOS_OK)
  {
    return LOS_NOK;
  }
  uwRet = creat_task1();
  if (uwRet != LOS_OK)
  {
    return LOS_NOK;
  }
  uwRet = creat_task2();
  if (uwRet != LOS_OK)
  {
    return LOS_NOK;
  }
  LOS_Start();

  /* USER CODE END 2 */
...
}
```

### step5 附加：配合组件化

如同旧版本liteOS一样去使用liteOS-lab当然是可以的，但这样就失去了liteOS-lab 配置化裁剪组件的好处。

通过Kconfig模板配置组件生成“iot_config.h/.config”后，重新编译就配置了liteOS-lab的集成组件，这种便利建议还是要使用。

与之配套的，组件的配置完成，需要一个名为“link_main”的任务，该任务定义在`$(SDK_DIR)/iot_link/link_main.c`中，该函数中会根据我们的条件编译宏来选择使能哪些组件（定时器、网络、各类Demo）。在我们的主程序中需要显示的执行该任务。如果有多个任务，建议该任务第一个执行。

下面的代码演示了执行该任务

```c++
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern int link_main(void *args);
static int link_test()
{
    int ret = -1;
    UINT32 uwRet = LOS_OK;
    UINT32  handle;
    TSK_INIT_PARAM_S task_init_param;

    (void) memset (&task_init_param, 0, sizeof (TSK_INIT_PARAM_S));
    task_init_param.uwArg = (unsigned int)NULL;
    task_init_param.usTaskPrio = 2;
    task_init_param.pcName =(char *) "link_main";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)link_main;
    task_init_param.uwStackSize = 0x1000;
    uwRet = LOS_TaskCreate(&handle, &task_init_param);
    if(LOS_OK == uwRet){
        ret = 0;
    }
    return ret;
}
/* USER CODE END 0 */

int main(void)
{
...
  /* USER CODE BEGIN 2 */
  if (LOS_KernelInit() != LOS_OK)  {
    return LOS_NOK;
  }  
  ...
  if (0!=link_test()) {
    return  LOS_NOK;
  }
  ...
  LOS_Start();

  /* USER CODE END 2 */
```

/* 导入Demo的函数link_main，该函数的定义在iot_link/link_main.c中
  * 该函数中最终调用standard_app_demo_main();启动特定Demo中的任务
  * 大家可以进到Demo文件夹下，每个Demo中都有这个standard_app_demo_main();函数的定义，这就是统一接口接口化编程的实例
  */

### 使用stm32cube IDE

前面已经创建了makefile工程， 这个工程可以通过import导入stm32cube IDE

此时"c/c++build-->builder settings-->"中的“generate makefiles automatical”是非勾选状态的。如果勾选了，我们前面准备的makfile就没有用了。

### 使用vscode iot LINK UI

这里仅仅对debug增加一些说明

因为nucleo板子的固件是stlink，所以无法支持jlink。因此如果基于iot link（vscode）进行调试，并且不使用openOCD,则需要对nucleo板子还需要额外的动作：使得nucleo fireware支持jlink，具体指导见[官方说明](https://www.segger.com/products/debug-probes/j-link/models/other-j-links/st-link-on-board/)，需要下载STLinkReflash.exe刷新固件。

官方stm32cubeide集成工具使用的是ST-LINK_gdbserver， 具体使用说明见“google  ST-LINK_gdbserver”

使用实例[使用VSCode和VS2017编译调试STM32程序 ](https://www.cnblogs.com/WeyneChen/p/8379214.html)

注意： 一旦通过上面方式更改为jlink，那就不再支持stlink。如果要恢复stlink，需要通过STLinkReflash.exe restore功能重置为stlink，并且根据实践这个恢复动作往往不成功，表现为stm32cubeide识别不了，因此还需要一个额外动作： stm32cubeide help/stlink upgrade 进行升级refresh。之后才OK。

## 1.2.liteOS版本(github代码仓)移植

[官网移植教程](https://support.huaweicloud.com/bestpractice-LiteOS/zh-cn_topic_0145350112.html),其实对应的就是liteOS-develop 版本的移植。 下载liteOS源码应采用`git clone -b develop https://github.com/LiteOS/LiteOS.git` 方式下载liteOS。

1、“demo-with-develop”目录对应的liteOS为使用`git clone -b develop https://github.com/LiteOS/LiteOS.git` 下载的liteOS，参考的教程是 [STM32L431移植LiteOS 手把手教程](https://bbs.huaweicloud.com/forum/thread-12430-1-1.html) 


## 1.4.liteOS版本(gitee master)移植

当前这个属于最新的liteos版本移植。 对应所需编译环境见[liteOS官网](https://support.huaweicloud.com/LiteOS/)中环境安装说明, 本次移植是基于[官方移植说明](https://support.huaweicloud.com/porting-LiteOS/zh-cn_topic_0314628477.html)进行操作的。本次是移植到STM32F411RETx-neucleo，即新增一个STM32F411RETx target。

从操作过程来看，与历史liteos版本移植有很大不同。 这里对官方移植说明进行一些自己认为缺失的补充说明。

### 浮点支持： 

`project config(F4)`中没有查找到配置的地方，所以先临时在.config中手动配置`LOSCFG_ARCH_FPU_ENABLE=y`. 如果没有设置，则会出现类似“selected processor does not support `vldmia r0!,{d8-d15}' in Thumb mode”问题

### lowpower

- lowpower：

  ```
  1. ifconfig 操作：
  首先勾选：kernel  -> enable low power management framework
  基于第一步，会出现"kernel->low power management configure" menu。 在该menu中勾选“ enable default mplementation of low poser manager framework
  ”
  2. 确认结果
  在.config 中确认下面三个配置项目的值为y
  LOSCFG_KERNEL_LOWPOWER=y
  #
  # Low Power Management Configure
  #
  LOSCFG_KERNEL_TICKLESS=y
  LOSCFG_KERNEL_POWER_MGR=y
  ```

### ifconfig补充

- 由于liteos是使用ifconfig进行配置的，因此新增一个新的芯片（开发板）支持，首先需要对该开发板提供默认配置。具体涉及动作如下：

  - 新增默认配置文件： 在`$(LITEOSTOPDIR)/tools/build/config/`目录下找一个类似配置文件拷贝并重命名为`${platform}.config`，作为该开发板的的默认配置文件
  - 以上一步得到的默认配置文件拷贝并重命名为".config"放置到`$(LITEOSTOPDIR)/`目录下。
  - 经过以上两步，就对该开发板使能ifconfig配置了。

- 如果需要支持"C++"，对应配置是`project config(F4)->component->kernel-> 选择支持c++`

- “$(LITEOSTOPDIR)\targets\STM32F411RETx\include\menuconfig.h” 文件是`project config(F4)`运行后自动生成的头文件。

  在这个文件中，类似下面两个macro很重要，并且它们是根据我们在`project config(F4)`根据芯片新增的board信息自动生成的。 在下一步“添加新开发板到系统配置中”，会经常使用到它们。
  ```c++
  #define LOSCFG_PLATFORM "STM32F411RETx"
  #define LOSCFG_PLATFORM_STM32F411RETx 1
  ```

### 新增开发板到liteos 系统配置中
- “$(LITEOSTOPDIR)\targets\bsp.mk”与“$(LITEOSTOPDIR)\targets\bsp\Makefile” 都需要执行[添加新开发板到系统配置中](https://support.huaweicloud.com/porting-LiteOS/zh-cn_topic_0314628532.html)中描述的修改。

  例如，本次仿照中新增了下面语句
  ```makefile
  # 在“$(LITEOSTOPDIR)\targets\bsp.mk”
  ######################### STM32F411RETx Options #######################
  else ifeq ($(LOSCFG_PLATFORM_STM32F411RETx), y)
      LITEOS_CMACRO_TEST += -DSTM32F411xE
      HAL_DRIVER_TYPE := STM32F4xx_HAL_Driver
  ```

  ```makefile
  # “$(LITEOSTOPDIR)\targets\bsp\Makefile”, 内容是根据stm32MX makefile中的hal列表
  else ifeq ($(LOSCFG_PLATFORM_STM32F411RETx), y)
  STM32F411RETX_HAL_SRC = \
      Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
      Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
      Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
      Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
      Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
      Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
      Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
      Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
      Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
      Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
      Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
      Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
      Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
      Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
      Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
      Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c 
      LOCAL_SRCS = $(STM32F411RETX_HAL_SRC)
  ```

### link script

通常我们使用裸机创建的ld script，然后与liteos SDK已经包含的范例开发板进行比较，书写一个我们移植开发板的ld script

注意 “$(LITEOSTOPDIR)\build\mk\los_config.mk” 中对链接文件的名字固定为"liteos.ld"，所以在移植中要遵守这一点

### stm32 HAL driver

- 通过stm32MX模板生成的裸机项目，其中Drivers目录可以不用，因此liteos中已经包含了HAL，例如“$(LITEOSTOPDIR)\targets\bsp\drivers\STM32F4xx_HAL_Driver”。 如果还是希望使用stm32MX模板携带的HAL，那就要修改“$(LITEOSTOPDIR)\targets\bsp.mk”，看了下，内容不是很多，应该比较容易修改。

- 对stm32 可以使用HAL或LL, 如果使用LL，则需要注意它的头文件、src都依赖“-DUSE_FULL_LL_DRIVER”，这点和HAL不同，HAL仅仅是头文件依赖“-DUSE_HAL_DRIVER”，下面的例子是使用了LL的“$(LITEOSTOPDIR)\targets\bsp.mk”
```makefile
######################### STM32F411RETx Options #######################
else ifeq ($(LOSCFG_PLATFORM_STM32F411RETX_NUCLEO), y)
    #由于LITEOS_CMACRO直接影响CFLAGs,因此对他赋值，可以避免在模块中重复去定义STM32F411xE
    LITEOS_CMACRO += -DSTM32F411xE
    HAL_DRIVER_TYPE := STM32F4xx_HAL_Driver
    PLATFORM_DEFS += -DUSE_FULL_LL_DRIVER
endif
```

### 书写main

从startup.s可以看到 , 先调用SystemInit，再调用main. 其中在SystemInit中会设置中断表等信息。
```c++
/* Call the clock system intitialization function.*/
  bl  SystemInit   
/* Call static constructors */
    bl __libc_init_array
/* Call the application's entry point.*/
  bl  main
```

但是一旦执行了liteos的OsMain，它会调用HalIrqInit执行“`SCB->VTOR = (UINT32)g_hwiVec;`”将中断向量表起始地址给替换掉

通常我们不使用stm32MX创建裸机project生成的main，而是使用liteos 中范例开发板的main进行修改得到我们新增开发板的main。 main()中最核心的是要包含OsMain()。

- normal: OsMain()->OsAppInit() ->OsAppTaskCreate()->创建任务（该任务入口是app_init）
- test:   OsMain()->OsTestInit()->创建任务（该任务入口是TestTaskEntry）

```c++
LITE_OS_SEC_TEXT_INIT UINT32 OsMain(VOID)
{
    ...

#ifdef LOSCFG_PLATFORM_OSAPPINIT
    ret = OsAppInit();
#else /* LOSCFG_TEST */
    ret = OsTestInit();
#endif
    ... 

    return LOS_OK;
}

```


下面是新增加的app_init函数，其中DemoEntry包含了liteos sdk已经写好的各个demo。要启用它们，在ifconfig中可以使能各个demo
```c++
#include "sys_porting.h"
#include "demo_entry.h"

VOID app_init(VOID)
{
    printf("app init!\n");
    DemoEntry();
}
```

建议：
- 裸机生成代码时，指示stm32cubeMX不生成main, 即勾选 `do not generate the main()` . 在user code部分增加我们自己的main()

### 网络数据流

以at为例，下面是at net启用的过程
1. app_init->DemoEntry->AgenttinyDemoTask(entry是AtinyDemoTaskEntry)-> 当启用at时会调用Esp8266Register->at_api_register(填写gp_at_adaptor_api，后续at_api_xx api都是使用该结构)

2. 对sal 的api atiny_net_xxx， 如果是启用at，那就是调用对应的at_api_xx； 如果是启用lwip或指明linux，那就是不是调用at_api_xx,而是posix net api

从上面也可以看出，我们配置时，如果启用at，那就不应启用lwip

### shell 数据流

DemoEntry ->OsShellInit->OsShellCreateTask-> 调用：1. ShellTaskInit(创建入口是ShellTask的task); 2. ShellEntryInit


ShellEntryInit->ShellEntry->ShellStdinLoop, 在ShellStdinLoop中无限循环从uart读取数据



### module makefile说明

大家观察sdk中的module makefile，大致都以下几种类型

- 类型1：
```makefile

include $(LITEOSTOPDIR)/config.mk
# 在$(MODULE)中MODULE_NAME将作为生成目标的名字
MODULE_NAME := dtls_server

LOCAL_SRCS :=
LOCAL_INCLUDE :=

LOCAL_INCLUDE += 

LOCAL_SRCS = 
LOCAL_FLAGS := 
# $(MODULE)是一个mk名字（即$(LITEOSTOPDIR)/build/mk/module.mk），在los_config.mk定义，$(MODULE)中定义了makefile compile rule
include $(MODULE)
```

- 类型2：

```makefile

include $(LITEOSTOPDIR)/config.mk
# 在$(MODULE)中MODULE_NAME将作为生成目标的名字
MODULE_NAME :=

MODULE_y :=
MODULE_y += xxx

# $(MODULE)是一个mk名字（即$(LITEOSTOPDIR)/build/mk/module.mk），在los_config.mk定义，$(MODULE)中定义了makefile compile rule
include $(MODULE)
```

- 类型3：

  他是类型1与类型2的结合，代表了它不但编译LOCAL_SRCS 列表，编译结果为$(MODULE)， 另外还编译由MODULE_y 目录列表所代表的各个子模块
```makefile

include $(LITEOSTOPDIR)/config.mk
# 在$(MODULE)中MODULE_NAME将作为生成目标的名字
MODULE_NAME := dtls_server

LOCAL_SRCS :=
LOCAL_INCLUDE :=

LOCAL_INCLUDE += 

LOCAL_SRCS = 
LOCAL_FLAGS := 

MODULE_y :=
MODULE_y += xxx


# $(MODULE)是一个mk名字（即$(LITEOSTOPDIR)/build/mk/module.mk），在los_config.mk定义，$(MODULE)中定义了makefile compile rule
include $(MODULE)
```

其中命名为“MODULE_y” 是作为特别独立模块处理的，它们会被自动链接（解释见下面的“module编译结果的使用”）。处理代码见“$(LITEOSTOPDIR)/build/mk/module.mk”的下面动作执行编译
```makefile
SUB_MODULE_BUILD: $(MODULE_y)
	$(HIDE) for dir in $(MODULE_y); 		\
		do $(MAKE) -C $$dir all || exit 1; 	\
	done
```

对module编译结果的使用，分为两种情况：

- 施加到“MODULE_y”的，在“$(LITEOSTOPDIR)/build/mk/module.mk”中是以“SUB_MODULE_BUILD”目标标识的，有个专门的动作针对它：
  ```makefile
  $(LIBA): $(LOCAL_OBJS) SUB_MODULE_BUILD
    $(HIDE)$(OBJ_MKDIR)
    $(call add_lib_to_baselib_file,$(MODULE_NAME))
  ```
  其中的`add_lib_to_baselib_file`行为会收集它们的列表记录到`$(BASELIB_FILE)`。LD在链接的时候会去链接它们。 这意味着只要是按照“MODULE_y”来构建的module，liteos build mk会自动去链接它们。


- 对不是“MODULE_y”施加构建的模块，那就要手工添加到“$(LITEOSTOPDIR)/targets/bsp.mk”中的`$(LITEOS_BASELIB)`里面，比如
  ```makefile
      ifeq ($(LOSCFG_DEMOS_GUI), y)
          LITEOS_BASELIB += -lgui_demo
      endif
  ```
  LD在链接的时候会去链接`$(LITEOS_BASELIB)`. 当然，有些是肯定会去链接的，它们在“$(LITEOSTOPDIR)/targets/bsp.mk”中已经写好了，例如`LITEOS_BASELIB += -l$(LITEOS_PLATFORM) -lsec -lbase -linit -lbsp -lc -lm -losdepends`这些就不用你再手动写了。
 
  例外：上面对`$(LITEOS_BASELIB)`描述，是针对linux环境，如果是非linux环境，自己增加的module_y，需要手工在“$(LITEOSTOPDIR)/targets/bsp.mk”中添加
  ```makefile
  #determine libs to link for windows
  ifneq ($(OS), Linux)
      LITEOS_BASELIB += -l$(LITEOS_PLATFORM) -lsec -lbase -linit -lbsp -lc -lm -losdepends
      ...
  ```

  因为自动更新`$(LITEOS_BASELIB)`的脚本仅仅支持在linux环境下使用，相关代码见“$(LITEOSTOPDIR)/Makefile”中的下面语句：
  ```makefile
  $(LITEOS_TARGET):
  ifeq ($(OS), Linux)
    $(call update_from_baselib_file)
  endif
  ```


### 启用at esp8266

需要启用下面的模块：`components->network-> enable AT, choose AT device, enable sal `

对应使得.config 改变如下：
```diff
$ git diff
diff --git a/.config b/.config
index 4347af5..8831c11 100644
--- a/.config
+++ b/.config
@@ -221,9 +221,9 @@ LOSCFG_LIB_ZLIB=y
 #
 # Network
 #
-# LOSCFG_COMPONENTS_NET_AT is not set
+LOSCFG_COMPONENTS_NET_AT=y
-# LOSCFG_COMPONENTS_NET_SAL is not set
+LOSCFG_COMPONENTS_NET_SAL=y
-# LOSCFG_COMPONENTS_NET_AT_ESP8266 is not set
+LOSCFG_COMPONENTS_NET_AT_ESP8266=y
-# LOSCFG_COMPONENTS_NETWORK is not set
+LOSCFG_COMPONENTS_NETWORK=y
```

liteos SDK提供的BSP， 几个配置是写死在代码中，因此下面改为条件定义，使得它们可以在makefile中define它们的值
```diff
diff --git a/components/net/at_device/wifi_esp8266/esp8266.h b/components/net/at_device/wifi_esp8266/esp8266.h
index 424f49f..f92d2b8 100644
--- a/components/net/at_device/wifi_esp8266/esp8266.h
+++ b/components/net/at_device/wifi_esp8266/esp8266.h
@@ -31,11 +31,16 @@

 #include "at_frame/at_main.h"

+#ifndef WIFI_SSID
 #define WIFI_SSID                 "HWTEST"
+#endif
+#ifndef WIFI_PASSWD
 #define WIFI_PASSWD               ""
-
+#endif
 #define AT_MODU_NAME              "ESP8266"
+#ifndef AT_USART_PORT
 #define AT_USART_PORT             2
+#endif
 #define AT_BUARDRATE              115200
 #define AT_CMD_TIMEOUT            10000 // ms
 #define AT_MAX_LINK_NUM           4

```

```diff
diff --git a/targets/STM32F411RETx/Makefile b/targets/STM32F411RETx/Makefile
@@ -50,9 +50,10 @@ C_DEFS += \
     -DSTM32F411xE \
     -DNDEBUG \
     -D__LITEOS__ \
-    -D_ALL_SOURCE
-
-
+    -D_ALL_SOURCE \
+    -DAT_USART_PORT=6 \
+    -DWIFI_SSID="testssid"  \
+    -DWIFI_PASSWD=""

```

另外， 我们本次移植的STM32F411xE开发板，是不支持类似USART3,4...的，为了编译通过，针对该芯片增加下面的条件编译
```diff
diff --git a/targets/bsp/drivers/at_hal/at_hal.c b/targets/bsp/drivers/at_hal/at_hal.c
index cebe7d4..2f683b5 100644
--- a/targets/bsp/drivers/at_hal/at_hal.c
+++ b/targets/bsp/drivers/at_hal/at_hal.c
@@ -49,6 +49,20 @@ static void at_usart_adapter(uint32_t port)
     g_atIRQn = LPUART1_IRQn;
     return;
 #endif
+
+#ifdef  STM32F411xE
+    switch (port) {
+        case 6: // 6: usart6
+            g_atUSART = USART6;
+            g_atIRQn = USART6_IRQn;
+            break;
+        case 2: // 2: usart2
+            g_atUSART = USART2;
+            g_atIRQn = USART2_IRQn;
+            break;
+    }
+    return;
+#else
     switch (port) {
         case 1: // 1: usart1
             g_atUSART = USART1;
@@ -70,6 +84,8 @@ static void at_usart_adapter(uint32_t port)
         default:
             break;
     }
+#endif
+    return;
 }
```

### 中断号

在liteos中使用中断有两种方式： 使用stm32 裸机中断； 使用liteos 封装的中断（LOS_HwiCreate）

对裸机中断，中断ISR是放在“.isr_vector” section中，对liteos中断ISR是放在“.data.vector”section中。

如果使用裸机中断，中断号显然直接使用stm32 hal中定义的中断，比如TIM3_IRQn。

如果使用使用liteos封装的中断，中断号需要使用类似`#define TIM_IRQ                 (TIM3_IRQn + 16) // 16: cortex-m irq shift`。 这里magic number 16的原因是因为“#define NVIC_USER_IRQ_OFFSET          16”的缘故，查看cortex-m的类似“__NVIC_GetVector(IRQn_Type IRQn)”api可以看到它的使用。 依据16这个值在liteos中有很多地方是依赖它的，例如：

- 在liteos限定中断号的合法范围是`[OS_USER_HWI_MIN, OS_USER_HWI_MAX]`

  在“$(LITEOSTOPDIR)\drivers\interrupt\include\hal_hwi.h” 中有设定：
  ```c++
  #if defined(LOSCFG_CORTEX_M_NVIC)
  #define OS_USER_HWI_MIN                 15
  #else
  #define OS_USER_HWI_MIN                 0
  #endif

  #if defined(LOSCFG_PLATFORM_HIFIVE1_REV1_B01)
  #define OS_USER_HWI_MAX                 (OS_HWI_MAX_NUM - 1)
  #else
  #define OS_USER_HWI_MAX                 (LOSCFG_PLATFORM_HWI_LIMIT - 1)
  #endif

  #define HWI_NUM_VALID(num)              (((num) >= OS_USER_HWI_MIN) && ((num) <= OS_USER_HWI_MAX))
  ```
  上面使用到的15这个数字，不是随便写的。而是根据“Cortex-M4 Processor Exceptions Numbers”的范围来取的一个值。将OS_USER_HWI_MIN值定为15，当我们将“16: cortex-m irq shift”约定为16，显然将“`SysTick_IRQn                = -1,     /*!< 15 Cortex-M4 System Tick Interrupt`”作为用户态的中断号


-  

### 启用shell

需要启用shell，需要配套的usart bsp移植，即“$(LITEOSTOPDIR)\drivers\uart\src\arm_generic\uart_debug.c”中代码的适配。

shell使用说明见"$(LITEOSTOPDIR)\shell\README_CN.md"

### lwip

[Huawei LiteOS LwIP API Reference](https://usermanual.wiki/Document/Huawei20LiteOS20LwIP20Developer20Guide.144895066/view)

lwip部分已升级为在线组件形式，需要调用在线组件脚本，当前stuido还未支持在线组件。



#### uart

    ifeq ($(LOSCFG_DRIVERS_SIMPLE_UART), y)
        LITEOS_BASELIB += -luart
    endif

MODULE_$(LOSCFG_DRIVERS_UART_ARM_PL011) += src/arm_pl011
MODULE_$(LOSCFG_DRIVERS_UART_CSKY_PORT) += src/csky

ifneq ($(findstring y, $(LOSCFG_DRIVERS_UART_CSKY_PORT) $(LOSCFG_DRIVERS_UART_ARM_PL011)), y)
    LOCAL_SRCS_$(LOSCFG_DRIVERS_SIMPLE_UART) += src/arm_generic/uart_debug.c
endif

### 书写testcase
见`### 书写main`中描述。

下面记录了testcase如何被引入到$(LITEOSTOPDIR)/makefile中被编译的，显然只要配置了`LOSCFG_PLATFORM_OSAPPINIT`，就会编译testcase：

- 1. 首先有下面的文件包含关系：
```
"$(LITEOSTOPDIR)/Makefile.mk"
     |--"$(LITEOSTOPDIR)/build/mk/config.mk"
        |--"$(LITEOSTOPDIR)/build/mk/los_config.mk"
```
- 2. 在"$(LITEOSTOPDIR)/build/mk/los_config.mk"中有如下内容，该文件:
```makefile
...
ifeq ($(LOSCFG_PLATFORM_OSAPPINIT), y)
else
    -include $(LITEOSTOPDIR)/test/test.mk
endif
...
```

- 3. 我们查看“$(LITEOSTOPDIR)/test/test.mk”，可以指导将相应模块追加到`LIB_SUBDIRS`， 而我们知道在"$(LITEOSTOPDIR)/Makefile.mk"中会

```makefile
	$(HIDE)for dir in $(LIB_SUBDIRS); \
		do $(MAKE) -C $$dir all || exit 1; \
```
  因此我们知道了编译testcase的过程。


## JTAG 与SWD

两种接口的全接口都是20针，但通常都不会用那么多 [对比](https://img-blog.csdnimg.cn/20181030143036713.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3UwMTMyNzMxNjE=,size_16,color_FFFFFF,t_70)

```TEXT
        JTAG       SWD
1/2     VCC        VCC
3       TRST       N/A
5       TDI        N/A
7       TMS        SWDIO
9       TCLK       SWCLK
11      RTCLK      N/A
13      TDO        SWO
15      RESET      RESET
```

通常JTAG就只接  VCC(1), TMS(7),GND, TCLK(9),RST(15)

通常SWD就只接   VCC(1), DIO(7),GND,SWCLK(9),RST(15)

ST-Link/V2 协议支持 JTAG/SWD标准接口，但在20针中对的接口中JTAG/SWD定义为GND的接口做了[重定义](https://blog.csdn.net/wxbluesun/article/details/94738272?utm_medium=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-2.channel_param&depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-2.channel_param)，如

USART-RX(4), USART-TX(6)




# liteOS使用

## 任务

[官方指导](https://support.huaweicloud.com/kernelmanual-LiteOS/zh-cn_topic_0145350126.html)

注意这个任务是“普通任务”，不同于中断服务函数。中断服务函数是一种需要特别注意的上下文环境，它运行在非任务的执行环境下（一般为芯片的一种特殊运行模式（也被称作特权模式））



在LiteOS 系统中，每一个任务都是独立的，它们的运行环境都单独的保存在任务栈空
间中。那么在定义好任务函数之后，还要为任务定义一个栈，任务的栈占用的是MCU 内
部的RAM，当任务越多的时候，需要使用的栈空间就越大，即需要使用的RAM 空间就越
多。一个MCU 能够支持多少任务，就得看它的RAM空间有多少。

`LOSCFG_BASE_CORE_TSK_LIMIT`配置系统支持的最大任务数（包括空闲任务），当不需要那么
多任务的时候可以进行裁剪，以减少系统的内存开销。

任务通过`LOS_TaskCreate`创建，例如：

```c
UINT32 creat_task2()
{
  UINT32 uwRet = LOS_OK;
  TSK_INIT_PARAM_S task_init_param;

  task_init_param.usTaskPrio = 1;  //任务优先级, 数值越小，优先级越高
  task_init_param.pcName = "task2";  //任务名
  task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)task2;  //任务入口函数
  task_init_param.uwStackSize = 0x200;  //任务栈大小

  uwRet = LOS_TaskCreate(&g_TskHandle, &task_init_param);
  if (LOS_OK != uwRet)
  {
    return uwRet;
  }
  return uwRet;
}
```

注意任务优先级指定的值应该在(LOS_TASK_PRIORITY_HIGHEST, LOS_TASK_PRIORITY_LOWEST)之间，这两个值在target_config.h中定义



### 定义任务

通常每个独立的任务都是一个无限循环，不能返回。 如果任务只执行一次之后就不会再次使用的话，那就
进行删除任务，这样子不会影响系统的运行，所以只执行一次的任务，对应也不需要无穷循环，在执行完毕要记得及时删除。

例如：
```c
void task2(void)
{
  int count = 1;
  while (1)
  {
    printf("This is task2,count is %d\r\n", count++);
    LOS_TaskDelay(2000);
  }
}
```
### 任务ID

且具有唯一性。在任务创建时通过参
数返回给用户，用户可以通过任务ID 对指定任务进行任务挂起、任务恢复、查询任务名等
操作

例如，`uwRet = LOS_TaskCreate(&g_TskHandle, &task_init_param);`, 后面通过g_TskHandle来操纵这个创建的任务

比如通过`LOS_TaskDelete`关闭任务

### 任务控制块

### 常用任务函数

任务创建函数 LOS_TaskCreate()

任务删除函数 LOS_TaskDelete ()

任务延时函数 LOS_TaskDelay ()，作用是将调用延时函数的任务进入阻塞态，这样子，其它任务优先级较低的任务就能完成任务切换。延时的基本单位为tick，也就是系统
节拍， 配置LOSCFG_BASE_CORE_TICK_PER_SECOND 即可改变tick 的值， 如果
LOSCFG_BASE_CORE_TICK_PER_SECOND 配置为1000，那么tick 为1 个ms


任务挂起函数 LOS_TaskSuspend ()，被挂起的任务绝不会得到处理器使用权，不管该任务具有什么优先级。

任务恢复函数 LOS_TaskResume ()，与LOS_TaskSuspend配套

锁任务LOS_TaskLock，锁住任务，防止高优先级任务调度. 配套对应的是解锁任务LOS_TaskUnlock，让任务按照优先级进行调度。


### 空闲任务

空闲任务是LiteOS 系统中没有其他工作进行时自动进入的系统任务。开发者可以通过
宏定义LOSCFG_KERNEL_TICKLESS 与LOSCFG_KERNEL_RUNSTOP 选择自己需要的
特殊功能，如低功耗模式，睡眠模式等。不过需要注意的是，空闲任务是不允许阻塞也不
允许被挂起的，空闲任务是唯一一个不允许出现阻塞情况的任务，因为LiteOS 需要保证系
统永远都有一个可运行的任务。

LOSCFG_KERNEL_RUNSTOP 是运行停止配置项，默认不使用。

LOSCFG_KERNEL_TICKLESS 是配置LiteOS 内核不使用滴答定时器，而工程项目需要使用滴答定时器的，所以这个宏定义为NO。

任务控制块TCB 包含了任务上下文栈指针（stack
pointer）、任务状态、、任务ID、任务名、等信息。TCB 可以反映
出每个任务运行情况

# liteOS components

## network

[官方端云互通](https://support.huaweicloud.com/sdkreference-LiteOS/zh-cn_topic_0147911562.html)

[基于STM32的ESP8266使用教程](http://www.stmcu.org.cn/module/forum/thread-616077-1-1.html)

在主程序main.c中增加下面 代码
```C++
#if defined WITH_AT_FRAMEWORK
#include "at_frame/at_api.h"
#endif


#if defined(WITH_AT_FRAMEWORK) && (defined(USE_ESP8266) || defined(USE_SIM900A)) 
     // esp8266_interface defined in "Middlewares\LiteOS\components\net\at_device\wifi_esp8266\esp8266.c"
     extern at_adaptor_api esp8266_interface; 
     at_api_register(&esp8266_interface); //注册开发者定义的接口
     //LiteOS SDK端云互通组件的入口函数。该接口将进行agent tiny的初始化相关操作，创建上报任务，并调用agent tiny主函数体。
     //agent_tiny_entry();
 #endif
```

## 消息队列

LiteOS 中使用队列数据结构实现任务异步通信工作.

队列可以在任务与任务间、中断和任务间传送信息，实现了接收来自任务或中断的不固定长度的消息，并根据不同的接口选择传递消息是否存放在自己空间。任务能够从队列里面读取消息，当队列中的消息是空时，读取消息的任务将被阻塞，用户还可以指定阻塞的任务时间uwTimeOut，在这段
时间中，如果队列为空，该任务将保持阻塞状态以等待队列数据有效

队列支持FIFO,LIFO

LiteOS 为我们提供了两种队列的数据传输方式，一种是拷贝方式，拷贝就是我们
常说的拷贝（复制），另一种是引用方式，就只传递了指向数据存储地址的指针

### 常用队列函数


使用队列模块的典型流程如下。
（1） 创建消息队列LOS_QueueCreate。
（2） 创建成功后，可以得到消息队列的ID 值。
（3） 写队列操作函数LOS_QueueWrite(指针方式)。或 LOS_ QueueWriteCopy（带拷贝写入），注意其中的等待时间，等待时间，其值范围为0~LOS_WAIT_FOREVER
（4） 读队列操作函数LOS_QueueRead。或LOS_ QueueReadCopy
（5） 删除队列LOS_QueueDelete。

两个读取的区别见下面例子，指针方式是提供一个指针（不需要分配空间），而拷贝读是预先分配好空间

```c++
 UINT32 uwRet = LOS_OK;/* 定义一个创建队列的返回类型，初始化为创建成功的返回值 */
 UINT32 Test_Queue_Handle = 0;
 /* 创建一个测试队列*/
 uwRet = LOS_QueueCreate("Test_Queue", /* 队列的名称，保留，未使用*/
                            128, /* 队列的长度 */
                            &Test_Queue_Handle, /* 队列的ID(句柄) */
                            0, /* 队列模式，官方暂时未使用 */
                            16); /* 最大消息大小（字节）*/
 if (uwRet != LOS_OK)
 {
 printf("Test_Queue 队列创建失败！\n");
 }

 //发送数据
 UINT32send_data2 = 2; /* 写入队列的第二个数据 */
 /* 将数据写入（发送）到队列中，等待时间为 0 */
 uwRet = LOS_QueueWrite(Test_Queue_Handle, /* 写入的队列ID */
 &send_data1, /* 写入（发送）的数据 */
 sizeof(send_data1),/* 数据的大小 */
 0); /* 等待时间为 0 */

// 读取
 UINT32 uwRet = LOS_OK;
 UINT32 *r_queue = NULL; /* r_queue 地址作为队列读出来的存放地址 */
 /* 队列读取（接收），等待时间为一直等待 */
 uwRet = LOS_QueueRead(Test_Queue_Handle,/* 读取队列的ID(句柄) */
                        &r_queue, /* 读取（接收）的数据保存位置 */
                        sizeof(r_queue),/* 读取数据的长度 */
                        LOS_WAIT_FOREVER); /* 等待时间：一直等 */
 if (LOS_OK == uwRet) {
 printf("本次接收到的数据是%d\n",*r_queue);
 } else {
 printf("数据接收出错\n");
 }

// 或copy读
//  UINT32 r_queue; /* r_queue 地址作为队列读出来的存放地址 */
//  UINT32 buffsize; /*把读取到的数据的大小写在这个指针指向的空间里*/
//  /* 队列读取（接收），等待时间为一直等待 */
//  uwRet = LOS_ QueueReadCopy (Test_Queue_Handle,/* 读取队列的ID */
//                                 &r_queue, /* 读取（接收）的数据保存位置 */
//                                 &buffsize, /* 保存读取的数据的长度 */
//                                 LOS_WAIT_FOREVER); /* 等待时间：一直等 */


 /* 删除队列 */
 uwRet = LOS_QueueDelete(Test_Queue_Handle); 
 if (uwRet != LOS_OK) /* 删除队列失败，返回其它错误码 */
 {
 printf("删除队列失败！\n");
 } else /* 删除队列成功，返回LOS_OK */
 {
 printf("删除队列成功！\n");
 }

```

## 信号量

如果需要使用信号量，需要配置`LOSCFG_BASE_IPC_SEM`为YES。 另外还需要配置`LOSCFG_BASE_IPC_SEM_LIMIT`为合适的数量。

LOSCFG_BASE_IPC_SEM这个宏定义表示信号量的配置项，配置为YES 则表示默认使用信号量

LOSCFG_BASE_IPC_SEM_LIMIT 这个宏定义表示LiteOS 最大支持信号量的个数，默认为20 个，用户可以自定义设置信号量个数。

例子见“Middlewares\LiteOS\demos\kernel\api\los_api_sem.c”



### 二值信号量

二值信号量创建函数 LOS_BinarySemCreate()， `LITE_OS_SEC_TEXT_INIT UINT32 LOS_BinarySemCreate (UINT16 usCount/*信号量可用个数，只能是1*/, UINT32 *puwSemHandle/*信号量ID*/)`.

查看LOS_BinarySemCreate的实现代码可以知道，usCount只能输入1，否则会报错。通过它的代码实现可以知道二值信号量是计数信号量的一个特例包装。

信号量释放函数 LOS_SemPost (),对应信号量获取函数LOS_SemPend()

信号量删除函数 LOS_SemDelete ()

为什么叫二值信号量呢？因为信号量资源被获取了，信号量值就是 0，信号量资源被释放，信号量值就
是 1，把这种只有 0 和 1 两种情况的信号量称之为二值信号量。

二值信号量获取，任何任务都可以从已创建的二值信号量资源中获取一个二值信号量，
若当前信号量有效，则获取成功并且返回正确，否则任务会根据用户指定的阻塞超时时间
来等待其它任务/中断释放信号量。在等待这段时间，系统将任务变成阻塞态，任务将被挂
到该信号量的阻塞等待列表中。

### 计数信号量

典型使用场景，例如模拟停车位的信号量。 假设有100个车位，对应信号量创建“LOS_SemCreate(100, &handle)”

计数型信号量创建函数 LOS_ SemCreate (), `LITE_OS_SEC_TEXT_INIT UINT32 LOS_SemCreate (UINT16 usCount/*初始化可用信号量个数*/, UINT32 *puwSemHandle/*信号量ID*/)`

信号量释放函数 LOS_SemPost ()，对应信号量获取函数LOS_SemPend()

信号量删除函数 LOS_SemDelete ()

## 互斥信号量（互斥锁）

如果需要使用互斥锁，需要配置`LOSCFG_BASE_IPC_MUX`为YES，同时配置LOSCFG_BASE_IPC_MUX_LIMIT为合适的数量

互斥锁又称互斥信号量，是一种特殊的二值信号量，它和信号量不同的是，它支持互
斥锁所有权、递归访问以及降低优先级翻转危害的特性，常用于实现对临界资源的独占式
处理

LOS_MuxCreate 创建互斥锁

LOS_MuxDelete 删除指定的互斥锁

LOS_MuxPend 申请指定的互斥锁

LOS_MuxPost  释放指定的互斥锁

## 事件

事件通信只能是事件类型的通信，无数据传输。事件不与任务相关联，事件相互独立。多次向任务发送同一事件类型，等效于只发送一次。

每个事件获取的时候，用户可以选择感兴趣的事件，并且选择读取
事件信息标记，它有三个属性，分别是逻辑与（LOS_WAITMODE_AND），逻辑或
（LOS_WAITMODE_OR）以及是否清除事件标记（LOS_WAITMODE_CLR）。当任务等
待事件同步时，可以通过任务感兴趣的事件位和事件信息标记来判断当前接收的事件是否
满足要求，如果满足则说明任务等待到对应的事件，系统将唤醒等待的任务；否则，任务
会根据用户指定的阻塞时间继续等待下去。

创建一个事件，就创建了事件集合（一个32 位的变量），用于标识该任务发生的事件类型，其中每一位表示一种事件类型（0 表示该事件类型未发生、1 表示该事件类型已经发生），一共31 种事件类型（第25 位保留）。

### 常用事件函数

事件初始化函数 LOS_EventInit()

事件销毁函数 LOS_EventDestory()

写指定事件函数 LOS_EventWrite()

读指定事件函数 LOS_EventRead()

清除指定事件函数LOS_EventClear ()，如果在获取事件的时候没有将对应的标志位清除，那就需要用到LOS_EventClear()这个函数进行显式清除事件标志

## 链表

liteOS提供了双向链表数据结构，使用方法参考“Middlewares\LiteOS\demos\kernel\api\los_api_list.c”



# stm32 时钟

nucleo  stm32F411的外部晶振由stlink mco输出的，如果自己额外加外部晶振，仔细看原理图注意焊掉两个0欧姆电阻，然后焊接另外两个0欧姆电阻

- HSE	外部高速时钟信号	一般选择外接晶振，最常用的时钟信号。
- HSI	内部高速时钟信号	由单片机内部的16MHzRC振荡器生成，成本低，但精度低。
- PLL	锁相环	由HSE或HSI提供时钟信号，主要用于生成高速系统时钟
- LSE	外部低速时钟信号	外接的32.768Khz晶振，用于驱动RTC时钟。RTC可以提供时钟、日历功能，功耗低精度高。
- LSI	内部低速时钟信号	32Khz左右，功耗低，可在停机、待机状态下使运行，供看门狗和自动唤醒单元使用

在stm32cubeMX配置 RCC  HSE时，STM32CubeMX中外部时钟配置可选类型为 Disable 、 BYPASS Clock Source（旁路时钟源） 、Crystal/Ceramic Resonator（石英/陶瓷 晶振） 三种类型

对这几种类型的区别是：

-  如果外部提供时钟接入OSC_IN引脚，OSC_OUT引脚悬空，对应选择为BYPASS Clock Source；
-  如果OSC_IN 与 OSC_OUT引脚都要连接，对应选择为Crystal/Ceramic Resonator。

根据[en.DM00105823.pdf UM1724]中的“6.7 OSC clock”章节，当采用stlink mco作为时钟源时，为8MHZ。


# 功耗

[NB-IoT 终端设备低功耗--软件设计](https://bbs.huaweicloud.com/forum/thread-13407-1-1.html)


# stm32裸机笔记



## LiteOS 的启动流程

```c
int main (void)
 {
 /* 硬件初始化 */
 BSP_Init(); //硬件初始化这一步还属于裸机的范畴

 /* RTOS 系统初始化 */
 LOS_KernelInit(); //RTOS 系统初始化。比如RTOS 里面的全局变量的初始化，空闲任
                   //务的创建等。不同的RTOS，它们的初始化有细微的差别。LiteOS 就是根据target_config.h
                   //中配置的模块进行一系列初始化。

 /* 创建任务1，但任务1 不会执行，因为调度器还没有开启 */ (3)
 Creat_Test1_Task();
 /* 创建任务2，但任务2 不会执行，因为调度器还没有开启 */
 Creat_Test2_Task();

 /* ......继续创建各种任务 */

 /* 启动RTOS，开始调度 */
 LOS_Start(); (4)
 }
```

在系统上电的时候第一个执行的是启动文件里面由汇编编写的复位函数
`Reset_Handler`，

如果在target_config.h 中使用了LOSCFG_PLATFORM_HWI 这个宏
定义，则进行硬件中断模块的初始化。则表示LiteOS 接管了系统的中断，使用时需要注册
中断，否则无法响应中断，而如果不使用LOSCFG_PLATFORM_HWI 这个宏定义，系中
断将由硬件响应，系统不接管中断的操作与裸机基本是差不多的。

### LiteOS核心初始化

在创建任务之前，需要将LiteOS 的核心组件进行初始化，LiteOS 已经为我们提供了一
个函数接口——LOS_KernelInit()。使用它就能将整个LiteOS 的核心部分初始化完成，然后
我们就能根据自己需要进行创建任务，信号量等，然后愉快地玩耍LiteOS 了。

`LOSCFG_PLATFORM_HWI` 是配置是否接管中断

