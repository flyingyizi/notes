
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

完全手动新增一个activity，涉及以下：

   - 创建类：在App对应的package下，通过"new/activity/empty activity"方式，不勾选generate layout file与launcher activity选项。生成activity class文件。
   - 创建布局：在“res/layout” 下面新增对应该activity的layout文件
   - 加载布局：比如在类的onCreate方法中新增`setContentView(R.layout.资源id);`
   - 注册：在“AndroidManifest.xml”里面新增activity注册类似<activity android:name="..."></activity>

### 在activity中使用Toast提醒

使用简单就是建立个对象，然后show

   ```java
    Toast.makeText(FirstActivity.this,
        "you click button", Toast.LENGTH_SHORT).show();
   ```

### 在activity中使用MENU

涉及如下：

  - 创建菜单资源： 在“res/menu”下面新增菜单资源，在这个资源中重点要建立菜单项item
  - 创建加载菜单：通过重写对应activity的onCreateOptionsMenu方法

    ```java
        @Override
        public boolean onCreateOptionsMenu(Menu menu) {
            getMenuInflater().inflate(R.menu.main,menu);
            return  true;
            //return super.onCreateOptionsMenu(menu);
        }
    ```

  - 使用菜单： 通过重新对应activity的onOptionsItemSelected

    ```java
        @Override
        public boolean onOptionsItemSelected(@NonNull MenuItem item) {
            switch (item.getItemId()) {
                case R.id.add_item:
                ...
            }
            return true;
            //return super.onOptionsItemSelected(item);
    }
    ```

### 在activity中使用button

涉及如下：

   - 在对应activity layout中添加button

   ```xml
    <LinearLayout ...>
        <Button
            android:id="@+id/start_normal_activity"
            android:layout_width="match_parent"
            android:layout_height="wrap_content"
            android:text="start normal activity"/>
    ...
    </LinearLayout>
   ```

   - 在activity中为button注册处理程序

   ```java
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button startDialogActivity = (Button) findViewById(R.id.start_dialog_activity);
        startDialogActivity.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, DialogActivity.class);
                startActivity(intent);
            }
        });

      }
    }
   ```



### activity生命周期

![android活动生命周期](image/android活动生命周期.png)

activity生命周期有四种状态:

- 1.运行状态: 当Activity位于栈顶时，此时正好处于屏幕最前方，此时处于运行状态；

- 2.暂停状态: 当Activity失去了焦点但仍然对用于可见（如栈顶的Activity是透明的或者栈顶Activity并不是铺满整个手机屏幕），此时处于暂停状态；

- 3.停止状态: 当Activity被其他Activity完全遮挡，此时此Activity对用户不可见，此时处于停止状态；

- 4.销毁状态: 当Activity由于人为或系统原因（如低内存等）被销毁，此时处于销毁状态；

### activity临时数据存放 

