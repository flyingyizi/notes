
# junit4

下载电子书来自www.32r.com, 来自https://github.com/ppatil9096/books



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



## 单元测试任务包括：

- 接口功能测试：用来保证接口功能的正确性。
- 局部数据结构测试（不常用）：用来保证接口中的数据结构是正确的
    - 比如变量有无初始值
    - 变量是否溢出
- 边界条件测试
    - 变量没有赋值（即为NULL）
    - 变量是数值（或字符)
        - 主要边界：最小值，最大值，无穷大（对于DOUBLE等）
        - 溢出边界（期望异常或拒绝服务）：最小值-1，最大值+1
        - 临近边界：最小值+1，最大值-1
    - 变量是字符串
        - 引用“字符变量”的边界
        - 空字符串
        - 对字符串长度应用“数值变量”的边界
    - 变量是集合
        - 空集合
        - 对集合的大小应用“数值变量”的边界
        - 调整次序：升序、降序
    - 变量有规律
        - 比如对于Math.sqrt，给出n^2-1，和n^2+1的边界
- 所有独立执行通路测试：保证每一条代码，每个分支都经过测试
    - 代码覆盖率
        - 语句覆盖：保证每一个语句都执行到了
        - 判定覆盖（分支覆盖）：保证每一个分支都执行到
        - 条件覆盖：保证每一个条件都覆盖到true和false（即if、while中的条件语句）
        - 路径覆盖：保证每一个路径都覆盖到

- 相关软件
    - Cobertura/jaCoCo：语句覆盖
    - Emma: Eclipse插件Eclemma
- 各条错误处理通路测试：保证每一个异常都经过测试



## 最佳实践

- 对还没有实现的测试代码应抛出异常
- 总是为跳过测试填写说明原因@Ingore(value="....")
- 使用[hamcrest](http://hamcrest.org/)来优化断言的编写
- 不要再mock object中写入业务逻辑，它必须是傻对象，只是测试驱动的，这个特性和stub相反
- 

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

### @before @After @BeforeClass @AfterClass

JUnit在调用每一个测试方法(使用@Test注解)前，会调用@Before注解修饰的方法。之后会调用@After。  无论测试是通过还是失败。

如果有多个@Before/@After，那它们的执行顺序是不被定义的。


@Before：初始化方法   对于每一个测试方法都要执行一次（注意与BeforeClass区别，后者是对于所有方法执行一次）
@After：释放资源  对于每一个测试方法都要执行一次（注意与AfterClass区别，后者是对于所有方法执行一次）

@BeforeClass：针对所有测试，只执行一次，且必须为static void 
@AfterClass：针对所有测试，只执行一次，且必须为static void 
一个JUnit4的单元测试用例执行顺序为： 
@BeforeClass -> @Before -> @Test -> @After -> @AfterClass; 

每一个测试方法的调用顺序为： 

@Before -> @Test -> @After; 

常用导入：

```java
import org.junit.Test;
import org.junit.Before;
import org.junit.BeforeClass;
import static org.junit.Assert.*;

```


### @Ingore

通过该注解来跳过某个测试方法举例

```java
public class AppTest {
    @Ignore(value='ingnore for now until we decide a decent time-limit')
    @Test(timeout=130)
     public void xxxxx() {
        ...
    }
}
```

如果将该注解放置在test class上，那就会将这整个测试类跳过


### @Test(expected=xxx.class)

期待该测试方法抛出xxx异常

例如下面的例子

```java
    @Test(expected=RuntimeException.class)
    public void exception() {
    throw new RuntimeException();
    }   
```

### @Test(timeout=xxx)

期待该测试方法的超时异常是xxx毫秒


### @FixMethodOrder

@FixMethodOrder是控制@Test方法执行顺序的注解，她有三种选择

- MethodSorters.JVM: This leaves the test methods in the order returned by the JVM.
This order may vary from run to run.
- MethodSorters.NAME_ASCENDING: This sorts the test methods by the method name in
the lexicographic order.
- MethodSorters.DEFAULT: This is the default value that doesn’t guarantee the
execution order.

 ```java
 import org.junit.Test;
import org.junit.FixMethodOrder;
import org.junit.runners.MethodSorters;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
public class TestExecutionOrder {
...
}
```

###  测试假设 TODO

Assume.assumeFalse

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

```
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

## build.gradle测试过滤
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

# 测试覆盖率Exploring Code Coverage

术语含义：

- missed Instructions代码覆盖率，
- Missed Branches逻辑分支覆盖率，
- Missed Cxty判断执行数，
- missed Lines代码行数，
- Methods方法个数。

[jacoco代码覆盖率报告分析](https://www.jianshu.com/p/ef987f1b6f2f)

## cobertura (不支持JDK8以及后续版本)
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

## jaCoCo

[jacoco_plugin](https://docs.gradle.org/current/userguide/jacoco_plugin.html)


Note that while tests should be executed before generation of report, jacocoTestReport task does not depend on the test task. By default, a HTML report is generated at $buildDir/reports/jacoco/test.

执行，例如

```sh
$./gradlew test
$./gradlew jacocoTestReport
```



# test  doubles 

## concept 

### dummy

测试哑元仅仅是为了测试中缺少元素的错误.

dummy objects are passed to avoid NullPointerException for mandatory parameter objects as follows:

```java
Book javaBook = new Book("Java 101", "123456");
Member dummyMember = new DummyMember());
javaBook.issueTo(dummyMember);
assertEquals(javaBook.numberOfTimesIssued(),1);
```

### stub

测试桩用于给调用方提供简单输入

You typically use a stub  object when you want to replace a real implementation with an object that will return the same response every time. Let’s return to our theater ticket pricing example to see this in action.

```java
 @Test
  public void tenPercentDiscount() {
     Price price = new StubPrice(); 
     Ticket ticket = new Ticket(price);           
     assertEquals(9.0,
                 ticket.getDiscountPrice().doubleValue(),0.0001);       
  }

