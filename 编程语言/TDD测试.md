
TDD是一种增量式软件开发技术，简单来说，就是在没有失败的单元测试前提下不可以写产品代码，这些测试要小，而且自动化。

重构：就是在不改变当前外部行为的条件下对现有代码结构进行修改的过程。其目的式通过写易于理解/演化并且易于我们维护的代码使得工作简单。

Unity介绍
    Unity 是一个用纯C语言编写的测试工具. 它简洁实用,多应用于嵌入式系统.Unity工具可以裁剪用于各种规模的嵌入式项目,当然,只要是纯C语言的项目,Unity都可以使用.

    主页：http://throwtheswitch.org/
    源码下载：https://github.com/ThrowTheSwitch/Unity/downloads 
    github主页：https://github.com/ThrowTheSwitch/Unity


# 嵌入式单元测试

在嵌入式开发中对一些功能模块进行单元测试，可以使用类似cppUtest，googleTest单元测试框架。  通常在PC软件开发时，以下两个Eclipse插件是需要安装的

    |插件	| 描述 |
    |-------|-----|
    |C/C++ Unit Testing Support   |	在Eclipse中执行单元测试的基本插件。|
    |CppUTest Eclipse Test Runner |	它是一个用于在上述插件上执行CppUTest的插件。|

由于平时常用的IDE是stm32cubeIDE，发现这个IDE有特殊的地方，下面如果涉及到IDE，我们都是以该IDE为例进行说明

## 目录建议

测试目录建议

```text
└─tests
    ├─fakes
    ├─header_overrides
    ├─makefiles
    ├─mocks
    ├─src
    └─stubs
```

- 其中makefiles目录存放每个测试待测试source文件的makefile，
- 其中“header_overrides”目录的组织按照原始include路径组织，例如对“`#include <stm32/xxxx.h>`”，那应该建立目录“`tests/header_overrides/stm32/xxxx.h`”

## cppUTest

### 构建cppUTest

