

## kotlin
- https://github.com/Kotlin/KEEP/tree/master/proposals
- [Building and Testing a Parser With ANTLR and Kotlin](https://dzone.com/articles/building-and-testing-a-parser-with-antlr-and-kotli-1)

- In Kotlin, if is an expression, not a statement. The difference between a statement and an expression is that an expression has a value, which can be used as part of another expression, whereas a statement is always a top-level element in its enclosing block, and does not have its own value. In Java, all  ontrol structures are statements. In Kotlin, most control structures, except for the loops ( for , do , and do / while ) are expressions.

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


# android 开发

[kotlin-for-android-developers-zh](https://legacy.gitbook.com/book/wangjiegulu/kotlin-for-android-developers-zh/details),[对应代码](https://github.com/antoniolg/Kotlin-for-Android-Developers)


##  关于android:id="@+id/xx"的理解

android:id属性的内容比较特别：

例如：@+id/height

“@”符号是提示XML解析器应该把后面的字符串解析成标识符号。

“+”符号代表将添加一个标识符号。

“id/”表示这个标识符号回被归类在“id”下面。

"height"是这个界面主见的“android:id”。

以后的程序中，会使用“R.id.height”来取得这个界面组件。因此"@+id/height"的意思是我们在此创建了名为“height”的标识符，可以通过这个标识符来控制所对应的界面组件，“R”类会自动配置一个地址给这个界面组件。“R”类的内容，这可以通过查看“R.java”文件得知。

## 使用android studio

[使用布局编辑器构建界面](https://developer.android.google.cn/studio/write/layout-editor.html)

