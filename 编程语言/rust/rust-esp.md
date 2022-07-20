
[The Rust on ESP Book](https://esp-rs.github.io/book/dependencies/index.html)，按照其中指导进行rust esp安装，特别注意需要在nightly下安装。


[Embedded Rust on Espressif](https://espressif-trainings.ferrous-systems.com/01_intro.html)

如果架构是RISC-V，那rust是可以原生支持的，例如ESP32-C3 platform

## 原生支持
对risc-v架构当前rust可以原生支持。不过需要nightly，具体要求见模板“https://github.com/esp-rs/esp-template ”对esp32-c3生成的项目。

另外还需要安装Espressif toolchain, 对应安装指令是：
```shell
$ cargo install cargo-generate cargo-espflash espmonitor bindgen ldproxy
```

## std (esp-idf-hal)支持

支持除ESP8266以外的所有其他架构

Using the Rust Standard Library (std)
Espressif provides a C-based development framework called esp-idf which has support for all Espressif chips starting with the ESP32; note that this framework does not support the ESP8266.

|Relevant           | esp-rs crates|
|-------------------|--------------|
|Repository	        |Description|
|esp-rs/esp-idf-hal	|An implementation of the embedded-hal and other traits using the esp-idf framework.|
|esp-rs/esp-idf-svc	|An implementation of embedded-svc using esp-idf drivers.|
|esp-rs/esp-idf-sys	|Rust bindings to the esp-idf development framework. Gives raw (unsafe) access to drivers, Wi-Fi and more.|
|esp-rs/embedded-svc|Abstraction traits for embedded services. (WiFi, Network, Httpd, Logging, etc.)|


The aforementioned crates have interdependencies, and this relationship can be seen below.

- [esp-idf-hal Documentation](https://esp-rs.github.io/esp-idf-hal/esp_idf_hal/)
- [esp-idf-svc Documentation](https://esp-rs.github.io/esp-idf-svc/esp_idf_svc/)
- [esp-idf-sys Documentation](https://esp-rs.github.io/esp-idf-sys/esp_idf_sys/)

## no-std支持

仅仅支持ESP32 or the ESP8266

早先， no-std 通过“via esp-rs/esp32-hal and esp-rs/esp8266-hal.” 聚焦在ESP32，以及较低程度对ESP8266的支持, 

当前esp32-hal and esp8266-hal 还不支持Wi-Fi and Bluetooth。 虽然有个 esp-rs/esp32-wifi计划来补充，但当前它还不可用.

为了补齐no-std的能力，最近新起了“esp-rs/esp-hal”These new HALs can be found in the esp-rs/esp-hal repository.

Relevant esp-rs Crates
|Repository	        |Description|
|-------------------|--------------|
|esp-rs/esp-pacs	| A monorepo containing PACs for each supported device.|
|esp-rs/esp-hal	    |An implementation of the embedded-hal traits and more for the ESP32, ESP32-C3, ESP32-S2, and ESP32-S3.|
|esp-rs/esp8266-hal	|An implementation of the embedded-hal traits and more for the ESP8266.|

- [esp-rs/esp-pacs](https://github.com/esp-rs/esp-pacs)	
- [esp-rs/esp-hal](https://github.com/esp-rs/esp-hal)	   
- [esp-rs/esp8266-hal](https://github.com/esp-rs/esp8266-hal)

## 工具链安装

具体内容见 [install rust Prerequisites](https://esp-rs.github.io/book/dependencies/installing-rust.html)。根据其中的指示进行rust安装。

### Xtensa Toolchain
If you are developing for an Xtensa chip (ESP32, ESP32-S2, ESP32-S3) you must also install the appropriate Xtensa toolchain.

建议直接使用预编译的Xtensa工具链：https://dl.espressif.cn/dl/esp-idf/

### RISC-V (ESP32-C3)
The RISC-V architecture has support in the mainline Rust compiler so setup is relatively simple, all we must do is add the appropriate compilation target.

There are two suitable targets for this chip:

- For bare-metal (no_std) applications, use riscv32imc-unknown-none-elf
- For applications which require std, use riscv32imc-esp-espidf

```shell
# just use the stock nightly Rust compiler, and a recent, stock Clang (as in Clang 11+)
$ rustup component add rust-src --toolchain nightly

$ rustup target add riscv32imc-unknown-none-elf
$ rustup target add riscv32imc-esp-espidf
```

## 模板

```shell
#using the Rust standard library (std) you can use the esp-idf-template template
$ cargo generate --git https://github.com/esp-rs/esp-idf-template cargo

#For bare-metal applications (no_std) you can instead use the esp-template template: 支持mcu包括
#  esp32
#❯ esp32c3
#  esp32s2
#  esp32s3
$ cargo generate --git https://github.com/esp-rs/esp-template 
```