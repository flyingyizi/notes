

[书籍附带源码](http://github.com/cnoviello/mastering-stm32)

- c语言代码查看汇编
     That assembly code was generated compiling in thumb mode with any optimization disabled, invoking GCC in the following way: 
     ```shell
     $arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -fverbose-asm -save-temps -O0 -g -c file.c
     ```

- coretex-M内存布局

     ![](img/cortexM_fixed_memory_addressspace.PNG)

     对其中的code area(0.5G)中，FLASH固定是0x0800 0000开始，结束依据厂家而定，即不同行号的flash大小不一样。
     
     对其中的sram(0.5G)其中最关键的是bitband。采用bit-band访问的地址可以总结为下面的公式:
     ```text
     This is the formula to compute the addresses for alias regions:
     bit_band_address = alias_region_base + (region_base_offset x 32) + (bit_number x 4)
     ```

     显然对peripheral:
     - alias_region_base == 0x42000000
     - region_base_offset == 地址（e.g. GPIOA） - - 0x40000000
     - 举例，对GPIOA PIN5采用bit-band访问的地址应该是
     ```text
     alias_region_base = 0x42000000
     region_base_offset = 0x40020014 - 0x40000000 = 0x20014
     bit_band_address = 0x42000000 + 0x20014*32 + (0x5 x 0x4) = 0x42400294
     ```

    ```c
    //We can define two macros in C that allow to easily compute bit-band alias addresses:
    // Define base address of bit-band
    #define BITBAND_SRAM_BASE 0x20000000
    // Define base address of alias band
    #define ALIAS_SRAM_BASE 0x22000000
    // Convert SRAM address to alias region
    #define BITBAND_SRAM(a,b) ((ALIAS_SRAM_BASE + (uint32_t)&(a)-BITBAND_SRAM_BASE)*32 + (b*4)))
    
    // Define base address of peripheral bit-band
    #define BITBAND_PERI_BASE 0x40000000
    // Define base address of peripheral alias band
    #define ALIAS_PERI_BASE 0x42000000
    // Convert PERI address to alias region
    #define BITBAND_PERI(a,b) ((ALIAS_PERI_BASE + (uint32_t)a-BITBAND_PERI_BASE)*32 + (b*4)))
    ```

## 开源工具类

CooCox³ and System Workbench for STM32⁴ (shortened as SW4STM32) , Atollic trueStudio

[gnu arm工具](https://developer.arm.com/)

[ST新一代烧写工具 STM32CubeProgrammer]


# 7. Interrupts Management

## 7.2 Enabling Interrupts

mcu一旦启动，只有`Reset, NMI and Hard Fault exceptions`是使能的。其他的中断都需要通过`void HAL_NVIC_EnableIRQ(IRQn_Type IRQn);`开启中断。

对应的disable是`void HAL_NVIC_DisableIRQ(IRQn_Type IRQn);`

以GPIO EXTIXX中断为例， “7.2.2 Enabling Interrupts With CubeMX”章节介绍了可以自动生成哪些代码。

## 7.3 Interrupt Lifecycle

状态可以是：
- 1. either be disabled (default behavior) or enabled;• we enable/disable it calling the HAL_NVIC_EnableIRQ()/HAL_NVIC_DisableIRQ() function;
- 2. either be pending (a request is waiting to be served) or not pending;
- 3. either be in an active (being served) or inactive state.

# 8. Universal Asynchronous Serial Communications

uart可以工作的模式：polling（blocking）, interrupt（no-blocking） and DMA mode.

在“8.3.1 Installing a Serial Console in Windows”章节介绍了在集成环境安装serial terminal工具，如果集成工具是stm32cubeide，有些特殊，请按照下面步骤按照
```text
Go to menu Help -> Install New Software
Select Work with -> All available sites
In the search box type "terminal"
When search is finished select "TM Terminal" and "TM Terminal Serial Extensions" and install them.

Restart CubeIDE
From menu Window -> Show View -> Other select "Terminal"

Terminal window should appear in IDE.
Click Open Terminal icon (or Shift+Alt+Ctrl+T) and set up your connection parameters.
This is the result in my case:
....
```

如果是vscode环境， 如果我们按照了IoT link，则已经自动按照了serial terminal工具了。在任务栏可以看到。

## 8.4 UART Communication in Interrupt Mode

在该模式下，通过"`HAL_UART_Transmit_IT`, `HAL_UART_Receive_IT`"进行数据

The list of USART related interrupts：

- Transmit Data Register Empty ：TXE
- Clear To Send (CTS) flag ：CTS
- Transmission Complete ：TC
- Received Data Ready to be Read： RXNE
- Overrun Error Detected： ORE
- Idle Line Detected： IDLE
- Parity Error： PE
- Break Flag： LBD
- Noise Flag, Overrun error and Framing，Error in multi buffer communication ： NF or ORE or FE EIE




$(\frac{89}{2})^2 + 5^2 = $