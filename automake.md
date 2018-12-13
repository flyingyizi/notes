

- [[link]](https://www.gnu.org/software/automake/manual/automake.html#Introduction)


# 介绍

Automake是一个输入Makefile.am自动生成 Makefile.in的工具。每个Makefile.am文件基本上是一系列的“make valriable”的定义，偶尔也有make rule。生成的Makefile.ins符合GNU Makefile标准。

GNU Makefile标准文档很长，很复杂，并且可能会有变化。Automake的目标是消除Makefile维护的负担（并将其由Automake维护者承担）。

典型的Automake输入文件只是一系列“variable definitions”。每个这样的文件都被处理来创建一个Makefile.in。

Automake一定程度上限定了project的组织方式; 例如，它假定该project使用autoconf，并被configure.ac中的某些内容强制约束。

Automake在生成 Makefile.ins过程中需要perl。但是，由Automake构造的结果是完全符合GNU标准，并且不需要perl构建的。

# 2 Autotools简介

如果你是Automake的新手，也许你知道它是一组名为The Autotools的工具的一部分。也许你已经钻研了一个完整的命名文件包配置， configure.ac， Makefile.in， Makefile.am， aclocal.m4，...，其中一些声称是由 Autoconf或Automake 生成的。但是这些文件及其关系的确切目的可能是模糊的。本章的目标是向你介绍相关机制，向你展示它是如何工作的以及它有多强大。如果您从未安装过或看过过这样的软件包，请不要担心：本章将引导您完成此过程。

如果您需要一些教学材料，更多插图或automake更深入的内容，可以在Alexandre Duret-Lutz的[[Autotools教程]](http://www.lrde.epita.fr/~adl/autotools.html)中找到有关此介绍的一些幻灯片 。本章是他教程第一部分的书面版本。

- • GNU Build System:	  	Introducing the GNU Build System
- • Use Cases:	  	Use Cases for the GNU Build System
- • Why Autotools:	  	How Autotools Help
- • Hello World:	  	A Small Hello World Package 


## 2.1 Introducing the GNU Build System

这是一个普遍公认的事实，新开发软件的的开发人员，你必然需要一个构建系统。

在Unix世界中，这样的构建系统是使用传统的make命令，在makefile文件中你记录你的要求来构建软件包。这个文件是一组rules来构建软件包中的文件。例如程序PROG 可以program prog may be built by running the linker on the files main.o, foo.o, and bar.o; the file main.o may be built by running the compiler on main.c; 等等 。每次make运行，它都会读取Makefile文件，检查提到的文件的存在和修改时间，确定需要构建（或重建）的文件，并运行相关的命令。

当一个软件包需要建立在不同于它开发的平台上时， Makefile文件通常需要调整。例如，编译器可能有其他名称或需要更多选项。1991年，David J. MacKenzie厌倦了定制 Makefile文件他必须处理的20个平台。取而代之的是，他手工制作了一个名为“shell”的脚本配置 自动调整 Makefile文件(see Genesis in The Autoconf Manual)。编译他的软件包现在和运行一样简单./configure && make。

今天，这个过程已经在GNU项目中得到了标准化。GNU编码标准解释了GNU项目的每个包如何应该有一个配置脚本以及它应该拥有的最小界面。该Makefile文件也应遵循一些既定的惯例。结果就是一个统一的构建系统使得安装者几乎不用区分不同的软件包了。在最简单的情况下，所有安装者无非要做的就是 unpack the package, run ./configure && make && make install, 并重复安装下一个包。

我们把这个构建系统称为GNU构建系统，因为它是从GNU项目发展而来的。然而，它被大量的其他软件包使用：遵循任何现有的约定都有其优点。

Autotools是为你的软件包创建一个GNU构建系统的工具。Autoconf主要关注于configure，Automake 关注于Makefiles。完全可以在没有这些工具的帮助下创建一个GNU构建系统。然而，它相当繁重和容易出错。在对GNU构建系统进行了一些说明之后，我们将再次讨论这个问题。


## 2.2 Use Cases for the GNU Build System

在本节中，我们将探讨GNU构建系统的一些用例。你可以在amhello-1.0.tar.gz package 上实践这些例子。如果automake已经在你的系统安装了，你可以从prefix/share/doc/automake/amhello-1.0.tar.gz得到一份该包的副本，如果找不到，你还可以从automake package的 doc/目录下得到。

我们的目标是向您展示由Autotools创建的构建系统将为您提供的许多功能。
- • Basic Installation:	  	Common installation procedure
- • Standard Targets:	  	A list of standard Makefile targets
- • Standard Directory Variables:	  	A list of standard directory variables
- • Standard Configuration Variables:	  	Using configuration variables
- • config.site:	  	Using a config.site file
- • VPATH Builds:	  	Parallel build trees
- • Two-Part Install:	  	Installing data and programs separately
- • Cross-Compilation:	  	Building for other architectures
- • Renaming:	  	Renaming programs at install time
- • DESTDIR:	  	Building binary packages with DESTDIR
- • Preparing Distributions:	  	Rolling out tarballs
- • Dependency Tracking:	  	Automatic dependency tracking
- • Nested Packages:	  	The GNU Build Systems can be nested 


### 2.2.1 Basic Installation

最常见的安装过程如下所示.
```shell
~ % tar zxf amhello-1.0.tar.gz
~ % cd amhello-1.0
~/amhello-1.0 % ./configure
…
config.status: creating Makefile
config.status: creating src/Makefile
…
~/amhello-1.0 % make
…
~/amhello-1.0 % make check
…
~/amhello-1.0 % su
Password:
/home/adl/amhello-1.0 # make install
…
/home/adl/amhello-1.0 # exit
~/amhello-1.0 % make installcheck
…
```

用户首先解包该包装。在这里，在下面的例子中，为了简单起见，我们将使用不可移植的命令tar zxf。在没有tar安装GNU的系统上，该命令应该是类似gunzip -c amhello-1.0.tar.gz | tar xf -。

用户然后进入新创建的目录运行 配置脚本。这个脚本探测系统的各种功能，并最终创建Makefile文件。在这个demo例子中只有两个Makefile文件但在现实世界的项目中，可能还有更多，通常是每个目录都有一个 Makefile文件 。

现在可以运行make。这将构建package需要的programs, libraries, and scripts 。在我们的例子中，这就编译好了hello程序。所有文件都是在source tree中构建的; 我们稍后会看到如何改变这一点。

make check使得package's tests 运行。此步骤不是强制性的，但在决定安装它们之前，确保已构建的程序按照它们应该的方式运行是很好的。我们的例子不包含任何测试，所以运行make check是没有任何操作的。

一切built，或还有tested后，现在是时候将它安装在系统上。这意味着将programs, libraries, header files, scripts, and other data files从源目录复制到系统上的最终目标。该命令make install将做到这一点。但是，默认情况下，所有的东西都会被安装在/usr/local的子目录中：二进制文件将进入/usr/local/bin目录，libraries将在/usr/local/lib目录等等。这个目的地通常不能被任何用户写入，所以我们假设我们必须成为root才能运行make install。在我们的例子中，运行make install 将copy program hello into /usr/local/bin and README into /usr/local/share/doc/amhello.。

最后一个可选步骤是运行make installcheck。该命令可以对已经安装的文件运行测试。

目前大多数软件包都没有任何installcheck测试，因为它的存在installcheck是鲜为人知的，它的用处被忽略了。我们的小玩具包并不是最好的：make installcheck什么也没有做。

### 2.2.2 Standard Makefile Targets

到目前为止，我们在GNU编译系统见识了四种方式运行make：make，make check，make install，和 make installcheck。The words check, install, and installcheck, passed as arguments to make, are called targets. 。 make是make all的简写， all是GNU构建系统中的默认目标。

以下是GNU编码标准指定的最有用的目标列表。

        make all
            Build programs, libraries, documentation, etc. (same as make). 
        make install
            Install what needs to be installed, copying the files from the package’s tree to system-wide directories. 
        make install-strip
            Same as make install, then strip debugging symbols. Some users like to trade space for useful bug reports..
        make uninstall
            与之相反的是make install：擦除已安装的文件。（这需要从安装的同一个build tree中运行。）
        make clean
            从构建树中删除构建的文件make all。
        make distclean
            另外擦除./configure创建的任何东西。
        make check
            运行测试套件（如果有的话）。
        make installcheck
            Check the installed programs or libraries, if supported. 
        make dist
            Recreate package-version.tar.gz from all the source files. 

### 2.2.3 Standard Directory Variables

GNU编码标准还指定了用于表示安装目录的变量层次结构。其中一些是：
| Directory variable | Default value                 |
| ------------------ | ----------------------------- |
| prefix             | /usr/local                    |
| exec_prefix        | ${prefix}                     |
| bindir             | ${exec_prefix}/bin            |
| libdir             | ${exec_prefix}/lib            |
| …                  |                               |
| includedir         | ${prefix}/include             |
| datarootdir        | ${prefix}/share               |
| datadir            | ${datarootdir}                |
| mandir             | ${datarootdir}/man            |
| infodir            | ${datarootdir}/info           |
| docdir             | ${datarootdir}/doc/${PACKAGE} |
| …                  |                               |

这些目录中的每一个都有一个通常从其名称就能看出来的角色。在一个包中，任何可安装的文件都将被安装在这些目录之一中。例如amhello-1.0，在程序中 你好将被安装在二进制文件的目录bindir中。此目录的默认值是 在/usr/local/bin目录，但用户在call configure 时可以提供不同的值。同时readme文件将被安装到默认的docdir中， 默认是/usr/local/share/doc/amhello.

作为用户，如果您希望自行安装软件包，您可以按照以下步骤操作：
```shell
~/amhello-1.0 % ./configure --prefix ~/usr
…
~/amhello-1.0 % make
…
~/amhello-1.0 % make install
…
```
This would install ~/usr/bin/hello and ~/usr/share/doc/amhello/README.

The list of all such directory options is shown by ./configure --help. 

### 2.2.4 Standard Configuration Variables

GNU编码标准还定义了在构建期间使用的一组标准配置变量。其中一些是：

        CC
            C compiler command 
        CFLAGS
            C compiler flags 
        CXX
            C++ compiler command 
        CXXFLAGS
            C++ compiler flags 
        LDFLAGS
            linker flags 
        CPPFLAGS
            C/C++ preprocessor flags 
        …

为上述variables设置合适的值，configure通常都做的很好，但有些情况下您可能想要覆盖它们。例如，您可能安装了多个版本的编译器，并且希望使用另一个版本，您可能会在编译器的缺省搜索路径之外安装头文件，或者甚至是使用链接器之外的库。

下面演示了是如何调用configure来强制它使用 gcc-3作为C编译器，在编译时使用~/usr/include中的头文件，以及在链接时使用~/usr/lib中的libraries 。
```bash
~/amhello-1.0 % ./configure --prefix ~/usr CC=gcc-3 \
CPPFLAGS=-I$HOME/usr/include LDFLAGS=-L$HOME/usr/lib
```
Again, a full list of these variables appears in the output of ./configure --help. 

### 2.2.5 Overriding Default Configuration Setting with config.site

当使用同样的配置安装多个package时，使用一个文件来记录common settings会方便很多。如果存在一个“ prefix/share/config.site ”的文件，configure将会首先“source it”然后再执行其他。

回想一下上一节中的命令：
```bash
~/amhello-1.0 % ./configure --prefix ~/usr CC=gcc-3 \
CPPFLAGS=-I$HOME/usr/include LDFLAGS=-L$HOME/usr/lib
```
假设我们要在“~/usr”安装很多个package, 并且希望使用同样的“ CC, CPPFLAGS, and LDFLAGS”定义,我们可以通过创建下面的“~/usr/share/config.site file”来自动化 :
```bash
test -z "$CC" && CC=gcc-3
test -z "$CPPFLAGS" && CPPFLAGS=-I$HOME/usr/include
test -z "$LDFLAGS" && LDFLAGS=-L$HOME/usr/lib
```
Now, any time a configure script is using the ~/usr prefix, it will execute the above config.site and define these three variables.
```bash
~/amhello-1.0 % ./configure --prefix ~/usr
configure: loading site script /home/adl/usr/share/config.site
…
```

有关此功能的更多信息,参见 [[Setting Site Defaults]](http://www.gnu.org/software/autoconf/manual/autoconf.html#Site-Defaults) 

### 2.2.6 Parallel Build Trees (a.k.a. VPATH Builds)

GNU构建系统对“the source tree, and the build tree.”区别对待 。

“source tree”是指包含“configure”的目录。它包含所有的源文件，并可能有几个子目录。

“build tree”是指“configure”运行时所在的目录，它会被所有的“all object files, programs, libraries, and other derived files built from the sources (and hence not distributed)”所填充。“build tree”通常与“source tree”具有相同的子目录布局; “build tree”的子目录由构建系统自动创建。

如果configure是在其自己的目录中执行的，源代码和构建树结合在一起：派生文件与源代码构建在相同的目录中。我们的第一个安装示例就是这种情况（请参阅基本安装）。

通常用户希望将“all derived files ”限制在一个单独的目录中，从而保持source directories的整洁。下面是我们如何在一个名为“build/”的子目录中运行configure去构建的例子.
```bash
~ % tar zxf ~/amhello-1.0.tar.gz
~ % cd amhello-1.0
~/amhello-1.0 % mkdir build && cd build
~/amhello-1.0/build % ../configure
…
~/amhello-1.0/build % make
…
```
通过这样的设置，使得source tree与build tree不一样的方式，经常被称作“parallel builds or VPATH builds”。 称为“parallel builds”一定程度上是有误导的。“parallel ”一词是“build tree”相对“source tree”而言，它并不涉及构建命令运行方式中的某些并发性。出于这个原因，我们后续使用名称VPATH 来指这样的设置。 VPATH是在Makefiles中使用的一种make feature (see VPATH Search Path for All Prerequisites in The GNU Make Manual).

VPATH构建还有其他有趣的用途。一种是用多种配置构建相同的源。例如：
```bash
~ % tar zxf ~/amhello-1.0.tar.gz
~ % cd amhello-1.0
~/amhello-1.0 % mkdir debug optim && cd debug
~/amhello-1.0/debug % ../configure CFLAGS='-g -O0'
…
~/amhello-1.0/debug % make
…
~/amhello-1.0/debug % cd ../optim
~/amhello-1.0/optim % ../configure CFLAGS='-O3 -fomit-frame-pointer'
…
~/amhello-1.0/optim % make
…
```
使用网络文件系统，可以使用类似的方法在不同的机器上构建相同的资源。例如，假设source源安装在由两台主机共享的目录中：HOST1 、HOST2可能是不同的平台。
```bash
~ % cd /nfs/src
/nfs/src % tar zxf ~/amhello-1.0.tar.gz
```
在第一台主机上，您可以创建一个本地build directory：
```bash
[HOST1] ~ % mkdir /tmp/amh && cd /tmp/amh
[HOST1] /tmp/amh % /nfs/src/amhello-1.0/configure
...
[HOST1] /tmp/amh % make && sudo make install
...
```
(Here we assume that the installer has configured sudo so it can execute make install with root privileges; it is more convenient than using su like in Basic Installation).

在第二台主机上，你可能会在同一时间做同样的事情：
```bash
[HOST2] ~ % mkdir /tmp/amh && cd /tmp/amh
[HOST2] /tmp/amh % /nfs/src/amhello-1.0/configure
...
[HOST2] /tmp/amh % make && sudo make install
...
```
在这种情况下，并不要求 /nfs/src/amhello-1.0 目录不能是只读。实际上，VPATH构建也是从只读介质（如CD-ROM）构建软件包的一种手段。（在GNU项目变得如此之大之前，FSF过去常销售带有解压源代码的CD-ROM。）

### 2.2.7 Two-Part Installation

在我们的最后一个示例中（参见VPATH构建版本），source tree被两台主机共享，但编译和安装分别在每台主机上完成。

GNU构建系统还支持联网设置，其中部分安装文件应该在多个主机之间共享。它通过区分独立于架构的文件和独立于架构的文件并提供两个文件来实现Makefile文件 目标来安装这些类文件中的每一个。

这些目标是install-exec针对体系结构相关文件和install-data独立于体系结构的文件。我们迄今使用的命令"make install"可以被认为是一种简写"make install-exec install-data"。

从GNU构建系统的角度来看，依赖于体系结构的文件和独立于体系结构的文件之间的区别仅限于用于指定其安装目标的目录变量。在我们前面提供的目录变量列表中（请参阅标准目录变量），基于exec-prefix的所有变量均指定其文件将由其安装的依赖于体系结构的目录make install-exec。其他人指定独立于架构的目录，并提供由其安装的文件make install-data。有关更多详细信息，请参阅安装的两个部分。

以下是我们如何重新访问我们的双主机安装示例，假设（1）我们想要直接安装包到“/usr”，并且（2）目录“/usr/share”  由两个主机共享。

在第一台主机上，我们将运行

```bash
[HOST1] ~ % mkdir /tmp/amh && cd /tmp/amh
[HOST1] /tmp/amh % /nfs/src/amhello-1.0/configure --prefix /usr
...
[HOST1] /tmp/amh % make && sudo make install
...
```
在第二台主机上，我们只需要安装特定于体系结构的文件。
```bash
[HOST2] ~ % mkdir /tmp/amh && cd /tmp/amh
[HOST2] /tmp/amh % /nfs/src/amhello-1.0/configure --prefix /usr
...
[HOST2] /tmp/amh % make && sudo make install-exec
...
```

在具有安装检查的软件包中，运行make installcheck（see Basic Installation）是有意义的， 以验证软件包是否可以正常工作，尽管显然是部分安装。

### 2.2.8 Cross-Compilation
交叉编译是在一个平台build，将在另一个平台上运行。在谈到交叉编译时，区分执行编译的构建平台(build pathform)和预期生成可执行文件的主机平台很重要。以下 configure选项用于指定它们中的每一个：

        --build=build
          The system on which the package is built. 
        --host=host
          The system where built programs and libraries will run. 
当使用“--host”选项， configure会搜索此平台的交叉编译套件。交叉编译工具通常将其目标体系结构作为其名称的前缀。比如我的交叉编译器对Mingw32有专门的名为i586-mingw32msvc-gcc，i586-mingw32msvc-ld， i586-mingw32msvc-as，等的运行文件。

下面是演示在GNU/Linux PC上为i586-mingw32msvc构建amhello-1.0。

```bash
~/amhello-1.0 % ./configure --build i686-pc-linux-gnu --host i586-mingw32msvc
checking for a BSD-compatible install... /usr/bin/install -c
checking whether build environment is sane... yes
checking for gawk... gawk
checking whether make sets $(MAKE)... yes
checking for i586-mingw32msvc-strip... i586-mingw32msvc-strip
checking for i586-mingw32msvc-gcc... i586-mingw32msvc-gcc
checking for C compiler default output file name... a.exe
checking whether the C compiler works... yes
checking whether we are cross compiling... yes
checking for suffix of executables... .exe
checking for suffix of object files... o
checking whether we are using the GNU C compiler... yes
checking whether i586-mingw32msvc-gcc accepts -g... yes
checking for i586-mingw32msvc-gcc option to accept ANSI C...
…
~/amhello-1.0 % make
…
~/amhello-1.0 % cd src; file hello.exe
```
hello.exe: MS Windows PE 32-bit Intel 80386 终端可执行文件

“--host and --build”这两个选项通常就是我们在交叉编译时需要关心的选项。唯一的例外是如果正在构建的包本身是一个交叉编译器：我们需要第三个选项来指定其目标架构。
        
        --target=target

### 2.2.9 Renaming Programs at Install Time
The GNU Build System provides means to automatically rename executables and manpages before they are installed (see Man Pages). This is especially convenient when installing a GNU package on a system that already has a proprietary implementation you do not want to overwrite. For instance, you may want to install GNU tar as gtar so you can distinguish it from your vendor’s tar. 
This can be done using one of these three configure options. 

        --program-prefix=prefix
        Prepend prefix to installed program names. 
        --program-suffix=suffix
        Append suffix to installed program names. 
        --program-transform-name=program
        Run sed program on installed program names. 
The following commands would install hello as /usr/local/bin/test-hello, for instance. 
```bash
~/amhello-1.0 % ./configure --program-prefix test-
…
~/amhello-1.0 % make
…
~/amhello-1.0 % sudo make install
…
```

### 2.2.10 Building Binary Packages Using DESTDIR
The GNU Build System’s make install and make uninstall interface does not exactly fit the needs of a system administrator who has to deploy and upgrade packages on lots of hosts. In other words, the GNU Build System does not replace a package manager. 
Such package managers usually need to know which files have been installed by a package, so a mere make install is inappropriate. 
The DESTDIR variable can be used to perform a staged installation. The package should be configured as if it was going to be installed in its final location (e.g., --prefix /usr), but when running make install, the DESTDIR should be set to the absolute name of a directory into which the installation will be diverted. From this directory it is easy to review which files are being installed where, and finally copy them to their final location by some means. 
For instance here is how we could create a binary package containing a snapshot of all the files to be installed. 
```bash
~/amhello-1.0 % ./configure --prefix /usr
…
~/amhello-1.0 % make
…
~/amhello-1.0 % make DESTDIR=$HOME/inst install
…
~/amhello-1.0 % cd ~/inst
~/inst % find . -type f -print > ../files.lst
~/inst % tar zcvf ~/amhello-1.0-i686.tar.gz `cat ../files.lst`
./usr/bin/hello
./usr/share/doc/amhello/README
```
After this example, amhello-1.0-i686.tar.gz is ready to be uncompressed in / on many hosts. (Using `cat ../files.lst` instead of ‘.’ as argument for tar avoids entries for each subdirectory in the archive: we would not like tar to restore the modification time of /, /usr/, etc.) 
Note that when building packages for several architectures, it might be convenient to use make install-data and make install-exec (see Two-Part Install) to gather architecture-independent files in a single package. 
See Install, for more information. 

### 2.2.13 Nested Packages
Although nesting packages isn’t something we would recommend to someone who is discovering the Autotools, it is a nice feature worthy of mention in this small advertising tour. 
Autoconfiscated packages (that means packages whose build system have been created by Autoconf and friends) can be nested to arbitrary depth. 

一个典型的设置是package A将分发在其子目录中的某个libraries。library B时一个有自己完整构建系统的library B。在configure A的脚本将运行configure B的脚本. 

It is possible to gather several packages like this. GCC is a heavy user of this feature. This gives installers a single package to configure, build and install, while it allows developers to work on subpackages independently. 

When configuring nested packages, the configure options given to the top-level configure are passed recursively to nested configures. A package that does not understand an option will ignore it, assuming it is meaningful to some other package. 

命令 "configure --help=recursive" 可用于显示所有包含软件包支持的选项。. 

See Subpackages, for an example setup. 

## 2.4 A Small Hello World

### 2.4.2 amhello’s configure.ac Setup Explained
让我们从 configure.ac的内容开始.
```bash 
AC_INIT([amhello], [1.0], [bug-automake@gnu.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_OUTPUT
```
这个文件被autoconf（创建configure）,与automake（生成各种Makefile.ins）读取。它是由各种m4宏所组成，这些宏将被扩展为shell code并最终生成configure脚本。这里我们不会详细说明这个文件的语法，因为[[see Writing configure.ac in The Autoconf Manual]](http://www.gnu.org/software/autoconf/manual/autoconf.html#Writing-Autoconf-Input)有详细的说明.

带有“AC_” 前缀的宏是Autoconf macros, 详细说明见 [[(see Autoconf Macro Index in The Autoconf Manual)]](http://www.gnu.org/software/autoconf/manual/autoconf.html#Autoconf-Macro-Index). 
带有“AM_”前缀的宏是Automake macros, 相信说明见[[(see Macro Index)]](https://www.gnu.org/software/automake/manual/automake.html#Macro-Index). 

configure.ac 文件的前两行初始化 Autoconf 与 Automake. AC_INIT 宏的参数包括：package name，version number，报错支持联系方式 (例如在“./configure --help”的末尾输出).

AM_INIT_AUTOMAKE的参数是一个automake的[[options]](https://www.gnu.org/software/automake/manual/automake.html#Options). "-Wall and -Werror"要求 automake 打开所有的warnings并将他们对待为errors. “foreign”参数告诉Automake这个package不会遵循 GNU Standards. GNU packages应总是分发额外的文件，例如ChangeLog, AUTHORS, 等等.我们不想automake在我们的demo中抱怨这些丢失的文件。

“AC_PROG_CC ”这行使得configure脚本查找C compiler并将它的名字赋给CC变量。Automake生成的“src/Makefile.in”文件使用CC变量去构建hello, 如果configure脚本需要从“src/Makefile.in”生成“src/Makefile”, 它会将CC用找到的值赋值。如果Automake 要求创建一个使用CC的"/Makefile.in"，但configure.ac并没有定义它， 那它将建议你添加个AC_PROG_CC. 

"AC_CONFIG_HEADERS([config.h])"使得configure 脚本创建" config.h"文件，该文件收集在 configure.ac中由其他宏定义的 ‘#define’s . 在我们的例子中 AC_INIT 宏已经定义了其中的一些，在confingure运行后，下面是config.h的部分摘录: 
```bash
…
/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "bug-automake@gnu.org"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "amhello 1.0"
…
```

正如你可能注意到的， SRC/main.c中 包括 config.h,所以它可以使用PACKAGE_STRING。在现实世界的项目中， config.h中 可以长得非常大，被那些系统中探测到的特性所定义的“#define”。

“AC_CONFIG_FILES”宏声明confingure需要从“*.in”模板去创建的文件列表. Automake也会扫描这个列表来查找它必须处理的Makefile.am文件. (这一点很重要，要记住：添加一个新的目录到你的项目中时，你应该添加它的Makefile文件 到这个列表，否则Automake将永远不会处理你在那个目录下新写的 Makefile.am。) 

“AC_OUTPUT”宏是一个关闭命令，它同时负责创建“AC_CONFIG_HEADERS“，”AC_CONFIG_FILES”登记的文件

当开始一个新项目时，我们建议你从这么简单的configure.ac开始 ，并逐渐添加其他需要的测试。该命令autoscan还可以建议您的软件包可能需要的一些测试 [[(see Using autoscan to Create configure.ac in The Autoconf Manual)]](http://www.gnu.org/software/autoconf/manual/autoconf.html#autoscan-Invocation)。

### 2.4.3 amhello’s Makefile.am Setup Explained
We now turn to src/Makefile.am. This file contains Automake instructions to build and install hello. 
bin_PROGRAMS = hello
hello_SOURCES = main.c
A Makefile.am has the same syntax as an ordinary Makefile. When automake processes a Makefile.am it copies the entire file into the output Makefile.in (that will be later turned into Makefile by configure) but will react to certain variable definitions by generating some build rules and other variables. Often Makefile.ams contain only a list of variable definitions as above, but they can also contain other variable and rule definitions that automake will pass along without interpretation. 
Variables that end with _PROGRAMS are special variables that list programs that the resulting Makefile should build. In Automake speak, this _PROGRAMS suffix is called a primary; Automake recognizes other primaries such as _SCRIPTS, _DATA, _LIBRARIES, etc. corresponding to different types of files. 
The ‘bin’ part of the bin_PROGRAMS tells automake that the resulting programs should be installed in bindir. Recall that the GNU Build System uses a set of variables to denote destination directories and allow users to customize these locations (see Standard Directory Variables). Any such directory variable can be put in front of a primary (omitting the dir suffix) to tell automake where to install the listed files. 
Programs need to be built from source files, so for each program prog listed in a _PROGRAMS variable, automake will look for another variable named prog_SOURCES listing its source files. There may be more than one source file: they will all be compiled and linked together. 
Automake also knows that source files need to be distributed when creating a tarball (unlike built programs). So a side-effect of this hello_SOURCES declaration is that main.c will be part of the tarball created by make dist. 
Finally here are some explanations regarding the top-level Makefile.am. 
SUBDIRS = src
dist_doc_DATA = README
SUBDIRS is a special variable listing all directories that make should recurse into before processing the current directory. So this line is responsible for make building src/hello even though we run it from the top-level. This line also causes make install to install src/hello before installing README (not that this order matters). 
The line dist_doc_DATA = README causes README to be distributed and installed in docdir. Files listed with the _DATA primary are not automatically part of the tarball built with make dist, so we add the dist_ prefix so they get distributed. However, for README it would not have been necessary: automake automatically distributes any README file it encounters (the list of other files automatically distributed is presented by automake --help). The only important effect of this second line is therefore to install README during make install. 
One thing not covered in this example is accessing the installation directory values (see Standard Directory Variables) from your program code, that is, converting them into defined macros. For this, see Defining Directories in The Autoconf Manual. 



# ADX

## Autoconf-Macro-Index

- [[manual link]](https://www.gnu.org/software/autoconf/manual/autoconf.html#Autoconf-Macro-Index)

| macro | description | output variables |
|--|--|--|
|AC_INIT (package, version, [bug-report], [tarname], [url]) | Set the *name* of the package and its *version*. The arguments of AC_INIT must be static | PACKAGE_NAME,PACKAGE_TARNAME,<br/> PACKAGE_VERSION,PACKAGE_STRING,<br/>PACKAGE_BUGREPORT |
|AC_CONFIG_SRCDIR (unique-file-in-source-dir)| is some file that is in the package's source directory; configure checks for this file's existence to make sure that the directory that it is told contains the source code in fact does. Occasionally people accidentally specify the wrong directory with --srcdir; this is a safety check. | |
|AC_CONFIG_HEADERS (header ..., [cmds], [init-cmds])|it Make AC_OUTPUT create the file(s) in the blank-or-newline-separated list header. If header already exists and its contents are identical to what AC_OUTPUT would put in it, it is left alone. <br/>example:AC_CONFIG_FILES([file.hpp:file.hpp.in:another.hpp.in]) will perform text substitution on both file.hpp.in and another.hpp.in and then concatenate both results into file.hpp. ||
|AC_CHECK_HEADERS (header-file..., [action-if-found], [action-if-not-found], [includes])|For each given system header file header-file in the blank-separated argument list that exists, define HAVE_*header-file* (in all capitals). If action-if-found is given, it is additional shell code to execute when one of the header files is found. You can give it a value of ‘break’ to break out of the loop on the first match. If action-if-not-found is given, it is executed when one of the header files is not found. ||
