

## adblock

- https://github.com/bluele/adblock

- [广告规则过滤区](https://bbs.kafan.cn/thread-1850279-1-1.html)

## antlr

```shell
curl -O http://www.antlr.org/download/antlr-4.0-complete.jar
export CLASSPATH="/usr/local/lib/antlr-4.0-complete.jar:$CLASSPATH"
alias antlr4='java -jar /usr/local/lib/antlr-4.0-complete.jar'
```
windows下的批命令antlr4.bat
```bat
CLASSPATH=.;%JAVA_HOME%\lib;%JAVA_HOME%\lib\tools.jar
java -cp c:\prog\antlr-4.0-complete.jar;%CLASSPATH% org.antlr.v4.Tool %*
```
grun批命令
```bat
java -cp c:\prog\antlr-4.0-complete.jar;%CLASSPATH% org.antlr.v4.runtime.misc.TestRig %*
```

- [source code](http://media.pragprog.com/titles/tpantlr2/code/tpantlr2-code.zip)




# gcc
- 查看gnu工具链包含的工具：
```shell
atmel@atmel-virtual-machine:~$ dpkg -L  binutils | xargs ls -l | grep "/usr/bin"
lrwxrwxrwx    1 root root      26 11月  9 00:06 /usr/bin/addr2line -> x86_64-linux-gnu-addr2line
lrwxrwxrwx    1 root root      19 11月  9 00:06 /usr/bin/ar -> x86_64-linux-gnu-ar
...
-rwxr-xr-x    1 root root   27728 11月  9 00:06 /usr/bin/x86_64-linux-gnu-strings
-rwxr-xr-x    1 root root  224440 11月  9 00:06 /usr/bin/x86_64-linux-gnu-strip
/usr/bin:
```

中间表示（IR intermediate representation）这个核心概念，重点介绍gcc的三种中间表示： 抽象语法树（AST abstract synatax tree）/ GIMPLE和寄存器传输语言（RTL ）

- 查看源码, "ctrl+]", "ctrl+O"
```shell
cd gcc-4.4.0
ctags -R
vim gcc/main.c
```
- cgdb对gdb的封装很有特点，方便查看源码



# gdb与gdbserver

## 编译host gdb
host是x86_64 pc linux。 编译过程如下：
```shell
~/ $  wget -c https://ftp.gnu.org/gnu/gdb/gdb-10.2.tar.xz
~/ $  xz -d gdb-10.2.tar.xz 
~/ $  tar xvf gdb-10.2.tar 
~/ $  cd gdb-10.2/
 # in x86_64 host do compile, so the ./configure default is: --host=x86_64-pc-linux-gnu 
~/gdb-10.2/ $  ./configure --target=arm-linux-gnueabi   --enable-sim  --prefix=/usr/local/arm-gdb -v
~/gdb-10.2/ $  make -j4
~/gdb-10.2/ $  ls
~/gdb-10.2/ $  sudo make install
# 确认安装成功了 
~/gdb-10.2/ $  ls /usr/local/arm-gdb/bin
   arm-linux-gnueabi-gdb  arm-linux-gnueabi-gdb-add-index  arm-linux-gnueabi-run
~/gdb-10.2/ $  /usr/local/arm-gdb/bin/arm-linux-gnueabi-gdb
   GNU gdb (GDB) 10.2
   ...
   For help, type "help".
   Type "apropos word" to search for commands related to "word".
   (gdb) 
```

## 编译target gdbserver（交叉编译）

提醒： 优先使用交叉编译器自带的gdbserver，如果没有的情况才自己编译它。例如raspberry的交叉编译器就自带了“arm-bcm2708-linux-gnueabi/arm-bcm2708-linux-gnueabi/debug-root/usr/bin/gdbserver”，例如海思也自带了“hisi-linux/x86-arm/aarch64-himix100-linux/host_bin/gdbserver”。

gdbserver一般会比较大，建议通过strip瘦身。

背景：在x86_64 pc linux 上交叉编译在arm target上的gdbserver。

在实践中发现gdbserver需要依赖gdb source下的gdbsupport，gnulib...等的编译，因为没有找到在我们背景下的官方编译方式，下面的编译方式可能不是简洁的编译方式，但可行。
```shell
# 前面编译了host gdb，为了环境简单，所以直接采用暴力归零
 ~/ $  rm -rf  gdb-10.2 
 ~/ $  tar xvf gdb-10.2.tar 
 ~/ $  cd gdb-10.2
# 通过下面的步骤，准备gdbserver需要的文件，类似".a","config.h" 
 ~/gdb-10.2/ $ ./configure --host=arm-linux-gnueabi --target=arm-linux-gnueabi CC=arm-linux-gnueabi-gcc CXX=arm-linux-gnueabi-g++
 ~/gdb-10.2/ $  make -j4
# 编译target gdbserver
 ~/gdb-10.2/ $ cd gdbserver/
 ~/gdb-10.2/gdbserver/ $ ./configure --host=arm-linux-gnueabi --target=arm-linux-gnueabi CC=arm-linux-gnueabi-gcc CXX=arm-linux-gnueabi-g++
 ~/gdb-10.2/gdbserver/ $ make -j4
        GEN    version-generated.cc
        CXX    version.o
        CXXLD  gdbreplay
        CXXLD  gdbserver
# 可以看到最终生成了gdbserver，这个拷贝到target（比如我们的arm单板）就可以使用了
 ~/gdb-10.2/gdbserver/ $ ls -F|grep "*"
        config.status*
        configure*
        gdbreplay*
        gdbserver*
```



## gdbserver的简单使用

gdbserver 需要放置在target机器上，建议放入"`$PATH`"环境变量，以便在任何地方都可以执行它

角色概念：host和target:
  - host是运行gdb的机器
  - target是运行gdbserver的机器

gdbserver提供一个网络服务，gdb remote到gdbserver上后进行调试

基本要求：
   - host和target可以网络通信，ping通
   - host上的gdb和target上的gdbserver版本尽量一致（？）

例子： 在pc上"`~/abc`"为我们例子的源码目录，在该目录中的"`~/abc/a.out`"是采用交叉编译的得到arm单板可执行含debug信息的文件，该文件已经通过类似ftp方式放到了arm单板的"/arm_target/a.out"位置上。

- 在arm单板上执行：``
```shell
# 7788端口是举例，可以是任意空闲端口
/arm_target/ $ gdbserver :7788 a.out
```
- 在host pc上执行：

```shell
# arm-linux-gnueabi-gdb是我们前面编译得到的
# 其中xx.xx.xx.xx是target的ip地址, 7788是先前在target上gdbserver对应的端口号, [args]表示参数列表
~/abc/ $ arm-linux-gnueabi-gdb a.out
  (gdb) target remote xx.xx.xx.xx:7788 [args]
#到此时，就可以在host pc上按照通常的方式使用类似断点等命令了  
```  

## vscode配合使用gdbserver

配合就是在host pc上对vscode配置launch.json。 在目标板上通过gdbserver拉起目标程序的步骤照旧。

主要修改项:

- name: 调试的项目名
- program: 应用程序路径，这个最好放在共享目录，和板子用同一个文件。例如"`"home/testuser/abc/a.out"`"
- cwd: 程序源代码路径，这个千万不要搞错了，要不调试的时候找不到源代码文件。例如"`"${workspaceFolder}"`"
- miDebuggerPath： 交叉编译工具中的gdb,例如"`"/usr/local/arm-gdb/arm-linux-gnueabi-gdb"`"
- miDebuggerServerAddress: 远程gdbserver服务，根据自己设备对应修改.例如"`"192.168.1.5:7788"`"


# gdb常见用法

[调试器GDB的基本使用方法](https://www.cnblogs.com/arnoldlu/p/9633254.html)

## stepping

step steps into subroutines, but next will step over subroutines.

https://sourceware.org/gdb/onlinedocs/gdb/Continuing-and-Stepping.html

### step into
   ```text
    (gdb) help step
    step, s
    Step program until it reaches a different source line.
    Usage: step [N]
    Argument N means step N times (or till program stops for another reason).
   ```
   sample "`for(i=0;i<10;i++) { printf("%d\n",i); }`" is one source line but multiple instructions.

### step over
   ```text
    (gdb) help next
    Step program, proceeding through subroutine calls.
    Usage: next [N]
    Unlike "step", if the current source line calls a subroutine,
    this command does not enter the subroutine, but instead steps over
    the call, in effect treating it as a single source line.
   ```
### step out
   `finish` sometimes does the same thing as 'step-out'. It'll finish what the stack is doing (a function, usually), and go to the next line after that. Look up the command for more info.
   
    1.如果想要立即中断当前函数运行并返回，用return，当前函数的剩余语句将不被执行。（return可带参数，可以此随意设定当前函数的返回值）

    2.如果想让程序执行到当前函数返回之后停止，用finish，当前函数的剩余语句将会正常运行

   ```text
    (gdb) help return
    Make selected stack frame return to its caller.
    Control remains in the debugger, but when you continue
    execution will resume in the frame above the one now selected.
    If an argument is given, it is an expression for the value to return.
    (gdb) help finish
    finish, fin
    Execute until selected stack frame returns.
    Usage: finish
    Upon return, the value returned is printed and put in the value history.
   ```


## watchpoints

### 设置观察点
- watch :	使你能监视一个变量的值而不管它何时被改变
- rwatch :	指定一个变量，如果这个变量被读，则暂停程序运行，在调试器中显示信息，并等待下一个调试命令。参考rwatch和watch命令
- awatch :	指定一个变量，如果这个变量被读或者被写，则暂停程序运行，在调试器中显示信息，并等待下一个调试命令。参考rwatch和watch命令

例如："`(gdb) awatch init_task`"

### 删除/disable观察点

指令：“`delete <断点号>`”，“`disable <断点号>`”。disable了的，可以通过enable指令重新使能。

例如：
```text
(gdb) info watchpoints 
Num     Type           Disp Enb Address    What
1       acc watchpoint keep y              init_task
        breakpoint already hit 3 times
(gdb) info breakpoints 
Num     Type           Disp Enb Address    What
1       acc watchpoint keep y              init_task
        breakpoint already hit 3 times
(gdb) delete 1
(gdb) info watchpoints 
No watchpoints.
(gdb) 
```
从该例子也可以看出watchpoint是一种特殊的breakpoint

## breakpoints

### 设置断点
- `break [file:]function`	在(file文件的)function函数中设置一个断点
- `rbreak [file:]function` : Set a breakpoint for all functions matching REGEXP. 正则表达式和grep中使用的一样

   - 比如设置断点到一个文件中的所有函数上："`(gdb) rbreak file.c:. `"
   - `(gdb) break 函数名`
   - `(gdb) break 行号`
   - `(gdb) break 文件名:函数名`
   - `(gdb) break 文件名:行号`
   - `(gdb) break +偏移量`
   - `(gdb) break -偏移量`
   - `(gdb) break *地址`
   - `break 断点 if 条件；比如break sum if value==9，当输入的value为9的时候才会断住`
   - `condition 断点编号 条件：给指定断点添加触发条件`
   - `condition 断点编号：给指定断点删除触发条件`


### 删除/disable断点

参见"删除/disable观察点"


## 使用GDB查看core文件

默认编译出来的程序在出现Segmentation fault 时并没有生成core崩溃文件，可以在gcc/g++编译时增加-g选项, 比如在cmakefile.txt中设置"`set(CMAKE_BUILD_TYPE Debug)  #vs Release`"。

如果仍然没有生成core文件，则可能是因为系统设置了core文件大小为0，可以通过："`ulimit -a`" 查询得知。

执行 "`ulimit -c unlimited`" 命令后可以使core文件大小不受限制。此时再次运行程序应该就能在同级目录看到core.XXX文件了

使用 "`gdb ./a.out core.XXX`" 可以查看出错所在行信息，这样就进入了 gdb core 调试模式。

通过"`gdb>bt`"追踪产生segmenttation fault的位置及代码函数调用情况。
  
  
## 使用GDB调试kernel

前提：对kernel的make menuconfig，应额外增加以下配置:

```text
kernel hacking --->
   compile-time checks and compiler options --->
     [*] compile the kernel with debug info
```

首先携带额外的"-S -s"参数启动虚拟机
```shell
# -S ： 表示qemu虚拟机会冻结cpu，直到远程的gdb输入相应控制命令
# -s ： 表示在1234端口接受gdb的调试连接
$ qemu-system-arm -M vexpress-a9 -m 512M -kernel arch/arm/boot/zImage -dtb arch/arm/boot/dts/vexpress-v2p-ca9.dtb -nographic -append "root=/dev/mmcblk0 rw console=ttyAMA0 loglevel=8" -sd ../busybox/a9rootfs.ext3 -S -s
```

然后在另外一个终端启动arm gdb
```shell
~/$ cd kernel
~/kernel $ arm-linux-gnueabi-gdb --tui vmlinux

(gdb) target remote localhost:1234
  Reading symbols from vmlinux...
(gdb) b start_kernel
  Breakpoint 1 at 0x80b00960: file init/main.c, line 931.
(gdb) c
  Continuing.

Breakpoint 1, start_kernel () at init/main.c:931
(gdb) 

```

关闭：在另外的console(即非启动虚拟机的console)中执行"`$ killall qemu-system-arm`"
