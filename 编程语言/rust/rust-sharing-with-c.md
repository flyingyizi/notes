

## c call rust

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
