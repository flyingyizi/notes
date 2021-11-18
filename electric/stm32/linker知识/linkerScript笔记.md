

[Linker Scripts](http://sourceware.org/binutils/docs/ld/Scripts.html#Scripts),[中文](https://blog.csdn.net/yyww322/article/details/50827418?utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromMachineLearnPai2%7Edefault-1.control&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromMachineLearnPai2%7Edefault-1.control)

[link script reference document](http://blog.chinaunix.net/uid-10678279-id-2936584.html)

在embedded develop中，`link-time garbage collection is in use (‘--gc-sections’)`务必需要，mark sections that should not be eliminated. This is accomplished by surrounding an input section’s wildcard entry with KEEP(), as in `KEEP(*(.init)) or KEEP(SORT_BY_NAME(*)(.ctors))`.

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

```
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

如果是将lds文件中的`    .data        :     {  *(.data) }  > rom`更改为
                  `    .data        :  AT ( LOADADDR (.text) + SIZEOF (.text) )   {  *(.data) }  > rom`,
                  
重新执行上面的命令得到的结果是：
```
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


# 语法

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

## 1.ENTRY
```
ENTRY(SYMBOL)
```

将符号 SYMBOL 的值设置成入口地址。入口地址（entry point）: 进程执行程序的第一 条指令在进程虚拟地址空间中的地址。ld 有多种方法设置入口地址，按以下顺序排列：

- ld 命令行的 -e 选项
- 链接脚本的 ENTRY(SYMBOL) 指令
- 如果定义了 start 符号， 使用 start 符号值
- 如果存在 .text section，使用 .text section 的第一字节的位置值
- 使用值 0

## 2.EXPRESSION

表达式格式如下：
```
SYMBOL = EXPRESSION
SYMOBL += EXPRESSION
SYMBOL -= EXPRESSION
SYMBOL *= EXPRESSION
SYMBOL /= EXPRESSION
SYMBOL <<= EXPRESSION
SYMBOL >>= EXPRESSION
SYMBOL &= EXPRESSION
SYMBOL |= EXPRESSION
```
一个简单例子
```c++
SECTIONS
{
    //_edata 符号的值是 .data section 的末尾值（绝对值，在程序地址空间 内）
    .data : { *(.data) _edata = ABSOLUTE(.); }
}
```

### 链接脚本相关的内建函数：

```
ABSOLUTE(EXP): 转换成绝对值

ADDR(SECTION): 返回某 section 的 VMA 值

ALIGN(EXP): 返回定位符 . 的修调值，对齐后的值，(. + EXP - 1) & ~(EXP - 1)

BLOCK(EXP): 如同 ALIGN(EXP)，为了向前兼容

DEFINED(SYMBOL): 如果符号 SYMBOL 在全局符号表内，且被定义了，那么返回 1， 否则返回 0
LOADADDR(SECTION): 返回某 SECTION 的 LMA

MAX(EXP1, EXP2): 返回大者

MIN(EXP1,EXP2): 返回小者

NEXT(EXP): 返回下一个能被使用的地址，该地址是 EXP 的倍数，类似于 ALIGN(EXP)。除非使用 MEMORY 命令定义了一些非连续的内存块，否则 NEXT(EXP) 与 ALIGN(一定相同)

SIZEOF(SECTION): 返回 SECTION 的大小。当 SECTION 没有被分配时，即此时 SECTION 大小不能确定，链接器会报错，

SIZEOF_HEADERS: 返回输出文件的文件头大小，用以确定第一个 section 的开始 地址
```

## 3.PHDRS
```
PHDRS {
    NAME TYPE [FILEHDR][PHDRS][AT(ADDRESS)]
    [FLAGS(FLAGS)]
}
```

PHDRS 命令仅仅在生成 ELF 目标文件时有效。ELF 目标文件格式用 program headers 程 序头 (程序头内包含一个或多个 segment 程序段描述) 来描述程序如何被载入内存。可 以使用 objdump -p 命令查看。

## 4.SECTIONS

```
SECTIONS
{
    SECTIONS-COMMAND
    SECTIONS-COMMAND
}
```

SECTIONS 命令告诉 ld 如何把输入文件的 sections 映射到输出文件的各个 section。 对于如何将输入 section 合为输出 section；以及如何把输出 section 放入程序地址空 间 (VMA) 和进程地址空间 (LMA)。使用方式如上。SECTIONS-COMMAND 有四种，分别为：
- ENTRY
- 符号赋值语句
- 输出 section 描述
- Section 叠加描述

如果整个链接脚本内没有 SECTIONS-COMMAND， 那么 ld 将所有同名输入 section 合为 一个输出 section 内，各输入 section 的顺序为他们被链接器发现的顺序。如果某输入 section 没有在 SECTIONS 命令中提到，那么该 section 将被直接拷贝成输出 section。

## 5.output section description

该部分内容详见“输出section描述”

## 6.overlay

[overlay description](https://biscuitos.github.io/blog/LD-SECTIONS/#Section%20%E5%8F%A0%E5%8A%A0%E6%8F%8F%E8%BF%B0)

overlay语法，以及与SECTIONS 之间的层级关系：
```
SECTIONS
{
    //NOCROSSREFS 关键字指定各 section 之间不能交叉引用，否则报错
    //START 指定所有 SECTION 的 VMA 地址
    //AT 指定了第一个 SECTION 的 LMA 地址
    OVERLAY [START] : [NOCROSSPEFS][AT(LDADDR)]
    {
        SECTION1 [ADDRESS][(TYPE)]:[AT(LMA)]
        {
            OUTPUT-SECTION-COMMAND
            OUTPUT-SECTION-COMMAND
            ....
        } [>REGION][AT>LMA_REGION][:PHDR HDR ...][=FILLEXP]
        SECTION2 [ADDRESS][(TYPE)]:[AT(LMA)]
        {
            OUTPUT-SECTION-COMMAND
            OUTPUT-SECTION-COMMAND
            ....
        } [>REGION][AT>LMA_REGION][:PHDR HDR ...][=FILLEXP]
    }
}
```

同一覆盖图内的 section 具有相同的 VMA。SECTION2 的 LMA 为 SECTION1 的 LMA 加上 SECTION1 的大小。同理计算 SECTION2,3,4… 的 LMA。 SECTION1 的 LMA 由 该输出段 的 AT() 指定，如果没有指定，那么由 START 关键字指定，如果没有 START，那么由当 前位符号的值决定。

对于 OVERLAY 描述的每个 section，链接器定义两个符号 __load_start_SECNAME 和 __load_stop_SECNAME，这两个符号的分别代表 SECNAME section 的 LMA 地址开始和结 束。链接器处理完 OVERLAY 描述语句之后，将定位符号的值加上所有覆盖图内 section 大小的最大值。

一个简单例子,针对这个例子，会定义符号 __load_start_DemoText 与 __load_stop_DemoText； 以及 __load_start_DemoData 与  __load_stop_DemoData
```
SECTIONS
{
    OVERLAY 0x10000 : AT(0x4000)
    {
        DemoText  { *(.text) }

        DemoData  { *(.data) }
    } 
}
```

# 输出section描述

输出 section 描述的具体格式如下,以及输出 section 描述 与 SECTIONS 之间的关系如下：

```
SECTIONS
{
  ...
  SECTION [ADDRESS][(TYPE)]:[AT(LMA)]
  {
      OUTPUT-SECTION-COMMAND
      OUTPUT-SECTION-COMMAND
      ....
  } [>REGION][AT>LMA_REGION][:PHDR HDR ...][=FILLEXP]
  ...
}
```

在链接脚本的内建函数中，与输出section相关的函数包括：
- LOADADDR(section) : Return the absolute LMA of the named section. (see Output Section LMA).
- ADDR(section) : Return the address (VMA) of the named section.
- SIZEOF(section) : Return the size in bytes of the named section,


一个简单的例子：

SECTIONS
{
    DemoText : { *(.text) }
}

## 输出 section 名字(SECTION)

输出 section 名字简单例子如下：
```
SECTIONS
{
    DemoText : { *(.text) }
}
```

输出 section 名字必须符合输出文件的格式要求，比如： a.out 格式的文件只允许存 在 .text， .data 和 .bss section 名。而有的格式只允许存在数字名字，那么此时应 该用引号将所有名字内核数字组合在一起；另外，还有一些格式运行任何顺序的字符存在 于 section 名字内，此时如果名字内包含特殊字符 (比如空格，逗号等)，那么需要引号 将其组合在一起。

SECTION 左右的空白，圆括号，冒号是必须的，换行符合其他空格是可选的。并且 SECTION 和冒号之间必须有一个空格，

## 输出 section 类型(TYPE)

每个输出 section 都有一个类型，如果没有指定 TYPE 类型，那么链接器根据输出 section 引用输入 section 的类型设置该输出 section 的类型，TYPE 可以有五种值：

- NOLOAD: 该 section 在程序运行时，不被载入内存
- DESCT： 向后兼容保留下来的，这种类型的 section 必须被标记为 “不可被加载 的”，以便在程序运行不为它们分配内存。
- COPY：向后兼容保留下来的，这种类型的 section 必须被标记为 “不可被加载的”， 以便在程序运行不为它们分配内存。
- INFO：向后兼容保留下来的，这种类型的 section 必须被标记为 “不可被加载的”， 以便在程序运行不为它们分配内存。
- OVERLAY：向后兼容保留下来的，这种类型的 section 必须被标记为 “不可被加载 的”，以便在程序运行不为它们分配内存。

一个简单例子如下：
```
SECTIONS
{
    DemoText (NOLOAD) : { *(.comment) }
}
```

## 输出 section VMA 地址 (ADDRESS)

ADDRESS 是一个表达式，它的值用于设置 VMA，VMA 表示可执行程序的地址空间。

- 如果没 有该选项且有 REGION 选项，那么链接器根据 REGION 设置 VMA。
- 如果也没有 REGION 选 项，那么链接器将根据定位符号 “.”的值设置该 section 的 VMA.

```
简单例子：

SECTIONS
{
    .text               . : { *(.text) }
    .stab        ALIGN(0) : { *(.stab) }
    .stabstr            0 : { *(.stabstr) }
    .comment   0x08058000 : { *(.comment) }
}
```


## 输出 section 段 LMA 地址： AT(LMA)

LMA 为程序的加载地址，默认情况下，LMA 等于 VMA，但可以通过关键字 AT() 指定 LMA。 如果不用 AT() 关键字，那么可以用 AT>LMA_REGION 表达式设置指定该 section 加载地 址的范围。

一个简单例子：
```
SECTIONS
{
    DemoText : AT(0x0848000) { *(.text) }

    MEMORY { rom : ORIGIN = 0x1000, LENGTH = 0x1000 }

    DemoData : { *(.data)} > rom
}
```

### LMA和VMA

LMA和VMA进行说明：每个output section都有一个LMA和一个VMA，LMA是其存储地址，而VMA是其运行时地址，例如将全局变量g_Data所在数据段.data的LMA设为0x80000020（属于ROM地址），VMA设为0xD0004000（属于RAM地址），那么g_Data的值将存储在ROM中的0x80000020处，而程序运行时，用到g_Data的程序会到RAM中的0xD0004000处寻找它。

由于默认情况下LMA=VMA，因此可以只定义VMA而不必指明LMA


总结来说，要指定某个section的LMA和VMA了，有4种方法：

- a) `[<address>] + [AT(<lma>)]`；例如`.my_data {} 0xD0004000 AT 0x80000020`

- b) `[<address>] + [AT><lma region>]`；例如`.my_data :　{} 0xD0004000 AT> rom`

- c) `[><region>] + [AT><lma region>]`；例如`.my_data :　{} > ram AT> rom`

