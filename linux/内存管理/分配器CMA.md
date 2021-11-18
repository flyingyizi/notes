

# 分配器：CMA

连续内存分配器(CMA - Contiguous Memory Allocator)的作用

cma，全称（contiguous memory allocation）,在内存初始化时预留一块连续内存，可以在内存碎片化严重时通过调用dma_alloc_contiguous接口并且gfp指定为__GFP_DIRECT_RECLAIM从预留的那块连续内存中分配大块连续内存。

CMA的初始化必须在伙伴系统（buddy system）工作之前和memblock分配器初始化完成之后；

为了在内核中使用 CMA，在内核配置中请确保相关的宏已经启用, 如下:
```
CONFIG_CMA
CONFIG_CMA_AREAS
CONFIG_DMA_CMA
CONFIG_CMA_ALIGNMENT
```

## kernel 实现CMA

### 如何声明CMA

cma的使用和原理说明

cma区域可以通过设备树、cmdline和menuconfig指定，并且可以通过设备树的phandle机制和单独的设备绑定，具体的实现和原理说明如下:

设备树指定

在设备树中添加reserved-memory节点的子节点，并且该子节点compatible属性指定为shared-dma-pool。在需要使用该CMA连续内存的设备节点中通过memory-region引用该子节点.详细说明见 “linux\Documentation\devicetree\bindings\reserved-memory\reserved-memory.txt”

cmdline指定

在bootargs可以添加cma属性指定cma区域

bootargs = "cma= 0x400000@0x70000000-0x80000000"

其中0x400000是大小，0x70000000-0x80000000指定cma的分配区间。

在“kernel/dma/contiguous.c”文件中，`early_param("cma", early_cma)` 解析bootargs的cmd参数，并将cma的大小赋值给size_cmdline,cma的起始地址赋值给base_cmdline,cma的结束地址赋值给limit_cmdline。

### start_kernel初始化CMA

目的是保证内存被复用。 CMA 通过在启动阶段预先保留内存。这些内存叫做 CMA 区域或 CMA 上下文，稍后返 回给伙伴系统从而可以被用作正常申请。如果要保留内存，则需要恰好在底层 MEMBLOCK 分配器初始化之后，及大量内存被占用之前调用，并在伙伴系统建立 之前调用“dma_contiguous_reserve”：

在内核启动的过程中会声明CMA内存，具体流程如下：
```
start_kernel
 –>setup_arch
  –>arm_memblock_init
   –>dma_contiguous_reserve
    –>cma_declare_contiguous
     –>memblock_alloc_range 
最终调用memblock_alloc_range为CMA连续内存申请分配内存空间。
```

### CMA driver初始化
```
early_initcall(cma_init_reserved_areas);
 –>cma_activate_area 针对每一个page向下调用
  –>init_cma_reserved_pageblock  这里会将将页设置为MIGRATE_CMA类型
```

## 普通驱动使用CMA

从设备驱动角度看，任何事情都不应该被影响。因为 CMA 是被集成到 DMA 子系统， 所以以前调用 DMA API（例如dma_alloc_coherent()）的地方应该照常工作。事实 上，设备驱动永远不需要直接调用 CMA API，因为它是在页和页帧编号（PFNs）上 操作而无关总线地址和内核映射，并且也不提供维护缓存一致性的机制。获取更多 信息，可以参考这两份有用的文档。这两篇文档描述了DMA 提供的方法接口及使用 用例。

linux/Documentation/DMA-API.txt

linux/Documentation/DMA-API-HOWTO.txt

### 驱动的CMA内存初始化

`of_reserved_mem_device_init` 该函数实现与驱动相关的 CMA 内存的初始化。输入参数为 device 结构体。通常在设备驱动的 probe 阶段调用该函数，主要用来挂钩和设备相关的 cma 数据结构。

### 驱动申请CMA内存

内核如果 要分配 CMA 里面的连续物理内存，可以使用:`dma_alloc_from_contiguous` 该函数实现驱动对 CMA 内存的申请。输入参数 dev 为声明 CMA 的设备 device 结构体， count 表示要分配的 page 数量（以 page 为单位分配而非字节），align 参数表示以 2^align 个 page为单位对齐。分配成功则返回连续 page 的起始地址

### 释放CMA连续内存

当使用完 CMA 连续物理内存之后，可以将物理内存归还给 CMA 内存管理器， 使用如下:`dma_release_from_contiguous`释放dma_alloc_from_contiguous返回的page开始的连续count个page。建议申请和释放配对使用，并且两者的count参数保持一致。
