

书籍内容见《实战Nginx.取代Apache的高性能Web服务》 第三章



- 反向代理（ Reverse Proxy ）是指以代理服务器来接受Internet 上的连接请求，然后将请求转发给内部网络上的服务器，并将从服务器上得到的结果返回给Internet 上请求连接的客户端，此时代理服务器对外就表现为一个服务器。[反向代理官方配置文档](http://nginx.org/en/docs/http/ngx_http_proxy_module.html#proxy_pass)

- 通常的代理服务器，只用于代理内部网络对Internet 的连接请求


## Upstream 模块是Nginx 负载均衡的主要模块



## 部署 nginx 反向代理 golang web举例

https://www.cnblogs.com/zdz8207/p/golang-learn-9.html


```conf
http {

#与后端服务器连接的超时时间——发起握手等候响应超时时间
proxy_connect_time。ut 600;
#连接成功后－等候后端服务器响应时间－其实已经进入后端的排队之中等候处理
proxy_read_timeout 600;
#后端服务器数据回传时间一就是在规定时间内后端服务器必须传完所有的数据
proxy_send_timeout 600;


#代理请求缓存区一这个缓存区间会保存用户的头信息以供Nginx 进行规则处理一一般只要能保存下头信息即可
proxy_buffer_size 16k;
#同上告诉Nginx 保存单个用的几个Buffer 最大用多大空间
proxy_buffers 4 32k;
#如果系统很忙的时候可以申请更大的proxy_buffers 官方推荐*2
proxy_busy_buffers_size 64k;
#proxy 缓存临时文件的大小
proxy_temp_file_write_size 64k;

#Upstream 指令用于设置一组可以在proxy_pass 和fastcgi_pass 指令中使用的代理服务器，
#默认的负载均衡方式为轮询。Upstream 模块中的Server 指令用于指定后端服务器的名称和参数，
#服务器的名称可以是一个域名、一个IP 地址、端口号或UNIX Socketo
upstream server_pool {
server 192.168.1.15:80 weight=l max_fails=2 fail_timeout=30s;
server 192.168.1.16:80 weight=l max_fails=2 fail_timeout=30s;
}

    # 反向代理server_pool这组服务器
    server {
        listen       80;
        server_name  a.com;
     
        charset utf-8;
        access_log  /home/a.com.access.log;
     
        # 以匹配...正则表达式的字符串开头的
        location /(css|js|fonts|img)/ {
            root "/path/to/app_a/static";
    
            access_log off;
            # cache 1天，这个在响应中通过header传递给客户端
            expires 1d; 
            
            #“@name”是用于重新定向的，因此见后面的“location @backend”
            try_files $uri @backend;
        }
    
        #匹自己任何以"/"开头的查询，但是其他类似正则表达式及较长的字符串（例如/admin/）如果存在将被优先匹配。
        location / {
            
            try_files /_not_exists_ @backend;
        }
     
        #访问http://wwwl.a.com:80/(css|js|fonts|img)/*** 地址，反向代理server_pool:8080 这组服务器
        location @backend {
            #proxy set_header 指令用于在向反向代理的后端Web 服务器发起请求时添加指定的Header头信息。
            #以前一跳的客户端地址填写给下一跳http header中的x-forwarded-for
            proxy_set_header X-Forwarded-For $remote_addr;
            #以前一跳xxx填写给下一跳http header中的Host
            proxy_set_header Host            $http_host;
            
            #设置进行反向代理的upstream 服务器集群。
            proxy_pass http://server_pool;
        }
    }

}    
```

# ngx_http_proxy_module 模块

ngx_http_proxy_module 模块允许将请求传递给另外一台服务器

配置举例

```conf
location / {
    proxy_pass       http://localhost:8000;
    proxy_set_header Host      $host;
    proxy_set_header X-Real-IP $remote_addr;
}
```

Directives

### proxy_bind

```conf
Syntax:	proxy_bind address [transparent] | off;
Default:	—
Context:	http, server, location
This directive appeared in version 0.8.22.
```

使得出方向连接到被代理服务器的连接，关联到该指令指定的IP/PORT参数，该参数中可以使用变量。该指令将会禁止从上层配置继承过来的配置：允许系统自动分配的本地ip/PORT.

其中的transparent参数，允许该address中的IP不是本地IP，例如是一个真实公网ip地址，例如:

```conf
proxy_bind $remote_addr transparent;
```

若要使该参数transparent生效，通常需要让nginx worker进程具有超级用户权限，如果在linux上则不需要，因为nginx worker进程从master进程继承了CAP_NET_RAW能力. 另外，需要配置kernel routing table拦截从被代理服务器过来的流量.

### proxy_buffer_size

```conf
Syntax:	proxy_buffer_size size;
Default:	
proxy_buffer_size 4k|8k;
Context:	http, server, location
```

设置缓存大小，该缓存用于读取来自被代理服务器发出响应的前面一部分，这前面一部分通常包含一个小的响应头。 默认情况下，该缓存大小是一个内存页的大小，4k、8k具体取决于所在平台。你也可以将它设置的小于一个内存页大小。


### proxy_buffering

```conf
Syntax:	proxy_buffering on | off;
Default:	
proxy_buffering on;
Context:	http, server, location
```

启用或禁用缓存来自被代理服务器发出的响应。

当启用时，nginx将立即接收到来自被服务器得响应，并将它保存在缓存，该缓存由proxy_buffer_size and proxy_buffers 指令定义. 如果整个响应与该缓存内存不匹配，那一部分将会保存到磁盘的临时文件. 写入临时文件这个过程受proxy_max_temp_file_size and proxy_temp_file_write_size 指令控制.

当禁用时，一收到响应就会同步方式传递给客户端。nginx不会尝试先收到完整的来自被代理服务器的响应。nginx从服务器一次可接收的最大数据大小，由proxy_buffer_size 指令控制.

启用或禁用缓存也可以通过响应头中“X-Accel-Buffering” 字段取值为“yes” or “no” 来控制。该能力可以通过proxy_ignore_headers 指令禁用掉.


### proxy_buffers

```conf
Syntax:	proxy_buffers number size;
Default:	
proxy_buffers 8 4k|8k;
Context:	http, server, location
```

设置用来缓存来自被代理服务器响应的缓存数量与大小。对单个连接，默认缓存大小是一个内存页大小，4k、8k具体取决于所在平台。

### proxy_busy_buffers_size

```conf
Syntax:	proxy_busy_buffers_size size;
Default:	
proxy_busy_buffers_size 8k|16k;
Context:	http, server, location
```

在启用缓存来自被代理服务器的响应情况下，该指令用于限制总的缓存大小，这些大小的缓存们被忙于向客户端发送响应，然而响应并未被完全读取。同时，其余的缓存可以被用来读取读取保存在其中的响应，如果需要，还可以将缓存中的部分响应保存到临时文件里面。 默认情况下，
When buffering of responses from the proxied server is enabled, limits the total size of buffers that can be busy sending a response to the client while the response is not yet fully read. In the meantime, the rest of the buffers can be used for reading the response and, if needed, buffering part of the response to a temporary file. By default, size is limited by the size of two buffers set by the proxy_buffer_size and proxy_buffers directives.

启用代理服务器响应缓冲时，限制在响应尚未完全读取时可能正忙于向客户端发送响应的缓冲区的总大小。同时，其余的缓冲区可以用于读取响应，如果需要，还可以将响应的一部分缓冲到临时文件。默认情况下，该指令中的size受到 由由proxy-buffer-size和proxy-buffers 指令定义的两个缓存的大小限制。

### proxy_cache

```conf
Syntax:	proxy_cache zone | off;
Default:	
proxy_cache off;
Context:	http, server, location
```

定义用于缓存的共享内存区域。同一共享内存区域可用于多个地方。参数值可以包含变量。当参数是off，代表禁用从上一个配置级别继承的缓存。

### proxy_cache_background_update

```conf
Syntax:	proxy_cache_background_update on | off;
Default:	
proxy_cache_background_update off;
Context:	http, server, location
This directive appeared in version 1.11.10.
```

允许启动个后台子请求以更新过期的缓存项，同时向客户端返回过时的缓存响应。请注意，在更新过时的缓存响应时，必须允许使用过时缓存响应。

Allows starting a background subrequest to update an expired cache item, while a stale cached response is returned to the client. Note that it is necessary to allow the usage of a stale cached response when it is being updated.

### proxy_cache_bypass

```conf
Syntax:	proxy_cache_bypass string ...;
Default:	—
Context:	http, server, location
```

定义不从缓存中获取响应的条件。如果字符串参数中至少有一个值不为空且不等于“0”，则不会从缓存中获取响应：

Defines conditions under which the response will not be taken from a cache. If at least one value of the string parameters is not empty and is not equal to “0” then the response will not be taken from the cache:

```conf
proxy_cache_bypass $cookie_nocache $arg_nocache$arg_comment;
proxy_cache_bypass $http_pragma    $http_authorization;
```
上述这些参数能与proxy_no_cache指令一起使用

Can be used along with the proxy_no_cache directive.

### proxy_cache_convert_head

```conf
Syntax:	proxy_cache_convert_head on | off;
Default:	
proxy_cache_convert_head on;
Context:	http, server, location
This directive appeared in version 1.9.7.
```

当“HEAD”方法转换为“GET”时，是否启用或禁用缓存。当转换被禁用时，应将“cache key”指令“proxy_cache_key string;”配置为包含“$request_method”。

Enables or disables the conversion of the “HEAD” method to “GET” for caching. When the conversion is disabled, the cache key should be configured to include the $request_method.

### proxy_cache_key

```conf
Syntax:	proxy_cache_key string;
Default:	
proxy_cache_key $scheme$proxy_host$request_uri;
Context:	http, server, location
```

定义了需要缓存的key，例如：

```conf
proxy_cache_key "$host$request_uri $cookie_user";
```

By default, the directive’s value is close to the string

```conf
proxy_cache_key $scheme$proxy_host$uri$is_args$args;
```

### proxy_cache_lock

```conf
Syntax:	proxy_cache_lock on | off;
Default:	
proxy_cache_lock off;
Context:	http, server, location
This directive appeared in version 1.1.12.
```

如果启用，一次只允许一个请求去填充根据"proxy_cache_key"指令标识的新缓存元素，该请求是像被代理服务器发送个请求 。同一缓存元素的其他请求要不等待缓存中出现响应，要不释放此元素的缓存锁，直到“proxy_cache_lock_timeout”指令设置的时间为止。

When enabled, only one request at a time will be allowed to populate a new cache element identified according to the proxy_cache_key directive by passing a request to a proxied server. Other requests of the same cache element will either wait for a response to appear in the cache or the cache lock for this element to be released, up to the time set by the proxy_cache_lock_timeout directive.

### proxy_cache_lock_age

```conf
Syntax:	proxy_cache_lock_age time;
Default:	
proxy_cache_lock_age 5s;
Context:	http, server, location
This directive appeared in version 1.7.8.
```

如果为填充新缓存元素而传递给被代理服务器的最后一个请求在指定的时间内未完成，则可以再向代理服务器传递一个请求。

If the last request passed to the proxied server for populating a new cache element has not completed for the specified time, one more request may be passed to the proxied server.

### proxy_cache_lock_timeout

```conf
Syntax:	proxy_cache_lock_timeout time;
Default:	
proxy_cache_lock_timeout 5s;
Context:	http, server, location
This directive appeared in version 1.1.12.
```

设置“proxy_cache_lock”指令的超时时间，当超时时，请求将被传给被代理服务器。注意，该请求的响应是不会被缓存的。

Sets a timeout for proxy_cache_lock. When the time expires, the request will be passed to the proxied server, however, the response will not be cached.

Before 1.7.8, the response could be cached.

### proxy_cache_max_range_offset

```conf
Syntax:	proxy_cache_max_range_offset number;
Default:	—
Context:	http, server, location
This directive appeared in version 1.11.6.
```

Sets an offset in bytes for byte-range requests. If the range is beyond the offset, the range request will be passed to the proxied server and the response will not be cached.

Syntax:	proxy_cache_methods GET | HEAD | POST ...;
Default:	
proxy_cache_methods GET HEAD;
Context:	http, server, location
This directive appeared in version 0.7.59.

If the client request method is listed in this directive then the response will be cached. “GET” and “HEAD” methods are always added to the list, though it is recommended to specify them explicitly. See also the proxy_no_cache directive.

Syntax:	proxy_cache_min_uses number;
Default:	
proxy_cache_min_uses 1;
Context:	http, server, location
Sets the number of requests after which the response will be cached.

Syntax:	proxy_cache_path path [levels=levels] [use_temp_path=on|off] keys_zone=name:size [inactive=time] [max_size=size] [manager_files=number] [manager_sleep=time] [manager_threshold=time] [loader_files=number] [loader_sleep=time] [loader_threshold=time] [purger=on|off] [purger_files=number] [purger_sleep=time] [purger_threshold=time];
Default:	—
Context:	http
Sets the path and other parameters of a cache. Cache data are stored in files. The file name in a cache is a result of applying the MD5 function to the cache key. The levels parameter defines hierarchy levels of a cache: from 1 to 3, each level accepts values 1 or 2. For example, in the following configuration

proxy_cache_path /data/nginx/cache levels=1:2 keys_zone=one:10m;
file names in a cache will look like this:

/data/nginx/cache/c/29/b7f54b2df7773722d382f4809d65029c
A cached response is first written to a temporary file, and then the file is renamed. Starting from version 0.8.9, temporary files and the cache can be put on different file systems. However, be aware that in this case a file is copied across two file systems instead of the cheap renaming operation. It is thus recommended that for any given location both cache and a directory holding temporary files are put on the same file system. The directory for temporary files is set based on the use_temp_path parameter (1.7.10). If this parameter is omitted or set to the value on, the directory set by the proxy_temp_path directive for the given location will be used. If the value is set to off, temporary files will be put directly in the cache directory.

In addition, all active keys and information about data are stored in a shared memory zone, whose name and size are configured by the keys_zone parameter. One megabyte zone can store about 8 thousand keys.

As part of commercial subscription, the shared memory zone also stores extended cache information, thus, it is required to specify a larger zone size for the same number of keys. For example, one megabyte zone can store about 4 thousand keys.
Cached data that are not accessed during the time specified by the inactive parameter get removed from the cache regardless of their freshness. By default, inactive is set to 10 minutes.

The special “cache manager” process monitors the maximum cache size set by the max_size parameter. When this size is exceeded, it removes the least recently used data. The data is removed in iterations configured by manager_files, manager_threshold, and manager_sleep parameters (1.11.5). During one iteration no more than manager_files items are deleted (by default, 100). The duration of one iteration is limited by the manager_threshold parameter (by default, 200 milliseconds). Between iterations, a pause configured by the manager_sleep parameter (by default, 50 milliseconds) is made.

A minute after the start the special “cache loader” process is activated. It loads information about previously cached data stored on file system into a cache zone. The loading is also done in iterations. During one iteration no more than loader_files items are loaded (by default, 100). Besides, the duration of one iteration is limited by the loader_threshold parameter (by default, 200 milliseconds). Between iterations, a pause configured by the loader_sleep parameter (by default, 50 milliseconds) is made.

Additionally, the following parameters are available as part of our commercial subscription:

purger=on|off
Instructs whether cache entries that match a wildcard key will be removed from the disk by the cache purger (1.7.12). Setting the parameter to on (default is off) will activate the “cache purger” process that permanently iterates through all cache entries and deletes the entries that match the wildcard key.
purger_files=number
Sets the number of items that will be scanned during one iteration (1.7.12). By default, purger_files is set to 10.
purger_threshold=number
Sets the duration of one iteration (1.7.12). By default, purger_threshold is set to 50 milliseconds.
purger_sleep=number
Sets a pause between iterations (1.7.12). By default, purger_sleep is set to 50 milliseconds.
In versions 1.7.3, 1.7.7, and 1.11.10 cache header format has been changed. Previously cached responses will be considered invalid after upgrading to a newer nginx version.
Syntax:	proxy_cache_purge string ...;
Default:	—
Context:	http, server, location
This directive appeared in version 1.5.7.

Defines conditions under which the request will be considered a cache purge request. If at least one value of the string parameters is not empty and is not equal to “0” then the cache entry with a corresponding cache key is removed. The result of successful operation is indicated by returning the 204 (No Content) response.

If the cache key of a purge request ends with an asterisk (“*”), all cache entries matching the wildcard key will be removed from the cache. However, these entries will remain on the disk until they are deleted for either inactivity, or processed by the cache purger (1.7.12), or a client attempts to access them.

Example configuration:

proxy_cache_path /data/nginx/cache keys_zone=cache_zone:10m;

map $request_method $purge_method {
    PURGE   1;
    default 0;
}

server {
    ...
    location / {
        proxy_pass http://backend;
        proxy_cache cache_zone;
        proxy_cache_key $uri;
        proxy_cache_purge $purge_method;
    }
}
This functionality is available as part of our commercial subscription.
Syntax:	proxy_cache_revalidate on | off;
Default:	
proxy_cache_revalidate off;
Context:	http, server, location
This directive appeared in version 1.5.7.

Enables revalidation of expired cache items using conditional requests with the “If-Modified-Since” and “If-None-Match” header fields.