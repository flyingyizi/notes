
## stm32 GPIO  通过标准库使用STM32GPIO

### 操作步骤：

1. 使能GPIO对应的外设时钟,例如：

```C
   //使能GPIOA、GPIOB、GPIOC对应的外设时钟
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC , ENABLE);
```

2. 声明一个GPIO_InitStructure结构体 ​例如：

```C
  GPIO_InitTypeDef GPIO_InitStructure;
```

3. 选择待设置的GPIO管脚例如：

```C
//选择待设置的GPIO第7、8、9管脚位 ，中间加“|”符号
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 |GPIO_Pin_8 | GPIO_Pin_9;
```

1. 设置选中GPIO管脚的速率例如：

```c
//设置选中GPIO管脚的速率为最高速率2MHz
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//最高速率2MHz
```

4. 设置选中GPIO管脚的模式例如：

```c
//设置选中GPIO管脚的模式为开漏输出模式
GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_OD; //开漏输出模式
```

5. 根据GPIO_InitStructure中指定的参数初始化外设GPIOX,例如： 

```c
GPIO_Init(GPIOC, &GPIO_InitStructure);
```

7.其他应用

```c
 //例： 将端口GPIOA的第10、15脚置1（高电平） 
 GPIO_SetBits(GPIOA,GPIO_Pin_10 | GPIO_Pin_15);
 //例：将端口GPIOA的第10、15脚置0（低电平） 
 GPIO_ResetBits(GPIOA,GPIO_Pin_10 | GPIO_Pin_15);
```

### GPIO寄存器：

|寄存器  |  描述                   |   
| ------|-------------------------|
|CRL     |   端口配置低寄存器       |    
|CRH     |   端口配置高寄存器       |     
|IDR     |    端口输入数据寄存器    |      
|ODR     |  端口输出数据寄存器      |    
|BSRR    |  端口位设置/复位寄存器   |      
|BRR     |   端口位复位寄存器       |     
|LCKR    |  端口配置锁定寄存器      |     
|EVCR    |  事件控制寄存器          |    
|MAPR    | 复用重映射和调试         |    
|I/O     |     配置寄存器           |  
|EXTICR  | 外部中断线路0-15配置寄存器|    
|||

### GPIO库函数：

|函数名                 |     描述       |
|----------------------|----------------|        
|GPIO_DeInit            | 将外设GPIOx寄存器重设为缺省值                       |
|GPIO_AFIODeInit        | 将复用功能（重映射事件控制和EXTI设置）重设为缺省值    |
|GPIO_Init              | 根据GPIO_InitStruct中指定的参数初始化外设GPIOx寄存器 |
|GPIO_StructInit        | 把GPIO_InitStruct中的每一个参数按缺省值填入  |
|GPIO_ReadInputDataBit  | 读取指定端口管脚的输入  |
|GPIO_ReadInputData     | 读取指定的GPIO端口输入  |
|GPIO_ReadOutputDataBit | 读取指定端口管脚的输出  |
|GPIO_ReadOutputData    | 读取指定的GPIO端口输出  |
|GPIO_SetBits           | 设置指定的数据端口位    |  
|GPIO_ResetBits         | 清除指定的数据端口位    |  
|GPIO_WriteBit          | 设置或者清除指定的数据端口位  |
|GPIO_Write             | 向指定GPIO数据端口写入数据    |
|GPIO_PinLockConfig     | 锁定GPIO管脚设置寄存器       |
|GPIO_EventOutputConfig |  选择GPIO管脚用作事件输出  |
|GPIO_EventOutputCmd    |  使能或者失能事件输出  |
|GPIO_PinRemapConfig    |  改变指定管脚的映射  |
|GPIO_EXTILineConfig    |  选择GPIO管脚用作外部中断线路|

库函数： 函数GPIO_DeInit
功能描述：将外设GPIOx寄存器重设为缺省值 例：
GPIO_DeInit(GPIOA);

