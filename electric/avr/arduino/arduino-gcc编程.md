
# 使用AVR-GCC编程Arduino

[英文](http://www.javiervalcarce.eu/wiki/Program_Arduino_with_AVR-GCC),[中文](https://blog.csdn.net/liujiandu101/article/details/79869616)

建议仔细看“doc\avr-libc\avr-libc-user-manual\group__demo__project.html”


- blink例子

```shell
c:\home\notes\电子\arduino>C:\tools\msys64\mingw64\bin\mingw32-make.exe hex

c:\home>C:\prog\Arduino\hardware\tools\avr\bin\avr-gcc.exe   -g -Os -mmcu=atmega8 -c blink.c
In file included from blink.c:2:0:
c:\prog\arduino\hardware\tools\avr\avr\include\util\delay.h:92:3: warning: #warning "F_CPU not defined for <util/delay.h>" [-Wcpp]
 # warning "F_CPU not defined for <util/delay.h>"
   ^
c:\home>C:\prog\Arduino\hardware\tools\avr\bin\avr-gcc.exe   -g -mmcu=atmega8 -o blink.elf blink.o
c:\home>
c:\home>C:\prog\Arduino\hardware\tools\avr\bin\avr-objcopy.exe  -j .text -j .data -O ihex blink.elf blink.hex
c:\home>
```



## vscode avr-gcc环境

在vscode中编辑c_cpp_properties.json：修改includePath看起来像这样：
```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "C:/prog/Arduino/hardware/tools/avr/avr/include",
                "${workspaceFolder}/**",
                "C:/prog/Arduino/hardware/tools/avr/lib/gcc/avr/4.9.2/include",
                "C:/prog/Arduino/hardware/arduino/avr/cores/arduino",
                "C:/prog/Arduino/hardware/arduino/avr/variants/standard",
                "C:/prog/Arduino"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE",
                "__AVR_ATmega328P__"
            ],
            "windowsSdkVersion": "",
            "compilerPath": "C:\\prog\\Arduino\\hardware\\tools\\avr\\bin\\avr-g++.exe",
            "cStandard": "c11",
            "cppStandard": "c++17",
            "intelliSenseMode": "clang-x64"
        }
    ],
    "version": 4
}
```

因为`#include <avr/io.h>`中是通过定义不同的mcu引入的，为了查看代码所以任意定义了个`__AVR_ATmega328P__`.

## 调试

通常调试都采用串口输出方式，如果需要类似单步那就需要使用硬件（avarice）或软件模拟（simulavr,simavr）了。

从[avr simulavr homepage](https://www.nongnu.org/simulavr/)看window version已经很久没有更新过了，例如atmega328p不支持。在linux环境下可以使用"apt-get install simavr"，看它支持的模拟列表还是挺多的

[simavr homepage](https://github.com/buserror/simavr),
[Debugging-AVR-code-in-Linux-with-simavr](http://www.instructables.com/id/Debugging-AVR-code-in-Linux-with-simavr/),[simavr get started](http://fabricesalvaire.github.io/simavr/gitorious-get-started.html)


AVaRICE is a program which interfaces the GNU Debugger GDB with the AVR JTAG ICE available from Atmel. [AVR-GDB_and_AVaRICE_Guide](http://winavr.sourceforge.net/AVR-GDB_and_AVaRICE_Guide.pdf)



## AVRGCC通用Makefile（带注释）

```makefile
#××××××××××××××××××××××××××××××
# AVR-GCC makefile 模板 ×
# semilog ×
# E-mail: 
# 时间：2007年11月9日 ×
# 地点：南京理工大学 ×
# 注：在原来的WinAVR 的例程的基础上修改,翻译,注释,并且测试 ×
#××××××××××××××××××××××××××××××
# Hey Emacs, this is a -*- makefile -*-
#----------------------------------------------------------------------------
# WinAVR Makefile Template written by Eric B. Weddington, Joerg Wunsch, et al.
#
# Released to the Public Domain
#
# Additional material for this makefile was written by:
# Peter Fleury
# Tim Henigan
# Colin O'Flynn
# Reiner Patommel
# Markus Pfaff
# Sander Pool
# Frederik Rouleau
# Carlos Lamas
#
#----------------------------------------------------------------------------
# 编译操作命令如下:
#
# make all = 编译整个工程.
#
# make clean = 删除所有前一次编译的整个工程所产生的文件
#
# make coff = 转换 ELF 调试文件到 AVR 的 COFF 调试文件.适用于AVRStudio.3.X及以前的版本. 
# VMLAB <3.10 
#
# make extcoff = 转换 ELF 调试文件到 AVR 的 Extended COFF 调试文件. VMLAT 3.10+ # 适用于AVRStudio.4.07 及以后的版本。也就是现在我们要使用的版本 # 但是好像AVRStdio.4.10及以上的版本开始支持 ELF 调试文
#
# make program = 用 avrdude(gnu的avr单片机下载软件) 下载 hex 文件到AVR单片机内,
# 不要再这之前要先配置好 avrdude .注：我们可以用双龙的下载软件等。
#
# make debug = 启动 simulavr 或 avarice 进行调试，你用 avr-gdb 或是avr-insight作的你的调试前端 
#
# make filename.s = 单独编译 filename.c 到汇编文件filename.s . 
#
# make filename.i = 通过这个命令进行预编译，来查看GCC工程是否有错. 
#
# 说了这么多，最有用的就是下面这两句： # make clean # make all # 这样你的工程就编译完成了。 #----------------------------------------------------------------------------

#下面对AVR-GCC参数的说明 
#注：关于Makefile的知识网上有很多，看看Makefile的手册
# AVR单片机的名字。这个很重要，不可写错，因为这个参数真接关系到文件所包含的文件
# 这个文件包含了所以的对于 你所选的AVR 单片机的所有引脚和寄存器定义。
# 就如keil 51中的引用或一样
MCU = atmega128
# MCU 的运行频率.
# 下面是给出的一些典型值:
# F_CPU = 1000000  #默认的1Mhz
# F_CPU = 1843200
# F_CPU = 2000000
# F_CPU = 3686400
# F_CPU = 4000000
# F_CPU = 7372800
# F_CPU = 8000000
# F_CPU = 11059200
# F_CPU = 14745600
# F_CPU = 16000000
# F_CPU = 18432000
# F_CPU = 20000000
# 在使用中的延时程序时很有用
F_CPU = 7372800
# 输出文件格式，对于单片机编程来说，一般我们先intel 的hex格式. (可以是: srec, ihex, binary)
FORMAT = ihex
# 目标文件名，一般就是你最main()函数所在文件名，也是最后输的hex文年的文件名(不带扩展名).
TARGET = m128
#目标文件的临时目录，就选 "obj"这个名子吧，一般不用修改
OBJDIR = obj
# 列出你的C语言的工程中的所有的 *.C 源文件名. (C 的所有的文件依赖关系会自己动建立，后面会说.)
# 对于只有一个源文的工程也可以写成: SRC = $(TARGET).c
SRC = m128.c
# 列出你的 C++工程中的所有的 *.cpp源文件名. (C++的所有的文件依赖关系也会自己动建立.)
# 对于我们对AVR单片机编程来说，一般我们只用C语言就可以了。所以这里我们什么都不用写
# 如果你要用C++也可以，这里你就要配置下。
CPPSRC = 
# 列出你的工程中的所有的汇编源程序
# 记住要用大写.S保存你的汇编文件名，不要用小写的.s 。要不然AVR-GCC不认为它是个汇编源文件
# 而且在执行 make clean 时也删除 .s 的文件，因为它会认为是编译C文件生成的汇编过程文件
ASRC =
 
# AVR-GCC代码优化级别，选项：[0, 1, 2, 3, s]. 级别越高，优化程度越大
# 0 = 不优化. s = 2 + 优化代码大小,3 优化并不总是最好的.使用-O时表示-O1;
OPT = s
# 调试文件格式.
# 如果是用Winavr 的GDB调试的话选 dwarf-2 [默认] 或者stabs也行。
# 从AVR Studio 4.10 （我不知道是不是从这个版本开始，反正4.12是可以的）
# 开始AVRStudio已开始支持ELF/DWARF的调试文件,所以这时你可以选 dwarf-2.
# 编译你的工程然后开始调试，我测试过，而且在AVRStudio4.12的帮助文档中找到了
# AVRStudio目前支持的 Debugging File 的说明，你们也可以自己去看一下。
# 当然 AVR [Extended] COFF 格式还是支持，不过你要在编译时在这里选 stabs 选项
# 然后在用"make all" 编译后再用"make extcoff"调用avr-objcopy 生成 [Extended] COFF.
# 这时你会觉得有点烦，呵呵。那就选 dwarf-2选项吧，不用动它就可以了
DEBUG = dwarf-2
 
# 列出你的用＃i nclude 包含的.h头文件的目录.make 会在当前目录或标准库的目录找不到时
# 到这个目录在寻找 例: EXTRAINCDIRS = c:\m128\inc 如果是makefile当前目内的子目录inc
# 那么就直接写 EXTRAINCDIRS = inc .如果有多个目录，中间用空格隔开。都在一个目录就不用写
EXTRAINCDIRS =
 
# 编译标记，用来选标准C的级别,一般不用动
# c89 = "ANSI" C
# gnu89 = c89 plus GCC extensions
# c99 = ISO C99 standard (还没有完全完成)
# gnu99 = c99 plus GCC extensions
CSTANDARD = -std=gnu99
 
# 在C语言源文件前面加上 -D or -U 选项（具体什么意思我还没有弄明白）
CDEFS = -DF_CPU=$(F_CPU)UL
# 这个和上面的说明一样，不过是对于C++文件的
CPPDEFS = -DF_CPU=$(F_CPU)UL
#----------------------- C 编译选项-------------------------------
# -g*: 产生调试信息
# -O*: 代码优代级别
# -f...: 用于改变内建一些默认的函数,具体的看avr-libc手册
# -Wall..: 警告的级别
# -Wa,...: 告诉GCC 后面的选项不用管，把它传给汇编选项.
# -Wl,...: 和-Wa选项差不多，告诉GCC 传递后面的选项给liker链接器.
# -a[cdhlmns=file]: 创建汇编列表文件,默认的一般选 -adhlns
# 子选项如下:
# c 省略错误的条件
# d 省略调试的指令
# h 包含最高层的源文件
# l 包含汇编文件
# m 包含宏的展开
# n 省略处理的框架（forms processing）（我不知是什么意思）
# s 包含标号
# =file 设置生成的汇编列表文件的文件名,可以包含目录名用"/"隔开. 
#-------------------------集合所有C编译选项到变量 CFLAGS 中-------------------
#DUBUG选项
CFLAGS = -g$(DEBUG)
#CPU时钟频率
CFLAGS += $(CDEFS)
#优化级别
CFLAGS += -O$(OPT)
#把int 当成 8-bit integer,avr-libc并不真的支持，所以一般不要用.
#CFLAGS += -mint8
#在大于8K 单片机使用 rjmp/rcall (默认情况用jmp/call)
#CFLAGS += -mshort-calls
#这个选项会关掉自动内建的函数(但是可以在实际使用的函数前加上 __builtin_)
#当你使用这个参数时，void main() 不出现警告
#这个参数同时也会影响你的代码的优化，使代码增加。一般不使用就可以了。
#更多的说明请参考avr-libc说明。
#CFLAGS += -ffreestanding
#编译器总认为 char = unsigned char
CFLAGS += -funsigned-char
#当你定义一个位变量bit区时，总认为它是unsigned bitfields, 所以定义 aa:4 = unsigned aa:4
CFLAGS += -funsigned-bitfields
#定义结构体的时候，使里面的每一个元素的存储空间都是连续，一个接一个存，中间没有存储间隔
CFLAGS += -fpack-struct
#定义枚举类型的时候，使用里面存储空间最大的一个变量作为整个枚举变量的存储空间
CFLAGS += -fshort-enums
#在制作执行文件前分析整个编译单位。提供某些额外最佳化套用的机会，但会使用更多内存。
#这个参数我也不知是什么意思。不用它吧。
#CPPFLAGS += -fno-unit-at-a-time
#很多警告的集合，具体参见GCC 手册
CFLAGS += -Wall
#如果函数的声明或定义没有指出参数类型,编译器就发出警告
CFLAGS += -Wstrict-prototypes
#如果使用未定义宏，则进行警告
CFLAGS += -Wundef
#对程序中没有使用的代码作出警告
#CFLAGS += -Wunreachable-code
#在有符号数和无符号数进行值比较时,有符号数可能在比较之前被转换为无符号数而导致结果错误
#CFLAGS += -Wsign-compare
#创建汇编列表文件， $(<:%.c=$(OBJDIR)/%.lst)的意思是把"<"表示的目标的 #所有.c文件换成.lst 文件，作为输出lst文件的文件名 CFLAGS += -Wa,-adhlns=$(<:%.c=$(OBJDIR)/%.lst)
 
#这句的意思是在$(EXTRAINCDIRS)定义目录变量中，为每一个用空格隔开的目录前加上 -I
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
#标准C的级别
CFLAGS += $(CSTANDARD)
#--------------------------- C编译选项 CFLAGS 结束---------------------------
 
#----------------下面是 C++ 的编译选项(见C编译选顶，不重复说了) -------------
# -g*: generate debugging information
# -O*: optimization level
# -f...: tuning, see GCC manual and avr-libc documentation
# -Wall...: warning level
# -Wa,...: tell GCC to pass this to the assembler.
# -adhlns...: create assembler listing
CPPFLAGS = -g$(DEBUG)
CPPFLAGS += $(CPPDEFS)
CPPFLAGS += -O$(OPT)
#CPPFLAGS += -mint8
#CPPFLAGS += -mshort-calls
CPPFLAGS += -funsigned-char
CPPFLAGS += -funsigned-bitfields
CPPFLAGS += -fpack-struct
CPPFLAGS += -fshort-enums
CPPFLAGS += -fno-exceptions
#CPPFLAGS += -fno-unit-at-a-time
CPPFLAGS += -Wall
#CPPFLAGS += -Wstrict-prototypes
CPPFLAGS += -Wundef
#CPPFLAGS += -Wunreachable-code
#CPPFLAGS += -Wsign-compare
CPPFLAGS += -Wa,-adhlns=$(<:%.cpp=$(OBJDIR)/%.lst) CPPFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS)) #CPPFLAGS += $(CSTANDARD) #--------------------------- C++ 编译选项 CFLAGS 结束-------------------------
 
 
#---------------- Assembler Options ----------------
# -Wa,...: 告诉GCC 后面的选项不用管，把它传给汇编选项..
# -ahlms: 创建列表文件
# -gstabs: stabs debugging symbols for assembler source lines.
# This enables avr-gdb to trace through assembler source files
# 当源文件中由C编译产生的.s汇编文件时不要用这个选项，因为
# 其已包含了合适的行信息。
#ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2 #汇编文件由C产生时用 
ASFLAGS = -Wa,-adhlns=$(<:%.S=$(OBJDIR)/%.lst),-gstabs
 
 
#--------------------------------- 有关库的选项-------------------------------
# printf 函数最小的版本
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min
# 标准 printf 还加上 Floating point (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt
# 如果PRINTF_LIB不写为空,那么gcc-libc 就使用标准版的 printf 函数.
PRINTF_LIB =
#PRINTF_LIB = $(PRINTF_LIB_MIN)
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)
# 最小的 scanf 函数的版本
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min
# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt
# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB =
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)
# -l[lib-name] 库名去掉lib 和.a后缀 如 libm.a 则写成 -lm 如下所示：
MATH_LIB = -lm
 
#----------------------外部存储器选项 ---------------------------
# 关于--defsym 的说明
# --defsym symbol=expr #定义一个全局的标志,用expr作为symbol 的值.
# -Tbss org 
# -Tdata org
# -Ttext org 
# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# 64 KB的外部扩展的 RAM, 地址接在内部 RAM后面
# used for variables (.data/.bss) and heap (malloc()).
# 变量(.data/.bss）还有heap(malloc()),都使用这个区不用内部RAM
# 前面加上0x800000 表示是RAM数据区，而不是程序区,因为AVR 是哈弗结构
# EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff
# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# 外部RAM只用作 heap区，供malloc()分配的空间存储,变量(.data/.bss）仍在内部RAM.
#EXTMEMOPTS = -Wl,--defsym=__heap_start=0x801100,--defsym=__heap_end=0x80ffff
EXTMEMOPTS = -Wl,--defsym=__heap_start=0x801100,--defsym=__heap_end=0x807fff
#显示地注明堆栈最高地址,也就是栈顶地址，C语言堆栈是向下生长,一般定义在RAM的尾部
EXTMEMOPTS += -minit-stack=0x1100
 
#---------------- 链接选项 LDFLAGS----------------
# -Wl,...: 告诉GCC 传递后面的选项给liker链接器.
# -Map: 创建 map 文件
# --cref: 把对照信息加到 map 文件中
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)
#链接器使用linker_script.x脚本，avr-gcc自带脚本存在 WinAVR\avr\lib\ldscripts下
#LDFLAGS += -T linker_script.x
 
#---------------- 编程下载选项 (avrdude) ----------------
# 用到少，我就不翻译了,而且也很简单。
# Programming hardware: alf avr910 avrisp bascom bsd
# dt006 pavr picoweb pony-stk200 sp12 stk200 stk500
#
# Type: avrdude -c ?
# to get a full listing.
#
AVRDUDE_PROGRAMMER = stk500
# com1 = serial port. Use lpt1 to connect to parallel port.
AVRDUDE_PORT = com1 # programmer connected to serial device
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep
# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
# AVRDUDE_ERASE_COUNTER = -y
# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
# AVRDUDE_NO_VERIFY = -V
# Increase verbosity level. Please use this when submitting bug
# reports about avrdude. See <[url]http://savannah.nongnu.org/projects/avrdude>[/url]
# to submit bug reports.
# AVRDUDE_VERBOSE = -v -v
AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)
#---------------------调试选项--------------------
# For simulavr only - target MCU frequency.
# MCU 的频率 (only for simulavr)
DEBUG_MFREQ = $(F_CPU)
# Set the DEBUG_UI to either gdb or insight.
# DEBUG_UI = gdb
DEBUG_UI = insight
# Set the debugging back-end to either avarice, simulavr.
DEBUG_BACKEND = avarice
#DEBUG_BACKEND = simulavr
# GDB Init Filename.
GDBINIT_FILE = __avr_gdbinit
# When using avarice settings for the JTAG
JTAG_DEV = /dev/com1
# Debugging port used to communicate between GDB / avarice / simulavr.
DEBUG_PORT = 4242
# Debugging host used to communicate between GDB / avarice / simulavr, normally
# just set to localhost unless doing some sort of crazy debugging when
# avarice is running on a different computer.
DEBUG_HOST = localhost
 
#============================================================================
#---------------定义编译软件和命令的别名，便于移植--------------------
SHELL = sh
CC = avr-gcc
#----------------------复制和翻译目标文件成其他的格式，如: elf -> ihex
OBJCOPY = avr-objcopy
#----------------------avr-objdump 显示关于目标文件和汇编文件的一些信息
OBJDUMP = avr-objdump
SIZE = avr-size
#----------------------avr-nm 从目标文件中列出所有标号
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
#----------------------删除目标及目录内文件
REMOVEDIR = rm -rf
COPY = cp
WINSHELL = cmd
 
#-------------------定义一些用于编译时显示的信息-------------------
# English (英文)
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = -------- end --------
MSG_SIZE_BEFORE = Size before:
MSG_SIZE_AFTER = Size after:
MSG_COFF = Converting to AVR COFF:
MSG_EXTENDED_COFF = Converting to AVR Extended COFF:
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling C:
MSG_COMPILING_CPP = Compiling C++:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:
MSG_CREATING_LIBRARY = Creating library:
 
# 定义所有的目标文件.
OBJ = $(SRC:%.c=$(OBJDIR)/%.o) $(CPPSRC:%.cpp=$(OBJDIR)/%.o) $(ASRC:%.S=$(OBJDIR)/%.o) 
# 定义所有的 listing 文件.
LST = $(SRC:%.c=$(OBJDIR)/%.lst) $(CPPSRC:%.cpp=$(OBJDIR)/%.lst) $(ASRC:%.S=$(OBJDIR)/%.lst) 
# 编译选项，用来生成所有的依赖文件. -MF 用于指定写入的.d的文件名，-MMD 不包系统头文件名
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d
 
# 合并所有需要的flags 和可选项的 flags
# 并且把 MCU 型号加入到 flags 中.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_CPPFLAGS = -mmcu=$(MCU) -I. -x c++ $(CPPFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)
 
# 默认的目标,也就是Makefile中出现的第一个目标.
all: begin gccversion sizebefore build sizeafter end
# 把编译工程生成的 目标文件转换成.HEX 文件或是 .a库文件等等.默认不生成库文件
build: elf hex eep lss sym
#build: lib
elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym
LIBNAME=lib$(TARGET).a
lib: $(LIBNAME)
 
# Eye candy.
# AVR Studio 3.x 不会检查 make 退出的代码，但是却依赖下面的字符串
# echo 是用来显示的命令，和DOS中的命令不一样，不是一个显示开关。
begin:
@echo
@echo $(MSG_BEGIN)
end:
@echo $(MSG_END)
@echo
 
# 显示各个文件的大小
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) -A $(TARGET).elf
AVRMEM = avr-mem.sh $(TARGET).elf $(MCU)
sizebefore:
@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); \
$(AVRMEM) 2>/dev/null; echo; fi
sizeafter:
@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); \
$(AVRMEM) 2>/dev/null; echo; fi
# 显示avr-gcc的版本信息.
gccversion:
@$(CC) --version
 
#下载程序到目标MCU 
program: $(TARGET).hex $(TARGET).eep
$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)
 
# 生成 avr-gdb 配置/初始化文件，主要按下面来做:
# 定义复位信号，加载目标文件,连接目标，把断点设置在main()函数
gdb-config:
@$(REMOVE) $(GDBINIT_FILE)
@echo define reset >> $(GDBINIT_FILE)
@echo SIGNAL SIGHUP >> $(GDBINIT_FILE)
@echo end >> $(GDBINIT_FILE)
@echo file $(TARGET).elf >> $(GDBINIT_FILE)
@echo target remote $(DEBUG_HOST):$(DEBUG_PORT) >> $(GDBINIT_FILE)
ifeq ($(DEBUG_BACKEND),simulavr)
@echo load >> $(GDBINIT_FILE)
endif
@echo break main >> $(GDBINIT_FILE)
debug: gdb-config $(TARGET).elf
ifeq ($(DEBUG_BACKEND), avarice)
@echo Starting AVaRICE - Press enter when "waiting to connect" message displays.
@$(WINSHELL) /c start avarice --jtag $(JTAG_DEV) --erase --program --file \
$(TARGET).elf $(DEBUG_HOST):$(DEBUG_PORT)
@$(WINSHELL) /c pause
else
@$(WINSHELL) /c start simulavr --gdbserver --device $(MCU) --clock-freq \
$(DEBUG_MFREQ) --port $(DEBUG_PORT)
endif
@$(WINSHELL) /c start avr-$(DEBUG_UI) --command=$(GDBINIT_FILE)
 
# 把 ELF 文件转换成 COFF 文件,供 AVR Studio 或 VMLAB 调试与仿真.
COFFCONVERT = $(OBJCOPY) --debugging
COFFCONVERT += --change-section-address .data-0x800000
COFFCONVERT += --change-section-address .bss-0x800000
COFFCONVERT += --change-section-address .noinit-0x800000
COFFCONVERT += --change-section-address .eeprom-0x810000
 
coff: $(TARGET).elf
@echo
@echo $(MSG_COFF) $(TARGET).cof
$(COFFCONVERT) -O coff-avr $< $(TARGET).cof
 
extcoff: $(TARGET).elf
@echo
@echo $(MSG_EXTENDED_COFF) $(TARGET).cof
$(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof
 
 
# 通过输出的 ELF 文件创建最终的(.hex, .eep)文件.
%.hex: %.elf
@echo
@echo $(MSG_FLASH) $@
$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@ %.eep: %.elf @echo @echo $(MSG_EEPROM) $@ -$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \ --change-section-lma .eeprom=0 -O $(FORMAT) $< $@
 
# 通过 ELF 文件创建扩展的列表文件.
%.lss: %.elf
@echo
@echo $(MSG_EXTENDED_LISTING) $@
$(OBJDUMP) -h -S $< > $@
# 通过 ELF 文件创建列表文件
%.sym: %.elf
@echo
@echo $(MSG_SYMBOL_TABLE) $@
$(NM) -n $< > $@
# 通过.o目标文件创建.a 库文
.SECONDARY : $(TARGET).a
.PRECIOUS : $(OBJ)
%.a: $(OBJ)
@echo
@echo $(MSG_CREATING_LIBRARY) $@
$(AR) $@ $(OBJ)
 
# 链 接: 通过.o目标文件创建 ELF 文件
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)
%.elf: $(OBJ)
@echo
@echo $(MSG_LINKING) $@
$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)
 
# 编 译: 编译 C 源文件创建 .o 目标文件
$(OBJDIR)/%.o : %.c
@echo
@echo $(MSG_COMPILING) $< $(CC) -c $(ALL_CFLAGS) $< -o $@
 
 
# 编 译: 编译 C++ 源文件创建 .o 目标文件
$(OBJDIR)/%.o : %.cpp
@echo
@echo $(MSG_COMPILING_CPP) $< $(CC) -c $(ALL_CPPFLAGS) $< -o $@

# 编 译: 编译 C 源文件创建 .s 汇编文件
%.s : %.c
$(CC) -S $(ALL_CFLAGS) $< -o $@

# 编 译: 编译 C++ 源文件创建 .s 汇编文件
%.s : %.cpp
$(CC) -S $(ALL_CPPFLAGS) $< -o $@

# 汇 编: 编译 .S 汇编源文件创建 .o 目标文件(必须大写"S"作后缀)
$(OBJDIR)/%.o : %.S
@echo
@echo $(MSG_ASSEMBLING) $< $(CC) -c $(ALL_ASFLAGS) $< -o $@

# 只对源文件进行预处理，查看是源文件是否有错
%.i : %.c
$(CC) -E -mmcu=$(MCU) -I. $(CFLAGS) $< -o $@

# 目 标: 清除整个工程,便于下次编译
clean: begin clean_list end
clean_list :
@echo
@echo $(MSG_CLEANING)
$(REMOVE) $(TARGET).hex
$(REMOVE) $(TARGET).eep
$(REMOVE) $(TARGET).cof
$(REMOVE) $(TARGET).elf
$(REMOVE) $(TARGET).map
$(REMOVE) $(TARGET).sym
$(REMOVE) $(TARGET).lss
$(REMOVEDIR) $(OBJDIR)
$(REMOVE) $(SRC:.c=.s)
$(REMOVE) $(SRC:.c=.d)
$(REMOVEDIR) .dep

# 创建用于存目标文件的目录 $(OBJDIR)
$(shell mkdir $(OBJDIR) 2>/dev/null)
# 包含依赖文件
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
# 把所有的目标都定义成伪目标,伪目标的特性总是被执行
.PHONY : all begin finish end sizebefore sizeafter gccversion \
build elf hex eep lss sym coff extcoff \
clean clean_list program debug gdb-config
```


# 附 工具链

## AVR-GCC for linux 

如果是在linux环境，直接通过apt安装即可
```shell
#C库的手册在 /usr/share/doc/avr-libc/avr-libc-user-manual/index.html 。
$ apt-get install gcc-avr avr-libc avrdude
```


## AVR-GCC for Windows 32 and 64 bit 

如果在windows环境，安装arduino IDE后就安装了avr-gcc，但avr-libc需要自己去下载安装，因此建议使用下面的发行版：

[AVR-GCC 8.3.0 for Windows 32 and 64 bit](http://blog.zakkemble.net/avr-gcc-builds/)

该发行版中包含了Binutils, AVR-LibC, AVRDUDE, Make and GDB. 因此建议如果在windows环境下使用该工具链

### upgrading the Arduino IDE

Upgrading the Arduino IDE is pretty easy, though there could be some incompatibilities with certain libraries. I’ve only tested this with Arduino 1.8.2.

- Download and extract one of the downloads above
- Navigate to your Arduino IDE folder
- Go to hardware/tools
- Move the avr folder somewhere else, like to your desktop (renaming the folder won’t work, Arduino has some auto-detect thing which sometimes gets confused)
- Move the extracted folder from earlier to the tools folder and rename it to avr
- Copy the builtin_tools_versions.txt file and etc folder from the old avr folder to the new one
- Done! Open up the Arduino IDE, load up the Blink example, upload it to your Arduino and make sure the LED is blinking!
