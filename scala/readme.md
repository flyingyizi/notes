# 收藏

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
    scala没有提供读取二进制文件能力需要使用java类库
    ```scala
    val file = new File(filename)
    val in = new FileInputStream(file)
    val bytes = new Array[Byte](file.length.toInt)
    in.read(bytes)
    in.close()
    ```
    scala没有提供写文件能力需要使用java类库
    ```scala
    val out= new java.io.PrintWriter(filename)
    for(i<-1 to 100) out.println(i)
    out.close
    ```
- 正则表达式与正则表达式组
  ```scala
  import scala.util.matching.Regex
  val numPattern = """[0-9]+""".r //构造一个regrex对象
  numPattern.findAllIn("xxxxxxx")
  ```
  分组可以让我们方便的获取正则表达式的子表达式，通过在想要提取的子表达式两侧加上圆括号
  ```scala
  import scala.util.matching.Regex
  val numPattern = """([0-9]+) ([a-z]+) """.r //构造一个regrex对象
  for ( numPattern(num,item) <- numPattern.findAllIn("xxxxxxx")) {
      //处理num和item
  }
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

## spark sql
建个测试数据 people.txt
```json
1,       "a1",321
2,       "a2",322
3,       "a3",323
4,       "a4",324
5,       "a5",325
6,       "a6",326
7,       "a7",327
8,       "a8",328
9,       "a9",329
10,      "a10",330
```
建个测试数据 salary.txt
```json
{"id":1,"name":"a1", "salary":1321.0}
{"id":2,"name":"a2", "salary":1322.0}
{"id":3,"name":"a3", "salary":1323.0}
{"id":4,"name":"a4", "salary":1324.0}
{"id":5,"name":"a5", "salary":1325.0}
{"id":6,"name":"a6", "salary":1326.0}
{"id":7,"name":"a7", "salary":1327.0}
{"id":8,"name":"a8", "salary":1328.0}
{"id":9,"name":"a9", "salary":1329.0}
{"id":10,"name":"a10", "salary":330.0}
```

[spark Sql](https://www.cnblogs.com/hadoop-dev/p/6742677.html)

[spark结构化数据处理：Spark SQL、DataFrame和Dataset](https://www.cnblogs.com/seaspring/p/5831677.html)

[Spark SQL DataFrame和DataSet](https://www.jianshu.com/p/a19486f5a0ea)

[Spark编程基础](http://dblab.xmu.edu.cn/blog/spark/)

[How do I apply schema with nullable = false to json reading](https://stackoverflow.com/questions/47443483/how-do-i-apply-schema-with-nullable-false-to-json-reading?r=SearchResults)

- sparksql:创建通过编程接口指定Schema

    当不能被预先定义的时候，编程创建DataFrame分为三步：

    - 从原来的RDD创建一个Row格式的RDD.
    - 创建与RDD中Rows结构匹配的StructType，通过该StructType创建表示RDD的Schema.
    - 通过SQLContext提供的createDataFrame方法创建DataFrame，方法参数为RDD的Schema.
    ```scala
    //schema sample:
    //Array(
    //(StructField("id", IntegerType, true),0),
    //(StructField("name", StringType, true),1),
    //(StructField("age", IntegerType, true),2)
    //)

    /**
        * load file to temp table with the field schema.
        * temp table named
        */
    def loadTXT(sc: SparkContext,
                filename: String,
                schema: Array[(StructField, Int)],
                tempViewName:String): Unit = {

        //从指定的地址创建RDD,假设使用的文件每行中字段通过逗号分隔
        val personRDD = sc.textFile(filename).map(_.split(","))
        //通过StructType直接指定每个字段的schema
        val schemas = StructType(schema.map(x => x._1))
        val indexs = schema.map(x => x._2)
        //将RDD映射到rowRDD
        //val rowRDD = personRDD.map(s => Row(s(0).trim.toInt, s(1).trim, s(2).trim.toInt))
        val rowRDD = personRDD.map(line => {
        Row(schema.map(s => {
            if (s._1.dataType.isInstanceOf[StringType]) {
            line(s._2).trim
            } else if (s._1.dataType.isInstanceOf[IntegerType]) {
            line(s._2).trim.toInt
            }
        }): _*)
        })

        //创建SQLContext
        val sqlContext = new SQLContext(sc)
        //将schema信息应用到rowRDD上
        val personDataFrame = sqlContext.createDataFrame(rowRDD, schemas)
        //注册表
        //personDataFrame.registerTempTable(tempViewName)
        personDataFrame.createOrReplaceTempView(tempViewName)
    //personDataFrame.createOrReplaceGlobalTempView(tempViewName)
        //执行SQL
    //    val df = sqlContext.sql("select * from t_person order by age desc")
    //    //将结果以JSON的方式存储到指定位置
    //    df.write.json(args(1))
    //    //停止Spark Context
    //    sc.stop()
    }
    ```

- sparksql:通过自定义Schema解析部分json
  下面的代码演示了生成json schema
   ```scala
   object Hello {
    def main(args: Array[String]): Unit = {
        val schema =
        "props.name as realName,\nevent.info.eventId,\nprops.age,\nevent.own.telnum as tel"
        //println("input: " + args(0))
        JsonSchemaBuilder.getJsonSchema(schema).printTreeString()
    }
  }
   //schema dls sample
   //(
   //props.name as realName,
   //event.info.eventId,
   //props.age,
   //event.own.telnum as tel
   //)
   // ref https://blog.csdn.net/zgjdzwhy/article/details/72876913
   object JsonSchemaBuilder {
    final val columnSplitPattern = """\s*,\s*""".r
    private final val fieldSplitPattern = """\.""".r
    private final val fieldPattern = """([\w\.]+)(\s+as\s+\w+)?""".r
    def getJsonSchema(schema: String): StructType = {
        val fieldsList = columnSplitPattern
        .split(schema)
        .map(singleField => { //for each field map to list
            singleField match {
            case fieldPattern(fieldSource, xx) => {
                val fieldArray = fieldSplitPattern.split(fieldSource)
                fieldArray.toList
            }
            case _ => {
                throw new IllegalArgumentException(
                s"field format error:$singleField ,we need parent.children(as aliasName)")
            }
            }
        })
        .toList

        //get stuctType schema
        val structTypes = fieldsList.map(getStrcutType) 
        //println(structTypes)
        structTypes.reduce(mergeStructType)
    }

    private def getStrcutType(fields: List[String]): StructType = {
        fields match {
        case head :: Nil =>
            StructType(StructField(head, StringType, true) :: Nil) //只有一个
        case head :: tail =>
            StructType(StructField(head, getStrcutType(tail), true) :: Nil)
        }
    }

    private def mergeStructType(left: StructType,
                                right: StructType): StructType = {
        val newFields = ArrayBuffer.empty[StructField]
        val leftFields = left.fields
        val rightFields = right.fields
        val rightMapped = fieldsMap(rightFields)
        leftFields.foreach {
        case leftField @ StructField(leftName, leftType, leftNullable, _) =>
            rightMapped
            .get(leftName)
            .map {
                case rightField @ StructField(_, rightType, rightNullable, _) =>
                leftField.copy(
                    dataType = mergeStructType(leftType.asInstanceOf[StructType],
                                            rightType.asInstanceOf[StructType]),
                    nullable = leftNullable || rightNullable)
            }
            .orElse(Some(leftField))
            .foreach(newFields += _)
        }

        val leftMapped = fieldsMap(leftFields)
        rightFields
        .filterNot(f => leftMapped.get(f.name).nonEmpty)
        .foreach(newFields += _)
        StructType(newFields)
    }

    private def fieldsMap(
        fields: Array[StructField]): Map[String, StructField] = {
        import scala.collection.breakOut
        fields.map(s ⇒ (s.name, s))(breakOut)
    }
   }
   ```