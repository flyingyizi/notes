
[rsync常见问题及解决办法](https://blog.whsir.com/post-392.html)

## 客户端使用

[下载 cwrsync客户端](https://www.itefix.net/cwrsync-free-edition)

[linux中Rsync命令的实际示例](https://www.cnblogs.com/weifeng1463/p/7809851.html)


与rsync命令一起使用的一些常见选项
```text
-v：详细
-r：将数据递归（但不保存时间戳和权限，同时传输数据
-a：归档模式下，归档模式允许递归地拷贝文件，同时也保留了符号链接，文件权限，用户组所有权和时间戳
-z：压缩文件中的数据
-h：在人类可读的格式人类可读，输出数字
```

例如：

```sh
#rsync options source destination
#backup.tar需要的文件名被复制或同步到/tmp/备份/文件夹。
[root@howtoing]$rsync -zvh backup.tar /tmp/backups/

#将来自本地计算机的目录同步到远程机器，例如 ：有一个在其中包含了一些RPM包本地计算机“rpmpkgs”的文件夹，你想那个地方目录的内容发送到远程服务器，您可以使用以下命令。
[root@howtoing]$rsync -avz rpmpkgs/ root@192.168.0.101:/home/
```

## Linux 服务侧配置

安装 rsync

```shell
# 检查是否安装了 rsync
aptitude search rsync
# 如果没有安装的话，进行安装

# 将 rsync 设置成开启自启，并启动
#rsync默认是没有启动的，可通过ps -e |grep  rsync查看

#可在  /etc/init.d  路径下执行：
    ./rsync start　#启动　
    ./rsync start　#关闭

#或
$sudo systemctl status/start/stop rsync.service

#可在　/etc/default 路径下的rsync文件中将其改为自启动
　　将 RSYNC_ENABLE=true 
```

## 编写配置文件

主要是`/etc/rsyncd.conf`的书写,[参考1](https://blog.51cto.com/6226963/1560355),[参考2](https://www.cnblogs.com/felixzh/p/4950049.html)

默认是没有配置文件的，查看rsync服务的状态时，会提示需要新建`/etc/rsyncd.conf `

```sh
atmel:/etc/default$sudo systemctl status rsync.service
* rsync.service - fast remote file copy program daemon
   Loaded: loaded (/lib/systemd/system/rsync.service; enabled; vendor preset: enabled)
   Active: inactive (dead)
Condition: start condition failed at Sun 2020-05-24 06:49:15 CST; 10s ago
           `- ConditionPathExists=/etc/rsyncd.conf was not met
```

通过查看安装包的情况，知道在`examples/rsyncd.conf`下面有一个范例，因此配置文件从它开始是合适的。

```sh
atmel:/etc/default$dpkg -L rsync
/.
/etc
/etc/default
/etc/default/rsync
...
/usr/share/doc/rsync/examples
/usr/share/doc/rsync/examples/logrotate.conf.rsync
/usr/share/doc/rsync/examples/rsyncd.conf
...
```

下面是配置文件中的一个模块举例,这里需要特别注意的是secrets file的权限模式必须是600. 并且该文件的内容是“username:passwd”模式

```text
[common]
path = /home/user/input
comment = Web content
read only = no
write only = yes
list = no
auth users = upload
secrets file = /home/user/test/rsync/etc/test.pass
```

例如下面是服务侧运行后查看服务侧module的结果
```shell
c:\home\temp>rsync --list-only mypc@192.168.1.8::
ftp             public archive
common          Web content
```

例如下面是将服务侧样例同步到客户端的示例
```shell
c:\home\temp>rsync -vzrtopg --progress mypc@192.168.1.8::common .
Password:
receiving incremental file list
./
mytest.txt
              0 100%    0.00kB/s    0:00:00 (xfr#1, to-chk=0/2)

sent 46 bytes  received 108 bytes  34.22 bytes/sec
total size is 0  speedup is 0.00
```
例如下面将`c:\mayi` 目录同步到服务侧
```shell
rsync -azv --progress /cygdrive/C/mayi   mypc@192.168.1.8::common/xyz
```