
进程是资源分配的最小单位，而线程是CPU调度的的最小单位。进程不仅包括可执行程序的代码段，还包括一系列的资源，比如：打开的文件、内存、CPU时间、信号量、多个执行线程流等等。而线程可以共享进程内的资源空间。

在Linux内核中，进程和线程都使用struct task_struct结构来进行抽象描述。

主调度器（就是schedule函数）的主要功能是从该CPU的runqueue找到一个最合适的进程调度执行。

与调度相关的某些字段:
```c++
struct task_struct {
    /* ... */
    
    /* 进程状态 */
    volatile long			state; //ref linux\include\linux\sched.h, e.g.TASK_RUNNING

    /* 调度优先级相关，策略相关 */
	int				prio;
	int				static_prio;
	int				normal_prio;
	unsigned int			rt_priority;
    unsigned int			policy;
    
    /* 调度类，调度实体相关，任务组相关等 */
    const struct sched_class	*sched_class;
	struct sched_entity		se;
	struct sched_rt_entity		rt;
#ifdef CONFIG_CGROUP_SCHED
	struct task_group		*sched_task_group;
#endif
	struct sched_dl_entity		dl;
    
    /* 进程之间的关系相关 */
    	/* Real parent process: */
	struct task_struct __rcu	*real_parent;

	/* Recipient of SIGCHLD, wait4() reports: */
	struct task_struct __rcu	*parent;

	/*
	 * Children/sibling form the list of natural children:
	 */
	struct list_head		children;
	struct list_head		sibling;
	struct task_struct		*group_leader;
    
    /* ... */
}
```

全局变量： 
- struct task_struct init_task = INIT_TASK(init_task);
- DEFINE_PER_CPU(struct task_struct *, current_task) = &init_task;




# scheduler 调度器