public class StubPrice implements Price {
  @Override
  public BigDecimal getInitialPrice() {
    return new BigDecimal("10");         
  }
}
```



### fake

仿冒比桩更丰满，几乎和生产代码作的一样多
A  fake  object can be seen as an enhanced stub that almost does the same work as your production code, but that takes a few shortcuts in order to fulfill your testing requirements. Fakes are especially useful when you’d like your code to run against something that’s very close to the real third-party subsystem or dependency that you’ll use in the live implementation

 Most well-grounded Java developers will sooner or later have to write code that interacts with a database, typically performing CRUD  operations on Java objects. Prov-ing that your Data Access Object ( DAO) code works before running it against the pro-duction database is often left until the system integration test phase, or it isn’t checked at all! It would be of great benefit if you could check that the  DAO code works during your unit test or integration test phase, giving you that all important, fast feedback.

 A fake object could be used in this case—one that represents the database you’re interacting with. But writing your own fake object representing a database would be quite  difficult!  Luckily,  over  recent  years,  in-memory  databases  have  become  small enough, lightweight enough, and feature-rich enough to act as a fake object for you.

HSQLDB (www.hsqldb.org) is a popular in-memory database used for this purpose.

### spy

### mock


[Java 各种Mock工具比较](https://blog.csdn.net/yasi_xi/article/details/24642517)

所有的mock框架基本流程都是：

record（录制）---- replay（回放） ---- verify（验证）

- record : 设置将要被调用的方法和返回值。Expections中的方法至少被调用一次，否则会出现missing invocation错误。调用次数和调用顺序不限。StrictExpectations中方法调用的次数和顺序都必须严格执行。如果出现了在StrictExpectations中没有声明的方法，会出现unexpected invocation错误。

- replay：调用（未被）录制的方法，被录制的方法调用会被JMockit拦截并重定向到record阶段设定的行为。

- verify：基于行为的验证，测试CUT是否正确调用了依赖类，包括：调用了哪些方法；通过怎样的参数;调用了多少次;调用的相对顺序（VerificationsInOrder）等。可以使用times，minTimes，maxTimes来验证。


## Mockito

[The only mocking framework supported (by default) by Spring Boot is Mockito](https://docs.spring.io/spring-boot/docs/current/reference/html/boot-features-testing.html). If you use Spring, the answer is quite obvious.

如果要使用spring，那就使用[mockito](https://github.com/mockito/mockito)吧。

[mockito中文文档](https://github.com/hehonghui/mockito-doc-zh)

[Mockito Cookbooks and Examples](https://github.com/eugenp/tutorials/tree/master/testing-modules/mockito), 其中的verify介绍 [Mockito Verify Cookbook](https://www.baeldung.com/mockito-verify)

[Mockito专题](https://www.baeldung.com/tag/mockito/)

Mockito不能mock的是：cannot mock or spy the following functions: 
- final classes, 
- final methods, 
- enums, 
- static methods, 
- private methods, 
- the hashCode() and equals() methods, 
- anonymous classes,
- and primitive types.

### 预备

core
```groovy
// https://mvnrepository.com/artifact/org.mockito/mockito-core
testImplementation group: 'org.mockito', name: 'mockito-core', version: '3.3.3'
```

all
```groovy
// https://mvnrepository.com/artifact/org.mockito/mockito-all
testImplementation group: 'org.mockito', name: 'mockito-all', version: '1.10.19'
```

### 生成 Mock 对象；

首先需要通过在对应测试类进行初始化工作,采用：

```java
@RunWith(MockitoJUnitRunner.class)
public class MockitoAnnotationTest {
    ...
}
```
或
```java
@Before
public void init() {
    MockitoAnnotations.initMocks(this);
}
```

MockitoJUnitRunner 和 initMocks(this) 对UT提供mock初始化工作
在单元测试中使用@Mock, @Spy, @InjectMocks等注解时，需要进行初始化后才能使用
若在单元测试类中使用了`@RunWith(SpringJUnit4ClassRunner.class)` 就不能再使用`@RunWith(SpringJUnit4ClassRunner.class)`，可以使用 MockitoAnnotations.initMocks(this) 来代替
MockitoAnnotations.initMocks(this)，其中this就是单元测试所在的类，在initMocks函数中Mockito会根据类中不同的注解（如@Mock, @Spy等）创建不同的mock对象，即初始化工作

通过@Mock注解（采用注解方式必须指定runner `@RunWith(MockitoJUnitRunner.class)`）：

```java
import org.mockito.junit.MockitoJUnitRunner;

