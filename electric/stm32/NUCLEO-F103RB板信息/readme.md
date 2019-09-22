[6.Nucleo板：NUCLEO-F103RB（STM32F103RBT6 ） ](https://www.stmcu.org.cn/document/list/index/category-720)

[STM32F103RB开发板原理图](https://www.stmcu.org.cn/document/download/index/id-206350)

- 从UM1724的sch图看，LD2是由PORTA5控制

在用cube配置时钟时，有下面两个选项：BYPASS Clock Source（旁路时钟源）；Crystal/Ceramic Resonator（晶体/陶瓷晶振）
下面来解释一下：
 所谓HSE旁路时钟源，是指无需使用外部晶体时所需的芯片内部时钟驱动组件，直接从外界导入时钟信号。犹如芯片内部的驱动组件被旁路了。
 外部晶体/陶瓷谐振器(HSE晶体)模式该时钟源是由外部无源晶体与MCU内部时钟驱动电路共同配合形成，有一定的启动时间，精度较高。
