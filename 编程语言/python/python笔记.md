
# 1.安装最新python3

安装python分为linux平台下安装，与windows环境下安装。对生产环境通常需要更精简，平台版本精确，不要包含多余的东西。对开发环境通常要求便捷，少出现缺少模块等浪费时间的事情发生。因此一般采用python生产环境部署在linux；开发环境安排在windows上，较多选择anaconda。

## 1.1-linux

通过最新python3源码为例安装python3

### 1.1.2-源码python安装预准备

- python3编译需要高版本openssl lib，先安装最新openssl。安装方式就是上[openssl官网](https://www.openssl.org/source/)下载源码，按照默认配置，执行“`./configure &&make&& sudo make install` ”就可以。

    如果python中没有合适openssl参与构建，将导致类似pip安装模块时出现“`Caused by SSLError("Can't connect to HTTPS URL because the SSL module is not available)`”问题.

    ```sh
    wget https://www.openssl.org/source/openssl-1.1.1g.tar.gz
    $wget https://www.openssl.org/source/openssl-1.1.1g.tar.gz
    $tar xzvf openssl-1.1.1g.tar.gz 
    $cd openssl-1.1.1g/
    $./config
    $make&& sudo make install
    
    :$openssl version
    openssl: relocation error: openssl: symbol EVP_mdc2 version OPENSSL_1_1_0 not defined in file libcrypto.so.1.1 with link time reference
    ```

- python3需要libffi支持ctyps，需要安装libffi，如果缺少它，python安装后将会缺乏ctyps模块导致很多功能不能使用

    ```sh
    #确保安装python前系统中有libffi-dev，不然后面python运行会缺少ctypes module。如果已经有了，下面指令不用执行
    $sudo apt-get install libffi-dev
    ```
- python3需要“libsqlite3”支持，否则按照python运行一些模块会出现类似“`ModuleNotFoundError: No module named '_sqlite3'`”问题

    ```shell
    ##确保安装python前系统中有libsqlite3-dev，如果已经有了，下面指令不用执行
    $sudo apt-get install libsqlite3-dev
    ```

### 1.1.3-安装python3

下面开始安装python3，先从官网下载自己需要的源码，比如下面是下载最新版本源码。然后按照通常操作“`./configure && make && make install`”就可以。

按照个人习惯，对这个源码我通常都保留着，因为有可能在python运行时发现python缺少哪个模块，需要先安装该库，例如"缺少ctypes支持"，要回头重新编译python。

```sh
#下载最新python
$wget https://www.python.org/ftp/python/3.9.0/Python-3.9.0b1.tar.xz
#解压包
$tar xvf Python-3.9.0b1.tar.xz
$cd Python-3.9.0b1
#编译安装
$./configure --prefix=/usr/local --enable-shared LDFLAGS="-Wl,-rpath /usr/local/lib"
$make 
$sudo make install

$python3 -V
Python 3.9.0b1
```

### 1.1.4-升级pip

采用下面的指令升级pip

```shell
#升级默认提供的pip3为最新版本
~$sudo python3 -m pip install --upgrade --force-reinstall pip  --trusted-host pypi.douban.com

~$pip3 -V
pip 20.1.1 from /usr/local/lib/python3.9/site-packages/pip (python 3.9)
```

### 1.1.5-python安装模块举例

#### 1.1.5.1-源码安装python中模块举例

##### Twisted 安装

以python中的Twisted为例

先上[pypi官网](https://pypi.org/project/Twisted/)下载Twisted源码，然后通过`python setup.py install`指令进行安装

    ```sh
    $wget https://files.pythonhosted.org/packages/4a/b4/4973c7ccb5be2ec0abc779b7d5f9d5f24b17b0349e23240cfc9dc3bd83cc/Twisted-20.3.0.tar.bz2 --no-check-certificate
    $tar -xjvf Twisted-20.3.0.tar.bz2 
    $cd Twisted-20.3.0/
    $sudo python3 setup.py  install
    ```

#### 1.1.5.2-安装scipy

pip 安装 scipy 时，因为是编译安装，所以如果缺少一些编译库，会报很多错误，以下总结可能缺失的安装包：

```sh
$pip3 install Cython
$sudo apt-get install gfortran libopenblas-dev liblapack-dev libatlas-base-dev libblas-dev
#最后执行以下命令即可
$pip3 install scipy
```

#### 1.1.5.3-安装scrapy

```shell
#pip安装scrapy
$pip3 install scrapy==2.1.0

#查看默认安装问题
atmel:~$which scrapy
/home/atmel/.local/bin/scrapy

#执行`scrapy shell`
atmel:~$scrapy shell
2020-05-30 17:16:44 [scrapy.utils.log] INFO: Scrapy 2.1.0 started (bot: scrapybot)

2020-05-30 17:25:18 [scrapy.extensions.telnet] INFO: Telnet console listening on 127.0.0.1:6023
[s] Available Scrapy objects:
[s]   scrapy     scrapy module (contains scrapy.Request, scrapy.Selector, etc)
[s]   crawler    <scrapy.crawler.Crawler object at 0x7f8edf312040>
[s]   item       {}
[s]   settings   <scrapy.settings.Settings object at 0x7f8edf30ef70>
[s] Useful shortcuts:
[s]   fetch(url[, redirect=True]) Fetch URL and update local objects (by default, redirects are followed)
[s]   fetch(req)                  Fetch a scrapy.Request and update local objects 
[s]   shelp()           Shell help (print this help)
[s]   view(response)    View response in a browser
>>> 
```

注意，安装scrapy，同时请安装ipython，通过`pip3 install ipython`安装，因为如果没有ipython，“scrapy shell”使用起来挺不方便的。并且ipython这个python终端的能力比默认终端能力强大的多，平时使用python终端时，建议都使用ipython。


### 1.1.6-pip配置

#### 1.1.6.1-设置镜像源

经过上面安装完成以后，配置下python镜像，以便今后速度快些，如下面所示，为当前用户增加的对应pip配置文件，通常来说，配置一个就够了，下面的例子为了举例，所以配置了3个。

这个pip配置文件，linux是在"~/.pip/pip.conf"; 如果是在windows下，应该是`%APPDATA%/pip/pip.ini`文件。

    ```shell
    ~$cat ~/.pip/pip.conf 
    atmel:~$cat .pip/pip.conf 
    [global]

    index-url=https://mirrors.aliyun.com/pypi/simple/
    extra-index-url=
        https://pypi.tuna.tsinghua.edu.cn/simple/
        http://pypi.douban.com/simple/

    [install]

    trusted-host=
        mirrors.aliyun.com
        pypi.tuna.tsinghua.edu.cn
        pypi.douban.com    
    ```

国内常用的几个镜像完整列表是：

    ```text
    http://pypi.douban.com/simple/ 豆瓣 
    https://mirrors.aliyun.com/pypi/simple/ 阿里 
    http://pypi.hustunique.com/simple/ 华中 
    http://pypi.sdutlinux.org/simple/ 山东理工 
    http://pypi.mirrors.ustc.edu.cn/simple/ 科大

    https://pypi.tuna.tsinghua.edu.cn/simple/  清华
    ```

#### 1.1.6.2-pip查询库版本号

使用pip安装package需要指定版本号，有个变通的方法，如下演示的，给一个不合法的版本号。让pip返回合法的版本号

```sh
~$pip install scrapy==?
Defaulting to user installation because normal site-packages is not writeable
Looking in indexes: http://pypi.douban.com/simple
ERROR: Could not find a version that satisfies the requirement scrapy==? (from versions: 0.7, 0.8, 0.9, 0.10.4.2364, 0.12.0.2550, 0.14.1, 0.14.2, 0.14.3, 0.14.4, 0.16.0, 0.16.1, 0.16.2, 0.16.3, 0.16.4, 0.16.5, 0.18.0, 0.18.1, 0.18.2, 0.18.3, 0.18.4, 0.20.0, 0.20.1, 0.20.2, 0.22.0, 0.22.1, 0.22.2, 0.24.0, 0.24.1, 0.24.2, 0.24.3, 0.24.4, 0.24.5, 0.24.6, 1.0.0rc1, 1.0.0rc2, 1.0.0rc3, 1.0.0, 1.0.1, 1.0.2, 1.0.3, 1.0.4, 1.0.5, 1.0.6, 1.0.7, 1.1.0rc1, 1.1.0rc2, 1.1.0rc3, 1.1.0rc4, 1.1.0, 1.1.1, 1.1.2, 1.1.3, 1.1.4, 1.2.0, 1.2.1, 1.2.2, 1.2.3, 1.3.0, 1.3.1, 1.3.2, 1.3.3, 1.4.0, 1.5.0, 1.5.1, 1.5.2, 1.6.0, 1.7.0, 1.7.1, 1.7.2, 1.7.3, 1.7.4, 1.8.0, 2.0.0, 2.0.1, 2.1.0)
ERROR: No matching distribution found for scrapy==?
```


### 1.1.7-python2迁移到python3注意事项

注意事项请参考官网迁移说明[python3porting](http://python3porting.com/preparing.html#use-instead-of-when-dividing-integers)。 简要说，包含以下几点：

#### 整数除法
Python3改变了"除"运算符行为。细节请看网站.

    python3整除采用`//`, `/`含义是普通除法。 在c中，是通过`floor,ceil`两个函数实现向下，向上的整除能力。

    ```shell
    atmel:~$python2
    Python 2.7.17 (default, Nov  7 2019, 10:07:09) 
    >>> 5/2
    2
    >>> 
    >>> from __future__ import division
    >>> 5/2
    2.5
    >>> 

    atmel:~$python3
    Python 3.9.0b1 (default, May 30 2020, 17:24:23) 
    >>> 5/2
    2.5
    >>> 2
    >>> 5//2
    2
    >>> 
    ```

#### 二进制数据与字符串

在Python 3中，将bytes object用于二进制数据，并将str object用于各种文本数据（不论是否是Unicode），

在Python 2中，使用str object保存二进制数据和ASCII文本，而比ASCII中可用字符更多的字符的文本数据保存在 unicode object中。

### 1.1.8-常用运维指令

注：Python 3，它默认安装了pyvenv

- 环境切换

```sh
#创建一个存放虚拟环境的目录：
$mkdir python-env
$cd python-env
#创建一个虚拟环境：
$pyvenv env_A
#上面的命令将会导致生成"python-env/env_A"目录，里面有bin，include，lib...等目录，以及pyvenv.cfg...等文件

#使用创建的虚拟环境env_A：
$source env_A/bin/activate
```

注1：当如果需要指定虚拟环境的python版本，那运行venv时就需要使用指定版本的python，例如采用 `python3.6 -m venv neural`





## 1.2-windows

在windows环境，以anaconda集成环境为例说明。这个安装比较简单，从[anaconda官网](https://anaconda.org/)下载按照包安装提示进行安装就可以了，下面对常用运维指令进行下说明

### 1.2.1-常用运维指令

```sh
#安装镜像源
conda config –add channels https://mirrors.ustc.edu.cn/anaconda/pkgs/free/
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/pkgs/free/
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/pkgs/main/
conda config --set show_channel_urls yes

#检查conda版本
conda --version
#升级conda
conda update conda
#创建新环境
conda create -n myenv

#创建新环境，并指定python版本
conda create -n py36 python=3.6 anaconda

#移除环境
conda remove -n myenv --all
#列出有哪些env
conda env list
#激活某个环境
conda activate myenv
#返回原来环境
deactivate
#列出所有环境
conda info --envs
#给新环境安装Python3，并命名为python3：
conda create -n python3 python=3
#检查环境内的Python版本：
python --version
#查找包
conda search beautifulsoup4
```

## 1.3-vscode编程ide

如果采用vscode作为scrapy编程开发环境，以下几点需要注意：

  - 通过`scrapy startproject ...`生成项目后，应进入`scrapy.cfg`文件所在目录，然后再`code .`方式来打开你的爬虫项目，这样确保vscode的工作目录与scrapy.cfg所在目录一致，如果不保持一致，在调试时会有问题。
  - 对项目的".vscode/launch.json"中应配置“`"configurations": ["justMyCode": false,]`”,否则将导致调试时无法跳转到scrapy源码

如果采用vsocde作为python编程环境，以下几个操作常用：

  - 在vscodeCommand Palette，通过ctrl+shift+p拉起，执行“Python: Select Interpreter”。通过该命令在多个python环境中选择自己希望的python环境。这个操作的结果是会设置“settings.json”配置中“python.pythonPath ”的值

  - 在vscode setting中，搜索“python>auto complete: add brackets”,选择enable，这样对函数类会自动加上括号

  - typeshed 是 Python 的 library stub 的集合，用于为Python标准库和Python内建，以及第三方软件包。此数据可用于静态分析，类型检查和类型推断。在 Python>Auto Complete:Typeshed Paths选项中添加Python安装路径中 site-packages 和 Scrips 所在文件夹的路径（例如“C:\prog\Anaconda3\Lib\site-packages”与“C:\prog\Anaconda3\Scripts”）

  - 如果第三方python库的自动提示有问题，可以尝试enable jedi，不使用微软的python server。enable jedi方法是在vscode setting中搜索jedi enabled，并选中。

由于vscode提供了remote ssh开发环境，因此python解释编译在linux是挺方便的事。推荐大家采用vscode作为开发环境。



# 2.python3基础知识

## 2.0-调试debug

- 适用vscode集成环境：在调试配置中，点击齿轮构建python launch.json， 然后选择边栏中的调试视图。注意在launch.json中添加`"justMyCode": false`, 以便能step into 系统库等。

- 使用PDB。
    ```python
    (deveco-venv) atmel@atmel-vm:~/.deveco-device-tool/core/deveco-venv/bin$ python -m pdb hb
    > /home/atmel/.deveco-device-tool/core/deveco-venv/bin/hb(3)<module>()
    -> import re
    (Pdb) n
    > /home/atmel/.deveco-device-tool/core/deveco-venv/bin/hb(4)<module>()
    -> import sys
    (Pdb) n
    > /home/atmel/.deveco-device-tool/core/deveco-venv/bin/hb(5)<module>()
    -> from hb.__main__ import main
    (Pdb) n
    > /home/atmel/.deveco-device-tool/core/deveco-venv/bin/hb(6)<module>()
    -> if __name__ == '__main__':
    (Pdb) n
    > /home/atmel/.deveco-device-tool/core/deveco-venv/bin/hb(7)<module>()
    -> sys.argv[0] = re.sub(r'(-script\.pyw|\.exe)?$', '', sys.argv[0])
    (Pdb) n
    > /home/atmel/.deveco-device-tool/core/deveco-venv/bin/hb(8)<module>()
    -> sys.exit(main())
    (Pdb) b main
    Breakpoint 1 at /home/atmel/.deveco-device-tool/core/deveco-venv/lib/python3.8/site-packages/hb/__main__.py:34
    (Pdb) c
    > /home/atmel/.deveco-device-tool/core/deveco-venv/lib/python3.8/site-packages/hb/__main__.py(35)main()
    -> parser = argparse.ArgumentParser(usage="hb",
    (Pdb) 

    ```


## 2.1-基础数据结构

涉及到使用`len`取长度的，需要记住它不支持`None`, 因此典型使用常常是 `if o and len(o) >= xx `

### 2.1.1-取切片

sytax:  `L[start:end:step] # start through not past stop, by step`

或   ： `L[slice(start, stop, step)]`

工厂函数： slice

其中start、end如果是负值代表从结尾处开始算

```python
a[start:stop]  # items start through stop-1
a[start:]      # items start through the rest of the array
a[:stop]       # items from the beginning through stop-1
a[:]           # a copy of the whole array

a[-1]    # last item in the array
a[-2:]   # last two items in the array
a[:-2]   # everything except the last two items
```

其中step也可以是负值,这个不好理解，最好少用

```python
a[::-1]    # all items in the array, reversed
a[1::-1]   # the first two items, reversed
a[:-3:-1]  # the last two items, reversed
a[-3::-1]  # everything except the last two items, reversed
```

切片最好采用命名方式

```python
#         01234567890123456789 
record = '100 .......513.25 ..........'
SHARES = slice(0, 3)
PRICE = slice(11, 17)
cost = int(record[SHARES]) * float(record[PRICE])
```

###  2.1.3-列表

    `list:  []`，工厂函数list

    支持切片

    有`append,count,insert,reverse,clear,extend,pop,sort,copy,index,remove`方法

    ```python
    >>> a=["a","b","c"]
    >>> type(a)
    <class 'list'>
    >>> a.append("d")
    >>> a, a[0], a[-1]
    (['a', 'b', 'c', 'd'], 'a', 'd')
    >>> x=a, a[0], a[-1]
    >>> type(x),x
    (<class 'tuple'>, (['a', 'b', 'c', 'd'], 'a', 'd'))
    ```

### 2.1.2-元组

    tuple: (,)  是不可变的 。工厂函数tuple

    支持切片

    有`count,index`方法

    list/tuple slice: `L[start:end:step]`    
    
    - `L[-2:]`代表变量的倒数第二个元素以及往后的所有元素， 记住倒数第一个元素的索引是-1；
    - `L[:10:2]`代表前10个元素每2个取1个；
    - `L[::5]`代表所有数，每5个取一个：

### 2.1.4-集合

    set: {}

### 2.1.5-字典

    dict :{x:y}  工厂函数dict

    ```shell
    #dict工厂函数使用举例
    In [130]: dict([('Name', 'Tom'), ('Age', 19), ('Sex', 'male')])
    Out[130]: {'Name': 'Tom', 'Age': 19, 'Sex': 'male'}
    ```


- ython的变量标识符没有类型。
- python list的index 从0开始，倒数第一个的index是-1。 list 增加元素采用append


- isinstance () 它允许检查某个特定标识符是否包含某个特定类型的数据

### range

语法是`range(start, stop[, step]) -> range object`

例如:

```python
In [60]: list(range(1,4))
Out[60]: [1, 2, 3]
```

- range(4)生成0，1，2，3这四个数字

### 2.1.6-str与bytes

字符串与字节流是完全不同的两种数据，这点需要注意

```python
# bytes object
b = b"example"
# str object
s = "example"

# str to bytes
sb = bytes(s, encoding = "utf8")  #或sb = str.encode(s)
# bytes to str
bs = str(b, encoding = "utf8")    #或bs = bytes.decode(b)
```


### for控制形式

for语句有两种形式，其中一种是for-else形式。else形式对判断是否完整走完迭代对象比较方便

```python

for target_list in expression_list:
    pass

#当迭代对象完成所有迭代后且此时的迭代对象为空时，如果存在else子句则执行else子句，没有则继续执行后续代码；
#如果迭代对象因为某种原因（如带有break关键字）提前退出迭代，#则else子句不会被执行，
#程序将会直接跳过else子句继续执行后续代码
for target_list in expression_list:
    pass
else:
    pass

```

#### 带条件for

例子1：

```python
#随机获取指定数量
sample_keys = random.sample(list(all_sets), maxnum)
#
samples = {k: v for k, v in all_sets.items()  if k in sample_keys}
```

### 维护python库的搜索路径

```python
>>> import sys; sys.path
['', 'C:\\prog\\Anaconda3\\envs\\myenv\\python38.zip', 'C:\\prog\\Anaconda3\\envs\\myenv\\DLLs', 'C:\\prog\\Anaconda3\\envs\\myenv\\lib', 'C:\\prog\\Anaconda3\\envs\\myenv', 'C:\\prog\\Anaconda3\\envs\\myenv\\lib\\site-packages', 'C:\\prog\\Anaconda3\\envs\\myenv\\lib\\site-packages\\win32', 'C:\\prog\\Anaconda3\\envs\\myenv\\lib\\site-packages\\win32\\lib', 'C:\\prog\\Anaconda3\\envs\\myenv\\lib\\site-packages\\Pythonwin']

#类似下面的代码就增加了搜索路径，这种方式是运行时修改，退出运行后就失效了。长期修改那就更改设置环境变量PYTHONPATH
>>> sys.path.append('c:\\Users\\xx\\scripts')
```


### 列表生成式ListComprehensions
    
    典型语法：    `[x * x for x in range(1, 11)]`就生成了`[1x1, 2x2, 3x3, ..., 10x10]`

    例1

    ```python
    >>> [m + n for m in 'ABC' for n in 'XYZ']
    ['AX', 'AY', 'AZ', 'BX', 'BY', 'BZ', 'CX', 'CY', 'CZ']
    ```

    例2：演示通过列表生成式来写一个排序的函数

    ```python
    import sys
    if sys.version_info < (2, 4):
       def sorted(data, key):
           mapping = {}
           for x in data:
               mapping[key(x)] = x
           keys = mapping.keys()
           keys.sort()
           return [mapping[x] for x in keys]
    data = ['ant', 'Aardvark', 'banana', 'Dingo']
    
    sorted(data, key=str.lower)
    >>>['Aardvark', 'ant', 'banana', 'Dingo']
    ```

### 生成器generator

    创建L和g的区别仅在于最外层的[]和()，L是一个list，而g是一个generator。通常访问生成器都是通过for去访问
    ```python
    >>> L = [x * x for x in range(10)]
    >>> L
    [0, 1, 4, 9, 16, 25, 36, 49, 64, 81]
    >>> g = (x * x for x in range(10))
    >>> g
    <generator object <genexpr> at 0x1022ef630>

    >>> next(g)
    0
    >>> next(g)
    1
    >>> next(g)
    4
    >>> next(g)
    9
    ```

### 整形与字符串

#### 整形转字符串

转十六进制：`'0x{:02x}'.format(0x9fa370 ^ 0x68e0)`

### 字符编码

```python
"中".encode('utf-8')
Out[44]: b'\xe4\xb8\xad'

"中".encode('gbk')
Out[45]: b'\xd6\xd0'
```

```python
byte="中国".encode('utf-8')

byte.decode("utf-8")
Out[52]: '中国'

byte
Out[53]: b'\xe4\xb8\xad\xe5\x9b\xbd'

ascii=byte.decode("utf-8").encode("gbk")

ascii
Out[55]: b'\xd6\xd0\xb9\xfa'

ascii.decode("gbk")
Out[56]: '中国'
```

### 判断变量是否可以迭代
```python
isinstance('abc', Iterable) # str是否可迭代
```

### 通过enumerate内置函数实现“索引-元素对”
```python
 for i, value in enumerate(['A', 'B', 'C']):
```


## 2.2.函数

### 2.2.1-python3 引入注解

```python
a:str = "abc"
def xyz(x:int) -> int:
```

你可以通过 mypy 库来检验最终代码是否符合注解。

```sh
#安装 mypy：
$pip install mypy
#执行代码：
$mypy test.py
```

### 2.2.2-位置参数关键字参数

`def fun(pos1,pos2,*args, **kwargs):`是包含了两种参数形式的例子。其中`p1/p2`是位置参数， `*args, **kwargs`是关键字参数。

- *args 与 **kwargs 的区别，两者都是 python 中的可变参数：
- *args 表示任何多个无名参数，它本质是一个 tuple
- **kwargs 表示关键字参数，它本质上是一个 dict
- 如果同时使用 *args 和 **kwargs 时，必须 *args 参数列要在 **kwargs 之前。

```python
In [90]: def test(p1,p2,p3,*args, **kwargs):
    ...:     print("positions:{},{},{}".format(p1,p2,p3))
    ...:     print('args=', args)
    ...:     print('kwargs=', kwargs)
    ...:
    ...: test("p1","p2","p3",1, 2, A='a', B='b', C='c')
    ...:
positions:p1,p2,p3
args= (1, 2)
kwargs= {'A': 'a', 'B': 'b', 'C': 'c'}
```

如果函数的形参是定长参数，也可以使用 *args 和 **kwargs 调用函数，类似对元组和字典进行解引用：

tuple解引用调用位置参数

```python
In [91]: args = ("p1", "p2", "p3")
    ...: test(*args)
    ...:
positions:p1,p2,p3
args= ()
kwargs= {}
```

dict解引用调用位置参数,这里要注意的是**dict的key必须与位置参数的参数名字一样**

```python
In [92]: kwargs = {"p1": "p1", "p2": "p2", "p3": "p3"}
    ...:

In [93]: test(**kwargs)
positions:p1,p2,p3
args= ()
kwargs= {}
```

### 2.2.3-asynchronous generators 

Python 3.5之后出现的async/await的使用方法.

Starting with Python 3.6 we have [asynchronous generators](https://www.python.org/dev/peps/pep-0525/) and able to use yield directly inside coroutines.

对异步协程函数(`async def async_fun():`)的调用返回的是一个协程对象

例子：

```python
import asyncio

async def async_generator():
    for i in range(3):
        await asyncio.sleep(1)
        yield i*i

async def main():
    async for i in async_generator():
        print(i)


loop = asyncio.get_event_loop()
try:
    loop.run_until_complete(main())
finally:
    loop.run_until_complete(loop.shutdown_asyncgens())  # see: https://docs.python.org/3/library/asyncio-eventloop.html#asyncio.loop.shutdown_asyncgens
    loop.close()
```    

## 2.3.函数式编程

### 内建map()和reduce()函数

```python
help(map)
Help on class map in module builtins:

class map(object)
 |  map(func, *iterables) --> map object
 |  
 |  Make an iterator that computes the function using arguments from
 |  each of the iterables.  Stops when the shortest iterable is exhausted.
 ...
```

### filter

```python
#过滤出列表中的所有奇数：

def is_odd(n):
    return n % 2 == 1
 
newlist = filter(is_odd, [1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
print(newlist)
```

### 生成器函数next与send

生成器函数是一种特殊的函数；如果函数中有`yield`语句，那这个函数就是一个生成器函数。

调用生成器函数会返回一个特殊的迭代器，这个特殊的迭代器称为生成器（generator-iterator）。

#### yield expression方式

假设函数`functionname`含有"`receive=yield expression`"，因此函数`functionname`是一个生成器（generator-iterator）。

 "`receive=yield expression`"其实包含了3个步骤：

- step1、向函数`functionname`外抛出（返回）expression结果
- step2、函数`functionname`暂停(pause)，等待next()或send()恢复
- step3、赋值receive=MockGetValue() 。 这个MockGetValue()是假想函数，用来接收send()发送进来的值

注：如果是"`yield expression`"，就只有上面的`step1`与`step2`,就没有了`step3`

对生成器的启动是通过`g.send(None)或者next(g)`来启动的，一旦启动就从该函数的最开始执行到"`receive=yield expression`"语句结束的位置，也就是执行了上面`step1`与`step2`。

总结来说就是： 

- 初次启动是：执行`step1->step2`
- 除去初次启动，函数外的每一次next/send，都是 `step3->step1->step2`

下面以下面的`foo`函数为例，说明下生成器的next函数，与send函数

```python
def foo():
    print("starting...")   #1
    while True:
        res = yield 4      #2
        print("res:",res)  #3
```

对生成器执行举例，与过程说明：

```python
In [19]: g=foo()  #得到生成器g，将生成器赋值给一个变量，会使得该变量自动生成next方法

In [20]: next(g)  #触发生成器启动，执行了语句1，执行了语句2的step1、step2，并在此暂停
starting...
Out[20]: 4

In [21]: next(g) #执行语句2的的step3，因为没有通过send传递值进去，所以语句2的step3结果就是None
                 #因为while True导致重新执行语句2，执行它的step1，step2
res: None
Out[21]: 4

In [22]: next(g)
res: None
Out[22]: 4
...
In [26]: g.send(30) #前面暂停在语句2的step3，这时通过send送进来数据30，使得res赋值30，
                    #继续，又回到语句2执行step1、step2
res: 30
Out[26]: 4

In [27]: g.send(31)
res: 31
Out[27]: 4
```

下面是一个更复杂的例子，对这个例子的说明如下：

```python
In [51]: def double_inputs():
    ...:     while True:
    ...:         x=yield          #1
    ...:         print("res:",x)  #2
    ...:         yield x*3        #3

In [56]: g=double_inputs() #得到生成器g

In [57]: next(g)    #触发生成器启动，执行了语句1的step1、step2，并在此暂停，显然没有返回值

In [58]: g.send(10) #执行语句1的step3使得x值为10，执行语句2，执行语句3的step1/step2，使得返回值为30
                    ## 如果此时将g.send(*)替换为next(g)会报错，因为那样x没有值会使得语句3的step1不能计算
res: 10
Out[58]: 30

In [59]: next(g)    #因为while True，执行语句1的step1/step2，并在此暂停，没有返回值

In [60]: g.send(6)  #执行语句1的step3使得x值为6，执行语句2，执行语句3的step1/step2，使得返回值为18
res: 6
Out[60]: 18

In [61]: 
```

#### yield from iterable方式

`yield from iterable`本质上等于缩写版

```python
for item in iterable: 
    yield item
```

它起到的作用仅仅是简化yield的用法。

例如:

```python
In [36]: def test(i):
    ...:     yield from range(i)
    ...:     
In [37]: g=test(5)
In [38]: next(g)
Out[38]: 0
In [39]: next(g)
Out[39]: 1
In [40]: next(g)
Out[40]: 2
In [41]: next(g)
Out[41]: 3
In [42]: next(g)
Out[42]: 4
In [43]: next(g)
Traceback (most recent call last):
  File "<ipython-input-43-e734f8aca5ac>", line 1, in <module>
    next(g)
StopIteration
In [44]: 
In [44]: 
```

### 装饰器 todo



## 2.4.共享你的代码

共享通过： 模块、发布工具实现。

- 模块就是一个包含Python代码的文本文件。对模块的主要蒂求就是要求文件名以PY结尾，这是Python扩展名。要把你的函数转换为一个模块，需要把代码保存到一个适当命名的文件
- pyPI（python package index）为第三方库提供了一个集中的存储库

###  建立模块的例子

下面是一个最简单的例子，更复杂的例子见 [Python 库打包分发(setup.py 编写)简易指南](https://blog.konghy.cn/2018/04/29/setup-dot-py/)

首先建立几个文件
```sh
packageDemo/$touch nester.py
packageDemo/$touch setup.py
packageDemo/$touch readme.md
packageDemo/$touch __init__.py
```

它们的内容分别是：

```python
# setup.py 内容
import setuptools
setuptools.setup(
    name="mydemo",
    version="0.0.1",
    author="",
    author_email="",
    py_modules=["nester"], #需要打包的 Python 单文件列表
                           #packages，需要处理的包目录(通常为包含 __init__.py 的文件夹)，通常
                           # 应优先使用这种方式，而不是py_modules
	
    packages=setuptools.find_packages(),
)

# nester.py内容是随意写的一个函数print_lol

# __init__.py 内容空
```

首先执行sdist指令，构建发布文件

```sh
(myenv) packageDemo>python setup.py sdist
running sdist
running egg_info
writing mydemo.egg-info\PKG-INFO
writing dependency_links to mydemo.egg-info\dependency_links.txt
writing top-level names to mydemo.egg-info\top_level.txt
reading manifest file 'mydemo.egg-info\SOURCES.txt'
writing manifest file 'mydemo.egg-info\SOURCES.txt'
running check
warning: check: missing required meta-data: url

warning: check: missing meta-data: either (author and author_email) or (maintain
er and maintainer_email) must be supplied

creating mydemo-0.0.1
creating mydemo-0.0.1\mydemo.egg-info
copying files to mydemo-0.0.1...
copying nester.py -> mydemo-0.0.1
copying setup.py -> mydemo-0.0.1
copying mydemo.egg-info\PKG-INFO -> mydemo-0.0.1\mydemo.egg-info
copying mydemo.egg-info\SOURCES.txt -> mydemo-0.0.1\mydemo.egg-info
copying mydemo.egg-info\dependency_links.txt -> mydemo-0.0.1\mydemo.egg-info
copying mydemo.egg-info\top_level.txt -> mydemo-0.0.1\mydemo.egg-info
Writing mydemo-0.0.1\setup.cfg
Creating tar archive
removing 'mydemo-0.0.1' (and everything under it)

(myenv) c:\tuxueqing\pythondemo\packageDemo>
```

接着执行install，将构建安装到本地

```sh
(myenv) packageDemo>python setup.py install
running install
...
Installed c:\prog\anaconda3\envs\myenv\lib\site-packages\mydemo-0.0.1-py3.8.egg
Processing dependencies for mydemo==0.0.1
Finished processing dependencies for mydemo==0.0.1

(myenv) c:\tuxueqing\pythondemo\packageDemo>
```

最后导入该package，并执行的验证结果如下

```SH
(myenv) c:\tuxueqing\pythondemo\packageDemo>python
Python 3.8.1 (default, Jan  8 2020, 15:55:49) [MSC v.1916 64 bit (AMD64)] :: Ana
conda, Inc. on win32
Type "help", "copyright", "credits" or "license" for more information.
>>> import nester
>>> cast=["a","b",]
>>> nester.print_lol(cast)
a
b
>>>
```

## 2.5 异步模型：python协程

Lua、Python 语言都支持 coroutine 特性。coroutine 与 goroutine 在名字上类似，都可以将函数或者语句在独立的环境中运行，但是它们之间有两点不同：

- goroutine 可能发生并行执行；
- 但 coroutine 始终顺序执行。
- goroutines 通过通道来通信；
- coroutines 通过让出和恢复操作来通信

coruntine是一种异步模型。在异步模型中，一个任务要想运行必须显式放弃当前运行的任务的控制权。


## 2.6python对象

Python面向对象编程中，类中定义的方法可以是 @classmethod 装饰的类方法，也可以是 @staticmethod 装饰的静态方法，用的最多的还是不带装饰器的实例方法



## 2.5.文件与异常

操作文件示例见“编程语言/golang-java-python文件操作.md”

[Python 工匠：高效操作文件的三个建议](https://www.zlovezl.cn/articles/three-tips-on-writing-file-related-codes/)

下面是一个完整的示例,else/finally是可选的

```python
try:
     Normal execution block
except A:
     Exception A handle
except B:
     Exception B handle
except:
     Other exception handle
else:
     if no exception,get here
finally:
     print("不论是否有异常都会执行，finally")   
```