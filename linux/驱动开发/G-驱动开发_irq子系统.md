
# 9. IRQ子系统

查看系统当前的中断统计信息：`cat /proc/interrupts`, 他能统计每个终端号在每个cpu上的发生次数

[LINUX 中断子系统专栏之一 子系统架构简述](https://blog.csdn.net/lickylin/article/details/108031868)

针对一个irq controller而言，一个irq controller可以包含多个hw irq line，而一个hw irq line上可以由多个外设共用（共享中断），因此针对一个hw irq line，需支持提供多个action，实现针对不同外设的中断响应。

 “irq-controller”级联扩展概念示意：
```
soc--> “irq-controller” ------> “irq-controller”
             |                           |
             |                           |------> “irq-controller”
             |-------> “irq-controller”
             |-------> “irq-controller”
        	 |-------> ...
```

```c++
/dts-v1/;

/ {
	#address-cells = <0x01>;
	#size-cells = <0x01>;
	compatible = "st,stm32f429i-disco\0st,stm32f429";
	...
	nvic: interrupt-controller@e000e100 {
		compatible = "arm,armv7m-nvic";
		interrupt-controller;
		#interrupt-cells = <0x01>;
		...
	};

	soc {
		compatible = "simple-bus";
		interrupt-parent = <&nvic>;
		ranges;

		timer@40000000 {
			compatible = "st,stm32-timer";
			interrupts = <0x1c>;
			...;
		};

		rtc: rtc@40002800 {
			compatible = "st,stm32-rtc";
			interrupt-parent = <&exti>;
			interrupts = <17 1>;
			interrupt-names = "alarm";
			...
		};

		usart1: serial@40011000 {
			compatible = "st,stm32-usart\0st,stm32-uart";
			interrupts = <0x25>;
			...
		};

		adc: adc@40012000 {
			compatible = "st,stm32f4-adc-core";
			interrupts = <0x12>;
			interrupt-controller;
			#interrupt-cells = <0x01>;

			adc1: adc@0 {
				compatible = "st,stm32f4-adc";
				interrupt-parent = <&adc>;
				interrupts = <0>;
			};
		};

		exti: interrupt-controller@40013c00 {
			compatible = "st,stm32-exti";
			interrupt-controller;
			#interrupt-cells = <0x02>;
			interrupts = <1>, <2>, <3>, <6>, <7>, <8>, <9>, <10>, <23>, <40>, <41>, <42>, <62>, <76>;
		};
	};

};
```
根据上面DTS所表达的级联关系，我们可以知道“级联多个irq controller”如下所示：
```
        nvic-controoler  === usart1-device 
            |      |
            |      |----->exti-controller  === rtc-device
            |
		    |------> adc-controller === adc1-device
```

针对系统中所有存在的hw irq line，均为其分配logic irq index，且logic irq index是系统唯一的，因此需要提供一种map机制，实现logic irq index与hw irq line的映射。另外针对一个irq controller的所有hw irq line而言，针对该hw irq line的enable、disable。

涉及数据结构：
其中：

- `struct irq_chip` ：用于操作irq controller，实现hw irq line的使能、去使能、中断触发类型等操作；
- `struct irq_desc` ：主要用于hw irq line触发后的处理操作接口的注册等，而且该struct irq_desc是IRQ子系统的最主要数据结构，其连接了irq_chip、irq_domain数据结构；
- `struct irq_domain` ：主要用于实现logic irq index与hw irq index的映射关系，以便根据logic irq index，即可找到该irq_domain下对应的hw irq index。

## irq DTS相关

中断信号与地址不一样，中断信号可以源自和终止于机器中的任何设备。因此中断信号采用与设备树无关，而是节点之间连接（as links between nodes independent of the tree. ）来表示，我们称之为中断连接（ interrupt connections）。

中断连接（ interrupt connections）具有以下4个属性：
- `interrupt-controller` - 是一个空属性， 表示将节点声明为接收中断信号的设备
- `#interrupt-cells`- 这是中断控制器节点的属性。它说明此中断控制器的中断描述符（interrupt specifier）中有多少个单元格（类似于#address-cells和#size-cells）。
- `interrupt-parent`- 设备节点的属性，其中包含到它所连接的中断控制器的phandle。没有该属性(中断父属性)的节点也可以从其父节点继承该属性。
- `interrupts-extended` - 当一个节点需要引用多个中断控制器中的中断时，可以使用 interrupts-extended 属性。 interrupts-extended 的每个入口包含了中断控制器的 phandle 和中断标识。 interrupts-extended 只有节点需要多个中断控制器的中断时才使用，其他均不适用。例如"`interrupts-extended = <&intc1 5 1>, <&intc2 1 0>;`"
- `interrupts`- 包含中断描述符列表的设备节点的属性，列表中每个元素代表一个该设备中断输出的中断描述符。 例如`interrupts = <0 48 4>,  <0 49 4>;` ，显然该例子中，对应中断控制器的`#interrupt-cells = <3>`。 这里要注意中断描述符完全是设备自己定义的，不存在适用所有设备的中断描述符，例如对stm32，中断描述符就是IRQn_Type的合法值。
	例如，有下面的设备节点：
	```
		exti: interrupt-controller@40013c00 {
			compatible = "st,stm32-exti";
			interrupt-controller;
			#interrupt-cells = <0x02>;
			reg = <0x40013c00 0x400>;
			interrupts = <1>, <2>, <3>, <6>, <7>, <8>, <9>, <10>, <23>, <40>, <41>, <42>, <62>, <76>;
			linux,phandle = <0x03>;
			phandle = <0x03>;
		};
    ```
	对应的IRQn_Type值是：
	```c++
	typedef enum
	{
    ...
	PVD_IRQn                    = 1,      /*!< PVD through EXTI Line detection Interrupt           */
	TAMP_STAMP_IRQn             = 2,      /*!< Tamper and TimeStamp interrupts through the EXTI lin*/
	RTC_WKUP_IRQn               = 3,      /*!< RTC Wakeup interrupt through the EXTI line          */
	FLASH_IRQn                  = 4,      /*!< FLASH global Interrupt                              */
	//RCC_IRQn                    = 5,      /*!< RCC global Interrupt                                */
	//EXTI0_IRQn                  = 6,      /*!< EXTI Line0 Interrupt                                */
	EXTI1_IRQn                  = 7,      /*!< EXTI Line1 Interrupt                                */
	EXTI2_IRQn                  = 8,      /*!< EXTI Line2 Interrupt                                */
	EXTI3_IRQn                  = 9,      /*!< EXTI Line3 Interrupt                                */
	EXTI4_IRQn                  = 10,     /*!< EXTI Line4 Interrupt                                */
	EXTI9_5_IRQn                = 23,     /*!< External Line[9:5] Interrupts                       */
	EXTI15_10_IRQn              = 40,     /*!< External Line[15:10] Interrupts                                   */
	RTC_Alarm_IRQn              = 41,     /*!< RTC Alarm (A and B) through EXTI Line Interrupt                   */
	OTG_FS_WKUP_IRQn            = 42,     /*!< USB OTG FS Wakeup through EXTI line interrupt                     */
    ...
	ETH_WKUP_IRQn               = 62,     /*!< Ethernet Wakeup through EXTI line Interrupt                       */
	OTG_HS_WKUP_IRQn            = 76,     /*!< USB OTG HS Wakeup through EXTI interrupt                          */
	...
	} IRQn_Type;
    ```


一个中断描述符（interrupt specifier）是一个或多个`u32`（数量通过`#interrupt-cells`指定）组成，该其中断输入所述装置被附接到指定。大多数设备只有一个中断输出，如下例所示，但一个设备上可能有多个中断输出。中断描述符（interrupt specifier）的含义完全取决于中断控制器设备的绑定。每个中断控制器可以决定它需要多少个单元来唯一地定义一个中断输入。

示例见前面的级联示意。

## IRQCHIP_DECLARE说明

start_kernel-->init_IRQ-->irqchip_init-->of_irq_init(__irqchip_of_table)

宏IRQCHIP_DECLARE就是用来填充__irqchip_of_table，__irqchip_of_table用于存放中断控制器信息，用于最终来匹配设备；

在内核启动初始化中断的函数中，of_irq_init函数会去查找设备节点信息，该函数的传入参数就是__irqchip_of_table段，由于IRQCHIP_DECLARE已经将信息填充好了，of_irq_init函数会根据compatible（例如"arm,gic-400")去查找对应的设备节点，并获取设备的信息。中断控制器也存在级联的情况，of_irq_init函数中也处理了这种情况；

