

# pin muxing

解决pins数量有限，但需要使用pin的hardware blocks太多的矛盾。The pins are multiplexed: they expose either the functionality of hardware block A or the functionality of hardware block B

在linux中有pinctrl subsystem处理pin muxing，pinctrl要求在DT中描述pin muxing。 

pinctrl subsystem diagram:
```
 ┌─────────────┐
 │device driver├────────┐
 └─────────────┘        │
 ┌─────────────┐ request pin muxing
 │device driver├────────┐
 └─────────────┘        │
 ┌─────────────┐        │
 │device driver├────────┤             ┌─────────────────────────────┐
 └─────────────┘        │             │ Soc-specific pinctrl driver │
                        │             │ drivers/pinctrl/pinctrl-*.c │
 ┌──────────────────────▼──────┐      │ ┌────────────────┐          │
 │   pinctrl subsystem         │      │ │   pinctrl_desc │          │
 │         core                │      │ │pinctrl_ops     │          │
 │drivers/pinctrl/{core,pinconf◄──────┼─┤pinmux_ops      │          │
 │  ,devicetree,pinmux}.c      │      │ │pinconf_ops     │          │
 └─────────────────────────────┘      │ │                │          │
                                      │ └────────────────┘   BSP    │
                                      └────────▲────────────────────┘
    ┌──────────────────┐                       │
    │soc/board DT file ├───────────────────────┘
    └──────────────────┘                               
notes:
   - pinctrl_ops: list pins and pin groups
   - pinmux_ops: control muxing of pins
   - pinconf_ops: control configuration of pins

```

注意： pinctrl的实现不许用我们在驱动里调用任何它提供的api，pinctrl api对于驱动工程师是透明的，对于驱动工程师只需要通过设备树文件就可以起到配置整个系统pin的目的。。pinctrl在代码层级只与bsp工程师有关。

“other drivers” --------> gpio subsystem --------"pin controller dirver (BSP)" --- hardware layer

# pinctrl DTS相关

"Documentation\devicetree\bindings\pinctrl\pinctrl-bindings.txt"

== Generic pin multiplexing node content ==

See pinmux-node.yaml

== Generic pin configuration node content ==

See pincfg-node.yaml



## A.1.定义Pin controller节点

- pin controller：前者提供服务：可以用它来复用引脚、配置引脚。

Required properties: See the pin controller driver specific documentation

Pin controller devices should contain the pin configuration nodes that client
devices reference.

## A.2.定义client device节点(consumer)


- client device： 使用服务：声明自己要使用哪些引脚的哪些功能，怎么配置它们。 Pinctrl系统的客户，那就是使用Pinctrl系统的设备，使用引脚的设备。它在设备树里会被定义为一个节点，在节点里声明要用哪些引脚。

Device Tree properties for consumer devices
The devices that require certains pins to be muxed will use the `pinctrl-<x>` and
`pinctrl-names` Device Tree properties.

- The `pinctrl-<x>` properties link to a pin configuration for a given state of the device.
- The `pinctrl-names` property associates a name to each state. The name default is special, and is automatically selected by a device driver, without having to make an explicit pinctrl function call.

See Documentation/devicetree/bindings/pinctrl/pinctrl-bindings.txt for details.

```c++
// Most common case (arch/arm/boot/dts/kirkwood.dtsi)
i2c0: i2c@11000 {
...
pinctrl-0 = <&pmx_twsi0>;
pinctrl-names = "default";
...
};

// Case with multiple pin states (arch/arm/boot/dts/sama5d4.dtsi)
i2c0: i2c@f8014000 {
...
pinctrl-names = "default", "gpio";
pinctrl-0 = <&pinctrl_i2c0>;
pinctrl-1 = <&pinctrl_i2c0_gpio>;
...
};
```

