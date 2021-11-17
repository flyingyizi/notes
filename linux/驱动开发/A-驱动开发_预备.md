[The Linux Kernel documentation](https://www.kernel.org/doc/html/latest/index.html)


 1288  wget -c https://busybox.net/downloads/busybox-1.34.0.tar.bz2
 1289  ls
 1290  rm -rf busybox-1.24.0.tar.bz2  busybox-1.31.0  busybox-1.31.0.tar.bz2
 1291  ls
 1292  tar jxvf busybox-1.34.0.tar.bz2 
 1293  cd busybox-1.34.0/
 1294  make menuconfig
 1295  make install
 1296  uname -a
 1297  cd ..
 1298  ls
 1299  wget -c https://www.kernel.org/pub/linux/kernel/v4.x/linux-4.0.tar.gz
 1300  ls
 1301  tar zxvf linux-4.0.tar.gz 
 1302  ls
 1303  cd linux-4.0/
 1304  ls
 1305  cp   ../busybox-1.34.0/_install  .
 1306  cp -r  ../busybox-1.34.0/_install  .
 1307  ls
 1308  cd _install/
 1309  ls
 1310  mkdir etc
 1311  mkdir dev
 1312  mkdir mnt
 1313  mkdir -p etc/init.d/
 1314  touch etc/init.d/rcS
 1315  vi etc/init.d/rcS
 1316  chmod +x  etc/init.d/rcS
 1317  vi etc/fstab
 1318  vi etc/inittab
 1319  cd dev
 1320  ls
 1321  sudo mknod console c 5 1
 1322  sudo mknod null c 1 3
 1323  cd ../..
 1324  make vexpress_defconfig
 1325  make menuconfig
 1326  make bzImage -j4 ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
 1327  make bzImage -j4 
 1328  cd ..
 1329  ls

 1348  sudo apt-get install qemu
 1349  which qemu-system-arm
 1350  apt search qemu
 1351  locate qemu
 1352  cd
 1353  locate qemu-system-arm
 1354  apt search qemu-system
 1355  sudo apt-get install qemu-system-arm
 1356  ls
 1357  cd Downloads/qemu/linux-5.9/
 1358  ls

 1360  qemu-system-arm -M vexpress-a9 -smp 4 -m 1024M -kernel arch/arm/boot/zImage -append "rdinit=/linuxrc console=ttyAMA0 loglevel=8" -dtb arch/arm/boot/dts/vexpress-v2p-ca9.dtb -nographic




# 1.预备

## A.1.环境准备

```shell
# kernel-package: utility for building Linux kernel
sudo apt-get install build-essential  bison flex autoconf automake kernel-package libncurses5-dev libssl-dev
# download kernel source with version same as the system
$ uname -r
5.11.0-27-generic
$ apt-cache search linux-source
linux-source - Linux kernel source with Ubuntu patches
linux-source-5.4.0 - Linux kernel source for version 5.4.0 with Ubuntu patches
linux-gkeop-source-5.4.0 - Linux kernel source for version 5.4.0 with Ubuntu patches
linux-hwe-5.11-source-5.11.0 - Linux kernel source for version 5.11.0 with Ubuntu patches
linux-hwe-5.8-source-5.8.0 - Linux kernel source for version 5.8.0 with Ubuntu patches

# will generate /lib/modules/5.11.0-27-generic/build -> /usr/src/linux-headers-5.11.0-27-generic
$ sudo apt-get install linux-hwe-5.11-source-5.11.0

sudo make oldconfig
sudo make
# 生成和构建设备树 $make modules, $make modules_install两条指令
make modules
make modules_install
```

## A.2. 简单驱动实践

### sample1
测试程序: `~/Documents$ cat test.c`
```c++
//~/Documents$ cat test.c
// printk 打印内容通过dmesg 查看
#include <linux/init.h>
#include <linux/module.h>

static unsigned int panel_type = 6;


MODULE_LICENSE("GPL");
static int test_init(void) {
    printk(KERN_EMERG "hello: %d", panel_type);
    return 0;
}

static void test_exit(void) {
    printk(KERN_ALERT "goodbye");
}

module_init(test_init);
module_exit(test_exit);

// 演示使用module parameter， insmod test.ko panel_type=97
module_param(panel_type/*参数名*/, uint/*参数类型*/, S_IRUGO | S_IWUSR/*参数读写权限*/);
MODULE_PARM_DESC(panel_type, "Select the panel type: 37, 6, 97");

```

测试程序对应编译脚本： `~/Documents$ cat Makefile `
```makefile
# KERNELRELEASE是在内核源码的顶层Makefile中定义的一个变量，在第一次读取执行此Makefile时，KERNELRELEASE没有被定义，
# 所以make将读取执行else之后的内容，如果make的目标是clean，直接执行clean操作，然后结束。
# 当make的目标为all时，-C $(KDIR)指明跳转到内核源码目录下读取那里的Makefile；M=$(PWD) 表明然后
# 返回到当前目录继续读入、执行当前的Makefile。当从内核源码目录返回时，KERNELRELEASE已被定义，
# kbuild也被启动去解析kbuild语法的语句，make将继续读取else之前的内容。else之前的内容为kbuild语法
# 的语句，指明模块源码中各文件的依赖关系，以及要生成的目标模块名。param-objs := file1.o file2.o 
# 表示param.o由file1.o与file2.o 连接生成，obj-m := param.o表示编译连接后将生成param.o模块。


# 避免错误：module verification failed: signature and/or required key missing - tainting kernel
CONFIG_MODULE_SIG=n

ifneq ($(KERNELRELEASE),)

# obj-m表示把文件test.o作为"模块"进行编译，不会编译到内核，但是会生成一个独立的 "test.ko" 文件；
# obj-y表示把test.o文件编译进内核;
obj-m := test.o

else
PWD:=$(shell pwd)
KVER?=$(shell uname -r)
KDIR:=/lib/modules/$(KVER)/build

## Use make M=dir or set the environment variable KBUILD_EXTMOD to 
## specify the directory of external module to build. Setting M= takes precedence.

modules:
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers

endif
```


加载/卸载：
```shell
# or sudo modprobe test.ko
$ sudo insmod test.ko
$ lsmod |grep test
test                   16384  0
# query modle info in sysfs
$ tree  /sys/module/test/
/sys/module/test/
├── coresize
├── holders
├── initsize
├── initstate
├── notes
├── parameters
│   └── panel_type
├── refcnt
├── sections
│   ├── __mcount_loc
│   └── __param
├── srcversion
├── taint
└── uevent

# or sudo modprobe -r test
$ sudo rmmod test
$ lsmod |grep test
$
```

### sample2: sample1 + platform

补充：“linux\drivers\gpio\gpio-bcm-exp.c” 这个例子

封装为platform_driver. 加载后，将会在发现新增了“/sys/bus/platform/drivers/xxxxx”

```c++
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

static unsigned int panel_type = 6;


MODULE_LICENSE("GPL");
static int test_init(struct platform_device *pdev) {
	    printk(KERN_EMERG "hello: %d", panel_type);
	        return 0;
}


static int test_exit(struct platform_device *pdev) {
    printk(KERN_ALERT "goodbye");
    return 0;
}
module_param(panel_type/*参数名*/, uint/*参数类型*/, S_IRUGO | S_IWUSR/*参数读写权限*/);

//module_init(test_init);
//module_exit(test_exit);
static struct platform_driver xxx_driver={
		.driver = {
			.name = "xxxxx",
			.owner= THIS_MODULE,
			},
        .probe = test_init,
		.remove= test_exit,
};
module_platform_driver(xxx_driver);
// builtin_platform_driver
```

注： module_platform_driver是一个宏，如果不想使用该宏，而是想采用传统格式module_init/module_exit来书写platform_driver, “drivers/tty/serial/stm32-usart.c” 提供了一个范例。

```shell
# query modle info in sysfs
$ tree  /sys/module/test/
test/
├── coresize
├── drivers
│   └── platform:xxxxx -> ../../../bus/platform/drivers/xxxxx
├── holders
├── initsize
├── initstate
├── notes
├── parameters
│   └── panel_type
├── refcnt
├── sections
│   ├── __mcount_loc
│   └── __param
├── srcversion
├── taint
└── uevent
```

## A.3. Linux kernel开启DEBUG选项

kernel不会将这些日志输出到控制台上，除非：
- 1）开启了DEBUG宏，并且
- 2）kernel printk的默认日志级别大于7

### 设置kernel printk的默认日志级别为8

修改printk的默认日志级别的方法有多种，例如直接修改printk.c(新kernel为printk.h)中的`CONSOLE_LOGLEVEL_DEFAULT`宏定义。不过修改kernel原生代码的方式稍显粗暴，我们还有优雅一些的手段，例如通过命令行参数的loglevel变量传递，如下：

```diff
diff --git a/arch/arm64/configs/xprj_defconfig b/arch/arm64/configs/xprj_defconfig
index 5d0d591..9335d3f 100644
--- a/arch/arm64/configs/xprj_defconfig
+++ b/arch/arm64/configs/xprj_defconfig
@@ -320,7 +320,7 @@ CONFIG_FORCE_MAX_ZONEORDER=11
#
# Boot options
#
-CONFIG_CMDLINE="earlycon=owl_serial"
+CONFIG_CMDLINE="earlycon=owl_serial loglevel=8"
CONFIG_CMDLINE_FORCE=y
# CONFIG_EFI is not set
```

### DEBUG设置

- 针对单个文件开启debug

	在需要调用`pr_debug/dev_dbg`输出的模块开头，直接#define DEBUG即可

	例子：
	```c++
	/*linux\init\main.c*/
	#define DEBUG		/* Enable initcall_debug */
	...
	```

- 模块级生效debug

	在编译kernel的时候，通过KCFLAGS直接传递，例如：

    ```diff
	diff --git a/Makefile b/Makefile
	index 0835b1c..59625f4 100644
	--- a/Makefile
	+++ b/Makefile
	@@ -83,7 +83,7 @@ kernel-config:
	kernel:
			mkdir -p $(KERNEL_OUT_DIR)
			make -C $(KERNEL_DIR) CROSS_COMPILE=$(CROSS_COMPILE) KBUILD_OUTPUT=$(KERNEL_OUT_DIR) ARCH=$(BOARD_ARCH) $(KERNEL_D
	-       make -C $(KERNEL_DIR) CROSS_COMPILE=$(CROSS_COMPILE) KBUILD_OUTPUT=$(KERNEL_OUT_DIR) ARCH=$(BOARD_ARCH) $(KERNEL_T
	+       make -C $(KERNEL_DIR) CROSS_COMPILE=$(CROSS_COMPILE) KBUILD_OUTPUT=$(KERNEL_OUT_DIR) KCFLAGS=-DDEBUG ARCH=$(BOARD_
	``` 
	注意：打印太多可能会开机卡死，最好不要在顶层的Makefile中添加，比如在”`kernel\Makefile`“.

## A.4 增加系统调用

[ARM增加一个系统调用](https://biscuitos.github.io/blog/SYSCALL_hello/)

[X86_64 架构增加一个系统调用](https://biscuitos.github.io/blog/SYSCALL_ADD_NEW_X86_64/)

以arm为例：

- 首先在内核源码中修改文件 “arch/arm/tools/syscall.tbl”， 这会导致：“arch/arm/include/generated/uapi/asm/unistd-common.h” 文件中自动生成了系统调用号；在 “arch/arm/include/generated/” 目录下自动生成相应的系统调用入口文件；

- 接着添加实际的系统调用接口， 可以在内核源码数任意位置，添加一个 C 源码文件，并 将源码文件编译进内核即可

- 最后就是 在用户空间添加一个系统调用的函数。例如我们添加了一个“400”号系统调用

	```c++
	//用户空间通过 swi 指令可以执行一次系统调用，在执行系统调用 的时候，
	//将系统调用号存储在 r7 寄存器，并使用 “swi 0” 指令 触发系统调用。
	#define sys_eabi_api()  __asm__ __volaltile__ \
			("mov r7, #400\n\t" \
			"swi 0")
	int main(){
		sys_eabi_api();
		return 0;
	}		
	```

上面演示的方法不是通过类似libc/glibc库函数进行系统调用（使用库函数时，用户空间可以通过 “syscall()” 函数调用系统调用。）；不同架构是通过不同的软中断方式将指定信息传递给内核；例如 x86 通过 0x80 软中断进入内核系统调用处理， 例如 arm 通过 swi 指令进入内核的系统调用处理。当然无论以何种方式进入内核系统 调用处理程序，内核系统调用处理程序都会由用户态切换到内核态，并根据传递的 系统调用号信息，在内核中查找系统调用的入口。

