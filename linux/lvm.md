
## 概念

- 物理卷（physicalvolume）： pvdisplay  是LVM的基本存储逻辑块，它包含一个或多个物理存储介质（如分区、磁盘等），它包含有与LVM相关的管理参数

- 卷组（Volume Group）： vgdisplay， 它由物理卷组成。可以在卷组上创建一个或多个“LVM分区”（逻辑卷），LVM卷组由一个或多个物理卷组成。

- 逻辑卷（logicalvolume）： lvdisplay。 LVM的逻辑卷类似于非LVM系统中的硬盘分区，在逻辑卷之上可以建立文件系统(比如/home或者/usr等)。


- PE（physical extent）：每一个物理卷被划分为称为PE(Physical Extents)的基本单元，具有唯一编号的PE是可以被LVM寻址的最小单元。PE的大小是可配置的，默认为4MB。
- LE（logical extent）：逻辑卷也被划分为被称为LE(Logical Extents) 的可被寻址的基本单位。在同一个卷组中，LE的大小和PE是相同的，并且一一对应。

## 概论

### LV建立流程：

1.建立PV

2.建立VG，将PV加入到VG中。

3.建立LV，并设置LV大小。

4.格式化LV或LP成你需要的文件系统。

 ### LV删除流程：

1.umountFS

2.删除LV。

3.将PV从所在的VG中删除。

4.删除VG。


|     |             |          |            |           |             |           | 	
|     | 查看显示    |  创建      |  删除        |扩容         |激活     |   扫描查找 |   
| --  | ----        | ------   |  -------   | -------   | -------     | -----     |  
|LV   | lvdisplay   | lvcreate |   lvremove |   lvextend|    lvchange |   lvscan  | 
|PV   | pvdisplay   | pvcreate |   pvremove |           |    pvchange |   pvscan  | 
|VG   | vgdisplay   | vgcreate |   vgremove |   vgextend|    vgchange |   vgscan  |  
|     |             |          |            |           |             |           |

## 

### 1. 创建PV
下面的例子是将整个盘来创建PV， 以分区来创建类似
```shell
#例如使用sdb创建基于磁盘的物理卷
$ sudo pvcreate /dev/sdb
Physical volume "/dev/sdb"successfully created
$# pvdisplay
PV Name /dev/sdb
VG Name
PV Size 500.00 GB
Allocatable NO
...
```

### 2.1  创建新VG

创建data卷组：

```shell
$ sudo vgcreate data /dev/sdb
Volume group "data" successfullycreated
#查看卷组是否创建成功：
$ sudo vgdisplay
VG Name data
System ID
Format lvm2
...
# 激活VG
$ sudo vgchange -a y data
0 logical volume(s) in volume group"data" now active
```

### 2.2 将新的PV物理卷添加到现有VG卷组：

在2.1中是从一个pv新创建一个VG。 但也可以将一个pv放置到已经存在的VG。 例如通过步骤1创建了一个新pv "/dev/sdc"， 下面的操作是将该pv放置到已经存在的VG “data”中

```shell
$ sudo vgextend data /dev/sdc
Volume group "data" successfullyextended
```

### 3 从现有的VG中删除一个PV：

要从一个卷组中删除一个物理卷，首先要确认要删除的物理卷没有被任何逻辑卷正在使用，就要使用pvdisplay命令察看一个该物理卷信息，如果某个物理卷正在被逻辑卷所使用，就需要将该物理卷的数据备份到其他地方，然后再删除。删除物理卷的命令为vgreduce：
```shell
$ sudo vgreduce data /dev/sdc
Removed "/dev/sdc" fromvolume group "data" #已经删除成功
```

### 4.1 创建逻辑卷LV

创建逻辑卷的命令为lvcreate，分为两种：
```shell
#A、 创建指定大小的逻辑卷LV
##该命令就在卷组data上创建名字为data001，大小为200M的逻辑卷，并且设备入口为/dev/data/data001（data为卷组名，data001为逻辑卷名）。
$ sudo lvcreate -L200M -n data001 data
Logical volume"data001" created

# 查看 VG上的LV
$ sudo lvdisplay data
--- Logical volume ---
LV Name /dev/data/data001   #lv_pathname /dev/data/data001
VG Name data ----卷组的名称为data
....
 
#B、 创建卷组全部大小的逻辑卷LV
# 如果希望创建一个使用全部卷组的逻辑卷，则需要首先通过vgdisplay察看该卷组的Total PE数，然后在创建逻辑卷时指定
$ sudo lvcreate -l127949 -n data003 data
```

### 4.2 对已存在LV更改大小
需要两个操作：首先依据对应VG free size更改LV大小，然后扩容文件系统
例如， 先“lvextend -L+500M lv_pathname”，然后“resize2fs   lv_pathname”

### 5 创建文件系统：
```shell
#我们可以创建reiserfs文件系统，也可以创建ext2和ext3
$ sudo mkfs.ext3 /dev/data/data003
```

### 6 挂载文件系统 
通过mount挂载 ，例如“mount /dev/data/data003  文件系统名字”

