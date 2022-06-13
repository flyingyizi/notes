

[如何在Rust编程语言中使用C/C++的函数库？](https://magiclen.org/rust-c-library)

## c call rust

[如何在C/C++编程语言中使用Rust的函数库](https://magiclen.org/c-rust-library/)

以下例子是最简单的， 更具实际意义的例子参见 [C语言调用rust编译的静态库--cbindgen方式](https://www.cnblogs.com/renxinyuan/articles/15830009.html)
```shell
cargo new --lib foo
```
Cargo.toml文件，并在该文件中添加如下内容：
```toml
[dependencies]
libc = "*"

[lib]
crate-type = ["staticlib"]
#必须对panic进行标注否则会出现问题
[profile.release]
panic="abort"
[profile.devl]
panic="abort"
```

希望和c语言里定义的类型一致，此时就需要在rust中引入libc库，
```rust
extern crate libc;
use libc::uint32_t;

#[no_mangle]
pub extern "C" fn foo(a: uint32_t, b: uint32_t) {
    println!("hello : a + b = {}", a + b);
}
```
对应C文件中类似
```c
#include <stdint.h>
extern void foo(uint32_t a, uint32_t b);

int main() {
    foo(5, 7);
    return 0;
}
```

## rust call c

[Rust FFI Examples](https://github.com/iceqing/rust-ffi-examples)


```rust
// build.rs
use std::env;
use std::process::Command;
use std::path::Path;
 
fn main() {
    let out_dir = env::var("OUT_DIR").unwrap();
 
    Command::new("cc").args(&["hello-world/hello.c", "-O3","-c", "-fPIC", "-o"])
        .arg(&format!("{}/hello-world.o", out_dir))
        .status().unwrap();
 
    Command::new("ar").args(&["crus", "libhello-world.a", "hello-world.o"])
        .current_dir(&Path::new(&out_dir))
        .status().unwrap();
 
    println!("cargo:rustc-link-search=native={}", out_dir);
    println!("cargo:rustc-link-lib=static=hello-world");
}
```


```c
// hello-world/hello.c
#include <stdint.h>
#include <stdio.h>
 
void greet(char* s, int32_t a, int32_t b){
    printf("Hello %s! %d + %d = %d\n", s, a, b, a + b);
}
```

```rust
// src/main.rs
use std::ffi::CString;
 
#[link(name = "hello-world")]
extern {
    fn greet(s: *const std::os::raw::c_char, a: i32, b: i32);
}
 
fn main() {
    let prompt = CString::new("Rust").unwrap();
 
    unsafe {
        greet(prompt.as_ptr(), 7, 11);
    }
}
```