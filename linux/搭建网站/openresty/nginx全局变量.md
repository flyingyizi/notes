

## $uri
current URI in request, normalized.
The value of $uri may change during request processing, e.g. when doing internal redirects, or when using index files.
当前请求的URI。

## $arg_name
argument name in the request line.
请求URL中的参数名称。

## $args
arguments in the request line.
请求URL中包含的参数。

## $binary_remote_addr
client address in a binary form,value’s length is always 4 bytes for IPv4 addresses or 16 bytes for IPv6 addresses.
二进制形式的客户端地址，ipv4通常是4比特长度，ipv6通常为16比特长度。

## $body_bytes_sent
number of bytes sent to a client,not counting the response header;this variable is compatible withe the “%B” parameter of the mod_log_config Apache module.
发送给客户端的不包含响应头的字节数；这个变量兼容Apache的mod_log_config模块的“%B”参数。

## $bytes_sent
number of bytes sent to a client(1.3.8,1.2.5).
发送给客户端的字节数。

## $connection
connection serial number(1.3.8,1.2.5).
连接序列号。

## $connection_requests
current number of requests made through a connection(1.3.8,1.2.5).
当前通过连接发出的请求数。

## $content_length
“Content-Length” request header field.
请求头中的Content-Length字段。

## $content_type
“Content-Type” request header field.
请求头中的Content-Type字段。

## $cookie_name
the name cookie.
cookie的名称。

## $document_root
root or alias directive’s value for the current request.
当前请求的根目录或别名的值。

## $document_uri
same as $uri.
与$uri全局变量相同。

## $host
in this order of precedence:host name from the request line, or host name from the “Host” request header field, or the server name matching a request.
请求URL中的主机名字段，或请求头中的Host字段，或匹配请求的server_name（nginx配置）。

host是HTTP 1.1协议中新增的一个请求头字段，存放服务器端域名:端口。能够很好的解决一个ip地址对应多个域名的问题。当服务器接收到来自浏览器的请求时，会根据请求头中的host字段访问哪个站点。

举个栗子，我有一台服务器A ip地址为120.79.92.223,有三个分别为www.baidu.com、www.google.com、www.sohu.com的域名解析到了这三个网站上，当我们通过http://www.baidu.com这个网址去访问时，DNS解析出的ip为120.79.92.223，这时候服务器就是根据请求头中的host字段选择使用www.baidu.com这个域名的网站程序对请求做响应。


## $http_host
与$host相同，但如果请求信息中没有Host行，则可能不同。

## $hostname
host name.
主机名称。

## $http_name
arbitrary request header field; the last part of a variable name is the field name converted to lower case with dashes replaced by underscores.
任意请求头字段；变量名的最后一部份是转换为小写的字段名，通过下划线相连。

## $https
“on” if connection operates in ssl mode, or an empty string otherwise.
如果是https请求方式则值为on，否则为空字符串。

## $http_HEADER
http请求信息里的HEADER字段。


$http_cookie
客户端的cookie信息。

$http_referer
引用地址。

$http_user_agent
客户端代理信息。

$http_via
最后一个访问服务器的IP地址。

$http_x_forwarded_for
相当于网络访问路径。

$is_args
“?” if a request line has arguments, or an empty string otherwise.
如果URL包含参数则为？，否则为空字符串。

$limit_rate
setting this variable enables response rate limiting.
nginx配置中的limit_rate配置项的值，影响响应速度限制。

$msec
current time in seconds with the milliseconds resolution(1.3.9,1.2.6)
当前时间，单位为毫秒。

$nginx_version
nginx version.

$pid
PID of the worker process.
工作进程的PID。

$pipe
“p” if request was pipelined, “.” otherwise(1.3.12,1.2.7).

$proxy_protocol_addr
client address from the PROXY protocol header, or an empty string otherwise(1.5.12)
Thie PROXY protocol must be previously enabled by setting the proxy_protocol parameter in the listen directive.
来自代理协议头的客户端地址，否则为空字符串。
proxy_protocol在nginx配置listen参数时设定。

