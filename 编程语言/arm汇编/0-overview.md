
[ARM 开发手册以及 ARMv7 架构手册](https://github.com/BiscuitOS/Documentation/tree/master/Datasheet/ARM)

在ARMv7以及以前的架构中，只有32位。在2013年ARM推出64位架构ARMv8，寄存器是64位宽，相应的寄存器名称为x0,x1,x2,x3等。


## ARMv7 与 ARM7 概念区别：

ARMv7是架构名，ARM7是基于ARMv3架构生产的一个芯片的产品名称。到ARM11芯片之后，ARM公司更改了芯片的命名规则，由老的ARMx改为 Cortex系列，简言之：

Cortex-M系列：M-Profile，即"Microcontroller" -Profile，侧重微控制器单片机方面的场合。

Cortex-R系列：R-Profile，即"Real-Time"-Profile，侧重于实时系统的场合。

Cortex-A系列： A-Profile，即“Application”-Profile，侧重于应用功能的场合。

比如华为生产的Hi3798mv200芯片，大的架构基于ARMv8（又叫ARM64）的，而ARMv8中又有Cortex A53分支，准确表达应该是:华为Hi3798mv200是基于ARMv8的Cortex-A53系列的一款芯片。

|架构|处理器家族|
|------|-------|
|ARMv1 |  ARM1    |      
|ARMv2 |  ARM2,ARM3    |      
|ARMv3 |  ARM6,ARM7    |      
|ARMv4 |  strongARM,ARM7TDMI,ARM9TDMI    |      
|ARMv5 |  ARM7EJ,ARM9E,ARM10E,XScale     |      
|ARMv6 |  ARM11,ARM Cortex-M             |      
|ARMv7 |  ARM Cortex-A,ARM Cortex-M,ARM Cortex-R    |      
|ARMv8 |  ARM Cortex-A35,ARM Cortex-A50,ARM Cortex-A72,ARM Cortex-A73,    |      

## T1/T2/T3/A1/A2 encodings 编码概念

在查看指令时，常常见到同一条指令（e.g. MOV）给出了多种encoding，这里澄清下它概念。

那些 T1、T2、T3... 编码是指同一条指令有许多不同的编码，通常取决于可以使用的寄存器，编译器或汇编器将自动选择正确的编码来使用。也就是说编译器会自动根据所使用的寄存器/助记符...等多种因素对一个指令选择它认为合适的encoding。

这个问题的解答参见 [Which instruction format does Cortex-R support,encoding A1 orA2?](https://community.arm.com/support-forums/f/architectures-and-processors-forum/3550/which-instruction-format-does-cortex-r-support-encoding-a1-ora2)


## 汇编语言基本结构

例：

AREAInit,CODE,READONLY;AREA定义代码段，段名Init;代码段，只读

ENTRY       ;伪操作，第一条指令的入口

Start       ;标号，一段代码的开始，用于标记，无意义，必须顶格

MOV r0,#10  ;将10存入r0寄存器，整型常量前面用#号

MOV r1,#3   ;将3存入r1寄存器，r0和r1相当于两个变量，只是名称固定，在寄存器的存储空间内

ADD r0,r0,r1;将r0内数据与r1内数据相加，相加后数据放在r0内

;Stop;停止标号，下面的代码用于停止运行程序

;MOV r0,#0x18;软件异常中断响应

;LDR r1,=0x20026;ADP停止运行，应用退出

;SWI 0x123456;ARM半主机软件中断

END         ;程序结束
