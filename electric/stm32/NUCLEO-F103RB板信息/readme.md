[6.Nucleo板：NUCLEO-F103RB（STM32F103RBT6 ） ](https://www.stmcu.org.cn/document/list/index/category-720)

[STM32F103RB开发板原理图](https://www.stmcu.org.cn/document/download/index/id-206350)

- 从UM1724的sch图看，LD2是由PORTA5控制

在用cube配置时钟时，有下面两个选项：BYPASS Clock Source（旁路时钟源）；Crystal/Ceramic Resonator（晶体/陶瓷晶振）
下面来解释一下：
 所谓HSE旁路时钟源，是指无需使用外部晶体时所需的芯片内部时钟驱动组件，直接从外界导入时钟信号。犹如芯片内部的驱动组件被旁路了。
 外部晶体/陶瓷谐振器(HSE晶体)模式该时钟源是由外部无源晶体与MCU内部时钟驱动电路共同配合形成，有一定的启动时间，精度较高。

## Using the ST-LINK/V2-1 to program/debug the STM32 on board

CN2的两个跳帽都必须带上

## Using ST-LINK/V2-1 to program/debug an external STM32 application
It is very easy to use the ST-LINK/V2-1 to program the STM32 on an external application. Simply remove the two jumpers from CN2 as illustrated in Figure 7, and connect the application to the CN4 debug connector according to Table 4.

Note:SB12 NRST (target MCU RESET) must be OFF if CN4 pin 5 is used in the external application.

Table 4. Debug connector CN4 (SWD)
|Pin |CN4        |Designation |
|----|-----------|------------|
|1   |VDD_TARGET |VDD from application|
|2   |SWCLK      |SWD clock|
|3   |GND        |Ground|
|4   |SWDIO      |SWD data input/output|
|5   |NRST       |RESET of target MCU|
|6   |SWO        |Reserved|