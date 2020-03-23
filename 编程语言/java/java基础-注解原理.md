

注解（Annotation），也叫元数据（Metadata）

注解的属性也叫做成员变量。注解只有成员变量，没有方法。注解的成员变量在注解的定义中以“无形参的方法”形式来声明，其方法名定义了该成员变量的名字，其返回值定义了该成员变量的类型。
```java
@Target(ElementType.TYPE)
@Retention(RetentionPolicy.RUNTIME)
public @interface TestAnnotation {
    int id();
    String msg();
}
```

上面代码定义了 TestAnnotation 这个注解中拥有 id 和 msg 两个属性。在使用的时候，我们应该给它们进行赋值。

赋值的方式是在注解的括号内以 value=”” 形式，多个属性之前用 ，隔开。
```java
@TestAnnotation(id=3,msg="hello annotation")
public class Test {
}
```
注： 如果仅仅有一个成员并且名称是value，在赋值过程中可以简写。则也可以简写不用写“value=”。

从JDK5 开始，Java增加了对元数据(MetaData)的支持，也就是Annotation(即注解)，这种Annotation是代码里的特殊标记，这些标记可以在编译、类加载、运行时被读取，并执行相应的处理。通过使用注解，程序开发人员可以在不改变原有逻辑的情况下， 在源文件中嵌入一些补充的信息。代码分析工具、开发工具和部署工具，可以通过这些补充信息进行验证或者进行部署。

Annotation 能被用来为程序元素(类、方法、成员变量等)设置元数据。值得指出的是， Annotation不影响程序代码的执行，无论增加、删除Annotation ，代码都始终如一地执行。如果希望让程序中的Annotation 在运行时起一定的作用，只有通过某种配套的工具对Annotation 中的信息进行访问和处理，访问和处理Annotation 的工具统称APT ( Annotation Processing Tool)。

## java.lang提供的5 个基本注解

如下：

### @Override

    只修饰方法。声明是对super某个方法覆盖，当有了这个注解，而在写代码时将方法名写的和super类的方法名字不一样，编译器是会报错提醒的。

### @Deprecated

    声明过时

### @SuppressWarnings

    对该注解修饰的程序元素取消指定的编译器告警，例如`@SuppressWarnings(value="unchecked")`

### @SafeVarargs
### @Functionallnterface

    只修饰接口。对该注解修饰的接口声明只能是函数式接口（只有一个抽象方法）

## java.lang.annoation提供的基本注解

在`java.lang.annotation` 包下提供了6个Meta Annotation (元Annotation)，其中有5个元Annotation 都用于修饰其他的Annotation 定义。其中@Repeatable 专门用于定义Java 8 新增的重复注解，本章后面会重点介绍相关内容。此处先介绍常用的4个元Annotation。

### @Retention

  只能用于修饰Annotation 定义，用于指定被修饰的Annotation 可以保留多长时间，例如
  
  ```java
  @Target(ElementType.METHOD)
  @Retention(RetentionPolicy.SOURCE)
  public @interface Override {}
  ```

### @Target

  只能用于修饰Annotation 定义，用于指定被修饰的Annotation 能用于修饰哪些程序单元

### @Documented 

    只能用于修饰Annotation 定义，用于指定被该元Annotation 修饰的Annotation 类将被javadoc 工具提取成文档，如果定义Annotation 类时使用了@Documented 修饰，则所有使用该Annotation 修饰的程序元素的API 文档中将会包含该Annotation 说明。

### @Inherited 

    只能用于修饰Annotation 定义，元Annotation 指定被它修饰的Annotation 将具有继承性一一如果某个类使用了@Xxx 注解（定义该Annotation 时使用了@Inherited 修饰）修饰，则其子类将自动被@Xxx 修饰。

### @Repeatable

## 自定义注解 TODO

下面介绍如何自定义Annotation,并利用Annotation 来完成一些实际的功能。

新增一个自定义注解

```java
public @interface TestAnnotation {
    //Annotation 的成员变量在Annotation 定义中以无形参的方法形式来声明
    //一旦指定了成员变量，使用该注解时就必须给成员变量赋值
    int age() default 12;
    String name();
}
```

使用自定义注解进行注解举例

```java
@TestAnnotation(age=1,name="name")
public class MyClass {}
```

使用Annotation 修饰了类、方法、成员变量等成员之后，这些Annotation 不会自己生效，必须由开发者提供相应的工具来提取并处理Annotati on 信息。

例如下面提取注解

```java
        Annotion[] anns = Class.forName("className").getAnnotations();
        for(Annotion ann : anns) {}
```

### 编译时处理注解

