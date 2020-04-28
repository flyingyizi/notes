

[书源码](http://ifeve.com/artconcurrentbook-source/)

## java 锁 

基本使用

Java中的每一个对象都可以作为锁。具体表现为以下3种形式。

[Guide to the Synchronized Keyword in Java](https://www.baeldung.com/java-synchronized)

The synchronized keyword can be used on different levels:

- Instance methods  ：Instance methods are synchronized over the instance of the class owning the method. Which means only one thread per instance of the class can execute this method.

```java
public synchronized void synchronisedCalculate() {
    setSum(getSum() + 1);
}
@Test
public void givenMultiThread_whenMethodSync() {
    ExecutorService service = Executors.newFixedThreadPool(3);
    SynchronizedMethods method = new SynchronizedMethods();
 
    IntStream.range(0, 1000)
      .forEach(count -> service.submit(method::synchronisedCalculate));
    service.awaitTermination(1000, TimeUnit.MILLISECONDS);
 
    assertEquals(1000, method.getSum());
}
```

- Static methods : These methods are synchronized on the Class object associated with the class and since only one Class object exists per JVM per class, only one thread can execute inside a static synchronized method per class, irrespective of the number of instances it has.

```java
public static synchronized void syncStaticCalculate() {
    staticSum = staticSum + 1;
}
@Test
public void givenMultiThread_whenStaticSyncMethod() {
    ExecutorService service = Executors.newCachedThreadPool();
 
    IntStream.range(0, 1000)
      .forEach(count -> 
        service.submit(BaeldungSynchronizedMethods::syncStaticCalculate));
    service.awaitTermination(100, TimeUnit.MILLISECONDS);
 
    assertEquals(1000, BaeldungSynchronizedMethods.staticSum);
}
```

- Code blocks : Sometimes we do not want to synchronize the entire method but only some instructions within it. This can be achieved by applying synchronized to a block:

```java
public void performSynchronisedTask() {
    synchronized (this) {
        setCount(getCount()+1);
    }
}
@Test
public void givenMultiThread_whenBlockSync() {
    ExecutorService service = Executors.newFixedThreadPool(3);
    BaeldungSynchronizedBlocks synchronizedBlocks = new BaeldungSynchronizedBlocks();
 
    IntStream.range(0, 1000)
      .forEach(count -> 
        service.submit(synchronizedBlocks::performSynchronisedTask));
    service.awaitTermination(100, TimeUnit.MILLISECONDS);
 
    assertEquals(1000, synchronizedBlocks.getCount());
}
```


现在我们介绍避免死锁的几个常见方法。

- ·避免一个线程同时获取多个锁。
- ·避免一个线程在锁内同时占用多个资源，尽量保证每个锁只占用一个资源。
- ·尝试使用定时锁，使用lock.tryLock（timeout）来替代使用内部锁机制。
- ·对于数据库锁，加锁和解锁必须在一个数据库连接里，否则会出现解锁失败的情况。

对这些挑战，建议多使用JDK并发包提供的并发容器和工具类来解决并发问题，因为这些类都已经通过了充分的测试和优化，均可解决了本章提到的几个挑战。

## java 原子操作

直接使用JDK的原子操作，不要使用类似下面的循环CAS

- 使用循环CAS实现原子操作

  自旋CAS实现的基本思路就是循环进行CAS操作直到成功为止

```java
class Counter {

    private AtomicInteger atomicI = new AtomicInteger(0);
    private int           i       = 0;
    /**
     * 使用CAS实现线程安全计数器
     */
    private void safeCount() {
        for (;;) {
            int i = atomicI.get();
            boolean suc = atomicI.compareAndSet(i, ++i);
            if (suc) {
                break;
            }
        }
    }
}    
```

- JDK并发包
  
  从Java 1.5开始，JDK的并发包里提供了一些类来支持原子操作，如AtomicBoolean（用原子
方式更新的boolean值）、AtomicInteger ...

## java 线程

- 线程基本使用

```java
        Thread thread = new Thread( new Runnable(){

            @Override
            public void run() {
                // TODO Auto-generated method stub

            }

        });
        thread.start();
```

## java 内存模型

在Java中，所有实例域、静态域和数组元素都存储在堆内存中，堆内存在线程之间共享

# 并发编程基础

java 线程优先级不能作为程序正确性的依赖，因为操作系统可以完全不用理会Java线程对于优先级的设定

守护线程是在程序完成但线程仍在运行时不阻止JVM退出的线程。守护进程线程的一个例子是垃圾回收。

通过Thread方法interrupt(),cancel()可以安全的终止线程 stop()方法是deprecate，因为它不能安全释放资源。


- 并发库 `java.util.concurrent`
```java
import java.util.concurrent.TimeUnit;
TimeUnit.SECONDS.sleep(seconds);
```

## 中断

线程通过方法isInterrupted()来进行判断是否被中断，如果历史上是“running->interrupted->running”，那isInterrupted()还是返回false。

也可以调用静态方法Thread.interrupted()对当前线程的中断标识位进行复位。

```java
Thread busyThread = new Thread(new BusyRunner(), "BusyThread");
busyThread.interrupt();
System.out.println("BusyThread interrupted is " + busyThread.isInterrupted());
```

抛出InterruptedException的方法（例如Thread.sleep(long millis)方法）这些方法在抛出InterruptedException之前，Java虚拟机会先将该线程的中断标识位清除，然后抛出InterruptedException，此时调用isInterrupted()方法将会返回false。

## 线程间通信

## 　volatile和synchronized关键字

关键字volatile可以用来修饰字段（成员变量），就是告知程序任何对该变量的访问均需要从共享内存中获取，而对它的改变必须同步刷新回共享内存，它能保证所有线程对变量访问的可见性。



# 

## LinkedTransferQueue

[LinkedTransferQueue in Java with Examples](https://www.geeksforgeeks.org/linkedtransferqueue-in-java-with-examples/)

Class Hierarchy:

```text
java.lang.Object
 ↳ java.util.AbstractCollection<E>
    ↳ java.util.AbstractQueue<E>
        ↳ java.util.concurrent.LinkedTransferQueue<E>
```

```java
LinkedTransferQueue<Integer> LTQ 
            = new LinkedTransferQueue<Integer>(); 
  
// Add numbers to end of LinkedTransferQueue 
LTQ.add(7855642); 
LTQ.add(35658786); 
LTQ.add(5278367); 
LTQ.add(74381793);

// create object of LinkedTransferQueue 
        // using LinkedTransferQueue(Collection c) constructor 
LinkedTransferQueue<Integer> LTQ2 
    = new LinkedTransferQueue<Integer>(LTQ); 
  
```