- d) `[><region>] + [AT(<lma>)]`； 例如`.my_data :　{} > ram AT 0x80000020`


## 输出 section 的填充模板(FILLEXP)

任何输出 section 描述内的未指定的内存区域内的未指定内存区域，链接器用该模板填 充该区域。用法 =FILEEXP，前两字节有效，当区域大于两个字节时，重复使用这两个字 节以其填满

```
一个简单例子：

SECTIONS
{
    DemoText : { *(.text) } =0x3456

    DemoData : { *(.data)} 
}
```

## 输出 section 描述命令(OUTPUT-SECTION-COMMAND)

输出 section 描述命令(OUTPUT-SECTION-COMMAND)主要包含以下几类内容：

- 输入 section 描述
- 垃圾回收
- 输入 section 描述中存放数据命令
- 输入 section 描述中的命令关键字
- 输出 section 丢弃

### 输入 section 描述

最常见的输出 section 命令就是输入 section 描述。输入 section 描述的最基本的链 接脚本描述，基本语法如下：
```
//FILENAME 表示文件名，可以是一个特定的文件的名字，也可以是一个特定的字符串
FILENAME(EXCLUDE_FILE(FILENAME1 FILENAME2 ...) SECTION1 SECTION2 ...)
```

例如"` (*(EXCLUDE_FILE(*crtend.o *otherfile.o) .ctors))`"; 例如"`*(.text)`"

