# 1.最小系统

[参考](https://www.geek-workshop.com/thread-1647-1-1.html)，主要是下载isp口，晶振，电源等

例如使用USBTINYISP下载器，ICSP接口将会是如下的连线：
```
           ┌──────────┐
   MISO    │          │    vcc(5v)
  ──────── │1        2├──────────
   SCK     │          │    MOSI
  ──────── │3        4├───────────
   RESET   │          │    GND
  ──────── │5        6├──────────
           └──────────┘
```

注意，有两种方式可供上传到Arduino：

- ICSP(In-Circuit Serial Programming)
- 使用Bootloader，消耗2KB的程序存储器。如果使用bootload，则应确保“熔丝的BOOTRST=0，如果不是，Bootloader在复位后不会启动。”。BOOTRST熔丝位解释: 复位入口选择 1 1 程序从0x0000 地址开始执行0
程序从引导区确定的入口地址开始执行

实际上，第一个选项也并没有绝对优势。除非你只需要一个USB线，而不是两个。

# 2.USB ISP

硬件是淘宝买的[51+AVR AVR转接板 ISP下载线 isp ](https://detail.tmall.com/item.htm?id=40116348521&spm=a1z09.2.0.0.4c072e8dhDMo6O&_u=31861sba041),使用软件是progisp，是免驱动的。

# 3.usbtinyISP

硬件是淘宝购买的USBtinyisp v2.0,  软件使用avrdude, 也可以通过gui来操作，[AVRDUDESS – A GUI for AVRDUDE ](http://blog.zakkemble.net/avrdudess-a-gui-for-avrdude/)

驱动是C:\download\AVR单片机学习资料打包共享\usbtinyisp驱动资料\usbtinyisp，也可以从https://learn.adafruit.com/usbtinyisp/download 处下载。

注：如果使用Arduino GUI，需要向avr下载arduino bootloader：[图解如何使用 USBTinyIsp](https://www.arduino.cn/thread-21619-1-1.html)中说明了如何下载bootloader

## 使用usbtiny 记录


```shell
C:\Users\tu_xu>c:\prog\winavr\bin\avrdude -c usbtiny -p m16 -F
avrdude: AVR device initialized and ready to accept instructions
Reading | ################################################## | 100% 0.02s
avrdude: Device signature = 0x1e9403
avrdude: safemode: Fuses OK
avrdude done.  Thank you.


C:\Users\tu_xu>c:\prog\winavr\bin\avrdude -c usbtiny -p m16 -U luse:r:-:i

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.02s

avrdude: Device signature = 0x1e9403
"luse" memory type not defined for part "ATMEGA16"

avrdude: safemode: Fuses OK

avrdude done.  Thank you.


C:\Users\tu_xu>c:\prog\winavr\bin\avrdude -c usbtiny -p m16 -U lfuse:r:-:i

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.02s

avrdude: Device signature = 0x1e9403
avrdude: reading lfuse memory:

Reading | ################################################## | 100% 0.01s

avrdude: writing output file "<stdout>"
:01000000E11E
:00000001FF

avrdude: safemode: Fuses OK

avrdude done.  Thank you.


C:\Users\tu_xu>c:\prog\winavr\bin\avrdude -c usbtiny -p m16 -U efuse:r:-:i

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.02s

avrdude: Device signature = 0x1e9403
"efuse" memory type not defined for part "ATMEGA16"

avrdude: safemode: Fuses OK

avrdude done.  Thank you.


C:\Users\tu_xu>c:\prog\winavr\bin\avrdude -c usbtiny -p m16 -U hfuse:r:-:i

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.03s

avrdude: Device signature = 0x1e9403
avrdude: reading hfuse memory:

Reading | ################################################## | 100% 0.01s

avrdude: writing output file "<stdout>"
:0100000019E6
:00000001FF

avrdude: safemode: Fuses OK

avrdude done.  Thank you.


C:\Users\tu_xu>c:\prog\winavr\bin\avrdude -c usbtiny -p m16 -U lfuse:r:-:i

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.02s

avrdude: Device signature = 0x1e9403
avrdude: reading lfuse memory:

Reading | ################################################## | 100% 0.01s

avrdude: writing output file "<stdout>"
:01000000E11E
:00000001FF

avrdude: safemode: Fuses OK

avrdude done.  Thank you.


C:\Users\tu_xu>

```

## 集成在makefile

如果是arduino下的avrdude，因为conf文件放在其他目录，所以需要显示的通过`-C 配置文件`来指定下。

    c:\home>C:\prog\Arduino\hardware\tools\avr\bin\avrdude.exe  -c usbtiny -p m16 -C C:\prog\Arduino\hardware\tools\avr\etc\avrdude.conf

```Makefile
AVRDUDE = avrdude
#---------------- 编程下载选项 (avrdude) ----------------

# 用到少，我就不翻译了,而且也很简单。
# Programming hardware: alf avr910 avrisp bascom bsd
# dt006 pavr picoweb pony-stk200 sp12 stk200 stk500
#
# Type: avrdude -c ?
# to get a full listing.
#
AVRDUDE_PROGRAMMER = usbtiny
# com1 = serial port. Use lpt1 to connect to parallel port.
AVRDUDE_PORT = com1 # programmer connected to serial device
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep
# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
# AVRDUDE_ERASE_COUNTER = -y
# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
# AVRDUDE_NO_VERIFY = -V
# Increase verbosity level. Please use this when submitting bug
# reports about avrdude. See <[url]http://savannah.nongnu.org/projects/avrdude>[/url]
# to submit bug reports.
# AVRDUDE_VERBOSE = -v -v
AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)

#下载程序到目标MCU
program: $(TARGET).hex $(TARGET).eep
    $(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)

```

# 4.熔丝位设置
熔丝位是ATMEL公司AVR单片机比较独到的特征。在每一种型号的AVR单片机内部都有一些特定含义的熔丝位，其特性表现为多次擦写的E²PROM。用户通过配置（编程）这些熔丝位，可以固定地设置AVR的一些特性，参数以及I/O配置等。

特别注意：LB(lock bits)中的LB2/LB1两位不要设置为“10、或00”，这样就加密了。这两位是加密位设置，一旦加密就不能通过任何方式读取芯片内部的flash和eeprom数据。

在AVR的器件手册中，对熔丝位使用已编程（Programmed）和未编程（Unprogrammed）定义熔丝位的状态，“Unprogrammed”表示熔丝状态为“1”（禁止）；“Programmed”表示熔丝状态为“0”（允许）。因此，配置熔丝位的过程实际上是“配置熔丝位成为未编程状态“1”或成为已编程状态“0””。

[AVR® Fuse Calculator](https://www.engbedded.com/fusecalc/)网站提供了可视化的帮助手段让我们获得可以通过avrdude去设置熔丝位的值。

再次推荐[AVRDUDESS – A GUI for AVRDUDE ](http://blog.zakkemble.net/avrdudess-a-gui-for-avrdude/)，有它上面的AVR® Fuse Calculator网址都不用记了，它里面会有提示。