

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




## gcc
- 查看gnu工具链包含的工具：
```shell
atmel@atmel-virtual-machine:~$ dpkg -L  binutils | xargs ls -l | grep "/usr/bin"
lrwxrwxrwx    1 root root      26 11月  9 00:06 /usr/bin/addr2line -> x86_64-linux-gnu-addr2line
lrwxrwxrwx    1 root root      19 11月  9 00:06 /usr/bin/ar -> x86_64-linux-gnu-ar
lrwxrwxrwx    1 root root      19 11月  9 00:06 /usr/bin/as -> x86_64-linux-gnu-as
lrwxrwxrwx    1 root root      24 11月  9 00:06 /usr/bin/c++filt -> x86_64-linux-gnu-c++filt
lrwxrwxrwx    1 root root      20 11月  9 00:06 /usr/bin/dwp -> x86_64-linux-gnu-dwp
lrwxrwxrwx    1 root root      24 11月  9 00:06 /usr/bin/elfedit -> x86_64-linux-gnu-elfedit
lrwxrwxrwx    1 root root       7 11月  9 00:06 /usr/bin/gold -> ld.gold
lrwxrwxrwx    1 root root      22 11月  9 00:06 /usr/bin/gprof -> x86_64-linux-gnu-gprof
lrwxrwxrwx    1 root root      19 11月  9 00:06 /usr/bin/ld -> x86_64-linux-gnu-ld
lrwxrwxrwx    1 root root      23 11月  9 00:06 /usr/bin/ld.bfd -> x86_64-linux-gnu-ld.bfd
lrwxrwxrwx    1 root root      24 11月  9 00:06 /usr/bin/ld.gold -> x86_64-linux-gnu-ld.gold
lrwxrwxrwx    1 root root      19 11月  9 00:06 /usr/bin/nm -> x86_64-linux-gnu-nm
lrwxrwxrwx    1 root root      24 11月  9 00:06 /usr/bin/objcopy -> x86_64-linux-gnu-objcopy
lrwxrwxrwx    1 root root      24 11月  9 00:06 /usr/bin/objdump -> x86_64-linux-gnu-objdump
lrwxrwxrwx    1 root root      23 11月  9 00:06 /usr/bin/ranlib -> x86_64-linux-gnu-ranlib
lrwxrwxrwx    1 root root      24 11月  9 00:06 /usr/bin/readelf -> x86_64-linux-gnu-readelf
lrwxrwxrwx    1 root root      21 11月  9 00:06 /usr/bin/size -> x86_64-linux-gnu-size
lrwxrwxrwx    1 root root      24 11月  9 00:06 /usr/bin/strings -> x86_64-linux-gnu-strings
lrwxrwxrwx    1 root root      22 11月  9 00:06 /usr/bin/strip -> x86_64-linux-gnu-strip
-rwxr-xr-x    1 root root   27768 11月  9 00:06 /usr/bin/x86_64-linux-gnu-addr2line
-rwxr-xr-x    1 root root   60536 11月  9 00:06 /usr/bin/x86_64-linux-gnu-ar
-rwxr-xr-x    1 root root  369480 11月  9 00:06 /usr/bin/x86_64-linux-gnu-as
-rwxr-xr-x    1 root root   27248 11月  9 00:06 /usr/bin/x86_64-linux-gnu-c++filt
-rwxr-xr-x    1 root root 2724600 11月  9 00:06 /usr/bin/x86_64-linux-gnu-dwp
-rwxr-xr-x    1 root root   31552 11月  9 00:06 /usr/bin/x86_64-linux-gnu-elfedit
-rwxr-xr-x    1 root root   98616 11月  9 00:06 /usr/bin/x86_64-linux-gnu-gprof
lrwxrwxrwx    1 root root       6 11月  9 00:06 /usr/bin/x86_64-linux-gnu-ld -> ld.bfd
-rwxr-xr-x    1 root root 1220040 11月  9 00:06 /usr/bin/x86_64-linux-gnu-ld.bfd
-rwxr-xr-x    1 root root 2916864 11月  9 00:06 /usr/bin/x86_64-linux-gnu-ld.gold
-rwxr-xr-x    1 root root   40744 11月  9 00:06 /usr/bin/x86_64-linux-gnu-nm
-rwxr-xr-x    1 root root  224432 11月  9 00:06 /usr/bin/x86_64-linux-gnu-objcopy
-rwxr-xr-x    1 root root  356760 11月  9 00:06 /usr/bin/x86_64-linux-gnu-objdump
-rwxr-xr-x    1 root root   60536 11月  9 00:06 /usr/bin/x86_64-linux-gnu-ranlib
-rwxr-xr-x    1 root root  485184 11月  9 00:06 /usr/bin/x86_64-linux-gnu-readelf
-rwxr-xr-x    1 root root   31704 11月  9 00:06 /usr/bin/x86_64-linux-gnu-size
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



