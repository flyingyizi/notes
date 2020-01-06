
[rsync常见问题及解决办法](https://blog.whsir.com/post-392.html)

## 客户端使用

[下载 cwrsync客户端](https://www.itefix.net/cwrsync-free-edition)

[linux中Rsync命令的实际示例](https://www.cnblogs.com/weifeng1463/p/7809851.html)

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

#可在　/etc/default 路径下的rsync文件中将其改为自启动
　　将 RSYNC_ENABLE=true 
```

编写配置文件

主要是`/etc/rsyncd.conf`的书写,[参考1](https://blog.51cto.com/6226963/1560355),[参考2](https://www.cnblogs.com/felixzh/p/4950049.html)

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