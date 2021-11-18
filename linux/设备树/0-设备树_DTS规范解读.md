
# DTS语法

## node-name@unit-addrss

在DTS中，每个设备(device)在设备树中是一个节点(node)。一个node定义为：
  
```
[label:] node-name[@unit-address] {
[properties definitions]
[child nodes]
}
```

   值得花点时间讨论一下命名约定。每个节点都必须有一个格式为<name>[@<unit-address>]的名称。
        
   `<name>`是一个简单的 ascii 字符串，最长可达 31 个字符。通常，节点根据它代表的设备类型来命名。例如3com 以太网适配器的节点将使用名称ethernet，而不是3com509.

    如果节点描述具有地址的设备，则包括单元地址(unit-address)。通常，单元地址是用于访问设备的主要地址，并列在节点的reg属性中。我们将在本文档后面介绍 reg 属性。

    The unit-address must match the first address specified in the reg property of the node. If the node has no reg property, the `@` and `unit-address` must be omitted and the node-name alone differentiates the node from other nodes at the same level in the tree.
        
    兄弟节点必须唯一命名，但只要地址不同（即serial@101f1000 & serial@101f2000），多个节点使用相同的通用名称是正常的。有关节点命名的完整详细信息，请参阅 ePAPR 规范的第 2.2.1 节。

    注意： 根节点没有“node name”，适用slash "/"指示。


    假设有下面一台由"Acme"制造名为 "Coyote's Revenge"的机器:

    - One 32bit ARM CPU
    - processor local bus attached to memory mapped serial port, spi bus controller, i2c controller, interrupt controller, and external bus bridge
    - 256MB of SDRAM based at 0
    - 2 Serial ports based at 0x101F1000 and 0x101F2000
    - GPIO controller based at 0x101F3000
    - SPI controller based at 0x10170000 with following devices
        - MMC slot with SS pin attached to GPIO #1
    - External bus bridge with following devices
        - SMC SMC91111 Ethernet device attached to external bus based at 0x10100000
        - i2c controller based at 0x10160000 with following devices
        - Maxim DS1338 real time clock. Responds to slave address 1101000 (0x58)
        - 64MB of NOR flash based at 0x30000000

    基于上面的信息,我们将每个节点来填充设备树，当前每个节点还是空的。
    ```
    /dts-v1/;

    / {
        compatible = "acme,coyotes-revenge";

        cpus {
            cpu@0 {            compatible = "arm,cortex-a9";        };
            cpu@1 {            compatible = "arm,cortex-a9";        };
        };

        serial@101F0000 {        compatible = "arm,pl011";    };

        serial@101F2000 {        compatible = "arm,pl011";    };

        gpio@101F3000 {        compatible = "arm,pl061";    };

        interrupt-controller@10140000 {        compatible = "arm,pl190";    };

        spi@10115000 {        compatible = "arm,pl022";    };

        external-bus {
            ethernet@0,0 {            compatible = "smc,smc91c111";        };

            i2c@1,0 {
                compatible = "acme,a1234-i2c-bus";
                rtc@58 {                compatible = "maxim,ds1338";            };
            };

            flash@2,0 {            compatible = "samsung,k8f1315ebm", "cfi-flash";        };
        };
    };
    ```

## compatible property

用于选择设备驱动。 建议格式是：`“manufacturer1,model1” [, “manufacturer2,model2”]...` . 选择匹配时从左到右扫描，一旦匹配立即终止匹配。

例子：`compatible = “fsl,mpc8641-uart”, “ns16550"; `

## model property

用于指示设备制造商的型号(model)。 见格式是：`“manufacturer,model”`

例子： `model = “fsl,MPC8349EMITX”;`


## phandle property

用于在设备数中为一个节点分配一个数字ID。 其他节点可以适用该phandle来引用该节点。  在老版本中也有使用`linux,phandle`的。

编写DTS时，强烈不建议显式示适用phandle， 它的目的应仅仅是DTS后台编译所使用。

```c++
pic@10000000 { 
   phandle = <1>; 
   interrupt-controller; 
}; 

/* A phandle value of 1 is defined. Another device node 
   could reference the pic node with a 22 phandle value of 1: */
xyz {
    ...
    interrupt-parent = <1>;
    ...
};
```

## status property

用于定义设备的运行状态。 从linux-source看，几乎没有使用该属性。

Table 2-4 Values for status property 8

