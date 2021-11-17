


# 8.pwm子系统

linux\Documentation\pwm.txt

[Linux pwm子系统分析之一 系统框架说明](https://blog.csdn.net/lickylin/article/details/106449647)

[Linux pwm子系统分析之二 gpio-pwm驱动实践](https://blog.csdn.net/lickylin/article/details/106449675)


## BSP工程师视角使用PWM子系统API

pwm_chip层主要对应一个pwm控制器，一个pwm控制器可包含多个pwm_device，针对一个pwm_chip，提供了访问pwm 控制器的方法，通过pwm_chip提供的方法，实现对pwm 控制器的配置；

- 编写pwm controller驱动，主要是：构造pwm_chip, 然后通过 pwmchip_add注册它。简要来说就是：

    - 创建platform device，用于存储该pwm chip相关的配置参数，如pwm base index、pwm num、pwm操作相关的寄存器参数等  等；
    - 创建platform driver，在该driver的probe接口中完成pwm chip的注册，主要包括：
    - 申请struct pwm_chip类型的内存空间；
    - 实现struct pwm_ops中各成员接口，主要实现与pwm 控制器的通信；根据“linux\Documentation\pwm.txt”建议：
       - a.实现pwm_chip.apply。 Drivers are encouraged to implement ->apply() instead of the legacy
        ->enable(), ->disable() and ->config() methods. Doing that should provide
        atomicity in the PWM config workflow, which is required when the PWM controls
        a critical device (like a regulator).

       - b.推荐实现pwm_chip.get_state。 The implementation of ->get_state() (a method used to retrieve initial PWM state) is also encouraged for the same reason: letting the PWM user know about the current PWM state would allow him to avoid glitches.
    - 调用pwmchip_add，完成pwm chip的添加。

	实现以上几步，即可完成pwm chip的注册。


- When implementing polarity support in a PWM driver, make sure to respect the
signal conventions in the PWM framework. By definition, normal polarity
characterizes a signal starts high for the duration of the duty cycle and
goes low for the remainder of the period. Conversely, a signal with inversed
polarity starts low for the duration of the duty cycle and goes high for the
remainder of the period.


## 驱动工程师视角使用PWM子系统API

api通过pwm号在pwm_tree基数树中找到对应的pwm_device（所有注册的pwm device均会添加至pwm_tree中，另外一个pwm_chip可包含多个pwm_device，因此pwm_chip与pwm_device之间也存在关联），并借助pwm_chip提供的方法配置pwm控制器，实现pwm配置等操作；

https://gitee.com/low-level-of-logic/RaspberryPi/blob/master/docs/0025_硬件PWM控制风扇.md

### A.1.使能pwm

//请求pwm

- 获得pwm：
	|descriptor-based         |legacy          | 说明|
	|-----------------------|------------------|-----|
	|pwm_get                |pwm_request       |to request a PWM device|
	|devm_pwm_get           |                  ||
	||||

### A.2.配置pwm（配置pwm的占空比、频率等）

After being requested, a PWM has to be configured using::

	int pwm_apply_state(struct pwm_device *pwm, struct pwm_state *state);

This API controls both the PWM period/duty_cycle config and the
enable/disable state.

The pwm_config(), pwm_enable() and pwm_disable() functions are just wrappers
around pwm_apply_state() and should not be used if the user wants to change
several parameter at once. For example, if you see pwm_config() and
pwm_{enable,disable}() calls in the same function, this probably means you
should switch to pwm_apply_state().

The PWM user API also allows one to query the PWM state with pwm_get_state().

In addition to the PWM state, the PWM API also exposes PWM arguments, which
are the reference PWM config one should use on this PWM.
PWM arguments are usually platform-specific and allows the PWM user to only
care about dutycycle relatively to the full period (like, duty = 50% of the
period). struct pwm_args contains 2 fields (period and polarity) and should
be used to set the initial PWM config (usually done in the probe function
of the PWM user). PWM arguments are retrieved with pwm_get_args().

### A.3.去使能pwm

	|descriptor-based         |legacy          | 说明|
	|-----------------------|------------------|-----|
	|pwm_put                |pwm_free          |to free the PWM device|	
	|devm_pwm_put           |                  ||
	||||




### Using PWMs with the sysfs interface
-----------------------------------

If CONFIG_SYSFS is enabled in your kernel configuration a simple sysfs
interface is provided to use the PWMs from userspace. It is exposed at
/sys/class/pwm/. Each probed PWM controller/chip will be exported as
pwmchipN, where N is the base of the PWM chip. Inside the directory you
will find:

  npwm
    The number of PWM channels this chip supports (read-only).

  export
    Exports a PWM channel for use with sysfs (write-only).

  unexport
   Unexports a PWM channel from sysfs (write-only).

The PWM channels are numbered using a per-chip index from 0 to npwm-1.

When a PWM channel is exported a pwmX directory will be created in the
pwmchipN directory it is associated with, where X is the number of the
channel that was exported. The following properties will then be available:

  period
    The total period of the PWM signal (read/write).
    Value is in nanoseconds and is the sum of the active and inactive
    time of the PWM.

  duty_cycle
    The active time of the PWM signal (read/write).
    Value is in nanoseconds and must be less than the period.

  polarity
    Changes the polarity of the PWM signal (read/write).
    Writes to this property only work if the PWM chip supports changing
    the polarity. The polarity can only be changed if the PWM is not
    enabled. Value is the string "normal" or "inversed".

  enable
    Enable/disable the PWM signal (read/write).

	- 0 - disabled
	- 1 - enabled
