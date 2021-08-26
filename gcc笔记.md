

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



