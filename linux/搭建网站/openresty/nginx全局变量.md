

# 1.nginx与lua交互

lua语言相关资源：- 官方手册[Lua 5.3 Reference Manual](http://www.lua.org/manual/5.3/)， [中文手册](https://cloudwu.github.io/lua53doc/manual.html)



## 1.1-变量交互

以下面配置为例

```conf
        location ^~ /test/ {
            root  html;

            # 演示nginx如何操作变量
            set $a hello;  
            set $b $request_filename;  
            set $c "$a, $b"; 

            #content_by_lua_block表示{}内的代码，全部是lua代码,
            content_by_lua_block {
                ngx.say("")
                
                -- 演示了lua中怎么获取得到nginx的变量
                local arg = ngx.req.get_uri_args()
                for k,v in pairs(arg) do
                  ngx.say("[GET ] key:", k, " v:", v)
                end              
                
                -- 演示如果获取nginx内置变量  
                ngx.say("args：",ngx.var.args)
                ngx.say("host：", ngx.var.host, " server_port:",ngx.var.server_port," ;request_uri:",ngx.var.request_uri)
                ngx.say("document_uri：", ngx.var.document_uri," ;document_root:",ngx.var.document_root," ;request_filename:",ngx.var.request_filename)
                
                -- 演示如何获取nginx本地变量
                ngx.say("local c:", ngx.var.c) 
            }            
        }
```

测试结果如下：

```shell
$ curl 'localhost/test/efg/a?x=3&y=4'
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
100   281    0   281    0     0   1260      0 --:--:-- --:--:-- --:--:--  1265
[GET ] key:y v:4
[GET ] key:x v:3
args：x=3&y=4
host：localhost server_port:80 ;request_uri:/test/efg/a?x=3&y=4
document_uri：/test/efg/a ;document_root:c:/prog/openresty/html ;request_filename:c:/prog/openresty/html/test/efg/a
local c:hello, c:/prog/openresty/html/test/efg/a
```

以上测试表明一下几点：

- nginx.conf中插入lua代码采用`content_by_lua_block {...}`方式，lua代码的注释是“-”单行注释，“–[]” 多行注释，nginx的注释是“#”
- nginx变量是类似`$server_port`;lua访问nginx变量采用`ngx.var.server_port`
- nginx变量定义赋值是类似`set $a hello`,lua中变量定义赋值是类似`local arg = ...`

## 1.2-nginx引入lua代码

下面介绍通过lua脚本文件在nginx.conf中引入lua代码。

access_by_lua与content_by_lua都是调用lua脚本文件，但二者是nginx对于请求的不同处理阶段。

- access_by_lua在请求访问阶段处理，用于访问控制，适用于http、server、location、location if。

- content_by_lua是内容处理器，接受请求并输出响应，适用于location、location if。

在上一节中，演示了直接在`nginx.conf`文件中写inline lua代码方式，下面演示将lua代码单独分离开的写法

新建一个单独的lua文件test.lua

```lua
ngx.say("")
ngx.say("来自lua代码")
-- 演示了lua中怎么获取得到nginx的变量
local arg = ngx.req.get_uri_args()
for k,v in pairs(arg) do
  ngx.say("[GET ] key:", k, " v:", v)
end
           
-- 演示如果获取nginx内置变量  
ngx.say("args：",ngx.var.args)
ngx.say("host：", ngx.var.host, " server_port:",ngx.var.server_port," ;request_uri:",ngx.var.request_uri)
ngx.say("document_uri：", ngx.var.document_uri," ;document_root:",ngx.var.document_root," ;request_filename:",ngx.var.request_filename)
-- 演示如何获取nginx本地变量
ngx.say("local c:", ngx.var.c)
```

通过`content_by_lua_file`引入lua文件，该文件中代码将会被调用执行

```conf
location ^~ /test/ {
    root  html;

    # 演示nginx如何操作变量
    set $a hello;  
    set $b $request_filename;  
    set $c "$a, $b"; 
    # 执行lua代码
    content_by_lua_file C:/prog/openresty/conf/test.lua;
}
```

最后执行`nginx -s reload`使得配置修改生效，我们将发现效果和在nginx.conf中写inline lua code效果是一样的。这里需要提醒一点就是，如果test.lua文件内容发生变化，还是需要`nginx -s reload`，才会使得修改生效的。

## 1.3-写lua模块

require用于使用模块，module用于创建模块。

例如创建一个testmodule.lua模块：

```lua
local _mymodule = {}   -- 局部变量，模块名称

function _mymodule.xyz(x,y,z)
    return ...
end

return _mymodule
```

下面是一个使用上面创建模块的lua文件 main.lua

```lua
local m = require "testmodule"
local t1 = m.xyz(0, 1,2)
```

## 1.4-调试TODO

[使用idea调试lua代码-Openresty](https://segmentfault.com/a/1190000018430640)

# 2.nginx全局变量

例子
```shell
$curl 'localhost/test/efg/a?x=3&y=4'
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
100   240    0   240    0     0   1132      0 --:--:-- --:--:-- --:--:--  1132

args：x=3&y=4
[GET ] key:y v:4
[GET ] key:x v:3
$host：localhost;
$server_port:80;
$request_uri:/test/efg/a?x=3&y=4
$document_uri：/test/efg/a;
$document_root:c:/prog/openresty/html;
$request_filename:c:/prog/openresty/html/test/efg/a
```


### $uri
current URI in request, normalized.
The value of $uri may change during request processing, e.g. when doing internal redirects, or when using index files.
当前请求的URI。

### $arg_name
argument name in the request line.
请求URL中的参数名称。

### $args
arguments in the request line.
请求URL中包含的参数。

### $binary_remote_addr
client address in a binary form,value’s length is always 4 bytes for IPv4 addresses or 16 bytes for IPv6 addresses.
二进制形式的客户端地址，ipv4通常是4比特长度，ipv6通常为16比特长度。

### $body_bytes_sent
number of bytes sent to a client,not counting the response header;this variable is compatible withe the “%B” parameter of the mod_log_config Apache module.
发送给客户端的不包含响应头的字节数；这个变量兼容Apache的mod_log_config模块的“%B”参数。

### $bytes_sent
number of bytes sent to a client(1.3.8,1.2.5).
发送给客户端的字节数。

### $connection
connection serial number(1.3.8,1.2.5).
连接序列号。

### $connection_requests
current number of requests made through a connection(1.3.8,1.2.5).
当前通过连接发出的请求数。

### $content_length
“Content-Length” request header field.
请求头中的Content-Length字段。

### $content_type
“Content-Type” request header field.
请求头中的Content-Type字段。

### $cookie_name
the name cookie.
cookie的名称。

### $document_root
root or alias directive’s value for the current request.
当前请求的根目录或别名的值。 它是由root指令设置的。

### $document_uri
same as $uri.
与$uri全局变量相同。


### $host
in this order of precedence:host name from the request line, or host name from the “Host” request header field, or the server name matching a request.
请求URL中的主机名字段，或请求头中的Host字段，或匹配请求的server_name（nginx配置）。

host是HTTP 1.1协议中新增的一个请求头字段，存放服务器端域名:端口。能够很好的解决一个ip地址对应多个域名的问题。当服务器接收到来自浏览器的请求时，会根据请求头中的host字段访问哪个站点。

举个栗子，我有一台服务器A ip地址为120.79.92.223,有三个分别为www.baidu.com、www.google.com、www.sohu.com的域名解析到了这三个网站上，当我们通过http://www.baidu.com这个网址去访问时，DNS解析出的ip为120.79.92.223，这时候服务器就是根据请求头中的host字段选择使用www.baidu.com这个域名的网站程序对请求做响应。


### $http_host
与$host相同，但如果请求信息中没有Host行，则可能不同。

### $hostname
host name.
主机名称。

### $http_name
arbitrary request header field; the last part of a variable name is the field name converted to lower case with dashes replaced by underscores.
任意请求头字段；变量名的最后一部份是转换为小写的字段名，通过下划线相连。

### $https
“on” if connection operates in ssl mode, or an empty string otherwise.
如果是https请求方式则值为on，否则为空字符串。

### $http_HEADER
http请求信息里的HEADER字段。


### $http_cookie
客户端的cookie信息。

### $http_referer
引用地址。

### $http_user_agent
客户端代理信息。

### $http_via
最后一个访问服务器的IP地址。

### $http_x_forwarded_for
相当于网络访问路径。

### $is_args
“?” if a request line has arguments, or an empty string otherwise.
如果URL包含参数则为？，否则为空字符串。

### $limit_rate
setting this variable enables response rate limiting.
nginx配置中的limit_rate配置项的值，影响响应速度限制。

### $msec
current time in seconds with the milliseconds resolution(1.3.9,1.2.6)
当前时间，单位为毫秒。

### $nginx_version
nginx version.

### $pid
PID of the worker process.
工作进程的PID。

### $pipe
“p” if request was pipelined, “.” otherwise(1.3.12,1.2.7).

### $proxy_protocol_addr
client address from the PROXY protocol header, or an empty string otherwise(1.5.12)
Thie PROXY protocol must be previously enabled by setting the proxy_protocol parameter in the listen directive.
来自代理协议头的客户端地址，否则为空字符串。
proxy_protocol在nginx配置listen参数时设定。

### $proxy_protocol_port
client port from the PROXY protocol header, or an empty string otherwise(1.11.0).
来自代理协议头的客户端端口，其它与$proxy_protocol_addr相同。

### $query_string
same as $args.

### $realpath_root
an absolute pathname corresponding to the root or alias directive’s value for the current request, with all symbolic links resolved to real paths.
对应于当前请求的根目录或别名值的绝对路径名，所有符号连接都解析为真实路径。

### $remote_addr
client address.
客户端地址。

是nginx与客户端进行TCP连接过程中，获得的客户端真实地址. Remote Address 无法伪造，因为建立 TCP 连接需要三次握手，如果伪造了源 IP，无法建立 TCP 连接，更不会有后面的 HTTP 请求



### $remote_port
client port.
客户端端口。

### $remote_user
user name supplied with the Basic authentication.
用于基本验证的用户名。

### $request
full original request line.
完整的原始请求URL。

### $request_body
request body.
The variable’s value is made available in locations processed by the proxy_pass,fastcgi_pass,uwsgi_pass,and scgi_pass directives when the request body was read to a memory buffer.
请求体，当proxy_pass,fastcgi_pass,uwsgi_pass和scgi_pass指令将请求体读入缓存中时此变量值可用。

### $request_body_file
name of a temporary file with the request body.
请求主体的临时文件的名称。

### $request_completion
“OK” if a request has completed, or an empty string otherwise.
如果请求完成则值为OK，否则为空字符串。

## $request_filename
file path for the current request, based on the root or alias directives, and the request URI.
当前请求的文件路径，基于根目录或别名指令，以及请求URI。

### $request_id
unique request identifier generated from 16 random bytes, in hexadecimal(1.11.0).
16位随机字节生成的唯一标识符。

### $request_length
request length (including request line, header, and request body)(1.3.12,1.2.7).
请求长度，包含请求行，请求头，以及请求的消息体。

### $request_method
request method, usually “GET” or “POST”.
请求方法，通常为GET或POST。

### $request_time
request processing time in seconds with a milliseconds resolution(1.3.9,1.2.6); time elapsed since the first bytes were read from the client.
以毫秒为单位的请求处理时间；从客户端读取第1个字节之后的时间。

### $request_uri
full original request URI (with arguments).
完整的原始请求URI（带有参数）。

### $scheme
request scheme, “http” or “https”.
请求方案，http或https。

### $sent_http_name
arbitrary response header field; the last part of a variable name is the field name converted to lower case with dashes replaced by underscores.
任意的响应头字段；变量名的最后一部份是转换为小写的字段名，通过下划线相连。

### $send_trailer_name
arbitrary field sent at the end of the response(1.13.2); the last part of a variable name is the field name converted to lower case with dashes replaced by underscores.
响应结束时发送的任意字段；变量名的最后一部份是转换为小写的字段名，通过下划线相连。

### $server_addr
an address of the server which accepted a request.
响应请求的服务器地址。

### $server_name
name of the server which accepted a request.
响应请求的服务器名称。

### $server_port
port of the server which accepted a request.
响应请求的服务器端口。

### $server_protocol
request protocol, usually “HTTP/1.0”, “HTTP/1.1” or “HTTP/2.0”.
请求协议，通常是”HTTP/1.0”, “HTTP/1.1” 或”HTTP/2.0”。

### $status
response status(1.3.2,1.2.2)
响应状态。

### $tcpinfo_rtt
### $tcpinfo_rttvar
### $tcpinfo_snd_cwnd
### $tcpinfo_rcv_space
information about the client TCP connection; available on systems that support the TCP_INFO socket option.
客户端TCP连接的相关信息，在支持TCP_INFO套接字选项的系统上可用。

### $time_iso8601
local time in the ISO 8601 standard format(1.3.12,1.2.7).
ISO 8601标准格式下的本地时间。

### $time_local
local time in the Common Log Format(1.3.12,1.2.7).
通用日志格式的本地时间。