|Value      |  Description           |
|-----------|------------------------|
|“okay”     |  指示设备处于运行态     |
|“disabled” |  指示设备处于disable态  |
|“fail”     |  指示设备处于失败态，如果不修复不会变为运行态 |
|“fail-sss” |  指示设备处于特定失败条件的失败态，sss代表条件。如果不修复不会变为运行态 |

例子：
```c++
&i2c2 {
	...
	status = "okay";

	ltc3676: pmic@3c {
        ...
    };
```

## #address-cells and #size-cells 

`"#address-cells and #size-cells"` 属性可用于在设备树中具有子节点的任何设备节点，这些属性用于描述设备节点中的子节点如何寻址。 

- `#address-cells`属性定义子节点中`reg`属性中的address由几个u32构成。
- `#size-cells`属性定义子节点中`reg`属性中的size(length)由几个u32构成。

注意`#address-cells and #size-cells` 属性并不会有继承处理，它们必须需要使用它们的节点中显式的定义。

虽然有上面的规范要求，为了安全起见，约定如果缺失，则应假定`#address-cells = <2>; #size-cells=<1>;`


## reg  property

`reg`属性用于描述 在其父总线域中的 设备资源的地址,通常理解为`<offset, length>`。

格式是`reg = <address1 length1 [address2 length2] [address3 length3] ... >`. 其中address，length 各由几个u32组成，由该属性所在节点的父节点中的`#address-cells and #size-cells`指定。特别的，如果`#size-cells=<0>;`, 则length应省略。

