
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

