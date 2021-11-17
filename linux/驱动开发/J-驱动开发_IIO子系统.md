





# IIO子系统

[Linux 内核](https://blog.csdn.net/lickylin/category_6104997.html)

     本章我们对IIO子系统做一个简单的概述，IIO子系统主要提供对ADC、DAC相关的设备驱动框架，该子系统的设计目的主要是驱动那些分类时处于hwmon和input子系统之间的设备。同时查看linux内核的驱动代码，其实iio、hwmon之间的界限并不是那么明显。而iio子系统所面向的设备包含：

ADC芯片；
DAC芯片；
温度传感器；
光感器；
陀螺仪；
加速度计；
CDCs；
IMUs
压力传感器等等
————————————————
版权声明：本文为CSDN博主「jerry_chg」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/lickylin/article/details/108177756



# PM: 供电(Power Supply)

