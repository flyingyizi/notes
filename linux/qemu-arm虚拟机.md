

# 1.准备etc

下面的内容与"`http://files.cnblogs.com/files/pengdonglin137/etc.tar.gz`"一致。

### 目录文件
```
etc/
etc/profile
etc/inittab
etc/fstab
etc/sysconfig/
etc/sysconfig/HOSTNAME
etc/init.d/
etc/init.d/rcS
```

### etc/fstab文件
```shell
~/etc $ cat fstab
#device		mount-point	type	options		dump	fsck order
proc		/proc		proc	defaults		0	0
tmpfs		/tmp		tmpfs	defaults		0	0
sysfs		/sys		sysfs	defaults		0	0
tmpfs		/dev		tmpfs	defaults		0	0
var		/dev		tmpfs	defaults		0	0
ramfs		/dev		ramfs	defaults		0	0
debugfs		/sys/kernel/debug	debugfs		defaults	0	0
```

### etc/profile文件
```shell
~/etc $ cat profile 
USER="root"
LOGNAME=$USER
export HOSTNAME=`/bin/hostname`
export USER=root
export HOME=/root
export PS1="[$USER@$HOSTNAME \W]\# "
PATH=/bin:/sbin:/usr/bin:/usr/sbin
LD_LIBRARY_PATH=/lib:/usr/lib:$LD_LIBRARY_PATH
export PATH LD_LIBRARY_PATH
```

### etc/inittab文件
```shell
~/etc $ cat inittab 
# /etc/inittab
::sysinit:/etc/init.d/rcS
console::askfirst:-/bin/sh
::ctrlaltdel:/sbin/reboot
::shutdown:/bin/umount -a -r
::restart:/sbin/init
```

### etc/sysconfig/HOSTNAME文件
```shell
~/etc $ cat sysconfig/HOSTNAME 
vexpress
```

### etc/init.d/rcS文件
```shell
~/etc $ cat init.d/rcS 
#!/bin/sh

PATH=/sbin:/bin:/usr/sbin:/usr/bin
runlevel=S
prevlevel=N
umask 022
export PATH runlevel prevlevel

mount -a
mkdir -p /dev/pts
mount -t devpts devpts /dev/pts
#mount -n -t usbfs none /proc/bus/usb
echo /sbin/mdev > /proc/sys/kernel/hotplug
mdev -s
mkdir -p /var/lock

ifconfig lo 127.0.0.1
#ifconfig eth0 10.238.233.5

/bin/hostname -F /etc/sysconfig/HOSTNAME
```

# 2.编译busybox，制作rootfs

下面演示了使用busybox来制作rootfs

注意：编译rootfs，编译kernel 应提供一致的"CROSS_COMPILE, ARCH"环境参数

## 编译busybox
```shell
~/ $  git clone git://busybox.net/busybox.git
~/ $  cd busybox/
~/busybox $  export ARCH=arm
~/busybox $  export CROSS_COMPILE=arm-linux-gnueabihf-
~/busybox $  make clean
~/busybox $  make defconfig
~/busybox $  make menuconfig
# 选择static编译
#  x     -> Settings  
#           Build static binary (no shared libs)
#
~/busybox $  make -j4
~/busybox $  make install
```

## 制作rootfs

以下指令在"`~/busybox`"目录下执行，最终得到结果文件"`a9rootfs.ext3`"
```shell
rm -rf rootfs
rm -rf tmpfs
rm -f a9rootfs.ext3
mkdir rootfs
cp _install/*  rootfs/ -raf
mkdir -p rootfs/{lib,proc,sys,tmp,root,var,mnt,dev}
# 注意与CROSS_COMPILE 设定一致
cp -arf /usr/arm-linux-gnueabihf/lib rootfs/
rm rootfs/lib/*.a
# 该压缩包为上一节"准备etc"章节中要求的内容
wget http://files.cnblogs.com/files/pengdonglin137/etc.tar.gz
tar zxvf etc.tar.gz -C rootfs/

sudo chown -R root:root rootfs/*

sudo mknod rootfs/dev/tty1 c 4 1
sudo mknod rootfs/dev/tty2 c 4 2pro
sudo mknod rootfs/dev/tty3 c 4 3
sudo mknod rootfs/dev/tty4 c 4 4
sudo mknod rootfs/dev/console c 5 1
sudo mknod rootfs/dev/null c 1 3
dd if=/dev/zero of=a9rootfs.ext3 bs=1M count=32
mkfs.ext3 a9rootfs.ext3
mkdir -p tmpfs
sudo mount -t ext3 a9rootfs.ext3 tmpfs/ -o loop
sudo cp -r rootfs/*  tmpfs/
sudo umount tmpfs
rm-rf  tmpfs
```

