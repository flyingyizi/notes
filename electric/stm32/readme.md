
- STM32CubeMX user manual (UM1718).  [STM32 基础系列教程 1- CubeMX+GPIO](https://blog.csdn.net/zhanglifu3601881/article/details/88839590)
- [trueStudio开发环境配置笔记](https://blog.csdn.net/cshy2013/article/details/88948643)

- [STM32 基础系列教程](https://blog.csdn.net/zhanglifu3601881/article/details/88839590)

STM32系列单片机在startup_stm32fxx.s文件中，定义了Heap_Size 和 Stack_Size.常见的大小分别是0x200（512 bytes）,0x400（1024 bytes),大小可以修改，但是要考虑程序的规模和单片机IRAM的大小。

在stm32cube project manager中出现在linker setting中的minimum heap size （default:0x200）,minimum stack size （default:0x400）.堆和栈的区别：stack的空间由操作系统自动分配/释放，heap上的空间手动分配/释放。stack的空间有限，heap是很大的自由存储区(heap虽然有很大的存储区，但是这个存储区并不是无限大的，在stm32中，heap区的最大值由SRAM区决定，而SRAM区的大小可以参考具体的数据手册)。[参考](https://blog.csdn.net/qq_27312943/article/details/52943844)

[linux上搭建stm32开发环境 vscode+gcc+stm32cubeMX 和 vscode+platformIO](https://blog.csdn.net/baidu_36482169/article/details/80457184)

补充：VSCode的PlatformIO IDE插件里Platform选项安装“ST STM32”家目录下会自动安装相应工具，看下面：
```text
    ~/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc -v
```
如果你用PlatformI,不用另外装arm-none-eabi工具集了，

[windows 下vscode+platformio开发stm32f103c8t6最小系统](https://blog.csdn.net/lear1234/article/details/78477054)

- keil5 手动添加pack( STM32F10x为例 )
    github:https://github.com/oraSC
    从http://www.keil.com/dd2/Pack/#/eula-container上下载找到自己要下载的standalone的pack文件后 (版本限制，低版本的 MDK 不支持高版本的pack，选择对应的版本 pack下载) 点击下载pack运行即可完成pack手动导入，选择file/Device Database 即可看pack是否导入成功。

https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/8-2018q4/gcc-arm-none-eabi-8-2018-q4-major-win32.zip

[stm32d](https://www.strongerhuang.com/)

[微雪stm32课堂](http://www.waveshare.net/study/article-660-1.html)

STM32，大家常见的就是使用寄存器、或者标准外设库进行开发。但是，从15、16年开始，ST逐渐停止了对标准外设库的更新和维护，转向了HAL和LL库。停止标准库更新，使用STM32CubeMX配置HAL/LL库是主流；

- ·SPL：Standard Peripheral Library标准外设库
- ·HAL：Hardware Abstraction Layer硬件抽象层库
- ·LL：Low-layer底层库


# SPL库开发
以STM32F103为例。

## stm32f10x_stdperiph 开发

首先需要下载标准外设库[stm32f10x_stdperiph_lib](https://www.stmcu.org.cn/document/download/index/id-200305). 下载[RM0008 - STM32F10x参考手册](https://www.stmcu.org.cn/document/detail/index/id-200272)

1. 收集STM32F10x_StdPeriph_Lib_V3.5.0\Libraries下库文件

stm32f10x_stdperiph_lib， 上[https://www.stmcu.org.cn](https://www.stmcu.org.cn)查找下载。

```shell
export STDPERIPHV35=/c/home/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries
# CMSIS内核部分
mkdir -p ./stlib/CMSIS && cp ${STDPERIPHV35}/CMSIS/CM3/CoreSupport/core_cm3.h  "$_"
mkdir -p ./stlib/CMSIS && cp ${STDPERIPHV35}/CMSIS/CM3/CoreSupport/core_cm3.c  "$_"

mkdir -p ./stlib/CMSIS && cp ${STDPERIPHV35}/CMSIS/CM3/DeviceSupport/ST/STM32F10x/*.c  "$_"
mkdir -p ./stlib/CMSIS && cp ${STDPERIPHV35}/CMSIS/CM3/DeviceSupport/ST/STM32F10x/*.h  "$_"

## cl:互联型产品，stm32f105/107系列
## vl:stm32f100系列
## xl:超高密度(容量)产品，stm32f101/103系列
## ld:低密度(容量)产品，flash小于64KB
## md:中等密度(容量)产品，flash等于64KB或128KB
## hd:高密度(容量)产品，flash大于128KB
## 如stm32F103VET6是64KB RAM和512K ROM，属于高密度产品，所以期待文件选择startup_stm32f10x_hd.s
mkdir -p ./stlib/CMSIS/startup && cp ${STDPERIPHV35}/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/*  "$_"

# 外设函数部分
mkdir -p ./stlib/FW && cp -r ${STDPERIPHV35}/STM32F10x_StdPeriph_Driver/inc  "$_"
mkdir -p ./stlib/FW && cp -r ${STDPERIPHV35}/STM32F10x_StdPeriph_Driver/src  "$_"

# 标准模板部分
mkdir -p ./user && cp -r ${STDPERIPHV35}/../Project/STM32F10x_StdPeriph_Template/stm32f10x_*  "$_"
```

## stm32f4xx_stdperiph 开发

[【参考】](https://www.cnblogs.com/zzmv/p/5893267.html)

首先需要搜索STSW-STM32065下载标准外设库[stm32f4xx_stdperiph_lib](https://www.stmcu.org.cn/document/detail/index/id-213641). 

1. 收集STM32F4xx_StdPeriph_Lib_V3.5.0\Libraries下库文件

```shell
export STDPERIPHV18=/c/home/STM32F4xx_DSP_StdPeriph_Lib_V1.8.0/Libraries
# CMSIS内核部分
mkdir -p ./stlib/CMSIS && cp ${STDPERIPHV18}/CMSIS/Device/ST/STM32F4xx/Include/*.h  "$_"
mkdir -p ./stlib/CMSIS && cp ${STDPERIPHV18}/../Project/STM32F4xx_StdPeriph_Templates/system_stm32f4xx.c  "$_"

mkdir -p ./stlib/CMSIS && cp ${STDPERIPHV18}/CMSIS/Include/*  "$_"

mkdir -p ./stlib/CMSIS && cp ${STDPERIPHV18}/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c  "$_"
mkdir -p ./stlib/CMSIS && cp ${STDPERIPHV18}/CMSIS/Device/ST/STM32F4xx/Include/system_stm32f4xx.h  "$_"

## e.g. startup_stm32f40xx.s
mkdir -p ./stlib/CMSIS/startup && cp ${STDPERIPHV18}/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/*  "$_"

# 外设函数部分
mkdir -p ./stlib/FW && cp -r ${STDPERIPHV18}/STM32F4xx_StdPeriph_Driver/inc  "$_"
mkdir -p ./stlib/FW && cp -r ${STDPERIPHV18}/STM32F4xx_StdPeriph_Driver/src  "$_"

# 标准模板部分
mkdir -p ./user && cp -r ${STDPERIPHV18}/../Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_*  "$_"
```

# stm32cubeide 备忘

## serial terminal工具

https://community.st.com/s/question/0D50X0000BNv7IQ/does-stm32cubeide-offer-a-serial-terminal-this-functionality-was-available-in-the-atollic-truestudio-product

```text
Go to menu Help -> Install New Software
Select Work with -> All available sites
In the search box type "terminal"
When search is finished select "TM Terminal" and "TM Terminal Serial Extensions" and install them.

Restart CubeIDE
From menu Window -> Show View -> Other select "Terminal"

Terminal window should appear in IDE.
Click Open Terminal icon (or Shift+Alt+Ctrl+T) and set up your connection parameters.
This is the result in my case:
....
```