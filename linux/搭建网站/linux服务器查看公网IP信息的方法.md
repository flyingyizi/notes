
linux服务器查看公网IP信息的方法

```sh
    curl cip.cc

    或者

    curl  -s  icanhazip.com
```

## 设置linux email服务器

首先确认下电信宽带没有封禁25端口

```text
atmel@atmel:~$ telnet smtp.163.com 25
Trying 220.181.12.13...
Connected to smtp.163.com.
Escape character is '^]'.
220 163.com Anti-spam GT for Coremail System (163com[20141201])
```


1. 以163邮箱为例，进入邮箱网站，通过“设置” 开启“POP3/SMTP服务”，并查看得知服务器地址：

[阿里云服务器使用postfix通过三方stmp服务器发送邮件](https://blog.sbot.io/articles/61/%E9%98%BF%E9%87%8C%E4%BA%91%E6%9C%8D%E5%8A%A1%E5%99%A8%E4%BD%BF%E7%94%A8postfix%E9%80%9A%E8%BF%87%E4%B8%89%E6%96%B9SMTP%E6%9C%8D%E5%8A%A1%E5%99%A8%E5%8F%91%E9%80%81%E9%82%AE%E4%BB%B6)

//TODO 上面的没有成功

2. 只有下面的方式成功了。

heirloom-mailx
```sh
wget http://ftp.debian.org/debian/pool/main/h/heirloom-mailx/heirloom-mailx_12.5.orig.tar.gz
tar zxvf heirloom-mailx_12.5.orig.tar.gz
cd heirloom-mailx-12.5/
make && make install UCBINSTALL=/usr/bin/install
```

vi /etc/nail.rc（在配置文件最后添加）

```text
set bsdcompat
set from=flyingyizi@163.com
set smtp=smtp.163.com
set smtp-auth-user=flyingyizi@163.com
set smtp-auth-password=IQVGXKDPJQYOQTYI
set smtp-auth=login
```

3. 实现定时任务

编写脚本

vim ~/tool/send_ip.sh

```sh
#!/bin/bash 
#
history=/tmp/ip.txt

OLD=""
NEW=`curl  -s  icanhazip.com`
#
if [ -e $history ]
then
    OLD=`cat $history`
    if [ "$OLD"x != "$NEW"x ]
    then
        echo $NEW > $history
        echo $NEW |mailx -s "ip" flyingyizi@163.com
    fi
else
    echo $NEW > $history
    echo $NEW |mailx -s "ip" flyingyizi@163.com
fi
```

定时发送，创建任务
crontab -l，查看当前任务列表
crontab -e，编写任务
在末尾添加
0 12 * * * /bin/bash /home/atmel/tool/send_ip.sh

表示在每天中午十二点执行

ubuntu下crontab启动，重启，关闭命令

```sh
    状态：/etc/init.d/cron status ( service cron status )
    启动：/etc/init.d/cron start ( service cron start )
    重启：/etc/init.d/cron restart ( service cron restart )
    关闭：/etc/init.d/cron stop ( service cron stop )
```

4. 没有cron.log的问题

```text
    crontab在/var/log/目录下没有cron.log文件
    1、修改rsyslog文件：

    /etc/rsyslog.d/50-default.conf 
    　 　将  rsyslog  文件中的  #cron.*  前的  #  删掉；

    2、重启rsyslog服务：

    service rsyslog restart 
    3、重启cron服务：　　

    service cron restart
    4、查看日志文件：

    tail -f /var/log/cron.log 
```
    　　