

获取线程转储信息

```sh
$jps
8468 Main
19256 Jps

$jstack -l 18324 > stack.txt
```

示例程序

```java
public class DeadlockProgram { 
    public static void main(String[] args) throws Exception { 
        Object resourceA = new Object(); 
        Object resourceB = new Object(); 
        Thread threadLockingResourceAFirst = new Thread(new DeadlockRunnable(resourceA, resourceB)); 
        Thread threadLockingResourceBFirst = new Thread(new DeadlockRunnable(resourceB, resourceA)); 
        threadLockingResourceAFirst.start(); 
        Thread.sleep(500); threadLockingResourceBFirst.start(); 
    
    } 
    
    private static class DeadlockRunnable implements Runnable { 
        private final Object firstResource; 
        private final Object secondResource; 
        public DeadlockRunnable(Object firstResource, Object secondResource) { 
            this.firstResource = firstResource; 
            this.secondResource = secondResource; 
        } 
        
        @Override public void run() { 
            try { 
                synchronized(firstResource) { 
                    printLockedResource(firstResource); 
                    Thread.sleep(1000); 
                    synchronized(secondResource) { 
                        printLockedResource(secondResource); 
                    } 
                } 
            } 
            catch (InterruptedException e) { 
                System.out.println("Exception occurred: " + e); 
            } 
        } 
        
        private static void printLockedResource(Object resource) { 
            
            System.out.println(Thread.currentThread().getName() + ": locked resource -> " + resource); 
        } 
    } 
} 
```


# dump信息解读

