

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



##  

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

