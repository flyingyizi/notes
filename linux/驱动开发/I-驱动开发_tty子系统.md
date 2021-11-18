
# 10.tty子系统

[tty驱动分析](http://www.wowotech.net/sort/tty_framework)

下面表达的是tty子系统的不同层间的交互示意：
```
用户空间：  write()
              |
			  v
字符设备：  char device driver
              |
			  v
tty核心：   tty_write()
              |
			  v
线路规程驱动：ldisc.write()
              |
			  v
tty驱动：   tty_driver.write()
              |
			  v
硬件层：     
```

显然对于uart这种硬件，完全可以按照上面的方式来实现其驱动。但考虑到uart这类设备有其特殊的共性，为给开发者提供更多便利，针对uart硬件，linux对tty驱动支撑api（e.g.tty_register_driver...）进行了封装，提供了独有的uart驱动支撑(e.g.uart_register_driver...),这个我们可以查看“uart_register_driver”代码得知，它内部还是调用的“tty_register_driver”。 因此针对uart这类硬件上图就可以表达为：

```
用户空间：  write()
              |
			  v
tty核心：   tty_operations.write()
              |
			  v
线路规程驱动：ldisc.write()
              |
			  v
uart驱动(tty驱动特例)： uart_driver.write()
              |
			  v
硬件层：     uart硬件 
```



## BSP工程师视角使用

### 关键数据结构

- 特定UART相关的驱动程序结构：
```c++
struct uart_driver {
	struct module		*owner;
	const char		*driver_name;
	const char		*dev_name; /*/dev node name such as ttyS*/
	...
	struct tty_driver	*tty_driver; /*tty dirver*/
};
```

- struct uart_port

- struct uart_ops  uart驱动程序硬件上可完成的操作

### uart 驱动的关键步骤



下面的例子
```c++
static const struct uart_ops xxx_uart_ops = {
	.start_tx	= xxx_start_tx,
	...
};
static const struct dev_pm_ops xxx_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(xxx, xxx)
};
static const struct uart_port xxx_port ={
	.ops	= &xxx_uart_ops;
	...
}
static int xxx_serial_probe(struct platform_device *pdev)
{
	...
	ret = uart_add_one_port(&stm32_usart_driver, &xxx_port);
    ...
}
static struct platform_driver xxx_serial_driver = {
	.probe		= xxx_serial_probe,
	...
	.driver	= {.pm	= &xxx_pm_ops,...,
	},
};
static struct uart_driver xxx_usart_driver = {
	...
};

static int __init usart_init(void){
	...
	ret = uart_register_driver(&xxx_usart_driver);
	...
	ret = platform_driver_register(&xxx_serial_driver);
	...
}

static void __exit usart_exit(void)
{
	platform_driver_unregister(&xxx_serial_driver);
	uart_unregister_driver(&xxx_usart_driver);
}

module_init(usart_init);
module_exit(usart_exit);
```
