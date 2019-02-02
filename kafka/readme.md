

## vertx
- [[vertx实现redis版session共享]](https://segmentfault.com/a/1190000009470700)

- [[Walk a directory/Non-recursively]](http://rosettacode.org/wiki/Walk_a_directory/Non-recursively)

- [[dgate：an API Gateway based on Vert.x]](https://github.com/DTeam-Top/dgate)
- [[vertx-blueprint 中api gateway]](http://www.sczyh30.com/vertx-blueprint-microservice/index.html)
- [[source vertx-blueprint 中api gateway]](https://github.com/sczyh30/vertx-blueprint-microservice)
- [[微服务实战（二）：使用API Gateway]](http://www.open-open.com/lib/view/open1436089902667.html)
- [[Vert.x 3学习笔记---07]](http://blog.csdn.net/avastxa/article/details/50717880)

- [[使用Vert.x构建Web服务器和消息系统]](https://segmentfault.com/a/1190000005713918)
- [[Vert.x新手简易教程]](https://zhuanlan.zhihu.com/p/28045945)

- [[Java 异步非阻塞编程框架 Vert.x]](https://zhuanlan.zhihu.com/p/28538351)

- [[HTTP访问控制（CORS）]](https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Access_control_CORS)

- [[Vert.x how to pass/get messages from REST to message bus]](https://stackoverflow.com/questions/37897876/vert-x-how-to-pass-get-messages-from-rest-to-message-bus)

- [[vertx-examples]](https://github.com/vert-x3/vertx-examples)

- [[跨域资源共享(CORS handling)]](http://itfish.net/article/44576.html)

- [[Vert.x 官方文档中文翻译]](https://vertxchina.github.io/vertx-translation-chinese/)

- [[vertx  白木城全栈]](https://zhuanlan.zhihu.com/whitewoodcity)

- [[用Kotlin Coroutine改造Vert.x的函数回调地狱{ _ -> Callback Hell }]](https://zhuanlan.zhihu.com/p/28046403)

Thats wrong, you don't have to instantiate the http server inside the verticle multiple times. You have to instantiate the verticle itself multiple times.

You can start your verticle with the vertx binary or as fatjar (Here is an example https://github.com/floriankammermann/vertx-examples/tree/master/hello-world-java)

You can add this commandline parameter to deploy multiple instances: --instances 2

You can also deploy your verticle programatically and pass the number of instances. You find an example here: https://github.com/floriankammermann/vertx-examples/blob/master/verticle-instances/src/main/java/org/swisspush/vertx/examples/CryptoVerticleStarter.java

# 安装kafka本机环境

##  docker kafka 方式1

  ```shell
  docker pull wurstmeister/zookeeper
  docker pull wurstmeister/kafka

  docker run -d --name zookeeper  -p 2181:2181 -t wurstmeister/zookeeper 
  docker run -d --name kafka --publish 9092:9092 \
  --link zookeeper \
  --env KAFKA_ZOOKEEPER_CONNECT=zookeeper:2181 \
  --env KAFKA_ADVERTISED_HOST_NAME=127.0.0.1 \
  --env KAFKA_ADVERTISED_PORT=9092 \
  wurstmeister/kafka
  ```

  或通过下面的docker-compose.yml文件，运行`docker-compose up -d`方式启动. 
  ```yaml
  version: '2'
  services:
    zk_serv:
      image: wurstmeister/zookeeper
      ports:
        - "2181:2181"
    kafka_serv:
      image: wurstmeister/kafka
      ports:
        - "9092"
      environment:
        KAFKA_ADVERTISED_HOST_NAME: 127.0.0.1
        KAFKA_ZOOKEEPER_CONNECT: zk_serv:2181
  ```  

  进入docker容器，可以运行kafka命令，例如
  ```shell
  docker exec -it kafka_msg_sender_1 /bin/bash
  #/opt/kafka/bin/kafka-topics.sh --list  --zookeeper zk_serv:2181
  ```

```yaml
version: '2' # 固定，表示其版本为2 
services: # 固定，表示容器服务 
  gitlab: # 容器名称，根据实际修改 
    image: latelee/gitlab # 镜像名，如果不存在，则从网络下载 
    container_name: gitlab # 容器名，在运行名称是唯一的 
    restart: always # 开启此选项，在机器重启后，会自动启动这些容器，否则，就要手动启动容器 
    volumes: # 挂载数据卷，对应docker run命令的-v选项。 
      - /mnt/gitlab_data/config:/etc/gitlab 
    ports: # 端口映射 
      - "80:80" 
    networks: # 指定的网络，也可以不指定，必须与networks名称一致 
      - mygitlab-jenkins-net 
  jenkins：# 容器名称，根据实际修改，格式与前述一样 
  ... 
networks: # 固定，对网络配置，如无则不需要 
  mygitlab-jenkins-net: # 自定义的网络名称 
    driver: bridge # 桥接方式
```

  打开终端，在docker-compose.yml所在目录下执行docker-compose up -d
  ```shell
  c:\home>docker-compose up -d
  Creating network "home_default" with the default driver
  ...
  Creating home_zk_serv_1    ... done
  Creating home_kafka_serv_1 ... done
  ``` 

SmokeTest
  ```shell
  sudo docker exec -it kafka /bin/bash

  /opt/kafka/bin/kafka-console-producer.sh --topic=test --broker-list localhost:9092

  /opt/kafka/bin/kafka-console-consumer.sh --bootstrap-server localhost:9092 -from-beginning --to
  ```
##  docker kafka 方式2

- [[ Docker下kafka学习，三部曲之一：极速体验kafka ]](http://blog.csdn.net/boling_cavalry/article/details/71576775)

- [[ Docker下kafka学习，三部曲之三：java开发]](http://blog.csdn.net/boling_cavalry/article/details/71634631)

Kafka是一种高吞吐量的分布式发布订阅消息系统，从本章开始我们先极速体验，再实战docker下搭建kafka环境，最后开发一个java web应用来体验kafka服务。

我们一起用最快的速度体验kafka的消息发布订阅功能：

新建一个docker-compose.yml文件，内容如下：
```yaml

version: '2'
services:
  zk_serv:
    image: daocloud.io/library/zookeeper:3.3.6
    restart: always
  kafka_serv:
    image: bolingcavalry/kafka:0.0.1
    links:
      - zk_serv:zkhost
    command: /bin/sh -c '/usr/local/work/start_server.sh'
    restart: always
  msg_sender:
    image: bolingcavalry/kafka:0.0.1
    links:
      - zk_serv:zkhost
      - kafka_serv:kafkahost
    restart: always
  msg_consumer:
    image: bolingcavalry/kafka:0.0.1
    links:
      - zk_serv:zkhost
    restart: always
```
打开终端，在docker-compose.yml所在目录下执行docker-compose up -d，由于需要从hub.docker.com下载镜像，所以需要等待几分钟，下载结束后会自动启动四个容器，如下图：
```shell
...
Creating kafka_zk_serv_1
Creating kafka_kafka_serv_1
Creating kafka_msg_consumer_1
Creating kafka_msg_sender_1
atmel@atmel-virtual-machine:~/docker_build/kafka$
```
为了更好的观察效果，我们要打开三个终端对不同的容器做操作，在后面我们用编号来表达不同的终端，编号对应如下：
```text
    1号 : kafka server
    2号 : 消息生产者(producer)
    3号 : 消息消费者(consumer)
```

1号终端中输入以下命令，即可看到kafka server的实时输出信息：docker logs -f kafka_kafka_serv_1

2号终端中输入以下命令，进入消息生产者容器：docker exec -it kafka_msg_sender_1 /bin/bash

再执行以下命令创建一个主题：$WORK_PATH/$KAFKA_PACKAGE_NAME/bin/kafka-topics.sh --create --zookeeper zkhost:2181 --replication-factor 1 --partitions 1 --topic test001

继续在3号终端上执行以下命令，即可订阅主题为test001的消息：$WORK_PATH/$KAFKA_PACKAGE_NAME/bin/kafka-console-consumer.sh --zookeeper zkhost:2181 --topic test001  --from-beginning

# kafka go client

[How to use Golang client on Windows #128](https://github.com/confluentinc/confluent-kafka-go/issues/128)

## 在windows安装kafaka 客户端confluent-kafka-go

首先系统安装好了MINGW

### 安装librdkafka

  ```shell
  #get nuget from https://dist.nuget.org/win-x86-commandline/latest/nuget.exe
  $wget https://dist.nuget.org/win-x86-commandline/latest/nuget.exe
  #run nuget install librdkafka.redist -Version 0.11.6 ( this is currently the latest version )
  #this will download rdkafka into new librdkafka.redist.0.11.6 directory
  $./nuget install librdkafka.redist -Version 0.11.6
  $ ls
  librdkafka.redist.0.11.6  nuget.exe  
  #copy .\librdkafka.redist.0.11.6\build\native\include\ into C:\tools\msys64\usr\include\
  $cp -r ./librdkafka.redist.0.11.6/build/native/include/librdkafka /usr/include/
  #copy .\librdkafka.redist.0.11.6\build\native\lib\win7\x64\win7-x64-Release\v120\librdkafka.lib into C:\tools\msys64\usr\lib\librdkafka.a (notice .lib is renamed to .a)
  $cp ./librdkafka.redist.0.11.6/build/native/lib/win/x64/win-x64-Release/v120/librdkafka.lib /usr/lib/librdkafka.a
  #注：以上/usr/lib/目录的是C:\tools\msys64\usr\lib。上面的命令是在mingw shell执行，如果是在windows cmd下执行，对应的修改下目录书写方式

  #在/usr/lib/pkgconfig/下创建一个名字为 rdkafka.pc 的pkg config文件，文件的内容见in the project's root directory with following content:
  $ ls /usr/lib/pkgconfig/
  autoopts.pc  protobuf.pc       python-2.7.pc  rdkafka.pc
  libalpm.pc   protobuf-lite.pc  python2.pc
  
  #run go build, that will create executable
  #make sure to deliver librdkafka.dll, msvcr120.dll and zlib.dll from .\librdkafka.redist.0.11.6\runtimes\win7-x64\native\ alongside with newly created executable
  
  ```

  rdkafka.pc 文件内容
  ```text
  prefix=c:/
  libdir=C:/tools/msys64/usr/lib/
  includedir=C:/tools/msys64/usr/include

  Name: librdkafka
  Description: The Apache Kafka C/C++ library
  Version: 0.11.6
  Cflags: -I${includedir}
  Libs: -L${libdir} -lrdkafka
  Libs.private: -lssl -lcrypto -lcrypto -lz -ldl -lpthread -lrt
  ```

  librdkafka.redist.0.11.6\runtimes\win-x64\native中的几个dll文件需要放到

  最后，到windows 中设置PKG_CONFIG_PATH变量为C:\tools\msys64\usr\lib\pkgconfig

### 安装confluent-kafka-go

```cmd
C:\Users\xx>go get -u github.com/confluentinc/confluent-kafka-go/kafka
```

### 测试


# kafka 架构知识

[Kafka系列之(1)——Kafka概念和架构  时之结绳](https://www.jianshu.com/p/bfb799ae136b)

[Kafka系列之(2)——Kafka controller和消息分片&Leader election](https://www.jianshu.com/p/f717a1ea8f32)

## Consumer：
  消费消息。每个consumer属于一个特定的consumer group（可为每个consumer指定group name，若不指定group name则属于默认的group）。同一topic的一条消息只能被同一个consumer group内的一个consumer消费，但多个consumer group可同时消费这一消息。Consumer Group中的每个Consumer读取Topic的一个或多个Partitions，并且是唯一的Consumer；一个Consumer group的多个consumer的所有线程依次有序地消费一个topic的所有partitions,如果Consumer group中所有consumer总线程大于partitions数量，则会出现空闲情况。这样可以做到负载均衡，也可以实现顺序消费（group中只有一个consumer）。

  kafka consumer有很多属性可以设置，具体列表见[librdkafka CONFIGURATION](https://github.com/edenhill/librdkafka/blob/master/CONFIGURATION.md), 下面是一个使用了最常见属性创建consumer

```go

	c, err := kafka.NewConsumer(&kafka.ConfigMap{
		"bootstrap.servers":  broker,
		"group.id":           group,
		"session.timeout.ms": 6000,
		"auto.offset.reset":  "earliest"})

```
  消息消费是以group为单位，不同group完全独立，可以消费相同的消息。就同一个topic来说，group内不同消费者并行消费它的不同分片，就同一分片而言，是顺序消费的。如果有多个topic，group内同一个消费者可以消费不同topic的分片

## 分区Partition

leader-followers结构每个分区都由一个服务器作为“leader”，零或若干服务器作为“followers”,leader负责处理消息的读和写，followers则去复制leader.如果leader down了，followers中的一台则会自动成为leader。集群中的每个服务都会同时扮演两个角色：作为它所持有的一部分分区的leader，同时作为其他分区的followers，这样集群就会据有较好的负载均衡。同时，副本也保证了kafka的容错能力。Zookeeper负责选择leader和保存kafka的元数据(服务器和topic名称等信息)