前提条件：
- 已经安装mingw，具备pc gcc toolchain
- 下载[CppUTest](https://github.com/cpputest/cpputest/releases/download/v3.8/cpputest-3.8.zip)

方式1：

- `cmake.exe  -G "MinGW Makefiles" ..`
- `mingw32-make`
- `mingw32-make install`

or 方式2：

如果采用stm32cubeIDE构建， 以下是操作所需步骤,假设下载cppUTest源码目录是`${CPPUTEST_SOURCE_DIR}`：

- 新建基于pc gcc的c/c++项目
- 在c/c++ building--settings 中引入include path"`${CPPUTEST_SOURCE_DIR}/include`"
- 通过import 引入"`${CPPUTEST_SOURCE_DIR}/src/CppUTest`",与"`${CPPUTEST_SOURCE_DIR}/src/Platforms/Gcc`"

    注：如果是编译libCppUTestExt.a,则import的内容需要更改为引入"`${CPPUTEST_SOURCE_DIR}/src/CppUTestExt`"

后续使用`-I${CPPUTEST_HOME}/include`,`-L${CPPUTEST_HOME}/lib`, `-lCppUTest -lCppUTestExt` 

### 使用CppUTest

使用CppUTest建议方式通过自带发布的的`MakefileWorker.mk`

以下是一个使用例子

```Makefile
CPPUTEST_WARNINGFLAGS = -Wno-float-conversion -Wno-sign-conversion -Wno-int-to-pointer-cast -Wno-unused-parameter -Wall -Wextra -Wshadow -Wswitch-default -Wswitch-enum -Wconversion -Wno-long-long
CPPUTEST_CPPFLAGS += -DUSE_HAL_DRIVER -DSTM32F401xE

#Set this to @ to keep the makefile quiet
ifndef SILENCE
	SILENCE = @
endif

COMPONENT_NAME = CppUTestALL
CPPUTEST_HOME = D:/stm32/cpputest-latest

# whether use CppUTestExt
CPPUTEST_USE_EXTENSIONS = Y

SRC_DIRS = \

SRC_FILES = \


TEST_SRC_DIRS = \

INCLUDE_DIRS =\
  $(CPPUTEST_HOME)/include\

include $(CPPUTEST_HOME)/build/MakefileWorker.mk
```

### stm32cubeIDE使用cppUTest

不支持

通过实践看（最新stm32cubeIDE版本1.3.0），stm32cubeIDE无法安装"`CppUTest Eclipse Test Runner`"。


## googleTest

### 源码构建googleTest

前提条件：
- 已经安装mingw，具备pc gcc toolchain
- 下载[ninja](https://github.com/ninja-build/ninja/releases/download/v1.10.1/ninja-win.zip)并加入%PATH%
- 下载[googletest](https://github.com/google/googletest)

在windows，很多介绍都是通过"`cmake.exe  -G "MinGW Makefiles" ..`"生成Makefile从而进行编译的，但我自己的实践一直各种问题，所以下面是通过`ninja`进行编译

安装步骤：

build googletest
```shell
$git clone https://github.com/google/googletest.git && cd googletest
# $git checkout release-1.8.1
$mkdir build && cd build && cmake -G Ninja -DCMAKE_INSTALL_PREFIX=Work/gtest -Dgtest_disable_pthreads=OFF ..   #  -DCMAKE_BUILD_TYPE=Debug
$ninja -j6 install
```

后续使用`-I<Work-gtest-dir>/include`,`-L<Work-gtest-dir>/lib`, `-lgtest -lpthread` 

### linux安装googleTest

```shell
$sudo apt-get install libgtest-dev
#查看软件安装位置
$dpkg -L libgtest-dev
$cd /usr/src/gtest
$sudo mkdir build
$cd build
$sudo cmake ..
$sudo make
$sudo make install
$cd ..
$sudo rm -rf build
```

### 使用googleTest

为了使用方便，写了通用mk，如下：

Makefile文件，根据自身项目进行内容定制：

```makefile
#---------
#
# googleTest Examples Makefile
#
#----------

GOOTESTEST_WARNINGFLAGS = -Wno-float-conversion -Wno-sign-conversion -Wno-int-to-pointer-cast -Wno-unused-parameter -Wall -Wextra -Wshadow -Wswitch-default -Wswitch-enum -Wconversion -Wno-long-long
GOOTESTEST_FLAGS += -DUSE_HAL_DRIVER -DSTM32F401xE

#Set this to @ to keep the makefile quiet
ifndef SILENCE
	SILENCE = @
endif

#--- Inputs ----#
#   COMPONENT_NAME - the name of the thing being built
COMPONENT_NAME = CppUTestALL

LIBDIR=-LC:/tools/msys64/mingw64/lib/googletest

# whether use CppUTestExt
CPPUTEST_USE_EXTENSIONS = Y

# src maybe .cpp or .c
TEST_SRC_DIRS = \
	../fakes/grbl  \
	../fakes/grbl/custom_hal  \
  ../fakes/Drivers/STM32F4xx_HAL_Driver/Src \
	../src


TEST_CSRC_FILES = \
    ../../grbl/util.c \
    ../../grbl/Settings.c \
    ../../grbl/settings_flags.c \
    ../../grbl/Nvm.c \
    ../../grbl/custom_hal/eeprom.c \
     ../../grbl/System.c 

TEST_INCLUDE_DIRS =\
  ../header_overrides\
  ../../Inc \
  ../../grbl \
  ../../grbl/custom_hal \
  ../../Drivers/CMSIS/Device/ST/STM32F4xx/Include \
  ../../Drivers/CMSIS/Include \
  ../../Drivers/STM32F4xx_HAL_Driver/Inc \
  ../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy \

include ./Makefilegtestbase.mk
```

Makefilegtestbase.mk 文件：

```Makefile
# 必须定义VAR
# COMPONENT_NAME:
# TEST_SRC_DIRS， 或TEST_CSRC_FILES/TEST_CXXSRC_FILES：相关的source文件

# 可重定义VAR
# GOOTESTEST_WARNINGFLAGS：补充CFLAGS/CPPFLAGS定义
# GOOTESTEST_FLAGS：补充CFLAGS/CPPFLAGS定义
# GOOGLETEST_HOME： 如果没有定义，则从系统默认include，lib目录查找googletest include,lib，
#                   否则使用${GOOGLETEST_HOME}/lib, ${GOOGLETEST_HOME}/include
# LIBS：
# LIBDIR：
# SILENCE: 
# GCC_PATH: 如果gcc不在系统PATH，则可以通过定义它查找gcc

#######################################
# binaries
#######################################
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/gcc
CXX = $(GCC_PATH)/g++
AS = $(GCC_PATH)/gcc -x assembler-with-cpp
CP = $(GCC_PATH)/objcopy
SZ = $(GCC_PATH)/size
else
CC = gcc
CXX = g++
AS = gcc -x assembler-with-cpp
CP = objcopy
SZ = size
endif

RM = rd /s /q  # -rm  -fR  $(TESTBUILD_DIR)

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og


#######################################
# paths & target
#######################################
# Build path
TESTBUILD_DIR = $(COMPONENT_NAME)_build
TESTTARGET = $(COMPONENT_NAME)_tests

######################################
# source
######################################
# C sources
C_SOURCES =  

get_csrc_from_dir  = $(wildcard $1/*.c)
get_csrc_from_dir_list = $(foreach dir, $1, $(call get_csrc_from_dir,$(dir)))
TEST_CSRC = $(call get_csrc_from_dir_list, $(TEST_SRC_DIRS)) $(TEST_CSRC_FILES)
C_SOURCES +=  ${TEST_CSRC}

# C sources
CXX_SOURCES =  

get_cxxsrc_from_dir  = $(wildcard $1/*.cpp) $(wildcard $1/*.cc) 
get_cxxsrc_from_dir_list = $(foreach dir, $1, $(call get_cxxsrc_from_dir,$(dir)))
TEST_CXXSRC = $(call get_cxxsrc_from_dir_list, $(TEST_SRC_DIRS)) $(TEST_CXXSRC_FILES)
CXX_SOURCES +=  ${TEST_CXXSRC}


 
#######################################
# CFLAGS
#######################################

# C defines
C_DEFS = 

# includes
get_dirs_from_dirspec  = $(wildcard $1)
INCLUDES_DIRS_EXPANDED = $(call get_dirs_from_dirspec, $(TEST_INCLUDE_DIRS))
INCLUDES += $(foreach dir, $(INCLUDES_DIRS_EXPANDED), -I$(dir))
ifdef GOOGLETEST_HOME
INCLUDES += -I${GOOGLETEST_HOME}/include 
endif


# compile gcc flags
CFLAGS =  $(C_DEFS) $(INCLUDES) $(OPT) ${GOOTESTEST_FLAGS} ${GOOTESTEST_WARNINGFLAGS} -Wall -g 
CPPFLAGS =  $(C_DEFS) $(INCLUDES) $(OPT) ${GOOTESTEST_FLAGS} ${GOOTESTEST_WARNINGFLAGS}  -Wall -g 

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
CPPFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# libraries
LIBS += -lgtest -lm -lpthread 
ifdef GOOGLETEST_HOME
LIBDIR += -L${GOOGLETEST_HOME}/lib
endif

LDFLAGS = $(LIBDIR) $(LIBS) -Wl,-Map=$(TESTBUILD_DIR)/$(TESTTARGET).map 

# default action: build all
all: $(TESTBUILD_DIR)/$(TESTTARGET) 

#######################################
# build the application
#######################################
# list of objects
C_OBJECTS = $(addprefix $(TESTBUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

CXX_OBJECTS = $(addprefix $(TESTBUILD_DIR)/,$(notdir $(CXX_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CXX_SOURCES)))


$(TESTBUILD_DIR)/%.o: %.c Makefile | $(TESTBUILD_DIR) 
	$(SILENCE)$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(TESTBUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@
	$(SILENCE)echo $(notdir $(<:.c=.o))

$(TESTBUILD_DIR)/%.o: %.cpp Makefile | $(TESTBUILD_DIR) 
	$(SILENCE)$(CXX) -c $(CPPFLAGS) -Wa,-a,-ad,-alms=$(TESTBUILD_DIR)/$(notdir $(<:.cpp=.lst)) $< -o $@
	@echo $(notdir $(<:.cpp=.o))

$(TESTBUILD_DIR)/$(TESTTARGET): $(C_OBJECTS) $(CXX_OBJECTS) Makefile
	$(SILENCE)$(CXX) $(C_OBJECTS) $(CXX_OBJECTS)  $(LDFLAGS) -o $@
	@echo linking...

$(TESTBUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-${RM}  $(TESTBUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(TESTBUILD_DIR)/*.d)

# *** EOF ***
```

## IDE使用googleTest

### stm32cubeIDE使用cppUTest

如果选择IDE采用“stm32cubeIDE”，则可以参考该节内容。


通过“help/ install new software”，在“--All Available Sites--”下搜索“Unit Testing”，安装“C/C++ Unit Testing Support”，安装完成后，将会在`run configuration， debug configuration`看到，它已经安装了test runner，其中就包括googletest runner，但遗憾的是不包含cpputest runner


### 普通Eclipse说明

如果选择IDE采用“TrueSTUDIO”，或普通“Eclipse”，则可以参考该节内容。

从实践看，stm32cubeIDE并不能当作一个普通Eclipse对待，很多适合eclipse的操作，stm32cubeIDE找不到。 下面记录的是普通Eclipse按照测试相关插件的说明：

    在下面的文章中，开发环境不是“TrueSTUDIO”而是“Eclipse”，但“TrueSTUDIO”基于“Eclipse”，它的用法几乎相同。
    请注意，工具链中使用“Atollic PC Tool”代替“MinGW GCC”。


    - 1。在Eclipse中安装插件
    由于有必要在Eclipse中安装两个插件，我们将按顺序解释。

    |插件	| 描述 |
    |-------|-----|
    |C/C++ Unit Testing Support   |	在Eclipse中执行单元测试的基本插件。|
    |CppUTest Eclipse Test Runner |	它是一个用于在上述插件上执行CppUTest的插件。|

    对“C/C++ Unit Testing Support”，直接通过『Help』→『Install new Software…』进行安装。比如我安装的trueStudio是基于eclipse neon，那就在`Eclipse Repository - http://download.eclipse.org/releases/neon`仓库里面的program language里面找到“C/C++ Unit Testing Support”进行安装

    对“CppUTest Eclipse Test Runner”，通过下载“https://github.com/tcmak/CppUTestEclipseJunoTestRunner”进行手工安装。 下面介绍下eclipse安装它。注意对trueStudio，ECLIPSE根目录是“TrueSTUDIO for STM32 9.3.0\ide”。下文中标注"<ECLIPSE>"这种根目录写法的都要注意替换下。下面的安装介绍来自该git网页

    HOW TO INSTALL

    Create directory "<YOUR_PATH_TO_ECLIPSE>/eclipse/dropins/CppUTest".

    In this repository, change into the directory "org.eclipse.cdt.testrunner.cpputest".

    Copy the following artifacts from there to "<YOUR_PATH_TO_ECLIPSE>/eclipse/dropins/CppUTest":

    bin/org/ --> org/
    META-INF/
    plugin.properties
    plugin.xml

    You must copy "bin/org/" to "org/" and not to "bin/org/" !

    Close Eclipse.
    Start Eclipse from a command shell with the following command:

    Eclipse -clean -console -consoleLog

    "CppUTest Tests Runner" should now be selectable as a test runner within Eclipse and successfully run a CppUTest test project.


    复制后，重新启动Eclipse以激活插件。关闭Eclipse，启动命令提示符，移至Eclipse安装目录，然后执行以下命令。`eclipse.exe -clean`. Eclipse启动并激活CppUtest Eclipse Test Runner。

    注： eclipse.exe就是"C:\prog\TrueSTUDIO for STM32 9.3.0"\ide\TrueSTUDIO.exe。因为TrueSTUDIO是基于eclipse的工具。

### vscode debug 配置

如果选择IDE采用“vscode”，则可以参考该节内容。

首先需要配置debug,配置文件是“launch.json”,[官方配置说明](https://code.visualstudio.com/docs/editor/debugging#_launch-configurations)

全新环境情况下的debug配置：

在最左侧toolbar上点击“run”，出现“run”配置界面，点击“to customize run and debug create a launch.json file”, 出现选择界面选择“c++ (gdb/lldb)”,从而新创建一个“launch.json”

如果已经有“launch.json”，也可以在它基础上新增，具体操作是： 在“run”界面，选择下拉“add configuration...”，下面是一个“launch.json”例子：

```json
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [

        {
            "type": "iotlink-debug",
            "request": "launch",
            "name": "Launch Program",
            "cwd": "${workspaceFolder}",
            "initBreak": "main"
        },
        {
            "name": "(gdb) 启动",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/tests/makefiles4gtest/UTestALL_build/UTestALL_tests.exe",
            "args": [],
            ...            
        }
    ]
}
```

这里需要特别提示： 对c/c++ UTEST， TEST程序往往是单独的，所以配置“launch.json”种的"program"是和正常程序调试是不同的，在上面例子中就是一个例子

下面我们的例子是使用`googleTest Adapter`，

首先从vscode  market 中搜索安装“googleTest Adapter”， 然后在".vscode/settings.json"中手工增加类似下面"`gtest-adapter.*`"的配置

```json
{
    "editor.tokenColorCustomizations": {
       ...
    },
    "gtest-adapter.debugConfig": [
        "(gdb) 启动"
    ],
    "gtest-adapter.supportLocation": true,
    "gtest-adapter.showRunOutput": true,
    "gtest-adapter.clearRunOutput": true,    
}
```

这个手工增加的配置也可以省略，而是通过“vscode 左侧工具栏上的TEST”按钮出现TEST界面后，通过点击“switch test configuration”选择后自动增加

编译UTEST程序（e.g. `"${workspaceFolder}/tests/makefiles4gtest/UTestALL_build/UTestALL_tests.exe"`）,最终在TEST界面，刷新后将会看到UTEST的test case列表。


# debug

## gdb 备忘

- next命令用于单步执行，类似VC++中的step over。next的单步不会进入函数的内部，
- 与next对应的step（缩写s）命令则在单步执行一个函数时，会进入其内部，类似VC++中的step into

## openocd

https://stackoverflow.com/questions/42612329/executing-code-from-ram-in-stm32

[OpenOCD User’s Guide](http://openocd.org/doc-release/html/index.html#SEC_Contents)

```shell
$openocd -f stlink-v2-1.cfg -f stm32f4x.cfg
$C:\Users\xxxxx\.vscode\extensions\iotlink.iot-studio-1.1.0\bin\openocd>bin\openocd.exe -f interface/stlink-v2-1.cfg -f target/stm32f4x.cfg
xPack OpenOCD, 32-bit Open On-Chip Debugger 0.10.0+dev (2019-07-17-07:34)
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html
WARNING: interface/stlink-v2-1.cfg is deprecated, please switch to interface/stlink.cfg
Info : auto-selecting first available session transport "hla_swd". To override use 'transport select <transport>'.
Info : The selected transport took over low-level target control. The results might differ compared to plain JTAG/SWD
Info : Listening on port 6666 for tcl connections
Info : Listening on port 4444 for telnet connections
Info : clock speed 2000 kHz
Info : STLINK V2J37M26 (API v2) VID:PID 0483:374B
Info : Target voltage: 3.258383
Info : stm32f4x.cpu: hardware has 6 breakpoints, 4 watchpoints
Info : Listening on port 3333 for gdb connections



# 在另一个窗口

telnet localhost 4444

Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Open On-Chip Debugger
> 
> resume 0 #从0地址开始运行
> halt #暂停cpu
> mdw 0 12 #从0地址读12个数（32位）
>mww 地址 写的数 #向某地址写数（32位）
>reg #查看寄存器
>flash write_image erase /home/liuyang/Projects/Nuttx/nuttx/nuttx.hex #下载
```




