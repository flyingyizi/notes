
在idea中，maina 然后TAB可以快速生成fun main

## kotlin REPL

```sh
$export KOTLIN_HOME=/Users/jack/soft/kotlinc
$export PATH=$PATH:$KOTLIN_HOME/bin

$kotlinc
Welcome to Kotlin version 1.3.70 (JRE 11+28)
Type :help for help, :quit for quit
>>>
```

java application项目变更为java+kotlin项目

```groovy
// 1.新增kotlin plugin
plugins {
    // Apply the java plugin to add support for Java
    id 'java'

    // Apply the application plugin to add support for building a CLI application.
    id 'application'
    id 'org.jetbrains.kotlin.jvm' version '1.3.71'
}

// 2.新增kotlin-stdlib
dependencies {
    // This dependency is used by the application.
    implementation 'com.google.guava:guava:28.2-jre'

    // Use JUnit test framework
    testImplementation 'junit:junit:4.12'
    implementation "org.jetbrains.kotlin:kotlin-stdlib-jdk8"
}
```


## kotlin
- https://github.com/Kotlin/KEEP/tree/master/proposals
- [Building and Testing a Parser With ANTLR and Kotlin](https://dzone.com/articles/building-and-testing-a-parser-with-antlr-and-kotli-1)

- 对Kotlin语言, `if`是expression表达式，不是statement语句. 它们之间的差异在于表达式是有值value的，能作为其他表达式的一部分。对java所有的控制结构都是语句. 对Kotlin, 在控制结构中，除了循环loops ( for , do , and do / while ) ，其他的都是表达式。

- val (from value)—Immutable reference. A variable declared with val can’t be reassigned after it’s initialized. It corresponds to a final variable in Java. A val variable must be initialized exactly once during the execution of the block where it’s defined. But you can initialize it with different values depending on some condition, if the compiler can ensure that only one of the initialization statements will be executed

```kotlin
val languages = arrayListOf("Java")
languages.add("Kotlin")  // valid
```

- var (from variable)—Mutable reference. The value of such a variable can be changed. This declaration corresponds to a regular (non-final) Java variable. Even though the var keyword allows a variable to change its value, its type is fixed.

```kotlin
var answer = 42
answer = "no answer" // error
```

- Note that the modifier public disappeared during the conversion from Java to Kotlin. In Kotlin, public is the default visibility, so you can omit it.

- Custom accessors This section shows you how to write a custom implementation of a property accessor. Suppose you declare a rectangle that can say whether it’s a square. You don’t need to store that information as a separate field, because you can check whether the height is equal to the width on the go:

```kotlin
class Rectangle(val height: Int, val width: Int)
{ val isSquare: Boolean
  get() {
    return height == width
  }
}
```

- 
```kotlin
enum class Color(
           val r: Int, val g: Int, val b: Int
) {
  RED(255, 0, 0), ORANGE(255, 165, 0),
  YELLOW(255, 255, 0), GREEN(0, 255, 0), BLUE(0, 0, 255),
  INDIGO(75, 0, 130), VIOLET(238, 130, 238);
  fun rgb() = (r * 256 + g) * 256 + b
}
>>> println(Color.BLUE.rgb())
```

- [[Difference between List and Array types in Kotlin]](https://stackoverflow.com/questions/36262305/difference-between-list-and-array-types-in-kotlin)


# kotlin 语言


### String templates

```kotlin
var a = 1
// simple name in template:
val s1 = "a is $a" 
​
a = 2
// arbitrary expression in template:
val s2 = "${s1.replace("is", "was")}, but now is $a"
```

### Nullable values and null checks

  - 通常String，不能为null的写法

    ```kotlin
    var a: String = "abc" // Regular initialization means non-null by default
    a = null // compilation error
    ```

  - nullablie string 的写法
  
   ```kotlin
    var b: String? = "abc" // can be set null
    b = null // ok
    print(b)
   ```

  - 安全调用

    ```kotlin
    val a = "Kotlin"
    val b: String? = null
    println(b?.length) //如果b为null则打印null，否则打印b.length
    println(a?.length) // Unnecessary safe call
    ```

  - If not null and else shorthand

  ```kotlin
  val files = File("Test").listFiles()
  println(files?.size ?: "empty")
  val files = File("Test").listFiles()
  ​
  println(files?.size ?: "empty")
  ```

## 惯用方式

[Idioms](https://kotlinlang.org/docs/reference/idioms.html)

### 数据对象DTOs

```kotlin
data class Customer(val name: String, val email: String)
```

### list filter

```kotlin
val positives = list.filter { x -> x > 0 }
Or alternatively, even shorter:

val positives = list.filter { it > 0 }
```

### when语句 Instance Checks

```kotlin
when (x) {
    is Foo -> ...
    is Bar -> ...
    else   -> ...
}
```

### Checking element presence in a collection.

```kotlin
if ("john@example.com" in emailsList) { ... }
​
if ("jane@example.com" !in emailsList) { ... }
```

### 只读list/map

```kotlin
val list = listOf("a", "b", "c")
val map = mapOf("a" to 1, "b" to 2, "c" to 3)
```

### Lazy property

kotlin的延迟加载主要有两种：lateinit和lazy

其中，lazy() 是一个函数，可以接受一个 Lambda 表达式作为参数，第一次调用时会执行 Lambda 表达式，以后调用该属性会返回之前的结果。

lateinit 只能用于修饰变量 var，不能用于可空的属性和 Java 的基本类型。lateinit 可以在任何位置初始化并且可以初始化多次。

```kotlin
val p: String by lazy {
    // compute the string
}
```

### ?: 语句

Executing a statement if null

```kotlin
val values = ...
val email = values["email"] ?: throw IllegalStateException("Email is missing!")
```

### with语句

```kotlin
Calling multiple methods on an object instance (with)
class Turtle {
    fun penDown()
    fun penUp()
    fun turn(degrees: Double)
    fun forward(pixels: Double)
}
​
val myTurtle = Turtle()
with(myTurtle) { //draw a 100 pix square
    penDown()
    for (i in 1..4) {
        forward(100.0)
        turn(90.0)
    }
    penUp()
}
```

### apply语句

Configuring properties of an object (apply)

```kotlin
val myRectangle = Rectangle().apply {
    length = 4
    breadth = 5
    color = 0xFAFAFA
}
```

### TODO支持

TODO(): Marking code as incomplete
Kotlin's standard library has a TODO() function that will always throw a NotImplementedError. Its return type is Nothing so it can be used regardless of expected type. There's also an overload that accepts a reason parameter:

```kotlin
fun calcTaxes(): BigDecimal = TODO("Waiting for feedback from accounting")
```

## 位运算Bitwise operations

位运算仅适用于Int与long

```kotlin
val x = (1 shl 2) and 0x000FF000
```

支持的运算:

shl(bits) – signed shift left
shr(bits) – signed shift right
ushr(bits) – unsigned shift right
and(bits) – bitwise and
or(bits) – bitwise or
xor(bits) – bitwise xor
inv() – bitwise inversion

## 字符Char

Char不能直接与整型进行比较，必须是：

```kotlin
//We can explicitly convert a character to an Int number:

fun decimalDigitValue(c: Char): Int {
    if (c !in '0'..'9')
        throw IllegalArgumentException("Out of range")
    return c.toInt() - '0'.toInt() // Explicit conversions to numbers
}
```

## Arrays

在java中
```java
int[] anArray;
anArray = new int[10];
//系统提供了arraycopy方法
```


在kotlin中是Array class, 支持get/set，以及“[]”操作。

```kotlin
class Array<T> private constructor() {
    val size: Int
    operator fun get(index: Int): T
    operator fun set(index: Int, value: T): Unit
​
    operator fun iterator(): Iterator<T>
    // ...
}
```

- 创建方法1： arrayOf(1, 2, 3) creates an array [1, 2, 3]. 
- 创建方法2： arrayOfNulls() library function can be used to create an array of a given size filled with null elements.
- 创建方法3：通过构造函数,构造函数带尺寸和初始化
Another option is to use the Array constructor that takes the array size and the function that can return the initial value of each array element given its index:

  ```kotlin
  // Creates an Array<String> with values ["0", "1", "4", "9", "16"]
  val asc = Array(5) { i -> (i * i).toString() }
  asc.forEach { println(it) }
  ```

As we said above, the [] operation stands for calls to member functions get() and set().

原始类型的数组Primitive type arrays

Kotlin also has specialized classes to represent arrays of primitive types without boxing overhead: ByteArray, ShortArray, IntArray and so on. These classes have no inheritance relation to the Array class, but they have the same set of methods and properties. Each of them also has a corresponding factory function:

  ```kotlin
  val x: IntArray = intArrayOf(1, 2, 3)
  x[0] = x[1] + x[2]

  val arr = IntArray(5)

  val arr = IntArray(5) { 42 } ​

  var arr = IntArray(5) { it * 1 }

  ```

## function

https://kotlinlang.org/docs/reference/functions.html

如果在默认参数之后的最后一个参数是lambda，那它可以作为命名参数传递，或在括号外传递:

```kotlin
fun foo(bar: Int = 0, baz: Int = 1, qux: () -> Unit) { /*...*/ }
​
foo(1) { println("hello") }     // Uses the default value baz = 1
foo(qux = { println("hello") }) // Uses both default values bar = 0 and baz = 1 
foo { println("hello") }        // Uses both default values bar = 0 and baz = 1
```

### TODO Tail recursive functions

### TODO infix