![scheduler 调度器概览](https://img2018.cnblogs.com/blog/1771657/202002/1771657-20200201170552968-1185293225.png)

内核默认提供了5个调度器，Linux内核使用struct sched_class来对调度器进行抽象：

- Stop调度器， stop_sched_class：仅由内核使用.优先级最高的调度类，可以抢占其他所有进程，不能被其他进程抢占；
- Deadline调度器， dl_sched_class：使用红黑树，把进程按照绝对截止期限进行排序，选择最小进程进行调度运行；
- RT调度器， rt_sched_class：实时调度器，为每个优先级维护一个队列；
- CFS调度器， cfs_sched_class：完全公平调度器，采用完全公平调度算法，引入虚拟运行时间概念；
- IDLE-Task调度器， idle_sched_class：仅由内核使用. 空闲调度器，每个CPU都会有一个idle线程，当没有其他进程可以调度时，调度运行idle线程；

调度策略，调度策略是供用户程序来选择调度器：
- SCHED_DEADLINE：限期进程调度策略，使task选择Deadline调度器来调度运行, 即进入"`runqueues.dl`"队列；
- SCHED_RR：实时进程调度策略，时间片轮转，进程用完时间片后加入优先级对应运行队列的尾部，把CPU让给同优先级的其他进程, 即进入"`runqueues.rt`"队列；
- SCHED_FIFO：实时进程调度策略，先进先出调度没有时间片，没有更高优先级的情况下，只能等待主动让出CPU, 即进入"`runqueues.rt`"队列；
- SCHED_NORMAL：普通进程调度策略，使task选择CFS调度器来调度运行, 即进入"`runqueues.cfs`"队列；
- SCHED_BATCH：普通进程调度策略，批量处理，使task选择CFS调度器来调度运行, 即进入"`runqueues.cfs`"队列；
- SCHED_IDLE：普通进程调度策略，使task以最低优先级选择CFS调度器来调度运行, 即进入"`runqueues.cfs`"队列；

## runqueues 运行队列

常常使用宏 "`this_rq`"来获取本地的运行队列"runqueues"
```c++
//全局变量： 
DEFINE_PER_CPU_SHARED_ALIGNED(struct rq, runqueues);  
```

Linux内核使用struct rq结构来描述运行队列，关键字段如下：
```c++
/*
 * This is the main, per-CPU runqueue data structure.
 *
 * Locking rule: those places that want to lock multiple runqueues
 * (such as the load balancing or the thread migration code), lock
 * acquire operations must be ordered by ascending &runqueue.
 */
struct rq {
	/* runqueue lock: */
	raw_spinlock_t lock;

	/*
	 * nr_running and cpu_load should be in the same cacheline because
	 * remote CPUs use both these fields when doing load calculation.
	 */
	unsigned int nr_running;
    
    /* 三个调度队列：CFS调度，RT调度，DL调度 */
	struct cfs_rq cfs; //cfs_rq.tasks_timeline队列
	struct rt_rq rt;  //rt_rq.active数组的每个元素都是一个特定优先级的队列
	struct dl_rq dl;  //dl_rq.root队列

    /* stop指向迁移内核线程， idle指向空闲内核线程 */
    struct task_struct *curr, *idle, *stop;
    
    /* ... */
}  
```

## 组调度

[组调度（task_group）](https://www.cnblogs.com/LoyenWang/p/12459000.html)是使用Linux cgroup(control group)的cpu子系统来实现的，可以将进程进行分组，按组来分配CPU资源等。
比如，看一个实际的例子：
A和B两个用户使用同一台机器，A用户16个进程，B用户2个进程，如果按照进程的个数来分配CPU资源，显然A用户会占据大量的CPU时间，这对于B用户是不公平的。组调度就可以解决这个问题，分别将A、B用户进程划分成组，并将两组的权重设置成占比50%即可。

![任务分组示意](https://img2018.cnblogs.com/blog/1771657/202002/1771657-20200201170652689-605326625.png)

创建的taskgroup会同时加入task_groups和root_task_group为根节点的树里面。 注意root_task_group也会加入task_groups
```c++
//全局变量： 
#ifdef CONFIG_CGROUP_SCHED
/*
 * Default task group.
 * Every task in system belongs to this group at bootup.
 */
struct task_group root_task_group;  // 充当task_group的根节点，以它为根构建树状结构
/**
 * LIST_HEAD定义在linux\include\linux\list.h
 * 定义全局链表task_groups，创建的task_group会添加到这个链表中；
*/
LIST_HEAD(task_groups);

/* Cacheline aligned slab cache for task_group */
static struct kmem_cache *task_group_cache __read_mostly;
#endif
```

```c++
/* task group related information */
struct task_group {
    /* ... */

    /* 为每个CPU都分配一个CFS调度实体和CFS运行队列 */
    //为每个cpu都分配不好理解的话，可以参考sched_init里面为root_task_group初始化se/cfs_rq的代码来理解
#ifdef CONFIG_FAIR_GROUP_SCHED
	/* schedulable entities of this group on each cpu */
	struct sched_entity **se;
	/* runqueue "owned" by this group on each cpu */
	struct cfs_rq **cfs_rq;
	unsigned long shares;
#endif

    /* 为每个CPU都分配一个RT调度实体和RT运行队列 */
#ifdef CONFIG_RT_GROUP_SCHED
	struct sched_rt_entity **rt_se;
	struct rt_rq **rt_rq;

	struct rt_bandwidth rt_bandwidth;
#endif

    /* task_group之间的组织关系 */
	struct rcu_head rcu;
	struct list_head list;

	struct task_group *parent;
	struct list_head siblings;
	struct list_head children;

    /* ... */
};
```

## Linux调度器开放给用户空间的接口

总结：
- 调度器就是按照优先级（指rt priority）来工作，优先级高的永远是优先调度。范围落在[1,99]的rt priority是实时进程，而rt priority等于0的是普通进程。
- 对于普通进程，调度器还要根据nice value来进行调整。

一般习惯： 将"nice value"称为静态优先级，"rt-priority"称为实时优先级


### 主动进程切换

进程切换有两种，一种是当进程由于需要等待某种资源而无法继续执行下去，这时候只能是主动将自己挂起（调用schedule函数），引发一次任务调度过程。

### 修改调用进程的nice value
实际上调整nice value就是调整调度器分配给该进程的CPU时间
```c++
      //nice函数用来修改调用进程的nice value，
      /**
       * 例子：程序调用nice(3)，则将当前进程的nice value增加3，这也就是意味着该进程的优先级降
       * 低3个level（提升nice value也就是对别人更加nice，自己的优先级就会低）。如果程序调用nice(-5)，
       * 则将当前进程的nice value减去5，这也就是意味着该进程的优先级提升5个level。当调用错误的时候返回-1
       * 
       * 返回值：linux的系统调用和c库都是采用了操作成功返回0的方式。调用后如果想要得到当前优先级，需要调用getpriority函数
      */
      #include <unistd.h>
       int nice(int inc);
```

从前面的描述中，我们了解到了nice的函数的限制，例如只能修改自己的nice value，无法获取当前的nice value值等，为此我们给出加强版本的nice接口，也就是getpriority/setpriority函数了。getpriority/setpriority函数定义如下：
```c++
#include <sys/time.h>
#include  <sys/resource.h>
/**
 * 当which等于PRIO_PROCESS的时候，who需要传入一个process id的参数，getpriority将返回指定进程的nice value。
 * 当which等于PRIO_PGRP的时候，who需要传入一个process group id的参数，此时getpriority将返回指定进程组中优先级最高的那个（BTW，nice value是最小的）。
 * 当which等于PRIO_USER的时候，who需要user id的信息，这时候，getpriority将返回属于该user的所有进程中nice value最小的那个。
 * who等于0说明要get或者set的对象是当前进程（或者当前进程组，或者当前的user）。
*/
int getpriority(int which, int who);
int setpriority(int which, int who, int prio);
```
返回值说明：
- setpriority的返回值和其他函数类似，0表示成功，-1表示操作失败，
- 不过getpriority就稍微有一点绕了。作为linux程序员，我们都知道的nice value是[-20, 19]，如果getpriority返回这个范围，那么这里的-1优先级就有点尴尬了，因为一般的linux c库接口函数返回-1表示调用错误，我们是如何区分-1调用错误的返回还是优先级-1的返回值呢？getpriority是少数返回-1也是有可能正确的接口函数：在调用getpriority之前，我们需要首先将errno清零，调用getpriority之后，如果返回-1，我们需要看看errno是否还是保持0值，如果是，那么说明返回的是优先级-1，否则说明发生了错误。

### 修改调度策略 （scheduling policy）

调度器在运作的时候往往设定一组规则来决定何时，选择哪一个进程进入执行状态，执行多长的时间。那些“规则”就是调度策略。

```c++
#include <sched.h>
/**
 * sched_setscheduler函数是用来设定指定进程的scheduling policy，对于实时进程，该接口函数还
 * 可以设定rt priority。如果设定进程的调度策略是非实时的调度策略的时候（例如SCHED_NORMAL），那
 * 么param参数是没有意义的，其sched_priority成员必须设定为0
*/
int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param);
/**
 * sched_getscheduler函数可以获取指定进程的scheduling policy（如果pid等于0，那么是获取调用进程的调度策略）
*/
int sched_getscheduler(pid_t pid);
/**
 * sched_get_priority_max和sched_get_priority_min分别返回了指定调度策略的最大和最小的rt priority，
 * 不同的操作系统实现不同的优先级数量。在linux中，实时进程（SCHED_FIFO和SCHED_RR）的rt priority共计
 * 99个level，最小是1，最大是99。对于其他的调度策略，这些函数返回0。 * 
 * 
*/
int sched_get_priority_max(int policy);－－返回指定policy的最大的rt priority
int sched_get_priority_min(int policy);－－返回指定policy的最小的rt priority

int sched_setparam(pid_t pid, const struct sched_param *param);
int sched_getparam(pid_t pid, struct sched_param *param);
```

### 全能接口
 

五、一统江湖的接口

看起来前面小节描述的API已经够用了，然而，故事并未结束。经过前面关于调度接口的讨论，基本上我们对调度器的行为也已经有了了解：用户空间的进程可以通过各种前面描述的接口API来修改调度策略、nice value以及rt priority。一切看上去已经完美，CFS类型的调度器处理普通的运算密集形（例如编译内核）和用户交互形的应用（例如vi编辑文件）。如果有应用有实时需求，可以考虑让rt类型的调度器来运筹帷幄。但是，如何混合了一些realtime的应用以及有一些timing要求的应用的时候，SCHED_FIFO和SCHED_RR并不能解决问题，因为在这种调度策略下，高优先级的任务会永远的delay低优先级的任务，如果低优先级的任务有一些timing的需求，这时候，你根本控制不了调度延迟时间。

为了解决上一节中描述的问题，一类新的进程被定义出来，这类进程的优先级比实时进程和普通进程的优先级都要高，这类进行有自己的特点，参考下图：

deadline

这类进程的特点就是每隔固定的周期都会起来干活，需要一定的时间来处理事务。这类进程很牛，一上来就告诉调度器，我可是有点脾气的进程，和其他的那些妖艳的进程不一样的，我每隔一段时间（period）你就得固定分配给我一定的cpu资源（computer time），当然，分配的cpu time必须在该周期内执行完毕，因此就有deadline的概念。为了应对这种需求，3.14内核引入了一类新的进程叫做deadline进程，这类进程的调度策略是SCHED_DEADLINE。调度器对这类进程也会高看一眼，每当一个周期的开始时间到来的时候（也就是该deadline进程被唤醒的时间），调度器要优先处理这个deadline进程对cpu timer的需求，并且在某个指定的deadline时间内调度该进程执行。执行了指定的cpu time后，可以考虑调度走该进行，不过，当下一个周期到来的时候，调度器仍然要奋不顾身的在deadline时间内，再次调度该deadline进程执行。

虽然deadline进程优先级高于其他两类进程，但是用“优先级”来描述这类进程当然是不合理的，应该使用下面的三个参数来描述：

（1）周期时间（上图中的period）

（2）deadline时间（上图中的relative deadline）

（3）一次调度周期内分配多少的cpu时间（上图中的comp. time）

至此，估计您也已经发现，前面描述的接口其实都是不适合设定这些参数的，因此，GNU/linux操作系统中增加了下面的接口API：
```c++
#include <sched.h>
/**
 * attr这个参数的数据类型是struct sched_attr，这个数据结构囊括了一切你想要的关于调度的控制参数：policy，nice value，
 * rt priority，period，deadline等等。用这个接口可以完成所有前面几个小节描述API能完成的任务，
 * 唯一的不好的地方就是这个接口是linux特有的，不是posix标准，是否应用这个接口就是见仁见智了。更细节的知识这里就不描述了，大家还是参考man page好了。
*/
int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags);
int sched_getattr(pid_t pid, const struct sched_attr *attr, unsigned int size, unsigned int flags);
```

### 其他

上面描述的接口API都是和调度器参数相关，其实Linux调度器还有两类接口。一个是sched_getaffinity和sched_setaffinity，用于操作一个线程的CPU affinity。另外一个接口是sched_yield，该接口可以让出CPU资源，让Linux调度器选择一个合适的线程执行。这些接口很简单，大家仔细学习就OK了。

## ARM Linux上的系统调用

ARM工作模式中，处理器模式切换可以通过软件控制进行切换，即修改CPSR模式位，但这是在特权模式下，当我们处于用户模式下，是没有权限实现模式转换的。若想实现模式切换，只能由另一种方法来实现，即通过外部中断或是异常处理过程进行切换。于是ARM指令集中提供了两条产生异常的指令，通过这两条指令可以用软件的方法实现异常，其中一个就是中断指令SWI 


当用户空间的程序调用swi指令发起内核服务请求的时候，实际上程序其实是完成了一次“穿越”，该进程从用户态穿越到了内核态。一个swi后，用户空间的代码执行暂停了、stack（用户栈）上的数据，正文段、静态数据区、heap去的数据……一切都停下来了，程序的执行突然就转入另外一个世界，使用的栈变成了内核栈、正在执行的正文段程序变成vector_swi开始的binary code、与之匹配数据区也变化了…

[ARM Linux上的系统调用代码分析](http://www.wowotech.net/process_management/syscall-arm.html)