@RunWith(MockitoJUnitRunner.class)
public class StockBrokerBDDTest {
	@Mock MarketWatcher marketWatcher;
    ...
```

或是通过mock方法：

```java
import org.mockito.junit.MockitoJUnitRunner;
import static org.mockito.Mockito.mock;

public class StockBrokerBDDTest {
	MarketWatcher marketWatcher =  mock(MarketWatcher.class);
    ...
```

### stubbing method：设定 Mock 对象的预期行为和输出；

例子，其中comment部分是easyMock语法

```java
package com.example.d;

import static org.junit.Assert.assertEquals;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;

import com.example.domain.DBUtility;
import com.example.domain.SalesOrder;
import com.example.domain.SalesOrderImpl;

import org.junit.Test;
import org.mockito.junit.MockitoJUnitRunner;
import org.junit.runner.RunWith;

import static org.mockito.Mockito.when;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;

@RunWith(MockitoJUnitRunner.class)

public class MyMock {

    @Test
    public void FirstTest() {

        // step1 生成mock对象
		// IMocksControl control = EasyMock.createControl();
		
		// DBUtility mockDBUtility = control.createMock(DBUtility.class);
		// Connection mockConnection = control.createMock(Connection.class);
		// Statement mockStatement = control.createMock(Statement.class);
		// ResultSet mockResultSet = control.createMock(ResultSet.class);

        DBUtility mockDBUtility = mock(DBUtility.class);
        Connection mockConnection = mock(Connection.class);
        Statement mockStatement = mock(Statement.class);
        ResultSet mockResultSet = mock(ResultSet.class);

        try {
            // step2 录制mock预期行为与输出，以及调用次数
            // mockDBUtility.getConnection();
            // EasyMock.expectLastCall().andStubReturn(mockConnection);
            when(mockDBUtility.getConnection()).thenReturn(mockConnection);

            // mockConnection.createStatement();
            // expectLastCall().andStubReturn(mockStatement);
            when(mockConnection.createStatement()).thenReturn(mockStatement);

            // mockStatement.executeQuery(sqlEquals("SELECT * FROM sales_order_table"));
            // expectLastCall().andStubReturn(mockResultSet);
            when(mockStatement.executeQuery("SELECT * FROM sales_order_table")).thenReturn(mockResultSet);

            // mockResultSet.next();
            // expectLastCall().andReturn(true).times(3);
            // expectLastCall().andReturn(false).times(2);
            when(mockResultSet.next()).thenReturn(true, true, true, false, false);

            // mockResultSet.getString(1);
            // expectLastCall().andReturn("DEMO_ORDER_001").times(1);
            // expectLastCall().andReturn("DEMO_ORDER_002").times(1);
            // expectLastCall().andReturn("DEMO_ORDER_003").times(1);
            when(mockResultSet.getString(1)).thenReturn("DEMO_ORDER_001", "DEMO_ORDER_002", "DEMO_ORDER_003");

            // mockResultSet.getString(2);
            // expectLastCall().andReturn("Asia Pacific").times(1);
            // expectLastCall().andReturn("Europe").times(1);
            // expectLastCall().andReturn("America").times(1);
            when(mockResultSet.getString(2)).thenReturn("Asia Pacific", "Europe", "America");

            // mockResultSet.getDouble(3);
            // expectLastCall().andReturn(350.0).times(1);
            // expectLastCall().andReturn(1350.0).times(1);
            // expectLastCall().andReturn(5350.0).times(1);
            when(mockResultSet.getDouble(3)).thenReturn(350.0, 1350.0, 5350.0);

            // step3 切换状态为replay
            // control.replay();

            // //step4 调用mock对象进行单元测试
            Connection conn = mockDBUtility.getConnection();
            Statement stat = conn.createStatement();
            ResultSet rs = stat.executeQuery("SELECT * FROM sales_order_table");

            int i = 0;
            String[] priceLevels = { "Level_A", "Level_C", "Level_E" };
            while (rs.next()) {
                SalesOrder order = new SalesOrderImpl();
                order.loadDataFromDB(rs);
                assertEquals(order.getPriceLevel(), priceLevels[i]);
                i++;
            }
            // //step5 单元测试验证
            // control.verify();

            //在mockito中，verify起到作用是显示核对，不是easyMock中的触发验证作用，比如下面是核对调用了三次
            verify(mockResultSet,times(3)).getDouble(3);

        } catch (Exception e) {
            e.printStackTrace();
        }

    }

}
```

### 参数匹配

内置参数匹配包括`anyInt(), anyDouble(), anyString(), anyList(), and anyCol内置参数匹配`等。 [更多内置参数匹配支持](http://docs.mockito.googlecode.com/hg/latest/org/mockito/Matchers.html)


### mockito  spy  objects

A Mockito spy object allows us to use real objects instead of mocks by replacing some of
the methods with the stubbed ones. This behavior allows us to test the legacy code; one
cannot mock a class that needs to be tested. Legacy code comes with methods that cannot
be tested, but other methods use them; so, these methods need to be stubbed to work with
the other methods. A spy object can stub the nontestable methods so that other methods
can be tested easily.

Spying real objects and calling real methods on a spy has side effects; to counter this side
effect, use doReturn() instead of thenReturn().
spying 真实对象,通过spying调用对象的方法是会有副作用的,需要使用doReturn API, 见下面例子:

```java
    @Test
    public void doReturn_usage()() {

        List<String> list = new ArrayList<String>();
        List<String> spy = Mockito.spy(list);
        // impossible the real list.get(0) is called and fails
        // with IndexOutofBoundsException, as the list is empty
        //when(spy.get(0)).thenReturn("not reachable");

        // doReturn fixed the issue
        Mockito.doReturn("now reachable").when(spy).get(0);
        assertEquals("now reachable", spy.get(0));
    }
```

### 参数验证

通过ArgumentCaptor对象的`forClass(Class<T> clazz)`方法来构建ArgumentCaptor对象。然后便可在验证时对方法的参数进行捕获，最后验证捕获的参数值。如果方法有多个参数都要捕获验证，那就需要创建多个ArgumentCaptor对象处理。

需要监视这个类中的foo函数内部使用的data
```java
class A {
    public void foo(OtherClass other) {
        SomeData data = new SomeData("Some inner data");
        other.doSomething(data);
    }
}
```

可以采用下面的方式:

```JAVA
// Create a mock of the OtherClass
OtherClass other = mock(OtherClass.class);

// Run the foo method with the mock
new A().foo(other);

// Capture the argument of the doSomething function
ArgumentCaptor<SomeData> captor = ArgumentCaptor.forClass(SomeData.class);
verify(other, times(1)).doSomething(captor.capture());

// Assert the argument
SomeData actual = captor.getValue();
assertEquals("Some inner data", actual.innerData);
```


## JMockit

[jmockit中文网](http://jmockit.cn/)

### 预备

```groovy
    //如果使用Junit4.5以上，jmockit依赖需要在junit4之前；或者在测试类上添加注解 @RunWith(JMockit.class)。
    //如果是TestNG 6.2+ 或者 JUnit 5+， 没有位置限制
    // https://mvnrepository.com/artifact/org.jmockit/jmockit
    testImplementation group: 'org.jmockit', name: 'jmockit', version: '1.49'
    // Use JUnit test framework
    testImplementation 'junit:junit:4.12'
```


## easymock


[EasyMock 使用方法与原理剖析](https://www.ibm.com/developerworks/cn/opensource/os-cn-easymock/)

如果您要模拟的是一个具体类而非接口，那么您需要下载扩展包 EasyMock Class Extension 2.2.2。在对具体类进行模拟时，您只要用 org.easymock.classextension.EasyMock 类中的静态方法代替 org.easymock.EasyMock 类中的静态方法即可。

EasyMock 是一套用于通过简单的方法对于给定的接口生成 Mock 对象的类库。它提供对接口的模拟，能够通过录制、回放、检查三步来完成大体的测试过程，可以验证方法的调用种类、次数、顺序，可以令 Mock 对象返回指定的值或抛出指定异常。通过 EasyMock，我们可以方便的构造 Mock 对象从而使单元测试顺利进行。

### 预备

```groovy
// https://mvnrepository.com/artifact/org.easymock/easymock
testImplementation group: 'org.easymock', name: 'easymock', version: '3.1'
// https://mvnrepository.com/artifact/org.easymock/easymockclassextension
testImplementation group: 'org.easymock', name: 'easymockclassextension', version: '3.2'
```


### 生成 Mock 对象；

```java
import org.easymock.EasyMock;
import org.easymock.IMocksControl;
ResultSet mockResultSet = EasyMock.createMock(ResultSet.class);
```

或

```java
IMocksControl control = EasyMock.createControl();
java.sql.Connection mockConnection = control.createMock(Connection.class);
java.sql.Statement mockStatement = control.createMock(Statement.class);
java.sql.ResultSet mockResultSet = control.createMock(ResultSet.class);

```

第二种方式是EasyMock 类的 createControl 方法能创建一个接口 IMocksControl 的对象，该对象能创建并管理多个 Mock 对象。如果需要在测试中使用多个 Mock 对象，我们推荐您使用这一机制，因为它在多个 Mock 对象的管理上提供了相对便捷的方法


### stubbing method：设定 Mock 对象的预期行为和输出；


一个 Mock 对象将会经历两个状态：Record 状态和 Replay 状态。Mock 对象一经创建，它的状态就被置为 Record。在 Record 状态，用户可以设定 Mock 对象的预期行为和输出，这些对象行为被录制下来，保存在 Mock 对象中。

添加 Mock 对象行为的过程通常可以分为以下3步：

- 对 Mock 对象的特定方法作出调用；
- 通过 org.easymock.EasyMock 提供的静态方法 expectLastCall 获取上一次方法调用所对应的IExpectationSetters 实例；
- 通过 IExpectationSetters 实例设定 Mock 对象的预期输出。

例如:

```java
	mockDBUtility.getConnection();
	EasyMock.expectLastCall().andStubReturn(mockConnection);