函数GPIO_AFIODeInit
功能描述：将复用功能（重映射事件控制和EXTI设置）重设为缺省值 例：
GPIO_AFIODeInit();

函数GPIO_Init
功能描述：根据GPIO_InitStruct中指定的参数初始化外设GPIOx寄存器 例：
GPIO_InitTypeDefGPIO_InitStructure; 
GPIO_InitStructure.GPIO_Pin= GPIO_Pin_All;  
GPIO_InitStructure.GPIO_Speed= GPIO_Speed_10MHz;  
GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN_FLOATING;  
GPIO_Init(GPIOA,&GPIO_InitStructure);​
GPIO_InitTypeDefstructure

GPIO_InitTypeDef定义于文件“stm32f10x_gpio.h”： 
typedef struct  {​
u16GPIO_Pin;
GPIOSpeed_TypeDefGPIO_Speed;  
GPIOMode_TypeDefGPIO_Mode;  
} GPIO_InitTypeDef;

GPIO_Pin
该参数选择待设置的GPIO管脚，使用操作符“|”可以一次选中多个管脚。可以使用下表中的任意组合。 

GPIO_Pin_None：   无管脚被选中  
GPIO_Pin_x：          选中管脚x（0--15）
GPIO_Pin_All：        选中全部管脚

GPIO_Speed ​
GPIO_Speed：用以设置选中管脚的速率。 

GPIO_Speed_10MHz： 最高输出速率10MHz 
GPIO_Speed_2MHz：  最高输出速率2MHz
GPIO_Speed_50MHz： 最高输出速率50MHz

GPIO_Mode
GPIO_Mode：用以设置选中管脚的工作状态。 
GPIO_Mode_AIN：                 模拟输入
GPIO_Mode_IN_FLOATING： 浮空输入  
GPIO_Mode_IPD：                 下拉输入
GPIO_Mode_IPU：                 上 拉输入  
GPIO_Mode_Out_OD：           开漏输出  
GPIO_Mode_Out_PP：             推挽输出  
GPIO_Mode_AF_OD：            复用开漏输出  
GPIO_Mode_AF_PP：             复用推挽输出

函数GPIO_StructInit
功能描述：把GPIO_InitStruct中的每一个参数按缺省值填入 例：
GPIO_InitTypeDefGPIO_InitStructure;  
GPIO_StructInit(&GPIO_InitStructure); 
GPIO_InitStruct：
GPIO_Pin：GPIO_Pin_All  
GPIO_Speed：GPIO_Speed_2MHz 
GPIO_Mode：GPIO_Mode_IN_FLOATING

函数GPIO_ReadInputDataBit​
功能描述：读取指定端口管脚的输入 
例：
u8ReadValue;
ReadValue= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7);

函数GPIO_ReadInputData
功能描述：读取指定的GPIO端口输入​
例：
u16ReadValue;
ReadValue= GPIO_ReadInputData(GPIOC);

函数GPIO_ReadOutputDataBit 
功能描述：读取指定端口管脚的输出 
例：
u8ReadValue;
ReadValue= GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_7);

函数GPIO_ReadOutputData
功能描述：读取指定的GPIO端口输出 
例：
u16ReadValue;
ReadValue= GPIO_ReadOutputData(GPIOC);

函数GPIO_SetBits
功能描述：置位指定的数据端口位
例： 将端口GPIOA的第10、15脚置1（高电平） 
GPIO_SetBits(GPIOA,GPIO_Pin_10 | GPIO_Pin_15);

函数GPIO_ResetBits
功能描述：清除指定的数据端口位
例：将端口GPIOA的第10、15脚置0（低电平）
GPIO_ResetBits(GPIOA,GPIO_Pin_10 | GPIO_Pin_15);

函数GPIO_WriteBit
功能描述：设置或者清除指定的数据端口位 
例：
GPIO_WriteBit(GPIOA,GPIO_Pin_15, Bit_SET);

