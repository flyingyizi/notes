

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
- TIM_ITConfig(TIMx, TIM_IT_CC1 | TIM_IT_Update, ENABLE) --- TIM_IT_CC1由HAL_TIM_PWM_Start_IT ,TIM_IT_Update由HAL_TIM_Base_Start_IT
- 

## PWM原理：

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

![](http://6.eewimg.cn/news/uploadfile/2019/0530/20190530063656321.png)


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




