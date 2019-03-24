# USB ISP

硬件是淘宝买的[51+AVR AVR转接板 ISP下载线 isp ](https://detail.tmall.com/item.htm?id=40116348521&spm=a1z09.2.0.0.4c072e8dhDMo6O&_u=31861sba041),使用软件是progisp，是免驱动的。

# usbtinyISP

硬件是淘宝购买的USBtinyisp v2.0,  软件使用avrdude, 也可以通过gui来操作，[AVRDUDESS – A GUI for AVRDUDE ](http://blog.zakkemble.net/avrdudess-a-gui-for-avrdude/)

驱动是C:\download\AVR单片机学习资料打包共享\usbtinyisp驱动资料\usbtinyisp，也可以从https://learn.adafruit.com/usbtinyisp/download 处下载。

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

