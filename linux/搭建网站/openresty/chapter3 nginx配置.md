

书籍内容见《实战Nginx.取代Apache的高性能Web服务》 第三章

跟Apache 一样， Nginx 也可以配置多种类型的虚拟主机： 一是基于IP 的虚拟主机，二是基于域名的虚拟主机，三是基于端口的虚拟主机。一个虚拟主机对应配置文件中的一个server{}


- 对“http {...}”中可以用的指令见原书“13.1 HTTP的核心模块”章节

- 测试配置文件：当修改了配置文件，建议首先使用“-t、-T”测试下配置文件

```sh
atmel@mail:~$ sudo /usr/local/openresty/nginx/sbin/nginx -t -c /usr/local/openresty/nginx/conf/nginx.conf
[sudo] password for atmel: 
nginx: the configuration file /usr/local/openresty/nginx/conf/nginx.conf syntax is ok
nginx: configuration file /usr/local/openresty/nginx/conf/nginx.conf test is successful
atmel@mail:~$ 
```

```sh
atmel@mail:~$ /usr/local/openresty/nginx/sbin/nginx -h
nginx version: openresty/1.15.8.3
Usage: nginx [-?hvVtTq] [-s signal] [-c filename] [-p prefix] [-g directives]

Options:
  -?,-h         : this help
  -v            : show version and exit
  -V            : show version and configure options then exit
  -t            : test configuration and exit
  -T            : test configuration, dump it and exit
  -q            : suppress non-error messages during configuration testing
  -s signal     : send signal to a master process: stop, quit, reopen, reload
  -p prefix     : set prefix path (default: /usr/local/openresty/nginx/)
  -c filename   : set configuration file (default: conf/nginx.conf)
  -g directives : set global directives out of configuration file
```