函数GPIO_Write
功能描述：向指定GPIO数据端口写入数据 
例：
GPIO_Write(GPIOA,0x1101);

函数GPIO_PinLockConfig
功能描述：锁定GPIO管脚设置寄存器 
例：
GPIO_PinLockConfig(GPIOA,GPIO_Pin_0|GPIO_Pin_1);

函数GPIO_EventOutputConfig
功能描述：选择GPIO管脚用作事件输出 例：
GPIO_EventOutputConfig(GPIO_PortSourceGPIOE,GPIO_PinSource5);
GPIO_PortSource
GPIO_PortSource用以选择用作事件输出的GPIO端口。

函数GPIO_EventOutputCmd
功能描述：使能或者失能事件输出 例：
GPIO_EventOutputConfig(GPIO_PortSourceGPIOC,GPIO_PinSource6);  
GPIO_EventOutputCmd(ENABLE);

函数GPIO_PinRemapConfig​
功能描述：改变指定管脚的映射例：
GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);


## 一．GPIO概述

1、共有8种模式，可以通过编程选择：
   1.浮空输入    2. 带上拉输入   3. 带下拉输入      4. 模拟输入
   5.开漏输出——(此模式可实现hotpower说的真双向IO)       6. 推挽输出
   7.复用功能的推挽输出    8. 复用功能的开漏输出

模式7和模式8需根据具体的复用功能决定。      

2、专门的寄存器(GPIOx_BSRR和GPIOx_BRR)实现对GPIO口的原子操作，即回避了设置或清除I/O端口时的“读-修改-写”操作，使得设置或清除I/O端口的操作不会被中断处理打断而造成误动作。

3、每个GPIO口都可以作为外部中断的输入，便于系统灵活设计。

4、I/O口的输出模式下，有3种输出速度可选(2MHz、10MHz和50MHz)，这有利于噪声控制。这个速度是指I/O口驱动电路的响应速度而不是输出信号的速度，输出信号的速度与程序有关（芯片内部在I/O口的输出部分安排了多个响应速度不同的输出驱动电路，用户可以根据自己的需要选择合适的驱动电路）。通过选择速度来选择不同的输出驱动模块，达到最佳的噪声控制和降低功耗的目的。高频的驱动电路，噪声也高，当不需要高的输出频率时，请选用低频驱动电路，这样非常有利于提高系统的EMI性能。当然如果要输出较高频率的信号，但却选用了较低频率的驱动模块，很可能会得到失真的输出信号。  
   4.1各种接口的措施：     
      4.1.1对于串口，假如最大波特率只需115.2k，那么用2M的GPIO的引脚速度就够了，既省电也噪声小。
      4.1.2对于I2C接口，假如使用400k波特率，若想把余量留大些，那么用2M的GPIO的引脚速度或许不够，这时可以选用10M的GPIO引脚速度。
      4.1.3对于SPI接口，假如使用18M或9M波特率，用10M的GPIO的引脚速度显然不够了，需要选用50M的GPIO的引脚速度。
  4.2 GPIO口设为输入时，输出驱动电路与端口是断开，所以输出速度配置无意义。   
  4.3 在复位期间和刚复位后，复用功能未开启，I/O端口被配置成浮空输入模式。  
  4.4 所有端口都有外部中断能力。为了使用外部中断线，端口必须配置成输入模式。  
  4.5 GPIO口的配置具有上锁功能，当配置好GPIO口后，可以通过程序锁住配置组合，直到下次芯片复位才能解锁。

5、所有I/O口兼容CMOS和TTL，多数I/O口兼容5V电平。
6、大电流驱动能力：GPIO口在高低电平分别为0.4V和VDD-0.4V时，可以提供或吸收8mA电流；如果把输入输出电平分别放宽到1.3V和VDD-1.3V时，可以提供或吸收20mA电流。
7、具有独立的唤醒I/O口。
8、很多I/O口的复用功能可以重新映射。

