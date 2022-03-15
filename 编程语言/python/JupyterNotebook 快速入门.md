
# install 

## option1： 安装ipython notebook

```shell
pip install ipython
pip install “ipython[notebook]”
ipython notebook #浏览器会打开jupyter页面
```
或
```
pip install jupyter
jupyter notebook
```

## option2: 使用anaconda

安装 Anaconda，从开始菜单中打开 jupyter notebook 的快捷方式

或打开 Anaconda Prompt（CMD，PowerShell 等均可），执行"`jupyter notebook`"命令

## option3: NTERACT，所有ipynb文件直接双击打开。

https://github.com/nteract/nteract

# 简单使用

```shell
jupyter notebook
#jupyter notebook --no-browser --port=8889

运行上面的命令之后，你将看到类似下面这样的输出：

[I 20:06:36.367 NotebookApp] Writing notebook server cookie secret to /run/user/1000/jupyter/notebook_cookie_secret
[I 20:06:36.813 NotebookApp] Serving notebooks from local directory: /home/your_username
[I 20:06:36.813 NotebookApp] 0 active kernels
[I 20:06:36.813 NotebookApp] The IPython Notebook is running at: http://localhost:8888/
[I 20:06:36.813 NotebookApp] Use Control-C to stop this server and shut down all kernels (twice to skip confirmation).
```

如果想新建一个 notebook，只需要点击New，选择你希望启动的 notebook 类型即可。

Matplotlib 集成
如果你用 Python 绘制过图形，那你肯定知道 matplotlib。Matplotlib 是一个用于创建漂亮图形的 Python 库，结合 Jupyter notebook 使用时体验更佳。

要想在 Jupyter notebook 中使用 matplotlib，需要告诉 Jupyter 获取 matplotlib 生成的所有图形，并将其嵌入 notebook 中。为此，需要计算：
```python
%matplotlib inline
```
运行这个指令可能要花个几秒钟，但是在 notebook 中需要执行一次即可。接下来，我们来绘制一个图形，看看具体的集成效果：

import matplotlib.pyplot as plt
import numpy as np

x = np.arange(20)
y = x**2

plt.plot(x, y)
上面的代码将绘制方程式 y=x^2 。计算单元格后，会得到如下图形：

非本地内核
我们可以非常容易地在一台电脑上启动 Jupyter，而且支持多人通过网络连接同一个 Jupyter 实例。在上一篇文章中，你有没有注意启动 Jupyter 时出现过这样一段话：

The IPython Notebook is running at: http://localhost:8888/
这意味着，你的 notebook 是本地运行的，可以在浏览器上打开 http://localhost:8888/ ，从而访问 notebook。你也可以修改下配置，让该 notebook 可以被公开访问。这样，任何知道 notebook 地址的人都可以连接到 notebook 进行远程修改。


## 安装 golang jupyter

[install manual](https://github.com/gopherdata/gophernotes)

```shell
env GO111MODULE=on go get github.com/gopherdata/gophernotes
mkdir -p `jupyter --data-dir`/kernels/gophernotes
cd "$(go env GOPATH)"/src/github.com/gopherdata/gophernotes
cp kernel/* `jupyter --data-dir`/kernels/gophernotes
cd `jupyter --data-dir`/kernels/gophernotes
chmod +w ./kernel.json # in case copied kernel.json has no write permission
sed "s|gophernotes|$(go env GOPATH)/bin/gophernotes|" < kernel.json.in > kernel.json
```


## Jupyter notebook远程访问服务器

Jupyter notebook远程访问服务器

```shell
$jupyter notebook --generate-config
Writing default config to: /home/atmel/.jupyter/jupyter_notebook_config.py
$ipython
In [2]: from IPython.lib import passwd
In [3]: passwd()
Enter password: 
Verify password: 
Out[3]: 'sha1:456dafc566f0:a29c3bffe48fb0d8981d9c1fd67249e6b233e43e'

$vim /home/atmel/.jupyter/jupyter_notebook_config.py
#c.NotebookApp.ip='*'
#c.NotebookApp.password = u'sha1:456da...刚才复制的那个密文'
#c.NotebookApp.open_browser = False
#c.NotebookApp.port =8888 #随便指定一个端口
```

## Jupter notebook配置目录

有两种方法配置jupter notebook的工作目录

1.最简单的方法就是，在cmd里面直接输入： jupyter notebook 目录地址，然后就会在浏览器打开你的工作目录。

2.第二种方法，修改默认配置，先查看你的notebook配置文件在哪里，输入`jupyter notebook --generate-config`,得到配置文件路径. 例如"/home/atmel/.jupyter/jupyter_notebook_config.py". 在该配置文件中查找`c.NotebookApp.notebook_dir`,修改值为自己希望的目录

