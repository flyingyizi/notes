
[introduce I2C subsystem at page 175](https://bootlin.com/doc/training/linux-kernel/linux-kernel-slides.pdf)

What is I2C?
- A very commonly used low-speed bus to connect on-board and external devices to the processor.
- Uses only two wires: SDA for the data, SCL for the clock.
- It is a master/slave bus: only the master can initiate transactions, and slaves can only reply to transactions initiated by masters.
- In a Linux system, the I2C controller embedded in the processor is typically the master, controlling the bus.
- Each slave device is identified by an I2C address (you can’t have 2 devices with the same address on the same bus). Each transaction initiated by the master contains this address, which allows the relevant slave to recognize that it should reply to this particular transaction

An I2C bus example:
```
 ┌──────────────┐
 │ ┌──────────┐ │
 │ │ I2C      │ │
 │ │controller◄─┼───────┬────────────────┬───────────────┐
 │ └──────────┘ │       │                │               │
 │              │ ┌─────▼──────┐  ┌──────▼─────┐  ┌──────▼─────┐
 │processor     │ │  I2C       │  │  I2C       │  │ I2C        │
 └──────────────┘ │touchscreen │  │  GPIO      │  │audio       │
                  │ controller │  │ controller │  │codec       │
                  └────────────┘  └────────────┘  └────────────┘
                    addr=0x2C       addr=0x1A       addr=0x6E

```

- The core of the I2C bus driver is located in `drivers/i2c/`.
- The I2C controller drivers are located in `drivers/i2c/busses/`.
- The I2C device drivers are located throughout `drivers/`, depending on the
  framework used to expose the devices (e.g. `drivers/input/` for input devices).
## Registering an I2C device, in the DT

In the Device Tree, the I2C controller device is typically defined in the .dtsi file
that describes the processor. Normally defined with status = "disabled".

At the board/platform level:
- the I2C controller device is enabled (status = "okay")
- the I2C bus frequency is defined, using the clock-frequency property.
- the I2C devices on the bus are described as children of the I2C controller node,
  where the reg property gives the I2C slave address on the bus.

- See the binding for the corresponding driver for a specification of the expected DT
properties. Example: Documentation/devicetree/bindings/i2c/i2c-omap.txt

### Definition of the I2C controller
DT example From arch/arm/boot/dts/sun7i-a20.dtsi

```
i2c0: i2c@01c2ac00 {
    compatible = "allwinner,sun7i-a20-i2c",
    "allwinner,sun4i-a10-i2c";
    reg = <0x01c2ac00 0x400>;
    interrupts = <GIC_SPI 7 IRQ_TYPE_LEVEL_HIGH>;
    clocks = <&apb1_gates 0>;
    status = "disabled";
    #address-cells = <1>;
    #size-cells = <0>;
};
```

### Definition of the I2C device

DT example From arch/arm/boot/dts/sun7i-a20-olinuxino-micro.dts
```
&i2c0 {
    pinctrl-names = "default";
    pinctrl-0 = <&i2c0_pins_a>;
    status = "okay";
    axp209: pmic@34 {
        compatible = "x-powers,axp209";
        reg = <0x34>;
        interrupt-parent = <&nmi_intc>;
        interrupts = <0 IRQ_TYPE_LEVEL_LOW>;
        interrupt-controller;
        #interrupt-cells = <1>;
    };
};
```

## Registering an I2C device driver  

example : `"drivers\iio\accel\adxl345_i2c.c"`


## Communicating with the I2C device: 

### raw API

The most basic API to communicate with the I2C device provides functions to either
send or receive data:
- `int i2c_master_send(const struct i2c_client *client, const char *buf, int count);`
Sends the contents of buf to the client.

- `int i2c_master_recv(const struct i2c_client *client, char *buf, int count);`
  Receives count bytes from the client, and store them into buf.

Both functions return a negative error number in case of failure, otherwise the number
of transmitted bytes.

### message transfer

The message transfer API allows to describe transfers that consists of several
messages, with each message being a transaction in one direction:

- `int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num);`
- The `struct i2c_adapter` pointer can be found by using client->adapter
- The `struct i2c_msg` structure defines the length, location, and direction of the message.

example From drivers/input/touchscreen/st1232.c:
```c
static int st1232_ts_read_data(struct st1232_ts_data *ts)
{
...
struct i2c_client *client = ts->client;
struct i2c_msg msg[2];
int error;
...
u8 start_reg = ts->chip_info->start_reg;
u8 *buf = ts->read_buf;
/* read touchscreen data */
msg[0].addr = client->addr;
msg[0].flags = 0;
msg[0].len = 1;
msg[0].buf = &start_reg;
msg[1].addr = ts->client->addr;
msg[1].flags = I2C_M_RD;
msg[1].len = ts->read_buf_len;
msg[1].buf = buf;
error = i2c_transfer(client->adapter, msg, 2);
...
}
```

### SMBus calls

SMBus is a subset of the I2C protocol. It defines a standard set of transactions, such as reading/writing from a register-like interface.

Linux provides SMBus functions that should preferably be used instead of the raw
API with devices supporting SMBus.

- Such a driver will be usable with both SMBus and I2C adapters
    - SMBus adapters cannot send raw I2C commands
    - I2C adapters will receive an SMBus-like command crafted by the core

- Example: the i2c_smbus_read_byte_data() function allows to read one byte of data from a device “register”.
    - It does the following operations:
    `S Addr Wr [A] Comm [A] Sr Addr Rd [A] [Data] NA P`
    - Which means it first writes a one byte data command (Comm, which is the
    “register” address), and then reads back one byte of data ([Data]).

- See [i2c/smbus-protocol](https://www.kernel.org/doc/html/latest/i2c/smbus-protocol.html) for details.

List of SMBus functions

```c++
 //Read/write one byte
 s32 i2c_smbus_read_byte(const struct i2c_client *client);
 s32 i2c_smbus_write_byte(const struct i2c_client *client, u8 value);
 //Write a command byte, and read or write one byte
 s32 i2c_smbus_read_byte_data(const struct i2c_client *client, u8 command);
 s32 i2c_smbus_write_byte_data(const struct i2c_client *client, u8 command, u8 value);
 //Write a command byte, and read or write one word
 s32 i2c_smbus_read_word_data(const struct i2c_client *client, u8 command);
 s32 i2c_smbus_write_word_data(const struct i2c_client *client, u8 command, u16 value);
 //Write a command byte, and read or write a block of data (max 32 bytes)
 s32 i2c_smbus_read_block_data(const struct i2c_client *client, u8 command, u8 *values);
 s32 i2c_smbus_write_block_data(const struct i2c_client *client, u8 command, u8 length, const u8 *values);
 //Write a command byte, and read or write a block of data (no limit)
 s32 i2c_smbus_read_i2c_block_data(const struct i2c_client *client, u8 command, u8 length, u8 *values);
 s32 i2c_smbus_write_i2c_block_data(const struct i2c_client *client, u8 command, u8 length, const u8 *values);
```