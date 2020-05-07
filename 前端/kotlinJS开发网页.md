

TODO[How to call Kotlin/JS functions from regular Javascript?](https://stackoverflow.com/questions/60183300/how-to-call-kotlin-js-functions-from-regular-javascript), 类似kotlin.js 没有打包的情况怎么解决，还要看。

kotlinjs协程
implementation 'org.jetbrains.kotlinx:kotlinx-coroutines-core-js:1.1.1'


# 基础

kotlin版本必须不小于Kotlin 1.3.60，因为只有这个版本才能支撑我们需要的debug。

[Setting up a Kotlin/JS project](https://kotlinlang.org/docs/reference/js-project-setup.html)


## 向导生成框架

通过IDEA向导： “new project-->gradle--> kotlin/js for browser” 生成 网页项目。 注意`Be sure to clear the Java checkbox`

基于该向导生成的项目，

- 增加依赖：

```groovy
implementation  "org.jetbrains.kotlinx:kotlinx-html-js:0.7.1"
```

- 自动生成资源resource/xxx.html说明

```html
...
</body>
<!--因为要操纵tag，所以放到body后 -->
<script src="xxx.js"></script>
</html>
```

- 基本写法演示

```kotlin
import kotlin.browser.document
import kotlinx.html.*
import kotlinx.html.dom.create
import kotlinx.html.js.onClickFunction
import org.w3c.dom.HTMLInputElement

fun main() {
    document.write("Hello, world!")

    var root = document.getElementById("root")
    // create a div tag which contains xxx
    var div = document.create.div {
        h1 {
            + "hello javascript"
        }
        button(classes = "btn") {
            + "click me"
            onClickFunction = { println("clicked!")}
        }
    }
    root!!.appendChild(div)

}
```

## dynamic Type

关键字`dynamic`用于与无类型的语言（e.g. javascript）对接。它的作用是关闭Kotlin的类型检查:

- a value of this type can be assigned to any variable or passed anywhere as a parameter;
- any value can be assigned to a variable of type dynamic or passed to a function that takes dynamic as a parameter;
- null-checks are disabled for such values.

例子

```kotlin
val dyn: dynamic = ...
fun jsFun(p: dynamic): dynamic
```

## todo kotlin调用javascript

[External declarations for Kotlin/JS](https://github.com/Kotlin/js-externals)

### 编写 Inline JavaScript

使用[js("…") fun](https://kotlinlang.org/api/latest/jvm/stdlib/kotlin.js/js.html)在kotlin代码中写js代码，例如:

```kotlin
fun jsTypeOf(o: Any): String {
    return js("typeof o")
}
```

js 函数的参数必须是 string constant，因此下面的代码不正确：

```kotlin
fun jsTypeOf(o: Any): String {
    return js(getTypeof() + " o") // error reported here
}
fun getTypeof() = "typeof"
```

### 使用NPM包

[Using packages from npm](https://www.kotlincn.net/docs/tutorials/javascript/using-packages-from-npm.html)


## 持续集成

方式1：在gradle命令行中采用

```sh
$./gradlew run --continuous
```

方式2：如果是在idea中，通过“Run/Debug Configurations dialog”增加“--continuous”，即：add configuration --> add new configuration --> gradle --> with xx project, with `<run>` tasks, with `<--continuous>` arguments.

### 在浏览器debug

在上面持续集成状态下，在browser dev-tools source tab 下可以下断点，当page reload时看到debug效果


## 单元测试

依赖

```groovy
dependencies {
    testImplementation "org.jetbrains.kotlin:kotlin-test-js"
    testImplementation 'junit:junit:4.12'
}
```

### 在browser中执行单元测试

```sh
$./gradlew browserTest
```



# Typesafe HTML DSL.

注意“kotlin.js.*”是不用自己导入，默认已经导入。

通过[kotlinx.html](http://www.github.com/kotlin/kotlinx.html)和Kotlin DSL构造DOM元素

依赖：

```groovy
plugins {
    id 'org.jetbrains.kotlin.js' version '1.3.72'
}
repositories {
    // ...
    jcenter()
}

dependencies {
    implementation(kotlin("stdlib-js"))
    implementation("org.jetbrains.kotlinx:kotlinx-html-js:0.7.1")
    // ...
}
```

该节内容来自[kotlinx.html document](https://github.com/kotlin/kotlinx.html/wiki)

## DSL usage

类似HTMLInputElement，HTMLBodyElement，HTMLDivElement...定义见
[web API](https://developer.mozilla.org/en/docs/Web/API)

```kotlin
import org.w3c.dom.HTMLInputElement
val email = document.getElementById("email") as HTMLInputElement;
email.value = "hadi@jetbrains.com"
```    
