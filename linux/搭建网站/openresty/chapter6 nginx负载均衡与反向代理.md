

书籍内容见《实战Nginx.取代Apache的高性能Web服务》 第三章



- 反向代理（ Reverse Proxy ）是指以代理服务器来接受Internet 上的连接请求，然后将请求转发给内部网络上的服务器，并将从服务器上得到的结果返回给Internet 上请求连接的客户端，此时代理服务器对外就表现为一个服务器。

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
