[linux-insides](https://0xax.gitbooks.io/linux-insides/content/)

# booting

以80386为例, 一旦上电计算机处于reset，此时CPU会处于以下值状态,并处于real-mode态：
```text
IP          0xfff0
CS selector 0xf000
CS base     0xffff0000
```

在real-mode态， 地址总线是20-bit address bus，意味着可以寻址"`0-0xFFFFF or 1 megabyte address space`". 但同时此时仅拥有寄存器是"`16-bit registers`",这意味着寄存器寻址能力是"`2^16 - 1 or 0xffff (64 kilobytes)`", 由此引入了实模式下的段寻址"`PhysicalAddress = Segment Selector * 16 + Offset`".例如对“`CS:IP is 0x2000:0x0010`”，得到的物理地址是
```text
>>> hex((0x2000 << 4) + 0x0010)
'0x20010'
```

linux对bootloader协议要求，例如："linux\Documentation\arm\Booting", 例如："https://github.com/torvalds/linux/blob/v4.16/Documentation/x86/boot.txt"


对x86是： Execution of the setup part starts from arch/x86/boot/header.S at the _start symbol.

# Kernel initialization process


