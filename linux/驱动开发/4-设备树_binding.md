

# 3.bindings分类介绍

## 3.1.clock binding

“Documentation\devicetree\bindings\clock\clock-bindings.txt”

对一个clock器件，既可以是consumer身份 或是provider身份；也可能是consumer，同时是provider。因为它需要接受clock输入，经过处理后，输出clock。 但对根节点除外，根节点只能是provider身份。



### Clock providers

Required properties:
- `#clock-cells` ：Number of cells in a clock specifier; Typically 0 for nodes with a single clock output and 1 for nodes with multiple clock outputs. 典型使用方式是“0表示该clock仅有一个输出，1表示该clock有多个输出。”，但有些芯片不是采用典型用法，例如stm32就使用“`#clock-cells=2`表示多个输出，其中specifier要使用2个u32表达”

Optional properties， 这几个可选属性被"`of_clk_get_parent_name api`"使用:

- clock-output-names: 时钟输出信号名称列表，特别注意，在“clock consumer”不能直接使用它，仅仅会使用列表索引。
    Recommended to be a list of strings of clock output signal
	names indexed by the first cell in the clock specifier.
	However, the meaning of clock-output-names is domain
	specific to the clock provider, and is only provided to
	encourage using the same meaning for the majority of clock
	providers.  This format may not work for clock providers
	using a complex clock specifier format.  In those cases it
	is recommended to omit this property and create a binding
	specific names property.

	Clock consumer nodes must never directly reference
	the provider's clock-output-names property.

    For example:

        oscillator {
            #clock-cells = <1>;
            clock-output-names = "ckil", "ckih";
        };

    - this node defines a device with two clock outputs, the first named
    "ckil" and the second named "ckih".  Consumer nodes always reference
    clocks by index. The names should reflect the clock output signal
    names for the device.

- clock-indices: If the identifying number for the clocks in the node
		   is not linear from zero, then this allows the mapping of
		   identifiers into the clock-output-names array.

           配合“`clock-output-names`”属性使用

    For example, if we have two clocks <&oscillator 1> and <&oscillator 3>:

        oscillator {
            compatible = "myclocktype";
            #clock-cells = <1>;
            clock-indices = <1>, <3>;
            clock-output-names = "clka", "clkb";
        }

        This ensures we do not have any empty strings in clock-output-names


### Clock consumers

Required properties:

- `clocks` 属性: 它用来描述某“clock consumer”设备的input clock是哪些。 格式为"`clocks = <phandle1 specifier1 [phandle2 specifier2] [phandle3 specifier3] ... >`"，其中specifier是多少个u32构成由“`#clock-cells`”确定。对specifier的解释由各个芯片的clock binding文档确定，例如对stm32是由“\Documentation\ devicetree\bindings\ clock\st,stm32-rcc.txt”确定的.


例子：
```c++
// 确定时钟个数
int nr_pclks = of_count_phandle_with_args(dev->of_node,"clocks","#clock-cells");
// 获取时钟
for(int i=0;i<nr_pclks;i++){
    struct clk *clk = of_clk_get(dev->of_node,i);
}
//使能时钟
clk_prepare_enable(clk);
//禁止时钟clk_disable_unprepare(clk);
```


Optional properties:

- `clock-names`:	List of clock input name strings sorted in the same
		order as the clocks property.  Consumers drivers
		will use clock-names to match clock input names
		with clocks specifiers.
- `clock-ranges`:	Empty property indicating that child nodes can inherit named
		clocks from this node. Useful for bus nodes to provide a
		clock to their children.

"`clock-names`"与"`clock-ranges`"这两个属性被"`struct clk *clk_get(struct device *dev, const char *con_id)`"使用，使用规则就是：
- 查找起点定为`np=dev->of_node`，
- step1：如果np为空，则退出，否则继续step2
- step2：在`np`中查过该节点的“`clock-names`”，找到则退出。找不到继续step3
- step3：赋值"`np=dev->of_node->parent`",如果np不为空，并且具有“`clock-ranges`”属性，回到step2。

