# arduino bootloader

Bootloader是单片机系统在加电后执行的第一段代码，在它完成CPU和相关硬件的初始化之后，再将单片机系统映像或固化的运行程序代码装在到内存中然后跳转到运行程序代码所在的空间，启动单片机系统运行。对于单片机系统，Bootloader是基于特定硬件平台来实现的。因此，几乎不可能为所有的单片机系统建立一个通用的Bootloader，不同的处理器架构都有不同的Bootloader。Bootloader不但依赖于CPU的体系结构，而且依赖于单片机系统板级设备的配置。对于2块不同的单片机板而言，即使它们使用同一种处理器，要想让运行在一块板子上的Bootloader程序也能运行在另一块板子上，一般也都需要修改Bootloader的源程序。

Arduino Uno 是基于 ATmega328P 的单片机开发板。该开发板由14路数字输入/输出引脚(其中6路可以用作PWM输出)、 6路模拟输入、1个16MHz的石英晶体振荡器、一个USB接口、1个电源接头、1个ICSP数据头以及1个复位按钮组成。Arduino Uno相对于duemilanove的一个显著变化就是其bootloader，uno使用了新版的bootloader后，同样是328P芯片，使用空间有32256字节，而duemilanove只有30720字节。新版bootloader的主要特点：（1）可用空间大了，比起原来2K的体积，减少了接近1.5KB。（2）上传程序速度更快，新版的bootloader波特率是115200。（3）对代码进行了优化，提高了运行效率，而且无看门狗问题。（4）支持使用ATMEGA8、168、328芯片的Arduino，未来可能会支持更多芯片。

Arduino Uno的Bootloader是运行在Atmega328p上的引导程序，Arduino Uno通过预先写入的Bootloader，每次通过串口给Bootloader板下载程序时，首先会令Bootloader板复位一次，这样就先进入了Bootloader，接着向Arduino Uno板发送特定的下载指令，Bootloader识别到这些指令后就会紧接着开始接收剩下的新的程序代码写入到内部flash中，如果没有进行下载，Bootloader没有识别到下载指令，那么它就会跳转到程序区正常工作。

## 为Atmega328P定制bootloader 添加自己的板卡到Arduino IDE

当参照Arduino官方的电路设计自己做板卡的时候，Arduino官方提供的bootloader可能无法满足需求，要定制生成自己的bootloader，可以选择使用optiboot（Arduino\hardware\arduino\avr\bootloaders\optiboot）作为蓝本

## usbtinyIsp