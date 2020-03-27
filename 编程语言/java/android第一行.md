
[android studio developer](https://developer.android.google.cn/studio/command-line/logcat)

使用Android Studio模板节省开发时间， [参考](https://riggaroo.co.za/custom-file-templates-android-studio/)

- Live Templates ： “Android Studio -> settings -> Editor -> Live Templates”
- File and Code Templates ： 点击File -> New的时候，会在右侧显示一些快捷项，供我们快速的创建文件。“Android Studio -> settings -> Editor ->File and Code Templates”

[书籍附带sample源码](https://github.com/guolindev/booksource)

## Android的系统体系结构，

![示意图](image/Android系统体系结构.jpg)

- Linux Kernel：我们知道Android其实就是一个操作系统，其底层是基于Linux Kernel的，这一层主要完成的是操作系统所具有的功能，比如这一层有许多的驱动程序，正是通过这些驱动程序来驱动我们设备上的硬件设备的。

- Android Runtime：Android的运行环境，我们学过java的都知道，java程序的运行需要java的核心包的支持，然后通过JVM虚拟机来运行我们的应用程序，这里Android Runtime里的Core Libraries就相当于java的JDK，是运行android应用程序所需要的核心库，Dalvik Virtual Machine就相当于JVM，这时Google专为Android开发的运行android应用程序所需的虚拟机。

- Liberaries：这里面都是Android的库文件，例如我们访问SQLite数据库的库文件等等。

- Application Framework：应用程序的框架，这个是非常的重要的，相信Framework这个词大家都应该非常的熟悉了，我们学习Android也主要学的就是这一层，我们通过这些各种各样的框架来实现我们的Application。

- Application：这个就是我们开发的Application了。

Android一共有四种常用的组件：Activity、Service、ContentProvider、BroadcastReceiver

- Activity：Activity就是我们应用程序的界面，主要用来跟我们的用户进行交互的，比如一个文本框，按钮这些就是显示在Activity上的，并且可以接收数据，传递数据到后台，然后再显示出来。

- Service：Service我们是看不见的，对于绝大部分的数据的处理，业务的处理都是通过Service来完成的。

- ContentProvider：ContentProvider类似于我们的档案馆，里面存放了我们的各种数据，例如通讯录中的信息等，这个组件就是运行程序对其进行访问，然后得到里面的数据。

- BroadcastReceiver：BroadcastReceiver组件是广播接收器，主要是用来监听系统的各个行为的，例如当电量不足的时候会给我们发送一条广播信息。

## autostudio studio记录

使用启用gradle wrapper模式，可以通过“file/settings/build,execution,deployment/gradle ” 进行设置

一个android项目，在项目目录下的local.properties 文件中记录的是SDK路径。当SDK位置变化时应更新该文件

当打开一个资源文件时，在右上角有“code, split , design” 几种选项来观察。

需要查看API函数的功能和参数，在把鼠标放上去会提示，在Android studio ，通过"file/settings/editor/general/ "选择show quick documentation on mouse move




## 

androidManifest.xml 文件中,通过`intent-filter`注册活动

```xml
<manifest ...  package=..>
    <application ...>
        <activity android:name=".SecondActivity"></activity>
        
        <activity
            <!-- 这个“.”写法是基于上面节点已经定义过package包 -->
            android:name=".MainActivity"
            android:label="@string/app_name"
            android:theme="@style/AppTheme.NoActionBar">
            <intent-filter>
                <!--MAIN决定应用的入口Activity，也就是我们启动应用时首先显示哪一个Activity-->
                <action android:name="android.intent.action.MAIN" />
                <!--LAUNCHER表示activity应该被列入系统的启动器(launcher)(允许用户启动它)。Launcher是安卓系统中的桌面启动器，是桌面UI的统称-->
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
        </activity>
    </application>
</manifest>
```


## 日志logCat

在method外输入logt + TAB，会得到以当前类名自动生成的TAG，用于写log.i(..)等

```sh
$adb logcat --help

$ adb shell
# logcat
```

## 项目中资源

- 代码中访问资源： R.layout.activity_main
- XML中访问资源： @string/hello_word

## 活动activity


## Toast提醒

使用简单就是建立个对象，然后show

```java
   Toast.makeText(FirstActivity.this,
       "you click button", Toast.LENGTH_SHORT).show();
```                        

