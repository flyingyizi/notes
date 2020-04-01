


## Sharing Golang packages to C and Go
[Sharing Golang packages to C and Go](http://blog.ralch.com/tutorial/golang-sharing-libraries/)
[go-cshared-examples](https://github.com/vladimirvivien/go-cshared-examples)

The latest Go 1.5 version is out. As part of the new features, Go compiler can compile packages as a shared libraries.

It accepts -buildmode argument that determines how a package is compiled. These are the following options:
-  **archive**: Build the listed non-main packages into .a files. Packages named main are ignored.
-  **c-archive**: Build the listed main package, plus all packages it imports, into a C archive file.
-  **c-shared**: Build the listed main packages, plus all packages that they import, into C shared libraries.
- **shared**: Combine all the listed non-main packages into a single shared library.
- **exe**: Build the listed main packages and everything they import into executables. Packages not named main are ignored.

By default, listed main packages are built into executables and listed non-main packages are built into .a files.

In this article we will explore two major ways to share libraries between Go and C:

### Using shared library in Go

Assume that GOPATH contains this structure:
```text
.
├── calc
│   └── calc.go
└── cashier
    └── main.go
```


The calc package contains a set of functions that do arithmetic opertaions:
```go
// filename: calc.go
package calc

func Sum(x, y int) int {
    return x + y
}
```
Before compile any shared library, the standard builtin packages should be installed as shared library. This will allow any other shared library to link with them.
```shell
$ go install -buildmode=shared -linkshared std
```
Then the calc package can be compiled as shared library linked to std libraries:
```shell
$ go install -buildmode=shared -linkshared calc
```
Due to a [issue](https://github.com/golang/go/issues/12236), building and installing shared library should be from $GOPATH/src. 经实践，这个问题在go1.9下未发现。

Lets use the shared library *calc* in the *cashier* application:
```go
// package: cashier
// filename: main.go
package main

import "calc"
import "fmt"

func main() {
  fmt.Println("Cashier Application")
    fmt.Printf("Result: %d\n", calc.Sum(5, 10))
}
```
The application should be compiled and linked with calc library with the following command:
```shell
$ go build -linkshared -o app cashier
```
The output of executing the application is:
```shell
$ ./app
Cashier Application
Result: 15
```
*Note that this feature is available on linux/amd64 platform or when gccgo compiler is used.*

### Using shared Go library in C

Go functions can be executed from C applications. They should be exported by using the following comment line:
```go
//export <your_function_name>
```
In the code snippet below, the function SayHello and SayBye are exported:
```go
// package name: nautilus
package main

import "C"
import "fmt"

//export SayHello
func SayHello(name string) {
	fmt.Printf("Nautilus says: Hello, %s!\n", name)
}

//export SayBye
func SayBye() {
	fmt.Println("Nautilus says: Bye!")
}

func main() {
	// We need the main function to make possible
	// CGO compiler to compile the package as C shared library
}
```
The packaged should be compiled with buildmode flags c-shared or c-archive:
```shell
// as c-shared library
$ go build -buildmode=c-shared -o nautilus.a nautilus.go

// as c-archive 
$ go build -buildmode=c-archive -o nautilus.a nautilus.go
```
As result the **GO** compiler will produce a static/dynamic **C** library **nautilus.a** and header file **nautilus.h**. The header file contains type definitions that marshall and unmarshall data between Go and C:
```c
typedef signed char GoInt8;
typedef unsigned char GoUint8;
typedef short GoInt16;
typedef unsigned short GoUint16;
typedef int GoInt32;
typedef unsigned int GoUint32;
typedef long long GoInt64;
typedef unsigned long long GoUint64;
typedef GoInt64 GoInt;
typedef GoUint64 GoUint;
typedef __SIZE_TYPE__ GoUintptr;
typedef float GoFloat32;
typedef double GoFloat64;
typedef __complex float GoComplex64;
typedef __complex double GoComplex128;
typedef struct { char *p; GoInt n; } GoString;
typedef void *GoMap;
typedef void *GoChan;
typedef struct { void *t; void *v; } GoInterface;
typedef struct { void *data; GoInt len; GoInt cap; } GoSlice;

#endif

/* End of boilerplate cgo prologue.  */

#ifdef __cplusplus
extern "C" {
#endif


extern void SayHello(GoString p0);

extern void SayBye();

#ifdef __cplusplus
}
#endif
```
The header file **nautilus.h** shoulde be imported from every **C** application that executed **SayHello** and **SayBye** functions.

In the example below, the **SayHello** function is called with parameter of type GoString. It includes *char\** field and its length.
```c
// filename: _wale.c
#include "nautilus.h"
#include <stdio.h>

int main() {
  printf("This is a C Application.\n");
  GoString name = {"Jack", 4};
  SayHello(name);
  SayBye();
  return 0;
}
```
The _wale.c file is compiled with the following command:
```shell
$ gcc -o _wale _wale.c nautilus.a
```
Execution produce the following output:
```shell
$ ./wale
This is a C Application.
Nautilus says: Hello, Jack!
Nautilus says: Bye!
```
### Conclusion

Sharing libraries between C and Go gives opportunity to build greater and better application by using the best from both worlds. This provides to a legacy system a modern language that can improve their maintainance costs and business needs. It maximize code reusability in the Go ecosystem.

从下面可以看出，动态链接与默认的静态文件大小区别还是挺大的。
```shell
$go build -linkshared -o app main.go
$go build  -o appx main.go
$ ls -lh
total 1.9M
-rwxrwxr-x 1 atmel atmel  17K Dec 10 00:01 app
-rwxrwxr-x 1 atmel atmel 1.8M Dec 10 00:01 appx
-rw-rw-r-- 1 atmel atmel  144 Dec  9 23:59 main.go
$ ldd ./appx
	not a dynamic executable
$ ldd ./app 
	linux-vdso.so.1 =>  (0x00007ffd12d89000)
	libstd.so => /usr/local/go/pkg/linux_amd64_dynlink/libstd.so (0x00007f9e161cb000)
	libmy-calc.so => /home/atmel/.go/pkg/linux_amd64_dynlink/libmy-calc.so (0x00007f9e15fc7000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f9e15be4000)
	libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f9e159e0000)
	libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f9e157c3000)
	/lib64/ld-linux-x86-64.so.2 (0x000055fca2765000)
```


### 几个问题的解决

- /usr/bin/ld: cannot find crti.o: No such file or directory
```text
This is a BUG reported in launchpad, byt there is a workaround :

compiling 64 bits program in 32 bits platform :

LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LIBRARY_PATH 
export LIBRARY_PATH

for 32 bits native :

LIBRARY_PATH=/usr/lib32:$LIBRARY_PATH
export LIBRARY_PATH

That in your .bashrc file (or ran from a console) is enough for GCC to find the new location of library.

thx. to Iain Buclaw (ibuclaw) to point out in the right direction.
12.10 32 bits

hhlp@hhlp:~$ sudo find /usr/ -name crti*
/usr/lib/i386-linux-gnu/crti.o
hhlp@hhlp:~$ 

LIBRARY_PATH=/usr/lib/i386-linux-gnu:$LIBRARY_PATH 
export LIBRARY_PATH

12.10 64 bits

hhlp@hhlp:~$ sudo find /usr/ -name crti*
/usr/lib/x86_64-linux-gnu/crti.o
hhlp@hhlp:~$ 

LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LIBRARY_PATH 
export LIBRARY_PATH
```
