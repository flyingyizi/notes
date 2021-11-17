

# 2.基本概念

内核下的`Documentation/CodingStyle` 描述了Linux 内核对编码风格的要求，内核下的`scripts/checkpatch.pl` 提供了1 个检查代码风格的脚本。

### 应用程序 与设备驱动

应用程序和 VFS 之间的接口是系统调用． 而VFS与文件系统以及设备文件之间的接口是自file_operations 结构体成员函数．这个结构体包含对文件进行打开、关闭、读写、控制的一系列成员函数．
```text
          系统调用            file_opertion
应用程序 -------------> VFS -------------------> 设备驱动 -------->  硬件
```

###  devfs vs  udev

 注意devfs已经在3.26以后的内核被废弃， udev取代了devfs。udev的目标是想解决上面提到的这些问题，他通采用用户空间(user-space)工具来管理/dev/目录树，他和文件系统分开。

mdev是udev的简化版本,是busybox中所带的程序,最适合用在嵌入式系统,而udev一般用在PC上.它们都是用户空间程序，两者实现原理不一样：

- udev: udev 是基于netlink 机制的，它在系统启动时运行了一个deamon 程序udevd，通过监听内核发送的uevent 来执行相应的热拔插动作，包括创建/删除设备节点，加载/卸载驱动模块等等。
- mdev：mdev是基于uevent_helper 机制的，它在系统启动时修改了内核中的uevnet_helper 变量（通过写/proc/sys/kernel/hotplug），值为“/sbin/mdev”。这样内核产生uevent 时会调用uevent_helper 所指的用户级程序，也就是mdev，来执行相应的热拔插动作



使用devfs， 驱动在设备初始化时在`/dev`目录下创建设备文件，显然如果采用devfs，当打开一个并不存在的`/dev/节点`，devfs会自动加载对应的驱动。：
```c++
/* 创建设备目录*/
devfs_handle_t devfs_mk_dir(devfs_handle_t dir, const char *name, void *info);
/* 创建设备文件*/
devfs_handle_t devfs_register(devfs_handle_t dir, const char *name, unsigned int flags,
                              unsigned int major, unsigned int minor, umde_t mode,
                              void * ops, void *info);
/* 撤销设备文件*/
void devfs_unregister(devfs_handle_t de);
```

相反，udev思想是应该在设备被发现时加载驱动模块。系统中所有的设备都应该产生热插拔事件并加载恰当的驱动．而udev 能注意到这点并且为它创建对应的的设备节点。
udev 完全在用户态工作，利用设备加入或移除时内核所发送的热插拔 netlink事件来工作。
```shell
$ ps -aux | grep udev
root         431  0.0  0.0  24168  7684 ?        Ss   8月30   0:00 /lib/systemd/systemd-udevd
```
对于在udev启动前，开机时已经存在的冷插拔设备，如果没有特别处理，udev就会漏掉它们的事件。 为解决这个我问题。linux内核提供了sysfs下面的一个uevent节点（例如 `/sys/module/psmouse/uevent`）,在该节点中增加"add",会导致内核重新发送冷插拔的netlink事件.


udev的配置文件位于 `/etc/udev/ 和 /lib/udev/ `. udev的规则文件一般位于 `/lib/udev/rules.d/`，也可以位于  `/etc/udev/rules.d/`。规则文件是按照字母顺序处理的，不管放在哪个目录。对于相同名字的规则文件， /etc/udev/rules.d 比  /lib/udev/rules.d 优先。

规则文件里的规则有一系列的键/值对组成，键/值对之间用逗号(,)分割。每一个键或者是用户匹配键，或者是一个赋值键。这些值将影响udev创建的设备文件。
- 匹配键："`!=`,`==`"匹配键确定规则是否被应用
- 赋值键："`=`,`+=`,`:=`"赋值键表示分配某值给该键

```
...
ACTION!="add|change|move", GOTO="mm_candidate_end"
SUBSYSTEM=="tty", ENV{ID_MM_CANDIDATE}="1"
SUBSYSTEM=="net", ENV{ID_MM_CANDIDATE}="1"
KERNEL=="cdc-wdm*", SUBSYSTEM=="usb", ENV{ID_MM_CANDIDATE}="1"
KERNEL=="cdc-wdm*", SUBSYSTEM=="usbmisc", ENV{ID_MM_CANDIDATE}="1"
...
```
### uevent 
Uevent提供了“用户空间通知”的功能实现，通过该功能，当内核中有Kobject的增加、删除、修改等动作时，会通知用户空间。

