

[书源码](http://ifeve.com/artconcurrentbook-source/)

## java 锁 

基本使用

Java中的每一个对象都可以作为锁。具体表现为以下3种形式。

[Guide to the Synchronized Keyword in Java](https://www.baeldung.com/java-synchronized)

The synchronized keyword can be used on different levels:

- 实例方法  ：在拥有该方法的类实例上同步， 这意味着每个该类的实例仅有一个线程可以执行该方法。 Instance methods are synchronized over the instance of the class owning the method. Which means only one thread per instance of the class can execute this method.
    
    instance method能够保证在同一时刻最多只有一个线程执行该段代码。

    ```java
    public class Sync {
      ...
      private int sum;
      public synchronized void synchronisedCalculate() {
        setSum(getSum() + 1);
      }
    }

    public class SyncTest  {

    @Test
    public void givenMultiThread_whenMethodSync() {
        ExecutorService service = Executors.newFixedThreadPool(3);
        Sync method = new Sync();
        
        //拉起了100个待执行task，由于线程池大小是3，导致效果是3个，3个..
        //总共1000个。
        //同时由于执行的是实例同步方法，因此实际是串行执行。执行的结果会严格是1000.如果不是这个同步，执行的结果会是小于等于1000.
        IntStream.range(0, 1000)
            .forEach(count -> service.submit(method::synchronisedCalculate));

        try {
        service.awaitTermination(1000, TimeUnit.MILLISECONDS);
        } catch (InterruptedException e) {
        e.printStackTrace();
        }

        assertEquals(1000, method.getSum());
    }
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


java 线程优先级不能作为程序正确性的依赖，因为操作系统可以完全不用理会Java线程对于优先级的设定守护线程是在程序完成但线程仍在运行时不阻止JVM退出的线程。守护进程线程的一个例子是垃圾回收。

通过Thread方法interrupt(),cancel()可以安全的终止线程 stop()方法是deprecate，因为它不能安全释放资源。


### 并发库 `java.util.concurrent`
```java
import java.util.concurrent.TimeUnit;
TimeUnit.SECONDS.sleep(seconds);
```

## Future

Future的核心思想是：一个方法f，计算过程可能非常耗时，等待f返回，显然不明智。可以在调用f的时候，立马返回一个Future，可以通过Future这个数据结构去控制方法f的计算过程。这里的控制包括：

- get方法：获取计算结果（如果还没计算完，也是必须等待的）

- cancel方法：还没计算完，可以取消计算过程

- isDone方法：判断是否计算完

- isCancelled方法：判断计算是否被取消

同时引入了Callable接口，Callable可以看作是Runnable接口的补充，call方法带有返回值，并且可以抛出异常。

下面演示对Java原生Future模式最基本的应用

```java
package test;

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.FutureTask;

public class FutureCook {

    public static void main(String[] args) throws InterruptedException, ExecutionException {
        long startTime = System.currentTimeMillis();
        // 第一步 网购厨具
        Callable<Chuju> onlineShopping = new Callable<Chuju>() {

            @Override
            public Chuju call() throws Exception {
                System.out.println("第一步：下单");
                System.out.println("第一步：等待送货");
                Thread.sleep(5000);  // 模拟送货时间
                System.out.println("第一步：快递送到");
                return new Chuju();
            }
            
        };
        FutureTask<Chuju> task = new FutureTask<Chuju>(onlineShopping);
        new Thread(task).start();
        // 第二步 去超市购买食材
        Thread.sleep(2000);  // 模拟购买食材时间
        Shicai shicai = new Shicai();
        System.out.println("第二步：食材到位");
        // 第三步 用厨具烹饪食材
        if (!task.isDone()) {  // 联系快递员，询问是否到货
            System.out.println("第三步：厨具还没到，心情好就等着（心情不好就调用cancel方法取消订单）");
        }
        Chuju chuju = task.get();
        System.out.println("第三步：厨具到位，开始展现厨艺");
        cook(chuju, shicai);
        
        System.out.println("总共用时" + (System.currentTimeMillis() - startTime) + "ms");
    }
    
    //  用厨具烹饪食材
    static void cook(Chuju chuju, Shicai shicai) {}
    
    // 厨具类
    static class Chuju {}
    
    // 食材类
    static class Shicai {}

}
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

### 线程中join和yield

[Java多线程系列--“基础篇”08之 join()](https://www.cnblogs.com/skywang12345/p/3479275.html)

join() 的作用：让“主线程”等待“子线程”结束之后才能继续运行。

下面的例子，在Father主线程中，通过new Son()新建“子线程s”。接着通过s.start()启动“子线程s”，并且调用s.join()。在调用s.join()之后，Father主线程会一直等待，直到“子线程s”运行完毕；在“子线程s”运行完毕之后，Father主线程才能接着运行。 这也就是我们所说的“join()的作用，是让主线程会等待子线程结束之后才能继续运行”！

```java
// 主线程
public class Father extends Thread {
    public void run() {
        Son s = new Son();
        s.start();
        s.join();
        ...
    }
}
// 子线程
public class Son extends Thread {
    public void run() {
        ...
    }
}
```


###　volatile和synchronized关键字

关键字volatile可以用来修饰字段（成员变量），就是告知程序任何对该变量的访问均需要从共享内存中获取，而对它的改变必须同步刷新回共享内存，它能保证所有线程对变量访问的可见性。

### 等待/通知机制

Object类中关于等待/唤醒的API详细信息如下：

- notify()      -- 唤醒在此对象监视器上等待的单个线程。
- notifyAll()   -- 唤醒在此对象监视器上等待的所有线程。
- wait()        -- 让当前线程处于“等待(阻塞)状态”，“直到其他线程调用此对象的 notify() 方法或 notifyAll() 方法”，当前线程被唤醒(进入“就绪状态”)。
- wait(long timeout)    -- 让当前线程处于“等待(阻塞)状态”，“直到其他线程调用此对象的 notify() 方法或 notifyAll() 方法，或者超过指定的时间量”，当前线程被唤醒(进入“就绪状态”)。
- wait(long timeout, int nanos)  -- 让当前线程处于“等待(阻塞)状态”，“直到其他线程调用此对象的 notify() 方法或 notifyAll() 方法，或者其他某个线程中断当前线程，或者已超过某个实际时间量”，当前线程被唤醒(进入“就绪状态”)。



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