	mockConnection.createStatement();
	expectLastCall().andStubReturn(mockStatement);

	mockStatement.executeQuery(sqlEquals("SELECT * FROM sales_order_table"));
	expectLastCall().andStubReturn(mockResultSet);

	mockResultSet.next();
	expectLastCall().andReturn(true).times(3);
	expectLastCall().andReturn(false).times(1);

	mockResultSet.getString(1);
	expectLastCall().andReturn("DEMO_ORDER_001").times(1);
	expectLastCall().andReturn("DEMO_ORDER_002").times(1);
	expectLastCall().andReturn("DEMO_ORDER_003").times(1);

	mockResultSet.getString(2);
	expectLastCall().andReturn("Asia Pacific").times(1);
	expectLastCall().andReturn("Europe").times(1);
	expectLastCall().andReturn("America").times(1);

	mockResultSet.getDouble(3);
	expectLastCall().andReturn(350.0).times(1);
	expectLastCall().andReturn(1350.0).times(1);
	expectLastCall().andReturn(5350.0).times(1);

```

### 将 Mock 对象切换到 Replay 状态；

在使用 Mock 对象进行实际的测试前，我们需要将 Mock 对象的状态切换为 Replay。在 Replay 状态，Mock 对象能够根据设定对特定的方法调用作出预期的响应。将 Mock 对象切换成 Replay 状态有两种方式，您需要根据 Mock 对象的生成方式进行选择。

```java
EasyMock.replay(mockResultSet);
```
或

```java
control.replay();
```

### 调用 Mock 对象方法进行单元测试；

 在实际的测试阶段，Sales Order 对象的 loadDataFromDB 方法调用了 mockResultSet 对象的 getString 和 getDouble 方法读取 mockResultSet 中的数据。Sales Order 对象根据读取的数据计算出 Price Level，并和预期输出进行比较。

 ```java
 	Connection conn = mockDBUtility.getConnection();
	Statement stat = conn.createStatement();
	ResultSet rs = stat.executeQuery("select * from sales_order_table");