在GIC驱动程序中，使用IRQCHIP_DECLARE宏来声明结构信息，包括compatible字段和回调函数，该宏会将这个结构放置到__irqchip_of_table字段中；

## BSP工程师视角使用IRQ子系统API

例子： “`linux\drivers\irqchip\irq-stm32-exti.c`”

级联控制器的例子：“`linux\drivers\pinctrl\stm32\pinctrl-stm32.c`”

### A.1.创建注册IRQ Domain

"linux\Documentation\IRQ-domain.txt"

Linux使用IRQ domain来描述一个中断控制器（IRQ Controller）所管理的中断源。换句话说，每个中断控制器都有自己的domain。我们可以将IRQ Domain看作是IRQ Controller的软件抽象,

linux kernel提供irq_domain的管理框架， 将hwirq映射到虚拟中断号上。每一个中断控制器都需要注册一个irq_domain。用来操作抽象硬件IRQ domain的就是“irq_domain->gc”。

- IRQ Domain的创建：在中断源和irq_desc之间建立线性映射：
	|API                    | 说明|
	|-----------------------|-----|
	|irq_domain_add_linear  |线性映射.|
	|irq_domain_add_tree    |树映射|
	|irq_domain_add_nomap   |不映射|
	|irq_domain_create_hierarchy| 在“级联多个irq controller”中，对非根节点场景进行创建 |

   - 线性映射:线性映射保留一张固定的表,通过hwirq number来索引.当hwirq被映射后, 会相应地分配