Uevent是Kobject的一部分，用于在Kobject状态发生改变时，例如增加、移除等，通知用户空间程序。

同时，uevent机制也可用于kernel和应用之间的通讯，比如一个红外或senser的驱动里，把红外电平或senser的方向发到应用层，那么在driver里如何发送一个uevent呢？很简单，示例如下：
```c++
static void send_uevent(struct device *dev ，unsigned long code)
{
char event_string[128];
char *envp[] = { event_string, NULL }; 
//printk("----uevent send \n");
sprintf(event_string, "EVENT=%ld",code); 
kobject_uevent_env(&dev->kobj, KOBJ_CHANGE, envp);

}
```

### sysfs与procfs

内核的结构化设备模型在用户空间就称为sysfs 。与procfs类似。sysfs 虚拟文件系统提供了一种比 proc 更为理想的访问内核数据的途径。新设计的内核机制应该尽量使用 sysfs 机制，而将 proc 保留给纯净的“进程文件系统”。

sysfs 文件系统总是被挂载在 /sys 挂载点上。

[使用 /sys 文件系统访问 Linux 内核](https://blog.csdn.net/halazi100/article/details/39961467)



### DTS （device tree source）

document： `/usr/src/linux-source/Documentation/devicetree`

## 设备模型

需要关心： 总线/设备/驱动这三个实体． 总线将设备和驱动绑定。在系统每注册一个设备的时候． 会寻找与之匹配的驱动． 相反的，在系统每位注册一个驱动的时候． 会寻找与之匹配的设备． 而匹配由总线完成。

换句话说，在Linux 内核中． 设备和驱动是分开注册的. 注册1个设备的时候． 并不需要驱动已经存在．而l个驱动被注册的时候． 也不需要对应的设备设备已经被注册。设备和驱动各自涌向内核；而每个设备和驱动涌入内核的时候，都会去寻找自己的另一半．而正是bus_type 的match（）成员函数将两者捆绑在一起。简单地说．设备和驱动就是红尘中漂浮的男女，而bus_type的match（）则是牵引红线的月老．它可以识别什么设备与什么驱动是可以配对的。一旦配对成功，xxx_driver的probe()就会被执行, xxx是总线名，例如platform,pci,i2c,spi,usb...

内核提供了device_register和driver_register两个接口，供各个driver模块使用。而这两个接口的核心逻辑，是通过bus模块的bus_add_device和bus_add_driver实现的，下面我们看看这两个接口的处理逻辑。通常我们不直接使用device_register和driver_register两个接口，而是使用其他更易于使用的封装，例如：

- 对platform设备，提供了"`platform_device_add`"，它的调用关系简单来说就是："`platform_add_devices-->platform_device_register-->platform_device_add-->device_add->bus_add_device`"
- 对platform驱动，提供了"`platform_driver_register`"或宏"`module_platform_driver`"，它的调用关系简单来说就是："`platform_driver_register-->driver_register-->bus_add_driver`"

### 例子：设备与platform总线
在platform_device_add中，会明确将该设备的总线指定为platform_bus_type这个全局var，在后面的bus_add_device会将该设备添加到这个全局var中去
```c++
int platform_device_add(struct platform_device *pdev)
{
	...
	if (!pdev->dev.parent)
		pdev->dev.parent = &platform_bus;

	pdev->dev.bus = &platform_bus_type;
	...
	ret = device_add(&pdev->dev);
    ...
};
```

```c++
struct bus_type {
    ...
    /**
	 * 这个结构就是集合了一些bus模块需要使用的私有数据
	 * 其中例如klist_devices和klist_drivers两个链表，分别保存了本bus下所有的device和device_driver的指针。
	*/
    struct subsys_private *p;
    struct lock_class_key lock_key;
 };
```
在bus_add_device处理比较多，例如的处理逻辑：

- 调用内部的device_add_attrs接口，将由bus->dev_attrs指针定义的默认attribute添加到内核中，它们会体现在/sys/devices/xxx/xxx_device/目录中
- 调用sysfs_create_link接口，将该device在sysfs中的目录，链接到该bus的devices目录下，例如：
   ```text
	xxx# ls /sys/bus/spi/devices/spi1.0 -l                                                        
	lrwxrwxrwx root     root              2014-04-11 10:46 spi1.0 -> ../../../devices/platform/s3c64xx-spi.1/spi_master/spi1/spi1.0
	其中/sys/devices/…/spi1.0，为该device在sysfs中真正的位置，而为了方便管理，内核在该设备所在的bus的xxx_bus/devices目录中，创建了一个符号链接
   ```
- 调用sysfs_create_link接口，在该设备的sysfs目录中（如/sys/devices/platform/alarm/）中，创建一个指向该设备所在bus目录的链接，取名为subsystem，例如：
   ```text
	xxx # ls /sys/devices/platform/alarm/subsystem -l                                                
	lrwxrwxrwx root     root              2014-04-11 10:28 subsystem -> ../../../bus/platform
   ```	

- 最后，把该设备指针保存在"`bus->p->klist_devices`"中

### 例子：驱动与platform总线

在platform_driver_register中，会明确将该驱动的总线指定为platform_bus_type这个全局var，在后面的bus_add_device会将该设备添加到这个全局var中去
```c++
#define platform_driver_register(drv) \
	__platform_driver_register(drv, THIS_MODULE)
int __platform_driver_register(struct platform_driver *drv,
				struct module *owner)
{
	drv->driver.owner = owner;
	drv->driver.bus = &platform_bus_type;
	drv->driver.probe = platform_drv_probe;
	drv->driver.remove = platform_drv_remove;
	drv->driver.shutdown = platform_drv_shutdown;

	return driver_register(&drv->driver);
}
```
bus_add_driver的处理核心逻辑：

- 为该driver的struct driver_private指针（priv）分配空间，并初始化其中的priv->klist_devices、priv->driver、priv->kobj.kset等变量，同时将该驱动指针保存在"`bus->p->klist_drivers`"
- 将driver的kset（priv->kobj.kset）设置为bus的drivers kset（bus->p->drivers_kset），这就意味着所有driver的kobject都位于bus->p->drivers_kset之下（寄/sys/bus/xxx/drivers目录下）
- 以driver的名字为参数，调用kobject_init_and_add接口，在sysfs中注册driver的kobject，体现在/sys/bus/xxx/drivers/目录下，如/sys/bus/spi/drivers/spidev
- 将该driver保存在bus的klist_drivers链表中，并根据drivers_autoprobe的值，选择是否调用driver_attach进行probe
- 调用driver_create_file接口，在sysfs的该driver的目录下，创建uevent attribute
- 调用driver_add_attrs接口，在sysfs的该driver的目录下，创建由bus->drv_attrs指针定义的默认attribute
- 同时根据suppress_bind_attrs标志，决定是否在sysfs的该driver的目录下，创建bind和unbind attribute（具体可参考"Linux设备模型(5)_device和device driver”中的介绍）





## 字符设备： 顺序存取设备数据

比如时钟/磁带，打印件, 没有和任何物理硬件相连接但需要顺序访问的伪字符设备...

```shell
# c表示字符设备驱动程序， b表示块设备驱动程序， l表示符号链接
$ ls -lh  /dev
total 0
crw-r--r--  1 root root     10, 235 8月  30 09:17 autofs
drwxr-xr-x  2 root root         480 8月  30 09:18 block
...
```

字符驱动程序并不仅仅在`drivers/char/`目录下 。下面是一些“超级”字符驱动程序的例子，它们受到特别的对待，其目录位置也很特殊。
- 串行驱动程序是管理计算机串行端口的字符驱动程序． 然而，它们不仅仅是简单的字符驱动程序，因此被放在`drivers/serial/` 目录下．
- 输入驱动程序负责像键盘、鼠标和操纵杆这样的设备。它们位于单独的源文件目录：`drivers/input/`。
- 帧缓冲区(`/dev/fb*`)提供对显存的访问， `/dev/mem` 提供对系统内存的访问途径。
- －些设备类支持少量采用字符接口的硬件。例如SCSI设备一般是块设备，但SCSI磁带是字符设备。
- 一些内核层提供钩子，通过导出相应的字符接口实现用户空间的设备驱动程序。例如  `drivers/scsi/sg.c, i2c-dev`

关键数据结构: `struct cdev`, struct file_operations

linux内核操作`struct cdev`的函数：
```c++
/* 初始化cdev成员，并建立cdev与file_operations之间的连接*/
void cdev_init(struct cdev *, struct file_operations *);
struct cdev *cdev_alloc();
void cdev_put(struct cdev *p);
/* 向系统添加/删除一个cdev，完成字符设备的注册/注销*/
int cdev_add(struct cdev*, dev_t, unsigned);
void cdev_del(struct cdev*);
```
注意，在调用`cdev_add`前，应首先调用`register_chrdev_region or alloc_chrdev_region`向系统申请设备号。
注意，在调用`cdev_del`后，应调用`unregister_chrdev_region`向系统释放原来申请的设备号。

下面的例子，演示了一个简单的字符设备驱动加载/卸载
```c++
static int major;		/* default to dynamic major */
module_param(major, int, 0);
MODULE_PARM_DESC(major, "Major device number");

struct xxx_dev_t {
	struct cdev  cdev;
	...
} 
static xxx_dev_t xxx_dev;

dev_t devid;
static int xxx_init(void){
    ...
	if (major) {
		devid = MKDEV(major, 0);
		rc = register_chrdev_region(devid, 1, DEVNAME);
	} else {
		rc = alloc_chrdev_region(&devid, 0, 1, DEVNAME);
		major = MAJOR(devid);
	}

	/* ignore minor errs, and succeed */
	cdev_init(&xxx_dev.cdev, &xxx_fileops);
	cdev_add(&xxx_dev.cdev, devid, 1);
    ...
};
static void xxx_exit(){
    unregister_chrdev_region(devid, 1);
    cdev_del(&xxx_dev.cdev);
}

module_init(xxx_init);
module_exit(xxx_exit);
```

## 模块调试

21

第6章讲解了两种设备： 一个PC衍生器件上的触摸控制器和一个手机上的UART 。第8章则讲解了PC系统中的EEPROM和嵌入式设备中的实时钟。

第21章讨论了设备驱动秤，序的调试技术，开·扯驱动程序的时候，提前阅读该章会很有用。

[USB厂家ID](http://www.linux-usb.org/usb.ids)

## 操作系统是如何知道硬件参数的

[发现新硬件的前前后后](https://www.docin.com/p-1291428941.html)

作者：子者阳生之初
链接：https://www.zhihu.com/question/389774365/answer/1578725483
来源：知乎
著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。

IBM、Sun等厂家的服务器最初都在硬件设备中嵌入了相应的程序，称为固件(Firmware)，用于初始化系统配置，提供操作系统软件和硬件之间的接口，启动和运行系统。
后来为了标准化和兼容性，IBM、Sun等联合推出了固件接口IEEE 1275标准，让他们的服务器如IBM PowerPC pSeries，Apple PowerPC，Sun SPARC等均采用Open Firmware，在运行时构建系统硬件的设备树信息传递给内核，进行系统的启动运行。

这样做的好处有，减少内核对系统硬件的严重依赖，利于加速支持包的开发，降低硬件带来的变化需求和成本，降低对内核设计和编译的要求。随着Linux内核的发展，在内核代码中引入了Open Firmware API以使用标准固件接口。在嵌入式PowerPC中，一般使用U-Boot之类的系统引导代码，而不采用Open Firmware。

早期的U-Boot使用include/asm-ppc/u-boot.h中的静态数据结构struct bd_t将板子基本信息传递给内核。这样的接口不够灵活，硬件发生变化就需要重新定制编译烧写引导代码和内核，而且也不再适应于现在的内核。为了适应内核的发展及嵌入式PowerPC平台的千变万化，吸收标准Open Firmware的优点，U-Boot引入了扁平设备树FDT这样的动态接口，使用一个单独的FDT blob存储传递给内核的参数。一些确定信息，例如cache大小、中断路由等直接由设备树提供，而其他的信息，例如eTSEC的MAC地址、频率、PCI总线数目等由U-Boot在运行时修改。从上述描述来看，首先要了解设备树的相关知识。在Linux 2.6中，ARM架构的板极硬件细节过多地被硬编码在arch/arm/plat-xxx和arch/arm/mach-xxx，比如板上的platform设备、 resource、i2c_board_info、spi_board_info以及各种硬件的platform_data，这些板级细节代码对内核来讲只不过是垃圾代码。 而采用Device Tree后，许多硬件的细节可以直接透过它传递给Linux，而不再需要在kernel中进行大量的冗余编码。……（后来）ARM SOC board specific的代码被移除，由DeviceTree机制来负责传递硬件拓扑和硬件资源信息。本质上，Device Tree改变了原来用hardcode方式将HW 配置信息嵌入到内核代码的方法，改用bootloader传递一个DB的形式。

如果我们认为kernel是一个black box，那么其输入参数应该包括：

- 1. 识别platform的信息
- 2. runtime的配置参数
- 3. 设备的拓扑结构以及特性对于嵌入式系统，在系统启动阶段，bootloader会加载内核并将控制权转交给内核，此外， 还需要把上述的三个参数信息传递给kernel，以便kernel可以有较大的灵活性。
 
在linux kernel中，Device Tree的设计目标就是如此。……Device Tree由一系列被命名的结点（node）和属性（property）组成，而结点本身可包含子结点。所谓属性， 其实就是成对出现的name和value。在Device Tree中，可描述的信息包括（原先这些信息大多被hard code到kernel中）：

- o CPU的数量和类别
- o 内存基地址和大小
- o 总线和桥o 外设连接
- o 中断控制器和中断使用情况
- o GPIO控制器和GPIO使用情况
- o Clock控制器和Clock使用情况
 
它基本上就是画一棵电路板上CPU、总线、设备组成的树，Bootloader会将这棵树传递给内核，然后内核可以识别这棵树， 并根据它展开出Linux内核中的platform_device、i2c_client、spi_device等设备，而这些设备用到的内存、IRQ等资源， 也被传递给了内核，内核会将这些资源绑定给展开的相应的设备。是否Device Tree要描述系统中的所有硬件信息？答案是否定的。基本上，那些可以动态探测到的设备是不需要描述的， 例如USB device。不过对于SOC上的usb hostcontroller，它是无法动态识别的，需要在device tree中描述。同样的道理， 在computersystem中，PCI device可以被动态探测到，不需要在device tree中描述，但是PCI bridge如果不能被探测，那么就需要描述之。看来，操作系统不是通过和硬件通信获得硬件参数，而是直接被写好读取的。同时也有一些信息是可以动态获取的。用于实现驱动代码与设备信息相分离。在设备树出现以前，所有关于设备的具体信息都要写在驱动里，一旦外围设备变化，驱动代码就要重写。引入了设备树之后，驱动代码只负责处理驱动的逻辑，而关于设备的具体信息存放到设备树文件中。固件的任务BIOS和UEFI的最主要的功能：初始化硬件和提供硬件的软件抽象。ARM体系也要初始化具体主板相关硬件如GPIO和内存等，这些一般在BSP中完成。与X86体系不同之处在于这些硬件完全定制化，初始化的时候就预先知道有哪些设备，Solder Down了哪个品牌的哪种内存颗粒，到时候就照方抓药，初始化一大堆寄存器而已。X86系统配置情况在开机时候是不知道的，需要探测（Probe）、Training(内存和PCIe)和枚举（PCIe等等即插即用设备），相对较复杂。BIOS和UEFI提供了整个主板、包括主板上外插的设备的软件抽象。通过探测、Training和枚举，BIOS就有了系统所有硬件的信息。它通过几组详细定义好的接口，把这些信息抽象后传递给操作系统，这些信息包括SMBIOS（专栏稍后介绍）、ACPI表（ACPI与UEFI），内存映射表（E820或者UEFI运行时）等等。通过这层映射，才能做到做到操作系统完全不改而能够适配到所有机型和硬件。x86从 start_kernel() 到 PID 1https://zhuanlan.zhihu.com/p/34511698内核的硬件清单：设备树和 ACPI 表在引导时，内核需要硬件信息，不仅仅是已编译过的处理器类型。代码中的指令通过单独存储的配置数据进行扩充。有两种主要的数据存储方法：a href="https://www.youtube.com/watch?v=m_NyYEBxfn8"> 设备树(device-tree) 和 高级配置和电源接口（ACPI）表。内核通过读取这些文件了解每次启动时需要运行的硬件。对于嵌入式设备，设备树是已安装硬件的清单。设备树只是一个与内核源代码同时编译的文件，通常与 vmlinux 一样位于 /boot目录中。要查看 ARM 设备上的设备树的内容，只需对名称与 /boot/*.dtb 匹配的文件执行 binutils 包中的 strings 命令即可，这里 dtb 是指 设备树二进制文件(device-tree binary)。显然，只需编辑构成它的类 JSON 的文件并重新运行随内核源代码提供的特殊 dtc 编译器即可修改设备树。虽然设备树是一个静态文件，其文件路径通常由命令行引导程序传递给内核，但近年来增加了一个 设备树覆盖 的功能，内核在启动后可以动态加载热插拔的附加设备。x86 系列和许多企业级的 ARM64 设备使用 ACPI 机制。与设备树不同的是，ACPI 信息存储在内核在启动时通过访问板载 ROM 而创建的 /sys/firmware/acpi/tables 虚拟文件系统中。读取 ACPI 表的简单方法是使用 acpica-tools 包中的 acpidump 命令。

## /sys目录下各个子目录的具体说明

### /sys/devices

该目录下是全局设备结构体系，包含所有被发现的注册在各种总线上的各种物理设备。一般来说，所有的物理设备都按其在总线上的拓扑结构来显示，但有两个例外，即platform devices和system devices。platform devices一般是挂在芯片内部的高速或者低速总线上的各种控制器和外设，它们能被CPU直接寻址；system devices不是外设，而是芯片内部的核心结构，比如CPU，timer等，它们一般没有相关的驱动，但是会有一些体系结构相关的代码来配置它们。

(sys/devices是内核对系统中所有设备的分层次表达模型，也是/sys文件系统管理设备的最重要的目录结构)


### sys/dev

该目录下维护一个按照字符设备和块设备的主次号码(major:minor)链接到真是设备(/sys/devices)的符号链接文件。


### /sys/class

该目录下包含所有注册在kernel里面的设备类型，这是按照设备功能分类的设备模型，每个设备类型表达具有一种功能的设备。每个设备类型子目录下都是这种哦哦那个设备类型的各种具体设备的符号链接，这些链接指向/sys/devices/name下的具体设备。设备类型和设备并没有一一对应的关系，一个物理设备可能具备多种设备类型；一个设备类型只表达具有一种功能的设备，比如：系统所有输入设备都会出现在/sys/class/input之下，而不论它们是以何种总线连接到系统的。(/sys/class也是构成linux统一设备模型的一部分)




### /sys/block

该目录下的所有子目录代表着系统中当前被发现的所有块设备。按照功能来说防止在/sys/class下会更合适，但由于历史遗留因素而一直存在于/sys/block，但从linux2.6.22内核开始这部分就已经标记为过去时，只有打开了CONFIG_SYSFS_DEPRECATED配置编译才会有这个目录存在，并且其中的内容在从linux2.6.26版本开始已经正式移到了/sys/class/block，旧的接口/sys/block为了向后兼容而保留存在，但其中的内容已经变为了指向它们在/sys/devices/中真实设备的符号链接文件。





### /sys/bus

该目录下的每个子目录都是kernel支持并且已经注册了的总线类型。这是内核设备按照总线类型分层放置的目录结构，/sys/devices中的所有设备都是连接于某种总线之下的，bus子目录下的每种具体总线之下可以找到每个具体设备的符号链接，

一般来说每个子目录(总线类型)下包含两个子目录，一个是devices，另一个是drivers；其中devices下是这个总线类型下的所有设备，这些设备都是符号链接，它们分别指向真正的设备(/sys/devices/name/下)；而drivers下是所有注册在这个总线上的驱动，每个driver子目录下 是一些可以观察和修改的driver参数。

(它也是构成linux统一设备模型的一部分)

### /sys/fs

按照设计，该目录使用来描述系统中所有的文件系统，包括文件系统本身和按照文件系统分类存放的已挂载点。

### /sys/kernel

这个目录下存放的是内核中所有可调整的参数

### /sys/firmware

该目录下包含对固件对象(firmware object)和属性进行操作和观察的接口，即这里是系统加载固件机制的对用户空间的接口.(关于固件有专用于固件加载的一套API)

### /sys/hypervisor

该目录是与虚拟化Xen相关的装置。(Xen是一个开放源代码的虚拟机监视器)


### /sys/module

该目录下有系统中所有的模块信息，不论这些模块是以内联(inlined)方式编译到内核映像文件中还是编译为外模块(.ko文件)，都可能出现在/sys/module中。即module目录下包含了所有的被载入kernel的模块。


### /sys/power

该目录是系统中的电源选项，对正在使用的power子系统的描述。这个目录下有几个属性文件可以用于控制整个机器的电源状态，如可以向其中写入控制命令让机器关机/重启等等。


