## 积分离散化处理


计算机处理的都是离散量，但实际系统往往具有连续性（仿真系统的数值/时间），该如何来实现。 

[离散化原理及要求和常用的几种数值积分法](https://wenku.baidu.com/view/541cc6d333d4b14e85246890.html)

[常微分方程的解法 (一): 常微分方程的离散化 :差商近似导数、数值积分方法、Taylor 多项式近似](https://blog.csdn.net/qq_29831163/article/details/89703074)

假设系统模型为：

$$\left\{\begin{matrix}
\cfrac{dy}{dx}=y'=f(x,y,t), a \leq x \leq b
\\ 
y(a)=y_0
\end{matrix}\right.$$

并且离散后的变量为$\hat{x}(t_k),\hat{y}(t_k)$, 其中$t_k=kh$,$h$为仿真时间间隔或采样间隔。

如果$\hat{x}(t_k) \approx x(t_k),\hat{y}(t_k) \approx y(t_k)$. 我们可以认为俩模型是等价的，也就是可以使用离散量来仿真。



Taylor多项式展开：

$$y(x_{n+1}) = y(x_n) +hy'(x_n)$$



# PID

## 公式结论

![PID控制器与被控对象关系](https://img-blog.csdnimg.cn/20181216103726414.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3RpbmdmZW5naGFubGVp,size_16,color_FFFFFF,t_70)

预定义：
- $k$ 采样序号, $k=0,1,2,3,...$
- $T$ 采样周期
- $K_p$是比例系数
- $T_I$是积分常数
- $T_D$是微分常数
- $e_k$是第$k$次采样时刻，被控对象输出与给定值的偏差值
- $u_k$是第$k$次采样时刻，PID控制器输出,被控对象输入
- $u_0$是控制常量


全量式：

$$u_k =K_p \times e_k + T_I  \times \sum \limits_{j=0}^k e(j) +T_D \times (e_k-e_{k-1}) +u_0$$

增量式：

$$\Delta u_k= u_k - u_{k-1}=K_p \times ([e_k-e_{k-1}] + \cfrac{T}{T_I} e_k + T_D \cfrac{e_k-2e_{k-1}+e_{k-2}}{T})$$

积分分离式：

$$u_k = \left\{\begin{matrix}
K_p(1+\cfrac{T_D}{T})e_k - K_p\cfrac{T_D}{T}e_{k-1} +u_0 & |e_k| > X\\ 
\Delta u_k + u_{k-1} & |e_k| \le  X
\end{matrix}\right.$$

    - 积分分离式的思想是当偏差过大时去掉积分采用PD，否则采用PID
    - 其中X为认为选择的一个阈值


### 原理推导
将偏差的比例P，微分D，积分I通过线性组合构成控制量的算法。

假设：
- $r(t)$是给定值(setpoint),
- $y(t)$是系统实际输出值，
- $e(t)=r(t)-y(t)$是控制偏差。

定义$u(t)=K_p [e(t)+\cfrac{1}{T_I} \int \limits_0^t e(t)dt + T_D\cfrac{de(t)}{dt}] + u_0$,作为PID控制器的输出与被控对象(Process)的输入。


其中:
- $K_p$是比例系数，在比例环节，成比例地反映控制系统的偏差信号e(t)，偏差一旦产生，控制器立即产生控制作用，以减小偏差。当仅有比例控制时系统输出存在稳态误差（Steady-state error）。显然P参数越小比例作用越强，动态响应越快，消除误差的能力越强。但实际系统是有惯性的，控制输出变化后，实际y(t)值变化还需等待一段时间才会缓慢变化。由于实际系统是有惯性的，比例作用不宜太强，比例作用太强会引起系统振荡不稳定。P参数的大小应在以上定量计算的基础上根据系统响应情况，现场调试决定，通常将P参数由大向小调，以能达到最快响应又无超调(或无大的超调)为最佳参数。
- $T_I$是积分常数，在积分环节，控制器的输出与输入误差信号的积分成正比关系。主要用于消除静差，提高系统的无差度。积分作用的强弱取决于积分时间常数T,T越大，积分作用越弱，反之则越强。
- $T_D$是微分常数，在微分环节，反映偏差信号的变化趋势，并能在偏差信号变得太大之前，在系统中引入一个有效的早期修正信号，从而加快系统的动作速度，减少调节时间。在微分控制中，控制器的输出与输入误差信号的微分（即误差的变化率）成正比关系。
- $u_0$是控制常量

PID控制器与被控对象关系:

![](https://img2018.cnblogs.com/blog/772331/201903/772331-20190311190707222-1055112250.png)


PID调试一般原则，动画模拟[见](https://zh.wikipedia.org/wiki/PID%E6%8E%A7%E5%88%B6%E5%99%A8)：

- a.在输出不振荡时，增大比例增益P。
- b.在输出不振荡时，减小积分时间常数Ti。
- c.在输出不振荡时，增大微分时间常数Td。

前面我们已经定义了被控对象的新输入$u(t)$,它是在连续空间的定义，但我们的目标是在计算机上计算，因此需要将它转换为离散量来仿真。下面记录的是推导过程，即离散化的处理方法。

设定：
- 以T作为采样周期，$k=0,1,2,,,$作为采样序号,离散采用实际$kT$对应连续时间$t$

利用梯度法有：

- $t \approx kT$
- $\int \limits_0^t e(t)dt \approx T \sum \limits_{j=0}^{k}e(jT)$
- $\cfrac{de(t)}{dt} \approx \cfrac{e(kT)-e[(k-1)T]}{T}$

将上述离散变换代入连续$u(t)=K_p [e(t)+\cfrac{1}{T_I} \int \limits_0^t e(t)dt + T_D\cfrac{de(t)}{dt}] + u_0$，可以得到：

$u(kT)=K_p [e(kT)+\cfrac{T}{T_I} \sum \limits_{j=0}^{k}e(jT) + \cfrac{T_D}{T} \times (e(kT)-e[(k-1)T]) + u_0$

其中：
- $u(kT)是第$k$次采用时刻计算机输出值，即PID控制器的输出/被控对象的输入$
- $e(kT)是第$k$次采用时刻的偏差$

上面的方法称为全量式或位置式PID控制算法，因为每次PID控制器的输出都需要依赖历史的所有状态。这个使用时还是不方便的，考虑到实际使用中，往往只是需要控制量的增量$\Delta u_k$,例如典型的步进电机的驱动，所以常常使用增量方法。

定义：
- $\Delta u(kT) = u(kT) - u[(k-1)T]$

根据全量式计算可以得到：

$\Delta u(kT) =A \times e(kT) + B \times e([k-1]T)  - C \times e([k-2]T)$

其中：
- $A = K_p (1+\cfrac{T}{T_I} +\cfrac{T_D}{T})$
- $B = - K_p (1+2\cfrac{T_D}{T})$
- $C = K_p \cfrac{T_D}{T}$

显然全量式也可以通过增量式来表达：$u(kT) = u[(k-1)T] +\Delta u(kT)$


为了方便书写，后面也会用$e_k$来表达$e(kT)$,$u_k$来表达$u(kT)$,$\Delta u_k$来表达$\Delta u(kT)$


## 用途

使得被控对象按照给定的设定值运行


### 算法改进：积分分离

### 算法改进：遇限消弱积分

### 算法改进：不完全积分

### PID算法实现与仿真

```c++
    // # 标准直接计算公式1：u_k=Kp*e(t) + Ki*Sum[e(t)] + Kd*[e(t) - e(t-1)]
float pidctrl_direct(float real_output/*采样时刻被控对象实际输出*/, std::size_t k/*采样序号*/) {
        auto e_k = this->setpoint - real_output; //e(t)
        this->e_list.push_back(e_k);

        auto u_k = this->kp * e_k +
            this->ki * std::accumulate(e_list.begin(), e_list.end(), 0.0) +
            this->kd * (e_list[k] - e_list[k - 1]);
        return u_k;
    }


    // # 增量计算公式：
    // # delta=Kp*[e(t) - e(t-1)] + Ki*e(t) + Kd*[e(t) - 2*e(t-1) +e(t-2)]
float pidctrl_increa(float real_output/*采样时刻被控对象实际输出*/) {
        auto err = this->setpoint - real_output;

        auto delta = this->kp * (err - this->e_last) +
            this->ki * err +
            this->kd * (err - 2 * (this->e_last) + this->e_prev);

        this->e_prev = this->e_last;
        this->e_last = err;
        return delta;
    }

```

仿真模拟

下面对位置式PID，增量PID进行仿真模拟。被控对象的初始输入随便写了值。被控对象是一个有高斯噪声的线性系统

```c++
//   全量式算法仿真
TEST(electrict, DISABLED_simulate_direct) {
    float setpoint=100;

    float Kp = 0.01f;
    float Ki = 0.1f;
    float Kd = 0.05f;

    std::random_device rd;
    std::mt19937 gen(rd());

    sys_modal_t system_modal = [&gen](float a)->float {
        std::uniform_real_distribution<float> att_i(0, 1.0);
        return 5 * a +   att_i(gen);//线性系统引入噪声
    };

    PID  mypid(setpoint, Kp, Kd, Ki);

    int K = 100;  //
    //保存被控对象输出
    std::vector<float> real_output;

    // k=0时刻被控对象输入值
    float u_0 = (60.0f);// random 此时未经过pid控制器，随机选择一个值
    real_output.push_back(system_modal(u_0)); // random
    mypid.e_list.push_back(setpoint - real_output[0]);

    for (auto k = 1; k < K; k++)
    {
        auto u_k = mypid.pidctrl_direct(real_output[k -1 ], k);
        real_output.push_back(system_modal(u_k));
    }

    std::vector<std::tuple<double/*x*/, double/*y*/>> data1, data2;
    // Prepare data
    for (int i = 0; i < K; ++i)
    {
        data1.push_back({ i, setpoint });
        data2.push_back({ i, real_output[i] });
    }
    plt::show_plots({ data1,data2 }, 1);


}


//  增量式算法仿真
TEST(electrict, simulate_increa) {
    float setpoint=100;

    float Kp = 0.005f;
    float Ki = 0.12f;
    float Kd = 0.05f;

    std::random_device rd;
    std::mt19937 gen(rd());

    sys_modal_t system_modal = [&gen](float a)->float {
        std::uniform_real_distribution<float> att_i(0, 1.0);
        return 5 * a +   att_i(gen);//线性系统引入噪声
    };


    PID  mypid(setpoint, Kp, Kd, Ki);

    int time_length = 1600; // 
    int K = 100;  //
    auto t = linspace(0, time_length, K);

    // 记录被控对象输入
    float u;
    // # k=0时刻被控对象输入值
    float u_0 = (5.0f);// random 此时未经过pid控制器，随机选择一个值
    //保存被控对象输出
    std::vector<float> real_output;
    real_output.push_back(system_modal(u_0));

    u=u_0;
    for (auto k = 1; k < K; k++)
    {
        auto delta = mypid.pidctrl_increa(real_output[k-1]);
        u = u + delta;//# 计算增量之后的新的被控对象输入

        real_output.push_back(system_modal(u)); //# 计算下一时刻(k)被控对象新的输出值
    }

    std::vector<std::tuple<double/*x*/, double/*y*/>> data1, data2;
    // Prepare data
    for (int i = 0; i < t.size(); ++i)
    {
        data2.push_back({ t[i], real_output[i] });
        data1.push_back({ t[i], setpoint });
    }
    plt::show_plots({ data1,data2 }, 1);


}

```