一个irq_desc, IRQ number就被存在表中.
	当hwirqs是固定的而且小于256, 用线性映射更好.它的优势是寻找时间固定,并且irq_descs
只在in-use IRQs分配.缺点是表格和hwirq 最大numbers一样大.
   - 树映射: 此种方法使用radix tree来维护映射, 通过key来查找.此方法适合hwirq number非常大的时候, 因为它不需要分配和hwirq一样大的table. 缺点是查表效率依赖与table里的entries数量.
   - 不映射： 当有些硬件可以对hwirq number编程时,IRQ number被编进硬件寄存器里,那么就不需要映射了.这种情况下通过irq_create_direct_mapping()实现.


例子：
```c++
	struct irq_domain_ops irq_exti_domain_ops = {
		.map	= irq_map_generic_chip,
		.xlate	= irq_domain_xlate_onetwocell,
		.alloc  = stm32_exti_alloc,
		.free	= stm32_exti_free,
	};
static int __init stm32_exti_init(struct device_node *node,
				  struct device_node *parent)
{
    // 创建irq_domain
	struct irq_domain *domain = irq_domain_add_linear(node, nr_exti,
				       &irq_exti_domain_ops, NULL);
	...
    //Allocate generic chips for the irq domain
	ret = irq_alloc_domain_generic_chips(domain, nr_exti, 1, "exti",
					     handle_edge_irq, clr, 0, 0);
	...
	// fill information to generic chips
	struct irq_chip_generic *gc = domain->gc->gc[0];
	gc->reg_base                         = base;
	gc->chip_types->type               = IRQ_TYPE_EDGE_BOTH;
	gc->chip_types->chip.name          = gc->chip_types[0].chip.name;
	gc->chip_types->chip.irq_ack       = irq_gc_ack_set_bit;
	gc->chip_types->chip.irq_mask      = irq_gc_mask_clr_bit;
	gc->chip_types->chip.irq_unmask    = irq_gc_mask_set_bit;
	gc->chip_types->chip.irq_set_type  = stm32_irq_set_type;
	gc->chip_types->chip.irq_set_wake  = stm32_irq_set_wake;
	gc->chip_types->regs.ack           = EXTI_PR;
	gc->chip_types->regs.mask          = EXTI_IMR;
	gc->chip_types->handler            = handle_edge_irq;

	nr_irqs = of_irq_count(node);
	for (i = 0; i < nr_irqs; i++) {
		//Parse and map an interrupt into linux virq space
		unsigned int virq = irq_of_parse_and_map(node, i);
        //set virq handler data for an virq
		irq_set_handler_data(virq, domain);
		//Set a highlevel chained flow handler for a given IRQ.
		irq_set_chained_handler(virq, stm32_irq_handler);
	}

	return 0;

out_free_domain:
	irq_domain_remove(domain);
out_unmap:
	iounmap(base);
	return ret;	
}			
IRQCHIP_DECLARE(stm32_exti, "st,stm32-exti", stm32_exti_init);

```

### A.2.创建IRQ domain's generic chip

前面有提到操纵IRQ Domain是通过generic chip

	|API                             | 说明|
	|--------------------------------|-----|
	|irq_alloc_domain_generic_chips  |Allocate generic chips for an irq domain.|
	||||

