

# install 

注：截止 2022-07，发现安装最新nightly就可以，不必限制在nightly-2021-01-07。

在rust环境下为了支持AVR编译，需要安装：

- rust component rust-src: is required to allow Rust to compile libcore for any chip being targeted. 根据 [issure](https://github.com/avr-rust/blink/issues/38), 编译AVR当前应仅能使用“nightly-2021-01-07”toolchain。
- sudo apt install avr-libc gcc-avr pkg-config avrdude
- ravedude: a tool which seamlessly integrates flashing your board into the usual cargo workflow. 它是avrdude封装以适配rust workflow。

相应相关安装动作包括：
```shell
$ rustup toolchain install nightly-2021-01-07
$ sudo apt install avr-libc gcc-avr pkg-config avrdude
$ rustup +nightly-2021-01-07 component add rust-src
# 在“nightly-2021-01-07”toolchain下不支持ravedude编译
$ cargo +stable install ravedude
```
验证安装结果：
- [官方例子]("https://github.com/avr-rust/blink.git")进行验证

其他第三方工具：

- cargo-generate: 在“nightly-2021-01-07”toolchain下不支持，建议采用在stable toolchain（当前时间2022年）下安装 `cargo +stable install cargo-generate`. 配合[avr-hal](https://github.com/Rahix/avr-hal)工作。

  The best way to start your own avr project is via the [avr-hal-template](https://github.com/Rahix/avr-hal-template).`cargo generate --git https://github.com/Rahix/avr-hal-template.git`


补充：当前执行“rustc --print target-list”结果中涉及AVR的仅出现avr-unknown-gnu-atmega328。

# 建议

在创建的project目录下放置一个“rust-toolchain.toml”文件，内容如下：
```text
[toolchain]
channel = "nightly-2021-01-07"
components = [ "rust-src" ]
profile = "minimal"
```

# rust AVR resource

[The AVR-Rust Guidebook](https://book.avr-rust.com/)

avr topics in [Curated list of resources for Embedded and Low-level development in the Rust programming language](https://github.com/rust-embedded/awesome-embedded-rust)

# 问题处理

 ## avr-gcc

  WinAVR-20100110 链接有问题，使用新版本的avr-gcc没有问题，“https://blog.zakkemble.net/avr-gcc-builds/”。

  ## error: cannot find macro `llvm_asm` in this scope

  根据"https://github.com/avr-rust/blink.git"尝试rust 编写AVR程序，报错。

  解决方式：

  依据“https://github.com/avr-rust/blink/issues/38”中提示，采用：
  ```shell
  rustup override set nightly-2021-01-07
rustup component add rust-src
cargo build -Z build-std=core --target avr-atmega328p.json --release
```
Looks like it works only with nightly-2021-01-07 release.


## libm  failed to compile targeting avr

经常使用的num-trait 在嵌入式环境下需要使用libm，但libm在avr架构下编译有问题：
[libm  failed to compile targeting avr](https://github.com/rust-lang/libm/issues/250)。 