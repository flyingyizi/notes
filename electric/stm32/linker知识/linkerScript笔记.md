

## 实验一： MEMORY 与seciton VMA设置

- 实验对象

以下面的c文件main.c作为实验对象

```c
//for .data
char i='a';
//for .text
int xxmain(void) {  return 1; }
```

以下面linker script  linker.lds 链接文件作为实验对象

```ld
MEMORY
{
    rom (rx)  : ORIGIN = 0x500, LENGTH = 256k
    ram (!rx) : org = 0x400, l = 4M
}
SECTIONS  
{  
    .text 0x500  :  AT(0)  {  *(.text) }  > ram
    .data        :  AT(0x30)   {  *(.data) }  > rom
}  
```

执行下面的命令

```shell
/c/Users/tu_xu/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc -c  main.c -o main.o
/c/Users/tu_xu/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-ld -T linker.lds main.o -o t
/c/Users/tu_xu/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-objdump -h t
/c/Users/tu_xu/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-objcopy -O binary   t  t.out  
```
得到结果为：

```text
$ /c/Users/tu_xu/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-objdump -h t

t:     file format elf32-littlearm

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .text         0000001c  00000500  00000000  00000500  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .data         00000001  00000500  00000030  00010500  2**0
                  CONTENTS, ALLOC, LOAD, DATA
  2 .comment      0000007e  00000000  00000000  00010501  2**0
                  CONTENTS, READONLY
  3 .ARM.attributes 00000030  00000000  00000000  0001057f  2**0
                  CONTENTS, READONLY
$ ls -lh t.out
-rw-r--r-- 1 tu_xu 197609 49 4月  30 20:53 t.out
```

如果是将lds文件中的`    .data        :  AT(0x30)   {  *(.data) }  > rom`更改为`    .data        :  AT(0x30)   {  *(.data) }  > rom`,重新执行上面的命令得到的结果是：
```text
$ /c/Users/tu_xu/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-objdump -h t
t:     file format elf32-littlearm
Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .text         0000001c  00000500  00000000  00000500  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .data         00000001  00000500  00000500  00010500  2**0
                  CONTENTS, ALLOC, LOAD, DATA
  2 .comment      0000007e  00000000  00000000  00010501  2**0
                  CONTENTS, READONLY
  3 .ARM.attributes 00000030  00000000  00000000  0001057f  2**0
                  CONTENTS, READONLY

$ /c/Users/tu_xu/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-objcopy -O binary   t  t.out
$ ls -lh t.out
-rw-r--r-- 1 tu_xu 197609 1.3K 4月  30 20:56 t.out
```


从这里面可以反馈以下几个知识点：

- `> REGION`操作与直接指定out secion VMA同时存在情况下，`out secion VMA`是优先的，前提是该VMA value应在region范围里面，否正会报错“address xxxx of t section `.text' is not within region `ram'”

- LMA可以设定的与VMA完全无关，合理的LMA设置可以显著减少输出文件的大小，在上面例子中一个是49字节，一个是1.3k。愿意就是因为LMA的设置导致中间由很大空洞占用空间。

注意，上面的例子中的数字是随便设的，没有任何合理程度在里面。

同级目录下的makefile是实验lds的， 修改linker.lds文件， 执行`$ make clean  &&  make`看对应结果。

