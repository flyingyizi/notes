
# scala & spark & hadoop 安装

- 下载
  [下载hadoop-2.7.6](http://mirrors.hust.edu.cn/apache/hadoop/common/hadoop-2.7.6/hadoop-2.7.6.tar.gz), [下载winutils 2.7.1 ](https://github.com/steveloughran/winutils), [下载spark-2.4.0-bin-hadoop2.7](http://mirrors.tuna.tsinghua.edu.cn/apache/spark/spark-2.4.0/spark-2.4.0-bin-hadoop2.7.tgz)

## 安装hadoop
- 替换
  将下载winutils 2.7.1 bin目录替换 hadoop bin目录，将其中的其中的hadoop.dll在c:/windows/System32下也放一份；

- 配置环境变量：
    ```shell  
    #添加HADOOP_HOME: c:\prog\hadoop-2.7.6
    #PATH中添加%HADOOP_HOME%\bin；
    ```

- 配置文件内容：etc\hadoop\core-site.xml
```xml
<configuration>
    <property>
        <name>fs.defaultFS</name>
        <value>hdfs://localhost:9000</value>
    </property>
</configuration>
```

- 配置文件：hdfs-site.xml

    ```xml
    <configuration>
    <property>
            <name>dfs.replication</name>
            <value>1</value>
    </property>
    <property>
            <name>dfs.namenode.name.dir</name>
            <value>/C:/AppData/hadoopdata/namenode</value>
    </property>
    <property>
            <name>dfs.datanode.data.dir</name>
            <value>/C:/AppData/hadoopdata/datanode</value>  
    </property>
    </configuration>
    ```
- 配置文件：hadoop-env.cmd
    ```shell  
    #将JAVA_HOME用 @rem注释掉，编辑为JAVA_HOME的路径，然后保存；
    @rem set JAVA_HOME=%JAVA_HOME%
    set JAVA_HOME=C:\prog\Java\jdk1.8.0_144
    ```

- 运行格式化：
    ```shell
    #运行cmd窗口，执行
    hdfs namenode -format
    ```

- 启动hadoop:
  运行cmd窗口，切换到hadoop的sbin目录，执行“start-all.cmd”
    ```shell
    %HADOOP_HOME%\sbin\start-all.cmd
    ```

- 启动网址进行检测：
    ```shell
    #网页验证：
    http://localhost:50070/dfshealth.html#tab-overview
    #进程验证：cmd 执行
    jps 
    #文件验证：创建文件，执行文件上传
    hadoop fs -mkdir hdfs://localhost:9000/user
    hadoop fs -mkdir hdfs://localhost:9000/user/atmel

    hadoop fs -put D:\\1.txt hdfs://localhost:9000/user/atmel
    hadoop fs -ls hdfs://localhost:9000/user/atmel

    hadoop fs -put Sti_Trace.log hdfs://localhost:9000/user/atmel
    hadoop fs -ls hdfs://localhost:9000/user/atmel
    #Found 1 items
    #-rw-r--r--   1 tu_xu supergroup          0 2018-12-12 19:47 hdfs://localhost:9000/user/atmel/Sti_Trace.log

    hadoop fs -rm  hdfs://localhost:9000/user/atmel/*.log
    #18/12/12 19:51:17 INFO fs.TrashPolicyDefault: Namenode trash configuration: Deletion interval = 0 minutes, Emptier interval = 0 minutes.
    #Deleted hdfs://localhost:9000/user/atmel/Sti_Trace.log
    ```

## 安装spark

- 配置环境变量：
    ```shell  
    #添加SPARK_HOME: C:\prog\spark-2.4.0-bin-hadoop2.7
    #PATH中添加%HADOOP_HOME%\bin；
    ```
- 修改配置（举例）

  ```ini
  #对%SPARK_HOME%\conf\log4j.properties.template中的log4j.rootCategory由INFO修改为WARN
  log4j.rootCategory=WARN, console
  ```

- 启动spark-shell验证
    下面的验证打开一个文件并查看文件行数/首行内容，包含Spark字符串的行数
    ```shell
    C:\Users\tu_xu>spark-shell
    ## Setting default log level to "WARN".
    ## To adjust logging level use sc.setLogLevel(newLevel). For SparkR, use setLogLevel(newLevel).
    ## Spark context Web UI available at http://169.254.168.126:4040
    ## Spark context available as 'sc' (master = local[*], app id = local-1544616216761).
    ## Spark session available as 'spark'.
    ## Welcome to
    ##       ____              __
    ##      / __/__  ___ _____/ /__
    ##     _\ \/ _ \/ _ `/ __/  '_/
    ##    /___/ .__/\_,_/_/ /_/\_\   version 2.4.0
    ##       /_/
    ##
    ## Using Scala version 2.11.12 (Java HotSpot(TM) 64-Bit Server VM, Java 1.8.0_144)
    ## Type in expressions to have them evaluated.
    ## Type :help for more information.
    scala>
    scala> val textFile = sc.textFile("/C:/prog/spark-2.4.0-bin-hadoop2.7/README.md")
    #textFile: org.apache.spark.rdd.RDD[String] = /C:/prog/spark-2.4.0-bin-hadoop2.7/README.md MapPartitionsRDD[1] at textFile at <console>:24
    scala> textFile.count()
    #res2: Long = 105
    scala> textFile.first()
    #res3: String = # Apache Spark
    scala> val linesWithSpark = textFile.filter(line => line.contains("Spark")).count()
    #linesWithSpark: Long = 20
    ```

## 安装scala

由于spark是对scala版本有要求的，因此需要注意选择版本的匹配。如果是通过idea进行spark 编程，建议通过"file-project structure"设置scala sdk路径为 "%SPARK_HOME%\jars"目录，让idea选择与spark一样版本scala编译器编译。

[下载scala-2.12.7.msi](http://downloads.typesafe.com/scala/2.12.7/scala-2.12.7.msi), [下载sbt-1.2.7.msi](https://piccolo.link/sbt-1.2.7.msi)

- Configure system variables：
    ```shell
    #从当前在windows下的安装看，这些参数会自动设置
    #Linux
    export SCALA_HOME=/opt/scala
    PATH=%PATH%:$SCALA_HOME/bin
    #Windows
    SCALA_HOME=C:\Program Files (x86)\scala
    PATH=%SCALA_HOME%\bin;%PATH%
    ```

- set default direcotry
    ```shell
    #在%SBT_HOME%下的conf/sbtconfig.txt 文件增加以下内容：
    -Dfile.encoding=UTF8
    -Dsbt.global.base=C:/home/atmel/.sbt

    -Dsbt.boot.directory=d:/sbt-repository/boot/
    -Dsbt.ivy.home=d:/sbt-repository/

    #这两个是配置sbt.boot目录和ivy.home目录，用于缓存的jar包等信息存放位置。默认情况下jar包等信息存放于user home目录。另外运行sbt的下JVM参数也可以在sbtconfig.txt中指定。
    ```

- use fast mirror
    ```shell
    #在%SBT_HOME%下的repositories 文件增加以下内容：
    [repositories]
    local
    public: http://maven.aliyun.com/nexus/content/groups/public/  #这个maven
    typesafe:http://dl.bintray.com/typesafe/ivy-releases/ , [organization]/[module]/(scala_[scalaVersion]/)(sbt_[sbtVersion]/)[revision]/[type]s/[artifact](-[classifier]).[ext], bootOnly  #这个ivy
    ivy-sbt-plugin:http://dl.bintray.com/sbt/sbt-plugin-releases/, [organization]/[module]/(scala_[scalaVersion]/)(sbt_[sbtVersion]/)[revision]/[type]s/[artifact](-[classifier]).[ext]  #这个ivy
    sonatype-oss-releases

    sonatype-oss-snapshots
    ```
- create project from template
  ```shell
  c:\home\scalaTest>sbt new sbt/scala-seed.g8
  #Java HotSpot(TM) 64-Bit Server VM warning: ignoring option MaxPermSize=256m; support was removed in 8.0
  #[info] Set current project to scalatest (in build file:/C:/home/scalaTest/)
  #[info] Set current project to scalatest (in build file:/C:/home/scalaTest/)
  #A minimal Scala project.
  name [Scala Seed Project]: hello_world
  #Template applied in C:\home\scalaTest\.\hello_world
  c:\home\scalaTest>  
  ```

- create ensime config
    ```shell
    #在sbt安装ensime插件，是为了后面在vscode中能实现代码补全
    c:\home\scalaTest>echo addSbtPlugin("org.ensime" % "sbt-ensime" % "2.5.1") >> hello_world\project\pluings.sbt
    #生成项目的.enime文件供scala lanuage server plugin in vscode 使用
    c:\home\scalaTest>cd hello_world
    c:\home\scalaTest\hello_world>sbt ensimeConfig
    ```
- configure scala language server plugin in vscode
    ```shell
    #This setting is translated as Coursier's vm arguments: 
    Language server setttings:
    {
        "scalaLanguageServer.heapSize" : "768M"
    }
    ```


## sbt记录

- 增加库依赖
    ```scala
    //增加akka actor，spark...举例
    libraryDependencies ++= Seq(
        "org.scala-lang.modules" %% "scala-parser-combinators" % "1.1.1",
        "com.typesafe.akka" %% "akka-actor" % "2.5.19",
        "org.scala-lang.modules" %% "scala-swing" % "2.0.3",
        "org.apache.spark" %% "spark-core" % "2.4.0",
        "org.apache.spark" %% "spark-sql" % "2.4.0",
        scalaTest % Test
    )

    ```