例子见A.1节

### A.3.为抽象硬件IRQ domain's generic chip赋值操纵回调信息

例子见A.1节

### 为irq domain创建映射

上节的内容主要是向系统注册一个irq domain，具体HW interrupt ID和IRQ number的映射关系都是空的，因此，具体各个irq domain如何管理映射所需要的database还是需要建立的。例如：对于线性映射的irq domain，我们需要建立线性映射的lookup table，对于Radix Tree map，我们要把那个反应IRQ number和HW interrupt ID的Radix tree建立起来。创建映射有四个接口函数：

方式一： 
	
	需要（必须）提供HW interrupt ID，也就是意味着driver知道自己使用的HW interrupt ID，而一般情况下，HW interrupt ID其实对具体的driver应该是不可见的，不过有些场景比较特殊，例如GPIO类型的中断，它的HW interrupt ID和GPIO有着特定的关系，driver知道自己使用那个GPIO，也就是知道使用哪一个HW interrupt ID了。

   ```c++
	// 建立HW interrupt ID和IRQ number的映射关系
	// 返回IRQ number（这个IRQ number是动态分配的）
	extern unsigned int irq_create_mapping(struct irq_domain *host,
						irq_hw_number_t hwirq);
	// 为一组HW interrupt ID建立映射
	extern int irq_create_strict_mappings(struct irq_domain *domain,
						unsigned int irq_base,
						irq_hw_number_t hwirq_base, int count);
   ```

方式二：

	在使用Device tree情况下推荐使用方式

	`irq_create_of_mapping`利用device tree进行映射关系的建立。

	对于一个使用Device tree的普通驱动程序（我们推荐这样做），基本上初始化需要调用irq_of_parse_and_map获取IRQ number，然后调用request_threaded_irq申请中断handler。


	```c++
	unsigned int irq_of_parse_and_map(struct device_node *dev, int index)
	{
		struct of_phandle_args oirq;

		if (of_irq_parse_one(dev, index, &oirq))－－－－分析device node中的interrupt相关属性
			return 0;

		return irq_create_of_mapping(&oirq);－－－－－创建映射，并返回对应的IRQ number
	}
	```

    特别的，如果编写platform device，还有一个更简单的封装使用“`irq = platform_get_irq(pdev, index)`”,它直接从platform device resoure中查找“IORESOURCE_IRQ”资源

	这里注意，在dts的“interrupts”中存放的是 hwirq 列表。

例子：

	```c++
	/* probe platform driver */
	static int demo_probe(struct platform_device *pdev)
	{
		struct device_node *np = pdev->dev.of_node;
		int ret;
		
		/* Obtain interrupt ID from DTS */
		irq = of_irq_get(np, 0);

		/* Request irq handler */
		ret = request_threaded_irq(irq, NULL, demo_irq_handler,
						IRQF_ONESHOT | IRQF_TRIGGER_FALLING, "demo", NULL);
		if (ret) {
			printk("Failed to acquire irq %d\n", irq);
			return -EINVAL;
		}

		return 0;
	}

	```

### A.5.为virq设置handler_data,chained_handler

	|API                        | 说明|
	|---------------------------|-----|
	|irq_set_handler_data       |set virq handler data for an virq.|
	|irq_set_chained_handler    |Set a highlevel chained flow handler for a given IRQ.|
	||||

例子见A.1节

## 驱动工程师视角使用IRQ子系统API

GPIO按键驱动（"\linux\drivers\input\keyboard\gpio_keys.c"）是一个放之四海皆准的驱动，比较简单，因为简单所以通过它学习中断处理是一个较好的入口。


### A.1. 注册中断ISR

	|API                           | 说明|
	|------------------------------|-----|
	|[devm_]request_irq            |legacy|
	|[devm_]request_threaded_irq   |threaded handling method.|
	|[devm_]request_any_context_irq|根据上级中断是否是treaded，决定采用hardirq or threaded handling method|
	||||



主要是`request_irq`、`[devm_]request_threaded_irq`接口等，前者是legacy，后者是线程化的中断注册函数，目的是降低ISR对系统实时延迟的影响。如果想将ISR线程化，输入“handler=NULL， thread_fn， irqflags=IRQF_ONESHOT | ...” , irqflags特别的原因是因为”handler=NULL“意味着启用“irq_default_primary_handler”，而它是用于oneshot interrupt。