例如"`*(.text .data)`": 表示所有文件的 `.text section` 和 `.data section`，顺序是第一个 文件的 `.text section`, 第一个文件的 `.data section`， 第二个文件的 `.text section`, 第二个文件的 `.data section`

例如"`*(.text) *(.data)`": 表示所有文件的 `.text section` 和 `.data section`， 顺序是第 一个文件的 `.text section`， 第二个文件的 `.text section` … 最后一个文件的 `.text section`. 第一个文件的 `.data section`，第二个文件的 `.data section` …. 最 后一个文件的 `.data section`

### 垃圾回收/丢弃
在链接命令行内使用了选项 –gc-section 后，链接器可能将某些它认为没有用的 section 过滤掉，此时就有必要强制链接器保留一些 section，可用 KEEP 关键字，简 单例子如下：
```
SECTIONS
{
    DemoText : { *(.text) }
    DemoComment : { KEEP(*(.comment)) }
}
```

通过 DISCARD 关键字，将所有输入 文件的 .comment section 丢弃
```
SECTIONS
{
    DemoText : { *(.text) }
    /DISCARD/ :  { *(.comment) }
}
```

### 存放数据命令

能够在输出 section 中显示的填入指定的信息，可以使用如下关键字：

- BYTE(EXPRESSION) 1 字节
- SHORT(EXPRESSION) 2 字节
- LONG(EXPRESSION) 4 字节
- QUAD(EXPRESSION) 8 字节
- SQUAD(EXPRESSION) 64 位系统时是 8 字节