origin： [How to Read a Thread Dump](https://dzone.com/articles/how-to-read-a-thread-dump)

## 1. Introductory information

首行：dump文件产生时间；第二行： dump信息所在JVM的诊断信息
```
2020-04-26 13:48:48
Full thread dump Java HotSpot(TM) 64-Bit Server VM (11.0.6+8-LTS mixed mode):
```

## 2. General Threading Information

下面第二部分开始提供转储dump信息时运行的线程（thread）信息

```
Threads class SMR info:
_java_thread_list=0x00000000638371a0, length=12, elements={
0x0000000061e75800, 0x0000000061e7f000, 0x0000000061ea0800, 0x0000000061ea3800,
0x0000000061ea6800, 0x0000000061eab000, 0x0000000061eb4800, 0x00000000211fe800,
0x0000000062e73800, 0x0000000063a72000, 0x0000000064e5a000, 0x00000000004c5800
}
```

这部分包括线程列表安全内存回收（thread list Safe Memory Reclamation (SMR) ）信息, 这部分枚举了所有非JVM内部线程的地址（例如，non-VM and non-Garbage Collection (GC)）。如果我们检查这些地址，就会发现它们对应于转储中每个编号的线程的tid值（即本机线程对象的地址，而不是线程ID，我们将很快看到）（注意，省略号用于隐藏多余的信息）：

```
"Reference Handler" #2 daemon prio=10 os_prio=2 cpu=0.00ms elapsed=40.20s tid=0x0000000061e75800 nid=0x3fa0 waiting on condition  [0x000000006222f000]
   java.lang.Thread.State: RUNNABLE
	at java.lang.ref.Reference.waitForReferencePendingList(java.base@11.0.6/Native Method)
	at java.lang.ref.Reference.processPendingReferences(java.base@11.0.6/Reference.java:241)
	at java.lang.ref.Reference$ReferenceHandler.run(java.base@11.0.6/Reference.java:213)

   Locked ownable synchronizers:
	- None

"Finalizer" #3 daemon prio=8 os_prio=1 cpu=0.00ms elapsed=40.21s tid=0x0000000061e7f000 ...
"Signal Dispatcher" #4 daemon prio=9 os_prio=2 cpu=0.00ms elapsed=40.18s tid=0x0000000061ea0800 ...
...
"Thread-0" #12 prio=5 os_prio=0 cpu=46.80ms elapsed=3822.52s tid=0x0000000063a72000 nid=0x4444 waiting for monitor entry  [0x000000006479e000]
   java.lang.Thread.State: BLOCKED (on object monitor)
	at DeadlockProgram$DeadlockRunnable.run(DeadlockProgram.java:26)
	- waiting to lock <0x000000008c328690> (a java.lang.Object)
	- locked <0x000000008c3143f0> (a java.lang.Object)
	at java.lang.Thread.run(java.base@11.0.6/Thread.java:834)

   Locked ownable synchronizers:
	- None
...
"DestroyJavaVM" #14 prio=5 os_prio=0 cpu=936.01ms elapsed=38.72s tid=0x00000000004c5800 ...
```

## 3. threads

紧跟SMR信息的是线程列表。例如上面的第一个“Reference Handler”线程，以它为例解读信息如下

### 3.1 Thread Summary

第一行是该thread的汇总信息, 包括下面的项:


| SECTION |	EXAMPLE	            |DESCRIPTION |
|---------|---------------------|------------|
|Name	  |"Reference Handler"	|线程名字，通过Threadobject类setName方法设置，可通过对象getName方法获取.|
|ID	      |#2                   |线程对象unique ID. 该ID只读，可通过线程对象getId方法获得.|
|Daemon status|	daemon          |是否为守护线程(daemon thread)的标记，非守护线程无该标记  |
|Priority |	prio=10         |java thread的数字优先级。与调度Java线程时OS线程的优先级没有关系。可通过线程对象setPriority、getPriority方法设置获取。|
|OS Thread Priority|os_prio=2   |操作系统线程优先级。此优先级可以不同于java thread优先级，并对应于Java线程所调度的OS线程。|
|Address  |tid=0x00000250e4979000 | java thread地址，即后端JNI C++ Thread object真正地址。注意getId方法返回的不是它 |
|OS Thread ID| nid=0x3c28       | java thread对应OS线程的unique ID  |
|Status |waiting on condition |线程当前状态。它提供基本线程状态（见下文）以外的补充信息，可用于发现线程的预期操作（即线程试图获取锁或在阻塞时等待条件）|
|Last Known Java Stack Pointer|	[0x000000006222f000] | 与线程关联的堆栈的最后一个已知堆栈指针，该值由本地C++代码通过JNI提供，提供了last_Java_sp()获取它|
||||


### 3.2 Thread State

第二行是当前线程状态， 所有状态值见 [Thread.State enumeration](https://docs.oracle.com/javase/10/docs/api/java/lang/Thread.State.html):

包括： NEW，RUNNABLE，BLOCKED，WAITING，TIMED_WAITING，TERMINATED

### 3.3 Thread Stack Trace
之后的部分是包含stack trace。它类似于在发生未捕获异常时打印的堆栈跟踪. 这部分是可选的，例如在`"Thread-0" #12`就可以看到和其他标准线程stack trace不一样的地方

```
"Thread-0" #12 prio=5 os_prio=0 cpu=46.80ms elapsed=3822.52s tid=0x0000000063a72000 nid=0x4444 waiting for monitor entry  [0x000000006479e000]
   java.lang.Thread.State: BLOCKED (on object monitor)
	at DeadlockProgram$DeadlockRunnable.run(DeadlockProgram.java:26)
	- waiting to lock <0x000000008c328690> (a java.lang.Object)
	- locked <0x000000008c3143f0> (a java.lang.Object)
	at java.lang.Thread.run(java.base@11.0.6/Thread.java:834)

   Locked ownable synchronizers:
	- None
```

在这个堆栈跟踪中，我们可以看到已经添加了锁定信息，这告诉我们这个线程正在等待一个地址为`0x000000008c328690`（java.lang.object类型）的对象上的锁，在堆栈跟踪中的这一点上，持有地址为`0x000000008c3143f0`（也是java.lang.object类型）对象上的锁。在诊断死锁时，此附加锁信息非常重要，我们将在以下各节中看到。

### 3.4 Locked Ownable Synchronizer

线程信息的最后一部分是线程独占的同步器（可用于同步的对象，如锁）列表。根据官方JAVA文档"an ownable synchronizer is a synchronizer that may be exclusively owned by a thread and uses AbstractOwnableSynchronizer (or its subclass) to implement its synchronization property. ReentrantLock and the write-lock (but not the read-lock) of ReentrantReadWriteLock are two examples of ownable synchronizers provided by the platform.

关于锁定的可拥有同步器（locked ownable synchronizers）信息, 参考[this Stack Overflow post](https://stackoverflow.com/questions/41300520/what-is-locked-ownable-synchronizers-in-thread-dump).



## 4. JVM Threads
下一部分包含的是绑定到操作系统的JVM内部（非应用程序）线程。由于这些线程不存在于Java应用程序中，因此它们没有线程ID。这些线程通常由GC线程和JVM用来运行和维护Java应用程序的其他线程组成：

```
|"VM Thread"| os_prio=2 cpu=0.00ms elapsed=40.21s tid=0x0000000061e2e800 nid=0x2b40 runnable  

"GC Thread#0" os_prio=2 cpu=0.00ms elapsed=40.23s tid=0x00000000004dc000 nid=0x504 runnable  

"GC Thread#1" os_prio=2 cpu=0.00ms elapsed=39.37s tid=0x0000000063637000 nid=0x51b4 runnable  

"GC Thread#2" os_prio=2 cpu=0.00ms elapsed=38.73s tid=0x0000000064a5f800 nid=0x57a4 runnable  

"GC Thread#3" os_prio=2 cpu=0.00ms elapsed=38.73s tid=0x000000006387f800 nid=0x477c runnable  

"G1 Main Marker" os_prio=2 cpu=0.00ms elapsed=40.23s tid=0x000000000050d000 nid=0x3764 runnable  

"G1 Conc#0" os_prio=2 cpu=0.00ms elapsed=40.23s tid=0x000000000050e000 nid=0x48f4 runnable  

"G1 Refine#0" os_prio=2 cpu=0.00ms elapsed=40.23s tid=0x0000000021188800 nid=0x303c runnable  

"G1 Refine#1" os_prio=2 cpu=0.00ms elapsed=39.36s tid=0x0000000062f10000 nid=0x3b60 runnable  

"G1 Young RemSet Sampling" os_prio=2 cpu=0.00ms elapsed=40.23s tid=0x0000000021189000 nid=0x5540 runnable  
"VM Periodic Task Thread" os_prio=2 cpu=0.00ms elapsed=40.08s tid=0x0000000062f3c000 nid=0x468c waiting on condition  
```

## 5. JNI Global References

本节显示的是JVM通过JNI维护的全局引用的数量。在某些情况下，这些引用可能会导致内存泄漏，并且不会自动垃圾回收。

```
JNI global refs: 8, weak refs: 0
```

对于许多简单的问题，这些信息是没用的，但是理解这些全局引用的重要性是很重要的。想了解更多相关信息，请参阅[this Stack Overflow post](https://stackoverflow.com/questions/112603/what-is-jni-global-reference)。

## 6. Deadlocked Threads

最后一部分包含有关已发现死锁（discovered deadlocks）的信息。情况并非总是这样：如果应用程序没有一个或多个检测到的死锁，则不会有这部分。由于我们的示例应用程序会发生死锁，因此有下面的`eadlocked Threads`信息：

```
Found one Java-level deadlock:
=============================
"Thread-0":
  waiting to lock monitor 0x0000000061e7e780 (object 0x000000008c328690, a java.lang.Object),
  which is held by "Thread-1"
"Thread-1":
  waiting to lock monitor 0x0000000061e7e680 (object 0x000000008c3143f0, a java.lang.Object),
  which is held by "Thread-0"

Java stack information for the threads listed above:
===================================================
"Thread-0":
	at DeadlockProgram$DeadlockRunnable.run(DeadlockProgram.java:26)
	- waiting to lock <0x000000008c328690> (a java.lang.Object)
	- locked <0x000000008c3143f0> (a java.lang.Object)
	at java.lang.Thread.run(java.base@11.0.6/Thread.java:834)
"Thread-1":
	at DeadlockProgram$DeadlockRunnable.run(DeadlockProgram.java:26)
	- waiting to lock <0x000000008c3143f0> (a java.lang.Object)
	- locked <0x000000008c328690> (a java.lang.Object)
	at java.lang.Thread.run(java.base@11.0.6/Thread.java:834)

Found 1 deadlock.
```


