

Android Gradle 插件，包括：

- App 插件id: com.android.application
- Library 插件id: com.android.library
- Test 插件id: com.android.test

对应app应用工程（生成一个可运行的apk 应用），对应libary库工程（生成AAR包给其他的App 工程公用，就和我们的Jar 一样），对应Test 测试工程，用于对App 工程或者Library 库工程进行单元测试。

 `Android Gradle 插件`作为第三方插件， 需要先通过buildscript配置依赖classpath ， 这样当我们应用插件的时候， Gradle 系统才能找到它们：
例如：
```groovy
buildscript {    
    repositories {
        google()
        jcenter()        
    }
    dependencies {
        classpath 'com.android.tools.build:gradle:3.6.1'        

        // NOTE: Do not place your application dependencies here; they belong
        // in the individual module build.gradle files
    }
}
```
Android Gradle 工程的配置，都是在android{}中，这是唯一的一个入口。通过它，可以对Android Gradle 工程进行自定义的配置,其具体实现是com.android.build.gradle.AppExtension,是Project 的一个扩展

## defaultConfig默认配置

```groovy
android {
    compileSdkVersion 29
    buildToolsVersion "29.0.0"

    //defaultConfig 配置块配置说明：
    defaultConfig {
        // applicationId：是ProductFlavor 的一个属性，用于指定生成的App 的包名，默认情况下是null 。这时候在构建的时候,会从我们的AndroidManifest.xml 文件中读取，也就是我们在AndroidManifest.xmJ 文件中配置的manifest 标签的package 属性值。
        applicationId "com.example.myapplication"

        //minSdkVersion: 指定我们的App 最低支持的Android 操作系统版本. 这个字符串不是我们SDK API LEVEL 的字符串形式， 而是Code Name ，也就是每个Android SDK 的代号
        minSdkVersion 16
        //targetSdkVersion 用于配置我们基于哪个Andro i d SDK 开发，
        targetSdkVersion 29
        //versionCode: 用于配置Android App 的内部版本号，是一个整数值，通常用于版本的升级。没有配置的时候从AndroidManifest.xml让文件中读取，建议配置。 例如 <manifest package="com.example.myapplication" android:versionCode="2" android:versionName="1.1">
        versionCode 1
        //versionName:  和versionCode配合 一个是外部使用，一个是内部使用
        versionName "1.0"

        testInstrumentationRunner "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android-optimize.txt'), 'proguard-rules.pro'
        }
    }

}
```

## 配置签名信息

默认情况下， debug 模式的签名己经被配置好了，使用的就是Android SDK 自动生成的debug 证书。它一般位于`$HOME/.android/debug.keystore` ，其Key 和密码都是己知的， 一般情况下我们不需要单独配置debug 模式的签名信息。

详细说明参见另一份“[android签名说明文档](android%20签名.md)”文件。

例如：
```groovy
android {
    signingConfigs {
        debug {
            keyAlias 'key'
            keyPassword '123456'
            storeFile file('E:/key/keys.jks')
            storePassword '123456'
        }
        release {
            keyAlias 'key'
            keyPassword '123456'
            storeFile file('E:/key/keys.jks')
            storePassword '123456'
        }
    }

    buildTypes {
        debug {
            debuggable true
            zipAlignEnabled true
            debuggable true
            minifyEnabled false //是否混淆
            shrinkResources false //是否去除无效的资源文件
            proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.pro'           
            signingConfig signingConfigs.debug            
        }
        release {
            zipAlignEnabled true
            debuggable false
            minifyEnabled true //是否混淆
            shrinkResources true //是否去除无效的资源文件
            proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.pro'          
            signingConfig signingConfigs.release
           
        }
    }
    ...
}
```

