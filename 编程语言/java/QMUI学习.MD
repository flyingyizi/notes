
## 准备动作

[官方](https://qmuiteam.com/android/get-started/), [github](https://github.com/Tencent/QMUI_Android) , AND [WIKI](https://github.com/Tencent/QMUI_Android/wiki)

1. 引入库
最新的库会上传到 JCenter 仓库上，请确保配置了 JCenter 仓库源，然后直接引用：

- qmui库，与ach库
```groovy
def qmui_arch_version = '2.0.0-alpha06'

// QMUI 库
implementation "com.qmuiteam:qmui:$qmui_arch_version"

// arch库
implementation "com.qmuiteam:arch:$qmui_arch_version"
annotationProcessor "com.qmuiteam:arch-compiler:$qmui_arch_version" // use annotationProcessor if java
```

至此，QMUI 已被引入项目中。

2. 配置主题
把项目的 theme 的 parent 指向 QMUI.Compat ，至此，QMUI 可以正常工作。修改路径是“res/values/styles.xml/”中，即：

```xml
    <!--
    <style name="AppTheme" parent="Theme.AppCompat.Light.DarkActionBar">
    -->
    <style name="AppTheme" parent="QMUI.Compat">
```

3. 初始化arch库

- 在 Application 里初始化

新建一个继承Application的子类MyApplication并改写onCreate
```java
@Override fun onCreate() {
super.onCreate()
    QMUISwipeBackActivityManager.init(this)
}
```

对应的在andoid Manifest文件中将子类MyApplication注册上去

```xml
<manifest ...>

    <application
        android:name=".MyApplication"
        ...
    </application>

```

然后就可以使用 arch 库提供的 QMUIFragment、QMUIFragmentActivity、QMUIActivity 来作为基础类构建自己的界面了。

