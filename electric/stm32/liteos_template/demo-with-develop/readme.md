

备忘1：

使用`git clone -b develop https://github.com/LiteOS/LiteOS.git` 下载的liteOS，参考的教程是 [STM32L431移植LiteOS 手把手教程](https://bbs.huaweicloud.com/forum/thread-12430-1-1.html) 



备忘2：

使用`git clone  https://github.com/LiteOS/LiteOS.git` 下载的liteOS,使用方法会与上面有较大的差异。 

--  los_compiler.h 更改为 los_toolchain.h

备忘3：

Systick 和pendsv函数重复定义，只需要在stm32l4xx_it.c文件的两个函数前面加上__weak 关键字进行修饰即可
`__weak void PendSV_Handler(void)`

`__weak void SysTick_Handler(void)`

