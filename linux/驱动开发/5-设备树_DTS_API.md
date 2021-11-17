

# DTS API

[Linux设备树学习笔记(四、设备树常用 OF 操作函数)](https://blog.csdn.net/qq_35031421/article/details/105107629)

`#include <linux/of.h>`

## of_parse_phandle
```c++
/**
 * of_parse_phandle - Resolve a phandle property to a device_node pointer
 * @np: Pointer to device node holding phandle property
 * @phandle_name: Name of property holding a phandle value
 * @index: For properties holding a table of phandles, this is the index into
 *         the table
 *
 * Returns the device_node pointer with refcount incremented.  Use
 * of_node_put() on it when done.
 */
struct device_node *of_parse_phandle(const struct device_node *np,
				     const char *phandle_name, int index);

/*
* dts sample:
*		firmware {
*			compatible = "raspberrypi,bcm2835-firmware";
*			mboxes = <0x1b>;
*			phandle = <0x7>;
*		};
* using sample:
* 	struct device_node *fw_node = of_parse_phandle(pdev->dev->of_node, "firmware", 0);
*    int len;
*    struct property *pp=of_find_property(fw_node, "compatible",len);
*    pp->name=compatible  pp->value="raspberrypi,bcm2835-firmware"  len=
*/
```

## of_find_property, of_property_read_u32_index
```c++
/**
 * of_property_read_u32_index - Find and read a u32 from a multi-value property.
 *
 * @np:		device node from which the property value is to be read.
 * @propname:	name of the property to be searched.
 * @index:	index of the u32 in the list of values
 * @out_value:	pointer to return value, modified only if no error.
 *
 * Search for a property in a device node and read nth 32-bit value from
 * it. Returns 0 on success, -EINVAL if the property does not exist,
 * -ENODATA if property does not have a value, and -EOVERFLOW if the
 * property data isn't large enough.
 *
 * The out_value is modified only if a valid u32 value can be decoded.
 */
int of_property_read_u32_index(const struct device_node *np,
				       const char *propname,
				       u32 index, u32 *out_value)
/*
 *dts sample:
 *			uart0_ctsrts_gpio30 {
 *				brcm,pins = <0x1e 0x1f>;
 *				phandle = <0x54>;
 *				brcm,function = <0x7>;
 *			};
 *using sample:
 *	struct property pins = of_find_property(np, "brcm,pins", NULL);
 *    int 	num_pins = pins->length / 4;
 *	for (i = 0; i < num_pins; i++) {
 *        u32 pin;
 *		int err = of_property_read_u32_index(np, "brcm,pins", i, &pin);
 *        ...
 *    }
 */
```


## of_address_to_resource

of_address_to_resource 函数是从设备树里面提取资源值，但是本质上就是将 reg 属性值，然后将其转换为 resource 结构体类型

reg属性格式是`reg = <address1 length1 [address2 length2] [address3 length3] ... >`， 在下面例子中index=0，因此是获取`<address1 length1>`转换到resource里面去。

```c++
/**
 * of_address_to_resource - Translate device tree address and return as resource
 *
 * Note that if your address is a PIO address, the conversion will fail if
 * the physical address can't be internally converted to an IO token with
 * pci_address_to_pio(), that is because it's either called too early or it
 * can't be matched to any host bridge IO space
 */
int of_address_to_resource(struct device_node *dev, int index,
			   struct resource *r)

    /* dts sample
    *	gpio: gpio@7e200000 {
    *		compatible = "brcm,bcm2835-gpio";
    *		gpio-controller;
    *		reg = <0x7e200000 0xb4>;
    *     ...
    */
   /*using sample:
	*struct device_node *np = pdev->dev->of_node;
	*struct bcm2835_pinctrl *pc;
    *
	*struct resource iomem;
	*int err = of_address_to_resource(np, 0, &iomem);
	*-----writeb is 8bit, writew is 16bit, writel is 32bit val, writeq is 64-bit quantities 
    *writel(val, base + reg);    
    */
```

上面演示了如何从DTS中得到某个地址的“struct resource”结构，那我们是否可以手工构造这个结构体呢？ 是可以的，因为linux支持设备信息不但可以通过DTS表达，也可以手工编码在kernel代码中，手工构造resource如下：
```c++
struct resource res = {
        .start = 0x7e200000,
        .end = 0x7e200000+0xb4-1,
        .flags = IORESOURCE_MEM,
    };
// 或
// struct resource res = DEFINE_RES_MEM(0x7e200000,0xb4),
};
```

### devm_ioremap_resource 与 devm_ioremap 区别

devm_ioremap 就是直接映射， devm_ioremap_resource 是先申请，后映射。

在`/proc/iomem`可以查看到已经申请并映射的device resource. 一旦已经申请、映射后的，在使用时直接devm_ioremap 就可以。