```c++
/{
	soc {
		compatible = "simple-bus";
        //定义pinctrl client device节点
		usart1: serial@40011000 {
			compatible = "st,stm32-usart\0st,stm32-uart";
			reg = <0x40011000 0x400>;
			...
			//refer pin configuration nodes <&usart1_pins_a>, the pin configuration nodes 
			// must be child nodes of the pin controller that they configure.
			pinctrl-0 = <&usart1_pins_a>;
			// 	The list of names to assign states(pinctrl-x)
			pinctrl-names = "default";
		};

        // 定义pinctrl节点
		pinctrl: pin-controller {
			#address-cells = <0x01>;
			#size-cells = <0x01>;
			ranges = <0x00 0x40020000 0x3000>;
			interrupt-parent = <0x03>;
			st,syscfg = <0x0c 0x08>;
			pins-are-numbered;
			compatible = "st,stm32f429-pinctrl";
			linux,phandle = <0x0f>;
			phandle = <0x0f>;
            //定义gpio controller节点
			gpioa: gpio@40020000 {
				gpio-controller;
				#gpio-cells = <0x02>;
				interrupt-controller;
				#interrupt-cells = <0x02>;
				reg = <0x00 0x400>;
				clocks = <0x01 0x00 0x00>;
				st,bank-name = "GPIOA";
				gpio-ranges = <&pinctrl 0x00 0x00 0x10>;
				linux,phandle = <0x11>;
				phandle = <0x11>;
			};
            //定义pin configuration nodes
			// supported attrs: Documentation\devicetree\bindings\pinctrl\pincfg-node.yaml
			usart1_pins_a: usart1@0 {
				linux,phandle = <0x08>;
				phandle = <0x08>;
				pins1 {
					pinmux = <STM32F429_PA9_FUNC_USART1_TX>;
					bias-disable;
					drive-push-pull;
					slew-rate = <0>;
				};
				pins2 {
					pinmux = <STM32F429_PA10_FUNC_USART1_RX>;
					bias-disable;
				};
			};
		};
	};
};
```

## A.3.定义pin configuration节点
见“\Documentation\devicetree\bindings\pinctrl\pinctrl-bindings.txt”中“Generic pin configuration node content”章节

例子见上章节

## A.4.解析pin configuration nodes

注意：kernel pinctrl subsystem并不关心configuration的具体内容是什么，它只提供pin configuration get/set的通用机制，至于get到的东西，以及set的东西，到底是什么，是pinctrl driver自己的事情。

通常是在pinctrl probe中调用"`pinconf_generic_parse_dt_config`"来解析pin-configuration节点，比如出现了属性“bias-bus-hold”，那该api会解析它为字典值“PIN_CONFIG_BIAS_BUS_HOLD”。该类节点支持的参数见“\Documentation\devicetree\bindings\pinctrl\pinctrl-bindings.txt”中“Generic pin configuration node content”章节 

- pin-configuration节点默认支持解析的参数（不包括“`pins`,`group`,`pinmux`”）：
```c++
//defined in "drivers\pinctrl\pinconf-generic.c"
static const struct pinconf_generic_params dt_params[] = {
	{ "bias-bus-hold", PIN_CONFIG_BIAS_BUS_HOLD, 0 },
	{ "bias-disable", PIN_CONFIG_BIAS_DISABLE, 0 },
	{ "bias-high-impedance", PIN_CONFIG_BIAS_HIGH_IMPEDANCE, 0 },
	{ "bias-pull-up", PIN_CONFIG_BIAS_PULL_UP, 1 },
	{ "bias-pull-pin-default", PIN_CONFIG_BIAS_PULL_PIN_DEFAULT, 1 },
	{ "bias-pull-down", PIN_CONFIG_BIAS_PULL_DOWN, 1 },
	{ "drive-open-drain", PIN_CONFIG_DRIVE_OPEN_DRAIN, 0 },
	{ "drive-open-source", PIN_CONFIG_DRIVE_OPEN_SOURCE, 0 },
	{ "drive-push-pull", PIN_CONFIG_DRIVE_PUSH_PULL, 0 },
	{ "drive-strength", PIN_CONFIG_DRIVE_STRENGTH, 0 },
	{ "input-debounce", PIN_CONFIG_INPUT_DEBOUNCE, 0 },
	{ "input-disable", PIN_CONFIG_INPUT_ENABLE, 0 },
	{ "input-enable", PIN_CONFIG_INPUT_ENABLE, 1 },
	{ "input-schmitt", PIN_CONFIG_INPUT_SCHMITT, 0 },
	{ "input-schmitt-disable", PIN_CONFIG_INPUT_SCHMITT_ENABLE, 0 },
	{ "input-schmitt-enable", PIN_CONFIG_INPUT_SCHMITT_ENABLE, 1 },
	{ "low-power-disable", PIN_CONFIG_LOW_POWER_MODE, 0 },
	{ "low-power-enable", PIN_CONFIG_LOW_POWER_MODE, 1 },
	{ "output-disable", PIN_CONFIG_OUTPUT_ENABLE, 0 },
	{ "output-enable", PIN_CONFIG_OUTPUT_ENABLE, 1 },
	{ "output-high", PIN_CONFIG_OUTPUT, 1, },
	{ "output-low", PIN_CONFIG_OUTPUT, 0, },
	{ "power-source", PIN_CONFIG_POWER_SOURCE, 0 },
	{ "sleep-hardware-state", PIN_CONFIG_SLEEP_HARDWARE_STATE, 0 },
	{ "slew-rate", PIN_CONFIG_SLEW_RATE, 0 },
};
```

