
# 预备动作

## 资源

[吴恩达的Machine Learning](github.com/deeplearning-ai/machine-learning-yearning-cn/releases/download/v0.5.0/MLY-zh-cn.pdf)
[NYU的Data Science项目课程 DS1003 Machine Learning](https://davidrosenberg.github.io/ml2019/#home),[知乎介绍](https://www.zhihu.com/question/37031588)


## 安装keras以及相关开发包,

```sh
#NumPy系统是Python的一种开源的数值计算扩展。这种工具可用来存储和处理大型矩阵
#SciPy是一款方便、易于使用、专为科学和工程设计的Python工具包.它包括统计,优化,整合,线性代数模块,傅里叶变换,信号和图像处理,常微分方程求解器等等
#Matplotlib 是一个 Python 的 2D绘图库，它以各种硬拷贝格式和跨平台的交互式环境生成出版质量级别的图形。
#scikit-learn 是一个基于SciPy的面向机器学习的模块
#scikit-image是一个图像处理和计算机视觉的算法集合。
$pip3 install -U --pre numpy scipy matplotlib scikit-image
$pip3 install -U --pre scikit-learn
$pip3 install -U --pre tensorflow
$pip3 install -U --pre keras
```

- 注1，对源码安装的python3，由于缺少一些库，安装scipy是会失败的，所以请参考“python笔记.md”中对scipy安装的说明，增加额外库的安装
- 注2,对python3最好是3.6，如果是较新的3.8/3.9 类似scikit-learn还不支持

## mnist数据集

由于网络的原因mnist数据集网络下载有问题。 下面是解决方案之一：

- 通过[baidupan密码 db0s](https://pan.baidu.com/s/1Gk4SSCFENf1a55b7cQ5WCw)下载mnist.npz文件
- 修改mnist.py文件

```python
    #将下面path更改为
    #path = get_file(path,
    #                origin='https://s3.amazonaws.com/img-datasets/mnist.npz',
    #                file_hash='8a61469f7ea1b51cbae51d4f78837e45')
    path = '/home/atmel/xxx/mnist.npz'
```

# python机器学习
## chapter 1:什么是深度学习

机器学习系统是训练出来的，而不是明确用程序编写出来的。 它训练出来的结果是规则。

现状是： 机器学习（尤其是深度学习）呈现出相对较少的数学理论（可能太少了），并且是以工程为导向的。这是一门需要上手实践的学科，想法更多地是靠实践来证明，而不是靠理论推导。

logistic 回归（logistic regression，简称logreg），被认为是现代机器学习的“hello world”。这里特别强调logreg 是一种分类算法，而不是回归算法。

深度学习用于计算机视觉的两个关键思想：即卷积神经网络和反向传播。

## chapter 2:神经网络的数学基础

要理解深度学习，需要熟悉很多简单的数学概念：张量、张量运算、微分、梯度下降等。

MNIST 数据集，它是机器学习领域的一个经典数据集,MNIST 数据集预先加载在Keras 库中，其中包括4 个Numpy 数组。`from keras.datasets import mnist`

张量（tensor）：它是一个数据容器。比如常见矩阵就是二维张量， 标量就是0维张量。 张量轴的个数（ndim）也称为阶（rank）。张量形状是一个tuple，记录沿每个轴的元素个数。标量的形状是空。

例如mnist的train_images形状是(6000,28,28),即它是6000个矩阵，每个矩阵是(28,28)

下面演示了0D张量，1D张量，2D张量，
```python
In [12]: import numpy as np
#0D张量,形状是()
In [13]: x=np.array(12)
In [14]: x
Out[14]: array(12)
In [15]: x.ndim
Out[15]: 0
#1D张量，形状是(3,)
In [16]: x=np.array([1,2,3])
In [17]: x
Out[17]: array([1, 2, 3])
In [18]: x.ndim
Out[18]: 1
#2D张量，形状是(3,3)
In [20]: x=np.array([[1,2,3],[4,5,6]])
In [21]: x
Out[21]: 
array([[1, 2, 3],
       [4, 5, 6]])
In [22]: x.ndim
Out[22]: 2
In [23]: 
```

### numpy操作tensor

`my_slice = train_images[10:100, 0:28, 0:28] #选择第10~100 个数字（不包括第100 个）` 

## chapter 4:机器学习基础