	int i = 0;
	String[] priceLevels = { "Level_A", "Level_C", "Level_E" };
	while (rs.next()) {
		SalesOrder order = new SalesOrderImpl();
		order.loadDataFromDB(rs);
		assertEquals(order.getPriceLevel(), priceLevels[i]);
		i++;
	}
```


### 对 Mock 对象的行为进行验证。

在利用 Mock 对象进行实际的测试过程之后，我们还有一件事情没有做：对 Mock 对象的方法调用的次数进行验证。

为了验证指定的方法调用真的完成了，我们需要调用 verify 方法进行验证。和 replay 方法类似，您需要根据 Mock 对象的生成方式来选用不同的验证方式 verify 进行验证：

```java
verify(mockResultSet);
```

或

```java
control.verify();
```


将对 control 实例所生成的 Mock 对象 mockConnection、mockStatement 和 mockResultSet 等进行验证。如果将上例中 expectLastCall().andReturn(false).times(1) 的预期次数修改为2，

```java
	mockResultSet.next();
	expectLastCall().andReturn(true).times(3);
	expectLastCall().andReturn(false).times(1);
```

修改为:

```java
	mockResultSet.next();
	expectLastCall().andReturn(true).times(3);
	expectLastCall().andReturn(false).times(2);
```


在 执行测试用例 中将可以看到：
```
Message:
N/A
Stack trace:
java.lang.AssertionError: 
  Expectation failure on verify:
    ResultSet.next(): expected: 5, actual: 4
```

完整用例见：

```java

package com.example.d;

import org.junit.Test;
import static org.junit.Assert.*;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;

import com.example.domain.DBUtility;
import com.example.domain.SalesOrder;
import com.example.domain.SalesOrderImpl;
import static com.example.matcher.SQLEquals.sqlEquals;

import org.easymock.EasyMock;
import org.easymock.IMocksControl;
import static org.easymock.EasyMock.expectLastCall;

public class MyMockTest {
    @Test
    public void testSalesOrder() {    
        // step1 生成mock对象
		IMocksControl control = EasyMock.createControl();
		
		DBUtility mockDBUtility = control.createMock(DBUtility.class);
		Connection mockConnection = control.createMock(Connection.class);
		Statement mockStatement = control.createMock(Statement.class);
		ResultSet mockResultSet = control.createMock(ResultSet.class);

		try {
            //step2 录制mock预期行为与输出，以及调用次数
			mockDBUtility.getConnection();
			EasyMock.expectLastCall().andStubReturn(mockConnection);

			mockConnection.createStatement();
			expectLastCall().andStubReturn(mockStatement);

			mockStatement.executeQuery(sqlEquals("SELECT * FROM sales_order_table"));
			expectLastCall().andStubReturn(mockResultSet);

			mockResultSet.next();
			expectLastCall().andReturn(true).times(3);
			expectLastCall().andReturn(false).times(2);

			mockResultSet.getString(1);
			expectLastCall().andReturn("DEMO_ORDER_001").times(1);
			expectLastCall().andReturn("DEMO_ORDER_002").times(1);
			expectLastCall().andReturn("DEMO_ORDER_003").times(1);

			mockResultSet.getString(2);
			expectLastCall().andReturn("Asia Pacific").times(1);
			expectLastCall().andReturn("Europe").times(1);
			expectLastCall().andReturn("America").times(1);

			mockResultSet.getDouble(3);
			expectLastCall().andReturn(350.0).times(1);
			expectLastCall().andReturn(1350.0).times(1);
			expectLastCall().andReturn(5350.0).times(1);
            //step3 切换状态为replay
			control.replay();
			//step4 调用mock对象进行单元测试
			Connection conn = mockDBUtility.getConnection();
			Statement stat = conn.createStatement();
			ResultSet rs = stat.executeQuery("select * from sales_order_table");

			int i = 0;
			String[] priceLevels = { "Level_A", "Level_C", "Level_E" };
			while (rs.next()) {
				SalesOrder order = new SalesOrderImpl();
				order.loadDataFromDB(rs);
				assertEquals(order.getPriceLevel(), priceLevels[i]);
				i++;
			}
			//step5 单元测试验证
			control.verify();

		} catch (Exception e) {
			e.printStackTrace();
        }
    }    

}
```