Java 提供的javac.exe 工具有一个 `-processor` 选项，该选项可指定一个Annotation 处理器，如果在编译Java 源文件时通过该选项指定了Annotation 处理器， 那么这个Annotation 处理器将会在编译时提取并处理Java 源文件中的Annotation 。
每个Annotation 处理器都需要实现`javax.annotation.processing` 包下的Processor 接口。不过实现该接口必须实现它里面所有的方法，因此通常会采用继承AbstractProcessor 的方式来实现Annotation 处理器。一个Annotation 处理器可以处理一种或者多种Annotation 类型。


## 注解提取

注解通过反射获取。
- step1: 判断是否应用了某个注解

  首先可以通过 Class 对象的 isAnnotationPresent() 方法判断它是否应用了某个注解
```java
public boolean isAnnotationPresent(Class<? extends Annotation> annotationClass) {}
```
- step2: 提取
  
  然后通过 getAnnotation() 方法来获取 Annotation 对象。
```java
//返回指定类型的注解
public <A extends Annotation> A getAnnotation(Class<A> annotationClass) {}
 //或者是 getAnnotations() 方法。返回注解到这个元素上的所有注解
public Annotation[] getAnnotations() {}
```
- 提取注解使用举例
```java
@TestAnnotation()
public class Test {
    public static void main(String[] args) {
        boolean hasAnnotation = Test.class.isAnnotationPresent(TestAnnotation.class);

        if ( hasAnnotation ) {
            TestAnnotation testAnnotation = Test.class.getAnnotation(TestAnnotation.class);

            System.out.println("id:"+testAnnotation.id());
            System.out.println("msg:"+testAnnotation.msg());
        }
    }
}
```



## 元标签

元标签有 @Retention、@Documented、@Target、@Inherited、@Repeatable 5 种。

- @Retention

  Retention 的英文意为保留期的意思。当 @Retention 应用到一个注解上的时候，它解释说明了这个注解的的存活时间。，@Retention 去给一张标签解释的时候，它指定了这张标签张贴的时间。@Retention 相当于给一张标签上面盖了一张时间戳，时间戳指明了标签张贴的时间周期。
 
  它的取值如下：
  | value                 |  说明                              |
  |-----------------------|-----------------------------------|
  RetentionPolicy.SOURCE  |注解只在源码阶段保留，在编译器进行编译时它将被丢弃忽视。  |
  RetentionPolicy.CLASS   |注解只被保留到编译进行的时候，它并不会被加载到 JVM 中。   |
  RetentionPolicy.RUNTIME | 注解可以保留到程序运行的时候，它会被加载进入到 JVM 中，所以在程序运行时可以获 取到它们。 |
```java
@Retention(RetentionPolicy.RUNTIME)
public @interface TestAnnotation {
}
```
- @Documented

  顾名思义，这个元注解肯定是和文档有关。它的作用是能够将注解中的元素包含到 Javadoc 中去。

- @Target

  @Target 指定了注解运用的地方。你可以这样理解，当一个注解被 @Target 注解时，这个注解就被限定了运用的场景。比如只能张贴到方法上、类上、方法参数上等等。@Target 有下面的取值

  |value|说明|
  |--|--|
  |  ElementType.ANNOTATION_TYPE| 可以给一个注解进行注解  |
  |  ElementType.CONSTRUCTOR    | 可以给构造方法进行注解  |
  |  ElementType.FIELD          | 可以给属性进行注解      |
  |  ElementType.LOCAL_VARIABLE | 可以给局部变量进行注解  |
  |  ElementType.METHOD         | 可以给方法进行注解      |
  |  ElementType.PACKAGE        | 可以给一个包进行注解    |
  |  ElementType.PARAMETER      | 可以给一个方法内的参数进行注解 |
  |  ElementType.TYPE           | 可以给一个类型进行注解，比如类、接口、枚举 |

- @Inherited

  Inherited 是继承的意思，但是它并不是说注解本身可以继承，而是说如果一个超类被 @Inherited 注解过的注解进行注解的话，那么如果它的子类没有被任何注解应用的话，那么这个子类就继承了超类的注解。说的比较抽象。代码来解释。

```java
@Inherited
@Retention(RetentionPolicy.RUNTIME)
@interface Test {}

@Test
public class A {}

public class B extends A {}
```  
注解 Test 被 @Inherited 修饰，之后类 A 被 Test 注解，类 B 继承 A,类 B 也拥有 Test 这个注解。

- @Repeatable

  应用于注解的值可以同时取多个的场景。如一个人他既是程序员又是产品经理,同时他还是个画家。
```java
@interface Persons {
    Person[]  value();
}
@Repeatable(Persons.class)  //Persons作为容器注解呢，用来存放其它注解的地方。Persons它本身也是一个注解
@interface Person{
    String role default "";
}

@Person(role="artist")
@Person(role="coder")
@Person(role="PM")
public class SuperMan{
}
```