$proxy_protocol_port
client port from the PROXY protocol header, or an empty string otherwise(1.11.0).
来自代理协议头的客户端端口，其它与$proxy_protocol_addr相同。

$query_string
same as $args.

$realpath_root
an absolute pathname corresponding to the root or alias directive’s value for the current request, with all symbolic links resolved to real paths.
对应于当前请求的根目录或别名值的绝对路径名，所有符号连接都解析为真实路径。

## $remote_addr
client address.
客户端地址。

是nginx与客户端进行TCP连接过程中，获得的客户端真实地址. Remote Address 无法伪造，因为建立 TCP 连接需要三次握手，如果伪造了源 IP，无法建立 TCP 连接，更不会有后面的 HTTP 请求



## $remote_port
client port.
客户端端口。

$remote_user
user name supplied with the Basic authentication.
用于基本验证的用户名。

$request
full original request line.
完整的原始请求URL。

$request_body
request body.
The variable’s value is made available in locations processed by the proxy_pass,fastcgi_pass,uwsgi_pass,and scgi_pass directives when the request body was read to a memory buffer.
请求体，当proxy_pass,fastcgi_pass,uwsgi_pass和scgi_pass指令将请求体读入缓存中时此变量值可用。

$request_body_file
name of a temporary file with the request body.
请求主体的临时文件的名称。

$request_completion
“OK” if a request has completed, or an empty string otherwise.
如果请求完成则值为OK，否则为空字符串。

## $request_filename
file path for the current request, based on the root or alias directives, and the request URI.
当前请求的文件路径，基于根目录或别名指令，以及请求URI。

$request_id
unique request identifier generated from 16 random bytes, in hexadecimal(1.11.0).
16位随机字节生成的唯一标识符。

$request_length
request length (including request line, header, and request body)(1.3.12,1.2.7).
请求长度，包含请求行，请求头，以及请求的消息体。

$request_method
request method, usually “GET” or “POST”.
请求方法，通常为GET或POST。

$request_time
request processing time in seconds with a milliseconds resolution(1.3.9,1.2.6); time elapsed since the first bytes were read from the client.
以毫秒为单位的请求处理时间；从客户端读取第1个字节之后的时间。

$request_uri
full original request URI (with arguments).
完整的原始请求URI（带有参数）。

$scheme
request scheme, “http” or “https”.
请求方案，http或https。

$sent_http_name
arbitrary response header field; the last part of a variable name is the field name converted to lower case with dashes replaced by underscores.
任意的响应头字段；变量名的最后一部份是转换为小写的字段名，通过下划线相连。

$send_trailer_name
arbitrary field sent at the end of the response(1.13.2); the last part of a variable name is the field name converted to lower case with dashes replaced by underscores.
响应结束时发送的任意字段；变量名的最后一部份是转换为小写的字段名，通过下划线相连。

$server_addr
an address of the server which accepted a request.
响应请求的服务器地址。

$server_name
name of the server which accepted a request.
响应请求的服务器名称。

$server_port
port of the server which accepted a request.
响应请求的服务器端口。

$server_protocol
request protocol, usually “HTTP/1.0”, “HTTP/1.1” or “HTTP/2.0”.
请求协议，通常是”HTTP/1.0”, “HTTP/1.1” 或”HTTP/2.0”。

$status
response status(1.3.2,1.2.2)
响应状态。

$tcpinfo_rtt
$tcpinfo_rttvar
$tcpinfo_snd_cwnd
$tcpinfo_rcv_space
information about the client TCP connection; available on systems that support the TCP_INFO socket option.
客户端TCP连接的相关信息，在支持TCP_INFO套接字选项的系统上可用。

$time_iso8601
local time in the ISO 8601 standard format(1.3.12,1.2.7).
ISO 8601标准格式下的本地时间。

$time_local
local time in the Common Log Format(1.3.12,1.2.7).
通用日志格式的本地时间。



