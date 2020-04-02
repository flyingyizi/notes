
下载电子书来自www.32r.com


[书籍附带sourceCode](https://www.manning.com/downloads/732)，来自[链接](https://www.manning.com/books/junit-in-action-second-edition)页面。

准备工作1：添加junit依赖
```groovy
dependencies {
    ...
    // Use JUnit test framework
    testImplementation 'junit:junit:4.12'
}
```

准备工作2：用例编写：

```java
package com.example.helloWord;

// 导入下面junit包
import org.junit.Test;
import static org.junit.Assert.*;

//作为测试类的条件是：必须是public，必须有无参构造函数
public class AppTest {
    //作为测试方法，必须有@Test注解，并且是public，必须是返回void，必须无参
    @Test(timeout=130)
     public void testAppHasAGreeting() {
        App classUnderTest = new App();
        assertNotNull("app should have a greeting", classUnderTest.getGreeting());
    }
}
```

## 最佳实践

- 对还没有实现的测试代码应抛出异常
- 总是为跳过测试填写说明原因@Ingore(value="....")
- 使用[hamcrest](http://hamcrest.org/)来优化断言的编写

## 测试类

一个测试类必须要满足下面条件：

- 必须是public，必须有无参构造函数
- 测试方法必须有@Test注解，并且是public，必须是返回void，必须无参

最常用的断言：
||作用|
|--|--|
|assertArrayEquals("message",A,B) | 断言A数组与B数组相等  |
assertEquals("message",A,B) | 断言A对象与B对象相等，使用的是A.equals方法  |
assertSame("message",A,B) | 断言A对象与B对象相同，使用的是==操作符  |
assertTrue("message",A) | 断言A条件为true  |
assertNotNull("message",A,B) | 断言A对象不是null  |

JUnit的测试运行器包括：

- JUnit38ClassRunner 
- JUnit4
- Parameterized
- Suite

它们都继承自org.junit.runner.Runner类

### @before @After

JUnit在调用每一个测试方法(使用@Test注解)前，会调用@Before注解修饰的方法。之后会调用@After。  无论测试是通过还是失败。

如果有多个@Before/@After，那它们的执行顺序是不被定义的。

### @Ingore

通过该注解来跳过某个测试方法

```java
public class AppTest {
    @Ignore(value='ingnore for now until we decide a decent time-limit')
    @Test(timeout=130)
     public void xxxxx() {
        ...
    }
}
```

### @Test(expected=xxx.class)

期待该测试方法抛出xxx异常

### @Test(timeout=xxx)

期待该测试方法的超时异常是xxx毫秒


## 参数化测试(@RunWith(value=Parameterized.class))

要进行参数化测试，必须

- 测试类必须是`@RunWith(value=Parameterized.class)`注解
- 必须声明所使用的实例变量，
- 必须提供一个`@Parameters`注解的方法。这个方法的签名必须是`@Parameters public static java.util.Collection<Integer[]> `
- 必须提供与实例变量对应的构造函数

```java
package com.example.helloWord;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import static org.junit.Assert.*;

@RunWith(value=Parameterized.class)
public class ParameterizedTest {
    private double expeted;
    private double valueOne;
    private double valueTwo;
    
    @Parameters
    public static java.util.Collection<Integer[]> getTestParameters() {
        return java.util.Arrays.asList(new Integer[][] {
            {2,2,0}, //expeted, value1,value2
            {3,1,2},
        });
    }

    public ParameterizedTest(double expeted, double value1,double value2) {
        this.expeted = expeted;
        this.valueOne = value1;
        this.valueTwo = value2;
    }
    
    @Test
    public void sum(){
        Calculator calc = new Calculator() ;
        assertEquals(expeted, calc.add(valueOne, valueTwo),0);
    }
}
```

上面例子的运行过程是：
- junit调用getTestParameters方法，得到返回的集合，
- 对集合中的每个数组循环调用构造函数（如果有多个构造函数会抛出错误），并像平常一样调用`@Test`方法

## 组合测试(@RunWith(value = Suite.class))

[How can I run a test suite using gradle from the command line](https://stackoverflow.com/questions/30789839/how-can-i-run-a-test-suite-using-gradle-from-the-command-line)

测试类A文件：TestCaseA
```java
public class TestCaseA {
	@Test
	public void testA1() {
		assertEquals("Dummy test-case", 1+0, 2);
	}
}
```
测试类B文件：TestCaseB
```java
public class TestCaseB {
	@Test
	public void testB1() {
		assertTrue("Dummy test-case", true);
	}
}
```

组合测试集：MasterTestSuite
```java
package com.example.helloWord;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

/**
 * A test-suite that calls two other test-suites.
 * 
 * @version $Id: MasterTestSuite.java 552 2010-03-06 11:48:47Z paranoid12 $
 */
@RunWith(value = Suite.class)
@SuiteClasses(value = { TestCaseA.class, TestCaseB.class })
public class MasterTestSuite {}
```

### 命令行执行
命令行执行该测试集方式是：

```sh
$./gradlew   test   --tests  MasterTestSuite
```

```text
$ ./gradlew help --task  test  
> Task :help
Detailed task information for test
Path
     :test
Type
     Test (org.gradle.api.tasks.testing.Test)
Options
     --debug-jvm     Enable debugging for the test process. The process is started suspended and listening on port 5005.

     --fail-fast     Stops test execution after the first failed test.

     --tests     Sets test class or method name to be included, '*' is supported.
...
$
```

### build.gradle测试过滤
还可以直接在build.gradle中添加过滤规则，修改掉`./gradlew test`的运行范围。比如

```groovy
...
test {
    filter {
        // //include specific method in any of the tests
        // includeTestsMatching "*UiCheck"

        // //include all tests from package
        // includeTestsMatching "org.gradle.internal.*"

        // //include all integration tests
        // includeTestsMatching "*IntegTest"
        includeTestsMatching "TestCaseA"
    }
}
```
具体见[gradle test filtering](https://docs.gradle.org/current/userguide/java_testing.html#test_filtering)

## 测试覆盖率

术语含义：

- missed Instructions代码覆盖率，
- Missed Branches逻辑分支覆盖率，
- Missed Cxty判断执行数，
- missed Lines代码行数，
- Methods方法个数。

[jacoco代码覆盖率报告分析](https://www.jianshu.com/p/ef987f1b6f2f)

### cobertura (不支持JDK8以及后续版本)
cobertura是一个与JUnit集成的代码覆盖率测量工具

准备动作,在build.gradle中添加[plugin](https://github.com/stevesaliman/gradle-cobertura-plugin/blob/master/usage.md)：
 ```groovy
 plugins {
  id "net.saliman.cobertura" version "3.0.0"
}
```

- 注1：将gradle插件仅仅用于特定项目，[使用方法链接](https://docs.gradle.org/current/userguide/plugins.html#sec:subprojects_plugins_dsl)。

- 注2：If you have a multi-project build, and you need to have classes from more than one of them, you'll need to add some code to the coverage block of your project similar to the following:

  ```groovy
  cobertura {
    rootProject.subprojects.each {
        coverageDirs << file("${it.name}/build/classes/main")
    }
  }
  ```

执行：

```sh
$./gradlew  cobertura
```

### jaCoCo

[jacoco_plugin](https://docs.gradle.org/current/userguide/jacoco_plugin.html)


Note that while tests should be executed before generation of report, jacocoTestReport task does not depend on the test task. By default, a HTML report is generated at $buildDir/reports/jacoco/test.

执行，例如

```sh
$./gradlew test
$./gradlew jacocoTestReport
```

## stub 粗粒度测试

开发中依赖开发环境（e.g. 依赖数据库，依赖文件系统...）.STUB是一种机制，用来模拟真实代码或尚未完成的代码所产生的行为。

stub是一段代码，通常在运行时使用插入的stub代码来代替真实的代码，一边将其调用者与真正的实现隔离开来。

stub弊端：

- stub往往比较复杂难以编写，并且本身也需要调试
- stub不能很好的应用在细粒度测试
- 不同情况需要不同stub策略

