
https://blog.csdn.net/qq_37674858/article/details/81095101


https://github.com/datainq/go-mnist-client

- [发现问题是TensorFlow在BUILD target:libtensorflow.so的时候，根本没有把contrib相关内容打进去](https://blog.csdn.net/InsZVA/article/details/78616902)

- [一个 11 行 Python 代码实现的神经网络](http://python.jobbole.com/82758/)
- [C++ API载入tensorflow graph](https://blog.csdn.net/elaine_bao/article/details/78702236)

# 基础知识

- [特征提取方法: one-hot 和 TF-IDF](http://www.cnblogs.com/lianyingteng/p/7755545.html)
- []()

#
参考[install tenorflow source in windows](https://github.com/tensorflow/docs/blob/master/site/en/install/install_sources_windows.md)在windows进行源码编译。参考[libtensorflow_cpu.sh](https://github.com/tensorflow/tensorflow/blob/master/tensorflow/tools/ci_build/windows/libtensorflow_cpu.sh)对编译结果进行打包. 最终的相关命令如下： 
```shell

##Disable the path conversion heuristic in MSYS. MSYS automatically converts arguments that look like a Unix 
##path to Windows path when running a program, this will confuse Bazel. (eg. A Bazel label //foo/bar:bin is 
##considered a Unix absolute path, only because it starts with a slash)

export MSYS_NO_PATHCONV=1
export MSYS2_ARG_CONV_EXCL="*"

## Add the directory where you install Bazel in $PATH. Assume you have Bazel installed at C:\tools\bazel.exe, issue the following command:

## `:` is used as path separator, so we have to convert the path to Unix style.
export PATH="/c/tools:$PATH"

## Add the directory where you install Python in $PATH. Assume you have Python installed at C:\tools\Anaconda3, issue the following command:
export PATH="/c/tools/Anaconda3:$PATH"

./configure
bazel build  --config=monolithic  -c opt  --copt=-msse4.1 --copt=-msse4.2 --copt=-mavx  //tensorflow:libtensorflow.so
##bazel build  --config=monolithic -c opt  --copt=-msse4.1 --copt=-msse4.2 --copt=-mavx  //tensorflow:libtensorflow_cc.so
##bazel build  --config=monolithic  --config=opt   --copt=-msse4.1 --copt=-msse4.2 --copt=-mavx //tensorflow/tools/pip_package:build_pip_package
bazel build --config=mkl -c opt --copt=-mavx --copt=-mavx2 --copt=-mfma --copt=-mavx512f --copt=-mavx512pf --copt=-mavx512cd --copt=-mavx512er //tensorflow/tools/pip_package:build_pip_package

DIR=lib_package
rm -rf ${DIR}
mkdir -p ${DIR}
# Zip up the .dll, LICENSE and include files for the C library.
mkdir -p ${DIR}/include/tensorflow/c
mkdir -p ${DIR}/include/tensorflow/c/eager
mkdir -p ${DIR}/lib
cp bazel-bin/tensorflow/libtensorflow.so ${DIR}/lib/libtensorflow.dll
cp tensorflow/c/c_api.h ${DIR}/include/tensorflow/c
cp tensorflow/c/eager/c_api.h ${DIR}/include/tensorflow/c/eager
cp bazel-genfiles/tensorflow/tools/lib_package/include/tensorflow/c/LICENSE ${DIR}/include/tensorflow/c
cd ${DIR}
zip libtensorflow-cpu-windows-$(uname -m).zip \
  lib/libtensorflow.dll \
  lib/libtensorflow.dll.a \
  include/tensorflow/c/eager/c_api.h \
  include/tensorflow/c/c_api.h \
  include/tensorflow/c/LICENSE
rm -rf lib include
```

通过下面的命令生成gcc使用的lib文件
```shell
pexports -v libxxx.dll > libxxx.def
dlltool --dllname libxxx.dll --def libxxx.def --output-lib libxxx.dll.a 
```

最后按照[tensorflow-go](https://github.com/tensorflow/tensorflow/tree/master/tensorflow/go)中的指令验证下
```shell
c:\home\atmel\nnk>go test github.com/tensorflow/tensorflow/tensorflow/go
ok      github.com/tensorflow/tensorflow/tensorflow/go  0.471s
```

https://github.com/datainq/go-mnist-client


- [	
Tensorflow C++ 编译和调用图模型](https://blog.csdn.net/rockingdingo/article/details/75452711)

- 直接安装编译好的库.通过执行以下命令下载并解压 TensorFlow C 语言库到 /usr/local/lib 目录:
```shell
TF_TYPE="cpu" # Change to "gpu" for GPU support
TARGET_DIRECTORY='/usr/local'
curl -L \
  "https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-${TF_TYPE}-$(go env GOOS)-x86_64-1.4.0-rc0.tar.gz" |
sudo tar -C $TARGET_DIRECTORY -xz
```





### Go(GoLang)配置Grpc+ProtoBuf所需的一些资源
安装官方安装命令：go get google.golang.org/grpc

是安装不起的，会报：

package google.golang.org/grpc: unrecognized import path "google.golang.org/grpc"(https fetch: Get https://google.golang.org/grpc?go-get=1: dial tcp 216.239.37.1:443: i/o timeout)

原因是这个代码已经转移到github上面了，但是代码里面的包依赖还是没有修改，还是google.golang.org这种，

所以不能使用go get的方式安装，正确的安装方式：
```shell
    git clone https://github.com/grpc/grpc-go.git $GOPATH/src/google.golang.org/grpc
    git clone https://github.com/golang/net.git $GOPATH/src/golang.org/x/net
    git clone https://github.com/golang/text.git $GOPATH/src/golang.org/x/text
    go get -u github.com/golang/protobuf/{proto,protoc-gen-go}
    git clone https://github.com/google/go-genproto.git $GOPATH/src/google.golang.org/genproto
    cd $GOPATH/src/
    go install google.golang.org/grpc
```


- 以找tf.RandomNormal为例，它在go wrapper中是对应哪个函数？
" C:\home\GO\src\github.com\tensorflow\tensorflow\tensorflow\core\api_def\base_api\"路径中查找RandomNormal，发现是“api_def_RandomStandardNormal.pbtxt”文件，则对应的go wraper是RandomStandardNormal




## 在linux下编译tensorflow，
参见[TensorFlow in Go](https://github.com/tensorflow/tensorflow/tree/master/tensorflow/go)    

bazel build -c opt  --copt=-msse4.1 --copt=-msse4.2 --copt=-mavx  //tensorflow:libtensorflow.so

```shell
wget https://repo.anaconda.com/archive/Anaconda3-5.2.0-Linux-x86_64.sh
bash ./Anaconda3-5.2.0-Linux-x86_64.sh 
# 设置anaconda mirror
conda config –add channels https://mirrors.ustc.edu.cn/anaconda/pkgs/free/
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/pkgs/free/
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/pkgs/main/
conda config --set show_channel_urls yes
# 安装源码编译tensorflow pip包需要的包
conda install keras-applications
conda install -c anaconda keras-preprocessing
# 在tensorflow configure file 所在的目录下
./configure
#执行configure后再执行下面的命令
bazel build --config=mkl -c opt --copt=-mavx --copt=-mavx2 --copt=-mfma --copt=-mavx512f --copt=-mavx512pf --copt=-mavx512cd --copt=-mavx512er //tensorflow/tools/pip_package:build_pip_package
```

## Kaggle“手写识别”

- [训练数据和测试数据](https://www.kaggle.com/c/digit-recognizer/data)
- [其他人项目中包含有的kaggle训练/测试数据](https://github.com/dzhibas/kaggle-digit-recognizer)  ，[另一个](https://github.com/kurapan/CNN-MNIST)

- [手写识别 91 star](https://github.com/akshaybahadur21/Digit-Recognizer)



#  梯队下降

首先，我们将创建一个以参数θ为特征函数的代价函数 $$J\left( \theta \right)=\frac{1}{2m}\sum\limits_{i=1}^{m}{{{\left( {{h}_{\theta }}\left( {{x}^{(i)}} \right)-{{y}^{(i)}} \right)}^{2}}}$$ 其中：\[{{h}{\theta }}\left( x \right)={{\theta }^{T}}X={{\theta }{0}}{{x}{0}}+{{\theta }{1}}{{x}{1}}+{{\theta }{2}}{{x}{2}}+...+{{\theta }{n}}{{x}_{n}}\]

```python
def computeCost(X, y, theta):
    inner = np.power(((X * theta.T) - y), 2)
    return np.sum(inner) / (2 * len(X))
```    

# batch gradient decent（批量梯度下降）
$$\theta _j:=\theta _j-\alpha \frac{\partial }{\partial \theta _j}J\left( \theta  \right)$$
即：
$$\theta  _j:=\theta  _j -\alpha \frac{1}{m}\sum_{i=1}^{m} \left( h_{\theta } (x^{(i)}) -y^i  \right)x_{j}^{(i)}$$

```python
def gradientDescent(X, y, theta, alpha, iters):
    temp = np.matrix(np.zeros(theta.shape))
    parameters = int(theta.ravel().shape[1])
    cost = np.zeros(iters)
    
    for i in range(iters):
        error = (X * theta.T) - y
        
        for j in range(parameters):
            term = np.multiply(error, X[:,j])
            temp[0,j] = theta[0,j] - ((alpha / len(X)) * np.sum(term))
            
        theta = temp
        cost[i] = computeCost(X, y, theta)
        
    return theta, cost
```

