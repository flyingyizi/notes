

# demo

## build

build with default tag
```shell
./dockerUtil.sh   build  ./redis-cluster/ 
```
build with tag named abc
```shell
./dockerUtil.sh   build  ./redis-cluster/   abc
```



## 方式1： run 
创建 6 个 redis container。
```shell
./dockerUtil.sh   run 7000:6379
./dockerUtil.sh   run 7001:6379
./dockerUtil.sh   run 7002:6379
./dockerUtil.sh   run 7003:6379
./dockerUtil.sh   run 7004:6379
./dockerUtil.sh   run 7005:6379

docker container logs xxxxxxx
```
下面开始创建cluster,首先查看下原始信息
```shell
$redis-cli -h 192.168.1.8 -p 7000
192.168.1.8:7000> cluster nodes
b2fb456ba3cb440a5ed464539651d715aae20c23 :6379@16379 myself,master - 0 0 0 connected
192.168.1.8:7000>
```
首先通过redis-trib.rb 创建 （attention：当前只有进入某个redis container 内执行redis-trib.rb才成功，在host中失败）
```shell
echo "yes" | ruby ./redis-trib.rb create --replicas 1 172.17.0.2:6379 172.17.0.3:6379 172.17.0.4:6379 172.17.0.5:6379 172.17.0.6:6379 172.17.0.7:6379 
echo "yes" | ruby ./redis-trib.rb create --replicas 1 192.168.1.8:7000 192.168.1.8:7001 192.168.1.8:7002 192.168.1.8:7003 192.168.1.8:7004 192.168.1.8:7005 
```

## 方式2 run

docker run  -i -t -d --net=host -v ~/abc/redis.conf:/redis-conf/redis.conf   redis:alpine /usr/local/bin/redis-server /redis-conf/redis.conf  --port 7000  --protected-mode no 
docker run  -i -t -d --net=host -v ~/abc/redis.conf:/redis-conf/redis.conf   redis:alpine /usr/local/bin/redis-server /redis-conf/redis.conf  --port 7001  --protected-mode no 
docker run  -i -t -d --net=host -v ~/abc/redis.conf:/redis-conf/redis.conf   redis:alpine /usr/local/bin/redis-server /redis-conf/redis.conf  --port 7002  --protected-mode no 
docker run  -i -t -d --net=host -v ~/abc/redis.conf:/redis-conf/redis.conf   redis:alpine /usr/local/bin/redis-server /redis-conf/redis.conf  --port 7003  --protected-mode no 
docker run  -i -t -d --net=host -v ~/abc/redis.conf:/redis-conf/redis.conf   redis:alpine /usr/local/bin/redis-server /redis-conf/redis.conf  --port 7004  --protected-mode no 
docker run  -i -t -d --net=host -v ~/abc/redis.conf:/redis-conf/redis.conf   redis:alpine /usr/local/bin/redis-server /redis-conf/redis.conf  --port 7005  --protected-mode no 

./redis-trib.rb create --replicas 1 192.168.1.8:7000 192.168.1.8:7001  192.168.1.8:7002  192.168.1.8:7003  192.168.1.8:7004  192.168.1.8:7005  



docker run  -i -t -d -p 7000:7000 -v ~/abc/redis7000.conf:/redis-conf/redis.conf  --name 7000redis txq/redis-cluster:latest

##  UBUNTU 16.04 安装 redis-trib.rb 成功实践

首先安装Redis-Cluster依赖库

安装Ruby
```shell
apt-get install ruby
```
安装Redis.gem
```shell
gem intall redis
```
注意：本步骤需要翻/墙，可以先下载到本地，然后再安装，下载地址https://rubygems.org/gems/redis/versions/3.3.1
```shell
gem install /usr/downloads/redis-3.3.1.gem
```
获取redis/src/redis-trib.rb
```shell
$ wget http://download.redis.io/releases/redis-3.2.3.tar.gz
$ tar xzf redis-3.2.3.tar.gz
$ cd redis-3.2.3
```

## 其他资源

### 1
本文档专供需要Redis客户端API调用的开发人员，适用于具备Java开发经验的开发人员。 

http://developer.huawei.com/cn/ict/Products/BigData/FusionInsightHD/Redis/SDK


## Redis 数据序列化方法 serialize, msgpack, json, hprose 比较 

最近弄 Redis ，涉及数据序列化存储的问题，对比了：JSON, Serialize, Msgpack, Hprose 四种方式

http://blog.csdn.net/zhouzme/article/details/46863709

## UDF相关

- golang & mysql UDF https://github.com/dveeden/udf_fileexists_go/blob/master/udf_fileexists_go.go
- https://www.cnblogs.com/zhxilin/archive/2016/09/30/5923671.html

