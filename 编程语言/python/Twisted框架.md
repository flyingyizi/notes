
[Twisted中文教程](https://www.gitbook.com/book/fengyouchao/twisted-intro-cn)

异步编程： 简单理解就是在单线程下的任务交错完成。

虽然在操作系统层次，对单个处理器，多线程编程其实也是任务交错执行，但从逻辑角度还是必须按照多线程并发去理解，以避免迁移到多处理机的机器上出问题。

## reactor

- Twisted的reactor只有通过调用reactor.run()来启动。
- reactor循环是在其开始的进程中运行，也就是运行在主进程中。
- 一旦启动，就会一直运行下去。reactor就会在程序的控制下（或者具体在一个启动它的线程的控制下）。
- reactor循环并不会消耗任何CPU的资源。
- 并不需要显式的创建reactor，只需要引入就OK了。因为reactor是Singleton模式，即在一个程序中只能有一个reactor，并且只要你引入它就相应地创建一个。

下面演示了两种启动reactor的方式,它们的不同是底层实现reactor的技术不一样

```python
#方式1使用select
from twisted.internet import reactor
reactor.run()

#方式2：使用poll
from twited.internet import pollreactor
pollreactor.install()
from twisted.internet import reactor
reactor.run()
```

使用reactor的常用方法：

```python
from twisted.internet import reactor

# tell the Twisted reactor to monitor this xx for reading，要求xx必须实现IFileDescriptor接口
reactor.addReader(xx)
# stop monitoring this socket
reactor.removeReader(xx)
# see if there are any poetry sockets left
for reader in reactor.getReaders():
    pass

#注册callback，当reactor run时，调用xx callback
reactor.callWhenRunning(hello)

#注册callback， 参数1是几秒后执行， 参数2是callback
reactor.callLater(1, func)

#启动reactor，进入监听态
reactor.run()

#停止reactor, 显然它只有在注册到reactor的callback中被调用才有执行机会
reactor.stop()
```


## deferred

抽象机制-Deferred-让程序员在使用回调时更简便。

一个Deferred有一对回调链，一个是为针对正确结果，另一个针对错误结果。新创建的Deferred的这两条链是空的。我们可以向两条链里分别添加callback与errback。其后，就可以用正确的结果或异常来激活Deferred。激活Deferred意味着以我们添加的顺序激活callback或errback。下图展示了一个拥有callback/errback链的Deferred对象.

![](#images/deferred.png)

异步函数返回一个deferred，对用户意味着：

    我是一个异步函数。不管你想要什么，可能现在马上都得不到。但当结果来到时，我会激活这个deferred的callback链并返回结果。或者当出错时，相应地激活errback链并返回出错信息。


使用deferred的常用方法：

```python
from twisted.internet import defer
#新建个deferred
d = defer.Deferred()

# add a callback/errback pair to the chain
d.addCallbacks(got_poem, poem_failed)

# fire the chain with a normal result
d.callback('This poem is short.')

# fire the chain with an error result
d.errback(Failure(Exception('I have failed.')))

#add a callback to callback chain ,and to errback chain
d.addBoth(poem_done)
```

下面演示了在deferred被激活后是如何销毁其引用的。这种方式普便存在于Twisted的源代码中，这样做可以保证我们不会激活一个deferred两次。这也为Python的垃圾回收带来的方便。

```python
class xxx:
    def __init__(self, deferred):
        self.deferred = deferred

    def yy(self, poem):
        if self.deferred is not None:
            d, self.deferred = self.deferred, None
            d.callback(poem)
    def zz(self, connector, reason):
        if self.deferred is not None:
            d, self.deferred = self.deferred, None
            d.errback(reason)
```
