

##
- [[使用 Docker 一步搞定 ZooKeeper 集群的搭建]](https://segmentfault.com/a/1190000006907443)
- [[ZooKeeper Watcher 和 AsyncCallback 的区别与实现]](http://blog.jobbole.com/104833/)
```shell
docker image pull zookeeper
docker run --name zookeeper-1 --restart always -d zookeeper   //它默认会导出 2181 端口.
docker exec -it 容器id zkCli.sh 
```

## maven 安装

### win env
        - 下载常用的Maven版本，如apache-maven-3.5。下载地址:http://maven.apache.org/download.html.
        ```shell
        http://mirror.bit.edu.cn/apache/maven/maven-3/3.5.2/binaries/apache-maven-3.5.2-bin.zip
        （3） 解压到/usr/local目录下，命令：
        tar -zxvf apache-maven-3.3.9-bin.tar.gz  -C /usr/local。解压后的目录为：apache-maven-3.3.9。
        （4） 建立链接，方便升级：
        ln -s /usr/local/apache-maven-3.3.9/ maven
        （5） 在/etc/profile 添加下面两行:
        export M3_HOME=/usr/local/apache-maven
        export PATH=${M3_HOME}/bin:${PATH}
        （6） 执行下面命令，使环境参数生效:
        source /etc/profile
        （7） 查看mvn版本，看是否配置正确。命令：
        mvn –version
        ```
### linux env
        - 下载Linux下的最新版的Maven，即apache-maven-3.3.9-bin.tar.gz。下载地址:http://maven.apache.org/download.html.
        ```shell
        http://mirror.bit.edu.cn/apache/maven/maven-3/3.5.2/binaries/apache-maven-3.5.2-bin.tar.gz
        （3） 解压到/usr/local目录下，命令：
        tar -zxvf apache-maven-3.3.9-bin.tar.gz  -C /usr/local。解压后的目录为：apache-maven-3.3.9。
        （4） 建立链接，方便升级：
        ln -s /usr/local/apache-maven-3.3.9/ maven
        （5） 在/etc/profile 添加下面两行:
        export M3_HOME=/usr/local/apache-maven
        export PATH=${M3_HOME}/bin:${PATH}
        （6） 执行下面命令，使环境参数生效:
        source /etc/profile
        （7） 查看mvn版本，看是否配置正确。命令：
        mvn –version
        ```

## java 安装

### win env
        
        - 步骤1.1：下载JDK
        通过访问链接下载最新版本的JDK，并下载常见java版本的[[Java]](http://www.oracle.com/technetwork/java/javase/downloads/index.html)。

       推荐是JDK 8，例如jdk1.8.0_14464.msi，在你的机器上下载该文件。

        - 步骤1.2：安装
        双击jdk1.8.0_14464.msi，选择安装目录进行安装
        ```
        - 步骤1.3：设置路径

        要设置路径和JAVA_HOME变量，将下面两个添加到环境变量中。
        ```shell
        set JAVA_HOME = C:\prog\Java\jdk1.8.0_144
        set PATH=$PATH:%JAVA_HOME%\bin
        ```

        - 步骤1.5：检查是否安装成功
        ```shell
        java  -version
        ```



### linux env
        
        - 步骤1.1：下载JDK
        通过访问链接下载最新版本的JDK，并下载最新版本的[[Java]](http://www.oracle.com/technetwork/java/javase/downloads/index.html)。

        最新版本（在编写本教程时）是JDK 8u 60，文件是“jdk-8u60-linuxx64.tar.gz"。请在你的机器上下载该文件。

        - 步骤1.2：提取文件
        通常，文件会下载到download文件夹中。验证并使用以下命令提取tar设置。

        $ cd /go/to/download/path
        $ tar -zxf jdk-8u60-linux-x64.gz

        - 步骤1.3：移动到opt目录

        要使Java对所有用户可用，请将提取的Java内容移动到“/usr/local/java"文件夹。
        ```shell
        $ su 
        password: (type password of root user)
        $ mkdir /opt/jdk
        $ mv jdk-1.8.0_60 /opt/jdk/
        ```
        - 步骤1.4：设置路径

        要设置路径和JAVA_HOME变量，请将以下命令添加到〜/.bashrc文件中。
        ```shell
        export JAVA_HOME = /usr/jdk/jdk-1.8.0_60
        export PATH=$PATH:$JAVA_HOME/bin
        ```
        现在，将所有更改应用到当前运行的系统中。

        $ source ~/.bashrc

        - 步骤1.5：Java替代
        ```shell
        使用以下命令更改Java替代项。
        update-alternatives --install /usr/bin/java java /opt/jdk/jdk1.8.0_60/bin/java 100
        ```