类似activity回收后再现场景，activity中临时数据与状态的存放获取。典型代码见下面。 另外，也有根据这个原理提供了[saveState annoation processor](https://github.com/PrototypeZ/SaveState)可以参考使用。

屏幕切换时原来的activity被系统回收了，又重新创建了一个新的activity，时机：
onPause -> onSaveInstanceState -> onStop -> onDestroy -> onCreate -> onStart -> onRestoreInstanceState -> onResume

按HOME键返回桌面，又马上点击应用图标回到原来页面时，activity生命周期如下：
onPause -> onSaveInstanceState -> onStop -> onRestart -> onStart -> onResume
因为activity没有被系统回收，因此onRestoreInstanceState没有被调用。


```java
static final String STATE_SCORE = "playerScore";
// ...
@Override
public void onSaveInstanceState(Bundle savedInstanceState) {
    // Save the user's current game state
    savedInstanceState.putInt(STATE_SCORE, mCurrentScore);
    ...
     // Always call the superclass so it can save the view hierarchy state
    super.onSaveInstanceState(savedInstanceState);
}
```

与

```java
public void onRestoreInstanceState(Bundle savedInstanceState) {
    // Always call the superclass so it can restore the view hierarchy
    super.onRestoreInstanceState(savedInstanceState);

    // Restore state members from saved instance
    mCurrentScore = savedInstanceState.getInt(STATE_SCORE);
    ...
}
```

### activity实践

- 一般activity是“AppCompatActivity”，可以考虑中间再插一层BaseActivity类，中新插的一层,从而增加运行时日志中的信息

    ```java
     @Override
     protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d("BaseActivity",getClass().getSimpleName());
        ...
     }   
    ```

- 随时退出程序

    采用单独容器存放所有activity，在中间新插入的BaseActivity类，维护add/remove。

    最后在需要完全退出程序的地方，调用该容器的finishAll(//xx.finish();)

- 启动活动

涉及通过intent启动活动的场景（比如A 拉起B），建议统一由B实现个类似

```java
public static void actionStart(Context c, String data1, String data2) {
    Intent intent = new Intent(c, <B_CLASS>.class);
    c.startActivity(intent);
}    
```

## intent

每个intent只能指定一个action，但可以指定多个category

使用方式1：使用显示intent

```java
   Intent intent = new Intent(FirstActivity.this,SecondActivity.class);
   startActivity(intent);
```

使用方式2，使用隐式intent

这种隐式方式是通过指定抽象的category与action，然后交由系统去分析该intent，并找出合适的activity去启动

如：
   - 注册activity可以响应的action与category

    ```xml
    <activity android:name=".SecondActivity">
        <intent-filter>
            <action android:name="xx.ACTION_START" />
            <category android:name="android.intent.category.DEFAULT" />
        </intent-filter>
    </activity>
    ```
   - 调用
    ```java
    Intent intent = new Intent("xx.ACTION_START");
    startActivity(intent);
    ```

## broadcast 广播

android中的广播有两种类型

- normal broadcast：标准广播
- ordered broadcast：有序广告。这种广播是先到优先级高的，再到优先级低的广播接收器。其中广播接收器可以控制广播是否继续往下传




# android UI


## 常用控件

TextView, EditText, ImageView, ProgressBar, ProgressDialog

### Button

- 首先在资源文件中需要定义

    ```xml
    <LinearLayout ...>

        <Button
            android:id="@+id/start_normal_activity"
            android:layout_width="match_parent"
            android:layout_height="wrap_content"
            android:text="start normal activity"/>
        ....
    </LinearLayout>
    ```

- 然后在需要使用的地方

    ```java
    Button startDialogActivity = (Button) findViewById(R.id.start_dialog_activity);
    startDialogActivity.setOnClickListener(new View.OnClickListener(){
        ...
    }
    ```

### AlterDialog

这个不需要资源文件，直接是类似下面的方式

```java
AlterDialog .Builder dialog = new AlterDialog .Builder(MainActivity.this);
dialog.setTitle("this is dialog");
...
dialog.show();
```

## 重要控件之：ListView控件

在其中有个挺重要的知识点“适配器Adapter”，[android中Adapter适配器的讲解](https://www.cnblogs.com/Jeely/p/11059336.html)

BaseAdapter，ArrayAdapter，SimpleAdapter是几个常用的适配器

### ListView简单使用方式：

- 在activity layout中添加"<ListView .../>"
- 在对应activity 的onCreate()中, 为该listView 设置Adapter（即建立数据与listview之间的关系），如下面代码

   ```java
    private String[] data = {"apple","banana","grape"};

        ListView listView = (ListView)findViewById(R.id.list_view);
        ArrayAdapter<String>  adapter = new ArrayAdapter<String>(MainActivity.this,
                //系统ITEM样式：单独的一行文本框
                android.R.layout.simple_list_item_1,data);

        listView.setAdapter(adapter);
   ```

### Listview定制使用方式:

举例是每行文字+图片

- 新建个每行文字加图片的自定义布局fruit_layout，里面包括`<ImageView/>与<TextView/>`
- 新建一个自定义的适配器`class FruitAdapter extends ArrayAdapter<Fruit>`, 其中Fruit是一个有name/image属性的结构体。这个适配体需要实现构造函数，以及`View getView(...)`。其中在getView中需要将数据通过layoutInflater将当前项实例加载进对应布局中去,例如

   ```java
    public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {
        Fruit fruit=getItem(position);
        View view =LayoutInflater.from( getContext())
                .inflate(resourceId, parent, false);

        ImageView fruitImage = (ImageView) view.findViewById(R.id.fruit_image);
        TextView  fruitName  = (TextView)  view.findViewById(R.id.fruit_name);

        fruitImage.setImageResource(fruit.getImageId());
        fruitName.setText(fruit.getName());

        return  view;
    }
   ```
- 最后，在对应activity 的onCreate()中, 为该listView 设置自定义的Adapter（即建立数据与listview之间的关系, listView.setAdapter(...)）

### 点击

   ```java
    protected void onCreate(Bundle savedInstanceState) {
        ...
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                Fruit fruit = fruitList.get(position);
            }
        });
   ```

## TODO重要控件之：RecyclerView控件

这个控件目标是取代ListView. 它在support库中。因此首先在gradle.build中需要添加

```groovy
dependencies {
    ...
    implementation 'com.android.support:recyclerview-v7:24.2.1'
```

- 先在对应的main activity layout中增加RecyclerView控件`<RecyclerView/>`. 
   
   ```xml
    <?xml version="1.0" encoding="utf-8"?>
    <LinearLayout xmlns:android="http://schemas.android.com/apk/res/android"
        android:layout_width="match_parent"
        android:layout_height="match_parent">

        <androidx.recyclerview.widget.RecyclerView
            android:id="@+id/recycler_view"
            android:layout_width="match_parent"
            android:layout_height="match_parent"

    </LinearLayout>
   ```

- 新建FruitAdapter`class FruitAdapter extends RecyclerView.Adapter<FruitAdapter.ViewHolder>`

待续

RecyclerViewTest



## 布局

android中由四种基本布局：LinearLayout， RelativeLayout， 

所有控件都有下面两个属性：

- match_parent：表示让当前控件的大小和父布局的大小一样，也就是由父布局来决定当前控件的大小

- wrap_content：表示让当前的控件大小能够刚好包含里面的内容，也就是由控件内容决定当前控件的大小

在“android:layout_width”与“android:layout_weight”同时并存情况下，由weight控制。

android:layout_weight：系统先将xxx的所有控件的该属性值相加得到一个总值。然后每个控件所在比例的大小就是该控件layout_weight值与总值的比例。

### LinearLayout

### RelativeLayout

- 相对父布局定位，e.g

```xml
<RelativeLayout > 
    <Button
        android:id="@+id/button1"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        android:layout_alignParentLeft="true"
        android:layout_alignParentTop="true"
        android:text="button 1" />
    <Button
        android:id="@+id/button2"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        android:layout_alignParentRight="true"
        android:layout_alignParentTop="true"
        android:text="button 2" />
    <Button
        android:id="@+id/button3"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        android:layout_alignParentLeft="true"
        android:layout_alignParentBottom="true"
        android:text="button 3" />
    <Button
        android:id="@+id/button4"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        android:layout_alignParentRight="true"
        android:layout_alignParentBottom="true"
        android:text="button 4" />

    <Button
        android:id="@+id/button5"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        android:layout_centerInParent="true"
        android:text="button 5" />

```

- 相对控件定位

```xml
<RelativeLayout ...>
    <Button
        android:id="@+id/button5"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        android:layout_centerInParent="true"
        android:text="button 5" />


    <Button
        android:id="@+id/button1"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"

        android:layout_above="@id/button5"
        android:layout_toLeftOf="@id/button5"

        android:text="button 1" />
    <Button
        android:id="@+id/button2"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        android:layout_above="@id/button5"
        android:layout_toRightOf="@id/button5"
        android:text="button 2" />
    <Button
        android:id="@+id/button3"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        android:layout_below="@id/button5"
        android:layout_toRightOf="@id/button5"
        android:text="button 3" />
    <Button
        android:id="@+id/button4"
        android:layout_width="wrap_content"
        android:layout_height="wrap_content"
        android:layout_below="@id/button5"
        android:layout_toLeftOf="@id/button5"
        android:text="button 4" />

```

### FrameLayout

这种布局没有方便的定位方式，所有的控件默认放在布局的左上角。只有“android:layout_gravity=”这一个布局能力

### 百分比布局

只有LinearLayout支持layout_weight属性来实现按比例指定控件大小功能。 其他布局是不支持的。

为此android引入了PercentFrameLayout，与PercentRelativeLayout。这个放在support库中。
```groovy
dependencies {
  compile 'com.android.support:percent:24.2.1'
}
```

### ConstraintLayout

约束布局ConstraintLayout 是一个ViewGroup，可以在Api9以上的Android系统使用它，它的出现主要是为了解决布局嵌套过多的问题，以灵活的方式定位和调整小部件。从 Android Studio 2.3 起，官方的模板默认使用 ConstraintLayout。



### AbsoluteLayout

### TableLayout