- 如果pin-configuration节点默认支持解析的参数不满足，希望新增其他参数,可以通过pinctrl_desc中的"custom_params"向内核传递，下面代码片段演示了例子：
	```c++
	...
	enum lpc18xx_pin_config_param {
		PIN_CONFIG_GPIO_PIN_INT = PIN_CONFIG_END + 1,
	};
	static const struct pinconf_generic_params lpc18xx_params[] = {
		{"nxp,gpio-pin-interrupt", PIN_CONFIG_GPIO_PIN_INT, 0},
	};
	static struct pinctrl_desc lpc18xx_scu_desc = {
		...
		.num_custom_params = ARRAY_SIZE(lpc18xx_params),
		.custom_params = lpc18xx_params,
		...
	};
	...
	```

## 2.2 pinctrl子系统设备树：

```c++
device_node {
    ...
    /*分别对用pinctrl-0和pinctrl-1*/
    pinctrl-names = "gpio_active", "gpio_sleep";  
    pinctrl-0 = <&gpio_active>;         //引用
    pinctrl-1 = <&gpio_sleep>;          //引用
    ...
};
```

```c++
static int xxxxxxxxxx(struct platform_device *pdev){
　struct pinctrl *pinctrl = devm_pinctrl_get(&pdev->dev);    //获取device对应节点下的pinctrl
  struct pinctrl_state = pinctrl_lookup_state(pinctrl, "gpio_active");    //通过pinctrl名获取pinctrl对应状态
  pinctrl_select_state(pinctrl, pinctrl_state);        //设置pinctrl的状态为'gpio_active
  devm_pinctrl_put(pinctrl);          //使用完了释放资源

  //pinctrl_get_select 相当于pinctrl_get 与 pinctrl_select_state的结合。
}
```

```c++
//drivers/pinctrl/core.c
/**
 * pinctrl_select_state() - select/activate/program a pinctrl state to HW
 * @p: the pinctrl handle for the device that requests configuration
 * @state: the state handle to select/activate/program
 */
int pinctrl_select_state(struct pinctrl *p, struct pinctrl_state *state)
{
	...
	/* Apply all the settings for the new state */
	list_for_each_entry(setting, &state->settings, node) {
		switch (setting->type) {
		case PIN_MAP_TYPE_MUX_GROUP:
			ret = pinmux_enable_setting(setting);
			break;
		case PIN_MAP_TYPE_CONFIGS_PIN:
		case PIN_MAP_TYPE_CONFIGS_GROUP:
			ret = pinconf_apply_setting(setting);
			break;
		default:
			ret = -EINVAL;
			...;
		}
	}
    ...
}
```

```c++
//drivers/pinctrl/pinmux.c
int pinmux_enable_setting(const struct pinctrl_setting *setting)
{
	struct pinctrl_dev *pctldev = setting->pctldev;
	...
	if (pctlops->get_group_pins)
		ret = pctlops->get_group_pins(pctldev, setting->data.mux.group,
					      &pins, &num_pins);
	...
	/* Try to allocate all pins in this group, one by one */
	for (i = 0; i < num_pins; i++) {
		ret = pin_request(pctldev, pins[i], setting->dev_name, NULL);
		...
	}

	/* Now that we have acquired the pins, encode the mux setting */
	...
	ret = ops->set_mux(pctldev, setting->data.mux.func,
			   setting->data.mux.group);
    ...
}
```

# 驱动工程师视角使用pinctrl子系统

对于驱动工程师，只需要通过设备树文件就可以起到配置整个系统pin的目的。

pinctrl的实现不许用我们在驱动里调用任何它提供的api，所有的pinctrl动作都是在通用内核代码里完成了，对于驱动工程师是透明的。驱动工程师只需要通过设备树文件就能掌控整个系统的pin管理
