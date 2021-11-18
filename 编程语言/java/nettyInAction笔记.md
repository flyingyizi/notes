
[官网](netty.io)

[Netty实战.pdf](https://github.com/loveLynch/Book/blob/master/Netty%E5%AE%9E%E6%88%98.pdf)

[书籍代码](https://github.com/ReactivePlatform/netty-in-action-cn)

[基于netty的restful选择1：RestExpress](https://github.com/RestExpress/RestExpress)

```groovy
// https://mvnrepository.com/artifact/io.netty/netty-all
compile group: 'io.netty', name: 'netty-all', version: '5.0.0.Alpha2'
```

# netty 核心组件
Netty’s core components
In this section we’ll discuss Netty’s primary building blocks:

- ■ Channels，channels代表一个连接
- ■ Callbacks
- ■ Futures, 通过future为应用提供了获得操作结束通知的途径
- ■ Events and handlers, e.g, events include:

    - ■ Active or inactive connections
    - ■ Data reads
    - ■ User events
    - ■ Error events


```java
//通过future来获得异步连接的结束通知
Channel channel = ...;
// Does not block
ChannelFuture future = channel.connect(
new InetSocketAddress("192.168.0.1", 25));
future.addListener(new ChannelFutureListener() {
    ...
});
```

## 最小组成
所有netty servers 至少要包含：

- ■ 至少一个事件处理器ChannelHandler—这里记住ChannelInboundHandlerAdapter是ChannelInboundHandler的默认实现.
- ■ 至少一个启动类Bootstrapping—This is the startup code that configures the server. At a minimum,
it binds the server to the port on which it will listen for connection requests.
In the remainder of this section we’ll describe the logic and bootstrapping code

样例代码见 [书籍附带源码](https://github.com/ReactivePlatform/netty-in-action-cn) chapter2

在引导的过程中，在调用bind()或者connect()方法之前，必须调用以下方法来设置所需的组件：
-  group()；
-  channel()或者channelFactory()；
-  handler()。
如果不这样做，则将会导致IllegalStateException。对handler()方法的调用尤其重要，因为它需要配置好ChannelPipeline。

## Channel, EventLoop, and ChannelFuture

netty这几个概念可以认为是以下常规概念的抽象：

- ■ Channel—Sockets
- ■ EventLoop—Control flow, multithreading, concurrency
- ■ ChannelFuture—Asynchronous notification

netty这几个概念之间的关系是：

- ■ 一个EventLoopGroup包含多个EventLoops。
- ■ 每个EventLoops在它生命周期内都关联在为一个线程。
- ■ 在每个EventLoops中处理的所有I/O事件都是在它唯一确定的线程中处理。
- ■ 每个Channel在生命周期内仅能注册到一个EventLoop.一个EventLoop可以对应一个或多个Channels.

```
                1     *           1     *    
EventLoopGroup<---------EventLoop--------->Channel
                           ^                 |
                           |1                | 1
                           |-----------------|
```

## ChannelHandler and ChannelPipeline

从开发者角度来说，

- ChannelHandler是最基础的netty组件，它是作为应用app逻辑的容器。

- ChannelPipeline是作为ChannelHandler链的容器，并提供对该链进行维护的API。换句话，是一个拦截流经Channel的入站和出站事件的ChannelHandler 实例链

当创建个Channel时，它将被自动关联到它专属ChannelPipeline。并且ChannelHandlers被安装到ChannelPipeline，这个安装过程如下：

- ■ 一个ChannelInitializer实现被注册在ServerBootstrap. 其中ChannelInitializer是一个ChannelHandler的子类。
- ■ 当ChannelInitializer.initChannel()被调用时,ChannelInitializer会安装用户定义的ChannelHandlers到pipeline中.
- ■ 最后，ChannelInitializer将自己从ChannelPipeline移除.

The movement of an event through the pipeline is the work of the ChannelHandlers
that have been installed during the initialization, or bootstrapping phase of the application.
These objects receive events, execute the processing logic for which they have
been implemented, and pass the data to the next handler in the chain. The order in
which they are executed is determined by the order in which they were added. For all
practical purposes, it’s this ordered arrangement of ChannelHandlers that we refer to
as the ChannelPipeline.

## ByteBuf—Netty 的数据容器



## 引导-Bootstrapping

通过Bootstrapping将ChannelPipeline、ChannelHandler 和EventLoop 这些部分组织起来，成为一个可实际运行的应用程序。

Netty中Bootstrap的handler和childHandler区别: handler在初始化时就会执行，而childHandler会在客户端成功connect后才执行，这是两者的区别。