# 典型配置文件
```conf

# 使用的用户和组，用户和组之间用空格隔开，如果不设置这表示不限制
#user  nobody;

# 指定工作进程数量，一般设置为cpu总核数量的2倍，例如四核cpu这设置8
#usually equal to number of CPUs you have. run command 
#"grep processor /proc/cpuinfo | wc -l" to find it
worker_processes  8;
worker_cpu_affinity auto;

# 指定错误日志存放路径，错误日志的记录级别可选：[debug|info|notice|warn|error|crit]
#error_log  logs/error.log;
#error_log  logs/error.log  notice;
#error_log  logs/error.log  info;

# 指定pid存放路径
#pid        logs/nginx.pid;

# 指定文件描述符数量
#work_rlimit_nofile 51200

events {
    # 使用的网络I/O类型，LINUX推荐使用epool，freebsd推荐kqueue
    use epoll;
    # 允许连接数
    worker_connections  1024;
}


http {
    include       mime.types;
    default_type  application/octet-stream;

    # 设置使用的字符集，如果一个网站有多种字符集，不要随便设置，应让程序员在html代码中通过meta设置
    ##charset  gb2312

    # 这个将为打开文件指定缓存，默认是没有启用的，max 指定缓存数量，
    # 建议和打开文件数一致，inactive 是指经过多长时间文件没被请求后删除缓存。
    ##open_file_cache max=204800 inactive=20s;

    # open_file_cache 指令中的inactive 参数时间内文件的最少使用次数，
    # 如果超过这个数字，文件描述符一直是在缓存中打开的，如上例，如果有一个
    # 文件在inactive 时间内一次没被使用，它将被移除。
    ##open_file_cache_min_uses 1;
    
    # 这个是指多长时间检查一次缓存的有效信息
    ##open_file_cache_valid 30s;

    # 与日志相关的指令主要两条：log_format与access_log。它们的位置可在http{...},也可以在server{...}

    #log_format指令格式是“log_format name format [format ...]” .log_format指令有
    #个默认无需设置的combined日志格式设置。注意name是不能重复的
    #log_format  main  '$remote_addr - $remote_user [$time_local] "$request" '
    #                  '$status $body_bytes_sent "$http_referer" '
    #                  '"$http_user_agent" "$http_x_forwarded_for"';

    # access_log指令指定日志文件存放路径 “access_log path [formatname [buffer-size | off]]”
    # 其中path 表示日志文件的存放路径， formatname 表示使用log_format 指令设置的日志格式的名称，
    # 如果不想记录日志可以使用"access_log off"关闭日志记录
    # buffer=size 表示设置内存缓冲区的大小
    #access_log  logs/access.log  main;

    # 当打开日志后，用来设置经常被使用的日志文件描述符缓存
    # open_log_file_cache max=N [inactive=time] [min_uses=N] [valid=time] | off
    #open_log_file_cache max=lOOO inactive=20s min_uses=2 valid:lm;

    # 设置客户端能够上传的最大文件大小
    client_max_body_size   1024m;

    sendfile        on;
    #tcp_nopush     on;

    #keepalive_timeout  0;
    keepalive_timeout  65;

    # 开启gzip压缩
    ##gzip  on;
    ##gzip_min_length 1k;
    ##gzip_http_version 1.1;
    ##gzip_comp_level 2;
    ##gzip_types  text/plain application/x-javascript text/css application/xml;
    ##gzip_vary on;

    # 语法： expires [time|epoch|max|off], 作用域： http. server, location
    # 使用本指令可以控制Hπp 应答中的"Expires"和“ Cache-Control ”的Header 头信息（起到控制页面缓存的作用）
    #expires 30d;

    # 虚拟主机配置
    server {
        #监听的IP与端口 例如 192.168.0.1:80
        listen       8989;
        #主机名称列表，以空格分开。指令见“13.1.42 server name 指令”章节，它如果是正则表达式，这是以“~”开头的
        server_name  localhost;

        #关闭（显示）Nginx 版本号。
        server_tokens off;      

        #charset koi8-r;
        #访问日志文件存放路径
        #access_log  logs/host.access.log  main;
        
        # 自动列目录功能，前提条件是当前目录下不存在用index 指令设置的默认首页文件。如果须
        # 要在某一虚拟主机的location /｛……｝目录控制中配置自动列目录，需要配置autoindex on;
        location / {
            # 只允许内部调用,例如对数据库，对内部函数api接口
            #internal;

           # 基于根目录"/path/to/app_a/static"的，也就是说"/"指的就是"/path/to/app_a/static/"，
           # 而"/images/"指的就是"/path/to/app_a/static/images/"
           # 例如，如果我们的URI是/images/some/path/abc.html，那么nginx返回的文件就是
           # “/path/to/app_a/static/images/some/path/abc.html”
            root   /path/to/app_a/static;
            # 开启自动列表目录
            autoindex on;
            # 设定索引时文件大小的单位(B 、KB 、MB 或GB)
            autoindex_exact_size off;
            # 开启以本地时间来显示文件时间的功能。默认为关（ GMT 时间）。
            autoindex_localtime on;
            charset utf-8;
            #index  index.html index.htm;

            # 采用基于HTIP 基本身份验证的用户名和密码登录方式，来保护你的虚拟主机或目录。
            # auth_basic该指令用于指定弹出的用户名和密码登录框中提示的名称
            auth_basic "home";
            #auth_basic_user_file指令用于设置htpasswd 密码文件的名称
            auth_basic_user_file httppasswd;
        }

        #error_page  404              /404.html;

        # redirect server error pages to the static page /50x.html
        #
        error_page   500 502 503 504  /50x.html;
        location = /50x.html {
            root   html;
        }
    }

    server {
        listen       8080;
        server_name  localhost;

        #charset koi8-r;

        #access_log  logs/host.access.log  main;

        # 语法： location [=|~|~*|^~]   /uri/ { ... }
        # 对待匹配的“uri”，可能是字符串，也可能是正则。
        # "~"表示区分大小写的匹配，"~*"表示不区分大小写的匹配。先匹配字符串再匹配正则
        # "="表示进行精确匹配，如果找到匹配的uri这停止查询
        # "^~"表示禁止匹配到字符串后，再去检查正则
        location / {
            alias   /mnt/toshiba;
            charset utf-8;
            index  index.html index.htm;
        }

        #error_page  404              /404.html;

        # redirect server error pages to the static page /50x.html
        #
        error_page   500 502 503 504  /50x.html;
        location = /50x.html {

            # html网页文件存放的目录
            root   /data0/htdocs/html;
        }

        # proxy the PHP scripts to Apache listening on 127.0.0.1:80
        #
        #location ~ \.php$ {
        #    proxy_pass   http://127.0.0.1;
        #}

        # pass the PHP scripts to FastCGI server listening on 127.0.0.1:9000
        #
        #location ~ \.php$ {
        #    root           html;
        #    fastcgi_pass   127.0.0.1:9000;
        #    fastcgi_index  index.php;
        #    fastcgi_param  SCRIPT_FILENAME  /scripts$fastcgi_script_name;
        #    include        fastcgi_params;
        #}

        # deny access to .htaccess files, if Apache's document root
        # concurs with nginx's one
        #
        #location ~ /\.ht {
        #    deny  all;
        #}
    }


    # another virtual host using mix of IP-, name-, and port-based configuration
    #
    #server {
    #    listen       8000;
    #    listen       somename:8080;
    #    server_name  somename  alias  another.alias;

    #    location / {
    #        root   html;
             # 默认首页文件，顺序从左到右，如果找不到index.html，这尝试查找index.htm作为首页文件
    #        index  index.html index.htm;
    #    }
    #}


    # HTTPS server
    #
    #server {
    #    listen       443 ssl;
    #    server_name  localhost;

    #    ssl_certificate      cert.pem;
    #    ssl_certificate_key  cert.key;

    #    ssl_session_cache    shared:SSL:1m;
    #    ssl_session_timeout  5m;

    #    ssl_ciphers  HIGH:!aNULL:!MD5;
    #    ssl_prefer_server_ciphers  on;

    #    location / {
    #        root   html;
    #        index  index.html index.htm;
    #    }
    #}

}

```