DTS例子：
```c++
    /*
    *This represents a device with two clock inputs, named "baud" and "register".
    *The baud clock is connected to output 1 of the &osc device, and the register
    *clock is connected to output 0 of the &ref.
    *device需要使用两个clock，“baud”和“regitser”，由clock-names关键字指定；
    *baud取自“osc”的输出1，register取自“ref”的输出0，由clocks关键字指定。
    */
    device {
        clocks = <&osc 1>, <&ref 0>;
        clock-names = "baud", "register";
    };
```

针对上面的DTS例子，在驱动中要获取clk就可以采用下面的方式：
```c++
/* driver */
int xxx_probe(struct platform_device *pdev)
{
        struct clk *baud_clk = devm_clk_get(&pdev->dev, “baud”);
        ...
        int ret = clk_prepare_enable(baud_clk);
        ...
}
```



==Example==

    /* external oscillator */
    osc: oscillator {
        compatible = "fixed-clock";
        #clock-cells = <1>;
        clock-frequency  = <32678>;
        clock-output-names = "osc";
    };

    /* phase-locked-loop device, generates a higher frequency clock
     * from the external oscillator reference */
    pll: pll@4c000 {
        compatible = "vendor,some-pll-interface"
        #clock-cells = <1>;
        clocks = <&osc 0>;
        clock-names = "ref";
        reg = <0x4c000 0x1000>;
        clock-output-names = "pll", "pll-switched";
    };

    /* UART, using the low frequency oscillator for the baud clock,
     * and the high frequency switched PLL output for register
     * clocking */
    uart@a000 {
        compatible = "fsl,imx-uart";
        reg = <0xa000 0x1000>;
        interrupts = <33>;
        clocks = <&osc 0>, <&pll 1>;
        clock-names = "baud", "register";
    };

This DT fragment defines three devices: an external oscillator to provide a
low-frequency reference clock, a PLL device to generate a higher frequency
clock signal, and a UART.

* The oscillator is fixed-frequency, and provides one clock output, named "osc".
* The PLL is both a clock provider and a clock consumer. It uses the clock
  signal generated by the external oscillator, and provides two output signals
  ("pll" and "pll-switched").
* The UART has its baud clock connected the external oscillator and its
  register clock connected to the PLL clock (the "pll-switched" signal)

### Assigned clock parents and rates

Some platforms may require initial configuration of default parent clocks
and clock frequencies. Such a configuration can be specified in a device tree
node through assigned-clocks, assigned-clock-parents and assigned-clock-rates
properties. The assigned-clock-parents property should contain a list of parent
clocks in the form of a phandle and clock specifier pair and the
assigned-clock-rates property should contain a list of frequencies in Hz. Both
these properties should correspond to the clocks listed in the assigned-clocks
property.

To skip setting parent or rate of a clock its corresponding entry should be
set to 0, or can be omitted if it is not followed by any non-zero entry.

    uart@a000 {
        compatible = "fsl,imx-uart";
        reg = <0xa000 0x1000>;
        ...
        clocks = <&osc 0>, <&pll 1>;
        clock-names = "baud", "register";

        assigned-clocks = <&clkcon 0>, <&pll 2>;
        assigned-clock-parents = <&pll 2>;
        assigned-clock-rates = <0>, <460800>;
    };

In this example the <&pll 2> clock is set as parent of clock <&clkcon 0> and
the <&pll 2> clock is assigned a frequency value of 460800 Hz.

Configuring a clock's parent and rate through the device node that consumes
the clock can be done only for clocks that have a single user. Specifying
conflicting parent or rate configuration in multiple consumer nodes for
a shared clock is forbidden.

Configuration of common clocks, which affect multiple consumer devices can
be similarly specified in the clock provider node.

## 3.2 interrupt binding

Kernel/Documentation/devicetree/bindings/interrupt-controller/interrupts.txt

