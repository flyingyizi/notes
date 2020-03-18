


# 安装jdk

1. 设置环境变量: JAVA_HOME
2. 设置环境变量(后面有个.): CLASSPATH=.;%%JAVA_HOME%%\lib\dt.jar;%%JAVA_HOME%%\lib\tools.jar;  
3. 设置环境变量: 追加PATH=%%JAVA_HOME%%\bin;%%JAVA_HOME%%\jre\bin;  

[groovy教程](https://www.w3cschool.cn/groovy/)

[gradle教程](https://www.w3cschool.cn/gradle/)

# adb

1. 设置环境变量: ANDROID，变量值：C:\prog\Android\sdk\platform-tools(sdk路径)
2. 在Path末尾添加;%ANDROID%

# gradle

去看 <Android Gradle权威指南>

还有一份简单的[Android Gradle使用总结](https://www.cnblogs.com/zhaoyanjun/p/7603640.html)

任何一个 Gradle 构建都是由一个或多个 projects 组成。每个 project 都由多个 tasks 组成。每个 task 都代表了构建执行过程中的一个原子性操作。如编译，打包，生成 javadoc，发布到某个仓库等操作。

- 通过 `gradlew -?`, 查看有哪些命令可以使用。

- 通过`./gradlew tasks`查看所有可执行的Tasks

- 通过`./gradlew  help --task  <task>` 查看任务的具体信息，比如`./gradlew  help --task  build`，`./gradlew  help --task tasks`

## 配置gralde

设置 GRADLE_HOME 环境变量， 设置%GRADLE_HOME\bin% PATH 。运行`gradle -v` 查看输出是否正确验证安装成功。

加速仓库访问，在`~/init.gradle`中增加如下代理设置
```groovy
allprojects { 
    repositories {
        def REPOSITORY_URL = 'http://maven.aliyun.com/nexus/content/groups/public/'
        all { ArtifactRepository repo ->
            if(repo instanceof MavenArtifactRepository){
                def url = repo.url.toString()
                if (url.startsWith('https://repo.maven.org/maven2') || url.startsWith('https://jcenter.bintray.com/')) {
                    project.logger.lifecycle "Repository ${repo.url} replaced by $REPOSITORY_URL."
                    remove repo
                }
            }
        }
        maven {
            url REPOSITORY_URL
        }
    }  
} 
```


执行Gradle 命令的时候，会默认加载当前目录下的build.gradle 脚本文件。当然你也可以通过'- b' 参数指定想要加载执行的文件。

执行`gradle wrapper`命令（Gradle 提供了内置的Wrapper task） 帮助我们自动生成Wrapper 所需的目录文件。生成的文件如下：
```text
│  
│  gradlew
│  gradlew.bat
└─gradle
    └─wrapper
            gradle-wrapper.jar
            gradle-wrapper.properties
```

生成的wrapper通过gradlew 来调用。 gradlew命令使用方式完全同gradle

自定义Wrappe Task。 
前面我们讲了， gradle-wrapper.properties 是由Wrapper Task 生成的，那么我们是否可以自定义配置该Wr叩per task 来达到我们配置gradle-wrapper. properties 的目的呢？答案是肯定的。
在build.gradle 构建文件中录入如下脚本：
```groovy
task wrapper (type : Wrapper) {
gradleVersion = '4.4.1'
}
```

这样我们再执行`gradle wrapper` 的时候， 就会默认生成4.4.1 版本的wrapper 了，而不用使用--gradle-version 4.4.1 进行指定了。

## gradle 脚本基础

### setting文件

setting文件用于初始化以及工程树的配置。设置文件的默认名字是`settings.gradle` ，放在根工程目录下。
设置文件大多数的作用都是为了配置子工程。在Gradle 中多工程是通过工程树表示的，就相当于我们在Android Studio 看到的Project 和Modul e 概念一样。根工程相当于Android Studio 中的Project ，一个根工程可以有很多子工程， 也就是很多Module ，这样就和Android Studio 定义的Module 概念对应上了。 一个子工程只有在Settings 文件里配置了Gradle 才会识别，才会在构建的时候被包含进去

`settings.gradle`文件样例:
```gradle
rootProject.name = 'android-gradle-book-code' 
include ':example02'
project (':example02').projectDir =new File(rootDir, 'chapter01/example02')
```

### build文件

每个Project 都会有一个Build 文件，该文件是该Project 构建的入口，可以在这里针对该Project 进行配置，比如配置版本， 需要哪些插件， 依赖哪些库等。

### projects与tasks
在Gradle 中，可以有很多Project ，你可以定义创建一个P时ect 用于生成一个jar ，也可以定义另外一个Project 用于生成一个war 包，还可以定
义一个Project 用于发布上传你的war 等。其实一个P叫ect 就是在你的业务范围内，被你抽象出来的一个个独立的模块，你可以根据项目的情况抽象归类，最后这一个个的Pr句ect 组成了你的整个Gradle 构建。

一个Project 又包含很多个Task ，也就是说每个Project 是由多个Task 组成的。那么什么是Task 呢？ Task 就是一个操作， 一个原子性的操作，比如打个jar 包，复制一份文件，编译一次Java 代码， 上传一个jar 到Maven 中心库等

### 创建一个任务

```groovy
//方法1
task hello1 {
     doLast {
         println 'hello1'
     }
}
//方法2
tasks.create("hello2") {
    dependsOn "hello1"
     doLast {
         println 'hello2'
     }    
}
//方法3
def Task hello3 = task("hello3")
hello3.doLast {
    println "hello3"
}
```
### 任务依赖

见上面例子的dependsOn

### 自定义属性

```groovy
// 自定义一个Project 的属性
ext.age = 1

task hello {
    doLast {
        println "年龄是 $age"
    }
}
```

## 进阶 gradle任务（task）

通过下面的例子看下task的执行

```groovy
class CustomTask extends DefaultTask {
    @TaskAction
    def doSelf () {
        println 'task 自己本身执行 in doSelf'
    }
}
def Task hello33 = task("hello3", type:CustomTask)
hello33.doLast {
    println "hello3  dolast"
}
hello33.doFirst {
    println "hello3  dofirst"
}
```
执行的结果是：
```text
$.\gradlew hello3

> Task :hello3
hello3  dofirst
task 自己本身执行 in doSelf
hello3  dolast

BUILD SUCCESSFUL in 1s
1 actionable task: 1 executed
```

通过下面的`<task>.shouldRunAfter/mustRunAfter api`指定任务的执行顺序。 当然如果直接在命令行`'/gradlew  taskA  taskB taskC`也是可以的。

- taskB.shouldRunAfter(task.A)表示taskB 应该在task.A 执行之后执行，这里的应该而不是必须。所以有可能任务顺序并不会按预设的执行。
- taskB.mustRunAfter(task.A)表示taskB 必须在task.A 执行之后执行，这个规则就比较严格。

通过`<task>.enable api`对指定任务开启或关闭。

通过`<task>.onlyIf api`对指定任务是否enable进行决策，它接受一个闭包作为参数， 如果该闭包返回true 则该任务执行，否则跳过。

下面是演示的例子
```groovy
task  ex1 { 
    doLast {    println "todo ex1" }
     
}
task  ex2  {
    doLast {    println "todo ex2" }
}
task total {
    dependsOn ex1 ,ex2
}
ex1.onlyIf {
    false
}
ex2.onlyIf {
    true
}
```
执行结果就是
```text
c:\home\android-gradle-book-code>.\gradlew total

> Task :ex2
todo ex2

BUILD SUCCESSFUL in 1s
1 actionable task: 1 executed
c:\home\android-gradle-book-code>      
```

## gradle 插件基础

### apply插件

- 应用二进制插件
    ```groovy
    //以下三种写法等效
    apply plugin : 'java'
    apply plugin ：JavaPlugin
    apply plugin ：org.gradle.api.plugins.JavaPlugin

    // 下面是新的语法
    plugins {
        id 'java'
    }
    ```

- 应用第三方插件

    常规写法如下
    ```groovy
    buildscript {
    repositories {
        jcenter()
    }
    dependencies { classpath 'com.android.tools.build:gradle :l.5.0'  }
    }
    //基于上面已经配置了所需依赖，下面就可以应用插件了
    apply plugin : 'com.android.application'
    ```

    如果是托管在“https://plugins.gradle.org/ ”的第三方插件，并且采用新的plugins{}语法，那可以直接使用plugins,省掉在buildscript配置依赖

- 应用脚本插件
    ```groovy
    apply from : '<文件名>'
    ```groovy

### 多项目构建

[Compile gradle project with another project as a dependency](https://looksok.wordpress.com/2014/07/12/compile-gradle-project-with-another-project-as-a-dependency/)

- 场景1：Your project is a root project and dependency is under its root
    ```text
    Project
        |--build.gradle
        |--settings.gradle
        |--Dependency
            |--build.gradle
    ```  
    step1: Then to add Dependency to Project, you need to have Project/settings.gradle content like this:
    ```groovy
    include ':Dependency'
    ```
    step2: and in a Project/build.gradle dependencies section you need to compile the dependent project by adding:
    ```groovy
    dependencies {
       compile project(':Dependency')
    }
    ```
- 场景2：You have two independent projects and you need to use one of them as a dependency
    ```text
    Project
       |--build.gradle
       |--settings.gradle    
    Dependency
       |--build.gradle
    ```
    step1: To add Dependency to the Project, you need to include it, and show Dependency path manually. So the Project/settings.gradle content should be like this:
    ```groovy
    include ':Dependency'
    project(':Dependency').projectDir = new File(settingsDir, '../Dependency')
    ```
    step2: and in a Project/build.gradle dependencies section you need to compile the dependent project by adding:
    ```groovy
    dependencies {
       compile project(':Dependency')
    }
    ```
    Notice that tis is build.gradle is exactly the same as in previous section. 

根据上面配置依赖后，我们就可以在Project 项目中随意使用Dependency项目中的类了，就像我们在引用一个Jar 包一样：

### spring-boot gradle插件说明

[Gradle Spring Boot 配置](http://docs.spring.io/spring-boot/docs/current/reference/htmlsingle/%23getting-started-gradle-installation)

[通过 Spring Initializr 来创建一个空的spring项目](http://start.spring.io/)

## android gradle高级：使用共享库

Android的所有包（如andorid.app，android.content，android.view和android.widget等）都在应用程序自动链接的默认类库中。但是，有些包是在于独立的类库中，它们不能被自动的链接到应用程序包中，要查看这些包的文档，以确定要把哪些类库包含到包的代码中。

在src\main\AndroidManifest.xml中，可以指定要使用的共享库
```xml
<application
    ...
	...>
    	<uses-library
        	android:name="..."
        	android:required="true" />

</application>
```

## android gradle高级：使用adb

位置
```groovy
android {
    adbOptions {
       timeOutInMs = 5*1000   //设置执行adb命令的超时时间，原始单位毫秒
       installOptions '-r', '-s'   //设置adb install安装设置项
    }
}
```

  - l：锁定该应用程序
  - r：替换已存在的应用程序，即强制安装
  - t：允许测试包
  - s：把应用程序安装到SD卡上
  - d：允许进行降级安装，即安装版本比手机自带的低
  - g：为该应用授予所有运行时的权限