## location指令说明

```conf
 # 语法： location [=|~|~*|^~]   /uri/ { ... }
 # 对待匹配的“uri”，可能是字符串，也可能是正则。

 # 修饰符的解释：
 # "~"表示区分大小写的匹配，"~*"表示不区分大小写的匹配。先匹配字符串再匹配正则
 # "="表示进行精确匹配，如果找到匹配的uri这停止查询
 # "^~"表示禁止匹配到字符串后，再去检查正则

#修饰符补充解释
#完全匹配(=)
#无正则的普通匹配(^~)（^ 表示“非”，~ 表示“正则”，字符意思是：不要继续匹配正则）,就是一种前缀匹配
#正则表达式匹配(~或者~*)
#不带任何修饰符代表普通匹配，也表示前缀匹配，

```
匹配顺序

- 首先精确匹配 =
- 其次前缀匹配 `^~`
- 其次是按文件中顺序的正则匹配
- 然后匹配不带任何修饰的前缀匹配。
- 最后是交给 `/` 通用匹配
- 当有匹配成功时候，停止匹配，按当前匹配规则处理请求

基于商品的匹配顺序，建议配置文件中按照下面的顺序书写

```conf
# 直接匹配网站根，通过域名访问网站首页比较频繁,可以起到加速效果
location = / {...}

# 第二个必选规则是处理静态文件
location ^~ /static/ {...} #前缀匹配
location ~* \.(gif|jpg|jpeg|png|css|js|ico)$ {...} #后缀匹配

## 第三个规则就是通用规则，用来转发动态请求到后端应用服务器
location / {...}
```


