# install

[scala-tutorial](http://www.runoob.com/scala/scala-tutorial.html)
[scala school](https://github.com/twitter/scala_school/blob/master/web/zh_cn/basics.textile)
[Scala-Quick-Start-for-Java-Programmers](https://colobu.com/2015/01/14/Scala-Quick-Start-for-Java-Programmers/#more)
[stackoverflow scala info](https://stackoverflow.com/tags/scala/info)

# 语法记录

[what-are-all-the-uses-of-an-underscore-in-scala](https://stackoverflow.com/questions/8000903/what-are-all-the-uses-of-an-underscore-in-scala)

| scaladoc  | 解释 |
|--|--|
|def appendAll(xs:TraversableOnce[A]):Uint |xs参数可以是任何带有TraversableOnce特质的集合，该特质是scala集合层级中最通用的一个。其他你还可能在Scaladoc中遇到的通用特质有Traversable和Iterable。所有的Scala集合都实现这些特质，至于它们之间有什么区别，对于类库的使用者而言属于科研话题，你只需要把它们当做是在说“任意集合"即可。不过，seq特质需要元素能够通过整数下标访问。对应“数组、列表或字符串”
|def copyToArray\[B>:A](xs:Array[B]):Uint |注意这个函数将一个ArrayBuffer[A]拷贝成一个Array[B]。这里的B仅允许为A的超类。举例来说，你可以把一个ArrayBuffer[Int]拷贝成一个Array[Any]。初看时，可以忽略[B>:A]，并把B替换成A |
|||
|||
|||




- 基本语法，Scala 基本语法需要注意以下几点:

    - 区分大小写:  Scala是大小写敏感的，这意味着标识Hello 和 hello在Scala中会有不同的含义。

    - 类名: 对于所有的类名的第一个字母要大写。如果需要使用几个单词来构成一个类的名称，每个单词的第一个字母要大写。

        示例：class MyFirstScalaClass

    - 方法名称: 所有的方法名称的第一个字母用小写。如果若干单词被用于构成方法的名称，则每个单词的第一个字母应大写。

        示例：def myMethodName()

    - 程序文件名: 程序文件的名称应该与对象名称完全匹配(新版本不需要了，但建议保留这种习惯)。保存文件时，应该保存它使用的对象名称（记住Scala是区分大小写），并追加".scala"为文件扩展名。 （如果文件名和对象名称不匹配，程序将无法编译）。

        示例: 假设"HelloWorld"是对象的名称。那么该文件应保存为'HelloWorld.scala"

        def main(args: Array[String]) - Scala程序从main()方法开始处理，这是每一个Scala程序的强制程序入口部分

- 定义包package

    Scala 使用 package 关键字定义包，在Scala将代码定义到某个包中有两种方式：

    第一种方法和 Java 一样，在文件的头定义包名，这种方法就后续所有代码都放在该包中。 比如：
    ```scala
    package com.runoob
    class HelloWorld
    ```
    第二种方法有些类似 C#，如：第二种方法，可以在一个文件中定义多个包。
    ```scala
    package com.runoob {
    class HelloWorld
    }
    ```

- 引用import

    import语句可以出现在任何地方，而不是只能在文件顶部。import的效果从开始延伸到语句块的结束。这可以大幅减少名称冲突的可能性。注意：默认情况下，Scala 总会引入 java.lang._ 、 scala._ 和 Predef._，这里也能解释，为什么以scala开头的包，在使用时都是省去scala.的。

    如果想要引入包中的几个成员，可以使用selector（选取器）：
    ```scala
    import java.awt.{Color, Font}
    // 重命名成员
    import java.util.{HashMap => JavaHashMap}
    // 隐藏成员
    import java.util.{HashMap => _, _} // 引入了util包的所有成员，但是HashMap被隐藏了
    ```

- 函数与匿名函数定义
    ```scala
    scala> def addOne(m: Int): Int = m + 1
    scala> val addOne = (m: Int) => m + 1
    ```

- 使用数组Array

    索引基于0。定长数组使用Array，变长数组使用ArrayBuffer。
    

    ```scala
    val numberNames = Array("one","two","three")

    val numberNames= new Array[String](3)
    numberNames(0) = "one" //numberNames.update(0,"one")
    ```
    多维
    ```scala
    val myMatrix = Array.ofDim[Int](3,3)
    myMatrix(0)(0)=1
    ```
    通过for(...) yield 循环创建一个类型与原始类型相同的集合
    ```scala
    val a =Array(2,3,4,5)  //a: Array[Int] = Array(2, 3, 4, 5)
    val result = for (elem <-a) yield 2*elem  //result: Array[Int] = Array(4, 6, 8, 10)
    ```

- 使用collection: list, set, map,tuple 
  Map 有两种类型，可变与不可变，区别在于可变对象可以修改它，而不可变对象不可以。默认情况下 Scala 使用不可变 Map。如果你需要使用可变集合，你需要显式的引入 import scala.collection.mutable.Map 类. 在 Scala 中 你可以同时使用可变与不可变 Map，不可变的直接使用 Map，可变的使用 mutable.Map。

  List  索引基于0， List一旦创建不可改变，
  tuple 索引基于1,索引是"_N",tuple的类型取决于它所含的元素数量与这些元素的类型  

    ```scala
    // 定义整型 List
    val x = List(1,2,3,4)

    // 定义 Set
    val x = Set(1,3,5,7)

    // 定义 Map
    val x = Map("one" -> 1, "two" -> 2, "three" -> 3)
    val y = scala.collection.mutable.Map("one" -> 1, "two" -> 2, "three" -> 3)
    //for( (k,v)<- 映射 ) 处理k和v
    //for( (k,v)<- 映射 ) yield (v,k) 交换键与值

    // 创建两个不同类型元素的元组
    val x = (10, "Runoob")

    // 定义 Option
    val x:Option[Int] = Some(5)
    ```
- 文件操作
    ```scala  
    scala.io.fromFile("").getLines().toList
    ```

- for
    ```scala
    for (i <- m to n) //[m,n]
    for (i <- m until n) //[m,n)
    ```

    过滤器使用方法
    ```scala
    for (
        file <-  xxxx
        if file.xxx  ;
        if file.xxxxx
    )
    ```

- class

    类中的字段自动带getter方法与setter方法。可以用@BeanProperty注解生成javabeans的getXxx/setXxx方法。

    主构造器会执行类定义中的所有语句，例如下面的println是主构造器的一部分，每当对象被构造出来，这个代码都会被执行:
    ```scala
    class Person(val name: String, val age:Int) {
        ptintln("Just constructed another person")
        def description = name + " is " + age + " years old "
    }
    ```

- 单例对象  
    ```scala
    //当单例对象与某个类共享同一个名称时，称为这个类的伴生对象(companio object)
    class Account private(val id:Int) {
    }
    object Account {  //伴生对象
        def apply() = new Account(newUniqueNumber())
    }
    ```
    不与伴生类共享名称的单例对象称为独立对象(standalone object)

- 特质 trait

   trait中没有被实现的方法默认就是抽象的（absract），因此它可以替代abstract
    ```scala
    trait Logger {
        def log(msg: String) //这是一个抽象方法
    }
    trait X {}
    class ConsoleLogger extends Logger with X with Serializable { //关联多个trait通过with
        def log(msg: String) {println(msg)} //不需要写override
    }
    ```
- 操作符
  
  scala中变量/函数/类等的名称统称为标识符号。scala中的标识符比java/c有更多的选择，并且还可以通过反引号将保留关键字作为普通标识符使用，例如\`yield\`就是一个普通的标识符了。
    ```scala
    val `val`=43
    ```
  
  “a 标识符 b”中置(infix)表达式, 中置操作符是二元的，它有两个参数。
    ```scala
    1 to 10 //等价于  1.to(10)
    ```

  “a 标识符”后置(postfix)操作符，属于一元操作符
    ```scala
    1 toString //等价于  1.toString()
    ```

  “标识符 a”前置(prefix)操作符，属于一元操作符, 这个标识符只能是"+ - ! ~"
    ```scala
    -a //等价于  a.unary_-
    ```

- 提取器(//TODO)

- 高阶函数
    ```scala
    import scala.math._
    val fun = ceil _  
    // 后面的“—”意味着确实指的是这个函数，而不是碰
    //巧忘记了给他送参数,从技术上来说_将ceil方法转成了函数
    ```

- 柯里化(currying) //TODO
  将原来接受两个参数的函数变成新的接受一个参数的函数的过程。新的函数返回一个以原来第二个参数作为参数的函数。

```scala
def mul (x: Int, y:Int) = x*y

def mulOneAtATime(x:Int) = (y:Int) => x * y
def mulOneAtATime(x:Int)(y:Int) = x * y
```  


# spark & hadoop 安装
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
    //增加akka actor举例
    libraryDependencies ++=Seq(
        "com.typesafe.akka" %% "akka-actor" % "2.5.19",
        。。。
    )
    ```

