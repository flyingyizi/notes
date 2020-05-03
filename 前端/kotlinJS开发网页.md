
# 基础

kotlin版本必须不小于Kotlin 1.3.60，因为只有这个版本才能支撑我们需要的debug。

[kotlin javascript](https://www.kotlincn.net/docs/tutorials/javascript/setting-up.html)

## 向导生成框架

通过IDEA向导： “new project-->gradle--> kotlin/js for browser” 生成 网页项目。

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

## 持续集成

方式1：在gradle命令行中采用

```sh
$./gradlew run --continuous
```

方式2：如果是在idea中，通过“Run/Debug Configurations dialog”增加“--continuous”，即：add configuration --> add new configuration --> gradle --> with xx project, with `<run>` tasks, with `<--continuous>` arguments.

### 在浏览器debug

在上面持续集成状态下，在browser dev-tools source tab 下可以下断点，当page reload时看到debug效果

## 使用NPM包

[Using packages from npm](https://www.kotlincn.net/docs/tutorials/javascript/using-packages-from-npm.html)

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
