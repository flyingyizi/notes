

[下载 cwrsync客户端](https://www.itefix.net/cwrsync-free-edition)


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

主要是`/etc/rsyncd.conf`的书写,[参考](https://www.cnblogs.com/felixzh/p/4950049.html)

