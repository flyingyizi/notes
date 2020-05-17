
部署一般采用： https----nginx-----http------主机

这种部署方式相比主机直接对外提供https服务的效率更高

# linux openresty 安装与运行

## linux安装openresty

[OpenResty® Linux 包](http://openresty.org/cn/linux-packages.html)

[运行openresty](http://openresty.org/cn/getting-started.html)

### [nginx]查看安装了哪些模块

```sh
$ 2>&1 nginx -V | tr ' '  '\n'|grep ssl
--with-http_ssl_module
--with-mail_ssl_module
```

如果是安装的openresty，那要通过`openresty -V`查看，或`aptitude search openresty`看下相关的模块安装情况。

## NGINX重启/升级。。

`ps aux | grep nginx` 查看pid

[参考](https://www.cnblogs.com/wushuaishuai/p/9315611.html)

首先应该通过"nginx -t -c"命令来坚持conf文件的正确性。然后 通过“kill -HUP nginx主进程号” ，nginx的信号包括以下：
- TERM,INT 快速关闭
- QUIT 从容关闭
- HUP  平滑重启，重新加载配置文件
- USR1  重新打开日志文件，在切割日志时用途较大
- USR2  平滑升级可执行文件
- WINCH  从容关闭工作进程

例如

```sh
$kill -HUP `cat /usr/local/openresty/nginx/nginx.pid`
```

例如建立下面的crontab执行脚本实现日志的定时切割

```sh
#!/bin/bash
#这个脚本须在每天的00:00 运行
# nginx 志文件的存放珞径
logs_path="/data1/logs"
mkdir -p ${logs_path}$(date -d "yestaday"＋"%Y")/${date -d  "yestaday"＋"%m"}/ 

mv $(logs_path}access.log ${1ogs_path}$(date -d "yestaday"＋"%Y")/$(date -d "yestaday"＋"%m")/access_$(date -d "yestaday"＋"%Y%m%d").log

kill -HUP `cat /usr/local/openresty/nginx/nginx.pid`
```

### 使用systemctl来管理openresty

对Ubuntu，对应配置文件是“/lib/systemd/system/openresty.service”

```sh
#查看openresty状态
systemctl status openresty.service

#启动openresty
systemctl start openresty.service

#设置openresty开机自启动
systemctl enable openresty.service

#重启openresty
systemctl restart openresty.service

#
systemctl reload openresty.service

#停止openresty
systemctl stop openresty.service

#取消openresty开机自启动
systemctl disable openresty.service
```


#  docker安装与运行

下载镜像：
atmel@atmel-virtual-machine:~$ docker image ls
REPOSITORY            TAG                 IMAGE ID            CREATED             SIZE
openresty/openresty   latest              f1b53e06bdca        12 months ago       44.9MB

```shell
docker run -d --name="nginx" --net=host  \
    -v ~/docker_build/openresy/nginx.conf:/usr/local/openresty/nginx/conf/nginx.conf    \
    -v ~/docker_build/openresy/logs:/usr/local/openresty/nginx/logs    \
    -v ~/docker_build/openresy/html:/usr/local/openresty/nginx/html    \
    -v ~/docker_build/openresy/objstore:/usr/local/openresty/nginx/objstore    \
 openresty/openresty:latest
```

/usr/local/openresty # ls
bin          luajit       lualib       nginx        pod          resty.index  site

/usr/local/openresty/nginx/sbin # ls
-rwxr-xr-x    1 root     root      19336384 Nov 18  2016 nginx
/usr/local/openresty/nginx/conf # ls
fastcgi.conf            fastcgi_params.default  mime.types              nginx.conf.default      uwsgi_params
fastcgi.conf.default    koi-utf                 mime.types.default      scgi_params             uwsgi_params.default
fastcgi_params          koi-win                 nginx.conf              scgi_params.default     win-utf

测试是否正常
~ # /usr/local/openresty/nginx/sbin/nginx -t
nginx: the configuration file /usr/local/openresty/nginx/conf/nginx.conf syntax is ok
nginx: configuration file /usr/local/openresty/nginx/conf/nginx.conf test is successful
~ #

## 配置

### nginx日志文件配置与切割

涉及到的nginx指令主要就是两条：log_format, access_log. 前者用于设置日志格式；后者用户设置日志文件的存放路径/格式/缓存大小。。两条指令在配置文件的位置可以在http{...}之间,也可以是 server{...}之间。


### 下载nginx sysguard模块

    如果nginx被攻击或者访问量突然变大，nginx会因为负载变高或者内存不够用导致服务器宕机，最终导致站点无法访问。 今天要谈到的解决方法来自淘宝开发的模块nginx-http-sysguard，主要用于当负载和内存达到一定的阀值之时，会执行相应的动作，比如直接返回503,504或者其他的。一直等到内存或者负载回到阀值的范围内，站点恢复可用。简单的说，这几个模块是让nginx有个缓冲时间，缓缓。

$ wget https://github.com/alibaba/nginx-http-sysguard/archive/master.zip -O nginx-http-sysguard-master.zip
$ unzip nginx-http-sysguard-master.zip

安装 OpenResty

    OpenResty（也称为 ngx_openresty）是一个全功能的 Web 应用服务器。它打包了标准的 Nginx 核心，很多的常用的第三方模块，以及它们的大多数依赖项。 OpenResty 中的 LuaJIT 组件默认未激活，需使用 --with-luajit 选项在编译 OpenResty 时激活,使用--add-module，添加上sysguard模块

安装的版本：1.2.7.6

下载地址：

    http://openresty.org/#Download
    http://openresty.org/download/ngx_openresty-1.2.7.6.tar.gz

先安装依赖软件，然后在编译代码，编译时使用--perfix选项指定 OpenResty 的安装目录，--with-luajit 选项激活 LuaJIT 组件。

```sh
$ yum -y install gcc make gmake openssl-devel pcre-devel readline-devel zlib-devel

$ wget http://openresty.org/download/ngx_openresty-1.2.7.6.tar.gz
$ tar zvxf ngx_openresty-1.2.7.6.tar.gz
$ cd ngx_openresty-1.2.7.6
$ ./configure --with-luajit --with-http_stub_status_module --add-module=/opt/nginx-http-sysguard-master/
$ gmake && gmake install
```

创建软连接：

```sh
$ ln -s /usr/local/openresty/nginx/sbin/nginx /usr/sbin/nginx
```

# windows openresty记录

## 安装

从[官网](http://openresty.org/cn/download.html)下载windows版本，解压即可。以下假设安装在“C:\prog\openresty”

```text
C:\prog\openresty>tree
├─conf
├─fastcgi_temp
├─html
├─include
│  └─luajit-2.1
├─logs
├─lua
│  
├─lualib
...

```

## 启动、关闭、重启

  nginx在windows下，涉及到路径的录入注意路径的写法，必须采用反斜杠“/”这种方式，例如“nginx -p c:/prog/openresty”，如果路径采用“\\”方式，从实践看有不行。在nginx.conf中类似设置root时也需要采用这种路径的写法

- 启动

    ```cmd
    C:\prog\openresty>start nginx -p c:/prog/openresty
    ```

- 查看进程情况

    ```cmd
    C:\prog\openresty>tasklist /fi "imagename eq nginx.exe"
    ```

- 结束

    ```cmd
    C:\prog\openresty>nginx -s stop
    ```

- 重新加载

    ```cmd
    C:\prog\openresty>nginx -s reload
    ```


# wrk 安装与运行

看下面链接文章吧

https://www.digitalocean.com/community/tutorials/how-to-benchmark-http-latency-with-wrk-on-ubuntu-14-04

docker run -d -p 192.168.1.8:3000:3000 --name=http-debugging-application czerasz/http-debugger

curl -i -XPOST http://192.168.1.8:3000/test -d 'test=true'

docker run --rm williamyeh/wrk -t2 -c5 -d5s -H 'Host: example.com' --timeout 2s http://192.168.1.8:3000/


