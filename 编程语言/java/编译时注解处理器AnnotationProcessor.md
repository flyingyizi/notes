

编译时注解处理(annotation processor),以java code (or compiled byte code)中为输入，根据他们中的注解进行处理并生成输出，比如输出java文件。

注解处理(annotation processor)是在自己独立的JVM运行的。

[Debugging an Annotation Processor in your project](https://blog.xmartlabs.com/2016/03/28/Debugging-an-Annotator-Processor-in-your-project/)


[ANNOTATION PROCESSING 101](http://hannesdorfmann.com/annotation-processing/annotationprocessing101)

## 问题记录

1. 在编码过程中，识别出识别出一个问题 [AutoService注解无法生成META-INF文件](https://blog.csdn.net/cpcpcp123/article/details/103871815)
对这个问题,一种方式是新增`annotationProcessor`,一种是自己在processor下手写 `resources/META-INF/services/
javax.annotation.processing.Processor` 文件

```groovy
    implementation 'com.google.auto.service:auto-service:1.0-rc6'
    annotationProcessor 'com.google.auto.service:auto-service:1.0-rc6'
```

2. 

[annotation-processor-in-intellij-and-gradle](https://stackoverflow.com/questions/42441844/annotation-processor-in-intellij-and-gradle)


## Processor API

AbstractProcessor
Let’s have a look at the Processor API. Every Processor extends from AbstractProcessor as follows:




[自定义注解之编译时注解(RetentionPolicy.CLASS)（一）](https://blog.csdn.net/github_35180164/article/details/52121038)。



- new-->module--java lib, 新增两个annotations，Processer
- 修改build.gradle中plugin java-library为plugin java
- 在processer中的build.gradle 新增

```groovy
dependencies {
 
    implementation project(':annotations')
    implementation 'com.google.auto.service:auto-service:1.0-rc2'
    implementation 'com.squareup:javapoet:1.7.0'
}
```


`@AutoService(Processor.class)`，这是什么？这是一个其他注解处理器中引入的注解。AutoService注解处理器是Google开发的，用来生成META-INF/services/javax.annotation.processing.Processor文件的。

///////////////////////////////////////