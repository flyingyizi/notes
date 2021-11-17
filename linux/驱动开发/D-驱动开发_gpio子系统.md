
# 6.gpio 子系统

[gpio子系统和pinctrl子系统（上）](https://www.cnblogs.com/rongpmcu/p/7662751.html)
[gpio子系统和pinctrl子系统（中）](https://www.cnblogs.com/rongpmcu/p/7662755.html)
[gpio子系统和pinctrl子系统（下）](https://www.cnblogs.com/rongpmcu/p/7662756.html)


[linux内核中的GPIO系统之（1）：软件框架](http://www.wowotech.net/gpio_subsystem/io-port-control.html)

[linux内核中的GPIO系统之（2）：pin control subsystem](http://www.wowotech.net/gpio_subsystem/pin-control-subsystem.html)

[Linux内核中的GPIO系统之（3）：pin controller driver代码分析](http://www.wowotech.net/gpio_subsystem/pin-controller-driver.html)

[linux内核中的GPIO系统之（4）：pinctrl驱动的理解和总结](http://www.wowotech.net/gpio_subsystem/pinctrl-driver-summary.html)

[ linux内核中的GPIO系统之（5）：gpio subsysem和pinctrl subsystem之间的耦合](http://www.wowotech.net/gpio_subsystem/pinctrl-and-gpio.html)


gpio子系统帮助我们管理整个系统gpio的使用情况，同时通过sys文件系统导出了调试信息和应用层控制接口。它内部实现主要提供了两类接口，一类给bsp工程师，用于注册gpio chip（也就是所谓的gpio控制器驱动），另一部分给驱动工程师使用，为驱动工程师屏蔽了不同gpio chip之间的区别，驱动工程师调用的api的最终操作流程会导向gpio对应的gpio chip的控制代码，也就是bsp的代码。

“other drivers” --------gpio subsystem --------"gpio controller dirver (BSP)" --- hardware layer

## gpio DTS相关

“Documentation\devicetree\bindings\gpio\gpio.txt”

### 定义gpio-controller节点

定义gpio-controller节点： `gpio-controller` 与`#gpio-cells` 必须，其他可选

例子：
```c++
gpio-controller@00000000 {
	compatible = "foo";
	reg = <0x00000000 0x1000>;
	// 声明该节点是一个GPIO Controller
	gpio-controller;
	// 示这个控制器下每一个引脚要用2个32位的数(cell)来描述
	#gpio-cells = <2>;
	// informs the driver that only the first 18 GPIOs, at local offset 0 .. 17, are in use.
	ngpios = <18>;
	// indicates the start and size of the GPIOs that can't be used.
	gpio-reserved-ranges = <0 4>, <12 2>;
	//  defining the names of the GPIO lines going out of the GPIO controller.
	gpio-line-names = "MMC-CD", "MMC-WP", "VDD eth", "RST eth", "LED R",
		"LED G", "LED B", "Col A", "Col B", "Col C", "Col D",
		"Row A", "Row B", "Row C", "Row D", "NMI button",
		"poweroff", "reset";
}
```

### gpio- and pin-controller interaction

在实践中，通常将gpio-controller提供的gpios路由到pin-controller，这就需要引入“gpio-ranges”属性，如下面例子演示的，`pins 20..29 on pin controller "pinctrl" is mapped to GPIO line 0..9`
```c++
/ {
	soc {
		pinctrl: pin-controller {
			compatible = "st,stm32f429-pinctrl";
			#address-cells = <1>;
			#size-cells = <1>;
			ranges = <0 0x40020000 0x3000>;
			...
			pins-are-numbered;

			gpioa: gpio@40020000 {
				gpio-controller;
				#gpio-cells = <2>;
				reg = <0x0 0x400>;

				// The format is: <[pin controller phandle], [GPIO controller offset],
				//                 [pin controller offset], [number of pins]>;
				// This means:
				// - pins 20..29 on pin controller "pinctrl" is mapped to GPIO line 0..9 and
				// - pins 50..69 on pin controller "pinctrl" is mapped to GPIO line 10..29
                gpio-ranges = <&pinctrl 0 20 10>, <&pinctrl 10 50 20>;
				...
			};

			gpiob: gpio@40020400 {
				gpio-controller;
				#gpio-cells = <2>;
				reg = <0x400 0x400>;
				...
			};
		};
	};
};
```





### 设备树中引用gpio 引脚

在其他节点引用gpio可以使用的方式，GPIO properties should be named :
- "[<name>-]gpios", with `<name>` being the purpose of this GPIO for the device. While a non-existent `<name>` is considered valid for compatibility reasons (resolving to the "gpios" property), it is not allowed for new bindings. Also, GPIO properties named 
- "[<name>-]gpio" are valid and old bindings use it, but are only supported for compatibility reasons and should not be used for newer bindings since it has been deprecated.

GPIO properties can contain one or more GPIO phandles, but only in exceptional
cases should they contain more than one. If your device uses several GPIOs with
distinct functions, reference each of them under its own property, giving it a
meaningful name. The only case where an array of GPIOs is accepted is when
several GPIOs serve the same function (e.g. a parallel data line).

The exact purpose of each gpios property must be documented in the device tree
binding of the device.


```c++
	gpio1: gpio1 {
		gpio-controller;
		#gpio-cells = <2>;
	};
	[...]

	data-gpios = <&gpio1 12 0>,
		     <&gpio1 13 0>,
		     <&gpio1 14 0>,
		     <&gpio1 15 GPIO_ACTIVE_HIGH>;
```

In the above example, &gpio1 uses 2 cells to specify a gpio. The first cell is
a local offset to the GPIO line and the second cell represent consumer flags,
such as if the consumer desire the line to be active low (inverted) or open
drain. This is the recommended practice.


## BSP工程师视角使用GPIO子系统API

用于注册gpio chip（也就是所谓的gpio控制器驱动），即围绕gpio controller的相关API。

documents/gpio/driver.txt
documentation/driver-api/pinctrl.rst

[和device tree或者machine driver相关的接口](http://www.wowotech.net/linux_kenrel/pin-control-subsystem.html)

- 编写gpio controller驱动，主要是：构造gpio_chip, 然后通过 gpiochip_add_data/devm_gpiochip_add_data/gpiochip_add 注册它。

- 编写pinctrl驱动，主要： 构造struct pinctrl_desc用于表示一个pin控制器，然后通过`[devm_]pinctrl_register()`将其注册进pinctrl子系统；

	注：pinctrl是在linux 3.4引入的。

现在流行的编写方式是： 在设备树描述gpio信息，然后在pinctrl驱动里面顺便构造gpio_chip,并以gpiochip_add_data注册到gpio子系统。 参见“`drivers/pinctrl/stm32/pinctrl-stm32.c`”例子。

Requesting self-owned GPIO pins:

	有时候需要允许GPIO chip driver 使用GPIO description， 在这种情况下，可以使用下面的api，不过注意它们不会影响使用计数，并且它们只能在gpio chip driver自身内部使用

	```c++
	/*Descriptors requested with gpiochip_request_own_desc() must be released with
	  gpiochip_free_own_desc().*/
	struct gpio_desc *gpiochip_request_own_desc(struct gpio_desc *desc,
						    const char *label)   
	void gpiochip_free_own_desc(struct gpio_desc *desc)
	```

以"`gpiochip_add_data`"为例，它将gpio chip添加到gpio子系统中，核心是：
- 将chip添加到全局的gpio chip链表中，用于gpio管理
- 将该gpio chip对应的gpio段全部初始化
- 调用of_gpiochip_add初始化设备树相关的信息，用于后面的属性解析及向pinctrl子系统同步下
- 导出到sys



```c++
int gpiochip_add(struct gpio_chip *chip)
{
	...
	int		base = chip->base;
	//如果指定了base，也就是指定了启示gpio号，需要校验下chip的所有gpio是否有效
    //这里会用到ARCH_NR_GPIOS宏，它可以在配置的时候，通过CONFIG_ARCH_NR_GPIO修改，
    //否则采用默认值256
	if ((!gpio_is_valid(base) || !gpio_is_valid(base + chip->ngpio - 1))
			&& base >= 0) {
		status = -EINVAL;
		goto fail;
	}
    ...
	//如果没有指定base，那么需要基于该chip的gpio数量在系统支持的gpio范围里找一段区间给该chip
	if (base < 0) {
		base = gpiochip_find_base(chip->ngpio);
		if (base < 0) {
			status = base;
			goto unlock;
		}
		chip->base = base;
	}

	//到这里的时候，说明一切正常，把它加入到全局的gpiochip链表中去吧，注意，加入的时候会基于base排序
    //这也保证了gpiochip_find_base的实现
	status = gpiochip_add_to_list(chip);

	//如果加入成功，最后一步就是初始化该chip对应的那些gpio了
	if (status == 0) {
		chip->desc = &gpio_desc[chip->base];

		for (id = 0; id < chip->ngpio; id++) {
			struct gpio_desc *desc = &chip->desc[id];
            //将该chip对应的那些gpio对应的数据结构desc初始化，指向拥有它的chip
			desc->chip = chip;

			/* REVISIT:  most hardware initializes GPIOs as
			 * inputs (often with pullups enabled) so power
			 * usage is minimized.  Linux code should set the
			 * gpio direction first thing; but until it does,
			 * and in case chip->get_direction is not set,
			 * we may expose the wrong direction in sysfs.
			 */
            //如果chip driver没有指定chip->direction_input，意味着不是输入，那就设置为输出咯
			desc->flags = !chip->direction_input
				? (1 << FLAG_IS_OUT)
				: 0;
		}
	}

	spin_unlock_irqrestore(&gpio_lock, flags);
    ...
	//初始化设备树相关的信息，后面会详细讲一下这部分
	of_gpiochip_add(chip);
    ...
	//将该gpiochip导出到sys，用于调试和应用层直接操作
	status = gpiochip_export(chip);
	if (status)
		goto fail;

	pr_debug("%s: registered GPIOs %d to %d on device: %s\n", __func__,
		chip->base, chip->base + chip->ngpio - 1,
		chip->label ? : "generic");
	...
}
```

## 驱动工程师视角使用GPIO子系统API


 [GPIO和Pinctrl子系统的使用](https://www.bilibili.com/read/cv5153694)


建议使用新版的接口（documents/gpio/consumer.txt）,对应头文件使用“`#include <linux/gpio/consumer.h>`”,提供类似`gpiod_*`的API。老版的接口是“documents/gpio/gpio-legacy.txt”，对应头文件使用“`#include <linux/gpio.h>`”,提供类似`gpio_*`的API。





下面只讨论新版的接口使用。

函数名均是`gpiod_xxx`的。 内核中操作gpio的地方都是要调用该类接口。



- 基于消息机制访问GPIO
	类似I2C/SPI等，Some GPIO controllers must be accessed using message based buses like I2C or
	SPI. Commands to read or write those GPIO values require waiting to get to the
	head of a queue to transmit a command and get its response. This requires
	sleeping, which can't be done from inside IRQ handlers.

	Accessing such GPIOs requires a context which may sleep, for example a threaded
	IRQ handler, and those accessors must be used instead of spinlock-safe
	accessors without the cansleep() name suffix.

	Other than the fact that these accessors might sleep, and will work on GPIOs
	that can't be accessed from hardIRQ handlers, these calls act the same as the
	spinlock-safe calls.

	```c++
	/* Platforms that support this type of GPIO distinguish them from other GPIOs by
	   returning nonzero from this call:*/
	int gpiod_cansleep(const struct gpio_desc *desc)

    /* To access such GPIOs, a different set of accessors is defined:
	*/
	int gpiod_get_value_cansleep(const struct gpio_desc *desc)
	void gpiod_set_value_cansleep(struct gpio_desc *desc, int value)
    ```

- Interacting With the Legacy GPIO Subsystem

	“Documentation\gpio\consumer.txt”

	Many kernel subsystems still handle GPIOs using the legacy integer-based
	interface. Although it is strongly encouraged to upgrade them to the safer
	descriptor-based API, the following two functions allow you to convert a GPIO
	descriptor into the GPIO integer namespace and vice-versa:

		int desc_to_gpio(const struct gpio_desc *desc)
		struct gpio_desc *gpio_to_desc(unsigned gpio)

	The GPIO number returned by desc_to_gpio() can be safely used as long as the
	GPIO descriptor has not been freed. All the same, a GPIO number passed to
	gpio_to_desc() must have been properly acquired, and usage of the returned GPIO
	descriptor is only possible after the GPIO number has been released.

	Freeing a GPIO obtained by one API with the other API is forbidden and an
	unchecked error.


### A.1.请求gpio

//请求一个/一组gpio

- 获得GPIO：
	|descriptor-based         |legacy             | 说明|
	|-------------------------|-------------------|-----|
	|gpiod_get                |gpio_request       ||
	|gpiod_get_optional       |gpio_request_one   ||	
	|gpiod_get_index          |                   ||
	|gpiod_get_array          |gpio_request_array |处理多个gpio|
	|devm_gpiod_get           |devm_gpio_request  ||
	|devm_gpiod_get_optional  |gpio_request_one   ||	
	|devm_gpiod_get_index     |                   ||
	|devm_gpiod_get_array     |                   |处理多个gpio|
	||||

注：有前缀“devm_”的含义是“设备资源管理”(Managed Device Resource)，这是一种自动释放资源的机制。它的思想是“资源是属于设备的，设备不存在时资源就可以自动释放”。 

以"`int gpio_request(unsigned gpio, const char *label)`"为例，gpio_request主要做了以下动作：

- 检查是否已经被申请，没有的话，标记为已申请
- 填充label到该pin数据结构，用于debug
- 如果chip　driver提供了request回调，调用它
- 如果chip　driver提供了get_direction回调，调用它,通过它更新pin数据结构，标明gpio方向

gpio_request_one多一个flags参数，通过该参数，可以指定GPIOF_OPEN_DRAIN、GPIOF_OPEN_SOURCE、GPIOF_DIR_IN、GPIOF_EXPORT等标志，如果指定了GPIOF_DIR_IN，那么后面就不需要自己再额外调用gpio_direction_input或者gpio_direction_output了，如果指定了GPIOF_EXPORT，后面就不需要自己调用gpio_export了。

在下面例子中，演示了如何使用con_id参数，与idx参数：
```c++
	/*
	*struct gpio_desc *__must_check gpiod_get(struct device *dev, const char *con_id)
	*struct gpio_desc *gpiod_get_index(struct device *dev,
	*					  const char *con_id, unsigned int idx,
	*					  enum gpiod_flags flags)
	*/
	/* dts sample:
	*device_node {
	*                ...
	*              gpio_name = <&msm_gpio 99 0>;    //gpio_99
	*                ...
	*              led-gpios = <&gpio 15 GPIO_ACTIVE_HIGH>, // red 
	*                         <&gpio 16 GPIO_ACTIVE_HIGH>, // green 
	*                         <&gpio 17 GPIO_ACTIVE_HIGH>; // blue 
	*              power-gpios = <&gpio 1 GPIO_ACTIVE_LOW>;
	*}
	*/

  //legacy method
　int gpio_99 = of_get_named_gpio_flags(dev->of_node, "gpio_name", 0, NULL);    //从设备数节点, 可选
  gpio_request(gpio_99, "gpio_name");        //通过gpio号申请gpio
  gpio_direction_output(gpio_99, 1);         //设置gpio_99输出，初始值为1
  gpio_set_value(gpio_99, 0);                //设置gpio_99值为0
  gpio_free(gpio_99);                        //gpio_99不再使用后应当释放
  // gpio的申请和设置都可能会出现失败的情况，应该做好异常处理;；

  //descriptor method
  struct gpio_desc *red, *green, *blue, *power;
  red = gpiod_get_index(dev, "led", 0, GPIOD_OUT_HIGH); //内部会调用of_get_named_gpiod_flags
  green = gpiod_get_index(dev, "led", 1, GPIOD_OUT_HIGH);
  blue = gpiod_get_index(dev, "led", 2, GPIOD_OUT_HIGH);
  power = gpiod_get(dev, "power", GPIOD_OUT_HIGH); 
```
 
<!-- 
extern int pinctrl_gpio_request(unsigned gpio);
extern void pinctrl_gpio_free(unsigned gpio);
extern int pinctrl_gpio_direction_input(unsigned gpio);
extern int pinctrl_gpio_direction_output(unsigned gpio); -->

### A.2.设置gpio方向

//设置gpio方向为输入/输出
gpio_direction_input或者gpio_direction_output        ---------<2>

- 设置方向：
	|descriptor-based       |legacy               | 说明|
	|-----------------------|---------------------|-----|
	|gpiod_direction_input  |gpio_direction_input ||
	|gpiod_direction_output |gpio_direction_output||
	||||

gpio_direction_input或者gpio_direction_output用来设置该gpio为输入还是输出，它们主要是回调gpio chip driver提供的direction_input或者direction_output来设置该gpio寄存器为输入、输出。

- 通过"gpiod_direction_*()" 设置方向，如果已经通过"gpiod_get*()"设置了方向，也可以省略:
	```c++
	/* return value is zero for success, else a negative errno.*/
	int gpiod_direction_input(struct gpio_desc *desc)
	int gpiod_direction_output(struct gpio_desc *desc, int value)
	...
	/* return either GPIOF_DIR_IN or GPIOF_DIR_OUT.*/
	int gpiod_get_direction(const struct gpio_desc *desc);
    ```



### A.3.sysfs文件系统导出gpio

“Documentation\gpio\sysfs.txt”

//将该gpio通过sys文件系统导出，应用层可以通过文件操作gpio
                                          ---------<3>

	|descriptor-based       |legacy               | 说明|
	|-----------------------|---------------------|-----|
	|gpiod_export           |gpio_export          ||
	|gpiod_unexport         |gpio_unexport        ||
	|gpiod_export_link      |gpio_export_link     ||


Exporting from Kernel code

	Kernel code can explicitly manage exports of GPIOs which have already been
	requested using gpio_request():

		/* export the GPIO to userspace */
		int gpiod_export(struct gpio_desc *desc, bool direction_may_change);

		/* reverse gpio_export() */
		void gpiod_unexport(struct gpio_desc *desc);

		/* create a sysfs link to an exported GPIO node */
		int gpiod_export_link(struct device *dev, const char *name,
				struct gpio_desc *desc);

	After a kernel driver requests a GPIO, it may only be made available in
	the sysfs interface by gpiod_export(). The driver can control whether the
	signal direction may change. This helps drivers prevent userspace code
	from accidentally clobbering important system state.

	This explicit exporting can help with debugging (by making some kinds
	of experiments easier), or can provide an always-there interface that's
	suitable for documenting as part of a board support package.

	After the GPIO has been exported, gpiod_export_link() allows creating
	symlinks from elsewhere in sysfs to the GPIO sysfs node. Drivers can
	use this to provide the interface under their own device in sysfs with
	a descriptive name.

一旦导出到sysfs文件空间，下面演示了如何通过sysfs来操纵gpio

```shell
# show overview information
$ gpiodetect 
gpiochip0 [pinctrl-bcm2835] (54 lines)
gpiochip1 [brcmexp-gpio] (8 lines)
gpiochip2 [brcmvirt-gpio] (2 lines)

$ cd /sys/class/gpio 
/sys/class/gpio$ ls
export  gpiochip0  gpiochip100  gpiochip128  unexport
# export gpio4 from kernel to user space
$echo 4 > /sys/class/gpio/export
# set direction
$echo out >/sys/class/gpio/gpio4/direction
# set value to 1
$echo 1 >/sys/class/gpio/gpio4/value
# unregister gpio4 
$echo 4 > /sys/class/gpio/unexport

```


### A.4.读写gpio

//如果gpio为输入，获取gpio值，如果gpio为输出，可以设置gpio高低电平

内部也是调用gpio chip driver提供的get、set

- 读写：
	|descriptor-based |legacy         | 说明|
	|-----------------|---------------|-----|
	|gpiod_get_value  |gpio_get_value ||
	|gpiod_set_value  |gpio_set_value ||
	||||

-  access GPIOs from an atomic context:
	```c++
	/* The values are boolean, zero for low, nonzero for high.*/
	int gpiod_get_value(const struct gpio_desc *desc);
	void gpiod_set_value(struct gpio_desc *desc, int value);
	```


### A.5.注册gpio irq

"`request_irq(gpio_to_irq(gpio_num)...)`"将gpio转为对应的irq，然后注册该irq的中断handler

	|descriptor-based |legacy         | 说明|
	|-----------------|---------------|-----|
	|gpiod_to_irq     |gpiod_to_irq   ||
	||||


用于获取该gpio对应的中断号，这个需要设备树里的该gpio节点描述使用哪个中断号（并不是所有的gpio都可以触发中断的）。它里面的实现就是回调gpio chip driver提供的“gpio_chip.to_irq"

### A.5.释放gpio

//释放请求的一个或者一组gpio
gpio_free/devm_gpio_free、gpio_free_array                           ---------<6>

- 释放GPIO：
	|descriptor-based    |legacy             | 说明|
	|--------------------|-------------------|-----|
	|gpiod_free          |gpio_free          ||
	|gpiod_put           |gpio_free_array    ||
	|gpiod_put_array     |                   ||
	|dvm_gpiod_get       |                   ||
	|dvm_gpiod_put       |                   ||
	|dvm_gpiod_put_array |                   ||
	||||