# 3.编译kernel

注意：编译rootfs，编译kernel 应提供一致的"CROSS_COMPILE, ARCH"环境参数


 ```shell
# 下载source 
~/ $  git clone git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git kernel
~/ $  cd kernel/
~/kernel $  export ARCH=arm
~/kernel $  export CROSS_COMPILE=arm-linux-gnueabihf-
~/kernel $  make clean
# 配置
~/kernel $  make vexpress_defconfig
~/kernel $  make menuconfig
# disable L2x0 outer cache controller
#System Type ---> 
#  [] Enable the L2x0 outer cache controller
#
# 后面我们采用sd卡存放rootfs，所以确保不要提供Initramfs source
#  General setup ---> 
#    [*] Initial RAM filesystem and RAM disk (initramfs/initrd) support
#     ()    Initramfs source file(s)     

~/kernel $  make bzImage -j4
~/kernel $  make dtbs
```

# 4. 启动、关闭虚拟机

## 启动

```shell
# -M vexpress-a9 模拟vexpress-a9单板，你能够使用-M ?參数来获取该qemu版本号支持的全部单板
# -smp 4 启动4个cpu
# -m 512M 单板执行物理内存512M
# -kernel arch/arm/boot/zImage   告诉qemu单板执行内核镜像路径
# -dtb arch/arm/boot/dts/vexpress-v2p-ca9.dtb  告诉qemu单板的设备树（必须加入）
# -nographic 不使用图形化界面，仅仅使用串口
# -append "console=ttyAMA0" 内核启动參数。这里告诉内核vexpress单板执行。串口设备是哪个tty。
# 由于采用sd卡，所以启动参数使用了"root=/dev/mmcblk0 rw ",假如我们在"make menuconfig"中设置了`Initramfs source file`
# 那应该提供"rdinit=/linuxrc"的启动参数
$ qemu-system-arm -M vexpress-a9 -m 512M -kernel arch/arm/boot/zImage -dtb arch/arm/boot/dts/vexpress-v2p-ca9.dtb -nographic -append "root=/dev/mmcblk0 rw console=ttyAMA0 loglevel=8" -sd ../busybox/a9rootfs.ext3 
#
# 在"make menuconfig"中设置了`Initramfs source file`(例如`/home/xxx/busybox/_install`)
#$ qemu-system-arm -M vexpress-a9 -smp 4 -m 1024M -kernel arch/arm/boot/zImage -append "rdinit=/linuxrc console=ttyAMA0 #loglevel=8" -dtb arch/arm/boot/dts/vexpress-v2p-ca9.dtb -nographic
#
```


## 关闭

在另外的console(即非启动虚拟机的console)中执行"`$ killall qemu-system-arm`"

## 嵌入式开发时建议
在嵌入式开发时，往往需要直接debug，所以上面开串口通信的方式不太合适，下面演示了对LM3S6965 板子直接开通gdb debug的操作：

Testing it
This program is a valid LM3S6965 program; we can execute it in a virtual microcontroller (QEMU) to test it out.

在一个终端开启
```shell
$ # this program will block
$ qemu-system-arm \
      -cpu cortex-m3 \
      -machine lm3s6965evb \
      -gdb tcp::3333 \
      -S \
      -nographic \
      -kernel target/thumbv7m-none-eabi/debug/app
```

