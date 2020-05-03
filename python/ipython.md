# IPython提示与技巧

IPython用户手册包含关于使用IPython的大量信息，但是，为了帮你你更快的入门，这里快速介绍三个有用的功能：历史，魔法函数，别称和tab完成。
与Unix Shell相似，IPython支持命令历史。按上下键在之前输入的命令间切换：

```python
In [1]: x = 10
In [2]: <UP>
In [2]: x = 10
```

IPython通过在命令前加%字符的前缀，支持所谓魔法函数。例如，前面部分的函数run和whos都是魔法函数。请注意automagic设置默认是启用，允许你忽略前面的%。因此，你可以只输入魔法函数仍然是有效的。
其他有用的魔法函数：

- %cd 改变当前目录
- %timeit 允许你使用来自标准库中的timeit模块来记录执行短代码端的运行时间
- %cpaste 允许你粘贴代码，特别是来自网站的代码，前面带有标准的Python提示符 (即 >>>) 或ipython提示符的代码(即 in [3])：
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

- 内置的IPython手册可以通过%quickref魔法函数进入。
- 输入%magic会显示所有可用魔法函数的列表。
- 而且IPython提供了大量的别称来模拟常见的UNIX命令行工具比如ls等于list files，cp等于copy files以及rm等于remove files。输入alias可以显示所有的别称的列表：
- tab完成功能

