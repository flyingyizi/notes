
[第31章 TIM—基本定时器—零死角玩转STM32-F429系列](https://www.cnblogs.com/firege/p/5805886.html)

[第32章 TIM—高级定时器—零死角玩转STM32-F429系列](https://www.cnblogs.com/firege/p/5805894.html)

SMT32F1系列共有8个定时器：

高级定时器（TIM1、TIM8）；通用定时器（TIM2、TIM3、TIM4、TIM5）；基本定时器（TIM6、TIM7）。

SMT32F4系列共有15个定时器：

高级定时器（TIM1、TIM8）；通用定时器（TIM2、TIM3、TIM4、TIM5、TIM9~TIM14）；基本定时器（TIM6、TIM7）。

# code sample

## Timer time-base configuration
```c++
#define ANY_DELAY_RQUIRED 0x0FFF
/* Hardware-precision delay loop implementation using TIM6 timer
peripheral. Any other STM32 timer can be used to fulfill this function, but
TIM6 timer was chosen as it has the less integration level. Other timer
peripherals may be reserved for more complicated tasks */
/* Clear the update event flag */
// TIM6->SR = 0
LL_TIM_ClearFlag_UPDATE(TIM6);
/* Set the required delay */
/* The timer presclaer reset value is 0. If a longer delay is required the
presacler register may be configured to */
/*TIM6->PSC = 0 */
// TIM6->ARR = ANY_DELAY_RQUIRED
LL_TIM_SetAutoReload(TIM6, ANY_DELAY_RQUIRED);
/* Start the timer counter */
// TIM6->CR1 |= TIM_CR1_CEN//
LL_TIM_EnableCounter(TIM6);

/* Loop until the update event flag is set */
// while (!(TIM6->SR & TIM_SR_UIF));
while(!LL_TIM_IsActiveFlag_UPDATE(TIM6));
/* The required time delay has been elapsed */
/* User code can be executed */
```

## Timer channel-configuration in input mode
```c++
/* Variable to store timestamp for last detected active edge */
uint32_t TimeStamp;
/* The ARR register reset value is 0x0000FFFF for TIM3 timer. So it should
be ok for this snippet */
/* If you want to change it uncomment the below line */
/* TIM3->ARR = ANY_VALUE_YOU_WANT */
/* Set the TIM3 timer channel 1 as input */
/* CC1S bits are writable only when the channel1 is off */
/* After reset, all the timer channels are turned off */
TIM3->CCMR1 |= TIM_CCMR1_CC1S_0;
/* Enable the TIM3 channel1 and keep the default configuration (state after
reset) for channel polarity */
// TIM3->CCER |= TIM_CCER_CC1E;
LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);
/* Start the timer counter */
// TIM3->CR1 |= TIM_CR1_CEN;
LL_TIM_EnableCounter(TIM3);
/* Clear the Capture event flag for channel 1 */
// TIM3->SR = ~TIM_SR_CC1IF;
LL_TIM_ClearFlag_CC1(TIM3);
/* Loop until the capture event flag is set */
// while (!(TIM3->SR & TIM_SR_CC1IF));
WHILE(!LL_TIM_IsActiveFlag_CC1(TIM3));
/* An active edge was detected, so store the timestamp */
// TimeStamp = TIM3->CCR1;
TimeStamp = LL_TIM_OC_GetCompareCH1(TIM3);
```

## Timer channel-configuration in output mode
```c++
/* The ARR register reset value is 0x0000FFFF for TIM3 timer. So it should
be ok for this snippet. If you want to change it uncomment the below line */
/* TIM3->ARR = ANY_VALUE_YOU_WANT */
/* The TIM3 timer channel 1 after reset is configured as output */
/* TIM3->CC1S reset value is 0 */
/* To select PWM2 output mode set the OC1M control bit-field to '111' */
TIM3->CCMR1 |= TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
/* Set the duty cycle to 50% */
TIM3->CCR1 = TIM3->ARR / 2;
/* By default, after reset, preload for channel 1 is turned off */
/* To change it uncomment the below line */
/* TIM3->CCMR1 |= TIM_CCMR1_OC1PE;
/* Enable the TIM3 channel1 and keep the default configuration (state after
reset) for channel polarity */
// TIM3->CCER |= TIM_CCER_CC1E;
LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH1);
/* Start the timer counter */
// TIM3->CR1 |= TIM_CR1_CEN;
LL_TIM_EnableCounter(TIM3);

```

#
### 时基

https://blog.csdn.net/m0_38064214/article/details/84285587



在基于STM32 HAL的项目中，一般需要维护的 “时基” 主要有2个：

- HAL的时基，SYS Timebase Source
- OS的时基（仅在使用OS的情况下才考虑）

而这些 “时基” 该去如何维护，主要分为两种情况考虑：

一、裸机运行
在这种情况下，需要维护的时间仅有SYS Timebase Source，也就是HAL库中的 uwTick，这是HAL库中维护的一个全局变量，比如在 stm32f1xx_hal.c/stm32f4xx_hal.c 等不同系列的文件中都有如下定义：
```c++
__IO uint32_t uwTick;
```

在 CubeMX 配置界面中关于 SYS Timebase Source 的[选择部分](https://img-blog.csdnimg.cn/20181120113850438.png)




## timer

[STM32 Timer Cookbook](https://www.st.com/resource/en/application_note/dm00236305-generalpurpose-timer-cookbook-for-stm32-microcontrollers-stmicroelectronics.pdf)

[STM32 Timer Presentation](https://www.st.com/content/ccc/resource/training/technical/product_training/c4/1b/56/83/3a/a1/47/64/STM32L4_WDG_TIMERS_GPTIM.pdf/files/STM32L4_WDG_TIMERS_GPTIM.pdf/jcr:content/translations/en.STM32L4_WDG_TIMERS_GPTIM.pdf)

注意，在使用stm32cubemx生成的硬件代码如果要结合os使用，注意要更改系统时钟源头，参见[例子](https://blog.csdn.net/zhanglifu3601881/article/details/89844466)
![更改系统时钟源头](https://img-blog.csdnimg.cn/20190505142705638.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3poYW5nbGlmdTM2MDE4ODE=,size_16,color_FFFFFF,t_70)

常见硬件功能：

- 单脉冲模式
- 输出比较（Output compare (OC)）：当计时器达到特定值时切换引脚
- 输入捕捉（Input capture (IC)）：测量引脚事件之间的计时器计数
- 脉宽调制（PWM）：当计时器达到特定值时以及翻转时切换引脚。通过调整开启与关闭时间（占空比），您可以有效地控制流向另一台设备的电量。
- 

对某个定时器，通常有四个独立通道，每个通道都可以选择上面硬件功能之一。


预分频器(prescale)，它是对时钟源进行分频的硬件。例如，80的预分频器会将80 MHz时钟变成1 MHz时钟。

    在CubeMX  timer透视图中，打开Timers并选择TIM16。将预分频器（PSC）设置为79。我将写“ 80-1”以表明预分频器值为79实际上意味着使用时钟分频器或80。预分频器因基于0的值而被减1，因为：PSC值“ 0”表示使用1的预分频器（时钟分频器）。Counter Period设置为“ 65536-1”以表明该计数器实际上总共总共拍了65536次（从65535到0的转换算作一个刻度）。

注意，您可以在任何给定时刻通过读取其CNT寄存器来获取其值。__HAL_TIM_GetCounter

### 定时器计数模式

通用定时器可以向上计数、向下计数、向上向下双向计数模式。

- 向上计数模式：计数器从0计数到自动加载值（TIMx_ARR），然后重新从0开始计数并且产生一个计数器溢出事件。
- 向下计数模式：计数器从自动装入的值（TIMx_ARR）开始向下计数到0，然后从自动装入的值重新开始，并产生一个计数器向下溢出事件。
- 中央对齐模式（向上/向下计数）：计数器从0开始计数到自动装入的值-1，产生一个计数器溢出事件，然后向下计数到1并且产生一个计数器溢出事件；然后再从0开始重新计数。

定时器溢出时间：

$$T_{out} =\frac{(arr+1)*(psc+1)}{Tclk}$$

例如：arr=4999  psc=7199 Tclk=72Mhz        Tout = (5000*7200)/72  us  = 500ms

![](https://img-blog.csdn.net/20180417164424341)


### 计数时钟的选择
计数器时钟可由下列时钟源提供：

- 内部时钟（TIMx_CLK），即在stm32cubemx“clock configuration”中对应的"APBx timer clocks(MHZ)"的值
- 外部时钟模式1：外部捕捉比较引脚（TIx）
- 外部时钟模式2：外部引脚输入（TIMx_ETR） 仅适用TIM2,3,4 。比如在PA0的TIM2_ETR。
- 内部触发输入（ITRx）：使用一个定时器作为另一个定时器的预分频器，如可以配置一个定时器Timer1而作为另一个定时器Timer2的预分频器。

PWM

[【STM32】HAL库 STM32CubeMX教程七---PWM输出(呼吸灯)原](https://blog.csdn.net/as480133937/article/details/99231677)


###  标准库与HAL

- TIM_Cmd  ---__HAL_TIM_ENABLE/__HAL_TIM_DISABLE
- TIM_ITConfig ---__HAL_TIM_ENABLE_IT/__HAL_TIM_DISABLE_IT
<<<<<<< HEAD
- TIM_ITConfig(TIMx, TIM_IT_CC1 | TIM_IT_Update, ENABLE) --- 使用在HAL_TIM_Base_MspInit中调用__HAL_TIM_ENABLE_IT(tim_baseHandle, TIM_IT_UPDATE)， 或可以看到TIM_IT_CC1由HAL_TIM_PWM_Start_IT ,TIM_IT_Update由HAL_TIM_Base_Start_IT自动开启了
- 
=======
- TIM_ITConfig(TIMx, TIM_IT_CC1 | TIM_IT_Update, ENABLE) --- TIM_IT_CC1由HAL_TIM_PWM_Start_IT ,TIM_IT_Update由HAL_TIM_Base_Start_IT


-  HAL_RCC_GetPCLK1Freq  对应APB1 PERIPHERAL CLOCKS(mhz)
-  HAL_RCC_GetHCLKFreq 对应 HCLK(MHZ)

apb1 timer clocks mhz


# SysTick (Coretex system timer(MHz) )

stm32cubemx配置结果中Coretex system timer(MHz) 是systick,也就是通常说的嘀嗒定时器。

嘀嗒定时器 比通用定时器TIMx 简单很多。

Systick是一个24位的递减计数器，用户仅需掌握ARM的CMSIS软件提供的一个函数`__STATIC_INLINE uint32_t SysTick_Config(uint32_t)`
函数的形参表示内核时钟多少个周期后触发一次Systick定时中断，比如形参配置为如下数值。 
- - SystemCoreClock / 1000 表示定时频率为 1000Hz， 也就是定时周期为 1ms。 
- - SystemCoreClock / 500 表示定时频率为 500Hz， 也就是定时周期为 2ms。 
- - SystemCoreClock / 2000 表示定时频率为 2000Hz， 也就是定时周期为 500us。



## PWM 原理：

当使用PWM驱动电机时，有条经验法则， PWM 频率至少应该是电机最高频率的 10 倍。

向上计数模式

![](https://img-blog.csdn.net/20180418203254526)

- 在PWM输出模式下，除了CNT（计数器当前值）、ARR（自动重装载值）之外，还多了一个值CCRx（捕获/比较寄存器值）。
- 当CNT小于CCRx时，TIMx_CHx通道输出低电平；
- 当CNT等于或大于CCRx时，TIMx_CHx通道输出高电平。

- ​PWM的一个周期

    - 定时器从0开始向上计数
    - 当0-t1段,定时器计数器TIMx_CNT值小于CCRx值,输出低电平（或高电平）
    - t1-t2段,定时器计数器TIMx_CNT值大于CCRx值,输出高电平（或低电平）
    - 当TIMx_CNT值达到ARR时,定时器溢出,重新向上计数...循环此过程
    - 至此一个PWM周期完成

- 总结:TIMx_ARR寄存器确定PWM频率，TIMx_CCRx寄存器确定占空比。输出PWM的占空比为$\frac{A}{(N+1)}$,其中A为TIMx_CCR值，N为TIMx_ARR值。

    - __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, pwmVal);    //修改比较值，修改占空比;等价TIM3->CCR1 = pwmVal； 等价htim3.Instance->CCR1 = pwmVal. CCR在stm32cubmemx 界面上对应“PWM generation channelx”中的“Pulse”

通电导线在磁场中受到的力称为安培力。安培力的公式：$F=BIL=\frac{BLU}{R}$,即电流强度$I$和导线的长度$L$的乘积成正比.

 PWM的本质就是脉宽调制，通过输出不同的占空比，从而将直流电压转换成不同电压值的模拟信号.正常的电机频率在 6-16kHZ之间为好

 PWM如果频率为50Hz ，也就是说一个周期是20ms 那么一秒钟就有 50次PWM周期

定时器溢出中断（也称为更新中断）的处理逻辑如下

![更新中断逻辑](http://6.eewimg.cn/news/uploadfile/2019/0530/20190530063656321.png)


### 电机速度与频率

通过上面的公式我们就可以轻松计算出对应的定时器频率：但是这里我们需要将分频系数固定一个合适的值。设置成多大合适了，这里我们就要来分析一下我们控制系统的步进电机的细分步距角和减速比了。

已经知道我们电机参数如下：步距角 = 1.8°  细分=16  减速比= 2mm

因此：一圈360°需要的脉冲数 =$\frac{360}{1.8} \times 16$ = 3200 pulse 

又因为电机转一圈，对应的距离是2mm, 所以 电机带动轮子走1mm,需要$\frac{3200pulse}{2} = 1600pluse$

```c++
#define  MM_TO_PLUSE  1600       //1mm对应的脉冲数
#define  PLUSE_TO_MM (1/1600)    //一个脉冲对应的距离
#define  DIS_MM_TO_PLUSE(dis)  ( MM_TO_PLUSE * (dis)  ) //将以mm为单位的长度抓换成对应的脉冲数
#define  SPEED_TO_PLUSE(speed) ( (speed) *MM_TO_PLUSE ) //将mm/s的速度转换成HZ 
```

到此为止，电机之间脉冲和距离之间的关系已经搞明白了，那我们开始言归正传，如何计算出我们需要的定时器频率输出了？

假设我们系统需要达到30mm/s的速度而且我们用的是timer2，调用宏计算 $30 * 1600 = 48000HZ= 48KHZ$的频率。意思就是说们只要定时器输出的PWM能够满足48KHZ的频率就可以了。

将上面的公式换算成 输出频率 =  定时器的时钟频率（注意是时钟频率不是输出频率）/（分频系数 + 1）/（ 计数值+1）

将psc = 0;分频系数为1 ，内部自动加1 ，带入上面的公式就可以计算出计数值 = 1000。就可以输出对应的速度了。

```c++
#define TIMER_CLK   (48000000/1) //48Mhz 不分频
#define CALC_ARR(speed)  （TIMER_CLK /（speed）*MM_TO_PLUSE  ）
```

知道速度值就可以调用CALC_ARR宏返回对应的ARR寄存器值啦，我们就可以根据机器的系统参数来控制了。注意，速度不能高于30000ms/s  = 30m/s的速度。因为定时的的最大频率就是48MHZ 


### stm32cubeMX 设置参数的解释
以STM32F411RE  TIM1为例：

通过stm32cubeMX clock configuration设置SYSCLK(也称为SystemCoreClock)系统时钟为96MHz，同时设置APB2 timer clock为96MHz。

配置结果中Coretex system timer(MHz) 是systick


TIM1是挂载在APB2总线上，因此TIM1 Timer_default_frequency是96MHz.

在stm32cubeMX TIM1中设置它`precaller(PSC 16bit) `为“`48-1`”,设置“`count period(autoload register-16 bits)`”为“`200-1`”

根据公式:
$timerTickFrequency = \cfrac{TimerDefaultFrequency}{prescaller + 1}$：

$PWMFrequency = \cfrac{timerTickFrequency}{TIMPeriod + 1}$

得到,最终生成的PWM 频率为10KHz:
```c++
timer_tick_frequency = 96Mhz / 48  = 2MHz(即2 000 000 Hz)
PWM_frequency = 2 000 000 / 200 = 10 000 Hz
```

在通常的工作中，我们是已知目标`PWM_frequency`，需要反算`TIM_Period`，这可以通过上面的公式反推得到：


在本例子中，TIM_Period是`16 bits`最大65535，如果得到的TIM_Period大于这个`max timer value`，显然死不合法的。我们要去输入一个合适的`timer_tick_frequency`，这可以给一个合适的`precaller`得到


占空比调节可以通过调用函数“`__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,80(如果(TIM_Period + 1)是100，则占空比:80%));`"进行设置，也可以通过"`duty circle = TIM1->CCR1 / arr(单位：%)`"进行设置, 其中：

//

[STM32F4 PWM tutorial with TIMERs](http://stm32f4-discovery.net/2014/05/stm32f4-stm32f429-discovery-pwm-tutorial/)


[STM32Cube的PWM控制基础篇（三）定时器的PWM设置详解](https://blog.csdn.net/ASWaterbenben/article/details/93466073)

1、Mode（PWM的模式）
该选项中有PWM Mode 1和PWM Mode 2两个选项; PWM Mode 1 ：正常理解的PWM波; PWM Mode 2 ：与PWM1模式互补的波（PWM模式1为高电平时PWM2为低电平，反之亦然）

2、Pulse（脉冲宽度）
Pulse16位二进制数，可以输入范围为0-2^16等于 0-65535的10进制数. 改变Pulse就是改变PWM的初始脉冲宽度

4、CH Polarity（频道有效电平【High/Low】）
顾名思义，这个参数就是控制有效电平的，因为有一些芯片的处理需要的是高电平有效，一些是需要低电平有效，为了在提高STM32适应性的同时不把我们程序猿的脑子搞乱，就有了这个设置

5、CH Idle State（空闲状态【Set/Reset】）
同样顾名思义，CH Idle State为该频道PWM不输出时的状态. Set为高电平,Reset为低电平

### 单极性和双极性PWM调制的区别在哪里 详解PWM中的单极性和双极性

[单极性和双极性PWM调制的区别在哪里 详解PWM中的单极性和双极性](http://www.elecfans.com/analog/20180824738048.html)



# 关于时钟的一些记录    

- TIMx 的时钟来自它们挂在的所在APBx总线，但之间不是相等关系。 在stm32cubeMX clock configuration中使用“APBx Timer clocks”显示出来的是TIMx的时钟，它有时和“APBx peripheral clocks ”是不一样的。这个不同是因为"当APBx的分频系数不为1的时候，TIMx CLK的时钟就是APBx的时钟乘以2。"的[特殊规则存在](http://news.eeworld.com.cn/uploadfile/2016/1208/1481163323473078.png)。

-  HCLK（AHB总线时钟）由系统时钟SYSCLK分频得到。由HCLK分频得到PCLK1/PCLK2。 PCLK2对应APB2外设， PCLK1对应APB1外设， 

- 1HZ的周期是1秒,代表一秒一个脉冲；50HZ的周期是1/50=0.02秒，代表0.2秒一个脉冲；10HZ的周期是1/10=0.1秒
- 1MHZ  = 1 000 000  HZ
- microseconds(ms)  = 1 000  milliseconds(us)。通常使用的规则：1 s = 1 000 ms = 1 000 000 us
- stm32cubeMX中的缩写： SYSCLK 系统时钟，也称为SystemCoreClock。 HCLK :AHB总线时钟
- 
当使用8MHZ的HSE时钟，我们要得到180MHz的SystemCoreClock，就可以采用下面的配置，结合stm32cubeMX 查看能更好帮助理解，下面配置的最终结果是`uint32_t SystemCoreClock = 180 000 000 hz;`. 详细解释参考[Library 03- STM32F4 system clock and delay functions](http://stm32f4-discovery.net/2014/04/library-03-stm32f429-discovery-system-clock-and-pretty-precise-delay-library/)

```C++
/* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N */
#define PLL_M      8
#define PLL_N      360 //Set this to 336 for 168MHz clock
 
/* SYSCLK = PLL_VCO / PLL_P */
#define PLL_P      2
```



### 正常输出，与互补输出

注意CHX,与CHXN启动的区别，在配置时是选择“PWM Mode 1”，还是选择“PWM Mode 2”

```c++
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);    //starts PWM on CH1 pin
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1); //starts PWM on CH1N pin
```

# STM32 Timer Cookbook 笔记 

[STM32 Timer Cookbook](https://www.st.com/resource/en/application_note/dm00236305-generalpurpose-timer-cookbook-for-stm32-microcontrollers-stmicroelectronics.pdf)

## STM32 timer peripheral tear-down

The STM32 timer 包含下面四个模块:
- 1. The master/slave controller unit
- 2. The time-base unit
- 3. The timer channels unit
- 4. The Break feature unit.  中断功能单元仅由具有互补输出的定时器外围设备嵌入。换句话说，只有至少有一个通道和两个互补输出的定时器外设才嵌入中断功能。

### Timer time-base configuration

```c++
#define ANY_DELAY_RQUIRED 0x0FFF
/* Hardware-precision delay loop implementation using TIM6 timer
peripheral. Any other STM32 timer can be used to fulfill this function, but
TIM6 timer was chosen as it has the less integration level. Other timer
peripherals may be reserved for more complicated tasks */
/* Clear the update event flag */
TIM6->SR = 0
/* Set the required delay */
/* The timer presclaer reset value is 0. If a longer delay is required the
presacler register may be configured to */
/*TIM6->PSC = 0 */
TIM6->ARR = ANY_DELAY_RQUIRED
/* Start the timer counter */
TIM6->CR1 |= TIM_CR1_CEN
```

