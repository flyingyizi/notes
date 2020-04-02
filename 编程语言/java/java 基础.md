
# java开发

## 安装jdk

类目录的绝对路径叫做 class path。设置在系统变量 CLASSPATH 中。编译器和 java 虚拟机通过将 package 名字加到 class path 后来构造 .class 文件的路径。一个 class path 可能会包含好几个路径，多路径应该用分隔符分开。

1. 设置环境变量: JAVA_HOME
2. 设置环境变量(后面有个.): CLASSPATH=.;%%JAVA_HOME%%\lib\dt.jar;%%JAVA_HOME%%\lib\tools.jar;  
3. 设置环境变量: 追加PATH=%%JAVA_HOME%%\bin;%%JAVA_HOME%%\jre\bin;  

[groovy教程](https://www.w3cschool.cn/groovy/)

[gradle教程](https://www.w3cschool.cn/gradle/)

[廖雪峰java教程](https://www.liaoxuefeng.com/wiki/1252599548343744)

## java helloword

- java执行class文件是根据CLASSPATH指定的地方来找，不是我们理解当前目录。如果希望它查询当前目录，需要在CLASSPATH中加入“.;”,代表当前目录。

- java执行class文件对package的路径是强依赖的。它在执行的时候会严格以当前用户路径为基础，按照package指定的包路径转化为文件路径去搜索class文件。

注意：上面的执行路径约束在jdk11后就不会有了，因为它不再需要先编译在执行，直接`java xxx.java`就可以执行

样例文件App.java
```java
package helloWord;

public class App {
    public String getGreeting() {
        return "Hello world.";
    }

    public static void main(String[] args) {
        System.out.println(new App().getGreeting());
    }
}
```

执行以下命令

```sh
c:\home\mydemo\helloWord\src\main\java\helloWord>dir

2020/03/18  18:00               291 App.java
c:\home\mydemo\helloWord\src\main\java\helloWord>javac App.java

c:\home\mydemo\helloWord\src\main\java\helloWord>dir
2020/03/18  18:02               523 App.class
2020/03/18  18:00               291 App.java

c:\home\mydemo\helloWord\src\main\java\helloWord>cd ..
c:\home\mydemo\helloWord\src\main\java>java helloWord.App
Hello world.

c:\home\mydemo\helloWord\src\main\java\helloWord>java  -classpath  ..  helloWord.App
Hello world.

c:\home\mydemo\helloWord\src\main\java\helloWord>
```

## java 基础语法

[java随笔](https://www.cnblogs.com/cisum/category/1126085.html)

在 Java 中使用 final 关键字来修饰常量，声明方式和变量类似 `final double PI = 3.1415927;`

强制类型转换， `int i1 = 123;        byte b = (byte)i1;`


### Java 增强 for 循环

语法是下面的样子，其中表达式是表达式是要访问的数组名，或者是返回值为数组的方法。
```java
for(声明语句 : 表达式) 
{ //代码句子 
}
```
例子
```java
public class Test {
   public static void main(String args[]){
      int [] numbers = {10, 20, 30, 40, 50};
 
      for(int x : numbers ){
         System.out.print( x );
         System.out.print(",");
      }
      System.out.print("\n");
      String [] names ={"James", "Larry", "Tom", "Lacy"};
      for( String name : names ) {
         System.out.print( name );
         System.out.print(",");
      }
   }
}
```

### Java Number & Math 类

Math类是java.lang.Math,Number类是java.lang.Number, 因此不需要显示import。

当需要使用数字的时候，我们通常使用内置数据类型，如：byte、int、long、double 等.然而，在实际开发过程中，我们经常会遇到需要使用对象，而不是内置数据类型的情形。为了解决这个问题，Java 语言为每一个内置数据类型提供了对应的包装类。

所有的包装类（Integer、Long、Byte、Double、Float、Short）都是抽象类 Number 的子类。

拆箱/装箱的例子
```groovy
public class Number {
    public static void main(String[] args) {
        /**
        Integer i1 = 128;  // 装箱，相当于 Integer.valueOf(128);
        int t = i1; //相当于 i1.intValue() 拆箱
        System.out.println(t);
        */

        /**
        对于–128到127（默认是127）之间的值,被装箱后，会被放在内存里进行重用
        但是如果超出了这个值,系统会重新new 一个对象
        */
        Integer i1 = 200;
        Integer i2 = 200;

        /**
        注意 == 与 equals的区别
        == 它比较的是对象的地址
        equlas 比较的是对象的内容
        */
        if(i1==i2) {
            System.out.println("true");
        } else {
            System.out.println("false");
        }
    }
}
```
### Java Character 类

内置数据类型 char。 对应的包装是java.lang.Character

### java枚举 

```java 
  enum FreshJuiceSize{ SMALL, MEDIUM , LARGE }
```

### java String 类

`java.lang.String`, `java.lang.StringBuffer`, `java.lang.StringBuilder`

String 类是不可改变的，所以你一旦创建了 String 对象，那它的值就无法改变了。如果需要对字符串做很多修改，那么应该选择使用 StringBuffer & StringBuilder 类。

[字符串格式化的语法](https://www.cnblogs.com/cisum/p/8011485.html)

格式化字符串由4部分组成，特殊的格式常以`%index$`开头，index从1开始取值，表示将第index个参数拿进来进行格式化，[最小宽度]的含义也很好理解，就是最终该整数转化的字符串最少包含多少位数字。

- 1.对整数进行格式化：  %[index$][标识][最小宽度]转换方式
- 2.对浮点数进行格式化：%[index$][标识][最少宽度][.精度]转换方式

整数的转换方式包括：d-十进制 o-八进制 x或X-十六进制

浮点数的转换方式包括：
  ```text
    'e', 'E' -- 结果被格式化为用计算机科学记数法表示的十进制数
    'f' -- 结果被格式化为十进制普通表示方式
    'g', 'G' -- 根据具体情况，自动选择用普通表示方式还是科学计数法方式
    'a', 'A' -- 结果被格式化为带有效位数和指数的十六进制浮点数
   ```

使用举例
```java
System.out.println(String.format("%1$,09d", -3123));
```

### java 数组

`java.util.Arrays` 类是 JDK 提供的一个工具类，用来处理数组的各种方法，而且每个方法基本上都是静态方法，能直接通过类名Arrays调用。

创建
```java
// 1
dataType[] arrayRefVar = new dataType[arraySize];
//2
dataType[] arrayRefVar = {value0, value1, ..., valuek};
//3
int a[][] = new int[2][3];
//4
String s[][] = new String[2][];
s[0] = new String[2];
s[1] = new String[3];
s[0][0] = new String("Good");
s[0][1] = new String("Luck");
s[1][0] = new String("to");
s[1][1] = new String("you");
s[1][2] = new String("!");
```

### java 日期时间Date类 以及格式化

`java.util.Date` 不推荐, 推荐 `java.util.Calendar`。

Calendar本身是个抽象类，程序只能创建Calendar 子类的实例， Java 本身提供了一个GregorianCalendar 类，一个代表格里高利日历的子类，它代表了通常所说的公历。



[Java 格式化输出 printf 例子](https://www.runoob.com/w3cnote/java-printf-formate-demo.html)

```java
// 支持多个参数时，可以在%s之间插入变量编号，1$表示第一个字符串，3$表示第3个字符串
System.out.printf("%1$s = %3$s %2$s%n", "Name", "san", "Zhang");

/*** 输出日期类型***/
// %t表示格式化日期时间类型，%T是时间日期的大写形式，在%t之后用特定的字母表示不同的输出格式
Date date = new Date();
long dataL = date.getTime();
// 格式化年月日
// %t之后用y表示输出日期的年份（2位数的年，如99）
// %t之后用m表示输出日期的月份，%t之后用d表示输出日期的日号
System.out.printf("%1$ty-%1$tm-%1$td; %2$ty-%2$tm-%2$td%n", date, dataL);

/*** < 标志。它表明先前被格式化的参数要被再次使用 ***/
System.out.printf("%s %tB %<te, %<tY", "Due date:", date);
```

`java.text.SimpleDateFormat` 类有一些附加的方法，特别是parse()，它试图按照给定的SimpleDateFormat 对象的格式化存储来解析字符串。
```java
SimpleDateFormat ft = new SimpleDateFormat ("yyyy-MM-dd"); 
Date date = ft.parse("2020-1-1"); 
```



### java 方法

Java 允许定义这样的方法，它在对象被垃圾收集器析构(回收)之前调用，这个方法叫做 finalize( )，它用来清除回收对象。但是目前普遍的认识是不要使用，因为对java虚拟机的垃圾回收有影响。 [在java中为什么不推荐使用finalize](https://baijiahao.baidu.com/s?id=1655232869611610920&wfr=spider&for=pc)

通常Java 的内存回收可以由 JVM 来自动完成。如果你手动使用，则可以使用上面的finalize( )方法。

### java 异常处理

```java
try{
  // 程序代码
}catch(异常类型1 异常的变量名1){
  // 程序代码
}catch(异常类型2 异常的变量名2){
  // 程序代码
}finally{
    //finally 关键字用来创建在 try 代码块后面执行的代码块。
    //无论是否发生异常，finally 代码块中的代码总会被执行。
    //在 finally 代码块中，可以运行清理类型等收尾善后性质的语句。

  // 程序代码
}
```



### java 抽象类abstract和接口interface

接口特性

   - 接口中每一个方法也是隐式抽象的,接口中的方法会被隐式的指定为 public abstract（只能是 public abstract，其他修饰符都会报错）。
   - 接口中可以含有变量，但是接口中的变量会被隐式的指定为 public static final 变量（并且只能是 public，用 private 修饰会报编译错误）。
   - 接口中的方法是不能在接口中实现的，只能由实现接口的类来实现接口中的方法。

抽象类和接口的区别

   - 1. 抽象类中的方法可以有方法体，就是能实现方法的具体功能，但是接口中的方法不行。
   - 2. 抽象类中的成员变量可以是各种类型的，而接口中的成员变量只能是 public static final 类型的。
   - 3. 接口中不能含有静态代码块以及静态方法(用 static 修饰的方法)，而抽象类是可以有静态代码块和静态方法。
   - 4. 一个类只能继承一个抽象类，而一个类却可以实现多个接口。



什么时候使用抽象类和接口

   - 如果你拥有一些方法并且想让它们中的一些有默认实现，那么使用抽象类吧。
   - 如果你想实现多重继承，那么你必须使用接口。由于Java不支持多继承，子类不能够继承多个类，但可以实现多个接口。因此你就可以使用接口来解决它。
   - 如果基本功能在不断改变，那么就需要使用抽象类。如果不断改变基本功能并且使用接口，那么就需要改变所有实现了该接口的类。

### Java 泛型

在严格的泛型代码里，带泛型声明的类(e.g " `public class Apple <T> {}`")总应该带着类型参数。但为了与老的Java 代码保持一致，也允许在使用带泛型声明的类时不指定实际的类型参数。如果没有为这个泛型类指定实际的类型参数，则该类型参数被称作raw type （原始类型），默认是声明该类型参数时指定的第一个上限类型。

例如对" `public class Apple <T> {}`", "new Apple()"使用时，类型参数会默认为Object。

例如对" `public class Apple <T extends Number> {}`", "new Apple()"使用时，类型参数会默认为Number。

当把一个具有泛型信息的对象赋给另一个没有泛型信息的变量时，所有在尖括号之间的类型信息都
将被扔掉。比如一个List<String>类型被转换为List ， 则该List 对集合元素的类型检查变成了类型参数的上限(即Object).下面程序示范了这种擦除。

对类似定义的泛型类，在使用时如果没有传入实际的类型参数T，那系统会将`Apple<T>`类中的T当作Object类型处理。



语法举例

```java
public static <T extends Comparable<T>> T maximum(T x, T y, T z)
   {                     
      T max = x; // 假设x是初始最大值
      if ( y.compareTo( max ) > 0 ){
         max = y; //y 更大
      }
```

类型通配符一般是使用?代替具体的类型参数。例如 List<?> 在逻辑上是List<String>,List<Integer> 等所有List<具体类型实参>的父类。

```java
public static void getData(List<?> data) {
      System.out.println("data :" + data.get(0));
   }
```

`<? extends T>和<? super T>`的区别
   
   -  `<? extends T>`表示该通配符所代表的类型是T类型的子类。
   -  `<? super T>`表示该通配符所代表的类型是T类型的父类。



### java软引用、弱引用

引用对象在Java定义中有三种类型，从弱到强依次为：软引用SoftReference、弱引用WeakReference与虚引用，三种级别也各有所不同(软引用>弱引用)。

[Understanding Weak References](https://stackoverflow.com/questions/299659/whats-the-difference-between-softreference-and-weakreference-in-java)

google官方是推荐Android开发者使用WeakReference，而不建议SoftReference 引用，Android环境下与纯Java有所略同。

举例如何利用软引用和弱引用解决OOM问题，

```java
// cache，该cache保存的是弱引用
private Map<String, SoftReference<Bitmap>> imageCache = new HashMap<String, SoftReference<Bitmap>>();

public void addBitmapToCache(String path) { 
        // 强引用的Bitmap对象 
        Bitmap bitmap = BitmapFactory.decodeFile(path); 
        // 软引用的Bitmap对象 
        SoftReference<Bitmap> softBitmap = new SoftReference<Bitmap>(bitmap); 
        // 添加该对象到Map中使其缓存 
        imageCache.put(path, softBitmap); 
    }
 
 public Bitmap getBitmapByPath(String path) { 
        // 从缓存中取软引用的Bitmap对象 
        SoftReference<Bitmap> softBitmap = imageCache.get(path); 
        // 判断是否存在软引用 
        if (softBitmap == null) { 
            return null; 
        } 
        // 取出Bitmap对象，如果由于内存不足Bitmap被回收，将取得空 
        Bitmap bitmap = softBitmap.get(); 
        return bitmap; 
    }
```

### lambda表达式

Lambda 表达式支持将代码块作为方法参数，

Java 8 专门为函数式接口提供了`@FunctionalInterface` 注解，该注解通常放在接口定义前面，该注解程序功能没有任何作用， 它用于告诉编译器执行史严格检查一检查该接口必须是函数式接口， 否则编译器就会报错．

所谓函数式接口，当然首先是一个接口，然后就是在这个接口里面只能有一个抽象方法。这种类型的接口也称为SAM接口，即Single Abstract Method interfaces

函数式特点

   - 接口有且仅有一个抽象方法
   - 允许定义静态方法
   - 允许定义默认方法
   - 允许java.lang.Object中的public方法
   - @FunctionalInterface注解不是必须的，如果一个接口符合"函数式接口"定义，那么加不加该注解都没有影响。加上该注解能够更好地让编译器进行检查。如果编写的不是函数式接口，但是加上了@FunctionalInterface，那么编译器会报错

函数式接口定义举例

```java
    @FunctionalInterface
    interface GreetingService 
    {
        void sayMessage(String message);
        //默认方式不是抽象方法，因此合法
        default void doSomeMoreWork1(){System.out.println("call by default method ");}
    }
    public static void main(final String[] args) throws IOException {
        // Employee emp = new Employee("name");
        // emp.printEmployee();
        GreetingService greetService1 = message -> System.out.println("Hello " + message);
        greetService1.sayMessage("message");
        greetService1.doSomeMoreWork1();
    }
}
```

Runnable 是java内置提供的函数式接口
```java
        Runnable r = () -> {
            System.out.println("x");
        }
```

### 源文件声明规则

   当在一个源文件中定义多个类，并且还有import语句和package语句时，要特别注意这些规则。

   - 一个源文件中只能有一个public类
   - 一个源文件可以有多个非public类
   - 源文件的名称应该和public类的类名保持一致。例如：源文件中public类的类名是Employee，那么源文件应该命名为Employee.java。
   - 如果一个类定义在某个包中，那么package语句应该在源文件的首行。
   - 如果源文件包含import语句，那么应该放在package语句和类定义之间。如果没有package语句，那么import语句应该在源文件中最前面。
   - import语句和package语句对源文件中定义的所有类都有效。在同一源文件中，不能给不同的类不同的包声明。
   - 除了上面提到的几种类型，Java还有一些特殊的类，如：内部类、匿名类。

类有若干种访问级别，并且类也分不同的类型：抽象类和final类等。这些将在访问控制章节介绍。

### Java修饰符

像其他语言一样，Java可以使用修饰符来修饰类中方法和属性。主要有两类修饰符：

   - 访问控制修饰符 : default, public , protected, private
   - 非访问控制修饰符 : final, abstract, strictfp

Java中，可以使用访问控制符来保护对类、变量、方法和构造方法的访问。Javav支持 4 种不同的访问权限。

   - default (即缺省，什么也不写）: 在同一包内可见，不使用任何修饰符。使用对象：类、接口、变量、方法。

   - private : 在同一类内可见。使用对象：变量、方法。 注意：不能修饰类（外部类）

   - public : 对所有类可见。使用对象：类、接口、变量、方法

   - protected : 对同一包内的类和所有子类可见。使用对象：变量、方法。 注意：不能修饰类（外部类）。

### 初始化块  感觉没啥用徒增不清晰

### 静态导入Static import

使用静态导入可以使被导入类的静态变量和静态方法在当前类直接可见，使用这些静态成员无需再给出他们的类名。  感觉这个习惯不太好，因为会降低可读性。

静态导入的语法是：

  - import static 包名.类名.静态成员变量;
  - import static 包名.类名.静态成员函数;

例如存在下面的静态成员
```java
package com.example.learnjava;

public class Common
{

    public static final int AGE = 10;
    public static void output()
    {
        System.out.println("Hello World!");
    }
}
```
在另一个包中使用时，如果不用静态导入，是这样用的
```java
import com.example.learnjava.Common;
int a = Common.AGE;
Common.output();
```

使用静态导入

```java
import static com.example.learnjava.Common.AGE;
import static com.example.learnjava.Common.output;
int a = AGE;
output();
```

### import第三方库

例如 HelloWorld.jara中需要使用org.joda.time.LocalTime。 需要配合两步

- 在程序中import 类
- 在build.gradle中添加对应的库

例子如下
```java
import org.joda.time.LocalTime;
 
public class HelloWorld {
  public static void main(String[] args) {
    LocalTime currentTime = new LocalTime();
    System.out.println("The current local time is: " + currentTime);
 
    Greeter greeter = new Greeter();
    System.out.println(greeter.sayHello());
  }
}
```

```groovy
dependencies {
    compile "joda-time:joda-time:2.2"
}
```

## 与C++配合

加载文件和动态链接库主要对native 方法有用，对于一些特殊的功能（如访问操作系统底层硬件设备等） Java 程序元法实现，必须借助C 语言来完成，此时需要使用C 语言为Java 方法提供实现。其实现步骤如下：
- Java 程序中声明native 修饰的方法， 类似于abstract 方法，只有方法签名，没有实现．编译该Java 程序，生成一个class 文件．
- 用javah 编译第1 步生成的class 文件，将产生一个h 文件。
- 写一个.cpp 文件实现native 方法，这一步需要包含第2 步产生的. h 文件（这个.h文件中又包含了JDK 带的jni.h 文件），
- 将第3 步的.cpp 文件编译成动态链接库文件。
- 在Java中用System 类的loadLibrary..()方法或Runtime 类的loadLibrary..()方法加载
- 第4 步产生的动态链接库文件， Java 程序中就可以调用这个native 方法了。

## 正则

Java 提供了`java.util.regex.Pattern` 和 `java.util.regex.Matcher` 两个类专门用于提供正则表达式支持。

举例：

```java
    Pattern p =  Pattern.compile("a*b");
    Matcher m = p.matcher("aaaaab");
    boolean b = m.matches();
```

## 国际化

the native2ascii tool has bben removed from the JDK, because JDK9 and later release support UTF-8 based properties resource bundles.

列出java支持的所有语言

```java
Locale[]  locallist = Locale.getAvailableLocales();
 for (var l: locallist) {
    System.out.println(l.getDisplayCountry() + "=" + l.getCountry()+ "=" + l.getDisplayLanguage());
}
 for (var l: locallist) {
    System.out.println(l.getDisplayCountry() + "=" + l.getCountry()+ "=" + l.getDisplayLanguage()+ "=" +l.getLanguage()  );
}
```

```sh
jshell> Locale myl = Locale.getDefault(Locale.Category.FORMAT);
myl ==> zh_CN
```

资源文件的命名可以有如下三种形式。

- `<baseName>_<language>_ <country>.properties`
- `<baseName>_<language>.properties`
- `<baseName>.properties`


如果系统同时存在资源文件、类文件，系统将以类文件为主，而不会调用资源文件。对于简体中文
的Locale , ResourceBund1e 搜索资源文件的顺序是：

   - (1) baseName_zh_CN.class
   - (2) baseName_zh_CN.properties
   - (3) baseName_zh.c1ass
   - (4) baseName_zh.properties
   - (5) baseName.class
   - (6) baseName.properties

注：类文件的尝试了发现无法生效，不知道原因，后续看看 TODO

资源文件示例, mess_zh_CN.properties
```text
hello=你好
```

资源类文件示例


国际化示例：
```java
    public String getGreeting() {
        Locale myLocale = Locale.getDefault(Locale.Category.FORMAT);
        ResourceBundle bundle = ResourceBundle.getBundle("mess", myLocale);
        String msg = bundle.getString("hello");

        return msg;
        // 如果资源是类似"msg=Hello, {0} !Today is {1}." 这样带占位符的，需要下面的格式化处理
        //return MessageFormat.format(msg ,"yeeku" , new Date() ) ;
    }
    public static void main(String[] args) {
        System.out.println(new App().getGreeting());
    }
```
## java集合List/Set/Queue/Map

所有的集合类都位于java.util包下，后来为了处理多线程环境下的并发安全问题， Java 5 还在java. util.concurrent 包下提供了一些多线程支持的集合类。

Java 的集合类主要由两个接口派生而出： Collection 和Map, Collection 和Map 是Java 集合框架的根接口.

Iterator 接口也是Java 集合框架的成员. 注意 Iterator 并不是把集合元素本身传给了迭代变量，而是把集合元素的值传给了迭代变量，所以修改迭代变量的值对集合元素本身没有任何影响。

经常使用的就是Collection与Iterator了。 如：

```java
java.util.Collection c = new ArrayList();
c.forEach( obj -> {       ;     });

java.util.Iterator i = c.iterator();

while(i.hasNext()) { obj = i.next();  }
```

注：不推荐用Iterator，有些麻烦，直接用java新提供的for高级循环for(xx : 集合)就很简单。

HashMap举例

```java
        var m = new java.util.HashMap<String,Integer>();
        m.put("key", 1);

        for (var key : m.keySet()) {
            //m.get(key)
        }
```



## java 反射reflect

程序需要在运行时发现对象和类的真实信息。由于JVM为每个加载的class创建了对应的Class实例，并在实例中保存了该class的所有信息，包括类名、包名、父类、实现的接口、所有方法、字段等，因此，如果获取了某个Class实例，我们就可以通过这个Class实例获取到该实例对应的class的所有信息。

这种通过Class实例获取class信息的方法称为反射（Reflection）。

解决该问题有以下两种做法。

- 第一种做法是假设在编译时和运行时都完全知道类型的具体信息，在这种情况下，可以先使用instanceof 运算符进行判断，再利用强制类型转换将其转换成其运行时类型的变量即可。
- 第二种做法是编译时根本无法预知该对象和类可能属于哪些类， 程序只依靠运行时信息来发现该对象和类的真实信息，这就必须使用反射。

在Java 程序中获得Class 对象通常有如下三种方式。

### 获得Class 对象

- `<Class>.forName(String clazzName)`使用Class 类静态方法。该方法需要传入字符串参数，该字符串参数的值是某个类的全限定类名（必须添加完整包名） 。
- `<Class>.class`使用某个类的class 静态变量来获取该类对应的Class 对象。例如， Person.class 将会返回Person 类对应的Class 对象。
- `<Object>.getClass()`调用某个对象的getClass()方法。该方法是java.lang.Object 类中的一个方法，所以所有的Java对象都可以调用该方法，该方法将会返回该对象所属类对应的Class对象。

例如定义个类ClassTest，
```java
//下面代码可以获取ClassTest 对应的Class
Class<ClassTest> clazz = ClassTest.class;
//获取该Class 对象所对应类的全部构造器
Constructor[] ctors = clazz.getDeclaredConstructors() ;
//获取该Class 对象所对应类的全部public 方法
Method[] mtds = clazz.getMethods() ;
//获取该Class 对象所对应类的全部注解
Annotation [] anns = clazz.getAnnotations();

```

### 访问字段

Class类提供了以下几个方法来获取字段：

- Field getField(name)：根据字段名获取某个public的field（包括父类）
- Field getDeclaredField(name)：根据字段名获取当前类的某个field（不包括父类）
- Field[] getFields()：获取所有public的field（包括父类）
- Field[] getDeclaredFields()：获取当前类的所有field（不包括父类）

```java
public class Main {
    public static void main(String[] args) throws Exception {
        Class stdClass = Student.class;
        // 获取public字段"score":
        System.out.println(stdClass.getField("score"));
        // 获取继承的public字段"name":
        System.out.println(stdClass.getField("name"));
        // 获取private字段"grade":
        Field f = stdClass.getDeclaredField("grade");
        System.out.println(f);
        // 赋值
        f.setAccessible(true);
        f.set(p, 200);        
    }
}

class Student extends Person {
    public int score;
    private int grade;
}

class Person {
    public String name;
}
```

## java 调用native动态库

Java调用Native的动态库有两种方式，JNI和JNA，[JNA](https://github.com/java-native-access/jna)是Oracle最新推出的与Native交互的方式。

[JNA Examples](https://www.eshayne.com/jnaex/index.html)

![JNA调用过程](image/JNA调用过程.png)

注：JNA中，它提供了一个动态的C语言编写的转发器，可以自动实现Java和C的数据类型映射。

```groovy
dependencies 
{
    ...
    implementation 'net.java.dev.jna:jna:4.5.0'
}
```

golang构建动态库为例，`Go build -buildmode=c-shared -o libhello.dll .\libhello.go`

```golang
package main

import "C"

import "fmt"

//export Hello
func Hello(msg string)  *C.char  {
    fmt.Print("hello: " + msg)    
    return C.CString("hello: " + msg)
}

//export Sum
func Sum(a int, b int) int {   return a + b}

func main() {}
```

java 创建对应调用接口。 以整型与String为例

```java
package com.example.helloWord;

import com.sun.jna.Library;
import com.sun.jna.Native;
import java.util.List;
import java.util.Arrays;
import com.sun.jna.Structure;

public interface LibHello extends Library {
    LibHello INSTANCE = (LibHello) Native.loadLibrary("c:/home/libhello", LibHello.class);

    int Sum(int a, int b);

    // GoString class maps to:
    // C type struct { const char *p; GoInt n; }
    public class GoString extends Structure {
        public static class ByValue extends GoString implements Structure.ByValue {
        }

        public String p;
        public long n;

        protected List getFieldOrder() {
            return Arrays.asList(new String[] { "p", "n" });
        }

    }

    public String Hello(GoString.ByValue msg);
}

```

java 调用golang动态库为例，

```java
package com.example.helloWord;

public class App {
        public static void main(String[] args) {
        //System.out.println(new App().getGreeting());
        System.out.println(LibHello.INSTANCE.Sum(222, 333));

        LibHello.GoString.ByValue str = new LibHello.GoString.ByValue();
        str.p = "msg, Hello Java!";
        str.n = str.p.length();

        String returnstr = LibHello.INSTANCE.Hello(str);
        System.out.printf("\nreturned:%s\n",returnstr);
    }
}
```

将C头文件生成JNA接口，有个工具[java -jar jnaeratorStudio.jar](https://github.com/nativelibs4java/JNAerator)可以考虑使用。


# adb

1. 设置环境变量: ANDROID，变量值：C:\prog\Android\sdk\platform-tools(sdk路径)
2. 在Path末尾添加;%ANDROID%

为了使用真正的手机硬件进行调试，以下是几个需要的预备动作。

- 在手机上开启usb debug调试模式。
- 在android studio,通过“settings/system settings/android sdk/sdk tools”中选择“google usb driver”进行安装。
- 对android studio所在pc机，通过设备管理器，对已经连在pc机上的手机进行驱动更新。驱动是上一步下载得到，路径在“xxx\Android\sdk\extras\google\usb_driver”

以上预备动作作为后，后面在android studio上执行run，选择对应的手机硬件


# gradle

去看 <Android Gradle权威指南>

还有一份简单的[Android Gradle使用总结](https://www.cnblogs.com/zhaoyanjun/p/7603640.html)

任何一个 Gradle 构建都是由一个或多个 projects 组成。每个 project 都由多个 tasks 组成。每个 task 都代表了构建执行过程中的一个原子性操作。如编译，打包，生成 javadoc，发布到某个仓库等操作。

- 通过 `gradlew -?`, 查看有哪些命令可以使用。

- 通过`./gradlew tasks`查看所有可执行的Tasks

- 通过`./gradlew  help --task  <task>` 查看任务的具体信息，比如`./gradlew  help --task  build`，`./gradlew  help --task tasks`

在utf-8文件编码情况下，编译java时经常有中文编码方面的问题，加上下面的语句覆盖掉默认的gbk

```grooovy
tasks.withType(JavaCompile) {
    options.encoding = "UTF-8"
}
```

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


## java gradle: jar打包

如果通过gradle init生成的模板是适用application的。 没有jar设置。因此如果打包jar要运行，需要增加对应的信息，比如为manifest.mf中做些下面的准备。

```groovy
jar {
      manifest {
        attributes(
          'Main-Class': 'helloWord.App'
        )
      }
}
```

