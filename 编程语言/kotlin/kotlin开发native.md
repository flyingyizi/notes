
[Introduction to Kotlin/Native](https://play.kotlinlang.org/hands-on/Introduction%20to%20Kotlin%20Native/02_CreatingAProject)


## 使用idea

idea中:
- file -->new -->project -->Kotlin--> Native | Gradle
- Click Next and tick use auto-import

下面是idea生成的两个build.gradle例子

例1：build.gradle for mingw

```groovy
plugins {
    id 'org.jetbrains.kotlin.multiplatform' version '1.3.72'
}
repositories {
    mavenCentral()
}
kotlin {
    // For ARM, should be changed to iosArm32 or iosArm64
    // For Linux, should be changed to e.g. linuxX64
    // For MacOS, should be changed to e.g. macosX64
    // For Windows, should be changed to e.g. mingwX64
    mingwX64("mingw") {
        binaries {
            executable {
                // Change to specify fully qualified name of your application's entry point:
               entryPoint = 'sample.main'
                // Specify command-line arguments, if necessary:
                runTask?.args('')
            }
        }
    }
    sourceSets {
        // Note: To enable common source sets please comment out 'kotlin.import.noCommonSourceSets' property
        // in gradle.properties file and re-import your project in IDE.
        mingwMain {
        }
        mingwTest {
        }
    }
}

// Use the following Gradle tasks to run your application:
// :runReleaseExecutableMingw - without debug symbols
// :runDebugExecutableMingw - with debug symbols
```

例子2：gradle.build for mac

```groovy
plugins {
    id 'kotlin-multiplatform' version '1.3.31'
}
repositories {
    mavenCentral()
}
kotlin {
    // For ARM, should be changed to iosArm32 or iosArm64
    // For Linux, should be changed to e.g. linuxX64
    // For MacOS, should be changed to e.g. macosX64
    // For Windows, should be changed to e.g. mingwX64
    macosX64("macos") {
        binaries {
            executable {
                // Change to specify fully qualified name of your application's entry point:
               entryPoint = 'com.jetbrains.handson.http.main'
                // Specify command-line arguments, if necessary:
                runTask?.args('')
            }
        }
    }
    sourceSets {
        // Note: To enable common source sets please comment out 'kotlin.import.noCommonSourceSets' property
        // in gradle.properties file and re-import your project in IDE.
        macosMain {
        }
        macosTest {
        }
    }
}

// Use the following Gradle tasks to run your application:
// :runReleaseExecutableMacos - without debug symbols
// :runDebugExecutableMacos - with debug symbols
```


## 使用命令行编译器的 Hello Kotlin/Native

不推荐

首先需要下载[kotlinc native](https://github.com/JetBrains/kotlin/releases/download/v1.3.72/kotlin-native-windows-1.3.72.zip)

参见 [Kotlin 编译器选项](https://www.kotlincn.net/docs/reference/compiler-reference.html)中native选项

参见 [使用命令行编译器的 Hello Kotlin/Native](https://www.kotlincn.net/docs/tutorials/native/using-command-line-compiler.html)