Example: 
Suppose a device within a system-on-a-chip had two blocks of registers—a 32-byte block at offset 0x3000 in the SOC and a 256-byte block at offset 0xFE00. The reg property would be encoded as follows (assuming #address-cells and #size-cells values of 1): `reg = <0x3000 0x20 0xFE00 0x100>;`

## ranges property

用于提供节点地址域，与其父节点地址域的转换。
The ranges property provides a means of defining a mapping or translation between the address space of the bus (the child address space) and the address space of the bus node's parent (the parent address space). 

格式是`ranges = <child-bus-address1 parent-bus-address1  length1 [child-bus-address2 parent-bus-address2 length2] ... >`

- `child-bus-address` : is a physical address within the child bus’ address space. The number of cells to represent the address is bus dependent and can be determined from the #address-cells of this node (the node in which the ranges property appears). 
- `parent-bus-address`: is a physical address within the parent bus’ address space. The number of cells to represent the parent address is bus dependent and can be determined from the #address-cells property of the node that defines the parent’s address space. 
- `length` : specifies the size of the range in the child’s address space. The number of cells to represent the size can be determined from the #size-cells of this node (the node in which the ranges property appears). 

If the property is defined with an <empty> value, it specifies that the parent and child address space is identical, and no address translation is required. 
例子：
```c++
	reserved-memory {
		#address-cells = <2>;
		#size-cells = <2>;
		ranges;
		/*
		 * We just move frame buffer area to the very end of
		 * available DDR. And even though in case of ARC770 there's
		 * no strict requirement for a frame-buffer to be in any
		 * particular location it allows us to use the same
		 * base board's DT node for ARC PGU as for ARc HS38.
		 */
		frame_buffer: frame_buffer@9e000000 {
			compatible = "shared-dma-pool";
			reg = <0x0 0x9e000000 0x0 0x2000000>;
			no-map;
		};
	};
```

If the property is not present in a bus node, it is assumed that no mapping exists between children of the node and the parent address space.

例子：
```c++
/{
   soc {
        compatible = "simple-bus";
        #address-cells = <1>;
        #size-cells = <1>;
        /* specifies that for an 1024KB(0x00100000) range of address space, a child node addressed at physical 0x0 maps to a 
           parent address of physical 0xe0000000. */
        /* With this mapping, the serial device node can be addressed by a load or store at address 0xe0004600, 
           an offset of 0x4600 (specified in reg) plus the 0xe0000000 mapping specified in ranges*/
        ranges = <0x0 0xe0000000 0x00100000>;       
        ...
        serial@4600 {
            device_type = "serial";
            compatible = "ns16550";
            reg = <0x4600 0x100>;
            clock-frequency = <0>;
            interrupts = <0xA 0x8>;
            interrupt-parent = < &ipic >;            
        };
   }
};
```
## "Path Names"
## "virtual-reg"
## "dma-ranges"


# device tree usage

- 一个dts文件中就是若干嵌套组成的node，property以及child note、child note property描述。
- 在DTS中，涉及到"property"属性，格式是"`<property>[=<value>];`." 值：
  - empty, 可能没有，
  - 可能一个或多个u32值(e.g. `<0x40014000 0x400>`)，
  - 可能为字符串(e.g. `"memory"`)，
  - 可能为字符串列表(e.g. `"PowerPC,970"`) 

- 在DTS中，涉及到"cell"这个术语，在Device Tree表示32bit的信息单位

## 寻址原理

如果一个device node中包含了有寻址需求（要定义reg property）的sub node（后文也许会用child node，和sub node是一样的意思），那么该device node就必须要定义这两个属性:“#address-cells”/“#size-cells”

可寻址设备（device）使用以下属性将地址信息编码在设备树里面：
- `reg`  ： 格式是`reg = <address1 length1 [address2 length2] [address3 length3] ... >`. 每个“address1 length”代表被设备使用的address range。每个address 是x个u32的list，每个length是y个u32的list.
  总结来说就是reg信息怎么解析，由reg所在子节点的关联父节点的“#address-cells”/“#size-cells”规定来确定。
- `#address-cells=<x>`  : 表示用x 个u32 来描述地址
- `#size-cells=<y>`     ：表示用y 个u32 来描述大小（长度）

注意： 按照惯例，如果节点具有reg属性，则节点名称必须包含单元地址，这是reg属性中的第一个地址值。 这不是强制性的。

例如：
```cpp
/ {
        /**
         * 在 root node 下的sub-node使用 1 个 u32 來代表 address。
         * 在 root node 下的sub-node使用 0 个 u32 來代表 size。
        */
        #address-cells = <0x1>;    
        #size-cells = <0x0>; 
        ...
        memory {        // memory device
                ... 
                reg = <0x90000000>; // 0x90000000 是存取 memory 的 address 
                ... 
        }; 
        ... 
  }
```

```c++
/ {
        #address-cells = <0x2>;  
        #size-cells = <0x1>;  
        ...
        memory@90000000,00000000 {        // memory device
            ... 
            reg = <0x90000000 00000000 0x800000>; // 0x90000000 00000000 是存取 memory 的 address // 0x800000 是 memory 的 size。 
            ... 
        }; 
        ... 
  }
```

## 地址转换

注意非根直接子节点设备分配的地址仅仅是设备域（device domain）的，和CPU使用的地址（cpu domain）是不同的域，因此涉及设备地址到CPU可以使用地址的转换。

约定：

- 如果节点不是根的直接子节点，显然该节点不使用 CPU 的地址域。为了获得内存映射地址，设备树必须指定如何将地址从一个域转换为另一个域。ranges物业用于此目的。

- 根节点总是描述 CPU 对地址空间的看法。根的直接子节点已经在使用 CPU 的地址域，因此不需要任何显式映射。

- 如果父地址空间和子地址空间相同，则节点可以改为添加空ranges属性。空范围属性的存在意味着子地址空间中的地址按 1:1 映射到父地址空间。

我们已经讨论了如何为设备分配地址，但此时这些地址仅适用于设备节点。它还没有描述如何从这些地址映射到 CPU 可以使用的地址。

ranges是地址转换列表。范围表中的每个条目都是一个元组，包含子地址、父地址和子地址空间中区域的大小。每个字段的大小是通过取子#address-cells值、父#address-cells值和子#size-cells值来确定的。对于我们示例中的external-bus，子地址为 2 个单元格，父地址为 1 个单元格，大小也是 1 个单元格。 参见下面例子中的注释说明。

例子：添加了 range 属性的示例设备树。

```c++
/dts-v1/;

/ {
    compatible = "acme,coyotes-revenge";
    #address-cells = <1>;
    #size-cells = <1>;
    /* 根直接子节点，因此直接分配地址 0x101f0000 */
    serial@101f0000 { 
        compatible = "arm,pl011"; reg = <0x101f0000 0x1000>; 
    };     
    ...
    external-bus {
        #address-cells = <2>;
        #size-cells = <1>;
        /*  <the child address, the parent address, and the size of the region in the child address space>.*/
        /* 设备地址"0 0"映射到CPU可访问地址0x10100000..0x1010ffff*/
        /* 设备地址"1 0"映射到CPU可访问地址0x10160000..0x1016ffff*/
        /* 设备地址"2 0"映射到CPU可访问地址0x30000000..0x30ffffff*/
        ranges = <0 0  0x10100000   0x10000     // Chipselect 1, Ethernet
                  1 0  0x10160000   0x10000     // Chipselect 2, i2c controller
                  2 0  0x30000000   0x1000000>; // Chipselect 3, NOR Flash

        ethernet@0,0 {
            compatible = "smc,smc91c111";
            reg = <0 0 0x1000>;
        };
        
        /* You should also notice that there is no ranges property in the i2c@1,0 node. The reason for 
           this is that unlike the external bus, devices on the i2c bus are not memory mapped on the CPU's 
           address domain. Instead, the CPU indirectly accesses the rtc@58 device via the i2c@1,0 device. 
           The lack of a ranges property means that a device cannot be directly accessed by any device other than it's parent.*/
        i2c@1,0 {
            compatible = "acme,a1234-i2c-bus";
            #address-cells = <1>;
            #size-cells = <0>;
            reg = <1 0 0x1000>;
            rtc@58 {
                compatible = "maxim,ds1338";
                reg = <58>;
            };
        };

        flash@2,0 {
            compatible = "samsung,k8f1315ebm", "cfi-flash";
            reg = <2 0 0x4000000>;
        };
    };
};
```

## 传递runtime parameter

chosen node主要用来描述由系统firmware指定的runtime parameter。如果存在chosen这个node，其parent node必须是名字是“/”的根节点。原来通过tag list传递的一些linux kernel的运行时参数可以通过Device Tree传递。属性包括:
- bootargs : command line可以通过bootargs这个property这个属性传递；
- `linux,initrd-start` 与 `linux,initrd-end`: initrd的开始地址也可以通过linux,initrd-start这个property这个属性传递。在本例中，chosen节点是空的，在实际中，建议增加一个bootargs的属性，例如：

- stdout-path : A string that specifies the full path to the node representing the device to be used for boot console output. If the character “:” is present in the value it terminates the path. The value may be an alias. If the stdin-path property is not specified, stdout-path should be assumed
to define the input device.

- stdin-path ： used for boot console input
例子：
```c++
	chosen {
		bootargs = "root=/dev/ram";
		stdout-path = "serial0:115200n8";
		linux,initrd-start = <0x4500040>;
		linux,initrd-end   = <0x4800000>;
	};
```

## 别名 (/aliases 节点)

alias节点必须位于根，而且名字必须是aliases，即该节点全路径必须是"`/aliases`". 该节点的每条属性的格式是“`alias-name = string-of-full-path-to-a-node;`”, 其中“`alias-name`”的格式是“`<stem><id>`”, 例如对“spi0 = "/spi@13920000";”，stem是"spi",id是0.

例子如下：

```c++
/ {
	...

	aliases {
		serial0 = &uart0;
        spi0 = "/spi@13920000";
	};

	fpga {
		...
		uart0: serial@f0000000 {
            ...
		};
    };
};    
```

## 列表

下面两种列表表达方式是等效的
```
			clocks = <&clk_osc>,
				<&dsi0 0>, <&dsi0 1>, <&dsi0 2>,
				<&dsi1 0>, <&dsi1 1>, <&dsi1 2>;
```
```
			clocks = <&clk_osc &dsi0 0 &dsi0 1 &dsi0 2 &dsi1 0 &dsi1 1 &dsi1 2>;
```




# device bindings

bindings这个概念的理解：

- 当通过设备树来描述一个设备时，bindings应是该设备树所有属性的一个子集。 该子集能为该设备驱动提供所需的足够信息。

- 在设备树规范中，对每个设备，提出了device bindings规范要求。比如对Serial devices，要求有clock-frequency Property。

- 在linux-source中，每一个内核子系统或者framework都有自己的bindings要求，所有内核支持的bindings说明文档都位于Documentation/devicetree/bindings。这里面的内容就是内核所支持设备的支持代码所需要的设备树属性要求。

总结： DTS规范（devicetree-specification-v0.3.pdf）是basic，定义了通用的属性、规则，以及某些设备（比如serial devices）的bindings，它不可能罗列所有设备。比如对特定设备（比如gpio），它需要哪些属性？比如`gpio-controller`就是在linux下的bindings里面定义的。
