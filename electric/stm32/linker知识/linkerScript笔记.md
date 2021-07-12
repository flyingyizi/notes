

[link script reference document](http://blog.chinaunix.net/uid-10678279-id-2936584.html)

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
    .data        :     {  *(.data) }  > rom
}  
```
<!--  -->

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
  1 .data         00000001  00000500  00000500  00010500  2**0
                  CONTENTS, ALLOC, LOAD, DATA
  2 .comment      0000007e  00000000  00000000  00010501  2**0
                  CONTENTS, READONLY
  3 .ARM.attributes 00000030  00000000  00000000  0001057f  2**0
                  CONTENTS, READONLY
$ ls -lh t.out
-rw-r--r-- 1 tu_xu 197609 1.3K 4月  30 20:53 t.out
```

如果是将lds文件中的`    .data        :     {  *(.data) }  > rom`更改为`    .data        :  AT ( LOADADDR (.text) + SIZEOF (.text) )   {  *(.data) }  > rom`,重新执行上面的命令得到的结果是：
```text
$ /c/Users/tu_xu/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-objdump -h t
t:     file format elf32-littlearm
Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .text         0000001c  00000500  00000000  00000500  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .data         00000001  00000500  0000001c  00010500  2**0
                  CONTENTS, ALLOC, LOAD, DATA
  2 .comment      0000007e  00000000  00000000  00010501  2**0
                  CONTENTS, READONLY
  3 .ARM.attributes 00000030  00000000  00000000  0001057f  2**0
                  CONTENTS, READONLY

$ /c/Users/tu_xu/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-objcopy -O binary   t  t.out
$ ls -lh t.out
-rw-r--r-- 1 tu_xu 197609 29 4月  30 20:56 t.out
```

从这里面可以反馈以下几个知识点：

- `> REGION`操作与直接指定out secion VMA同时存在情况下，`out secion VMA`是优先的，前提是该VMA value应在region范围里面，否正会报错“address xxxx of t section `.text' is not within region `ram'”

- LMA可以设定的与VMA完全无关，合理的LMA设置可以显著减少输出文件的大小，在上面例子中一个是29字节，一个是1.3k。愿意就是因为LMA的设置导致中间由很大空洞占用空间。

注意，上面的例子中的数字是随便设的，没有任何合理程度在里面。

同级目录下的makefile是实验lds的， 修改linker.lds文件， 执行`$ make clean  &&  make`看对应结果。


## 语法

典型的linker script的内容包括如下部分：

```c++
/* 将符号xxxx SYMBOL的值设置成入口地址 */
ENTRY(xxxx)

/* Specify the memory areas */
MEMORY
{
  /* <name> [(<attr>)] : ORIGIN = <origin>, LENGTH = <len> */
  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
}

/* Define output sections */
SECTIONS
{
/*把定位器符号置为0x10000 (若不指定, 则该符号的初始值为0).*/  
. = 0x10000;
/* 将所有(*符号代表任意输入文件)输入文件的.textsection合并成一个.textsection,该section的地址由定位器符号的值指定, 即0x10000.*/
.text : { *(.text) }
/*把定位器符号置为0x8000000*/
. = 0x8000000;
/*将所有输入文件的.text section合并成一个.data section, 该section的地址被置为0x8000000.*/
.data : { *(.data) }
/*将所有输入文件的.bss section合并成一个.bss section，该section的地址被置为0x8000000+.datasection的大小.*/
.bss : { *(.bss) }  
}
```
这里强调下“定位符号器”，连接器每读完一个section描述后, 将定位器符号的值*增加*该section的大小.

### 符号赋值

在目标文件内定义的全局变量符号可以在链接脚本内被赋值. 下面演示了在链接脚本中更改这个符号对应的地址。

这个赋值操作支持任何c语言内的赋值操作，例如`+=, =, -=, *=, /=,...`

"`.`" “定位符号器”是一个特殊的符号，它是定位器，一个位置指针，指向程序地址空间内的某位置(或某section内的偏移，如果它在SECTIONS命令内的某section描述内)，该符号只能在SECTIONS命令内使用。当然支持这里提到的符号赋值规则。

```c
/* a.c */
#include <stdio.h>
int a = 100;
int main(void)
{
    printf( "&a=0x%p ", &a );
    return 0;
}
```

```lds
/* a.lds */
a = 3;
```

```shell
$ gcc -Wall -o a-without-lds a.c
&a = 0x8049598
$ gcc -Wall -o a-with-lds a.c a.lds
&a = 0x3
```

### PROVIDE

PROVIDE关键字
该关键字用于定义这类符号：在目标文件内被引用，但没有在任何目标文件内被定义的符号。
例子：

```c
SECTIONS
{
  .text :
  {
  *(.text)
  _etext = .;
  PROVIDE(etext = .);
  }
}
```
当目标文件内引用了etext符号，确没有定义它时，etext符号对应的地址被定义为.textsection之后的第一个字节的地址。


### LMA和VMA

LMA和VMA进行说明：每个output section都有一个LMA和一个VMA，LMA是其存储地址，而VMA是其运行时地址，例如将全局变量g_Data所在数据段.data的LMA设为0x80000020（属于ROM地址），VMA设为0xD0004000（属于RAM地址），那么g_Data的值将存储在ROM中的0x80000020处，而程序运行时，用到g_Data的程序会到RAM中的0xD0004000处寻找它。

由于默认情况下LMA=VMA，因此可以只定义VMA而不必指明LMA

```c++
/*address/region:用于设置VMA。两者只能存其一或都不存在。如果都不存在，则根据"."设置该section的VMA*/
/*               注意：`.text . :{...}`与`.text :{...}`这两个描述是截然不同的，第一个将
                 .text section的VMA设置为定位符号的值，而第二个则是设置成定位符号的修调值，满足对齐要求后的。*/
<section> [<address>] [(<type>)] : [AT(<lma>)]
{
　　<output−section−command>
　　<var{output−section−command>
　　...
} [><region>] [AT><lma region>] [:<phdr> :<phdr> ...] [=<fillexp>]
```

总结来说，要指定某个section的LMA和VMA了，有4种方法：

- a) [<address>] + [AT(<lma>)]；例如`.my_data {} 0xD0004000 AT 0x80000020`

- b) [<address>] + [AT><lma region>]；例如`.my_data :　{} 0xD0004000 AT> rom`

- c) [><region>] + [AT><lma region>]；例如`.my_data :　{} > ram AT> rom`

- d) [><region>] + [AT(<lma>)]； 例如`.my_data :　{} > ram AT 0x80000020`

