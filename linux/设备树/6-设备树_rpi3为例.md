
# rpi3 分析

[0001_无屏如何使用树莓派.md](https://gitee.com/low-level-of-logic/RaspberryPi/blob/master/docs/0001_%E6%97%A0%E5%B1%8F%E5%A6%82%E4%BD%95%E4%BD%BF%E7%94%A8%E6%A0%91%E8%8E%93%E6%B4%BE.md)

[BCM2835 datasheet](https://www.alldatasheet.com/datasheet-pdf/pdf/502533/BOARDCOM/BCM2835.html)

~/rpi-kernels$ cd linux
~/rpi-kernels/linux$ git checkout d9db059cb982c5478a464c1ff8ce8b17f7768dcc # --> (4.14.21)

rpi-kernels/linux/arch/arm/boot/dts/bcm2710-rpi-3-b.dts

## gpiomem为例 分析用户空间使用

由“linux\drivers\char\broadcom\bcm2835-gpiomem.c”可知道，“/dev/gpiomem”提供了gpio空间的mem-map。同时我们也知道，直接访问内存空间的方法如下：
```go
	file, err := os.OpenFile("/dev/gpiomem", os.O_RDWR|os.O_SYNC, 0660) //|os.O_CLOEXEC
	...
	gpio, err := syscall.Mmap(int(file.Fd()), GPIO_BASE, uint32BlockSize,
	...
```
这里关键的是如何知道“GPIO_BASE”应该填写什么值。 下面我们以raspberry pi 3b为例，说明如何从设备树中来得到它。

下面是rpi3 设备数的一个片段（在目标设备上的打印整个设备树可以通过："`$ dtc -I fs /proc/device-tree`"得到）
```
/ {
	compatible = "raspberrypi,3-model-b", "brcm,bcm2837";
	soc {
		compatible = "simple-bus";
		ranges = <0x7e000000 0x3f000000 0x1000000 0x40000000 0x40000000 0x1000>;
        ...
		gpiomem@7e200000 {
			compatible = "brcm,bcm2835-gpiomem";
			reg = <0x7e200000 0x1000>;
		};
        ...
		gpio: gpio@7e200000 {
			compatible = "brcm,bcm2835-gpio";
			gpio-controller;
			reg = <0x7e200000 0xb4>;
            ...
        }；
    };
};
```

- 首先根据“linux\drivers\char\broadcom\bcm2835-gpiomem.c”中的match compatible，确认该驱动对应的设备节点是"`/soc/gpiomem@7e200000`"
- gpiomem 节点的unit-address是“`0x7e200000`”. 
- gpiomem 节点的父节点（soc）有ranges属性（"`ranges = <0x7e000000 0x3f000000 0x1000000 0x40000000 0x40000000 0x1000>;`"），根据定义`ranges = <child-bus-address1 parent-bus-address1  length1 [child-bus-address2 parent-bus-address2 length2] ... >`,结合上面gpiomem unit-address信息，在soc层面gpiomem对应的地址应该是"`0x3f000000 + 0x200000`"

- 在soc层面gpiomem对应的地址,也就是我们前面要求解的“GPIO_BASE”

## 得到引脚信息 

- 得到一些关于正在使用的引脚的信息

	```bash
	root@raspberrypi:/sys/kernel/debug/pinctrl/3f200000.gpio# cat pingroups 
	registered pin groups:
	group: gpio0
	pin 0 (gpio0)

	group: gpio1
	pin 1 (gpio1)

	group: gpio2
	pin 2 (gpio2)

	group: gpio3
	pin 3 (gpio3)

	```

- 获得有关哪些引脚正在使用（已分配）的信息

	```shell
	root@raspberrypi:/sys/kernel/debug/pinctrl/3f200000.gpio# cat pinmux-pins 
	Pinmux settings per pin
	Format: pin (name): mux_owner gpio_owner hog?
	pin 0 (gpio0): (MUX UNCLAIMED) (GPIO UNCLAIMED)
	...
	pin 31 (gpio31): (MUX UNCLAIMED) (GPIO UNCLAIMED)
	pin 32 (gpio32): 3f201000.serial (GPIO UNCLAIMED) function alt3 group gpio32
	pin 33 (gpio33): 3f201000.serial (GPIO UNCLAIMED) function alt3 group gpio33
	pin 34 (gpio34): 3f300000.mmc (GPIO UNCLAIMED) function alt3 group gpio34
	pin 35 (gpio35): 3f300000.mmc (GPIO UNCLAIMED) function alt3 group gpio35
	pin 36 (gpio36): 3f300000.mmc (GPIO UNCLAIMED) function alt3 group gpio36
	pin 37 (gpio37): 3f300000.mmc (GPIO UNCLAIMED) function alt3 group gpio37
	pin 38 (gpio38): 3f300000.mmc (GPIO UNCLAIMED) function alt3 group gpio38
	pin 39 (gpio39): 3f300000.mmc (GPIO UNCLAIMED) function alt3 group gpio39
	pin 40 (gpio40): soc:audio (GPIO UNCLAIMED) function alt0 group gpio40
	pin 41 (gpio41): soc:audio (GPIO UNCLAIMED) function alt0 group gpio41
	pin 42 (gpio42): (MUX UNCLAIMED) (GPIO UNCLAIMED)

	```

- 得到完整的引脚列表

	```bash
	root@raspberrypi:/sys/kernel/debug/pinctrl/3f200000.gpio# cat pins |more
	registered pins: 54
	pin 0 (gpio0) function gpio_in in hi; irq 0 (none)
	pin 1 (gpio1) function gpio_in in hi; irq 0 (none)
	pin 2 (gpio2) function gpio_in in hi; irq 0 (none)
	pin 3 (gpio3) function gpio_in in hi; irq 0 (none)
	pin 4 (gpio4) function gpio_in in hi; irq 0 (none)
	pin 5 (gpio5) function gpio_in in hi; irq 0 (none)
	pin 6 (gpio6) function gpio_in in hi; irq 0 (none)
	pin 7 (gpio7) function gpio_in in hi; irq 0 (none)
	pin 8 (gpio8) function gpio_in in hi; irq 0 (none)
	pin 9 (gpio9) function gpio_in in lo; irq 0 (none)
	pin 10 (gpio10) function gpio_in in lo; irq 0 (none)

	```