例如：
```
SECTIONS
{
    .text : { *(.text) LONG(1) }
    .data : { *(.data) }
}
```

### 命令关键字

- CREATE_OBJECT_SYMBOLS ： 为每个输入文件建立一个符号，符号名为输入文件的名字。每个符号所在的 section 是 出现该关键字的 section。

  一个简单的例子：
  ```
  SECTIONS
  {
      DemoData : { 
          CREATE_OBJECT_SYMBOLS
          *(.data)
          _edata = ALIGN(0x808765)
      }
  }
  ```

- CONSTRUCTORS



## 输出 section 所在的程序段(PHDR)

仅适用ELF

可以将输出 section 放入预先定义的程序段 (Program Segment) 内。如果某个输出 section 设置了它所在的一个或多个程序段，那么接下来定义的输出 section 的默认 程序段与该输出 section 的相同。除非再次显示地指定。可以通过 :NONE 指定链接器 不把该 section 放入任何程序段内。

```
PHDRS
{
    headers PT_PHDR PHDRS;
    interp PT_INTERP;
    text PT_LOAD FILEHDR PHDRS;
    data PT_LOAD;
    dynamic PT_DYNAMIC;
}

SECTIONS
{
    . = 0x8048000 + SIZEOF_HEADERS;
    .interp : { *(.interp) } :text :interp
    .text { *(.text) } :text
   .rodata : { *(.rodata) } /* Defaults to .text */

    ....

    . = . + 0x1000;  /* mov to a new page in memory */
    .data : { *(.data) } :data
    .dynamic : { *(.dynamic) } :data : dynamic

    ....
}
```

