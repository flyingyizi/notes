

# 设备树overlay

## A.1.静态设置 

静态overlay指分include文件编写DTS

### 规则1：对于同一个节点的设置情况，可以覆盖

    对类似下面，不同的内容叠加，相同的属性以dts中为准。
    ```c++
    //file :xxxx.dts
    #include "xxxx.dtsi"
    ...
    ```

    但假如`#include `在最后，那就是：不同的内容叠加，相同的属性以dtsi中为准。

    显然这和我们在C中的include经验是一致的。

    下面的例子演示了覆盖dtsi的场景 .[例子图片版](https://img-blog.csdnimg.cn/20200301215648492.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3lpa3VuYmFpNTcwOA==,size_16,color_FFFFFF,t_70)：

    xx.dtsi 内容：
    ```c++
    /{
    compatible = "ti,am33xx";
    ocp {
        uart0 : serial@44e09000 {
            compatible = "ti,omap3-uart";
            reg = <0x44e09000 0x2000>;
            interrupts = <72>;
            status = "disabled";
        }
    };
    };
    ```

    xx.dts 内容：
    ```c++
    #include "xx.dtsi"
    /{
    compatible = "ti,am335x-bone","ti,am33xx";
    ocp {
        uart0 : serial@44e09000 {
            pinctrl-name = "default";
            pinctrl-0 = <&uart0_pins>;
            status = "disabled";
        }
    };
    };
    ```
    最终 xx.dts 内容将会是：
    ```c++
    /{
    compatible = "ti,am335x-bone","ti,am33xx";
    ocp {
        uart0 : serial@44e09000 {
            compatible = "ti,omap3-uart";
            reg = <0x44e09000 0x2000>;
            interrupts = <72>;

            pinctrl-name = "default";
            pinctrl-0 = <&uart0_pins>;
            status = "disabled";
        };
    };
    };
    ```



### 规则2：对于节点的修改，先引用后修改；例如原生节点定义如下：

    ```c++
    reserved_memory : reserved-memory {
    #address-cells = <2>;
    #size-cells = <2>;
    ranges;
    hyp_mem : hyp_region@45700000 {
        compatible = "removed-dma-pool";
        no-map;
        reg = <0x0 0x45700000 0x0 0x600000>;
    };
    };
    ```
    需要在reserved-memory节点中添加一个新的节点或者直接修改reserved-memory节点的属性，都需要先引用reserved_memory节点（注意节点的引用名与节点名可以不一致）.如下面案例中，引用reserved-memory节点，并删除了ranges属性，删除了hyp_mem节点，新增了kboot_uboot_logmem节点；
    ```c++
    @reserved_memory {
    /delete-property/ ranges;
    /delete-property/ hyp_mem;
    kboot_uboot_logmem : kboot_uboot_logmem@0x5FC00000 {
        ....
    };
    };
    ```

## A.2.动态设置

ref：linux\Documentation\devicetree\overlay-notes.txt

官方overlay例子： linux\arch\arm\boot\dts\overlays， 或 https://github.com/raspberrypi/linux/tree/rpi-5.4.y/arch/arm/boot/dts/overlays

### Overlay DTS Format

The DTS of an overlay should have the following format:
```c++
{
	/* ignored properties by the overlay */

	fragment@0 {	/* first child node */

		target=<phandle>;	/* phandle target of the overlay */
	or
		target-path="/path";	/* target path of the overlay */

		__overlay__ {
			property-a;	/* add property-a to the target */
			node-a {	/* add to an existing, or create a node-a */
				...
			};
		};
	}
	fragment@1 {	/* second child node */
		...
	};
	/* more fragments follow */
}
```
Using the non-phandle based target method allows one to use a base DT which does
not contain a __symbols__ node, i.e. it was not compiled with the -@ option.
The __symbols__ node is only required for the target=<phandle> method, since it
contains the information required to map from a phandle to a tree location.

例子：
```c++
//----base: foo.dts -----------------------------------------------------------------
	/* FOO platform */
	/ {
		compatible = "corp,foo";

		/* shared resources */
		res: res {
		};

		/* On chip peripherals */
		ocp: ocp {
			/* peripherals that are always instantiated */
			peripheral1 { ... };
		}
	};
//---- foo.dts -----------------------------------------------------------------

//The overlay bar.dts, when loaded (and resolved as described in [1]) should

//----overlay: bar.dts -----------------------------------------------------------------
/plugin/;	/* allow undefined label references and record them */
/ {
	....	/* various properties for loader use; i.e. part id etc. */
	fragment@0 {
		target = <&ocp>;
		__overlay__ {
			/* bar peripheral */
			bar {
				compatible = "corp,bar";
				... /* various properties and child nodes */
			}
		};
	};
};
//---- bar.dts -----------------------------------------------------------------

//----result: foo+bar.dts -------------------------------------------------------------
	/* FOO platform + bar peripheral */
	/ {
		compatible = "corp,foo";

		/* shared resources */
		res: res {
		};

		/* On chip peripherals */
		ocp: ocp {
			/* peripherals that are always instantiated */
			peripheral1 { ... };

			/* bar peripheral */
			bar {
				compatible = "corp,bar";
				... /* various properties and child nodes */
			}
		}
	};
//---- foo+bar.dts -------------------------------------------------------------

As a result of the overlay, a new device node (bar) has been created
so a bar platform device will be registered and if a matching device driver
is loaded the device will be created as expected.
```

```shell
# generate the system base dtb
dtc -I fs -O dtb -o base.dtb /proc/device-tree
# generate overlay dtbo and apply the overlay
dtc -I dts bar.dts -o bar.dtbo
dtoverlay  bar.dtbo
```


### kernel支持原理

对DTS overlay是通过linux configfs进行支持的，使用的核心API是：
overlays 的调用位置
```c++
drivers/of/overlay.c 核心代码。
// Create and apply an overlay
int of_overlay_create(struct device_node *tree);
// Removes an overlay
int of_overlay_destroy(int id);
// Removes all overlays from the system
int of_overlay_destroy_all(void);
```
`of_overlay_create` 被 drivers/of/configfs.c 使用。
在`configfs.c` 最后一行是 `late_initcall(of_cfs_init)`. 作用是标记 of_cfs_init 加入到 内核 .init 段。

"`.init`" 段被调用位置是：`start_kernel -->rest_init() -->kernel_init()-->kernel_init() --> kernel_init_freeable() -->do_basic_setup() -->do_initcalls()`. kernel_init 在完成一系列初始化之后启动第一个用户进程。内核启动过程就结束了。

调用 of_cfs_init 会在 /sys/kernel/config/ 目录下创建 /sys/kernel/config/device-tree/overlay 文件(内存文件系统)。

详细分析过程见[【Linux configfs机制】](https://blog.csdn.net/liujiliei/article/details/105276551)

Device Tree Overlays 核心定义：
在 kernel 启动以后系统加载时候修改或者增加部分dts，最终把整个系统需要的设备驱动全部加载进去。




## A.3.手工使用 device tree overlays

### 直接操作
通过外文网站获取到一些内容：
device tree overlays 的实际用法是，系统启动后 root 用户修改dtb文件，不需要重启！即可生效。
在 /sys/kernel/config/device-tree/overlays/ 目录下创建目录，创建完成后目录内自动会有三个文件 dtbo path status
直接复制 已经编译好的 *.dtbo 文件覆盖 dtbo 文件.
并对 status 赋值 1 即可(好像是不需要的，cp文件覆盖直接生效,如果 status 是只读文件 获取当前 dtbo 是否OK)。
```shell
root@npi:/sys/kernel/config/device-tree/overlays# mkdir test
root@npi:/sys/kernel/config/device-tree/overlays# cd test
root@npi:/sys/kernel/config/device-tree/overlays/test# ls
root@npi:/sys/kernel/config/device-tree/overlays/test# dtbo path status
root@npi:/sys/kernel/config/device-tree/overlays/test# cat status
root@npi:/sys/kernel/config/device-tree/overlays/test# unapplied
root@npi:/sys/kernel/config/device-tree/overlays/test# cp /lib/firmware/test.dtbo dtbo
root@npi:/sys/kernel/config/device-tree/overlays/test# cat status
root@npi:/sys/kernel/config/device-tree/overlays/test# applied
```
加载完成后，dtbo 内的设备会自动由系统安装。可以在 /dev 看到具体内容。

### 工具操作
参考树莓派的使用方式：
https://www.raspberrypi.org/documentation/configuration/device-tree.md

dtdiff dtoverlay dtparam 应该是一组工具。 这里不再描述。

### 工具操作：dtoverlay

loads and removes overlays while the system is running

[dtoverlay source](https://github.com/raspberrypi/userland)

### 工具操作：dtoverlay

