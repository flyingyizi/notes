
参考书籍：
- [Make: AVR Programming book source code](https://github.com/hexagon5un/AVR-Programming)

- AVR单片机GCC程序设计.pdf

ATMega16 也是个用量较多的器件，它的引脚兼容AT90S8535，可以取代产品中的AT90S8535芯片.AT90S8515的引脚则兼容51单片机，在设计中很容易的替代51单片机

# 1.存储器操作

寄存器堆（R0～R31）、I/O 寄存器和SRAM 被统一编址。所以对寄存器和I/O 口的操作使用与访问内部SRAM 同样的指令。注意AVR 单片机的内部有64～4K 字节的EEPROM 数据存储器，它们被独立编址，按字节组织。

32 个通用寄存器被编址到最前,I/O 寄存器占用接下来的64 个地址。从0X0060 开始为内部SRAM。外部SRAM 被编址到内部SRAM 后。

AVRI/O 寄存器又称为I/O 专用寄存器（SFR)。avr-libc 对SFR 在"avr/io.h"中定义了符号，例如`PORTB,PINB`

```c
#include <avr/io.h>
DDRB=_BV(PB0)|_BV(PB1);
```

avr-libc 对FLASH 存储器的读写支持API 和宏在头文件"avr/pgmspace.h" 中定义.

```c
#include <avr/pgmspace.h>
ram_val=pgm_read_byte(&flash_val); 
```

avr-libc 对EEPROM 存储器读写API 定义在"avr/eeprom.h" 中，

### AVR 通用寄存器

在AVR中，由命名为R0～R31的32个8位通用工作寄存器构成一个“通用快速工作寄存器组”。在AVR中，通用寄存器组与片内的数据存储器SRAM处在相同的空间，32个通用寄存器被直接映射到用数据空间的前32个地址



### AVR SFR 特殊功能寄存器(special function register)

特殊功能寄存器SFR，是SRAM地址已经确定的SRAM单元，每个IO口都由三个寄存器位控制。DDxn位是控制IO口方向（值为1代表Pxn配置为输出引脚；0代表配置为输入引脚），PINxn是用来读取IO口的寄存器, PORTxn是用来写IO口。

    PINB  - the port B input Pins address
    DDRB  - the port B data direction register
    PORTB - the port B data register

举例：

```c
    DDRB |=  (1 << DDB4) ; // 设置PB0为输出口
    PORTB |= (1 <<PORTB0); // 设置PB0输出1
```

在C语言环境下对SFR访问归纳起来有[两种方法](http://m.elecfans.com/article/577508.html),AVR-libc采用的是标准C的方法。

- 采用标准C的强制类型转换和指针来实现
    例如:`#define DDRB (*(volatile unsigned char *)0x25)`

- 对C编译器进行语法扩充

    对C编译器进行语法扩充。例如MCS51系列KeilC中扩充sfr关键字，举例如`sfr P0 = 0x80;`,这样操作0x80单元直接写P0即可。

另外，GCCAVR中宏_BV(bit)是操作I/O寄存器是频繁用到的，avr-libc建议使用这一宏进行寄存器的位操作，他在文件sfr_defs.h中定义,以下是他的使用示例;

```c
DDRB = _BV(PB0) | _BV(PB1); //器件头文件中已经定义PB0代表0，PB1代表1
```

他等同于“`DDRB=0x03;`”,这样写的目的是为了提供程序的可读性。不要担心它会生成比“`DDRB=0x03;`”更大的代码，编译器会处理这种事情，最终会输出与“`DDRB=0x03;`”同样的结果。

- 表6.1 I/O口引脚配置表

| DDRXn | PORTXn | PUD | I/O方式 | 内部上拉电阻 | 引脚状态说明 |
|-------|--------|-----|---------|-------------|-------------|
| 0     | 0      | X   | 输入    | 无效        | 三态（高阻） |
| 0     | 1      | 0   | 输入    | 有效        | 外部引脚拉低时输出电流(uA) |
| 0     | 1      | 1   | 输入    | 无效        | 三态（高阻）|
| 1     | 0      | X   | 输出    | 无效        | 推挽0输出，吸收电流(20mA) |
| 1     | 1      | X   | 输出    | 无效        | 推挽1输出，输出电流(20mA) |

表中的PUD为寄存器SFIOR中的一位，它的作用相当AVR全部I/O口内部上拉电阻的总开关。当PUD=1时，AVR所有I/O内部上拉电阻都不起作用（全局内部上拉无效）；而PUD=0时，各个I/O口内部上拉电阻取决于DDRXn的设置。

# 2.中断 interupt

早期版本的avr-libc 对中断服务程序的书写提供了两个宏SIGNAL 和INTERRUPT，并且需要包含两个头文件：avr/signal.h 和avr/interrupt.h。新版（如2007 版WINAVR）中，INTERRUPT 宏不再可用，而建议用ISR 宏替代SIGNAL宏，ISR 和SIGNAL 是一回事，但以后的版本中SIGNAL 宏将会逐渐被丢弃，所以新的程序建议使用ISR，也就是使用ISR作为中断服务函数名

以下是个外部中断 0 的重写示例：
```c
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

//SIG_INTERRUPT0 外部中断INT0,在io.h中定义
SIGNAL(SIG_INTERRUPT0) 
{
//中断处理程序
}
```

另外 avr-libc 提供两个API 函数用于置位和清零全局中断触发位，它们分别是：
void sei(void) 和 void cli(void)。它们是经常用到的。

# 3.定时器/计数器


# 库收集

## Quantum Mechanical Keyboard Firmware

[Quantum Mechanical Keyboard Firmware](https://github.com/qmk/qmk_firmware). This is a keyboard firmware based on the tmk_keyboard firmware with some useful features for Atmel AVR and ARM controllers, and more specifically, the OLKB product line, the ErgoDox EZ keyboard, and the Clueboard product line. document see  [the official documentation on docs.qmk.fm](https://docs.qmk.fm/)