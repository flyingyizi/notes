https://biscuitos.github.io/blog/LDD_I2C_PCF8574AT/

PCF8574AT 简介
PCF8574 是一个 8bit 的 GPIO 拓展芯片，提供了标准的 I2C 接口。PCF8574 具有 低电流消耗以及具有高电流驱动能力的锁存输出，可用于驱动 LED 等。PCF8574 还拥有一条中断线 (INT) 用于连接到主控的中断逻辑上。PCF8574AT 具有 8 个可 用的 GPIO，每当 GPIO 的状态改变时，PCF8574 的 INT 引脚将会产生一个中断 信号，从而知道 GPIO 的状态，而不必通过轮寻的方式检查 GPIO 的状态。主控 可以通过 I2C 总线向 8 个 GPIO 写值，也可以读取 8 个 GPIO 的值。PCF8574 一般用于 GPIO 紧缺的情况，可以在 I2C 总线上挂载多个 PCF8574 来增加 GPIO 的数量。

[PCF8574AT Datasheet](https://github.com/BiscuitOS/Documentation/blob/master/Datasheet/I2C/PCF8574AT.pdf)
