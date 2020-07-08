
### 目录


- [IPython提示与技巧](#ipython提示与技巧)
  - [1.魔法函数](#1魔法函数)
    - [1.1.改变工作目录](#11改变工作目录)
    - [1.2.记录执行时间](#12记录执行时间)
    - [1.3.方便粘贴代码](#13方便粘贴代码)
    - [1.4.进入事后除错](#14进入事后除错)
    - [1.5.进入内置ipython手册](#15进入内置ipython手册)
    - [执行py文件](#执行py文件)
    - [ipython 历史命令](#ipython-历史命令)
  - [其他](#其他)
    - [ipython命令行输出重定向到文件](#ipython命令行输出重定向到文件)
    - [ipython启动时执行](#ipython启动时执行)


# IPython提示与技巧

[官方介绍](https://ipython.org/ipython-doc/dev/interactive/tutorial.html)

IPython用户手册包含关于使用IPython的大量信息，但是，为了帮你你更快的入门，这里快速介绍三个有用的功能：历史，魔法函数，别称和tab完成。
与Unix Shell相似，IPython支持命令历史。按上下键在之前输入的命令间切换：

```python
In [1]: x = 10
In [2]: <UP>
In [2]: x = 10
```

## 1.魔法函数
IPython通过在命令前加%字符的前缀，支持所谓魔法函数。例如，前面部分的函数run和whos都是魔法函数。请注意automagic设置默认是启用，允许你忽略前面的%。因此，你可以只输入魔法函数仍然是有效的。
其他有用的魔法函数：


! calls out to a shell (in a new process), while % affects the process associated with the notebook (or the notebook itself; many % commands have no shell counterpart).
!cd foo, by itself, has no lasting effect, since the process with the changed directory immediayely terminates. %cd foo changes the current directory of the notebook process, which is a lasting effect.

### 1.1.改变工作目录

- %cd 改变当前目录


### 1.2.记录执行时间  
- %timeit 允许你使用来自标准库中的timeit模块来记录执行短代码端的运行时间

### 1.3.方便粘贴代码
- %cpaste 允许你粘贴代码，特别是来自网站的代码，前面带有标准的Python提示符 (即 >>>) 或ipython提示符的代码(即 in [3])：

### 1.4.进入事后除错

- %debug 允许你进入事后除错。也就是说，如果你想要运行的代码抛出了一个异常，使用%debug将在抛出异常的位置进入排错程序。

```python
In [7]: x === 10
File "<ipython-input-6-12fd421b5f28>", line 1
x === 10 ^
  SyntaxError: invalid syntax
In [8]: debug
> /home/esc/anaconda/lib/python2.7/site-packages/IPython/core/compilerop.py(87)ast_parse()
       86         and are passed to the built-in compile function."""
 ---> 87         return compile(source, filename, symbol, self.flags | PyCF_ONLY_AST, 1)
88
 ipdb>locals()
 {’source’: u’x === 10\n’, ’symbol’: ’exec’, ’self’:
 <IPython.core.compilerop.CachingCompiler instance at 0x2ad8ef0>,
 ’filename’: ’<ipython-input-6-12fd421b5f28>’}
```

### 1.5.进入内置ipython手册

- 内置的IPython手册可以通过%quickref魔法函数进入。

```python
%quickref

IPython -- An enhanced Interactive Python - Quick Reference Card
================================================================

obj?, obj??      : Get help, or more help for object (also works as
                   ?obj, ??obj).
?foo.*abc*       : List names in 'foo' containing 'abc' in them.
%magic           : Information about IPython's 'magic' % functions.

Magic functions are prefixed by % or %%, and typically take their arguments
without parentheses, quotes or even commas for convenience.  Line magics take a
single % and cell magics are prefixed with two %%.

Example magic function calls:

%alias d ls -F   : 'd' is now an alias for 'ls -F'
alias d ls -F    : Works if 'alias' not a python name
...
%%timeit:
    Time execution of a Python statement or expression
%%writefile:
    ::
```

- 输入%magic会显示所有可用魔法函数的列表。
- 而且IPython提供了大量的别称来模拟常见的UNIX命令行工具比如ls等于list files，cp等于copy files以及rm等于remove files。输入alias可以显示所有的别称的列表：
- tab完成功能

### 执行py文件

```python
%run  xx.py
```

### ipython 历史命令

在ipython session中可以通过上下键查找历史命令，可以通过`%history`查看

在`~/.ipython/profile_default/history.sqlite`里面也保存着

## 其他

### ipython命令行输出重定向到文件

编写下面的函数，

```python
import sys
@contextmanager
class redirect_output(object):
    """context manager for reditrecting stdout/err to files"""
    def __init__(self, stdout='', stderr=''):
            self.stdout = stdout
            self.stderr = stderr
    def __enter__(self):
            self.sys_stdout = sys.stdout
            self.sys_stderr = sys.stderr

            if self.stdout:
                sys.stdout = open(self.stdout, 'w',encoding='utf-8')
            if self.stderr:
                if self.stderr == self.stdout:
                    sys.stderr = sys.stdout
                else:
                    sys.stderr = open(self.stderr, 'w',encoding='utf-8')
    def __exit__(self, exc_type, exc_value, traceback):
            sys.stdout = self.sys_stdout
            sys.stderr = self.sys_stderr

```

需要使用时，使用下面的方法

```python
with redirect_output("my_output.txt"):
    %run my_script.py
```

### ipython启动时执行

Startup Files
If you want some code to be run at the beginning of every IPython session, the easiest way is to add Python (.py) or IPython (.ipy) scripts to your profile_default/startup/ directory. Files here will be executed as soon as the IPython shell is constructed, before any other code or scripts you have specified. The files will be run in order of their names, so you can control the ordering with prefixes, like 10-myimports.py.

该目录位于：`~/.ipython/profile_default`
