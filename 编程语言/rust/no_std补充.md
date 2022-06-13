

[如何撰写出无标准函数库的Rust程序？](https://magiclen.org/rust-no-std/)

## alloc
alloc是Rust 1.36后正式开放的内置crate，可以用来替代原本标准函数库内提供的用来操作堆积空间相关的结构。例如Box、String、Vec、HashMap等。

```rust
src/lib.rs
#![no_std]
 
extern crate alloc;
 
use alloc::string::String;
use alloc::vec::Vec;
 
pub fn separate_chars(s: String) -> Vec<char> {
    s.chars().collect()
}
```

alloc也有提供vec!、format!宏，可以通过在extern crate关键字的左方或上方加上#[marco_use]属性，或是直接使用路径来调用。

例如：
```rust
src/lib.rs
#![no_std]
 
#[macro_use]
extern crate alloc;
 
use alloc::string::String;
use alloc::vec::Vec;
 
 
pub fn get_greeting_strings(target: &str) -> Vec<String> {
    vec![alloc::format!("Hello, {}!", target), format!("Hi, {}!", target)]
}
```