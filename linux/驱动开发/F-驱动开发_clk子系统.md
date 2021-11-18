

# 4.PM: 时钟(common clock framework)

D:\stm32\abc\rpi-kernels\linux\drivers\clk\clk-stm32f4.c

参考：
- [《Linux common clock framework(1)_概述》](http://www.wowotech.net/pm_subsystem/clk_overview.html)、
- [《Linux common clock framework(2)_clock provider》](http://www.wowotech.net/pm_subsystem/clock_provider.html)、
- [《Linux common clock framework(3)_实现逻辑分析》](http://www.wowotech.net/pm_subsystem/clock_framework_core.html)以及
- [《Common Clock Framework系统结构》](http://www.wowotech.net/pm_subsystem/ccf-arch.html)。

- [Linux下时钟框架实践---一款芯片的时钟树配置](https://www.cnblogs.com/arnoldlu/p/10307827.html)

## clock DTS相关

### 定义clock provider

Required properties:  `#clock-cells` 


方式1： 将系统所有的clock，抽象为一个虚拟的设备，用一个DTS node表示。

方式2：每一个可输出clock的器件，如Oscillator、PLL、Mux等等，都是一个设备，用一个DTS node表示。

```c++
/ {

	compatible = "st,stm32f429i-disco","st,stm32f429";
    ...
	systick: timer@e000e010 {
		compatible = "arm,armv7m-systick";
		reg = <0xe000e010 0x10>;
		status = "okay";
    	clocks = <&rcc 1 SYSTICK>;
	};

	soc {
		#address-cells = <0x01>;
		#size-cells = <0x01>;
		compatible = "simple-bus";
		interrupt-parent = <&nvic>;
		ranges;

		rtc: rtc@40002800 {
			compatible = "st,stm32-rtc";
			reg = <0x40002800 0x400>;
			/*有clocks属性，说明它是一个consumer。该属性描述作为“clock consumer”设备，它的input clock是哪些*/
			/*它引用clock provider的clock-cells是2，所以specifier有2个u32组成*/
			clocks = <&rcc 1 CLK_RTC>;
			/*clock-names 属性用于类似clk_get api 去获取该rtc设备input clock的句柄*/
			clock-names = "ck_rtc";
			/*设置"<&rcc 1 CLK_RTC>"clock的parent为"<&rcc 1 CLK_LSE>"*/
			assigned-clocks = <&rcc 1 CLK_RTC>;
			assigned-clock-parents = <&rcc 1 CLK_LSE>;
            ...
		};


		rcc: rcc@40023810 {
			#reset-cells = <1>;
			/*#clock-cells不为0，代表它是一个provider，有输出*/
			#clock-cells = <2>;
			compatible = "st,stm32f42xx-rcc", "st,stm32-rcc";
			reg = <0x40023800 0x400>;
			clocks = <&clk_hse>, <&clk_i2s_ckin>;
			st,syscfg = <&pwrcfg>;
			assigned-clocks = <&rcc 1 CLK_HSE_RTC>;
			assigned-clock-rates = <1000000>;
			linux,phandle = <0x01>;
			phandle = <0x01>;
		};


		pinctrl: pin-controller {
			compatible = "st,stm32f429-pinctrl";
            ...
			gpioa: gpio@40020000 {
				gpio-controller;
				clocks = <&rcc 0 STM32F4_AHB1_CLOCK(GPIOA)>;
				...
			};
            ...
		};
	};

	clocks {

    	/*"clk_hse"该clock设备的名称，clock consumer可以根据该名称引用clock；*/
		clk_hse: clk-hse {
			/*#clock-cells为0，代表是clock根，只能是provider*/
			#clock-cells = <0x00>;
			compatible = "fixed-clock";
			clock-frequency = <0x7a1200>;
		};

		clk-lse {
			#clock-cells = <0x00>;
			compatible = "fixed-clock";
			clock-frequency = <0x8000>;
		};

		clk-lsi {
			#clock-cells = <0x00>;
			compatible = "fixed-clock";
			clock-frequency = <0x7d00>;
		};

		clk_i2s_ckin: i2s-ckin {
			#clock-cells = <0x00>;
			compatible = "fixed-clock";
			clock-frequency = <0x00>;
		};
	};
```


### 定义clock consummer

Required properties:  `clocks`

例子见上一节

### clock分类

根据clock的特点，clock framework将clock分为fixed rate、gate、devider、mux、fixed factor、composite六类

- fixed rate：
    对于fixed rate clocks，.compatible固定填充"fixed-clock"，并提供"clock-frequency"和"clock-output-names"关键字。之后不需要再driver中做任何处理，clock framework core会帮我们搞定一切。

- fixed factor：
    对于fixed factor clock，.compatible为"fixed-factor-clock"，并提供"clock-div"、"clock-mult"和"clock-output-names"关键字。clock framework core会帮我们搞定一切。

对于不能由clock framework core处理的clock，需要在driver中使用struct of_device_id进行匹配，并在初始化时，调用OF模块，查找所有的DTS匹配项，并执行合适的regitser接口，注册clock。

- gate clock：
	这一类clock只可开关（会提供.enable/.disable回调），可使用下面接口注册：
	```c++
     struct clk *clk_register_gate(struct device *dev, const char *name,
                     const char *parent_name, unsigned long flags,
                     void __iomem *reg, u8 bit_idx,
                     u8 clk_gate_flags, spinlock_t *lock);
	  struct clk_hw *clk_hw_register_gate(...)		  
	```

- divider clock:
	这一类clock可以设置分频值（因而会提供.recalc_rate/.set_rate/.round_rate回调），可通过下面两个接口注册：
	```c++
     struct clk *clk_register_divider(struct device *dev, const char *name,
                     const char *parent_name, unsigned long flags,
                     void __iomem *reg, u8 shift, u8 width,
                     u8 clk_divider_flags, spinlock_t *lock);
	  struct clk_hw *clk_hw_register_divider(...)		  

	 struct clk *clk_register_divider_table(struct device *dev, const char *name,
	        const char *parent_name, unsigned long flags,
	        void __iomem *reg, u8 shift, u8 width,
	        u8 clk_divider_flags, const struct clk_div_table *table,
	        spinlock_t *lock);
	  struct clk_hw *clk_hw_register_divider_table(...)		  
	```

- mux clock:
	这一类clock可以选择多个parent，因为会实现.get_parent/.set_parent/.recalc_rate回调，可通过下面两个接口注册：
	```c++
      struct clk *clk_register_mux(struct device *dev, const char *name,
                      const char **parent_names, u8 num_parents, unsigned long flags,
                      void __iomem *reg, u8 shift, u8 width,
                      u8 clk_mux_flags, spinlock_t *lock);
	  struct clk_hw *clk_hw_register_mux(...)		  
      
	  struct clk *clk_register_mux_table(struct device *dev, const char *name,
                      const char **parent_names, u8 num_parents, unsigned long flags,
                      void __iomem *reg, u8 shift, u32 mask,
                      u8 clk_mux_flags, u32 *table, spinlock_t *lock);
	  struct clk_hw *clk_hw_register_mux_table(...)		  
	```

- composite clock:
	顾名思义，就是mux、divider、gate等clock的组合，可通过下面接口注册：
	```c++
      struct clk *clk_register_composite(struct device *dev, const char *name,
                      const char **parent_names, int num_parents,
                      struct clk_hw *mux_hw, const struct clk_ops *mux_ops,
                      struct clk_hw *rate_hw, const struct clk_ops *rate_ops,
                      struct clk_hw *gate_hw, const struct clk_ops *gate_ops,
                      unsigned long flags);
	  struct clk_hw *clk_hw_register_composite(...)		  
	```

### 验证 

通过读取"`/sys/kernel/debug/clk/clk_summary`"信息，和时钟框图对照，可以验证DTS配置正确与否。

## BSP工程师视角使用clock子系统API

从clock driver的角度，分析怎么借助common clock framework管理系统的时钟资源。kernel称clock driver为clock provider。

clock provider的API位于include/linux/clk_provider.h。

clock framework使用"`struct clk`"结构抽象clock，但该结构对clock consumer是透明的（不需要知道它的内部细节）。同样，"`struct clk`"对clock provider也是透明的。framework提供了"`struct clk_hw`"结构，从clock provider的角度，描述clock.


api中出现“gate”， gate代表关闭clock

### A.1. 注册、取消注册clock
通用注册函数是下表。 针对特别类型clock注册，子系统提供了更为方便使用的api，见上一节“clock 分类”章节的描述：

	|API                       | 说明|
	|--------------------------|-----|
	|[devm_]clk_register       |注册|
	|[devm_]clk_hw_register    |注册|
	|[devm_]clk_unregister     |释放.|
	|[devm_]clk_hw_unregister  |释放.|
	|||

```c++
struct clk *clk_register(struct device *dev, struct clk_hw *hw);
struct clk *devm_clk_register(struct device *dev, struct clk_hw *hw);
``` 

```c++
void clk_unregister(struct clk *clk);
void devm_clk_unregister(struct device *dev, struct clk *clk);
```

### A.2.将注册的时钟加入到OF框架中

注册clock的同时，将返回的struct clk指针，保存在一个数组中，并调用of_clk_add_provider接口，告知clock framework core

  int of_clk_add_provider(struct device_node *np,
                          struct clk *(*clk_src_get)(struct of_phandle_args *args,
                                                     void *data),
                          void *data);


## 驱动工程师视角使用clock子系统API

面向的读者是使用clock的driver开发者，即在开发一个driver时，该driver是作为一个clock consumer的身份。

### A.1.clock获取有关的API


```c++
```

例子，在驱动中要获取clk就可以采用下面的方式：
```c++
    /*DTS例子：
    *This represents a device with two clock inputs, named "baud" and "register".
    *The baud clock is connected to output 1 of the &osc device, and the register
    *clock is connected to output 0 of the &ref.
    *device需要使用两个clock，“baud”和“regitser”，由clock-names关键字指定；
    *baud取自“osc”的输出1，register取自“ref”的输出0，由clocks关键字指定。
    *device {
    *    clocks = <&osc 1>, <&ref 0>;
    *    clock-names = "baud", "register";
    *};
    */
/* driver */
int xxx_probe(struct platform_device *pdev)
{
        struct clk *baud_clk = devm_clk_get(&pdev->dev, “baud”);
        ...
        int ret = clk_prepare_enable(baud_clk);
        ...
}
```

device driver在操作设备的clock之前，需要先获取和该clock关联的struct clk指针，获取的接口如下：

	|API                | 说明|
	|-------------------|-----|
	|[devm_]clk_get     |dev和id的任意一个可以为空。如果id为空，则必须有DTS的支持才能获得device对应的clk|
	|[devm_]clk_put     |释放.|
	|clk_get_sys        |类似clk_get，不过使用device的name替代device结构|
	|of_clk_get         |直接从相应的DTS node中，以index、name等为索引，获取clk|
	|of_clk_get_by_name ||
	|of_clk_get_from_provider||
	|||

```c++
   1: struct clk *clk_get(struct device *dev, const char *id);  
   2: struct clk *devm_clk_get(struct device *dev, const char *id);
   3: void clk_put(struct clk *clk);
   4: void devm_clk_put(struct device *dev, struct clk *clk);
   5: struct clk *clk_get_sys(const char *dev_id, const char *con_id);
   6: struct clk *of_clk_get(struct device_node *np, int index);
   7: struct clk *of_clk_get_by_name(struct device_node *np, const char *name);
   8: struct clk *of_clk_get_from_provider(struct of_phandle_args *clkspec);
```

### A.2.clock控制有关的API

	|API                          | 说明|
	|-----------------------------|-----|
	|clk_prepare/clk_unprepare    |启动clock前的准备工作/停止clock后的善后工作。可能会睡眠。|
	|clk_enable/clk_disable       |启动/停止clock。不会睡眠|
	|clk_[hw_]get_rate            |clock频率的获取,|
	|clk_[hw_]round_rate          |得到和需要设置的rate比较接近的那个值|
	|clk_set_rate                 |clock频率设置,调用则保障对应频率的分频比存在|
	|clk_get_parent/clk_set_parent|获取/选择clock的parent clock|
	|clk_prepare_enable           |clk_prepare和clk_enable组合|
	|clk_disable_unprepare        |clk_disable和clk_unprepare组合|
	|clk_[hw_]get_flags||
	|||

注2：prepare/unprepare，enable/disable的说明。

	这两套API的本质，是把clock的启动/停止分为atomic和non-atomic两个阶段，以方便实现和调用。因此上面所说的“不会睡眠/可能会睡眠”，有两个角度的含义：一是告诉底层的clock driver，请把可能引起睡眠的操作，放到prepare/unprepare中实现，一定不能放到enable/disable中；二是提醒上层使用clock的driver，调用prepare/unprepare接口时可能会睡眠哦，千万不能在atomic上下文（例如中断处理中）调用哦，而调用enable/disable接口则可放心。

	另外，clock的开关为什么需要睡眠呢？这里举个例子，例如enable PLL clk，在启动PLL后，需要等待它稳定。而PLL的稳定时间是很长的，这段时间要把CPU交出（进程睡眠），不然就会浪费CPU。

	最后，为什么会有合在一起的clk_prepare_enable/clk_disable_unprepare接口呢？如果调用者能确保是在non-atomic上下文中调用，就可以顺序调用prepare/enable、disable/unprepared，为了简单，framework就帮忙封装了这两个接口。

```c++
   1: int clk_prepare(struct clk *clk)
   2: void clk_unprepare(struct clk *clk)
   3:  
   4: static inline int clk_enable(struct clk *clk)
   5: static inline void clk_disable(struct clk *clk)
   6:  
   7: static inline unsigned long clk_get_rate(struct clk *clk)
   8: static inline int clk_set_rate(struct clk *clk, unsigned long rate)
   9: static inline long clk_round_rate(struct clk *clk, unsigned long rate)
  10:  
  11: static inline int clk_set_parent(struct clk *clk, struct clk *parent)
  12: static inline struct clk *clk_get_parent(struct clk *clk)
  13:  
  14: static inline int clk_prepare_enable(struct clk *clk)
  15: static inline void clk_disable_unprepare(struct clk *clk)
```  

### A.4.  clock rate改变的通知

	|API                          | 说明|
	|-----------------------------|-----|
	|clk_notifier_register        |注册 clock rate改变的通知|
	|clk_notifier_unregister      |注销 clock rate改变的通知|
	|||
```c++
1: int clk_notifier_register(struct clk *clk, struct notifier_block *nb);
2: int clk_notifier_unregister(struct clk *clk, struct notifier_block *nb);
```
这两个notify接口，用于注册/注销 clock rate改变的通知。例如某个driver关心某个clock，期望这个clock的rate改变时，通知到自己，就可以注册一个notify。


# 5.reset framework

Documentation/devicetree/bindings/reset/reset.txt

以最简单的device_reset为例说明kernel reset过程
```c++
// 对设备dev(是一个reset consumer)进行reset
int device_reset(struct device *dev)
{
	struct reset_control *rstc;
	int ret;
    /*通过dev设备的"resets属性找到reset controller(provider)"*/
	rstc = reset_control_get(dev, NULL);
	if (IS_ERR(rstc))
		return PTR_ERR(rstc);
    /*调用"rstc->rcdev->ops->reset"动作进行reset，该动作是provider driver提供*/
	ret = reset_control_reset(rstc);

	reset_control_put(rstc);

	return ret;
}
EXPORT_SYMBOL_GPL(device_reset);
```



## rewet DTS相关

### 定义Reset providers

Required properties:
- #reset-cells:	Number of cells in a reset specifier; Typically 0 for nodes with a single reset output and 1 for nodes with multiple reset outputs.
```
For example:

	rst: reset-controller {
		#reset-cells = <1>;
	};
```
### Reset consumers

Required properties:
- resets:		List of phandle and reset specifier pairs, one pair
		for each reset signal that affects the device, or that the
		device manages. Note: if the reset provider specifies '0' for
		#reset-cells, then only the phandle portion of the pair will
		appear.

Optional properties:
- reset-names:	List of reset signal name strings sorted in the same order as
		the resets property. Consumers drivers will use reset-names to
		match reset signal names with reset specifiers.

For example:
```
	device {
		resets = <&rst 20>;
		reset-names = "reset";
	};
This represents a device with a single reset signal named "reset".

	bus {
		resets = <&rst 10> <&rst 11> <&rst 12> <&rst 11>;
		reset-names = "i2s1", "i2s2", "dma", "mixer";
	};

This represents a bus that controls the reset signal of each of four sub-
ordinate devices. Consider for example a bus that fails to operate unless no
child device has reset asserted.
```

## BSP工程师视角（reset provider）

kernel为reset provider提供的API位于“include/linux/reset-controller.h”中，很简单，无非就是：创建并填充reset controller设备（struct reset_controller_dev），并调用相应的接口（reset_controller_register/reset_controller_unregister）注册或者注销之。

```c++
struct reset_controller_dev {                                                  
        struct reset_control_ops *ops;//ops提供reset操作的实现                                         
        struct module *owner;                                                  
        struct list_head list;                                                 
        struct device_node *of_node;      
		//of_xlate和of_reset_n_cells用于解析consumer device dts node中
		//的“resets = ; ”节点，如果reset controller比较简单（仅仅是线性的索引），
		//可以不实现，使用reset framework提供的简单版本----of_reset_simple_xlate即可。                                     
        int of_reset_n_cells;                                                  
        int (*of_xlate)(struct reset_controller_dev *rcdev,                    
                        const struct of_phandle_args *reset_spec);             
        unsigned int nr_resets;//该reset controller所控制的reset信号的个数。
};
```


## 驱动工程师视角(reset consumer)

从某一个硬件模块的驱动设计者来看，他的要求很简单：我只是想复位我的硬件，而不想知道到底用什么手段才能复位（例如控制哪个寄存器的哪个bit位，等等）。

这个要求其实体现了软件设计（甚至是任何设计）中的一个最最质朴的设计理念：封装和抽象。对设备驱动来说，它期望看到是“reset”这个通用概念，用这个通用概念去发号施令的话，这个驱动就具备了通用性和可移植性（无论在周围的环境如何变化，“reset”本身不会变化）。而至于怎么reset，是通过寄存器A的bit m，还是寄存器B的bit n，则是平台维护者需要关心的事情（就是本文的reset provider）。

看到这样的要求，Linux kernel说：OK，于是reset framework出场，提供了如下的机制（基于device tree）：

1）首先，提供描述系统中reset资源的方法（参考下面第3章的介绍），这样consumer可以基于这种描述在自己的dts node中引用所需的reset信号。

2）然后，consumer设备在自己的dts node中使用“resets”、“reset-names”等关键字声明所需的reset的资源，例如[1]（“resets”字段的具体格式由reset provider决定”）：
```
device {                                                               
        resets = <&rst 20>;                                            
        reset-names = "reset";                                         
};
```
3）最后，consumer driver在需要的时候，可以调用下面的API复位自己（具体可参考“include/linux/reset.h“）：

3-a）只有一个reset信号的话，可以使用最简单的device_reset API "`int device_reset(struct device *dev);`"

3-b）如果需要更为复杂的控制（例如有多个reset信号、需要控制处于reset状态的长度的等），可以使用稍微复杂的API
```c++
/* 通过reset_control_get或者devm_reset_control_get获得reset句柄 */
struct reset_control *reset_control_get(struct device *dev, const char *id);   
void reset_control_put(struct reset_control *rstc);                            
struct reset_control *devm_reset_control_get(struct device *dev, const char *id);

/* 通过reset_control_reset进行复位，或者通过reset_control_assert使设备处于复位生效状态，通过reset_control_deassert使复位失效 */
int reset_control_reset(struct reset_control *rstc);                           
int reset_control_assert(struct reset_control *rstc);                          
int reset_control_deassert(struct reset_control *rstc);
```

