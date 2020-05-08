- [basic](#basic)
- [C/C++环境准备](#cc%e7%8e%af%e5%a2%83%e5%87%86%e5%a4%87)
  - [windows 10 bash](#windows-10-bash)
  - [msys2](#msys2)
  - [install clionclion](#install-clionclion)
  - [关于命令行](#%e5%85%b3%e4%ba%8e%e5%91%bd%e4%bb%a4%e8%a1%8c)
  - [cmake](#cmake)
  - [atuomake makefile](#atuomake-makefile)
- [c and cplusplus 编程语言](#c-and-cplusplus-%e7%bc%96%e7%a8%8b%e8%af%ad%e8%a8%80)
  - [获取软件源码](#%e8%8e%b7%e5%8f%96%e8%bd%af%e4%bb%b6%e6%ba%90%e7%a0%81)
  - [常量](#%e5%b8%b8%e9%87%8f)
  - [控制语句举例](#%e6%8e%a7%e5%88%b6%e8%af%ad%e5%8f%a5%e4%b8%be%e4%be%8b)
  - [变量作用域](#%e5%8f%98%e9%87%8f%e4%bd%9c%e7%94%a8%e5%9f%9f)
  - [数组array](#%e6%95%b0%e7%bb%84array)
  - [指针](#%e6%8c%87%e9%92%88)
- [mingw gcc常用命令](#mingw-gcc%e5%b8%b8%e7%94%a8%e5%91%bd%e4%bb%a4)
    - [[[dump class layout]](https://stackoverflow.com/questions/2549618/is-there-any-g-option-to-dump-class-layout-and-vtables)](#dump-class-layout)
    - [get asm code](#get-asm-code)
    - [Include Path for use with MinGW Compilers](#include-path-for-use-with-mingw-compilers)
    - [生成share library](#%e7%94%9f%e6%88%90share-library)
    - [编译vs使用的lib库](#%e7%bc%96%e8%af%91vs%e4%bd%bf%e7%94%a8%e7%9a%84lib%e5%ba%93)
- [java 环境准备](#java-%e7%8e%af%e5%a2%83%e5%87%86%e5%a4%87)
  - [install maven](#install-maven)
  - [maven 安装](#maven-%e5%ae%89%e8%a3%85)
  - [java 安装](#java-%e5%ae%89%e8%a3%85)
  - [install IntelliJ IDEA Ultimate v2017.3.5](#install-intellij-idea-ultimate-v201735)



数据结构+算法

# basic 
 110100  -> asm -> fortran/c --->java,

# C/C++环境准备


## windows 10 bash

search "window 10 bash"


## msys2

- msys2 pacman最常用参数
    ```shell
    pacman -Ss #查找软件
    pacman -S  #安装软件
    pacman -Sl #列出支持软件
    pacman -Qi #查看某个软件包信息
    pacman -Ql #列出软件包内容
    ```

- [[下载MSYS2]](http://repo.msys2.org/distrib/x86_64/msys2-x86_64-20161025.exe)
  
  安装好了之后会在菜单栏生产三个命令行工具快捷方式，其中MSYS2 Shell是MSYS2系统管理的命令行工具，MinGW-w64 Win32 Shell和MinGW-w64 Win64 Shell分别是32位和64位下的MinGW

- [[ 给 MSYS2 添加中科大的源]](https://www.cnblogs.com/findumars/p/6546088.html)
  - mirrorlist.msys
  
  ```txt
  Server = http://mirrors.ustc.edu.cn/msys2/msys/$arch/
  Server = http://repo.msys2.org/msys/$arch
  Server = http://downloads.sourceforge.net/project/msys2/REPOS/MSYS2/$arch
  Server = http://www2.futureware.at/~nickoe/msys2-mirror/msys/$arch/
  ```
  
  - mirrorlist.mingw64
  
  ```txt
  Server = http://mirrors.ustc.edu.cn/msys2/mingw/x86_64/
  Server = http://repo.msys2.org/mingw/x86_64
  Server = http://downloads.sourceforge.net/project/msys2/REPOS/MINGW/x86_64
  Server = http://www2.futureware.at/~nickoe/msys2-mirror/x86_64/
  Server = http://mirror.bit.edu.cn/msys2/REPOS/
  ```
  
  - mirrorlist.mingw32
  
  ```txt
  Server = http://mirrors.ustc.edu.cn/msys2/mingw/i686/
  Server = http://repo.msys2.org/mingw/i686
  Server = http://downloads.sourceforge.net/project/msys2/REPOS/MINGW/i686
  Server = http://www2.futureware.at/~nickoe/msys2-mirror/i686/
  ```

- 更新MSYS2系统环境
  - 运行msys2_shell.bat
  - [[更新软件数据库,然后退出]] pacman --needed -Sy bash pacman pacman-mirrors msys2-runtime
  - 运行msys2_shell.bat
  - [[更新系統]] pacman -Syu
- 安装基本开发环境和常用工具
  - pacman -S  base-devel  git  mercurial  cvs  wget  p7zip  perl  ruby  python2
  -   
- 安装 MinGW-w64
  - pacman -S  mingw-w64-x86_64-toolchain

## install clionclion

- [[download path]](https://blog.jetbrains.com/clion/2017/11/clion-2017-3-release-candidate/)
- [[get liecense]](http://idea.lanyus.com/)
   -  Clion作为一个目前在unix类操作系统上最好的C/C++ IDE,引得无数在其平台上工作的人使用.然而其激活费用对于学生党来说确实是一笔不小的开支,于是我在网上找到了一个破解方法,在此分享给大家.在clion的激活方式中选择License Server方式,并将服务器设置为:http://idea.lanyus.com/即可成功激活!
- search "clion 配置环境" keywords to install clion



## 关于命令行

- [[gcc编译选项]](http://blog.chinaunix.net/uid-20737871-id-1881211.html)

-  在  <<程序员的自我修养-链接装载与库>>一书中曾提到ld默认使用的链接脚本说默认在/usr/lib/ldscripts/下结果我找了半天没找到  我的系统是fedora 8  find / -name *lds*  找不到 找script也找不到 晕了后来查了下发现可以用ld -verbose查看默认的lds
输出如下：
    ```text
    GNU ld version 2.19.51.0.14-34.fc12 20090722
    Supported emulations:
    elf_i386
    i386linux
    elf_x86_64
    using internal linker script:
    ==================================================
    /* Script for -z combreloc: combine and sort reloc sections */
    OUTPUT_FORMAT("elf32-i386", "elf32-i386",
    "elf32-i386")
    OUTPUT_ARCH(i386)
    ENTRY(_start)
    SEARCH_DIR("/usr/i686-redhat-linux/lib"); SEARCH_DIR("/usr/local/lib"); SEARCH_DIR("/lib"); SEARCH_DIR("/usr/lib");
    SECTIONS
    {
    /* Read-only sections, merged into text segment: */
    PROVIDE (__executable_start = SEGMENT_START("text-segment", 0x08048000)); . = SEGMENT_START("text-segment",
    0x08048000) + SIZEOF_HEADERS;
    ........
    后面略
    这就奇怪了，那么这个lds到底在哪呢？
    我们用find /usr/* |xargs grep "Script for -z combreloc"看看
    结果 Binary file /usr/bin/ld matches 可见已经集成到ld中了
    readelf -p --string-dump=.rodata ld便可以找到
    ```

## cmake

- [[awesome-cmake]](https://github.com/onqtam/awesome-cmake)

## atuomake makefile

写了一个范例，见 [[demo for automake]](https://github.com/flyingyizi/algorithm)


- 进入mingw64 shell环境,因为前面只安装了mingw64，所以只有在mingw64下才会有gcc等
  ```shell
  msys2_shell.cmd -mingw64
  ```
    以下面例子为例演示
    ```text
    $ mkdir test 
    $ cd test
    $ cp ../helloword.c ./ 
    ```
- 参考

  [[ 通过configure.ac文件生成copnfigure文件 ]](https://blog.csdn.net/babybirdtofly/article/details/69941756)

  [[例解 autoconf 和 automake 生成 Makefile 文件]](https://www.ibm.com/developerworks/cn/linux/l-makefile/)

  [[使用 GNU autotools 改造一个软件项目]](http://www.cppblog.com/liu1061/articles/54740.html)

- automake说明
   - 1）运行autoscan命令，产生configure.scan文件；
   - 2）将configure.scan 文件重命名为configure.ac，并修改configure.ac文件；
   - 3）运行aclocal命令，产生aclocal.m4文件；
   - 4）运行autoconf命令，产生configure文件；
   - 5）新建Makefile.am文件，新建NEWS、 README、 ChangeLog 、AUTHORS文件；
   - 6）修改Makefile.am，并运行automake -a命令；(automake --add-missing)
   - 7）运行./confiugre脚本。
   - 8）至此，Makefile已生产，然后执行make编译即可。

        ```graphviz
        digraph idp_modules{
            node [fontsize = 12, shape = "record" ];
            edge [fontsize = 12 ];

            source_code [label="Source code"];
            configure_scan [label="configure.scan"];
            configure_ac [label="configure.ac"];
            aclocal_m4 [label="aclocal.m4"];
            makefile_am [label="Makefile.am"];
            makefile_in [label="Makefile.in"];
            config_h_in [label="config.h.in"];
            configure [label="configure"];
            p [shape = "point"];
            final [shape = "point"];
            edit [shape = "point"];
            run [label="Makefile",style=filled,color="lightblue",shape="plaintext",skew=.4];

            source_code -> configure_scan [label="1):autoscan"];
            configure_scan -> configure_ac [label="2):edit"];

            configure_ac  -> config_h_in [label="autoheader"];
            configure_ac  -> aclocal_m4 [label="3):aclocal"];
            
            configure_ac -> p [dir=none,color="yellowgreen"] ;
            aclocal_m4 -> p[dir=none,color="yellowgreen"];
            p -> configure [label="4):autoconf"];

            edit -> makefile_am         [label="5):manual generate"]
            makefile_am  -> makefile_in [label="6):automake --add-missing"];

            makefile_in -> final [ color="yellowgreen", arrowhead="none"] ;
            config_h_in -> final [ color="yellowgreen", arrowhead="none"];
            configure -> final  [ color="yellowgreen", arrowhead="none"];   
            final -> run [label="7):run .\\configure"];
        }
        ```

- 从configure.scan得到configure.ac

除去中文，其他内容都是autoscan自动生成的，下面是如何编辑这个模板从而生成我们需要的"configure.ac"的说明
```shell
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
### AC_INIT 宏必须在前面，用来定义软件的名称和版本等信息,比如写为“AC_INIT(demo, 1.0)”
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
### AC_CONFIG_SRCDIR 宏通过侦测所指定的源码文件是否存在，来确定源码目录的有效性可以
### 选择源码目录中的任何一个文件作为代表
AC_CONFIG_SRCDIR([helloworld.c])
### AC_CONFIG_HEADER 宏用于生成 config.h 文件，里面存放 configure 脚本侦
### 测到的信息如果程序需要使用其中的定义，就在源码中加入“#include <config.h>”
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

### 简单的 Makefile.in 可以手动编写，如果使用 automake 产生，需要在 configure.ac
### 里面加入 AM_INIT_AUTOMAKE 宏进行声明
### 要输出 Makefile，还需要在 configure.ac 中使用 AC_CONFIG_FILES 宏指明该宏并不
### 是只处理 Makefile，而是将 FILE.in 文件转换为 FILE 文件因为 make 可以遍历子目录，
### 如果子目录中存在 Makefile，也将同时处理在本例中 src 目录下是源码，其他是
### 数据文件，可以使用单独一个 Makefile 放在根目录下面，也可以用多个 Makefile由于
### 每个子目录的 Makefile 只处理本目录的文件，分工明确，是模块化的方法，推荐使用
### 因此在 configure.ac 里面增加下面的宏，表示软件根目录和子目录中都需要生成
### Makefile 文件:
### 
### AC_CONFIG_FILES([Makefile
### src/Makefile
### data/Makefile
### docs/Makefile
### fonts/Makefile
### images/Makefile
### music/Makefile
### sound/Makefile])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
```

- 编写 Makefile.am



- error
   出现 $ autoscan

   Unescaped left brace in regex is deprecated, passed through in regex; marked by <-- HERE in m/\${ <-- HERE [^\}]*}/ at /usr/bin/autoscan line 361.

   将361行的左大括号“}”删掉即可，这时因为新版的perl不在支持左大括号的使用，删掉大括号，问题解决。


  
# c and cplusplus 编程语言

- [[ C 语言main 函数终极探秘 ]](https://blog.csdn.net/daiyutage/article/details/8579820)

## 获取软件源码
- [[Where can I find source code for Linux core commands]](https://stackoverflow.com/questions/11528267/where-can-i-find-source-code-for-linux-core-commands)
- [[github站点]](github.com)
- [[rosettacode.org]](https://rosettacode.org/wiki/Category:Sorting_Algorithms)
- [[stackoverflow.com]](https://stackoverflow.com/)
- [[GNU软件列表]](http://ftp.gnu.org/)



## 常量
```c++
#define CONST_VALUE_FOR_TUZE  4
const int  xyz = 1;
```

- keyword: int,  short, float, long, long long,char    

- [[理解字节序]](http://www.ruanyifeng.com/blog/2016/11/byte-order.html)
    ```c++
    union A{
        int16_t  value;
        char b[2];
    };
        A x;
        x.value = 0x3437;
        std::cout << x.b[0] << ":" <<x.b[1] << std::endl;
    ```

- normal input/output
    ```c++
    #include <iostream>

        char str[8];
        cin.getline(str, 5);
        cout<<str<<endl;
        cin.getline(str, 5);
        cout<<str<<endl;
    ```

- 关系运算符

- && 条件判断中logic and. 比如 true && false == false
- & 运算中logic and. 比如   0x01 & 0x11 ==  0x00

## 控制语句举例

   ```c++
    #include <iostream> 
    using namespace std;
    
    int main(){
        int a = 10;
        int b = 9;
    
        //if else语句
        if(a>b)cout<<"a>b";
        else if(a == b)cout<<"a==b";
        else cout<<"a<b"<<endl;
        
        //switch语句
        switch(a){
            case 1:
                cout<<"a==1";
                break;
            case 2:
                cout<<"a==2";
                break;
            case 3:
                cout<<"a==3";
                break;
            default:
                cout<<"a is not (1,2,3)";
                break;
        } 
        cout<<endl;
    
        //for
        for(int i=0;i<3;i++){
            cout<<i<<endl;
        }
    
        //while语句
        int j = 3;
        while(j>0){
            cout<<j<<endl;
            j--;
        }
    
        //do{}while();
        int k = 1;
        do{
            k ++;
            cout<<k<<endl;
        }while(k<3);
    
        //goto是支持的，但是是不推荐使用的。
        
        //bool?
        if(k)cout<<k<<"is true"<<endl;
    
        //在c++中if(k=0)是没有语法问题的，会给k赋值，而非判断k是否=0所以c++中习惯这么写
        //把常量放在前面
        if(0==k)cout<<"k==0"<<endl;     
    }
   ```
## 变量作用域

- static修饰符

    在C语言中，static修饰符用于定义静态变量。static除了可以修饰变量外，还可以修饰函数。对于外部变量和函数，static修饰符会限制它们的作用域，使它们只能在定义它们的文件中使用，作用域不能传播到文件外。使用static修饰符，提供了一种隐藏名字的方式，可以将名字限定在一个文件中，防止名字冲突。
    ```c
    static char buf[BUFSIZE];
    static int bufp = 0;
    int getch(void) { ... }
    void ungetch(int c) { ... }
    ```
    在上面的代码中，bufp和buf都只能在函数getch和ungetch中使用，而不能在别的文件中使用，即使在别的文件中使用了extern声明。

    static修饰符除了可以修饰外部变量和函数外，也可以作用于局部变量。通过static修饰的局部变量，在函数中的存储空间将不会再函数结束后销毁，而是一直存在直到程序结束。当程序再次被调用时，仍旧可以访问这个区域的内存。    

## 数组array

所谓数组就是在内存中一片连续的空间。数组名字就是指向这片连续空间的指针。

- 数组sample
    ```c
    int a[10]={0,1,2,3,4,5,6,7,8,9}; //说明整型数组a，有10个元素。并初始化
    float b[10]={1},c[20]; //说明实型数组b，有10个元素，实型数组c，有20个元素。
    char ch[20]; //说明字符数组ch，有20个元素。
    int d[10][20] ; // 二维数组
    ```
- 对一个m*n列的数组， 二维表示法与一维表示法是可以转换的，如下：
   
$$ array[i][j] == array[i \ast n + j]  $$ 

- array[i]这个如果以指针的来描述它，它表达的是array这个指针加i偏移的位置。
    ```c
    array[i]=1;  等效为  *(array+i)=1;
    ```
- 数组表示法与指针运算的关系
    ```c
    char *str="hello world";
    // 以下两种的结果是一样的，这演示了数组表示法的实质是什么，即相对数组头地址偏移多少位
            std::cout << str[i] << " ";
            std::cout << *(str+i) << " ";
    ```        

- argc, argv用法举例
    ```c
    #include <stdio.h>
    int main(int argc,char *argv[ ] )  
        {
            printf("argc:  %d.\n",argc);
            for(int i=0; i<argc; i++){
                printf("argv[%d]:  %s.\n",i,argv[i]);	        	
            }
            return 0;
        }
    ```

## 指针

```c

int i;
// 这个表达式的作用是将变量i的地址值存放到指针变量p中去，即指针变量p的值就是变量i的地址值；
int* p = &i;
//void* p= &i;

```

```c
typedef int (*PTRFUN)(int,double, double*);
int demo(int a,double b, double* c){
    *c = a+b;
    return *c;
}
```




# mingw gcc常用命令

### [[dump class layout]](https://stackoverflow.com/questions/2549618/is-there-any-g-option-to-dump-class-layout-and-vtables)
    ```shell
    g++ -fdump-class-hierarchy -c source_file.cpp
    ```
### get asm code
    ```shell
    gcc -S source_file.cpp
    ```

### Include Path for use with MinGW Compilers 

  [[Include Path for use with MinGW Compilers]](http://www.mingw.org/wiki/includepathhowto)

    ```shell
    gcc -v -c source_file.cpp
    ```

### 生成share library

```sh
$mingw32-gcc -m32 -c mydll.c
#再连接成dll：
$mingw32-gcc -m32 -shared - mydll.dll mydll.o
```


### 编译vs使用的lib库

    - 生成lib
    ```shell
    // generate def file
    gendef xxx.dll
    // generate lib file
    dlltool -D xxx.dll -d xxx.def -l xxx.lib
    ```
   - MinGw未生成.dll动态库
    ```shell
    // 在configure时使用下面的参数
    ./configure --disable-static --enable-shared
    ```
   - 直接使用.dll.a
    
    mingw编译出来的静态库后缀名为.a，编译出来的动态库的导入库后缀名为.dll.a，而在windows下后缀名为.lib的库可能是静态库也可能是动态库的导入库。
    mingw编译出来的动态库的导入库.dll.a可以直接在vc中直接使用，例如：
    ```c
    #pragma comment(lib, "xxx.dll.a")
    ```


# java 环境准备

## install maven 

## maven 安装

  - 下载常用的Maven版本，如apache-maven-3.5。下载地址:http://maven.apache.org/download.html.

    - [[download path]](http://mirror.bit.edu.cn/apache/maven/maven-3/3.5.2/binaries/apache-maven-3.5.2-bin.zip)

    - 解压到"d:\prog\apache-maven-3.5.2"目录下。
    - 添加下面的环境变量:
     
        export M3_HOME=d:\prog\apache-maven-3.5.2
     
        export PATH=%M3_HOME%\bin;%PATH%
    - 查看mvn版本，看是否配置正确。命令：
        mvn –version

    - Item0：更改maven 国内源设置,以及库今后存放的目录。更改settings.xml (全局的位置是 d:\prog\apache-maven-3.5.2\conf\settings.xml)
        
        源修改      
        ```xml
        <mirrors>
            <mirror>
            <id>alimaven</id>
            <name>aliyun maven</name>
            <url>http://maven.aliyun.com/nexus/content/groups/public/</url>
            <mirrorOf>central</mirrorOf>        
            </mirror>
        </mirrors>
        ```
        库存放位置修改(替换为需要的位置)
        ```xml
        <localRepository>C:\home\atmel\.m2\repository</localRepository>
        ```

    - Item 1:  最简单maven pom.xml文件
        ```xml
        <project xmlns="http://maven.apache.org/POM/4.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/maven-v4_0_0.xsd">
        <modelVersion>4.0.0</modelVersion>

        <groupId>com.txq</groupId>
        <artifactId>demo</artifactId>
        <version>0.0.1-SNAPSHOT</version>
        <packaging>pom</packaging>
        <name>demo</name>
        </project>
        ```
    - Item2：凭空创建一个项目
        ```shell
        ~#:mvn archetype:generate
        ```


## java 安装
       
   - 步骤1.1：下载JDK
        通过访问链接下载最新版本的JDK，并下载常见java版本的[[Java]](http://www.oracle.com/technetwork/java/javase/downloads/index.html)。

       推荐是JDK 8，例如jdk1.8.0_14464.msi，在你的机器上下载该文件。

   - 步骤1.2：安装
        双击jdk1.8.0_14464.msi，选择安装目录进行安装
        
   - 步骤1.3：设置路径

        要设置路径和JAVA_HOME变量，将下面两个添加到环境变量中。
        ```shell
        set JAVA_HOME = C:\prog\Java\jdk1.8.0_144
        set PATH=$PATH:%JAVA_HOME%\bin
        ```

   - 步骤1.5：检查是否安装成功
        ```shell
        java  -version
        ```

## install IntelliJ IDEA Ultimate v2017.3.5
   -  [[idea 2017.3.5]](http://www.0daydown.com/03/765405.html)
   -  [[Download 百度云]](https://pan.baidu.com/s/1D0SKQyFUKyq6FnEFzkfWSA)  密码: xveu

int a[] = {4, 65, 2, -31, 0, 99, 2, 83, 782, 1};

```graphviz
digraph idp_modules{

0 [label="4"];
1 [label="65"];
2 [label="2"];
3 [label="-31"];
4 [label="0"];
5 [label="99"];
6 [label="2"];
7 [label="83"];
8 [label="782"];
9 [label="1"];




}
```