9、GPIO口的配置具有上锁功能，当配置好GPIO口后，可以通过程序锁住配置组合，直到下次芯片复位才能解锁。此功能非常有利于在程序跑飞的情况下保护系统中其他的设备，不会因为某些I/O口的配置被改变而损坏——如一个输入口变成输出口并输出电流。

## 二．推挽结构

一般是指两个三极管分别受两互补信号的控制,总是在一个三极管导通的时候另一个截止.要实现线与需要用OC(opencollector)门电路 .如果输出级的有两个三极管，始终处于一个导通、一个截止的状态，也就是两个三级管推挽相连，这样的电路结构称为推拉式电路或图腾柱（Totem- pole）输出电路（可惜，图无法贴上）。当输出低电平时，也就是下级负载门输入低电平时，输出端的电流将是下级门灌入T4；当输出高电平时，也就是下级负载门输入高电平时，输出端的电流将是下级门从本级电源经 T3、D1 拉出。这样一来，输出高低电平时，T3 一路和 T4 一路将交替工作，从而减低了功耗，提高了每个管的承受能力。又由于不论走哪一路，管子导通电阻都很小，使RC常数很小，转变速度很快。因此，推拉式输出级既提高电路的负载能力，又提高开关速度。供你参考。

推挽电路是两个参数相同的三极管或MOSFET,以推挽方式存在于电路中,各负责正负半周的波形放大任务,电路工作时，两只对称的功率开关管每次只有一个导通，所以导通损耗小效率高。


输出既可以向负载灌电流，也可以从负载抽取电流

## 三．开漏电路

在电路设计时我们常常遇到开漏（open drain）和开集（open collector）的概念。所谓开漏电路概念中提到的“漏”就是指MOSFET的漏极。同理，开集电路中的“集”就是指三极管的集电极。开漏电路就是指以MOSFET的漏极为输出的电路。一般的用法是会在漏极外部的电路添加上拉电阻。完整的开漏电路应该由开漏器件和开漏上拉电阻组成。


组成开漏形式的电路有以下几个特点：


1. 利用 外部电路的驱动能力，减少IC内部的驱动。当IC内部MOSFET导通时，驱动电流是从外部的VCC流经R pull-up ，MOSFET到GND。IC内部仅需很下的栅极驱动电流。如图1。

2. 可以将多个开漏输出的Pin，连接到一条线上。形成 “与逻辑” 关系。如图1，当PIN_A、PIN_B、PIN_C任意一个变低后，开漏线上的逻辑就为0了。这也是I2C，SMBus等总线判断总线占用状态的原理。

3. 可以利用改变上拉电源的电压，改变传输电平。如图2, IC的逻辑电平由电源Vcc1决定，而输出高电平则由Vcc2决定。这样我们就可以用低电平逻辑控制输出高电平逻辑了。     

4. 开漏Pin不连接外部的上拉电阻，则只能输出低电平(因此对于经典的51单片机的P0口而言，要想做输入输出功能必须加外部上拉电阻，否则无法输出高电平逻辑)。     

5. 标准的开漏脚一般只有输出的能力。添加其它的判断电路，才能具备双向输入、输出的能力。

应用中需注意：

1.  开漏和开集的原理类似，在许多应用中我们利用开集电路代替开漏电路。例如，某输入Pin要求由开漏电路驱动。则我们常见的驱动方式是利用一个三极管组成开集电路来驱动它，即方便又节省成本。如图3。

2. 上拉电阻R pull-up的 阻值 决定了 逻辑电平转换的沿的速度 。阻值越大，速度越低功耗越小。反之亦然。

Push-Pull输出就是一般所说的推挽输出，在CMOS电路里面应该较CMOS输出更合适，应为在CMOS里面的push－pull输出能力不可能做得双极那么大。输出能力看IC内部输出极N管P管的面积。和开漏输出相比，push－pull的高低电平由IC的电源低定，不能简单的做逻辑操作等。 push－pull是现在CMOS电路里面用得最多的输出级设计方式。  at91rm9200 GPIO 模拟I2C接口时注意！！

