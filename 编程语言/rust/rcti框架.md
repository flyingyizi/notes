
[Real-Time Interrupt-driven Concurrency](https://rtic.rs/1/book/en/)

The device field is available when the peripherals argument is set to the default value true. In the rare case you want to implement an ultra-slim application you can explicitly set peripherals to false.

当设置“peripherals = true”， 在init通过上下文可以获得“cx.device”。这个设置也是默认值。
```rust
#[rtic::app(device = lm3s6965, peripherals = true)]
mod app {
    use cortex_m_semihosting::{debug, hprintln};

    //在#[shared]与#[local]声明的，是需要在init中访问处理，然后其他task在关联的
    #[shared]
    struct Shared {}
    #[local]
    struct Local {local_to_foo: i64,local_to_bar: i64,}

    #[init(local = [x: u32 = 0])]
    fn init(cx: init::Context) -> (Shared, Local, init::Monotonics) {
        // Cortex-M peripherals
        let _core: cortex_m::Peripherals = cx.core;

        // Device specific peripherals
        let _device: lm3s6965::Peripherals = cx.device;

        // Locals in `init` have 'static lifetime
        let _x: &'static mut u32 = cx.local.x;

        // Access to the critical section token,
        // to indicate that this is a critical seciton
        let _cs_token: bare_metal::CriticalSection = cx.cs;

        hprintln!("init").unwrap();

        debug::exit(debug::EXIT_SUCCESS); // Exit QEMU simulator

        (Shared {}, Local {}, init::Monotonics())
    }

    // 如果是没有#[shared]与#[local]声明的，那就是这个task专有的
    // `local_to_foo` can only be accessed from this context
    #[task(local = [local_to_foo,x: u32 = 0])]
    fn foo(cx: foo::Context) {
        let local_to_foo = cx.local.local_to_foo;
        *local_to_foo += 1;

        // error: no `local_to_bar` field in `foo::LocalResources`
        // cx.local.local_to_bar += 1;

        hprintln!("foo: local_to_foo = {}", local_to_foo).unwrap();
    }    
}
```

## 例子

stm32f4xx-hal crate 源码的example中有例子； 在“https://github.com/rtic-rs/rtic-examples”有官方例子,其中一个演示了如何在rtci框架下分模块

[使用网卡的例子](https://github.com/jonlamb-gh/stm32f429-smoltcp-mqtt-rtic/blob/master/src/main.rs)

[The goal of this project is to have a serial to wireless converter that allows the read and write to a serial port over wifi.](https://github.com/glbsalazar/wasp)