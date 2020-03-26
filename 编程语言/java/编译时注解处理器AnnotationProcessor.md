

编译时注解处理(annotation processor),以java code (or compiled byte code)中为输入，根据他们中的注解进行处理并生成输出，比如输出java文件。

注解处理(annotation processor)是在自己独立的JVM运行的。

常用apt：

- `@AutoService(Processor.class)`，这是一个其他注解处理器中引入的注解。AutoService注解处理器是Google开发的，用来生成META-INF/services/javax.annotation.processing.Processor文件的。

几个参考链接：

- [Debugging an Annotation Processor in your project](https://blog.xmartlabs.com/2016/03/28/Debugging-an-Annotator-Processor-in-your-project/)

- [ANNOTATION PROCESSING 101](http://hannesdorfmann.com/annotation-processing/annotationprocessing101)

## gradle/idea 处理apt问题记录

在编码过程中，识别出识别出一些问题，例如google AtuoService processor无法起作用生成`resources/META-INF/services/
javax.annotation.processing.Processor` 文件。 例如自己写的processor无法起作用。

找到解决方案[annotation-processor-in-intellij-and-gradle](https://stackoverflow.com/questions/42441844/annotation-processor-in-intellij-and-gradle) 中提到的[gavenkoas answer](https://stackoverflow.com/a/54611475/1041641)，如其中描述的，采用新版本gradle(This is becoming a reality with Gradle 5.2 and IntelliJ IDEA 2019.1.)，使用其中的`annotationProcessor`能力方式最简单。

例如下面的目录是实验代码目录，其中root project是helloWord(`helloWord/build.gradle`), annotions(helloWord/annotions/build.gradle)与processors(helloWord/processors/build.gradle)是root project的两个依赖模块。
```text
└─helloWord
    │  build.gradle
    │  gradlew
    │  gradlew.bat
    │  settings.gradle
    │
    ├─annotions
    │  │  build.gradle
    │  │  gradlew
    │  │  gradlew.bat
    │  │  settings.gradle
    │  │
    │  ├─gradle
    │  │  └─wrapper
    │  └─src
    │      ├─main
    │      │  ├─java
    │      │  └─resources
    │
    ├─gradle
    │  └─wrapper
    │
    ├─processors
    │  │  build.gradle
    │  │  gradlew
    │  │  gradlew.bat
    │  │  settings.gradle
    │  │
    │  ├─gradle
    │  │  └─wrapper
    │  │
    │  └─src
    │      ├─main
    │      │  └─java
    └─src
        ├─main
        │  └─java

```

在helloWord(`helloWord/settings.gradle`)，其中标注：
```groovy
rootProject.name = 'helloWord'
    ...
include  ':processors'
include  ':annotions'
```

在helloWord(`helloWord/build.gradle`)，其中标注：
```groovy
dependencies {
    ...
    //因为我们实验的仅仅是编译时注解与编译时apt
    compileOnly project(':annotions')
    compileOnly project(':processors')
    annotationProcessor project(':processors')
}
```

在processors(`helloWord/processors/build.gradle`),其中标注：
```groovy
dependencies {
    ...
    implementation project(':annotions')
    implementation 'com.google.auto.service:auto-service:1.0-rc6'
    annotationProcessor 'com.google.auto.service:auto-service:1.0-rc6'
}

```
在processors(`helloWord/processors/settings.gradle`),其中标注：
```groovy
rootProject.name = 'processors'
    ...
include ':annotions'
project(':annotions').projectDir = new File(settingsDir, '../annotions')
```

## Processor API

AbstractProcessor
Let’s have a look at the Processor API. Every Processor extends from AbstractProcessor as follows:


## javapoet使用

JavaPoet 是一个用来生成 .java源文件的Java API。

[JavaPoet的基本使用](https://blog.csdn.net/crazy1235/article/details/51876192)

///////////////////////////////////////