## 四.OC、OD


集电极开路门(集电极开路 OC 或源极开路OD)

open-drain是漏极开路输出的意思，相当于集电极开路(open-collector)输出，即ttl中的集电极开路（oc）输出。一般用于线或、线与，也有的用于电流驱动。

open-drain是对mos管而言，open-collector是对双极型管而言，在用法上没啥区别。

开漏形式的电路有以下几个特点：

1.利用外部电路的驱动能力，减少IC内部的驱动。 或驱动比芯片电源电压高的负载.     2. 可以将多个开漏输出的Pin，连接到一条线上。通过一只上拉电阻，在不增加任何器件的情况下，形成“与逻辑”关系。这也是I2C，SMBus等总线判断总线占用状态的原理。如果作为图腾输出必须接上拉电阻。接容性负载时，下降延是芯片内的晶体管，是有源驱动，速度较快；上升延是无源的外接电阻，速度慢。如果要求速度高电阻选择要小，功耗会大。所以负载电阻的选择要兼顾功耗和速度。

3.可以利用改变上拉电源的电压，改变传输电平。例如加上上拉电阻就可以提供TTL/CMOS电平输出等。

4.开漏Pin不连接外部的上拉电阻，则只能输出低电平。一般来说，开漏是用来连接不同电平的器件，匹配电平用的。

5.正常的CMOS输出级是上、下两个管子，把上面的管子去掉就是OPEN-DRAIN了。这种输出的主要目的有两个：电平转换和线与。      

6.由于漏级开路，所以后级电路必须接一上拉电阻，上拉电阻的电源电压就可以决定输出电平。这样你就可以进行任意电平的转换了。

7.线与功能主要用于有多个电路对同一信号进行拉低操作的场合，如果本电路不想拉低，就输出高电平，因为OPEN-DRAIN上面的管子被拿掉，高电平是靠外接的上拉电阻实现的。（而正常的CMOS输出级，如果出现一个输出为高另外一个为低时，等于电源短路。）


8.OPEN-DRAIN提供了灵活的输出方式，但是也有其弱点，就是带来上升沿的延时。因为上升沿是通过外接上拉无源电阻对负载充电，所以当电阻选择小时延时就小，但功耗大；反之延时大功耗小。所以如果对延时有要求，则建议用下降沿输出。

## 五.线或逻辑与线与逻辑
在一个结点(线)上, 连接一个上拉电阻到电源 VCC 或 VDD 和 n 个 NPN 或 NMOS 晶体管的集电极 C 或漏极 D, 这些晶体管的发射极 E 或源极 S 都接到地线上, 只要有一个晶体管饱和, 这个结点(线)就被拉到地线电平上.

因为这些晶体管的基极注入电流(NPN)或栅极加上高电平(NMOS),晶体管就会饱和, 所以这些基极或栅极对这个结点(线)的关系是或非 NOR 逻辑. 如果这个结点后面加一个反相器, 就是或 OR 逻辑.

注：个人理解：线与，接上拉电阻至电源。(~A)&(~B)=~(A+B)，由公式较容易理解线与此概念的由来 ；

如果用下拉电阻和 PNP 或 PMOS 管就可以构成与非 NAND 逻辑, 或用负逻辑关系转换与/或逻辑.

注：线或，接下拉电阻至地。(~A)+(~B)=~(AB);

这些晶体管常常是一些逻辑电路的集电极开路 OC 或源极开路 OD 输出端. 这种逻辑通常称为线与/线或逻辑, 当你看到一些芯片的 OC或 OD 输出端连在一起, 而有一个上拉电阻时, 这就是线或/线与了, 但有时上拉电阻做在芯片的输入端内.

顺便提示如果不是 OC 或 OD 芯片的输出端是不可以连在一起的, 总线 BUS 上的双向输出端连在一起是有管理的, 同时只能有一个作输出, 而其他是高阻态只能输入