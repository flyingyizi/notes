
# 密钥库生成

基于android studio开发环境，证书生成有几个途径：

- 手工命令行生成
- androidStudio界面方式生成签名文件

## 手工命令行方式生成密钥库

%JAVA_HOME%\bin\keytool.exe

```sh
keytool -genkey -alias myApp.keystore -keyalg RSA -validity 10000 -keystore myApp.keystore

#其中：
#-alias myApp.keystore  是别名,代表keystore中的一个key
#-validity 10000  是期限 以年为单位
#-keystore myApp.keystore 生成签名文件的名字
```

执行结果示例：
```sh
C:/MyApplication>keytool -genkey -alias myApp.keystore -keyalg RSA -validity 10000 -keystore myApp.keystore
输入密钥库口令:
密钥库口令太短 - 至少必须为 6 个字符
输入密钥库口令:
再次输入新口令:
您的名字与姓氏是什么?
  [Unknown]:
您的组织单位名称是什么?
  [Unknown]:
您的组织名称是什么?
  [Unknown]:
您所在的城市或区域名称是什么?
  [Unknown]:
您所在的省/市/自治区名称是什么?
  [Unknown]:
该单位的双字母国家/地区代码是什么?
  [Unknown]:  CN
CN=Unknown, OU=Unknown, O=Unknown, L=Unknown, ST=Unknown, C=CN是否正确?
  [否]:  是

输入 <myApp.keystore> 的密钥口令
        (如果和密钥库口令相同, 按回车):

C:/MyApplication>dir
2020/03/18  12:18    <DIR>          .
2020/03/18  12:18    <DIR>          ..
2020/03/18  12:18             2,241 myApp.keystore
C:/MyApplication>
```
依次输入口令、确认口令并记住，这个口令会在签名apk时用到。

最新版AS会有类似下面的警告提示,即将jks格式建议变换为PKCS12格式：

```text
Warning:
JKS 密钥库使用专用格式。建议使用 "keytool -importkeystore -srckeystore C:\Users\atmel\AndroidStudioProjects\recyclerViewDemo\myApp.keystore.jks -destkeystore C:\Users\atmel\AndroidStudioProjects\recyclerViewDemo\myApp.keystore.jks -deststoretype pkcs12" 迁移到行业标准格式 PKCS12。
```

## androidStudio界面方式生成密钥库

as菜单栏 ：bulid-->“Generate Signed APK”--> "Create New..."

# 使用密钥库进行apk签名

## 配置gradle让APP自动签名

as菜单栏 ：file-->“project  structure”--> "modules "--> "signing config"

然后自动更新app的build.gradle，新增类似下面内容
```groovy
android {
    signingConfigs {
        releaseSigning {
            storeFile file('C:\\Users\\tu_xu\\Desktop\\myApp.keystore')
            storePassword 'android'
            keyAlias = 'myApp.keystore'
            keyPassword 'android'
        }
    }
    ...
}    
```
然后，手动在buildType配置块中添加签名配置，或《as菜单栏 ：file-->“project  structure”--> "build types"》里面，对“signing config”选择对应的cfg。效果是一样的。
```groovy
    buildTypes {
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.pro'
            //签名配置，如果把下面注释掉，打出的包是未签名的，如果放开注释那么是签名的包
            //signingConfig signingConfigs.releaseSigning
        }
    }
```

然后进入当前项目目录下面运行(aR 是简写，assembleRelease )：
```sh
$ ./gradlew aR
#结果在类似下面的地方C:\Users\tu_xu\AndroidStudioProjects\MyApplication\app\build\outputs\apk\release\app-release.apk
```

生成未签名的apk就是把 signingConfig 那行注释放开就行了其余的一样

## 手动APP签名
在下面例子中，`C:\Users\tu_xu\Desktop\myApp.keystore`文件是前面生成的前面文件。 app-release-unsigned.apk是androidStudio生成的一个未签名APK。 “myApp.keystore”是对APP进行签名的key（这个key在生成签名文件时录入的）
```sh
$jarsigner -verbose -keystore C:\Users\tu_xu\Desktop\myApp.keystore  -signedjar result_singed.apk  C:\Users\tu_xu\AndroidStudioProjects\MyApplication\app\build\outputs\apk\release\app-release-unsigned.apk  myApp.keystore
```

## 签名APP 中的 MANIFEST.MF xx.SF和 xx.RSA文件

对比签名App与未签名App，对apk中META-INF目录，签名app会多出xx.SF和 xx.RSA两个文件。同时MANIFEST.MF文件中会多出下面的类似多条记录：
```text
Name: res/color/material_on_surface_disabled.xml
SHA-256-Digest: uDclNBC5vr2p6GrFgezJvF/aLDTikc7LJUSbNnc71NU=

Name: res/layout/test_toolbar.xml
SHA-256-Digest: Hp8o0Ha8te/rLUEgkPeyCFOekmPuqt/eV1aU1bRsbyY=
...
```
这几个文件的解释参见 [Android中的签名和签名文件的生成过程](https://blog.csdn.net/niyingxunzong/article/details/79977580)


