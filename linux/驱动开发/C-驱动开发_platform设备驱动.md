

# 3.platform 设备驱动

在Linux 2.6 以后的设备驱动模型中，需要关心： 总线/设备/驱动这三个实体． 总线将设备和驱动绑定。在系统每注册一个设备的时候． 会寻找与之匹配的驱动． 相反的，在系统每位注册一个驱动的时候． 会寻找与之匹配的设备． 而匹配由总线完成。

但是在SOC系统中，通常外设都是挂接在SOC内存空间，它们本身并不依附于类似PCI/USB/SPI/I2C..., 基于此，linux发明了一种虚拟的总线，称为platform总线，相应设备称为platform_device, 而驱动称为platform_driver.

“platform”这个概念专门为嵌入式引入。 注意所谓platform_device 并不是于字符设备/块设备/网络设备...等并列的概念。而只是一种附加的手段。

与platform_driver 地位对等的是： i2c_driver，spi_driver, usb_driver, pci_driver,...

通过platform_bus_type的定义可以知道， 设备和驱动的匹配是由platform_match完成的。
```c++
struct bus_type platform_bus_type = {
	.name		= "platform",
	.dev_groups	= platform_dev_groups,
	.match		= platform_match,
	.uevent		= platform_uevent,
	.dma_configure	= platform_dma_configure,
	.pm		= &platform_dev_pm_ops,
};
EXPORT_SYMBOL_GPL(platform_bus_type);
```
这个platform_match匹配有[四种方式](https://www.cnblogs.com/zhuangquan/p/12877244.html)：
```c++
static int platform_match(struct device *dev, struct device_driver *drv)
{
	...
	/* Attempt an OF style match first */
	if (of_driver_match_device(dev, drv))
		return 1;

	/* Then try ACPI style match */
	if (acpi_driver_match_device(dev, drv))
		return 1;

	/* Then try to match against the id table */
	if (pdrv->id_table)
		return platform_match_id(pdrv->id_table, pdev) != NULL;

	/* fall-back to driver name match */
	return (strcmp(pdev->name, drv->name) == 0);
}
```

为了不同的匹配支持，下面演示了怎么构建platform_driver：
```c++
static struct platform_driver  xxxx_driver = {
	.driver = {
		.name = ???,
		.owner = THIS_MODULE,
		/* to support OF style match*/
		.of_match_table = of_match_ptr(xxxx_of_matches),
		/* to support acpi style match*/
		// .acpi_match_table = ??
	},
	/* to support id table style match*/
	// .id_table = ??,
	.probe  = ???,
	.remove = ???
}
```



## 提供platform驱动
将一个驱动更改为platform驱动，在“环境准备”中提供了一个例子，这里有点需要注意的是，因为有了“struct platform_device *pdev”输入，所以建议做设备关联的malloc， 如下面：
更改的核心就是更改注册，同时有个小改动就是将
```c++
struct xxx_dev_t {
	struct cdev  cdev;
	...
} *xxx_devp;

static int xxx_init(struct platform_device *pdev){
	xxx_devp = devm_kzalloc(&pdev-dev, sizeof(xxx_dev_t));
    ...
};
// static void xxx_exit(void){
static int xxx_exit(struct platform_device *pdev){
	(VOID)pdev; // 不需要使用
	//改为操纵 xxx_devp->cdev
    ...
	return 0;
}
...
```

## 增加platform 设备

platform_device对象就是我们最终要注册到平台总线上的设备信息对象，对设备信息进行编码，其实就是创建一个platform_device对象，可以看出，platform_device和其他设备一样，都是device的子类。

要将一个设备让内核以platform设备对待，增加这个设备时，有两种方式，一种是将该设备在板文件中硬编码增加；一种是通过设备树提供必要的信息告知内核。

### 方式1：硬编码platform_device
在"arch/arm/mach-xxx/板文件"中硬编码增加。 这会使得在“`/sys/devices/platform`”目录下增加一个同名的子目录
```c++
static struct platform_device bast_device_ide1 = {
	.name		= "xxxxx",
	.id		= 1,	
	}
};

platform_add_devices(&bast_device_ide1,1);
```

### 方式2：DTS描述，kernel自动生成platform_device
在内核，是通过内核函数of_platform_default_populate_init, 遍历device_node树, 生成platform_device。这个函数对会满足以下条件的节点会处理为platform_device:
- b. 并非所有的device_node都会转换为platform_device只有以下的device_node会转换:
  - b.1 该节点必须含有compatible属性
  - b.2 根节点的子节点(节点必须含有compatible属性)
  - b.3 含有特殊compatible属性的节点的子节点(子节点必须含有compatible属性):
- 这些特殊的compatilbe属性为: “simple-bus”,“simple-mfd”,“isa”,"arm,amba-bus "
- 根节点是例外的，生成platfrom_device时，即使有compatible属性也不会处理

具体见“drivers/of/platform.c”中下面代码：
```c++
const struct of_device_id of_default_bus_match_table[] = {
	{ .compatible = "simple-bus", },
	{ .compatible = "simple-mfd", },
	{ .compatible = "isa", },
#ifdef CONFIG_ARM_AMBA
	{ .compatible = "arm,amba-bus", },
#endif /* CONFIG_ARM_AMBA */
	{} /* Empty terminated list */
};
```

对i2c/spi 总线设备，也会默认创建platorm 设备，具体见下面例子说明。
```c++
/{
   /* /mytest会被转换为platform_device,因为它兼容"simple-bus", 
      它的子节点/mytest/mytest@0 也会被转换为platform_device */	
   mytest{
       compatile = "mytest", "simple-bus";
	   mytest@0{
		   compatile = "mytest_0";
		   reg = <0>;
	   };
   };
   /* /i2c节点一般表示i2c控制器, 它会被转换为platform_device, 在内核中有对应的platform_driver;
      /i2c/at24c02节点不会被转换为platform_device, 它被如何处理完全由父节点的platform_driver决定, 
	  一般是被创建为一个i2c_client。*/
   i2c{
	   compatile = "samsung, i2c";
	   at24c02 {
		   compatile = "at24c02";
	   };
   };
   /* 类似的也有/spi节点, 它一般也是用来表示SPI控制器, 它会被转换为platform_device, 在内核中有对应的platform_driver;
   /spi/flash@0节点不会被转换为platform_device, 它被如何处理完全由父节点的platform_driver决定, 一般是被创建为一个spi_device。*/
   spi{
	   compatile = "samsung, spi";
	   flash@0 {
		   compatile = "winbond,w25q32dw";
		   reg = <0>;
	   };
   };
};
```

##  platform设备 与platform驱动的匹配

在前面章节中，已经描述了如何让内核将某个设备当作platform_device对待，以及怎么让一个驱动称为platform_driver。 在该前提下，我们下面描述platform设备与platform驱动的匹配。

在设备树中的“compatible”属性，通过它来与驱动关联。 

下面描述了设备驱动中如何使用“compatible”来与设备关联，也称为OF style match方法：

```c++
static const struct of_device_id  xxxx_of_matches[] = {
   {.compatible = "davicom,dm9000",},
   {}
};

static struct platform_driver  xxxx_driver = {
	.driver = {
		.name = ???,
		.owner = THIS_MODULE,
		/* to support OF style match*/
		.of_match_table = of_match_ptr(xxxx_of_matches),
	},
	.probe  = ???,
	.remove = ???
}
```

补充： 
- 如果使用DT，可以在probe中增加类似"`const struct of_device_id *match= of_match_device(xxxx_of_matches, &pdev->dev);`", 得到匹配的设备，从而对该匹配设备做写特殊的动作。另外不论是否采用了设备树，都可以在驱动中通过`platform_get_xx `去获取设备树里面的属性。
- 在驱动代码中，可以利用"`if (pdev->dev->of_node) {/*DT based instantiation*/} else {/*NON-DT based instantiation*/}`"识别做些动作。

