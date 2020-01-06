


## a scalable language

scala是面向对象函数编程语言，它是强类型语言。

`sbt:Hello> sbtVersion`查看sbt的版本号

## Chapter 2 First Steps in Scala

scala有两种变量：val 与var。val相当于java中的final variable。

注意到java.lang types are visible with their simple names in Scala
programs。例如java.lang.String 直接就可以用 String
```scala
scala> val msg3: java.lang.String = "Hello yet again, world!"
msg3: String = Hello yet again, world!
```

