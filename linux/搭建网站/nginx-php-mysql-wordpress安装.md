
本文演示了搭建完整wordpress环境所需各个部件的完整过程。

[搭建微信公众号后台——收发文本消息](https://support.huaweicloud.com/bestpractice-ecs/zh-cn_topic_0140957207.html)

本文基于ubuntu 18环境进行操作。在操作过程中可能有部分操作会出现乱码。建议在安装过程的终端中先执行“export LANG=C”


### 目录

- [1.安装nginx](#1安装nginx)
  - [1.1.安装](#11安装)
  - [1.2.nginx常用运维](#12nginx常用运维)
  - [1.3.nginx配置](#13nginx配置)
    - [1.3.1.location指令说明](#131location指令说明)
    - [1.3.2.构建https server](#132构建https-server)
- [2.安装PHP](#2安装php)
  - [2.1.安装](#21安装)
  - [2.2.启动](#22启动)
  - [2.3.记录php-fpm监听信息](#23记录php-fpm监听信息)
  - [2.4.同步nginx设置](#24同步nginx设置)
  - [2.5.phpfpm与nginx配置同步总结](#25phpfpm与nginx配置同步总结)
- [3.安装MySQL。](#3安装mysql)
  - [3.1.获取较新mysql包](#31获取较新mysql包)
  - [3.2.安装](#32安装)
  - [3.3创建数据库](#33创建数据库)
  - [3.4创建用户并赋远程权限](#34创建用户并赋远程权限)
    - [3.4.1.方式1](#341方式1)
    - [3.4.1.方式2](#341方式2)
- [4.安装vsftpd](#4安装vsftpd)
  - [4.1安装](#41安装)
  - [4.2配置vsftpd。](#42配置vsftpd)
  - [4.3.vsftpd常用运维](#43vsftpd常用运维)
  - [4.4.命令行模拟PASV](#44命令行模拟pasv)
    - [4.4.1.PASV下载](#441pasv下载)
    - [4.4.1.PASV上传](#441pasv上传)
  - [4.5. 典型客户端ftp-sftp工具](#45-典型客户端ftp-sftp工具)
    - [4.5.1.powershell](#451powershell)
    - [4.5.2.msys2-ftp](#452msys2-ftp)
    - [4.5.2.secureCRT](#452securecrt)
    - [4.5.2.scp](#452scp)
- [4.安装wordpress](#4安装wordpress)

# 1.安装nginx

可以使用原始nginx，但我更喜欢使用第三方openresty包。 下面安装nginx通过openresty安装实现。

服务器部署一般采用： https----nginx-----http------主机

这种部署方式相比主机直接对外提供https服务的效率更高

[nginx常见典型故障](https://www.cnblogs.com/johnnyblog/p/11479217.html)

## 1.1.安装

```sh
$sudo apt-get -y install openresty

#查看package信息
$dpkg -L openresty
...
/lib/systemd/system/openresty.service
...
/usr/local/openresty/nginx/conf/mime.types.default
/usr/local/openresty/nginx/conf/nginx.conf
...

```

## 1.2.nginx常用运维

```sh
#查看openresty状态
$sudo systemctl status openresty.service

#启动openresty
$sudo systemctl start openresty.service

#设置openresty开机自启动
$sudo systemctl enable openresty.service

#重启openresty
$sudo systemctl restart openresty.service

#reload configuration
$sudo systemctl reload openresty.service

#停止openresty
$sudo systemctl stop openresty.service

#取消openresty开机自启动
$sudo systemctl disable openresty.service
```

## 1.3.nginx配置

nginx配置是很复杂的事情，当前只要采用nginx的默认配置就足够了。配置文件在“/usr/local/openresty/nginx/conf/nginx.conf”，后续需要与php配合，对配置的修改将会在[php配置与nginx配置同步](#24同步nginx设置)中说明。

这里对nginx配置时最常用的两个知识点说明下，如果不关心，可以不关注，它们与后续其他安装没有关系。

### 1.3.1.location指令说明

```conf
 # 语法： location [=|~|~*|^~]   /uri/ { ... }
 # 对待匹配的“uri”，可能是字符串，也可能是正则。
 # "~"表示区分大小写的匹配，"~*"表示不区分大小写的匹配。先匹配字符串再匹配正则
 # "="表示进行精确匹配，如果找到匹配的uri这停止查询
 # "^~"表示禁止匹配到字符串后，再去检查正则

#补充解释
#完全匹配(=)
#无正则普通匹配(^~)（^ 表示“非”，~ 表示“正则”，字符意思是：不要继续匹配正则）
#正则表达式匹配(~或者~*)
#普通匹配

```

以无正则普通匹配(^~)为例，演示它与“~”正则规则匹配的关系，从演示结果看，“^~”优先级高于“~”正则。

基于下面的nginx.conf配置：

```conf
    location ~ /abcefg {
         return 399;
       }
    location ~ /abc {
         return 400;
       }
    location ^~ /abc {
         return 501;
    }
```

执行结果是如下，

```sh
$curl -I -s -w "%{http_code}\n" -o /dev/null  http://服务器ip/abcefg
501

$curl -I -s -w "%{http_code}\n" -o /dev/null  http://服务器ip/abc
501

$curl -I -s -w "%{http_code}\n" -o /dev/null  http://服务器ip/abc/efg
501
```

### 1.3.2.构建https server

生成私钥与证书

```sh
$openssl genpkey -algorithm RSA -out keya.pem -pkeyopt rsa_keygen_bits:4096
$openssl req -new -key keya.pem -out cert.csr  -utf8 -subj "/C=CN/ST=shanghai/L=shanghai/O=zzz/OU=zzz/CN=localhost"
$openssl x509 -req -days 3650 -in cert.csr  -signkey keya.pem -out newcertb.crt
```

配置nginx

```conf
    server {
        listen       443 ssl;
        server_name  localhost;

        ssl_certificate C:/tuxueqing/gotest/newcertb.crt;
        ssl_certificate_key C:/tuxueqing/gotest/keya.pem;
        ssl_session_cache   shared:mySSLShare:10m;     
        ...   
```

上面的例子使用的是私钥密码没有加密的方式，如果私钥密码加密了。那启动、重启时需要输入私钥密码。为了面输入，可以将私钥密码放置在“ssl_password_file”指令指示的文件中，具体细节参见 [Secure Distribution of SSL Private Keys with NGINX](https://www.nginx.com/blog/secure-distribution-ssl-private-keys-nginx/)

具体操作见下面演示

```sh
# 给私钥密码加密，命名keya_withsecure.pem
$openssl rsa -aes256 -in keya.pem -out keya_withsecure.pem
writing RSA key
Enter PEM pass phrase:  这里输入私钥加密密码如123456
Verifying - Enter PEM pass phrase:

#将私钥密码放入一个文件keyfile.txt
$ touch keyfile.txt
$ echo 123456 > keyfile.txt
$ cat keyfile.txt
123456
```

改写nginx.conf

```conf
    server {
        listen       443 ssl;
        server_name  localhost;
        
        ssl_certificate C:/tuxueqing/gotest/newcertb.crt;
        #更改私钥文件为私钥密码加密的keya_withsecure.pem
        ssl_certificate_key C:/tuxueqing/gotest/keya_withsecure.pem;
        #新增指示私钥加密密码文件
        ssl_password_file C:/tuxueqing/gotest/keyfile.txt;
        ssl_session_cache   shared:mySSLShare:10m;      
        ...  
```

最后reload nginx完成。


# 2.安装PHP

## 2.1.安装
```sh
$sudo apt-get install php7.2

#安装扩展,PHP-FPM(FastCGI Process Manager：FastCGI进程管理器)是一个PHPFastCGI管理器
$sudo apt-get install php7.2-fpm

#安装扩展
$sudo apt-get install php7.2-mysql php7.2-curl php7.2-json php7.2-cgi php7.2-xsl
#安装扩展
$sudo apt-get install php7.2-common php7.2-cli php7.2-mcrypt php7.2-mbstring php7.2-dom
$php -v
PHP 7.2.24-0ubuntu0.18.04.4 (cli) (built: Apr  8 2020 15:45:57) ( NTS )
...
```

## 2.2.启动
执行以下命令，启动PHP服务并设置开机自启动。

```sh
$sudo systemctl start php7.2-fpm.service
$sudo systemctl enable php7.2-fpm.service
#查看状态
$sudo systemctl status php7.2-fpm.service
* php7.2-fpm.service - The PHP 7.2 FastCGI Process Manager
   Loaded: loaded (/lib/systemd/system/php7.2-fpm.service; enabled; vendor preset: enabled)
   Active: active (running) since Wed 2020-05-27 22:46:34 CST; 45min ago
     Docs: man:php-fpm7.2(8)
 Main PID: 26819 (php-fpm7.2)
   Status: "Processes active: 0, idle: 2, Requests: 0, slow: 0, Traffic: 0req/sec"
    Tasks: 3 (limit: 2317)
   CGroup: /system.slice/php7.2-fpm.service
           |-26819 php-fpm: master process (/etc/php/7.2/fpm/php-fpm.conf)
           |-26837 php-fpm: pool www
           `-26838 php-fpm: pool www
...
```

## 2.3.记录php-fpm监听信息

我们查看下端口情况,从中可以看出php-fpm是在“/run/php/php7.2-fpm.sock”上面监听的。

```sh
atmel:/etc/php/7.2/fpm/pool.d$sudo netstat -apn
Active Internet connections (servers and established)
Proto Recv-Q Send-Q Local Address           Foreign Address         State       PID/Program name    
tcp        0      0 0.0.0.0:873             0.0.0.0:*               LISTEN      5551/rsync          
tcp        0      0 0.0.0.0:80              0.0.0.0:*               LISTEN      27920/nginx: master 
...
unix  2      [ ACC ]     STREAM     LISTENING     3592604  26819/php-fpm: mast  /run/php/php7.2-fpm.sock
...
unix  3      [ ]         STREAM     CONNECTED     261056   14180/upowerd        
unix  3      [ ]         STREAM     CONNECTED     3531320  5168/sshd: atmel [p  
unix  3      [ ]         STREAM     CONNECTED     3600819  27920/nginx: master  
atmel:/etc/php/7.2/fpm/pool.d$
```

这个监听设置信息，我们同时可以在“/etc/php/7.2/fpm/php-fpm.conf”中得到确认，在我本机上php-fpm.conf中有“include=/etc/php/7.2/fpm/pool.d/*.conf”指令，顺着这个查找得到：

```sh
atmel:/etc/php/7.2/fpm/pool.d$cat www.conf | grep listen
...
listen = /run/php/php7.2-fpm.sock
...
```

记住，这个监听设置信息很重要，在后面配置nginx时需要用到

## 2.4.同步nginx设置

本机安装的openresty，对应nginx配置文件在“/usr/local/openresty/nginx/conf/nginx.conf”.为了与php配合，做了以下改动

修改打开的“nginx.conf”文件。

- 在所支持的主页面格式中添加php格式的主页，如下所示：

    ```conf  
    location / {
        root   /usr/share/nginx/html;
        index index.php index.html index.htm;
    }
    ```

- 取消如下内容的注释，并设置字体加粗部分为Nginx的默认路径，如下图所示：
  
```conf  
location ~ \.php$ {
    root           html;
    fastcgi_pass   unix:/run/php/php7.2-fpm.sock;
    # 如果php是采用ip：port方式设置的监听，则应使用类似下面的方式
    #fastcgi_pass   127.0.0.1:9000;
    fastcgi_index  index.php;
    fastcgi_param  SCRIPT_FILENAME /usr/share/nginx/html$fastcgi_script_name;
    include        fastcgi_params;
}
```

- 重新加载nginx配置

```sh
$sudo systemctl reload openresty.service
```

- 访问测试

在nginx/html 下面放一个测试文件info.php

```php
<?php
 phpinfo();
?>
```

最后，使用浏览器访问“http://服务器IP地址/info.php”访问得到php的信息，说明搭建成功了。

## 2.5.phpfpm与nginx配置同步总结

一种是采用unix sock通信，一种是采用ip：port通信。推荐第二种方式，因为第一种方式在大流量情况下可能会有问题。

同时采用unix sock配置方式更复杂，因为涉及到权限问题。 如果配到权限问题，可以按照 [nginx-error-connect-to-php5-fpm-sock-failed-13-permission-denied](https://stackoverflow.com/questions/23443398/nginx-error-connect-to-php5-fpm-sock-failed-13-permission-denied)中指导来解决。


- 方式1，统一成ip+port的形式:

```conf
php-fpm.conf:  
listen = 127.0.0.1:9000 

nginx.conf:  
fastcgi_pass 127.0.0.1:9000;
```
- 方式2，统一成.sock的形式:

```conf
php-fpm.conf:  
listen = /run/php/php7.2-fpm.sock 

nginx.conf: 
fastcgi_pass unix:/run/php/php7.2-fpm.sock;
```

# 3.安装MySQL。

## 3.1.获取较新mysql包
注意：在ubuntu上mysqld –initialize ，要配置apparmor，这个可能有安全问题。因此在unbuntu上不要执行该指令

ubuntu默认库中的mysql是5.x版本，建议更新apt库安装较新版本。更新的话，首先从“https://dev.mysql.com/downloads/repo/apt/”下载apt-config，例如mysql-apt-config_0.8.15-1_all.deb，然后“sudo dpkg -i mysql-apt-config_0.8.15-1_all.deb”安装，最后“sudo apt-get update”

## 3.2.安装

依次执行以下命令，安装MySQL。在安装过程中会有一些交互问答，这个根据个人意愿回答即可。

```sh
sudo apt-get  -y install mysql-server
```

依次执行以下命令，启动MySQL服务并设置开机自启动。

```sh
$sudo systemctl start mysql

$sudo systemctl enable mysql
```

执行以下命令，并按照回显提示信息进行操作，加固MySQL。同时在这里设置mysql root的密码

```sh
sudo mysql_secure_installation
```

上述命令完成后，通过下面查看下mysql的状态. 下面显示的状态表明状态正常

```sh
:~$systemctl status mysql.service
* mysql.service - MySQL Community Server
   Loaded: loaded (/lib/systemd/system/mysql.service; enabled; vendor preset: enabled)
   Active: active (running) since Wed 2020-05-27 19:43:06 CST; 4min 28s ago
...
```


## 3.3创建数据库



创建数据库。
执行以下命令，并按照提示信息输入MySQL的root用户，登录到MySQL命令行。

```sh
$sudo mysql -u root -p
```

此时，可以查看数据的存储路径

```sql
mysql> show variables like 'datadir'; 
+---------------+-----------------+
| Variable_name | Value           |
+---------------+-----------------+
| datadir       | /var/lib/mysql/ |
+---------------+-----------------+
1 row in set (0.01 sec)

mysql> 
```

执行以下命令，创建一个新的数据库。

```sql
mysql> CREATE DATABASE wordpress;
Query OK, 1 row affected (0.01 sec)

mysql> show databases;
+--------------------+
| Database           |
+--------------------+
...
| wordpress          |

```

其中，“wordpress”为数据库名，可以自行设置。

## 3.4创建用户并赋远程权限

首先以root用户进入msql客户端

```sh
$sudo mysql -u root -p
```


### 3.4.1.方式1

执行以下sql命令，为数据库创建用户并为用户分配数据库的完全访问权限。

```sql
mysql>>GRANT ALL ON wordpress.* TO wordpressuser@localhost IDENTIFIED BY 'BLOck@123';
```

其中，“wordpressuser”为数据库用户名，“BLOck@123”为对应的帐户密码，可以自行设置。

执行以下命令，退出MySQL命令行。

```sql
exit
```

（可选）依次执行以下命令，验证数据库和用户是否已成功创建，并退出MySQL命令行。

```sh
mysql -u wordpressuser -p

SHOW DATABASES;

exit
```

其中，“wordpressuser”为刚刚创建的数据库用户名。


### 3.4.1.方式2

下面演示了分别创建数据库与用户，并演示了用户权限赋予。

执行以下sql命令，为创建msql用户

```sql

mysql> use mysql;
mysql> create user `wordpressuser`@localhost  identified by "abc@abc";
Query OK, 0 rows affected (0.00 sec)

mysql> mysql> select host ,user  from user;
+-----------+------------------+
| host      | user             |
+-----------+------------------+
...
| localhost | wordpressuser   |
+-----------+------------------+
6 rows in set (0.00 sec)
```

执行以下命令，为用户wordpressuser赋予数据库wordpress上的所有权限

```sql
mysql> GRANT all privileges ON wordpress to `wordpressuser`@`localhost`;
Query OK, 0 rows affected (0.00 sec)
```

# 4.安装vsftpd

通常服务器上的维护常用的是sftp协议进行上传、下载。但wordpress升级维护需要ftp协议的通信，因此这里安装下ftp服务器，采用vsftpd服务器。

## 4.1安装

```sh
#
$sudo apt-get install vsftpd
```

## 4.2配置vsftpd。

vsftpd安装后默认开启了匿名FTP的功能，使用匿名FTP，用户无需输入用户名密码即可登录FTP 服务器，但没有权限修改或上传文件。用户如果试图使用Linux操作系统中的账号登录服务器，将会被vsftpd拒绝，但可以在vsftpd里配置用户账号和密码登录。以下操作以在vsftpd 里配置用户账号和密码登录FTP服务器为例。

执行以下命令创建“ftpadmin”用户。此处“ftpadmin”用户为示例，可根据实际情况创建相应的用户。

```sh
$sudo useradd ftpadmin

#执行以下命令并按照提示设置“ftpadmin”用户密码。
$sudo passwd ftpadmin

#执行以下命令创建供FTP使用的文件目录，此处以“/var/ftp/work01”为例。
$sudo mkdir -p /var/ftp/work01

#执行以下命令将创建的文件目录所有者改为用于登录FTP的本地用户。
$sudo chown -R ftpadmin:ftpadmin /var/ftp/work01
```

下面开始修改“vsftpd.conf”配置文件。这个配置文件在什么地方，可以通过“dpkg -L vsftpd”的结果查看得到，也可以直接通过“locate vsftpd.conf”查找得到。

```sh
#执行以下命令打开配置文件“vsftpd.conf”。
$sudo vi /etc/vsftpd.conf
```
按i键进入编辑模式。修改打开的“vsftpd.conf”文件。

可根据实际需求选择将FTP配置为主动模式或者被动模式。

- 主动模式需要配置的参数如下：

```conf
#设置以下参数，不允许匿名登录FTP服务器，允许本地用户登录FTP服务器，并指定FTP本地用户使用的文件目录。
anonymous_enable=NO              #不允许匿名登录FTP服务器
local_enable=YES                 #允许本地用户登录FTP服务器
local_root=/var/ftp/work01       #FTP本地用户使用的文件目录

#设置以下参数，限制用户只能访问自身的主目录。
chroot_local_user=YES                      #所有用户都被限制在其主目录
chroot_list_enable=YES                     #启用例外用户名单
chroot_list_file=/etc/vsftpd.chroot_list   #例外用户名单
allow_writeable_chroot=YES                 
```

- 被动模式需要配置的参数：

被动模式除了需要配置主动模式所需的所有参数外，还需要配置的参数如下：

```conf
#设置以下参数，配置FTP支持被动模式。并指定FTP服务器的公网IP地址，以及可供访问的端口范围，端口范围请根据实际环境进行设置。
listen=YES                    
listen_ipv6=NO                
pasv_address=xx.xx.xx.xx      #FTP服务器的公网IP地址
           
pasv_min_port=3000            #被动模式下的最小端口
pasv_max_port=3100            #被动模式下的最大端口
```

根据配置文件中的chroot_list_file的值，新建对应文件

```sh
$sudo touch  /etc/vsftpd.chroot_list
```

“vsftpd.chroot_list”文件是限制在主目录下的例外用户名单。如果需要设置某个用户不受只可以访问其主目录的限制，可将对应的用户名写入该文件。如果没有例外也必须要有“vsftpd.chroot_list”文件，内容可为空。

- 开启上传

将“write_enable=YES”前面的#取消。

```conf
# Uncomment this to enable any form of FTP write command.
write_enable=YES
```

最后重启vsftpd，使得配置修改生效

```sh
$sudo systemctl restart vsftpd.service
```


## 4.3.vsftpd常用运维

```sh
#执行以下命令设置FTP服务开机自启动。
$sudo systemctl enable vsftpd.service
#执行以下命令启动FTP服务。
$sudo systemctl start vsftpd.service

$sudo systemctl restart vsftpd.service

#执行以下命令查看FTP服务端口。
$sudo netstat -antup | grep ftp
```

## 4.4.命令行模拟PASV

记录模拟ftp协议的PASV模式，该部分内容与安装ftp无关，只是为了更好理解ftp的pasv模式。

准备：

- 在ftp服务器上创建一个文件efgh，它的内容是“abc”

- 在客户端，打开两个终端窗口，一个是A窗口，作为命令窗口。一个是B窗口，作为数据窗口。

### 4.4.1.PASV下载

A窗口运行“telnet 服务器ip 21”, 如下图所示，分别输入“user 用户名”，“pass  密码”，最后再输入“pasv”

- 执行user命令是告知ftp用户名
- 执行pass命令是告知该ftp用户的密码
- 执行PASV命令是告诉服务器端要使用PASV模式传输数据。服务器端会返回数据端口的端口号。

```sh
$telnet 服务器ip 21
Trying *.*.*.*...
Connected to *.*.*.*.
Escape character is '^]'.
220 (vsFTPd 3.0.3)
user ******
331 Please specify the password.
pass ******
230 Login successful.
pasv
227 Entering Passive Mode (*.*.*.* ,20,52).
```

最终得到响应“227 Entering Passive Mode (*.*.*.* ,20,52)”。最后两个数字"20,52"是端口号，即端口号是5172，因为“20*256+52”的结果是5172


在B窗口使用上一步在A窗口得到的端口号5172，运行“telnet 服务器ip 5172”，如下图所示

```sh
$telnet *.*.*.* 5172
Trying *.*.*.*...
Connected to *.*.*.*.
Escape character is '^]'.

```

我们回到A窗口，继续输入“RETR  efgh”，该命令是表示从ftp服务器下载efgh文件。A窗口此时的界面显示是

```sh
$telnet 服务器ip 21
Trying *.*.*.*...
Connected to *.*.*.*.
Escape character is '^]'.
220 (vsFTPd 3.0.3)
user ******
331 Please specify the password.
pass ******
230 Login successful.
pasv
227 Entering Passive Mode (*.*.*.* ,20,52).
RETR efgh
150 Opening BINARY mode data connection for efgh (4 bytes).
226 Transfer complete.
```

此时观察B窗口看到的结果如下，其中“abc”就是我们在前面放在服务器上的文件efgh的内容

```sh
$telnet *.*.*.* 5172
Trying *.*.*.*...
Connected to *.*.*.*.
Escape character is '^]'.
abc
Connection closed by foreign host.
```

### 4.4.1.PASV上传

A窗口运行“telnet 服务器ip 21”, 如下图所示，分别输入：

- 执行user命令是告知ftp用户名
- 执行pass命令是告知该ftp用户的密码
- type i ，指示使用二进制传输
- 执行PASV命令是告诉服务器端要使用PASV模式传输数据。服务器端会返回数据端口的端口号。
- 执行"stor upload.txt",告知服务端准备上传文件了，文件名是upload.txt

```sh
$telnet 服务器ip 21
Trying *.*.*.*...
Connected to *.*.*.*.
Escape character is '^]'.
user ******
331 Please specify the password.
pass ******
230 Login successful.
pasv
227 Entering Passive Mode (*.*.*.* 22,243).
stor upload.txt
150 Ok to send data.
···
```

此时我们登录ftp服务器，发现在“/var/ftp/work”已经有个名字是“upload.txt”的文件，但文件大小为0，是空的。

在B窗口使用上一步在A窗口得到的端口号5875（=22*256+243），运行“telnet 服务器ip 5875”，如下图所示

```sh
$telnet *.*.*.* 5875
Trying *.*.*.*...
Connected to *.*.*.*.
Escape character is '^]'.

```

然后在这个B窗口手工输入“can you see？”
```sh
$telnet *.*.*.* 5875
Trying *.*.*.*...
Connected to *.*.*.*.
Escape character is '^]'.
can you see？

```

这时发现在ftp服务器，“/var/ftp/work/upload.txt”的内容变了. 这说明数据传递过去了。

```sh
:/var/ftp/work$cat upload.txt
can you see？
```

上面我们是手工模拟数据上传，如果编程时应该是读取出upload.txt的内容，然后通过数据连接（B窗口建立的数据连接）将内容发送到服务器去。


## 4.5. 典型客户端ftp-sftp工具

注意：spftp不是ftp，sftp是“Secure File Transfer Protocol）：安全文件传送协议，虽然它有着和ftp几乎一样的语法与功能，但他们是完全不同的两个协议，比如sftp协议中不存在PORT模式，PASV模式。

典型的ftp协议服务器是vsftpd，典型的sftp协议服务器是sshd。

### 4.5.1.powershell

在windows，cmd下的ftp是不支持pasv模式的，只有powershell中的ftp支持pasv模式。下面演示了在pasv模式下上传

```sh
PS C:\Users\atmel> ftp 服务器ip
连接到 *.*.*.*。
220 (vsFTPd 3.0.3)
用户(*.*.*.*:(none)): xxxx
331 Please specify the password.
密码:
230 Login successful.
ftp> help
命令可能是缩写的。  命令为:

!               delete          literal         prompt          send
?               debug           ls              put             status
append          dir             mdelete         pwd             trace
ascii           disconnect      mdir            quit            type
bell            get             mget            quote           user
binary          glob            mkdir           recv            verbose
bye             hash            mls             remotehelp
cd              help            mput            rename
close           lcd             open            rmdir
ftp> lcd c:\pythondemo
目前的本地目录 C:\pythondemo。
ftp> help literal
literal         发送任意 ftp 命令
ftp> literal pasv
227 Entering Passive Mode (124,71,114,110,20,191).
ftp> put upload.txt
200 PORT command successful. Consider using PASV.
150 Ok to send data.
226 Transfer complete.
ftp: 发送 6 字节，用时 0.08秒 0.08千字节/秒。
ftp>
```

### 4.5.2.msys2-ftp

msys2作为一个mingw bundle，其中的ftp是支持passive模式的。使用方式和linux没有区别，这里不具体操作了。

```sh
MINGW64 ~$ftp
ftp> help
Commands may be abbreviated.  Commands are:

!               dir             macdef          proxy           site
$               disconnect      mdelete         sendport        size
account         epsv4           mdir            put             status
append          form            mget            pwd             struct
ascii           get             mkdir           quit            system
bell            glob            mls             quote           sunique
binary          hash            mode            recv            tenex
bye             help            modtime         reget           trace
case            idle            mput            rstatus         type
cd              image           newer           rhelp           user
cdup            ipany           nmap            rename          umask
chmod           ipv4            nlist           reset           verbose
close           ipv6            ntrans          restart         ?
cr              lcd             open            rmdir
delete          lpwd            passive         runique
debug           ls              prompt          send
ftp>
```

### 4.5.2.secureCRT

secureCRT作为登录远程主机的工具，同时提供了sftp能力。在已经登录远程主机情况下， 运行“file-->connect sftp session”后将会出现sftp session标签页，在其中就可以进行ftp相关操作了。 这个操作还有个快捷方式：在当前登录session中按“alt+p”就直接出现sftp session。两种方式效果是一样。

以下是sftp的帮助，其中最常用的几个命令的翻译见中文部分：

```sh
sftp> help
Available commands:
ascii                          Set transfer mode to ASCII
binary                         Set transfer mode to binary
cd path                        改变远程上传目录;Change remote directory to 'path'
lcd path                       改变本地上传目录的路径;Change local directory to 'path'
detail remote-path             Display system information about remote
                                 file or folder
ldetail local-path             Display system information about local
                                 file or folder
chgrp group path               Change group of file 'path' to 'group'
chmod mode path                Change permissions of file 'path' to 'mode'
chown owner path               Change owner of file 'path' to 'owner'
exit                           Quit sftp
help                           Display this help text
include filename               Include commands from 'filename'
                                 Alternate: < filename
get [-r][-a | -b] remote-path  将远程目录中文件下载到本地目录;Download file
                                 -r downloads directory recursively
                                 force ascii (-a) or binary (-b) mode
ln [-s] existingpath linkpath  Hardlink / symlink remote file
ls [options] [path]            查询连接到当前linux主机所在目录有哪些文件;Display remote directory listing
lls [options] [path]           查询当前本地上传目录有哪些文件;Display local directory listing
mkdir path                     Create remote directory
lmkdir path                    Create local directory
mv oldpath newpath             Move remote file
lmv oldpath newpath            Move local file
open [user@]host[:port]        Connect to remote host
put [-r][-a | -b] local-path   将本地目录中文件上传到远程主机;Upload file
                                 -r uploads directory recursively
                                 force ascii (-a) or binary (-b) mode
pwd:                           查询linux主机所在目录(也就是远程主机目录) ;Display remote working directory
lpwd:                          查询本地目录;Print local working directory
quit                           Quit sftp
rename oldname newname         Rename remote file
lrename oldname newname        Rename local file
rmdir path                     Remove remote directory
lrmdir path                    Remove local directory
rm path                        Delete remote file
lrm path                       Delete local file
su username                    Substitutes the current user
                                 This is only supported with VShell for 
                                 Windows 3.5 or later.
type [transfer-mode]           Display or set file transfer mode
view remote-path               Download and open file
version                        Display protocol version
```

运行sftp客户端，客户端本地的默认目录的正确设置常常能增加我们使用过程中的便利性。要修改这个默认设置，通过“sftp session options-->ssh2-->sftp session-->initial directories-->local directory”进行设置。

不过从安全角度来说，建议还是每次使用时，通过lpwd、pwd指令核实本地当前目录与远程主机当前目录会更好。

### 4.5.2.scp

scp、winscp是最常用的sftp协议客户端。比如你安装了git bash，那就会自动带着scp。比如安装了msys2，并安装了openssh（通过`pacman -S openssh`）,也会有scp。

```sh
~$pacman -Ss openssh
msys/openssh 8.0p1-1 (net-utils)
    Free version of the SSH connectivity tools
...
:: 进行安装吗？ [Y/n] y
:: 正在获取软件包......
 heimdal-7.5.0-3-x86_64   428.3 KiB   987K/s 00:00 [#####################] 100%
 openssh-8.0p1-1-x86_64   705.9 KiB   513K/s 00:01 [#####################] 100%
...
(2/2) 正在安装 openssh                             [#####################] 100%

~$which scp
/usr/bin/scp

~$pacman -Ql openssh
...
openssh /usr/bin/findssl.sh
openssh /usr/bin/scp.exe
openssh /usr/bin/sftp.exe
openssh /usr/bin/ssh-add.exe
openssh /usr/bin/ssh-agent.exe
openssh /usr/bin/ssh-copy-id
openssh /usr/bin/ssh-keygen.exe
openssh /usr/bin/ssh-keyscan.exe
openssh /usr/bin/ssh.exe
openssh /usr/bin/sshd.exe
...
openssh /usr/lib/ssh/sftp-server.exe
openssh /usr/lib/ssh/ssh-keysign.exe
openssh /usr/lib/ssh/ssh-pkcs11-helper.exe
...
```


另外在windows下也可以单独winscp，它功能与scp是一样的。

# 4.安装wordpress


请自行获取WordPress软件包并上传至“/usr/share/nginx/html”目录。
后续操作软件包以“wordpress-5.3.2.tar.gz”为例。

```sh
#执行以下命令，解压缩软件包。解压后生成一个“wordpress”的文件夹。
/usr/share/nginx/html/$tar -zxvf wordpress-5.3.2.tar.gz

#执行以下命令，设置解压后的文件权限。今后实验下不执行它验证是否ok
/usr/share/nginx/html/$chmod -R 777 wordpress

#由于php-fpm是以www-data用户运行的，所以这里将owner修改下，使得后面类似升级场景www-data用户能操纵它
/usr/share/nginx/html/$sudo chown -R www-data:www-data wordpress
```

浏览器访问“http://服务器IP地址/wordpress”进入安装向导。

按照界面信息提示准备好数据库的信息，并单击“Let's go”。  这里数据的地址就填写默认的localhost，不要填写ip。 其他数据库信息填写在[安装mysql](#3安装mysql)中记录的信息。