而对于大多数驱动工程师而言，基本上只需要使用该接口即可完成中断处理函数的注册。

对两个参数补充说明：
- @dev_id用于共享中断，用来标识是哪个设备触发了中断，通常传入相应设备的中断状态寄存器的地址. 传入中断处理程序的参数,注册共享中断时不能为NULL,因为卸载时需要这个做参数,避免卸载其它中断服务函数
- @name 是中断ISR名称，该值可以在/proc/interrupt中看到
- @irqflags取值在"interrupt.h"有定义，常用的有`IRQF_DISABLED`和`IRQF_SHARED`，前者表示中断程序调用时屏蔽所有中断，"所有"指的是屏蔽所有中断线的中断，本中断线的中断本来就是屏蔽的，内核默认不允许中断嵌套。IRQF_SHARED表示多个设备共享中断，即该中断线上连接了多个相同或不同的设备。
除了中断类型，flags还需要"位或"上触发方式，eg：`IRQF_DISABLED|IRQF_TRIGGER_RISING`

```c++
/**
 *	request_threaded_irq - allocate an interrupt line
 *	@irq: Interrupt line to allocate
 *	@handler: Function to be called when the IRQ occurs.
 *		  Primary handler for threaded interrupts
 *		  If NULL and thread_fn != NULL the default
 *		  primary handler is installed
 *	@thread_fn: Function called from the irq handler thread
 *		    If NULL, no irq thread is created
 *	@irqflags: Interrupt type flags 
 *	@devname: An ascii name for the claiming device
 *	@dev_id: A cookie passed back to the handler function
 *
 *	This call allocates interrupt resources and enables the
 *	interrupt line and IRQ handling. From the point this
 *	call is made your handler function may be invoked. Since
 *	your handler function must clear any interrupt the board
 *	raises, you must take care both to initialise your hardware
 *	and to set up the interrupt handler in the right order.
 *
 *	If you want to set up a threaded irq handler for your device
 *	then you need to supply @handler and @thread_fn. @handler is
 *	still called in hard interrupt context and has to check
 *	whether the interrupt originates from the device. If yes it
 *	needs to disable the interrupt on the device and return
 *	IRQ_WAKE_THREAD which will wake up the handler thread and run
 *	@thread_fn. This split handler design is necessary to support
 *	shared interrupts.
 *
 *	Dev_id must be globally unique. Normally the address of the
 *	device data structure is used as the cookie. Since the handler
 *	receives this value it makes sense to use it.
 *
 *	If your interrupt is shared you must pass a non NULL dev_id
 *	as this is required when freeing the interrupt.
 *
 *	Flags:
 *
 *	IRQF_SHARED		Interrupt is shared
 *	IRQF_TRIGGER_*		Specify active edge(s) or level
 *
 */
int request_threaded_irq(unsigned int irq, irq_handler_t handler,
			 irq_handler_t thread_fn, unsigned long irqflags,
			 const char *devname, void *dev_id)

static inline int __must_check
request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
	    const char *name, void *dev)
{
	return request_threaded_irq(irq, handler, NULL, flags, name, dev);
}
```

例子：
```c++
//linux\drivers\tty\serial\stm32-usart.c
	port->irq	= platform_get_irq(pdev, 0); //或使用irq_of_parse_and_map(pdev->dev->of_node, index)
	ret = request_threaded_irq(port->irq, stm32_interrupt,
				   stm32_threaded_interrupt,
				   IRQF_NO_SUSPEND, name, port);
	free_irq(port->irq, port);
```

### A.1. 释放中断ISR

free_irq， devm_free_irq

`void free_irq(unsigned int irqno,void * dev_id);`

## 其他API
除了上述API，内核还提供了其他的中断操作API，在内核代码中被广泛使用。
```c++
raw_local_irq_save(x)   //禁止所有中断
raw_local_irq_enable    //取消禁止中断

//下面三个函数用于可编程中断控制器，对所有CPU都有效
//屏蔽一个中断
void disable_irq(int irq);  //立即返回
void disable_irq_nosync();  //等待目前中断处理返程

//使能一个中断
void enable_irq()
```


# GIC硬件原理
ARM公司提供了一个通用的中断控制器GIC（Generic Interrupt Controller），GIC的版本包括V1(已废弃),V2,V3,V4。 注意gic只是一个gic的架构，其实现的对应的IP是不同的，比如GICv2架构对应实现的IP是gic400