# 举例

## 

## Nginx指定路径时，root与alias区别在哪？

root与alias路径匹配主要区别在于nginx如何解释location后面的uri，这会使两者分别以不同的方式将请求映射到服务器文件上，alias是一个目录别名的定义，root则是最上层目录的定义。

- root的处理结果是：root路径＋location路径

- alias的处理结果是：使用alias路径替换location路径

1.root路径配置实例: 用户访问www.xuliangwei.com/image/test.gif，实际上Nginx会上/code/image/目录下找去找test.gif文件

```conf
server {
 listen 80;
 server_name www.xuliangwei.com;
 location /image/ {
 root /code;
 }
}
```

2.alias配置实例: 用户访问www.xuliangwei.com/image/test.gif，实际上Nginx会上/code/目录下找去找test.gif文件。

```conf
server {
 listen 80;
 server_name www.xuliangwei.com;
 location /image/ {
 alias /code;
 }
}
```

## 采用基于HTIP 基本身份验证登录虚拟主机举例

例如

```conf
    location / {
        root   /mnt;
        # 开启自动列表目录
        autoindex on;
        # 设定索引时文件大小的单位(B 、KB 、MB 或GB)
        autoindex_exact_size off;
        # 开启以本地时间来显示文件时间的功能。默认为关（ GMT 时间）。
        autoindex_localtime on;
        charset utf-8;
        #index  index.html index.htm;
        # 采用基于HTIP 基本身份验证的用户名和密码登录方式，来保护你的虚拟主机或目录。
        # auth_basic该指令用于指定弹出的用户名和密码登录框中提示的名称
        auth_basic "home";
        #auth_basic_user_file指令用于设置htpasswd 密码文件的名称
        auth_basic_user_file httppasswd;
    }
```

其中密码文件采用每行都是“用户名:加密密码”的方式记录，密码可以采用下面的指令生成：

- 首先生成pw.pl 脚本，该脚本以密码明文为输入得到加密密码

```sh
#!/usr/bin/perl
#
use strict;
my $pw=$ARGV[0];

print crypt($pw,$pw)."\n";
```

- 按照下面的方式得到加密密码

"pw.pl" [New] 6L, 75C written                       
```sh
$ chmod +x pw.pl 
$ ./pw.pl testpasswd
tek4edTZE898g
$
```


## location无正则普通匹配(^~)

同时表明该规则优先级高于“~”正则规则

配置：

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


## 基于IP配置多个虚拟主机

```conf

...

http {

    ....

    # 虚拟主机A配置
    server {
        listen       192.168.0.1:80;
        server_name  192.168.0.1;

        location / {
            root   /mnt/html;
            index  index.html index.htm;
        }
    }
    # 虚拟主机B配置
    server {
        listen       192.168.0.2:80;
        server_name  192.168.0.2;

        location / {
            root   /mnt/toshiba/html;
            index  index.html index.htm;
        }
    }
    # 虚拟主机C配置
    server {
        listen       192.168.0.3:80;
        server_name  192.168.0.3;

        location / {
            root   /mnt/tohonda/html;
            index  index.html index.htm;
        }
    }


}

```

## 基于域名配置多个虚拟主机

```conf

...

http {

    ....

    # 虚拟主机A配置
    server {
        listen       80;
        server_name  www.aaa.yourdomain.com;

        location / {
            root   /mnt/html;
            index  index.html index.htm;
        }
    }
    # 虚拟主机B配置
    server {
        listen       80;
        server_name  www.bbb.yourdomain.com;

        location / {
            root   /mnt/toshiba/html;
            index  index.html index.htm;
        }
    }
    # 虚拟主机C配置
    server {
        listen       80;
        server_name  www.ccc.yourdomain.com;

        location / {
            root   /mnt/tohonda/html;
            index  index.html index.htm;
        }
    }


}

```

## 构建https server

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