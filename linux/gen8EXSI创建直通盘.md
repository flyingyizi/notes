以下描述中，已经知道ESXI host的IP是192.168.1.5。  内容参考的是[gen8折腾记录](https://www.biaodianfu.com/gen8-esxi.html)

0. 开通ssh访问ESXI
   登录EXSI web管理界面（192.168.1.5），在“host/actions/services”中开启TSM（ESXi Shell）和TSM-SSH（SSH）这两个服务

1. 获取磁盘基本信息
   这个步骤涉及到两个信息，一个是要新增直通的磁盘所在路径，另外一个是将磁盘创建虚拟盘后，虚拟盘存放的路径两个信息，下面分别来获取这两个信息。

   登录EXSI web管理界面（192.168.1.5），在“storage/devices”中，点击要查看磁盘的信息. 例如新插入了HGST, TOSHIBA两块盘，查看到他们的磁盘所在路径

   ` 路径:/vmfs/devices/disks/t10.ATA_____HGST_HDN728080ALE604____________________VJGL71YX____________`

   ` 路径:/vmfs/devices/disks/t10.ATA_____TOSHIBA_MG05ACA800E_____________________________57R3K3DTFEUF`

   通过已经在系统中的虚拟磁盘可以看到虚拟磁盘放在什么地方。 在“storage/datastores”中随便找一个虚拟盘点击详情，可以看到

   ` Location: /vmfs/volumes/5b9e57f0-ccd833b2-f691-00fd45fda5dc`


2. 创建RDM  直通盘
   
   通过ssh登录ESXI（192.168.1.5）

   创建RDM需要使用ssh中才能进行，具体的指令为：
```shell
vmkfstools -z /vmfs/devices/disks/t10.ATA_____HGST_HDN728080ALE604____________________VJGL71YX____________ /vmfs/volumes/5b9e57f0-ccd833b2-f691-00fd45fda5dc/hgst.vmdk
vmkfstools -z /vmfs/devices/disks/t10.ATA_____TOSHIBA_MG05ACA800E_____________________________57R3K3DTFEUF /vmfs/volumes/5b9e57f0-ccd833b2-f691-00fd45fda5dc/toshiba.vmdk
```
具体含义：

    第一个参数：创建模式，可选参数-z或-r。两种模式的差别很小，正常使用2种模式都可以。
        -r 参数创建的是 Virtual Compabilitiy Mode RDM，即 ESXi 会截获除 READ / WRITE 之外所有 SATA 指令
        -z 参数则是创建 Physical Compability Mode RDM，即 ESXi 除了 LUN REPORT 指令，其他全部原样传递给物理磁盘。
    第二个参数：物理磁盘路径
    第三个参数：创建的.vmdk 文件的保存路径，文件名可自定义。

3. 将直通盘加入已有虚拟机去使用
   
   例如当前已经有个虚拟机ubuntu guest os，那在ESXI web界面中，通过该虚拟维护界面上的“edit/add hard disk/existing hard disk” 菜单，将前面创建的虚拟盘给到对应虚拟机使用了。

   最后进入虚拟机将磁盘挂载上来。例如`sudo mount -t ntfs-3g  /dev/sdb1  /mnt/toshiba/`

4. samba提供访问直通盘
   
   在linux系统下，考虑通过samba对外提供直通盘的访问，因为历史采用NFS访问时，乱码问题较难解决。
    - (2)安装samba服务器。
    sudo apt-get install samba samba-common

Ubuntu 18.04 安装Samba服务器及配置

(3)创建一个用于分享的samba目录。
sudo mkdir /home/linuxidc/linuxidc.com/share

(4)给创建的这个目录设置权限
sudo chmod 777 /home/linuxidc/linuxidc.com/share

Ubuntu 18.04 安装Samba服务器及配置

(5)添加用户(下面的linuxidc是我的用户名，之后会需要设置samba的密码)。
sudo smbpasswd -a linuxidc

Ubuntu 18.04 安装Samba服务器及配置

(6)配置samba的配置文件。
sudo nano /etc/samba/smb.conf

在配置文件smb.conf的最后添加下面的内容：

[share]
comment = share folder
browseable = yes
path = /home/linuxidc/linuxidc.com/share
create mask = 0700
directory mask = 0700
valid users = linuxidc
force user = linuxidc
force group = linuxidc
public = yes
available = yes
writable = yes

Ubuntu 18.04安装Samba服务器及配置

在上面valid users = linuxidc中的linuxidc为我的用户名。

(7)重启samba服务器。

sudo service smbd restart

(8)Windows徽标+R 在弹出的运行窗口中输入 \\ip即可访问。如\\192.168.182.188,输入samba用户名及密码访问即可看到共享，然后就可以在Linux系统与Windows系统直接进行文件共享了