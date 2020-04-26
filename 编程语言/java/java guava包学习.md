

[Guava 源码分析之Cache的实现原理](http://ifeve.com/category/guava-2/)

下面记录主要基于[Google Guava入门教程](https://www.cnblogs.com/yangkai-cn/p/4028216.html)

[java核心类 廖雪峰](https://www.liaoxuefeng.com/wiki/1252599548343744/1260576204194144)

使用guava，在gradle.build中需要添加对应依赖

```groovy
dependencies {
    // This dependency is used by the application.
    implementation 'com.google.guava:guava:28.1-jre'
```

## 基本工具 [Basic utilities]


### 1.1 使用和避免null

[使用和避免null](http://ifeve.com/using-and-avoiding-null/)：null是模棱两可的，会引起令人困惑的错误，有些时候它让人很不舒服。建议使用`Optional<T>`在可能的可选场景使用

### 1.2 前置条件 

[前置条件](http://ifeve.com/google-guava-preconditions/): 让方法中的条件检查更简单 。建议静态导入这些方法

```java
import static com.google.common.base.Preconditions.*;
checkArgument(i>1, "Argument was %s but expected nonnegative", i);
```

   - checkArgument(boolean)	检查boolean是否为true，用来检查传递给方法的参数。	IllegalArgumentException
   - checkNotNull(T)	检查value是否为null，该方法直接返回value，因此可以内嵌使用checkNotNull。	NullPointerException
   - checkState(boolean)	用来检查对象的某些状态。	IllegalStateException
   - checkElementIndex(int index, int size)	检查index作为索引值对某个列表、字符串或数组是否有效。index>=0 && index<size *	IndexOutOfBoundsException
   - checkPositionIndex(int index, int size)	检查index作为位置值对某个列表、字符串或数组是否有效。index>=0 && index<=size *	IndexOutOfBoundsException
   - checkPositionIndexes(int start, int end, int size)

### 1.3 常见Object方法

[常见Object方法](http://ifeve.com/google-guava-commonobjectutilities/): 简化Object方法实现，如hashCode()和toString()

```java
import java.util.Objects;
        Objects.equals(a, b);
        Objects.hash(values);
```

调试时好用的toString方法

```java
import com.google.common.base.MoreObjects;
// Returns "ClassName{x=1}"
MoreObjects.toStringHelper(this).add("x", 1).toString();
// Returns "MyObject{x=1}"
MoreObjects.toStringHelper("MyObject").add("x", 1).toString();
```

### 1.4 排序

[排序](http://ifeve.com/google-guava-ordering/): Guava强大的”流畅风格比较器”


在JDK 8以后，直接使用系统的Comparator接口。

- 实现了`java.util.Comparable<T>`接口的class，可以由通过Collections.sort或Arrays.sort进行自动排序
```java
public interface java.util.Comparable<T> 
{
    public int compareTo(T o);
}

public class Person implements Comparable<Person>
{
     @Override
    public int compareTo(Person p)
    {
        return this.age-p.getAge();
    }
    ...
}

Person[] people=new Person[]{....};
Arrays.sort(people);
```

- 实现了`java.util.Comparator<T>`接口的比较器，可以由通过Collections.sort或Arrays.sort通过该比较器对类进行排序

```java
public interface java.util.Comparator<T>
 {
    int compare(T o1, T o2);
    boolean equals(Object obj);
 }

public class PersonCompartor implements Comparator<Person>
{
    @Override
    public int compare(Person o1, Person o2)
    {
        return o1.getAge()-o2.getAge();
    }
}

Person[] people=new Person[]{....};
Arrays.sort(people,new PersonCompartor());
```

### 1.5 Throwables

[Throwables](http://ifeve.com/google-guava-throwables/)：简化了异常和错误的传播与检查




## 集合 [collections] 、

Guava对JDK集合的扩展，这是Guava最成熟和为人所知的部分

### 2.1 不可变集合

[不可变集合](http://ifeve.com/google-guava-immutablecollections/): 用不变的集合进行防御性编程和性能提升。

JDK 9 之后，可以通过 List，Set，Map 类中的静态方法快速方便的创建 immutable 集合。不需要使用guava扩展。
所谓的 immutable 对象是指对象的状态不可变，不可修改，因此这样的对象天生就具有线程安全性。

- 使用of，copyof

```java
        //创建 Immutable list
        List stringList = List.of("a", "b", "c");

        //通过copyof从list创建 imutable list
        List<Item> slist= new ArrayList<>();
        slist.addAll(getItemsFromSomewhere());
        slist.addAll(getItemsFromElsewhere());
        slist.addAll(getItemsFromYetAnotherPlace());        
        List<Item> snapshot = List.copyOf(list);

        //创建 Immutable set
        Set<String> stringSet = Set.of("a", "b", "c");
        //创建 Immutable map
        Map stringMap = Map.of("a", 1, "b", 2, "c", 3);
        int value = (int) stringMap.get("a");
        System.out.println(value);

        //创建 Immutable entry map
        Map <Integer, String> friendMap = Map.ofEntries(
            entry(1, "Tom"),
            entry(2, "Dick"),
            entry(3, "Harry"),
            entry(99, "Mathilde"));

        String xx =friendMap.get(1);
        System.out.println(xx);
```


### 2.2 新集合类型

[新集合类型](http://ifeve.com/google-guava-newcollectiontypes/): multisets, multimaps, tables, bidirectional maps等

- 新集合类型 Multiset，它可以多次添加相等的元素。维基百科从数学角度这样定义Multiset：”集合[set]概念的延伸，它的元素可以重复出现…与集合[set]相同而与元组[tuple]相反的是，Multiset元素的顺序是无关紧要的：Multiset {a, a, b}和{a, b, a}是相等的”。

    包括：HashMultiset，	TreeMultiset，LinkedHashMultiset，ConcurrentHashMultiset，ImmutableMultiset

    - hashMultiset 例子

    ```java
            import com.google.common.collect.HashMultiset;
            import com.google.common.collect.Multiset;

            Multiset<String> words = HashMultiset.create();
            words.addAll(List.of("a", "b", "c"));
            words.addAll(List.of("a", "b", "c"));
            for (String key : words.elementSet()) {
                //查看指定元素的个数
                System.out.println(key + "-->" + words.count(key));
            }    
            // 输出
            // a-->2
            // b-->2
            // c-->2

    ```

-  Multimap可以很容易地把一个键映射到多个值。换句话说，Multimap是把键映射到任意多个值的一般方式。


    包括：ArrayListMultimap， HashMultimap，LinkedListMultimap*，LinkedHashMultimap**，TreeMultimap，ImmutableListMultimap，ImmutableSetMultimap

   - HashMultimap例子
   ```java
    Multimap<Integer, Integer> map = HashMultimap.<Integer, Integer>create();
    
    map.put(1, 2);
    map.put(1, 2);
    map.put(1, 3);
    map.put(1, 4);
    map.put(2, 3);
    map.put(3, 3);
    map.put(4, 3);
    map.put(5, 3);
    System.out.println(map);
    // 输出
    //{1=[4, 2, 3], 2=[3], 3=[3], 4=[3], 5=[3]}
   ```

### 2.3 强大的集合工具类

[强大的集合工具类](http://ifeve.com/google-guava-collectionutilities/): 提供java.util.Collections中没有的集合工具



### 2.4 扩展工具类

[扩展工具类](http://ifeve.com/google-guava-collectionhelpersexplained/)：让实现和扩展集合类变得更容易，比如创建Collection的装饰器，或实现迭代器



## 缓存 [caching] 、

[缓存](http://ifeve.com/google-guava-cachesexplained/)

## 原生类型支持 [primitives support] 、

## 并发库 [concurrency libraries] 、

强大而简单的抽象，让编写正确的并发代码更简单

5.1 [ListenableFuture](http://ifeve.com/google-guava-listenablefuture/)：完成后触发回调的Future

5.2 [Service框架](http://ifeve.com/google-guava-serviceexplained/)：抽象可开启和关闭的服务，帮助你维护服务的状态逻辑

## 通用注解 [common annotations] 、

## 字符串处理

[字符串处理](http://ifeve.com/google-guava-strings/) [string processing] 、

### 连接器[Joiner]

不需要使用guava扩展“com.google.common.base.Joiner”，直接使用JDK8引入的StringJoiner.

- 使用StringJoiner实例

```java
        var sj = new StringJoiner(",");
        for(var name: new String[]{"a","b","c",})  {
            sj.add(name);
        }
        System.out.println(sj.toString());
```

- 使用static  String.join()

```java
String[] names = {"Bob", "Alice", "Grace"};
var s = String.join(", ", names);
```

- 使用StringBuilder

### 拆分器[Splitter]

guava spllitter vs  java  split

- guava  splitter

guava spllitter 是构造方法私有，通过static method “on”方法spllitter对象。

```java
import com.google.common.base.Splitter;
import java.util.Iterator;

Iterator<String> it = Splitter.on(",").trimResults().omitEmptyStrings().split("foo,bar,,   qux").iterator();
while (it.hasNext()) {
    System.out.print(it.next() + ",");
}
//输出
//foo,bar,qux,
```

- java  split

```java
        StringBuilder str = new StringBuilder("foo,bar,,   qux");
        String[]  l = str.toString().split(",");
```

### 字符匹配器[CharMatcher]

- 使用guava CharMatcher。

```java
import com.google.common.base.CharMatcher;

String noControl = CharMatcher.JAVA_ISO_CONTROL.removeFrom(string); //移除control字符
String theDigits = CharMatcher.DIGIT.retainFrom(string); //只保留数字字符
String spaced = CharMatcher.WHITESPACE.trimAndCollapseFrom(string, ' ');//去除两端的空格，并把中间的连续空格替换成单个空格
String noDigits = CharMatcher.JAVA_DIGIT.replaceFrom(string, "*"); //用*号替换所有数字
String lowerAndDigit = CharMatcher.JAVA_DIGIT.or(CharMatcher.JAVA_LOWER_CASE).retainFrom(string);
// 只保留数字和小写字母

```

- 使用java Pattern和Matcher

```java
import java.util.regex.Matcher;
import java.util.regex.Pattern;

Pattern p =Pattern.compile("a*b");
Matcher m =p.matcher("aaaaab");
boolean b = m.matches();
```

### 字符集[Charsets]

guava Charsets针对所有Java平台都要保证支持的六种字符集提供了常量引用。尝试使用这些常量，而不是通过名称获取字符集实例。

```java
import com.google.common.base.Charsets;
String str = "我abc";
//encoding
byte[] bytes = str.getBytes(Charsets.UTF_8);
```

## I/O 


## 10. 散列[Hash]
[散列[Hash]](http://ifeve.com/google-guava-hashing/)提供比Object.hashCode()更复杂的散列实现，并提供布鲁姆过滤器的实现

## 11. 事件总线[EventBus]
[事件总线[EventBus]](http://ifeve.com/google-guava-eventbus/)发布-订阅模式的组件通信，但组件不需要显式地注册到其他组件中

## 12. 数学运算[Math]
[数学运算[Math]](http://ifeve.com/google-guava-math/)优化的、充分测试的数学工具类

## 13. 反射[Reflection]
[反射[Reflection]](http://ifeve.com/guava-reflection/)Guava 的 Java 反射机制工具类