在另外一个终端调试
```shell
$ # on a different terminal
$ arm-none-eabi-gdb -q target/thumbv7m-none-eabi/debug/app
Reading symbols from target/thumbv7m-none-eabi/debug/app...done.

(gdb) target remote :3333
Remote debugging using :3333
Reset () at src/main.rs:8
8       pub unsafe extern "C" fn Reset() -> ! {

(gdb) # the SP has the initial value we programmed in the vector table
(gdb) print/x $sp
$1 = 0x20010000

(gdb) step
9           let _x = 42;

(gdb) step
12          loop {}

(gdb) # next we inspect the stack variable `_x`
(gdb) print _x
$2 = 42

(gdb) print &_x
$3 = (i32 *) 0x2000fffc

(gdb) quit
```


# 杂项

## qemu tutorial

[QEMU Tutorial with Examples](https://www.poftut.com/qemu-tutorial/)

基本语法是："`qemu-system-arm [OPTIONS] [IMAGE]`"，IMAGE是镜像文件"a raw hard disk image for IDE hard disk 0"，流行linux一般都会提供各自的云镜像文件

- 设置Qemu VM的CPU和核心计数(Set CPU and Core Count for Qemu VM)

  可以在Qemu中设置VM的CPU核心数。 我们将提供-smp选项，该选项将启用具有core=2值的多个CPU内核，这将在给定VM中设置内核计数2。例如"`qemu-system-arm -smp cores=2`"

- 设置Qemu VM的RAM或内存大小 (Set RAM or Memory Size For Qemu VM)

  RAM是VM的另一个重要组件。 默认情况下，为启动的VM设置128 MB RAM。 但是我们也可以使用-m选项显式设置RAM的大小，这是内存的简称。 我们还将以MB为单位提供大小，在此示例中，我们将设置256 MB Ram。例如"`qemu-system-arm -m 256`"

- 为Qemu VM指定磁盘文件或映像 (Specify Disk File or Image For Qemu VM)

  我们可以使用-drive选项指定磁盘/映像文件。 这将使我们能够指定额外的驱动程序相关选项。 file用于指定文件大小。 另外if用于提供磁盘的驱动程序或接口类型。例如"`qemu-system-arm -drive file=xxx.qcow2,if=virtio`"

- 禁用Qemu VM的GUI (Disable GUI for Qemu VM)

  默认情况下，VM控制台将作为GUI窗口提供。 但是我们也可以禁用控制台，仅向系统终端提供-nographic选项。例如"`qemu-system-arm -nographic`"

- 像virbr0一样连接到虚拟交换机 (Connect To Virtual Switch Like virbr0)

  默认情况下，启动的来宾系统将不连接网络，并且仅连接单个接口。 在大多数情况下，这不是实用且有用的功能。 我们可以添加网络接口，并将此接口连接到在主机操作系统中运行的桥接设备。 在此示例中，我们将通过virtio类型的网络接口将客户机连接到名为virbr0的虚拟交换机/桥中。例如"`qemu-system-arm  -drive file=xxx.qcow2,if=virtio -display none -net nic,model=virtio`"

- 对Qemu VM使用远程磁盘映像 (Use Remote Disk Images For Qemu VM)

  通常，使用VM磁盘映像的最佳方法是在本地使用，但是qemu还通过与诸如SSH等不同协议的网络共享来支持远程磁盘映像。在本示例中，我们将通过以下disk.img在远程系统上使用名为disk.img的磁盘映像：通过使用file选项的SSh。例如"`qemu-system-arm -drive file=ssh://x.x.x/disk.img`"

- 

例如"`qemu-system-arm -M vexpress-a9 --device help`"

## 镜像格式转化
qemu的镜像格式是qcow2，下面列举了几种镜像格式转化的方法。

### raw->qcow2     

    ` qemu-img convert -f raw centos.img -O qcow2 centos.qcow2`

     转换完成后，将新生产一个目标映像文件，原文件仍保存。

### VMDK–>qcow2:
    `qemu-img convert -f vmdk -O qcow2 SLES11SP1-single.vmdk SLES11SP1-single.img`

### VMDK–>qcow2:
    `qemu-img convert -f vmdk -O qcow2 SLES11SP1-single.vmdk SLES11SP1-single.img`

### qcow2–>raw:
    `qemu-img convert -O qcow2 image-raw.raw image-raw-converted.qcow`