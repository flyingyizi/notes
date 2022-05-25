[一个实际的rust embed项目](git clone https://github.com/petrohi/allbot )

[experiments with Rust + USB, Kevin making a touchpad.](https://github.com/lynaghk/touchtron)

[Rust on STM32: Getting started](https://jonathanklimt.de/electronics/programming/embedded-rust/rust-on-stm32-2/)

[The Embedded Rust Book](https://docs.rust-embedded.org/), [The Embedded Rust Book github](https://github.com/rust-embedded/book);  [The Embedded Rust Book中文](https://logiase.github.io/The-Embedded-Rust-Book-CN/).

[rust-embedded f3discovery](https://docs.rust-embedded.org/discovery/f3discovery/index.html), [source code](https://github.com/rust-embedded/discovery)

[rust-raspberrypi-OS-tutorials](https://github.com/rust-embedded/rust-raspberrypi-OS-tutorials)

[Curated list of resources for Embedded and Low-level development in the Rust programming language](https://github.com/rust-embedded/awesome-embedded-rust)

[A collection of small examples built with stm32f4xx-hal](https://github.com/lonesometraveler/stm32f4xx-examples)

[async/await on embedded Rust](https://ferrous-systems.com/blog/async-on-embedded/)

# 0.预准备

建议使用最新stable toolchain。这与avr单片机不一样，它仅仅支持“nightly-2021-01-07”toolchain。

```shell
$ rustup +stable component add rust-src
#for Cortex-M4f
$ rustup +stable target add thumbv7em-none-eabihf
# cargo-binutils
$ cargo +stable install cargo-binutils
$ rustup +stable component add llvm-tools-preview
# generate project form template
$ cargo +stable install cargo-generate

$ sudo apt-get install qemu-system-arm
```

## select which target?

对ARM，需要根据自己板子的情况选择对应的target，对应关系如下：
rustc provides 4 different targets that cover the different processor families within that architecture:
```text
# target = "thumbv6m-none-eabi"        # Cortex-M0 and Cortex-M0+
target = "thumbv7m-none-eabi"        # Cortex-M3
# target = "thumbv7em-none-eabi"       # Cortex-M4 and Cortex-M7 (no FPU)
# target = "thumbv7em-none-eabihf"     # Cortex-M4F and Cortex-M7F (with FPU)
# target = "thumbv8m.base-none-eabi"   # Cortex-M23
# target = "thumbv8m.main-none-eabi"   # Cortex-M33 (no FPU)
# target = "thumbv8m.main-none-eabihf" # Cortex-M33 (with FPU)
```

例如nucleo  STM32F411RE 是ARM Cortex-M4，具有FPU，对应工具链应选择`thumbv7em-none-eabihf`

## 常用工具

### probe-run

probe-run is a custom Cargo runner that transparently runs Rust firmware on an embedded device.

常用来作为custom runner

### cargo-generate

cargo-generate工具方便通过模板生成project。例如"`cargo generate --git https://github.com/rust-embedded/cortex-m-quickstart`".

也可以不使用该工具cargo-generate，将模板手动下载下来，然后自己将模板中类似“`"{{project-name}}"`”占位符替换为自己需要的值。

### templates

cargo generate --git https://github.com/rust-embedded/cortex-m-quickstart

cargo generate --git https://github.com/rtic-rs/defmt-app-template

## useful attributes

在嵌入式编程中，经常涉及的attribute有：

- `#[panic_handler]`: 标志了该属性的函数（签名必须是`fn(&PanicInfo) -> !`），当[painc]发生时它就会被调用。标准库会提供它自己的panic handler函数，但是在一个no_std环境里我们需要自己来定义它:
    ```rust
    use core::panic::PanicInfo;
    fn apnic(_info: &PanicInfo)->!{}
    ```
- `#![no_std]` ：Using Rust Without the Standard Library，需要放置在crate root。
- `#![no_main]` ：attribute告诉Rust编译器我们不需要默认的入口链
- `#[no_mangle]`: attribute来关闭 name mangling，否则编译器会生成诸如_ZN3blog_os4_start7hb173fedf945531caE这样编译器识别不了的隐晦符号。例如：
    ```rust
    #[no_mangle]
    pub extern "C" fn start()->!{}
    ```
- `#[cfg(debug_assertions)]`: 条件编译属性，dev profile
- `#[cfg(not(debug_assertions))]`:条件编译属性，release profile


### panic_handler属性补充说明

如前面介绍`#[panic_handler]`，你当然可以自己去实习自己的panic handler，但panic处理还是右挺多共性的，这些常见的共性行为已经有很多了实现并打包到了crates.io中，你完全可以直接使用它们而不必自己来定义带`#[panic_handler]`属性标签的处理函数。下面是一些例子，你可以通过[“ panic-handler keyword”](https://crates.io/keywords/panic-handler)在crates.io中找到更多例子：

  - panic-abort. [panic-abort](https://crates.io/crates/panic-abort),A panic causes the abort instruction to be executed.
  - panic-halt. [panic-halt](https://crates.io/crates/panic-halt),A panic causes the program, or the current thread, to halt by entering an infinite loop.
  - panic-itm. [panic-itm](https://crates.io/crates/panic-itm),The panicking message is logged using the ITM, an ARM Cortex-M specific peripheral.
  - panic-semihosting. [panic-semihosting](https://crates.io/crates/panic-semihosting),The panicking message is logged to the host using the semihosting technique.

例子1：
```rust
// 对应在Cargo.toml中应有以下内容
/**
 [dependencies]
 panic-halt = "0.2.0"
 panic-abort = "0.3.2"
*/

#![no_std]
//当dev profile (cargo build)时，链接panic_halt
// dev profile: easier to debug panics; can put a breakpoint on `rust_begin_unwind`
#[cfg(debug_assertions)]
use panic_halt as _;

//当release profile (cargo build --release). 链接panic_abort
// release profile: minimize the binary size of the application
#[cfg(not(debug_assertions))]
use panic_abort as _;

fn main() {
    panic!("argument is ignored");
}

```
"`use panic_halt as _`"语法是描述的是：让编译器清楚我们不会显式使用 crate 中的任何内容，但需要panic_abort panic handler包含在我们的最终可执行程序中。“`extern crate panic_abort`”语法是edition 2018之前使用的老语法。


## inspecting tools

###  LLVM Object Reader

类似linux readelf. 可通过"`cargo readobj -- --help`"获取llvm-readobj的详细参数。

```shell
$cargo readobj --bin armdemo -- --file-headers
    Finished dev [unoptimized + debuginfo] target(s) in 0.07s
ELF Header:
  Magic:   7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00
  Class:                             ELF32
  Data:                              2's complement, little endian
  Version:                           1 (current)
```

###  LLVM object size dumper


```shell
#如果在命令行添加`--release`参数，就可以检查优化版本的大小情况
$cargo size --bin armdemo -- -A
    Finished dev [unoptimized + debuginfo] target(s) in 0.07s
armdemo  :
section               size        addr
.vector_table         1024         0x0
.text                 1084       0x400
.rodata                312       0x83c
.data                    0  0x20000000
.bss                     0  0x20000000
.uninit                  0  0x20000000
.debug_abbrev         5666         0x0
.debug_info         156930         0x0
.debug_aranges        8448         0x0
.debug_str          219109         0x0
.debug_pubnames      57792         0x0
.debug_pubtypes       2687         0x0
.ARM.attributes         50         0x0
.debug_frame         27232         0x0
.debug_line         165334         0x0
.debug_ranges       104624         0x0
.debug_loc             109         0x0
.comment               109         0x0
Total               750510
```

"`cargo size --example xxx -- -A`" will shows example xxx size-information.

### llvm object file dumper

```rust
$ cargo objdump --bin armdemo --release -- --disassemble --no-show-raw-insn --print-imm-hex
    Finished release [optimized + debuginfo] target(s) in 0.07s

armdemo:        file format elf32-littlearm

Disassembly of section .text:

00000400 <Reset>:
     400:       push    {r7, lr}
     402:       mov     r7, sp
```     

### testing on qemu

guess a program is a valid LM3S6965 program; we can execute it in a virtual microcontroller (QEMU) to test it out. 

```shell
# this program will block
$ qemu-system-arm \
      -cpu cortex-m3 \
      -machine lm3s6965evb \
      -gdb tcp::3333 \
      -S \
      -nographic \
      -kernel target/thumbv7m-none-eabi/debug/app

# on a different terminal
$ arm-none-eabi-gdb -q target/thumbv7m-none-eabi/debug/app
Reading symbols from target/thumbv7m-none-eabi/debug/app...done.

(gdb) target remote :3333
Remote debugging using :3333
Reset () at src/main.rs:8
8       pub unsafe extern "C" fn Reset() -> ! {

(gdb) # the SP has the initial value we programmed in the vector table
(gdb) print/x $sp
$1 = 0x20010000

(gdb) step
...
(gdb) quit
```


# 1.from scratch building

[The Embedonomicon](https://docs.rust-embedded.org/embedonomicon/preface.html)介绍如何构建#![no_std]应用程序，如何精细控制 Rust 程序的内存布局的技巧。您将了解链接器、链接器脚本和 Rust 功能，这些功能可以让您控制一些 Rust 程序的 ABI。

类似的文章还有[A Freestanding Rust Binary](https://os.phil-opp.com/freestanding-rust-binary/)

## The smallest #![no_std] program(binary)

```shell
# 使用stable toolchain，最小edition是2018，因此使用2021当然ok
$ cargo new --edition 2018 --bin app

$ cd app

$ # modify main.rs so it has these contents
$ cat src/main.rs
```

src/main.rs内容如下，解释见注释
```rust
//The attribute which means that the program won't use the 
//standard main function as its entry point.
#![no_main]
// it is a crate level attribute that indicates that the 
//crate will link to the core crate instead of the std crate
#![no_std]

use core::panic::PanicInfo;

//this attribute marked with this attribute defines the behavior 
//of panics, both library level panics (core::panic!) and 
//language level panics (out of bounds indexing).
#[panic_handler]
fn panic(_panic: &PanicInfo<'_>) -> ! {
    loop {}
}
```

```shell
# 编译
$cargo build  --target thumbv7em-none-eabihf

# equivalent to `size target/thumbv7em-none-eabihf/debug/embedonomicon`
$ cargo size --target thumbv7em-none-eabihf --bin embedonomicon

   text	   data	    bss	    dec	    hex	filename
      0	      0	      0	      0	      0	embedonomicon

#Before linking, the crate contains the panicking symbol.
$ cargo rustc --target thumbv7em-none-eabihf -- --emit=obj

$ cargo nm -- target/thumbv7em-none-eabihf/debug/deps/embedonomicon-*.o | grep '[0-9]* [^N] '

00000000 T rust_begin_unwind
```

## Memory layout

链接器决定程序的最终内存布局，但我们可以使用链接器脚本对其进行一些控制。

对于符号（symbol ），rust语言允许我们通过以下属性控制符号名称和所在section：

- `#[export_name = "foo"]`将符号名称设置为foo.
- `#[no_mangle]`表示：使用函数或变量名（不是其完整路径）作为其符号名，即产生精确、明确的名称，不采用编译器自动生成的名称。 `#[no_mangle] fn bar()`将产生一个bar符号名.
- `#[link_section = ".bar"]`将符号放在名为`.bar`的section。

下面例子演示了如何创建reset-vector，并通过链接描述文件将他们放置在正确位置
```rust
//产生明确符号名称"Reset"
#[no_mangle]
// 采用C ABI，而不是rust ABI
pub unsafe extern "C" fn Reset() -> ! {
    let _x = 42;

    // can't return so we go into an infinite loop here
    loop {}
}

type ExportFn = unsafe extern "C" fn() -> !;
// The reset vector, a pointer into the reset handler. 该section将在链接描述文件中描述放置哪里
#[link_section = ".vector_table.reset_vector"]
#[no_mangle]
pub static RESET_VECTOR: ExportFn = Reset;
```

最后，我们查看下最终生成的内存布局
```shell
$cargo objdump --bin embedonomicon -- -d --all-headers
...
SYMBOL TABLE:
00000000 l    df *ABS*  00000000 embedonomicon.b38ecb13-cgu.0
00000008 l       .text  00000000 $t.0
00000004 g     O .vector_table  00000004 RESET_VECTOR
00000008 g     F .text  0000000a Reset

Disassembly of section .text:

00000008 <Reset>:
       8: 81 b0         sub     sp, #4
       a: 2a 20         movs    r0, #42
       c: 00 90         str     r0, [sp]
       e: ff e7         b       0x10 <Reset+0x8>        @ imm = #-2
      10: fe e7         b       0x10 <Reset+0x8>        @ imm = #-4
```


# 2.normal Embedded Rust

[The Embedded Rust Book](https://docs.rust-embedded.org/book/)

rust 支持嵌入式采用下面的组织形式。对已经有了哪些crate资源，在[awesome-embedded-rust](https://github.com/rust-embedded/awesome-embedded-rust)有完整收集，这个地方应该是找资源的优先地方。

```text

                               ┌───────────────────┐   ┌───────────────────┐
                             ┌─┤ micro-architecture│───┤ microproecssor    |
                             │ │   crate           │   │   (arm-cortex)    │
                             │ └───────────────────┘   └───────────────────┘
 ┌────────────┐ ┌──────────┐ │
 │ board crate├─┤HAL crate ├─┤
 └────────────┘ └──────────┘ │ ┌───────────────────┐
                             │ │peripheral asscess │
                             └─┤   crate(PAC)      │
                               └───────────────────┘
```

- Micro-architecture Crate : 针对microcontroller core共性，例如[cortex-m](https://crates.io/crates/cortex-m),与[cortex-m-rt](https://crates.io/crates/cortex-m-rt),其中cortex-m-rt专注于Startup code and minimal runtime for Cortex-M microcontrollers.通常在Cargo.toml中添加如下依赖：
    ```text
    [dependencies]
    cortex-m = "0.6.0"
    cortex-m-rt = "0.6.10"
    ```
      
- Peripheral Access Crate (PAC) ：这类crate为特定microcontroller产品的memory布局，通过它你将按照microcontroller产品手册说明与寄存器直接交互。例如micro stm32f40x系列的[stm32f40x](https://crates.io/crates/stm32f40x), Micro STM32F30x 系列的 [stm32f30x](https://crates.io/crates/stm32f30x),例如德州仪器 Tiva-C TM4C123 系列的 [tm4c123x](https://crates.io/crates/tm4c123x)

- HAL Crate - These crates offer a more user-friendly API for your particular processor, often by implementing some common traits defined in embedded-hal. For example, this crate might offer a Serial struct, with a constructor that takes an appropriate set of GPIO pins and a baud rate, and offers some sort of write_byte function for sending data. See the chapter on [Portability](https://docs.rust-embedded.org/book/portability/index.html) for more information on [embedded-hal](https://crates.io/crates/embedded-hal).

- Board Crate - 板级封装。例如 STM32F3DISCOVERY board的[stm32f3-discovery](https://crates.io/crates/stm32f3-discovery).nucleo-f411re板子的[nucleo-f411re](https://crates.io/crates/nucleo-f411re), 

### 

[dependencies.stm32f3xx-hal]
version = "0.7.0"
features = ["stm32f303xc", "rt"]


## 关于svd2rust

由于大多数HAL代码都是通过svd2rust生成，所以熟悉它生成api规则应该是有好处的。

### Peripheral API

[Peripheral API](https://docs.rs/svd2rust/0.19.0/svd2rust/#peripheral-api)

外设都设计为单实例，并且仅有`Peripherals::take`方法来获取外设实例,并且该方法仅允许调用一次否则panic：
```rust
let mut peripherals = stm32f30x::Peripherals::take().unwrap();
peripherals.GPIOA.odr.write(|w| w.bits(1));
```
外设提供了`static fn ptr`来绕过单实例约束, 这个方法在创建更高级别HAL时有用。
```rust
struct PA0 { _0: () }
impl PA0 {
    fn is_high(&self) -> bool {
        // NOTE(unsafe) actually safe because this is an atomic read with no side effects
        unsafe { (*GPIOA::ptr()).idr.read().bits() & 1 != 0 }
    }

    fn is_low(&self) -> bool {
        !self.is_high()
    }
}
struct PA1 { _0: () }
// ..

fn configure(gpioa: GPIOA) -> (PA0, PA1, ..) {
    // configure all the PAx pins as inputs
    gpioa.moder.reset();
    // the GPIOA proxy is destroyed here now the GPIOA register block can't be modified
    // thus the configuration of the PAx pins is now frozen
    drop(gpioa);
    (PA0 { _0: () }, PA1 { _0: () }, ..)
}
```

注：如果使用stm32f4xx_hal crate，对应ptr语法是"`let gp = &(*stm32f4xx_hal::pac::GPIOA::ptr());`"

例如在源码"stm32f4xx-hal-0.12.0\src\serial.rs"中

#### 绕过单实例约束封装例子

下面的例子演示了实现了OutPutPin trait自己封装的例子
```rust
use core::marker::PhantomData;
use embedded_hal::digital::v2::{OutputPin, PinState,InputPin};
use stm32f4xx_hal as hal;

/// - `P` is port name: `A` for GPIOA, `B` for GPIOB, etc.
/// - `N` is pin number: from `0` to `15`.
pub struct MyPin<const P: char, const N: u8> {
}

impl<const P: char, const N: u8> MyPin<P, N> {
    const fn ptr() -> *const hal::pac::gpioa::RegisterBlock {
        match P {
            'A' => hal::pac::GPIOA::ptr(),
            'B' => hal::pac::GPIOB::ptr() as _,
            'C' => hal::pac::GPIOC::ptr() as _,
            'D' => hal::pac::GPIOD::ptr() as _,
            'E' => hal::pac::GPIOE::ptr() as _,
            'H' => hal::pac::GPIOH::ptr() as _,
            _ => panic!("Unknown GPIO port"),
        }
    }
    pub fn new() -> Self {
        Self {
            // _p: PhantomData,
            // _n: PhantomData,
        }
    }
}

impl<const P: char, const N: u8> OutputPin for MyPin<P, N> {
    type Error = core::convert::Infallible;

    #[inline(always)]
    fn set_high(&mut self) -> Result<(), Self::Error> {
        unsafe { (*Self::ptr()).bsrr.write(|w| w.bits(1 << N)) }
        Ok(())
    }

    #[inline(always)]
    fn set_low(&mut self) -> Result<(), Self::Error> {
        unsafe { (*Self::ptr()).bsrr.write(|w| w.bits(1 << (N + 16))) }
        Ok(())
    }
    fn set_state(&mut self, state: PinState) -> Result<(), Self::Error> {
        match state {
            PinState::Low => self.set_low(),
            PinState::High => self.set_high(),
        }
    }
}
impl<const P: char, const N: u8> InputPin for MyPin<P, N> {
    type Error = core::convert::Infallible;

    /// Is the input pin high?
    fn is_high(&self) -> Result<bool, Self::Error>{
        match self.is_low(){
           Ok(b)=> return Ok(!b),
           Err(t)=> return Err(t),
        }        
    }

    /// Is the input pin low?
    fn is_low(&self) -> Result<bool, Self::Error>{
        let b:bool=unsafe { (*Self::ptr()).idr.read().bits() & (1 << N) == 0 };
        Ok(b)
    }
}

```


### read / modify / write API

[read / modify / write API](https://docs.rs/svd2rust/0.19.0/svd2rust/#read--modify--write-api)

### Interrupt API

[Interrupt API](https://docs.rs/svd2rust/0.19.0/svd2rust/#interrupt-api)

# 3.Debugonomicon

[Debugonomicon](https://docs.rust-embedded.org/debugonomicon/)

## openocd debug

https://stackoverflow.com/questions/42612329/executing-code-from-ram-in-stm32

[OpenOCD User’s Guide](http://openocd.org/doc-release/html/index.html#SEC_Contents)

for example, to program the target flash, you can use [openocd flash programming](https://openocd.org/doc-release/html/Flash-Programming.html#Flash-Programming): 

```text
# program and verify using elf/hex/s19. verify and reset
# are optional parameters
openocd -f board/stm32f3discovery.cfg \
	-c "program filename.elf verify reset exit"

# binary files need the flash address passing
openocd -f board/stm32f3discovery.cfg \
	-c "program filename.bin exit 0x08000000"
```

### 1.connect to board

手头板子是nucleo stm32F411RE，已经通过usb连接pc。在一个单独的terminal执行下面的命令，之所以要单独的terminal，因为该执行除非终止，不然不会返回的。

```shell
#openocd 0.10.0之后的版本建议使用 interface/stlink.cfg文件
$openocd -f interface/stlink-v2-1.cfg -f target/stm32f4x.cfg
xPack OpenOCD, 32-bit Open On-Chip Debugger 0.10.0+dev (2019-07-17-07:34)
...
Info : STLINK V2J37M26 (API v2) VID:PID 0483:374B
Info : Target voltage: 3.258383
Info : stm32f4x.cpu: hardware has 6 breakpoints, 4 watchpoints
Info : Listening on port 3333 for gdb connections

#上面命令的最后应出现类似hardware has x breakpoints, x watchpoints的提示，
#如果没有出现，则stlink.cfg-->stlink-v2-1.cfg->stlink-v2.cfg逐步降级尝试直到出现该提示，问题原因因为可能你的板子是旧的硬件版本
```
如前面的举例，当执行openocd时需要输入常常的参数。这里有个省事的方法：

- 在你需要执行openocd的目录中创建一个openocd.cfg文件，该文件包含下面的内容：这样今后只要简单执行"`$ openocd`"就和以前长长参数的执行效果是一样的。

    ```text
    source [find interface/stlink.cfg]
    source [find target/stm32f4x.cfg]
    ```

### 2. use pure openocd cmd to debug

openocd命令都需要通过telent连接去执行，支持命令参考[command index](http://openocd.org/doc-release/html/Command-and-Driver-Index.html#Command-and-Driver-Index_fn_letter-M)

在另一个终端上运行telnet

```shell
$telnet localhost 4444

Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Open On-Chip Debugger
> 
> resume 0 #从0地址开始运行
> halt #暂停cpu
> mdw 0 12 #从0地址读12个数（32位）
>mww 地址 写的数 #向某地址写数（32位）
>reg #查看寄存器
>flash write_image erase /nuttx.hex #下载
```

### 3. use gdb to debug

推荐这种方式，因为这种方式不仅可以执行openocd命令，也可以执行gdb命令。

在另一个终端上运行 GDB

```shell
$ arm-none-eabi-gdb -q target/examples/hello
#接下来将 GDB 连接到 OpenOCD，它正在等待端口 3333 上的 TCP 连接。
(gdb) target remote :3333
Remote debugging using :3333
0x00000000 in ?? ()

#现在继续使用命令将程序闪存（加载）到微控制器上 
(gdb) load
Loading section .vector_table, size 0x400 lma 0x8000000
Loading section .text, size 0x1e70 lma 0x8000400
Loading section .rodata, size 0x61c lma 0x8002270
Start address 0x800144e, load size 10380
Transfer rate: 17 KB/sec, 3460 bytes/write.

#该程序现在已加载。该程序使用半主机，因此在我们进行任何半主机
#调用之前，我们必须告诉 OpenOCD 启用半主机。您可以使用命令
#向 OpenOCD 发送命令
#You can see all the OpenOCD commands by invoking 
#the "monitor help" command.
(gdb) monitor arm semihosting enable
semihosting is enabled

#后面就是普通操作了
(gdb) break main
Breakpoint 1 at 0x8000d18: file examples/hello.rs, line 15.

(gdb) continue
Continuing.
```
上面演示了gdb去调试，但上面要敲那么多指令是比较烦的，建议使用gdb的“`-x Execute GDB commands from FILE`”参数来简化执行。 例如生成下面的文件(cat openocd.gdb)：

```text
target extended-remote :3333

# print demangled symbols
set print asm-demangle on

# detect unhandled exceptions, hard faults and panics
break DefaultHandler
break HardFault
break rust_begin_unwind

monitor arm semihosting enable

load

# start the process but immediately halt the processor
stepi
```
通过执行类似"`gdb -x openocd.gdb target/examples/hello`"将会立即连接gdb到openocd，并使能semihosting，加载，然后开启板上程序执行。

## 4.日志

[dependencies]
log = "0.4.11"
rtt-logger = "0.1"
rtt-target = { version = "0.3.1", features = ["cortex-m"] }


rtt-logger :An rtt-target logger implementation for Cortex-M embedded platforms
log :A Rust library providing a lightweight logging facade.

use log::{LevelFilter};
use rtt_logger::RTTLogger;
use rtt_target::rtt_init_print;

static LOGGER: RTTLogger = RTTLogger::new(LevelFilter::Debug);

rtt_init_print!(BlockIfFull, 4098);
log::set_logger(&LOGGER).unwrap();
log::set_max_level(log::LevelFilter::Info);
log::info!("init");

# stm32f4xx_hal crate笔记

## 时钟配置

下面演示了两种配置，被注释代码演示了底层编码方式
```rust
    let dp = Peripherals::take().unwrap();
    let rcc = dp.RCC.constrain();
    let clocks = rcc.cfgr.sysclk(16.MHz()).pclk1(8.MHz()).freeze();

    // rcc.apb1enr.modify(|_, w| w.tim2en().set_bit());
    // let psc = (sysclk.0 / 1_000_000) as u16;
    // timer.psc.write(|w| w.psc().bits(psc - 1));
    // timer.egr.write(|w| w.ug().set_bit());
```

下面的例子更为直接：
```rust
use stm32f4xx_hal::{
    pac::{RCC, TIM2, TIM5},
    rcc::Clocks,
};
impl<const FREQ: u32> MonoTimer<TIM2, FREQ> {
    pub fn new(timer: TIM2, clocks: &Clocks) -> Self {
        let rcc = unsafe { &(*RCC::ptr()) };
        rcc.apb1enr.modify(|_, w| w.tim2en().set_bit());
        rcc.apb1rstr.modify(|_, w| w.tim2rst().set_bit());
        rcc.apb1rstr.modify(|_, w| w.tim2rst().clear_bit());
        let pclk_mul = if clocks.ppre1() == 1 { 1 } else { 2 };
        let prescaler = clocks.pclk1().0 * pclk_mul / FREQ - 1;
        timer.psc.write(|w| w.psc().bits(prescaler as u16));
        timer.arr.write(|w| unsafe { w.bits(u32::MAX) });
        timer.egr.write(|w| w.ug().set_bit());
        timer.sr.modify(|_, w| w.uif().clear_bit());
        timer.cr1.modify(|_, w| w.cen().set_bit().udis().set_bit());
        Self(timer)
    }
}
```

### STM32 DWT cycle counter (CYCCNT) surprising behavior (rust)

[STM32 DWT cycle counter (CYCCNT) surprising behavior (rust)](https://stackoverflow.com/questions/64475200/stm32-dwt-cycle-counter-cyccnt-surprising-behavior-rust)

### PWM的代码片段
```rust
use stm32f4xx_hal::pac::TIM1;
use stm32f4xx_hal::timer::{PwmChannel,C1,C2};
pub static mut G_CH1: Option<PwmChannel<TIM1,C1>> = None;
pub static mut G_CH2: Option<PwmChannel<TIM1,C2>> = None;

    let gpioa = dp.GPIOA.split();
    // Set up TIM1 PWM
    let channels = (gpioa.pa8.into_alternate(), gpioa.pa9.into_alternate());
    let pwm = dp.TIM1.pwm_hz(channels, 20.kHz(), &clocks).split();
    let (mut ch1, mut ch2) = pwm;
    unsafe {
        pwm::G_CH1.insert(ch1);
        pwm::G_CH2.insert(ch2);

        match pwm::G_CH1.as_mut() {
            Some(v) => {
                let max_duty = v.get_max_duty();
                v.set_duty(max_duty / 2);
                v.enable();
            }
            None => {}
        }
```

## gpio代码片段

```rust
use stm32f4xx_hal::gpio::{ExtiPin,PinExt, Output, PushPull,Alternate},
type LedPin = stm32f4xx_hal::gpio::PA5<Output<PushPull>>;
pub static mut G_LED: Option<LedPin> = None;
    let led = gpioa.pa5.into_push_pull_output();
    // Move the pin into our global storage
    unsafe {
        led::G_LED.insert(led);
    }
```

类型备忘:
```rust
    // let mut serial: hal::serial::Serial<
    //     _,
    //     (
    //         stm32f4xx_hal::gpio::Pin<Alternate<_, 8_u8>, 'C', 6_u8>,
    //         stm32f4xx_hal::gpio::Pin<Alternate<_, 8_u8>, 'C', 7_u8>,
    //     ),
    //     u8,    >
    let mut serial: hal::serial::Serial6<(
        hal::gpio::gpioc::PC6<Alternate<_, 8_u8>>,
        hal::gpio::gpioc::PC7<Alternate<_, 8_u8>>,
    )>
```

## stepper

[stepper crate include drv8825 driver](https://crates.io/crates/stepper), [its github](https://github.com/flott-motion/stepper)