# 重要信息

- [AVR 各系列芯片简介](http://www.kovacs.cc/AVRBlog/Files/AVR_Microcontroller_Features.xls)


- 后来又找到一个avr的扩展库函数，名为[Procyon AVRlib](http://www.procyonengineering.com/embedded/avr/avrlib/)，里面涵盖了非常多的常用外设的驱动，都是标准C写的，注释详细，结构清晰，代码风格优美，从中学到了很多东西。  [avr-liberty](https://github.com/dreamiurg/avr-liberty)，是基于AVRlib，增加了功能的另外一个库。
- 在..\Arduino\reference 文件夹内找到arduino官方IDE的所有库函数的说明文档。

- 堆栈指针SPL/SPH设置

    只要程序中使用了中断和子程序调用，就必须正确的设置堆栈指针寄存器SP，在SRAM空间建立堆栈区。由于RESET后SPL/SPH都设置为0，所以重新设置堆栈是必须的动作。

    AVR的堆栈区是建立在SRAM空间的，16位的SP寄存器可以寻址的空间为64K。但在实际应用中，还必须考虑所使用AVR芯片SRAM空间的实际情况和所配备的SRAM容量的大小。

    首先，堆栈区应该避开寄存器区域所对应的SRAM空间，防止堆栈操作时改变了寄存器的设置。由于AVR的堆栈是向下增长的，即新数据进入堆栈时栈顶指针的数据将减小
    （注意：这里与51不同，51的堆栈是向上增长的，即进栈操作时栈顶指针的数据将增加），所以尽管原则上堆栈可以在SRAM的任何区域中，但通常初始化时将SP的指针设在SRAM最高处。

    对于具体的ATmega16芯片，堆栈指针必须指向高于$0060的SRAM 地址空间，因为低于$0060的区域为寄存器空间。
    ATmega16片内集成有1K的SRAM，不支持外部扩展SRAM，所以堆栈指针寄存器SP的初始值应设在SRAM的最高端：`$045F处（参考atmegea16数据手册 Figure 9）`。

C:\prog\Arduino\hardware\arduino\avr\cores\arduino\Arduino.h

- [Machinekit Raspbian Jessie for RPi1 and RPi2](https://www.raspberrypi.org/forums/viewtopic.php?p=920778)

linuxcnc raspberry : http://www.wire2wire.org/PICnc_5axis/PICnc_5axis.html, https://github.com/kinsamanka/PICnc-V2/wiki

# 引脚说明

## arduino UNO 引脚与AVR单片机(AtMega328p)管脚对应关系

    注：tu焊接的最小电路，在uno 13上有个led

   |UNO<br/>数字引脚|UNO<br>模拟引脚|AVR 管脚|AVR扩展功能|
   |--|--|----|--------|
   |0 |/ |PD 0|USART RX|
   |1 |/ |PD 1|USART TX|
   |2 |/ |PD 2|Ext int 0|
   |3 |/ |PD 3|PWM T2B,Ext int 1|
   |4 |/ |PD 4|/|
   |5 |/ |PD 5|PWM T0A|
   |6 |/ |PD 6|PWM T0B|
   |7 |/ |PD 7|/|
   |8 |/ |PB 0|input capture|
   |9 |/ |PB 1|PWM T1A|
   |10|/ |PB 2|PWM T1B,SS|
   |11|/ |PB 3|PWM T2A,MOSI|
   |12|/ |PB 4|SPI MISO|
   |13|/ |PB 5|SPI SCK|
   |14|A0|PC 0|/|
   |15|A1|PC 1|/|
   |16|A2|PC 2|/|
   |17|A3|PC 3|/|
   |18|A4|PC 4|I2C SDA|
   |19|A5|PC 5|I2C SCL|
   |||||


## AVR引脚使用

通常在采用正电压的电路上单片机上，是

- AVCC—正模拟电源电压
- AVSS—模拟电路地
- DVCC—正数字电源电压
- DVSS—数字电路地
- AVCC/AVSS一般为模拟部分提供电源，DVCC/DVSS一般为数字部分和内部I/O提供电源，为了避免引入电源干扰，通常需要把AVCC/AVSS和DVCC/DVSS隔离开。 

对于AVR单片机，引脚名字与通用的有些差别

- AVcc：为端口A和片内ADC模拟电路电源输入引脚。不使用ADC时，直接连接到电源正极；使用ADC时，应通过一个低通电源滤波器与Vcc连接。
- AREF：使用ADC时，可作为外部ADC参考源的输入引脚。

AVR 有32根I/O引脚，分成PA、PB、PC和PD四个8位端口，他们全部是可编程控制的双（多）功能复用的I/O引脚（口）。
四个端口的第一功能是通用的双向数字输入/输出（I/O）口，其中每一位都可以由指令设置为独立的输入口，或输出口。当I/O设置为输入时，引脚内部还配置有上拉电阻，这个内部的上拉电阻可通过编程设置为上拉有效或上拉无效。芯片Reset复位后，所有I/O口的缺省状态为输入方式，上拉电阻无效，即I/O为输入高阻的三态状态。

TODO

如果AVR的I/O口设置为输出方式工作，当其输出高电平时，能够输出20mA的电流，而当其输出低电平时，可以吸收40mA的电流。因此AVR的I/O口驱动能力非常强，能够直接驱动LED发光二极管、数码管等。而早期单片机I/O口的驱动能力只有5mA，驱动LED时，还需要增加外部的驱动电路和器件。


## avr系统时钟与晶振

时钟周期：单片机外接的晶振的振荡周期就是时钟周期，时钟周期=振荡周期。比方说，80C51单片机外接了一个11.0592M的晶体振荡器，那我们就说这个单片机系统的时钟周期是1/11.0592M，这里要注意11.0592M是频率，周期是频率的倒数。

机器周期：单片机执行指令所消耗的最小时间单位。

ATmega16的片内含有4种频率（1/2/4/8M）的RC振荡源，可直接作为系统的工作时钟使用。同时片内还设有一个由反向放大器所构成的OSC（Oscillator）振荡电路，外围引脚XTAL1和XTAL2分别为OSC振荡电路的输入端和输出端，用于外接石英晶体等，构成高精度的或其它标称频率的系统时钟系统。如ATmega16最高的工作频率为16M（16MIPS），这个最高频率是通过外部时钟达到。

使用AVR时要特别注意：AVR单片机有一组专用的，与芯片功能、特性、参数配置相关的可编程熔丝位。其中有几个专门的熔丝位（CKSEL3..0）用于配置芯片所要使用的系统时钟源的类型。
atmega16新芯片的缺省配置设定为使用内部1M的RC振荡源作为系统的时钟源。因此当第一次使用前，必须先正确的配置熔丝位，使其与使用的系统时钟源类型相匹配。另外，在配置其它熔丝位时，或进行程序下载时，千万不要对CKSEL3..0这几个熔丝位误操作，否则会组成芯片表面现象上的“坏死”，因为没有系统时钟源，芯片不会工作的。



### 通过指令周期实现延时

51单片机采用的CISC(复杂指令指令集，哈佛架构)，所以有很多条指令，并且各条指令执行的时间也可能不一样(有一样的哦)，但是它们执行的时间必须是机器周期的整数倍，这就是机器周期的意义所在。8051系列单片机又在这个基础上进行细分，将一个机器周期划分为6个状态周期，也就是S1-S6，每个状态周期又由两个节拍组成，P1和P2，而P1=P2=时钟周期。这也就是经常说的8051系列单片机的的时钟频率是晶振频率的12分频，或者是1/12，就是这个意思。现在(截至2012)新的单片机已经能做到不分频了，就是机器周期=时钟周期。

AVR单片机采用的是RISC指令集，它执行的机器周期与时钟周期是相等的，不像传统的51单片机要进行12分频。 当晶振为8MHZ时，机器周期和时钟周期均为1/8us。 ADC的时钟周期是给AD转换使用的周期，一般速度会低一些，在程序中往往对时钟周期进行一定的分频，把频率降下来。

假如晶振是1MHZ, 那执行一个指令的时间就是$\frac{1}{100 0000}$秒, 基于晶振频率增加，速度线性增加的考虑，对单片机的延时可以有以下算法：

```c
// CPU clock speed
//#define F_CPU        16000000     // 16MHz processor
//#define F_CPU        14745000     // 14.745MHz processor
//#define F_CPU        8000000      // 8MHz processor
//#define F_CPU        7372800      // 7.37MHz processor
//#define F_CPU        4000000      // 4MHz processor
//#define F_CPU        3686400      // 3.69MHz processor
// cpu cycles per microsecond 这个500000是为了取整
#define CYCLES_PER_US ((F_CPU+500000)/1000000)

// delay for a minimum of <us> microseconds 
// the time resolution is dependent on the time the loop takes 
// e.g. with 4Mhz and 5 cycles per loop, the resolution is 1.25 us
void delay_us(unsigned short time_us)
{
   unsigned short delay_loops;
   register unsigned short i;

   delay_loops = (time_us+3)/5*CYCLES_PER_US; // +3 for rounding up (dirty)

   // one loop takes 5 cpu cycles
   for (i=0; i < delay_loops; i++) {};
}


```

### 晶振

如果单片机的晶振频率为1MHz的话，则晶振周期是1us, 如果是4M，那就是0.25us，AVR单片机的机器指令一般是一个时钟周期。

晶振通常分为无源晶振和有源晶振两种类型，无源晶振一般称之为 crystal（晶体），而有源晶振则叫做 oscillator（振荡器）。

有源晶振是一个完整的谐振振荡器，它是利用石英晶体的压电效应来起振，所以有源晶振需要供电，当我们把有源晶振电路做好后，不需要外接其它器件，只要给它供电，它就可以主动产生振荡频率，并且可以提供高精度的频率基准，信号质量也比无源信号要好。

无源晶振自身无法振荡起来，它需要芯片内部的振荡电路一起工作才能振荡，它允许不同的电压，但是信号质量和精度较有源晶振差一些。相对价格来说，无源晶振要比有源晶振价格便宜很多。无源晶振两侧通常都会有个电容，一般其容值都选在 10 pF~40 pF 之间，如果手册中有具体电容大小的要求则要根据要求来选电容，如果手册没有要求，我们用 20 pF 就是比较好的选择，这是一个长久以来的经验值，具有极其普遍的适用性。

## 熔丝位FUSE

- BOD （电源检测缩写 Brownout Detector ）
    对于单片机5V系统，设置BOD电平为4.0V；对于3V系统，设置BOD电平为2.7V。然后允许BOD检测。

    AVR是宽电压工作的芯片，当电压跌至2.5V，系统程序还能工作。但已经是胡乱的跑了，所以生产系统必须要设置BOD的熔丝信息。[关于AVR的BOD](http://www.eeworld.com.cn/mcu/2013/0511/article_12991.html)里面介绍了个工程实践是5V系统设置2.7.

[avr单片机熔丝位设置方法及步骤及设置注意事项 - 全文](http://www.elecfans.com/dianzichangshi/20171122583848_a.html)

[Proteus仿真AVR单片机时使用外部晶振熔丝位设置](http://www.eeworld.com.cn/mcu/2015/1012/article_22884.html)_


CKDIV8  SUT



## LED发光二极管

发光二极管一般为砷化镓半导体二极管，其电路图6-5所示。当电压U1大于U2约1V以上时，二极管导通发光。当导通电流大于5mA时，人的眼睛就可以明显地观察到二极管的发光。导通电流越大，亮度越高。一般导通电流不要超过10mA，否则将导致二极管的烧毁或I/O引脚的烧毁。因此在设计硬件电路时，要在LED二极管电路中串接一个限流电阻，阻值在300～1000Ω之间，调节阻值的大小可以控制发光二极管的发光亮度

根据$I=\frac{U_1 - U_2 - V_{led}}{R}$,其中$U_1 , U_2$是两端电压，$V_{led}$是LED的导通电压，$R$ 是限流电阻。

同时根据[发光二极管（led）的导通压降和电流](http://blog.sina.com.cn/s/blog_817569a601019923.html), 按照${U_1 - U_2}$为5V，$V_{led}$为2V，$I$大小20mA来看，R的大小是$I=\frac{5 - 2}{20}1000=150$Ω

## 继电器与可控硅控制todo

- [如何用好可控硅？](https://www.amobbs.com/thread-643997-1-1.html)

在工业控制以及许多场合中，嵌入式系统要驱动一些继电器和电磁开关，用于控制马达的开启和关闭，阀门的开启和关闭等。继电器和电磁开关需要功率驱动，驱动电流往往需要几百毫安，超出了AVR本身I/O口的驱动能力，因此在外围硬件电路中要考虑使用功率驱动

晶闸管(Thyristor)又叫可控硅，按照其工作特性又可分单向可控硅(SCR)、双向可控硅(TRIAC)。其中双向可控硅又分四象限双向可控硅和三象限双向可控硅。同时可控硅又有绝缘与非绝缘两大类，如ST的可控硅用BT名称后的“A”、与“B”来区分绝缘与非绝缘。  



## 步进电机


- 八拍方式驱动，顺序为A AB B BC C CD D DA

- 1 ) 4988驱动板可以驱动57电机吗？4988可以驱动的电机跟尺寸关系不大，主要与工作电流有关，理论上电流小于2A的步进电机都是可以驱动的，不论是42还是57电机。
- 2）4988可以驱动多大的电流？如果4988芯片上没有加散热片，电流最好在1.2A以下。如果加散热片，电流可以达到2A。



- [VR Tutorial](http://www.ladyada.net/learn/avr/index.html)
- [Using AVR microcontrollers: Minimalist target boards](https://www.evilmadscientist.com/2007/using-avr-microcontrollers-minimalist-target-boards/)
- [ AVR Fuse Calculator](http://www.engbedded.com/fusecalc/)
- [Use Arduino as an ISP programmer to program non-Arduino AVR microcontrollers](https://hardwarefun.com/tutorials/use-arduino-as-an-isp-programmer-to-program-non-arduino-avr-microcontrollers)
- [gcc-AVR-Options](http://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html)


# atmega328P替代

[LGT8F328P 官网](http://www.lgtic.com/downloads/), 在arduino下的支持见该链接网页下的“LGT8FX8D/P系列Arduino硬件支持包(v3.6c)”