## location counter

位置计数器(location counter) 的符号是dot ".". 注意：
- 只能出现在section命令的表达式中
- 位置计数器只能向前移，不能向后移动， 即类似 "`. += xxx`"是合法的，类似 "`. -= xxx`"是非法的。
- dot "."指从当前包含对象(e.g SECTIONS, xx output section)开始的字节偏移量. 特别是对“SECTIONS”,它的start address是0，因此针对SECTIONS，dot "."也可以被当作绝对地址。在下面的这个例子中解释了在SECTIONS 与 特定section中对dot "." 解释的差异

```c++
SECTIONS
{
    /*The ‘.text’ section will be assigned a starting address of 0x100 and a size of 
      exactly 0x200 bytes*/
    . = 0x100
    .text: {
      *(.text)
      . = 0x200
    }
    /*The ‘.data’ section will start at 0x500 and it will have an extra 0x600 bytes worth of 
      space after the end of the values from the ‘.data’ input sections and before the 
      end of the ‘.data’ output section itself.*/
    . = 0x500
    .data: {
      *(.data)
      . += 0x600
    }
}
```

- 对相邻的两个output section之间的assignments，有如下规则：
  - it assumes that all assignments or other statements belong to the previous output section, except for the special case of an assignment to ..
  
  - as the linker assumes that an assignment to . is setting the start address of a following output section and thus should be grouped with that section. 
  
- 因此推荐在output section前面带一个dot "."赋值。下面的例子演示了相关影响

```c++
SECTIONS
{
    start_of_text = . ;
    .text: { *(.text) }
    end_of_text = . ;
    /*假如没有下面的".=.;", 如果存在orphan sections（孤立段）被插入，则下面的“start_of_data = . ;”结果可能不是我们期望的，
      因为orphan section有可能被放置在“start_of_data = . ;”后面*/
    . = . ;
    start_of_data = . ;
    .data: { *(.data) }
    end_of_data = . ;
}
```

## 显式填充output section

- using BYTE, SHORT, LONG, QUAD, or SQUAD as an output section command. 
- use the FILL command to set the fill pattern for the current section. 
- 
```c++
SECTIONS { 
   .text : { 
     *(.text) ; 
     /*using BYTE, SHORT, LONG, QUAD, or SQUAD as an output section command*/
     LONG(1) 
   } 
   /*You may use the FILL command to set the fill pattern for the current section. 
   It is followed by an expression in parentheses. Any otherwise unspecified regions 
   of memory within the section (for example, gaps left due to the required alignment 
   of input sections) are filled with the value of the expression, repeated as necessary. 
   
   A FILL statement covers memory locations after the point at which it occurs in the 
   section definition; by including more than one FILL statement, you can have different 
   fill patterns in different parts of an output section.*/
   .xdata : { 
     *(.xdata) ;
     FILL(0x90909090)
   } 
   .data : { 
     *(.data) } =0x90909090 
} 


```

#

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
如果目标文件中有定义etext符号，则链接脚本中通过`PROVIDE(etext = .);`定义的符号etext则会被忽略。

显然，如果目标文件中定义了_etext，则会导致重复定义的错误。



