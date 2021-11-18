这篇文章写的最清楚[内存管理](https://www.cnblogs.com/LoyenWang/tag/%E5%86%85%E5%AD%98%E7%AE%A1%E7%90%86/)

[Linux内存管理专题](https://www.cnblogs.com/arnoldlu/p/8051674.html#memory_management_architecture)

[万字整理，肝翻Linux内存管理所有知识点](https://www.cnblogs.com/ludongguoa/p/15318287.html)

[Understanding the Linux Virtual Memory Manager](https://www.kernel.org/doc/gorman/html/understand/understand005.html)


对机器硬件的RAM，linux分为三大部分：
- reserved(静态内存) : 首先，内存中的某些部分是永久的分配给内核的，比如内核代码段和数据段，ramdisk和fdt占用的空间等，它们是系统内存的一部分，但是不能被侵占，也不参与内存分配，称之为静态内存；
- reserved(预留内存) : 其次，GPU，Camera等都需要预留大量连续内存，这部分内存平时不用，但是系统必须提前预留好，称之为预留内存；
- memory(动态内存)最后，内存的其余部分称之为动态内存（dynamic memory），是需要内核管理的宝贵资源；


对内存管理分为两个阶段：
    - 先使用memblock工作 ： memblock在系统启动阶段进行简单的内存管理，记录物理内存的使用情况；memblock把物理内存划分为若干内存区，按使用类型分别放在memory和reserved两个集合（数组）中，memory即动态内存的集合，reserved集合包括静态内存和预留内存；
    - 然后是伙伴系统工作


linux kernl 概念：
- 内核逻辑/线性地址：其实对于linux内核来说，这个地址等同于物理地址，只是它们之间有一个固定的偏移量，linux内核中常提到的逻辑地址和线性地址其实是同一个东西. 在内核代码中常用"`virt`"指示。
  
- 内核虚拟地址：与上面的内核逻辑地址的区别在于，内核虚拟地址不一定是在硬件物理上是连续的，有可能是通过分页映射的不连续的物理地址. 在内核代码中指示使用"`page's virtual address`".对没有高端内存的，比如arm,这个virtual就是virt.


# 1.临时内核页表创建与开启MMU

CPU寻址的方式：
- 未使用MMU：CPU发出地址(PA) 直接内存寻址(SDRAM or DDRx)。
- 使用MMU  ：CPU发出地址(VA) MMU接收CPU发来的地址 经过某种转换变成实际的物理地址去寻址。


临时内核页表(init page table)由"`__create_page_tables`"创建。

__create_page_tables在内核代码区TEXT_OFF下部的16KB区域内进行页表的配置，完成turn_mmu_on的平映射以及kernel image的线性映射。

identify map，也就是物理地址和虚拟地址是相等的。为什么需要这么一个映射呢？我们都知道在MMU打开之前，CPU访问的都是物理地址，那么当MMU打开后访问的就是虚拟地址了，这段页表的映射就是从CPU到打开MMU之前的这段代码物理地址的映射，防止开启MMU后，无法获取页表。

swapper_pg_dir:Linux内核编译后，kernel image是需要进行映射的，包括text，data等各种段。

开启MMU是通过"`__enable_mmu-->__turn_mmu_on`"。

arm32：[arm-linux内核start_kernel之前启动分析（2）- 页表的准备](https://blog.csdn.net/skyflying2012/article/details/41447843)
arm64：[ARMv8 MMU及Linux页表映射](https://www.cnblogs.com/LoyenWang/p/11406693.html)


# 2.开启MMU之后

开启MMU之前 cpu pc使用的都是物理地址PA，开启MMU之后，cpu pc是使用的就是虚拟地址VA。

在__enable_mmu之后，执行“`__mmap_switched-->start_kernel`”,显然__mmap_switched已经执行在开启了mmu之后，并且此时init page table已经是就绪了。

下面的代码片段截取的是与内存管理相关部分的代码。

```c++
asmlinkage __visible void __init start_kernel(void)
{
	...
	/**
	 * 该函数初始化高端内存（High Memory）线性地址空间中永久映射相关的全局变量。所以在不支持高端内存即在没有配置CONFIG_HIGHMEM这个
	 * 宏的时候，该函数是个空函数什么也不做，在ARM系统中，是没有高端内存相关的代码的，所以这个函数就是个空函数。
	*/
	page_address_init();
	...
	setup_arch(&command_line);
	...
	/**
	 * 将.data.percpu中的数据拷贝到每个CPU的数据段中，每个CPU一份。
	 * 其中CPU n 对应的专有数据区的首地址为__per_cpu_offset[n]
	*/
	setup_per_cpu_areas();
	...
	/**
	 * build_all_zonelists主要的工作就是在当前处理的结点和系统中其它节点的内存域zone之间建立一种等级次序，之后将根据这种次序来分配内存
	*/
	build_all_zonelists(NULL);
	/**
	 * hotplug cpu下的callback设置。内存页初始化，主要是在cpu发生热插拔时将CPU管理的页面移动到空闲列表
	*/
	page_alloc_init();
	...
	/**
	 * mm_init对内存相关模块初始化，比如其中调用与架构相关的 mem_init
	 * 主要功能就是:对各种内存分配器的初始化，将memblock管理的空闲内存释放到伙伴系统...
	*/
	mm_init();
	...
	kmem_cache_init_late();
	...	
	...
	/**
	 * 初始化pcp(每cpu页框高速缓冲)
	 * 完成对每个cpu每个zone的pageset遍历，利用pageset_init()初始化pcp链表，和利用pageset_set_high_and_batch为每个pageset计算每次在高速缓存中将要添加或被删去的页框个数
	*/
	setup_per_cpu_pageset();
    ...
	numa_policy_init();
	...
```

```c++
//linux\arch\arm\kernel\setup.c


void __init setup_arch(char **cmdline_p)
{
	const struct machine_desc *mdesc;

	setup_processor();
    /**
	 * 对平台对象初始化
	 * 初始平台对象是"`__mach_desc_GENERIC_DT`", 由"`DT_MACHINE_START(GENERIC_DT, "Generic DT based system")`"声明
	 * 
	 * 这里面会探测系统可用内存
	 * 以fdt为例，内核解析memory节点，取得物理内存的拓扑结构（起始地址及大小），并添加到memblock子系统中，代码如下：
	 * "`setup_machine_fdt-->early_init_dt_scan_nodes-->of_scan_flat_dt(遍历Nodes)-->early_init_dt_scan_memory(初始化单个内存Node)`"。
	*/
	mdesc = setup_machine_fdt(__atags_pointer);
	if (!mdesc)
		mdesc = setup_machine_tags(__atags_pointer, __machine_arch_type);
	machine_desc = mdesc;
	machine_name = mdesc->name;
	dump_stack_set_arch_desc("%s", mdesc->name);

	if (mdesc->reboot_mode != REBOOT_HARD)
		reboot_mode = mdesc->reboot_mode;

	/**
	 * global init_mm信息是内核内存管理上下文
	*/
	init_mm.start_code = (unsigned long) _text;
	init_mm.end_code   = (unsigned long) _etext;
	init_mm.end_data   = (unsigned long) _edata;
	init_mm.brk	   = (unsigned long) _end;

	/* populate cmd_line too for later use, preserving boot_command_line */
	strlcpy(cmd_line, boot_command_line, COMMAND_LINE_SIZE);
	*cmdline_p = cmd_line;

	/**
	 * 在arm32是为两个页表（ swapper_pg_dir，bm_pte）建立联系，注意此时bm_pte中内容还没有填写，
	 * 在arm32由于由于是2级映射，因此其实是pgd(pud, pmd)-->pte
	*/
	early_fixmap_init();
	early_ioremap_init();

	parse_early_param();

#ifdef CONFIG_MMU
	early_mm_init(mdesc);
#endif
	setup_dma_zone(mdesc);
	xen_early_init();
	efi_init();
	/*
	 * Make sure the calculation for lowmem/highmem is set appropriately
	 * before reserving/allocating any mmeory
	 */
	adjust_lowmem_bounds();
	/**
	 * memblock_init会对整个物理内存进行整理，主要的工作就是将一些特殊的区域添加进reserved内存中
	*/
	arm_memblock_init(mdesc);
	/*
	 * Memory may have been removed so recalculate the bounds. 
	 * Make sure the calculation for lowmem/highmem is set appropriately
	 * before reserving/allocating any mmeory
	 */
	adjust_lowmem_bounds();

	early_ioremap_reset();

    /**
	 * 在paging_init调用之前，存放Kernel Image和DTB的两段物理内存区域可以访问了（相应的页表已经建立好,参见head.S中__create_page_tables）。
	 * 尽管物理内存已经通过memblock_add添加进系统，但是这部分的物理内存到虚拟内存的映射还没有建立，可以
	 * 通过memblock_alloc分配一段物理内存，但是还不能访问，一切还需要等待paging_init的执行。最终页表建立好后，可以通过虚拟地址去访问最终的物理地址了。
	*/
	paging_init(mdesc);
	request_standard_resources(mdesc);

	if (mdesc->restart)
		arm_pm_restart = mdesc->restart;

	unflatten_device_tree();

	arm_dt_init_cpu_maps();
	psci_dt_init();
#ifdef CONFIG_SMP
	if (is_smp()) {
		if (!mdesc->smp_init || !mdesc->smp_init()) {
			if (psci_smp_available())
				smp_set_ops(&psci_smp_ops);
			else if (mdesc->smp)
				smp_set_ops(mdesc->smp);
		}
		smp_init_cpus();
		smp_build_mpidr_hash();
	}
#endif

	if (!is_smp())
		hyp_mode_check();

	reserve_crashkernel();

#ifdef CONFIG_MULTI_IRQ_HANDLER
	handle_arch_irq = mdesc->handle_irq;
#endif

#ifdef CONFIG_VT
#if defined(CONFIG_VGA_CONSOLE)
	conswitchp = &vga_con;
#elif defined(CONFIG_DUMMY_CONSOLE)
	conswitchp = &dummy_con;
#endif
#endif

	if (mdesc->init_early)
		mdesc->init_early();
}

```




## 2.1.物理内存初始化

Node->Zone->Page的层级进行初始化，直到内存达到可用状态。关于Nodes、Zones、Pages三者之间的关系，[《ULVMM》](https://www.kernel.org/doc/gorman/html/understand/understand005.html) Figure 2.1介绍。它们的关系为：
```
     1 n       1  n
Node ---> Zone ---> Page
```
一个node包含多个zone，一个zone包含多个page

- 每个node由pg_data_t表示，类型是"`struct pglist_data`" .
- Zones are described by a struct zone_struct
- Every physical page frame in the system has an associated "`struct page`"

### A.用户空间和内核空间划分
在arch/arm/include/asm/memory.h中，可以看出PAGE_OFFSET是用户空间和内核空间的分水岭。也是用内核空间的起始点。
其中CONFIG_PAGE_OFFSET这个值是通过“`prompt "Memory split"`”menuconfig配置得到的，参见kconfig:

```TEXT
choice
	prompt "Memory split"
	depends on MMU
	default VMSPLIT_3G
	help
	  Select the desired split between kernel and user memory.

	  If you are not absolutely sure what you are doing, leave this
	  option alone!

	config VMSPLIT_3G
		bool "3G/1G user/kernel split"
	config VMSPLIT_3G_OPT
		depends on !ARM_LPAE
		bool "3G/1G user/kernel split (for full 1G low memory)"
	config VMSPLIT_2G
		bool "2G/2G user/kernel split"
	config VMSPLIT_1G
		bool "1G/3G user/kernel split"
endcho

config PAGE_OFFSET
	hex
	default PHYS_OFFSET if !MMU
	default 0x40000000 if VMSPLIT_1G
	default 0x80000000 if VMSPLIT_2G
	default 0xB0000000 if VMSPLIT_3G_OPT
	default 0xC0000000
```

```c++
/* PAGE_OFFSET - the virtual address of the start of the kernel image */
#define PAGE_OFFSET        UL(CONFIG_PAGE_OFFSET)

static inline phys_addr_t __virt_to_phys(unsigned long x)
{
    return (phys_addr_t)x - PAGE_OFFSET + PHYS_OFFSET;
}

static inline unsigned long __phys_to_virt(phys_addr_t x)
{
    return x - PHYS_OFFSET + PAGE_OFFSET;
}
```

补充：上面的描述是对ARM32. 在ARM64，是通过CONFIG_ARM64_VA_BITS来间隔内核空间和用户空间。比如我使用的内核中有效位配置为CONFIG_ARM64_VA_BITS=39，用户空间地址范围：0x00000000_00000000 ~ 0x0000007f_ffffffff，大小为512G，内核空间地址范围：0xffffff80_00000000 ~ 0xffffffff_ffffffff，大小为512G。

### B.探测系统可用内存
内核是如何知晓物理内存的拓扑结构呢？相信很多人都有过类似的疑问；
通过DDR的模式寄存器(MR8)，可以很容易获得内存密度，进而推断出内存容量，这部分工作通常由bootloader完成，然后使用fdt或者atag等方式传递给内核；
以fdt为例，内核解析memory节点，取得物理内存的拓扑结构（起始地址及大小），并添加到memblock子系统中，代码如下：

"`start_kernel-->setup_arch-->setup_machine_fdt-->early_init_dt_scan_nodes-->of_scan_flat_dt(遍历Nodes)-->early_init_dt_scan_memory(初始化单个内存Node)`"。

```c++
void early_init_dt_add_memory_arch(base, size)
{
     ...
     memblock_add(base, size);
}

early_init_dt_scan_memory()
{
     ... ...
     reg = of_get_flat_dt_prop(node, "reg", &l);
     ... ...
     while ((endp - reg) >= (dt_root_addr_cells + dt_root_size_cells)) {
          u64 base, size;

          base = dt_mem_next_cell(dt_root_addr_cells, &reg);
          size = dt_mem_next_cell(dt_root_size_cells, &reg);
          ... ...
          early_init_dt_add_memory_arch(base, size);
     }
}
```

## 2.2.分配器：memblock

memblock在系统启动阶段进行简单的内存管理，记录物理内存的使用情况；memblock把物理内存划分为若干内存区，按使用类型分别放在memory和reserved两个集合（数组）中，memory即动态内存的集合，reserved集合包括静态内存和预留内存；

memblock初始化（还没有讲物理内存信息放进去）是直接定义的"`struct memblock memblock __initdata_memblock = {...}`"

如果需要了解memblock的详细分配流程，可以通过在bootargs中加入“memblock=debug”。在内核启动后，通过/proc/kmsg查看调试信息。

查看内存地址范围和reserved区域可以通过："`/sys/kernel/debug/memblock/memory`","`/sys/kernel/debug/memblock/reserved`"

"`start_kernel-->setup_arch--->arm_memblock_init`",在其中会处理内核代码段、initrd、dtb以及dtb中reserved-memory。这里要注意：对DTS中的"memory"节点的信息是在之前的“探测系统可用内存”阶段（"`start_kernel-->setup_arch-->setup_machine_fdt-->early_init_dt_scan_nodes-->of_scan_flat_dt(遍历Nodes)-->early_init_dt_scan_memory(初始化单个内存Node)-->memblock_add`"。）就已经放进memblock去了。

内核在free_all_bootmem()中将memblock中reserved之外的regions返还给内核的buddy allocator使用。

```c++
struct memblock {
    bool bottom_up;  /*分配器的分配方式 is bottom up direction? */
    phys_addr_t current_limit;  //内存块大小的限制。
    
    struct memblock_type {
        unsigned long cnt;    /* number of regions */
        unsigned long max;    /* size of the allocated array */
        phys_addr_t total_size;    /* size of all regions */

        struct memblock_region {
            phys_addr_t base; //region的基地址
            phys_addr_t size; //region的大小
            /* Definition of memblock flags. 
             *enum {
             *    MEMBLOCK_NONE        = 0x0,     No special request 
             *    MEMBLOCK_HOTPLUG    = 0x1,      hotpluggable region
             *    MEMBLOCK_MIRROR        = 0x2,   mirrored region 
             *    MEMBLOCK_NOMAP        = 0x4,    don't add to kernel direct mapping
             *};
             */ 
            unsigned long flags;//region的标志，上面枚举体定义。
        #ifdef CONFIG_HAVE_MEMBLOCK_NODE_MAP
            int nid;
        #endif
        };
        struct memblock_region *regions;//指向regions数组。
    };
     struct memblock_type memory;  //memory即动态内存的集合
     struct memblock_type reserved;//reserved集合包括静态内存和预留内存
#ifdef CONFIG_HAVE_MEMBLOCK_PHYS_MAP
    struct memblock_type physmem;
#endif
};


```


A memblock基本操作
1) 添加内存区
```c++
// memblock_add用于添加region到memblock.memory中
int memblock_add (phys_addr_t base , phys_addr_t size );
//memblock_reserve将region添加到memblock.reserved中
int memblock_reserve (phys_addr_t base , phys_addr_t size );
```
1) 移除内存区
```c++
//memblock_remove将一个region从memblock.memory中移除
int memblock_remove (phys_addr_t base, phys_addr_t size);
//memblock_free等用于将一个region从memblock.reserved中移除
```

1) 分配内存
phys_addr_t memblock_alloc (phys_addr_tsize, phys_addr_t align);
phys_addr_t memblock_alloc_range(phys_addr_tsize, phys_addr_t align,   phys_addr_tstart, phys_addr_t end);

使用该函数向kernel申请一块可用的物理内存，memblock使用自顶向下（取决于bottom_up的值）的方式查找空闲内存，实际操作是在memory region中查找合适的内存，并加入到reserved region中以标记这块内存已经被使用；


## 2.3.物理内存映射

[arm32-page-tables](https://people.kernel.org/linusw/arm32-page-tables)


PGD中包含若干PUD的地址，PUD中包含若干PMD的地址，PMD中又包含若干PT的地址。每一个页表项指向一个页框，页框就是真正的物理内存页。

32bit的Linux采用三级映射：PGD-->PMD-->PTE，

64bit的Linux采用四级映射：PGD-->PUD-->PMD-->PTE，多了个PUD。

在ARM32 Linux采用两层映射, 所以PGD->PMD->PTE中间的PMD被省略的.,除非在定义了CONFIG_ARM_LPAE才会使用3级映射。参见kconfig中的配置：
```
config PGTABLE_LEVELS
	int
	default 3 if ARM_LPAE
	default 2
```


在内核代码中：
- 当CONFIG_PGTABLE_LEVELS=4时：pgd-->pud-->pmd-->pte;
- 当CONFIG_PGTABLE_LEVELS=3时，没有PUD页表：pgd(pud)-->pmd-->pte;
- 当CONFIG_PGTABLE_LEVELS=2时，没有PUD和PMD页表：pgd(pud, pmd)-->pte

LINUX启动建立临时映射表的过程见heard.S中调用的__create_page_tables；linux是在paging_init中建立正常的页表映射，并在其中删除前面head.s建立的临时页表映射。

涉及映射代码见下面的注释部分
```c++
/*
 * paging_init() sets up the page tables, initialises the zone memory
 * maps, and sets up the zero page, bad page and bad page tables.
 */
void __init paging_init(const struct machine_desc *mdesc)
{
	void *zero_page;

    //prepare_page_table用于清空页表项，其实清空了三段地址页表项，0~MODULES_VADDR、MODULES_VADDR~PAGE_OFFSET、end-of-the-first-block-of-lowmem_limit~VMALLOC_START。
	prepare_page_table();
	/**
	 * 内存映射，分别创建kernel区间（MT_MEMORY_RWX）、normal区间（MT_MEMORY_RW）
	 * 主要过程是遍历每个memblock region：
	 *  A. 如果 该 memblock region 完全位于 kernel 代码 VA区间 下方, 则RWX映射
	 *  B. 如果 该 memblock region 完全位于 kernel 代码 VA区间 上方, 则RW映射
	 *  C. 到这里说明存在交集，此时
	 *      c.1 如果“存在[start,kernel_x_start]区间,映射采用RW属性”，
	 *      c.2 如果"存在[kernel_x_end ,end]区间,映射采用RW属性"
	 *      C.3 映射"[kernel_x_start,kernel_x_end]"，采用RWX属性
	 *     
	*/
	map_lowmem();
	memblock_set_current_limit(arm_lowmem_limit);
	dma_contiguous_remap();
	early_fixmap_shutdown();
    //还有一部分内存映射在devicemaps_init中进行，对vectors进行映射
	devicemaps_init(mdesc);
	kmap_init();
	tcm_init();

	top_pmd = pmd_off_k(0xffff0000);

	/* allocate the zero page. */
	zero_page = early_alloc(PAGE_SIZE);

    /**
	 * 里面东西挺多，参考[Linux内存模型之Sparse Memory Model](https://www.cnblogs.com/LoyenWang/p/11523678.html)
	 * 与[Linux内存管理zone_sizes_init](https://www.cnblogs.com/LoyenWang/p/11568481.html)
	*/
	bootmem_init();

	empty_zero_page = virt_to_page(zero_page);
	__flush_dcache_page(NULL, empty_zero_page);

	/* Compute the virt/idmap offset, mostly for the sake of KVM */
	kimage_voffset = (unsigned long)&kimage_voffset - virt_to_idmap(&kimage_voffset);
}
```

### A. io 静态映射与对应内核页表的建立

常用的ioremap或者of_iomap都是动态映射，静态映射的接口是iotable_init

iotable_init 一般是在machine desc 的map_io的call函数里具体的call stack如下:
"`start_kernel-->setup_arch-->paging_init-->devicemaps_init-->[mdesc->map_io()]`"

iotable_init会调用函数create_mapping来创建内核页表和映射关系



## 2.4.zone初始化
bootmem_init-->zone_sizes_init-->free_area_init_node-->free_area_init_core

内存管理将一个内存Node分成若干个zone进行管理，定义zone类型在enum zone_type中。

zone的初始化在"`paging_init-->bootmem_init`"中进行。

Vexpress之定义了NORMAL和HIGHMEM两种，通过find_limits找出物理内存开始帧号min_low_pfn、结束帧号max_pfn、NORMAL区域的结束帧号max_low_pfn。

## 2.5. 物理内存初始化
物理内存页面需要加入到伙伴系统中，伙伴系统是一种动态存储管理方法。在用户提出申请时，分配一块大小合适的内存块，反之在释放时回收内存块。

伙伴系统分配对空闲页面的管理是根据两个属性：页的大小，2的order次幂个页；和页的迁移类型。

# 内存管理

注： global init_mm 是内核内存管理的上下文

如前面介绍，内存管理可以分为两个大的阶段：
- 先是memblock管理所有内存（系统预留内存，动态内存）
- 然后是伙伴系统管理动态内存。

也就是说：Linux内核使用伙伴系统管理内存，那么在伙伴系统工作前，即memblock在系统启动阶段进行简单的内存管理，记录物理内存的使用情况； 这是一般用法，也可能在驱动中将reserver mem、dynamic mem进行调整。

![32位寻址时代内存管理概图](https://img-blog.csdn.net/20170327205238020)

## linux内存管理之page

```c++

struct address_space;
struct mem_cgroup;
struct hmm;

/*
 * Each physical page in the system has a struct page associated with
 * it to keep track of whatever it is we are using the page for at the
 * moment. Note that we have no way to track which tasks are using
 * a page, though if it is a pagecache page, rmap structures can tell us
 * who is mapping it.
 *
 * The objects in struct page are organized in double word blocks in
 * order to allows us to use atomic double word operations on portions
 * of struct page. That is currently only used by slub but the arrangement
 * allows the use of atomic double word operations on the flags/mapping
 * and lru list pointers also.
 */
struct page {
	/* First double word block */
    /* 用于页描述符，一组标志(如PG_locked、PG_error)，同时页框所在的管理区和node的编号也保存在当中 */
    /* 在lru算法中主要用到的标志
     * PG_active: 表示此页当前是否活跃，当放到或者准备放到活动lru链表时，被置位
     * PG_referenced: 表示此页最近是否被访问，每次页面访问都会被置位
     * PG_lru: 表示此页是处于lru链表中的
     * PG_mlocked: 表示此页被mlock()锁在内存中，禁止换出和释放
     * PG_swapbacked: 表示此页依靠swap，可能是进程的匿名页(堆、栈、数据段)，匿名mmap共享内存映射，shmem共享内存映射
     */
	unsigned long flags;		/* Atomic flags, some possibly
					 * updated asynchronously */
	union {
		/**
		 *当文件或设备被内存映射时，它们的 inode 有一个关联的address_space. 如果页面属于
		 *该文件，则该字段将指向该地址空间。如果页面是匿名的并且设置了映射，
		 *则address_space是 管理交换地址空间的swapper_space；
		*/
		struct address_space *mapping;	/* If low bit clear, points to
						 * inode address_space, or NULL.
						 * If page mapped as anonymous
						 * memory, low bit is set, and
						 * it points to anon_vma object:
						 * see PAGE_MAPPING_ANON below.
						 */
		void *s_mem;			/* slab first object */
		atomic_t compound_mapcount;	/* first tail page */
		/* page_deferred_list().next	 -- second tail page */
	};

	/* Second double word */
	union {
		pgoff_t index;		/* Our offset within mapping. */
		void *freelist;		/* sl[aou]b first free object */
		/* page_deferred_list().prev	-- second tail page */
	};

	union {
#if defined(CONFIG_HAVE_CMPXCHG_DOUBLE) && \
	defined(CONFIG_HAVE_ALIGNED_STRUCT_PAGE)
		/* Used for cmpxchg_double in slub */
		unsigned long counters;
#else
		/*
		 * Keep _refcount separate from slub cmpxchg_double data.
		 * As the rest of the double word is protected by slab_lock
		 * but _refcount is not.
		 */
		unsigned counters;
#endif
		struct {

			union {
				/*
				 * Count of ptes mapped in mms, to show when
				 * page is mapped & limit reverse map searches.
				 *
				 * Extra information about page type may be
				 * stored here for pages that are never mapped,
				 * in which case the value MUST BE <= -2.
				 * See page-flags.h for more details.
				 */
				atomic_t _mapcount;

				unsigned int active;		/* SLAB */
				struct {			/* SLUB */
					unsigned inuse:16;
					unsigned objects:15;
					unsigned frozen:1;
				};
				int units;			/* SLOB */
			};
			/*
			 * Usage count, *USE WRAPPER FUNCTION* when manual
			 * accounting. See page_ref.h
			 */
			atomic_t _refcount;
		};
	};

	/*
	 * Third double word block
	 *
	 * WARNING: bit 0 of the first word encode PageTail(). That means
	 * the rest users of the storage space MUST NOT use the bit to
	 * avoid collision and false-positive PageTail().
	 */
        /* 页处于不同情况时，加入的链表不同
         * 1.是一个进程正在使用的页，加入到对应lru链表和lru缓存中
         * 2.如果为空闲页框，并且是空闲块的第一个页，加入到伙伴系统的空闲块链表中(只有空闲块的第一个页需要加入)
         * 3.如果是一个slab的第一个页，则将其加入到slab链表中(比如slab的满slab链表，slub的部分空slab链表)
         * 4.将页隔离时用于加入隔离链表
         */	
	union {
		struct list_head lru;	/* Pageout list, eg. active_list
					 * protected by zone_lru_lock !
					 * Can be used as a generic list
					 * by the page owner.
					 */
		struct dev_pagemap *pgmap; /* ZONE_DEVICE pages are never on an
					    * lru or handled by a slab
					    * allocator, this points to the
					    * hosting device page map.
					    */
		struct {		/* slub per cpu partial pages */
			struct page *next;	/* Next partial slab */
#ifdef CONFIG_64BIT
			int pages;	/* Nr of partial slabs left */
			int pobjects;	/* Approximate # of objects */
#else
			short int pages;
			short int pobjects;
#endif
		};

		struct rcu_head rcu_head;	/* Used by SLAB
						 * when destroying via RCU
						 */
		/* Tail pages of compound page */
		struct {
			unsigned long compound_head; /* If bit zero is set */

			/* First tail page only */
#ifdef CONFIG_64BIT
			/*
			 * On 64 bit system we have enough space in struct page
			 * to encode compound_dtor and compound_order with
			 * unsigned int. It can help compiler generate better or
			 * smaller code on some archtectures.
			 */
			unsigned int compound_dtor;
			unsigned int compound_order;
#else
			unsigned short int compound_dtor;
			unsigned short int compound_order;
#endif
		};

#if defined(CONFIG_TRANSPARENT_HUGEPAGE) && USE_SPLIT_PMD_PTLOCKS
		struct {
			unsigned long __pad;	/* do not overlay pmd_huge_pte
						 * with compound_head to avoid
						 * possible bit 0 collision.
						 */
			pgtable_t pmd_huge_pte; /* protected by page->ptl */
		};
#endif
	};

	/* Remainder is not double word aligned */
	union {
		unsigned long private;		/* Mapping-private opaque data:
					 	 * usually used for buffer_heads
						 * if PagePrivate set; used for
						 * swp_entry_t if PageSwapCache;
						 * indicates order in the buddy
						 * system if PG_buddy is set.
						 */
#if USE_SPLIT_PTE_PTLOCKS
#if ALLOC_SPLIT_PTLOCKS
		spinlock_t *ptl;
#else
		spinlock_t ptl;
#endif
#endif
		struct kmem_cache *slab_cache;	/* SL[AU]B: Pointer to slab */
	};

#ifdef CONFIG_MEMCG
	struct mem_cgroup *mem_cgroup;
#endif

	/*
	 * On machines where all RAM is mapped into kernel address space,
	 * we can simply calculate the virtual address. On machines with
	 * highmem some memory is mapped into kernel virtual memory
	 * dynamically, so we need a place to store that address.
	 * Note that this field could be 16 bits on x86 ... ;)
	 *
	 * Architectures with slow multiplication can define
	 * WANT_PAGE_VIRTUAL in asm/page.h
	 */
#if defined(WANT_PAGE_VIRTUAL)
	void *virtual;			/* Kernel virtual address (NULL if
					   not kmapped, ie. highmem) */
#endif /* WANT_PAGE_VIRTUAL */

#ifdef LAST_CPUPID_NOT_IN_PAGE_FLAGS
	int _last_cpupid;
#endif
}
```

## linux内存管理之node

CPU被划分为多个节点(node), 内存则被分簇, 每个CPU对应一个本地物理内存, 即一个CPU-node对应一个内存簇bank，即每个内存簇被认为是一个节点

系统的物理内存被划分为几个节点(node), 一个node对应一个内存簇bank，即每个内存簇被认为是一个节点

首先, 内存被划分为结点. 每个节点关联到系统中的一个处理器, 内核中表示为`pg_data_t`的实例. 系统中每个节点被链接到一个以NULL结尾的pgdat_list链表中<而其中的每个节点利用pg_data_tnode_next字段链接到下一节．而对于PC这种UMA结构的机器来说, 只使用了一个全局变量"`contig_page_data`".

node初始化： zone_sizes_init()在开始阶段主要负责对各个类型zone 大小进行计算，并根据内核启动参数movable_node和kernelcore，以及实际物理内存情况，计算出各个node节点所划分出的可以移动的内存给ZONE_MOVABLE，并将各个节点ZONE_MOVABLE的内存保存到zone_movable_pfn数组中，最后所有准备工作完成之后，free_area_init()在最后会具体初始各个节点信息。
"`setup_arch-->paging_init-->bootmem_init-->zone_sizes_init-->free_area_init_node`"


```c++
typedef struct pglist_data pg_data_t;

//linux\mm\bootmem.c
#ifndef CONFIG_NEED_MULTIPLE_NODES
struct pglist_data __refdata contig_page_data = {
	.bdata = &bootmem_node_data[0]
};
EXPORT_SYMBOL(contig_page_data);
#endif
#endif
```


```c++

/*
 * On NUMA machines, each NUMA node would have a pg_data_t to describe
 * it's memory layout. On UMA machines there is a single pglist_data which
 * describes the whole memory.
 *
 * Memory statistics and page replacement data structures are maintained on a
 * per-zone basis.
 */
struct bootmem_data;
typedef struct pglist_data {
    //node_zones: node拥有的若干zones
	struct zone node_zones[MAX_NR_ZONES];

	/**
	 * node_zonelists: 当我们要在某一个指定的zone中分配内存，而该zone中又没有足够多的空闲内
	 * 存时，怎么办？正所谓狡兔三窟，我们得给自己留点后路，node_zonelists正是这种情况下的后路。
	 * 它指定了所有的备选的zones。当然这些备选zones也是有优先顺序的。
	 * build_zonelists() in mm/page_alloc.c sets up the order when called 
	 * by free_area_init_core().
	*/
	struct zonelist node_zonelists[MAX_ZONELISTS];
    //nr_zones: 顾名思义，就是该node中zone的个数。
	int nr_zones;
#ifdef CONFIG_FLAT_NODE_MEM_MAP	/* means !SPARSEMEM */
	/**
	 * This is the first page of the struct page array representing each physical frame in the node. It will be placed somewhere within the global mem_map array;
	*/
	struct page *node_mem_map;
#ifdef CONFIG_PAGE_EXTENSION
	struct page_ext *node_page_ext;
#endif
#endif
#ifndef CONFIG_NO_BOOTMEM
	struct bootmem_data *bdata;
#endif
#ifdef CONFIG_MEMORY_HOTPLUG
	/*
	 * Must be held any time you expect node_start_pfn, node_present_pages
	 * or node_spanned_pages stay constant.  Holding this will also
	 * guarantee that any pfn_valid() stays that way.
	 *
	 * pgdat_resize_lock() and pgdat_resize_unlock() are provided to
	 * manipulate node_size_lock without checking for CONFIG_MEMORY_HOTPLUG.
	 *
	 * Nests above zone->lock and zone->span_seqlock
	 */
	spinlock_t node_size_lock;
#endif
    //node_start_pfn: 注意所有node中的page frame是统一顺序编号的，并不是说这个师里有个士兵9527，那个师里还有个士兵9527。9527在整个部队中只有一个。而node_start_pfn就是该node中第一个page frame的编号。
	/**
	 * The starting physical address of the node. An unsigned
	 * long does not work optimally as it breaks for ia32 with Physical Address
	 * Extension (PAE) for example. PAE is discussed further in Section 2.5. A
	 * more suitable solution would be to record this as a Page Frame Number
	 * (PFN). A PFN is simply in index within physical memory that is counted
	 * in page-sized units. PFN for a physical address could be trivially defined as
	 * (page_phys_addr >> PAGE_SHIFT);
	*/
	unsigned long node_start_pfn;
	/**
	 * node中存在的物理页总数
	*/
	unsigned long node_present_pages; /* total number of physical pages */

	/**
	 * node_spanned_pages：这个也是该node中page frames的个数，不过
	 * 把memory holes也包含进来了，所以node_spanned_pages一般要比node_present_pages大。
	*/
	unsigned long node_spanned_pages; /* total size of physical page
					     range, including holes */
	//node_id： 该node的编号，0 1 2 ...
    int node_id;
    //kswapd_wait, kswapd, kswapd_max_order: 这三个成员变量用于swapping 机制
	wait_queue_head_t kswapd_wait;
	wait_queue_head_t pfmemalloc_wait;
	struct task_struct *kswapd;	/* Protected by
					   mem_hotplug_begin/end() */
	int kswapd_order;
	enum zone_type kswapd_classzone_idx;

	int kswapd_failures;		/* Number of 'reclaimed == 0' runs */

#ifdef CONFIG_COMPACTION
	int kcompactd_max_order;
	enum zone_type kcompactd_classzone_idx;
	wait_queue_head_t kcompactd_wait;
	struct task_struct *kcompactd;
#endif
#ifdef CONFIG_NUMA_BALANCING
	/* Lock serializing the migrate rate limiting window */
	spinlock_t numabalancing_migrate_lock;

	/* Rate limiting time interval */
	unsigned long numabalancing_migrate_next_window;

	/* Number of pages migrated during the rate limiting time interval */
	unsigned long numabalancing_migrate_nr_pages;
#endif
	/*
	 * This is a per-node reserve of pages that are not available
	 * to userspace allocations.
	 */
	unsigned long		totalreserve_pages;

#ifdef CONFIG_NUMA
	/*
	 * zone reclaim becomes active if more unmapped pages exist.
	 */
	unsigned long		min_unmapped_pages;
	unsigned long		min_slab_pages;
#endif /* CONFIG_NUMA */

	/* Write-intensive fields used by page reclaim */
	ZONE_PADDING(_pad1_)
    /* lru链表使用的自旋锁 
     * 当需要修改lru链表描述符中任何一个链表时，都需要持有此锁，也就是说，不会有两个不同的lru链表同时进行修改
     */
	spinlock_t		lru_lock;

#ifdef CONFIG_DEFERRED_STRUCT_PAGE_INIT
	/*
	 * If memory initialisation on large machines is deferred then this
	 * is the first PFN that needs to be initialised.
	 */
	unsigned long first_deferred_pfn;
	/* Number of non-deferred pages */
	unsigned long static_init_pgcnt;
#endif /* CONFIG_DEFERRED_STRUCT_PAGE_INIT */

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
	spinlock_t split_queue_lock;
	struct list_head split_queue;
	unsigned long split_queue_len;
#endif

	/* Fields commonly accessed by the page reclaim scanner */
	struct lruvec		lruvec;

	/*
	 * The target ratio of ACTIVE_ANON to INACTIVE_ANON pages on
	 * this node's LRU.  Maintained by the pageout code.
	 */
	unsigned int inactive_ratio;

	unsigned long		flags;

	ZONE_PADDING(_pad2_)

	/* Per-node vmstats */
	struct per_cpu_nodestat __percpu *per_cpu_nodestats;
	atomic_long_t		vm_stat[NR_VM_NODE_STAT_ITEMS];
} pg_data_t;

```

## linux内存管理之zone

kernel将pages分到了不同的zone中。kernel会判断pages的属性，将具有相似属性的pages分到同一个zone。linux不能将所有的pages同等看待。

典型 x86 Linux分为了三个zone：

- ZONE_DMA（0-16 MB）: 包含 ISA/PCI 设备需要的低端物理内存区域中的内存范围，这块区域由标准工业架构(Industry Standard Architecture)设备使用，适合DMA内存。这部分区域大小和CPU架构有关，在x86架构中，该部分区域大小限制为16MB。。
- ZONE_NORMAL（16-896 MB）：由内核直接映射到高端范围的物理内存的内存范围。所有的内核操作都只能使用这个内存区域来进行，因此这是对性能至关重要的区域。
- ZONE_HIGHMEM（896 MB 以及更高的内存）：系统中内核不能映射到的其他可用内存。


```c++

//linux\include\linux\mmzone.h
struct zone {
	/**
     * zone watermarks, access with *_wmark_pages(zone) macros
	 * Zone Watermarks， 每个zone都有自己独立的水线值
	 * 	WMARK_MIN,WMARK_LOW,WMARK_HIGH
	*/
	unsigned long watermark[NR_WMARK];

	unsigned long nr_reserved_highatomic;

	/*
	 * We don't know if the memory that we're going to allocate will be
	 * freeable or/and it will be released eventually, so to avoid totally
	 * wasting several GB of ram we must reserve some of the lower zone
	 * memory (otherwise we risk to run OOM on the lower zones despite
	 * there being tons of freeable ram on the higher zones).  This array is
	 * recalculated at runtime if the sysctl_lowmem_reserve_ratio sysctl
	 * changes.
	 */
	long lowmem_reserve[MAX_NR_ZONES];//页分配器使用，保留多少页不能借给高的区域类型

#ifdef CONFIG_NUMA
	int node;
#endif
	struct pglist_data	*zone_pgdat;//Points to the parent pg_data_t;
	struct per_cpu_pageset __percpu *pageset;

#ifndef CONFIG_SPARSEMEM
	/*
	 * Flags for a pageblock_nr_pages block. See pageblock-flags.h.
	 * In SPARSEMEM, this map is stored in struct mem_section
	 */
	unsigned long		*pageblock_flags;
#endif /* CONFIG_SPARSEMEM */

	/* zone_start_pfn == zone_start_paddr >> PAGE_SHIFT */
    //当前区域的起始物理页号
	unsigned long		zone_start_pfn;

	/*
	 * spanned_pages is the total pages spanned by the zone, including
	 * holes, which is calculated as:
	 * 	spanned_pages = zone_end_pfn - zone_start_pfn;
	 *
	 * present_pages is physical pages existing within the zone, which
	 * is calculated as:
	 *	present_pages = spanned_pages - absent_pages(pages in holes);
	 *
	 * managed_pages is present pages managed by the buddy system, which
	 * is calculated as (reserved_pages includes pages allocated by the
	 * bootmem allocator):
	 *	managed_pages = present_pages - reserved_pages;
	 *
	 * So present_pages may be used by memory hotplug or memory power
	 * management logic to figure out unmanaged pages by checking
	 * (present_pages - managed_pages). And managed_pages should be used
	 * by page allocator and vm scanner to calculate all kinds of watermarks
	 * and thresholds.
	 *
	 * Locking rules:
	 *
	 * zone_start_pfn and spanned_pages are protected by span_seqlock.
	 * It is a seqlock because it has to be read outside of zone->lock,
	 * and it is done in the main allocator path.  But, it is written
	 * quite infrequently.
	 *
	 * The span_seq lock is declared along with zone->lock because it is
	 * frequently read in proximity to zone->lock.  It's good to
	 * give them a chance of being in the same cacheline.
	 *
	 * Write access to present_pages at runtime should be protected by
	 * mem_hotplug_begin/end(). Any reader who can't tolerant drift of
	 * present_pages should get_online_mems() to get a stable value.
	 *
	 * Read access to managed_pages should be safe because it's unsigned
	 * long. Write access to zone->managed_pages and totalram_pages are
	 * protected by managed_page_count_lock at runtime. Idealy only
	 * adjust_managed_page_count() should be used instead of directly
	 * touching zone->managed_pages and totalram_pages.
	 */
	unsigned long		managed_pages;//伙伴分配器管理的物理页的数
	unsigned long		spanned_pages;
	unsigned long		present_pages;

	const char		*name;  //zone name

#ifdef CONFIG_MEMORY_ISOLATION
	/*
	 * Number of isolated pageblock. It is used to solve incorrect
	 * freepage counting problem due to racy retrieving migratetype
	 * of pageblock. Protected by zone->lock.
	 */
	unsigned long		nr_isolate_pageblock;
#endif

#ifdef CONFIG_MEMORY_HOTPLUG
	/* see spanned/present_pages for more description */
	seqlock_t		span_seqlock;
#endif

	int initialized;

	/* Write-intensive fields used from the page allocator */
	ZONE_PADDING(_pad1_)

	/**
	 * Free area bitmaps used by the buddy allocator;
	 * 由伙伴系统使用，比如free_area[2]中nr_free的值为5，表示有5个大小为4的页框块，那么总的空闲页为20.
	 */ 
	/* free areas of different sizes */
	struct free_area	free_area[MAX_ORDER];

	/* zone flags, see below */
	unsigned long		flags;

	/* Primarily protects free_area */
	spinlock_t		lock;

	/* Write-intensive fields used by compaction and vmstats. */
	ZONE_PADDING(_pad2_)

	/*
	 * When free pages are below this point, additional steps are taken
	 * when reading the number of free pages to avoid per-cpu counter
	 * drift allowing watermarks to be breached
	 */
	unsigned long percpu_drift_mark;

#if defined CONFIG_COMPACTION || defined CONFIG_CMA
	/* pfn where compaction free scanner should start */
	unsigned long		compact_cached_free_pfn;
	/* pfn where async and sync compaction migration scanner should start */
	unsigned long		compact_cached_migrate_pfn[2];
#endif

#ifdef CONFIG_COMPACTION
	/*
	 * On compaction failure, 1<<compact_defer_shift compactions
	 * are skipped before trying again. The number attempted since
	 * last failure is tracked with compact_considered.
	 */
	unsigned int		compact_considered;
	unsigned int		compact_defer_shift;
	int			compact_order_failed;
#endif

#if defined CONFIG_COMPACTION || defined CONFIG_CMA
	/* Set to true when the PG_migrate_skip bits should be cleared */
	bool			compact_blockskip_flush;
#endif

	bool			contiguous;

	ZONE_PADDING(_pad3_)
	/* Zone statistics */
	atomic_long_t		vm_stat[NR_VM_ZONE_STAT_ITEMS];
	atomic_long_t		vm_numa_stat[NR_VM_NUMA_STAT_ITEMS];
} ____cacheline_internodealigned_in_smp;```
```

### Zone Watermarks
在进行内存分配的时候，如果分配器（比如buddy allocator）发现当前空余内存的值低于"low"但高于"min"，说明现在内存面临一定的压力，那么在此次内存分配完成后，kswapd将被唤醒，以执行内存回收操作。在这种情况下，内存分配虽然会触发内存回收，但不存在被内存回收所阻塞的问题，两者的执行关系是异步的。

这里所说的"空余内存"其实是一个zone总的空余内存减去其lowmem_reserve的值。对于kswapd来说，要回收多少内存才算完成任务呢？只要把空余内存的大小恢复到"high"对应的watermark值就可以了，当然，这取决于当前空余内存和"high"值之间的差距，差距越大，需要回收的内存也就越多。"low"可以被认为是一个警戒水位线，而"high"则是一个安全的水位线。

![Zone Watermarks](https://www.kernel.org/doc/gorman/html/understand/understand-html002.png)

[Linux内存调节之zone watermark](https://www.cnblogs.com/sky-heaven/p/13747568.html)

### 为什么64位系统不需要高端内存

为什么32位系统有HIGHMEM_ZONE而64位没有HIGHMEM_ZONE？

首先需要知道高端内存的最基本思想：借一段地址空间，建立临时地址映射，用完后释放，达到这段地址空间可以循环使用，访问所有物理内存。也就是说，内核可用的地址空间在现实中物理内存足够大导致可能不足以支撑所有内存访问，所以内核引入了High Memory 的概念。

其次需要知道，对于64位，普遍遵循AMD的Canonical Form, 即只有虚拟地址的最低48位才会在地址转换时被使用, 且任何虚拟地址的48位至63位必须与47位一致,即在该情况下总的虚拟地址空间为256TB，用户空间与内核空间各使用128TB。这虽然没有完全使用64位（范围达到1024*1024TB）大，但也已经足够大。

在32位时代，以典型的“VMSPLIT_3G”配置为例，内核使用可以使用VA范围是"`[0xC000 0000,0xFFFF FFFF]`"大小是1G。假如内核有场景需要使用一个大内存，比如2G。内核的这个VA范围是支撑不了的，因为就拿内核的所有VA资源都无法映射覆盖住。 这就是引入“高端内存临时映射”目的，以及HEIGHMEM概念存在的原因。

而在64位时代，前面介绍背景我们已经知道，内核可以使用的VA范围已经明显扩大，比如128T。从现实情况来说，我们可以有理由认为当前不存在有使用这么大物理内存的情况与需求。也就是可以认为内核的VA范围已经可以支持它需要访问的物理内存大小。

这就是在linux 64位系统不存在将HIGHMEM区分出来，而只分为ZONE_DMA/ZONE_NORMAL的原因。


## PAGE管理：

page信息存放在全局数组"`struct page *mem_map;`",存放的内容是页描述符。对这个数组的序号称为页帧号(pfn)。注意这个page结构描述的是某片物理页，而不是它包含的数据。

每个物理页对应一个page结构体，称为页描述符，内存节点的pglist_data实例成员node_mem_map指向该内存节点包含的所有物理页的页描述符组成的数组。
结构体page的成员flags的布局如下：
| [SECTION] | [NODE] | ZONE | [LAST_CPUID] |...| FLAGS |
其中，SECTION是稀疏内存模型中的段编号，NODE是节点编号，ZONE是区域类型，FLAGS是标志位。


### page结构的相关宏/函数：
```c++
/**
 * 作用是struct page* 和 前面提到的页帧号(pfn)之间的转换. 就是简单
 * 地和 mem_map 进行加减操作
*/
#define page_to_pfn __page_to_pfn
#define pfn_to_page __pfn_to_page

/**
 * 分别是将struct page*和物理地址之间进行转换
 * 例如page_to_phys, 通过page_to_pfn宏取得相应的pfn后，假设PAGE_SHIFT是12，假设pfn是1，左移12位，
 * 就是4096，也就是第二个对应的物理页的位置，这样就取得了物理地址（虽然内核在虚拟地址中是在高地址的，
 * 但是在物理地址中是从0开始的，所以这里也是从0开始） 
*/
#define phys_to_page(phys) (pfn_to_page(phys >> PAGE_SHIFT))
#define page_to_phys(page) (page_to_pfn(page) << PAGE_SHIFT)

/**
 * 这两个宏的作用是在struct page*和内核逻辑/线性地址 之间做转换
*/
#define virt_to_page(kaddr) pfn_to_page(__pa(kaddr) >> PAGE_SHIFT)
#define page_to_virt(page)  __va(page_to_pfn(page) << PAGE_SHIFT)

/**
 * return page's virtual address
 * 对有高端内存的(highMem)，则通过高端内存进行映射得到； 否则如果不是，直接调用
 * lowmem_page_address(这个函数内部实现就是page_to_virt)
*/
void *page_address(struct page *page)
```

在内核中分配内存，最后要通过伙伴系统接口进行实际物理页面的分配，一个重要的接口便是alloc_page/alloc_pages,它们的返回值是"`struct page *`".
```c++
/**
 * gfp_mask是一组决定分配器行为方式的标志,例如GFP_KERNEL,GFP_USER
*/
#define alloc_pages(gfp_mask, order) \
		alloc_pages_node(numa_node_id(), gfp_mask, order)// 分配 1<<order 个连续的物理页
#define alloc_page(gfp_mask) alloc_pages(gfp_mask, 0)// 分配一个物理页
```        


page 管理接口：

返回page结构的：
struct page * alloc_pages(gfp_mask, order)          
struct page * alloc_page(gfp_mask)                     
 
返回page对应的逻辑地址的：
__get_free_pages(gfp_mask, order)                    // 和alloc_pages一样，只不过返回的是第一个页的内核逻辑地址
__get_free_page(gfp_mask)                              // 返回一个页的逻辑地址

分配和释放都牵涉到底层的伙伴算法




# 分配器：伙伴系统

Buddy System伙伴系统，使用的主要算法是Binary Buddy Allocator. 用于分配/管理物理页的系统，支持连续的物理页面分配和释放。此外，使用与碎片相关的算法来确保最大的连续页面。

原理：空闲的物理页框按大小分组成0~MAX_ORDER个链表，每个链表存放页框的大小为2的n次幂，其中n在0 ~ MAX_ORDER-1中取值。

它背后的基本概念非常简单。内存被分成大块的页面，其中每个块都是页面数的幂。如果没有所需大小的块，则将一个大块分成两半，这两个块是彼此的伙伴，也称伙伴对。一半用于分配，另一半保持空闲态。这些块根据需要连续减半，直到有所需大小的块可用。当一个块后来被释放时，会检查伙伴，如果它是空闲的，则两者合并。

分配器维护空闲页块，其中每个块是页数的幂。两个大小块的幂的指数称为阶数. 一个free_area结构数组为每个指向一个空闲页面块链表的顺序维护，如下图所示：

![空闲页块维护](https://www.kernel.org/doc/gorman/html/understand/understand-html029.png)

该结构数组对应数据结构是：
```c++
struct zone {
	...
	/* free areas of different sizes */
	struct free_area	free_area[MAX_ORDER]; //MAX_ORDER定义为11
	...
};
```

请求分配例子,假设请求分配2^8 = 256个页框块：
- 检查n = 8的链表，检查是否有空闲块，找到了则直接返回；
- 没有找到满足需求的，则查找n = 9的链表，找到512大小空闲块，拆分成两个256大小块，将其中一个256大小块返回，另一个256大小块添加到n = 8的链表中；
- 在n = 9的链表中没有找到合适的块，则查找n = 10的链表，找到1024大小空闲块，将其拆分成512 + 256 + 256大小的块，返回需要获取的256大小的块，将剩下的512大小块插入n = 9链表中，剩下的256大小块插入n = 8的链表中；

合并过程是上述流程的逆过程，试图将大小相等的Buddy块进行合并成单独的块，并且会迭代合并下去，尝试合并成更大的块。合并需要满足要求：
- 两个Buddy块大小一致；
- 们的物理地址连续；
- 第一个Buddy块的起始地址为 (2 x N x 4K)的整数倍，其中4K为页面大小，N为Buddy块的大小；

struct page结构中，与Buddy System相关的字段有：
- _mapcount： 用于标记page是否处在Buddy System中，设置成-1或PAGE_BUDDY_MAPCOUNT_VALUE(-128)；
- private： 一个2^k次幂的空闲块的第一个页描述符中，private字段存放了块的order值，也就是k值；
- index： 存放MIGRATE类型；
- _refcount： 用户使用计数值，没有用户使用为0，有使用的话则增加；

## 关键数据结构

```c++
// 代表xx阶所对应页框链表
struct free_area {
	// 表示的就是当前分配阶所对应的页框块的链表(多个)。MIGRATE_TYPES表示的是迁移页的类型，见enum migratetype定义
	// 其中每一个链表中的内存页面按照其自身是否可以释放或者迁移被归为一类，于是凡是请求“不可迁移”页面的分配请求全部在free_list[MIGRATE_UNMOVABLE]这条链表上分配
	struct list_head	free_list[MIGRATE_TYPES];
	//指的是当前链表中空闲页框块的数目，
	unsigned long		nr_free;
};


struct zone {
	...
	//每处理器页集合, per-cpu page frame cache
	struct per_cpu_pageset __percpu *pageset;
	...
	/* free areas of different sizes */
	//比如free_area[2]中nr_free的值为5，表示有5个大小为4的页框块，那么总的空闲页为20.
	struct free_area	free_area[MAX_ORDER]; //MAX_ORDER定义为11
	...
};

//全局的二维fallbacks的数组,支撑后从备迁移类型中进行分配
/*
 * This array describes the order lists are fallen back to when
 * the free lists for the desirable migrate type are depleted
 */
static int fallbacks[MIGRATE_TYPES][4] = {
	[MIGRATE_UNMOVABLE]   = { MIGRATE_RECLAIMABLE, MIGRATE_MOVABLE,   MIGRATE_TYPES },
	[MIGRATE_RECLAIMABLE] = { MIGRATE_UNMOVABLE,   MIGRATE_MOVABLE,   MIGRATE_TYPES },
	[MIGRATE_MOVABLE]     = { MIGRATE_RECLAIMABLE, MIGRATE_UNMOVABLE, MIGRATE_TYPES },
#ifdef CONFIG_CMA
	[MIGRATE_CMA]         = { MIGRATE_TYPES }, /* Never used */
#endif
#ifdef CONFIG_MEMORY_ISOLATION
	[MIGRATE_ISOLATE]     = { MIGRATE_TYPES }, /* Never used */
#endif
};
```
MIGRATE_TYPE限制了内存页面的分配地点从而避免碎片，而不再仅仅寄希望于它们被释放时通过合并避免碎片。详细说明参见 [linux内核对伙伴系统的改进--migrate_type](https://blog.csdn.net/dog250/article/details/6108028)。另外，类似的机制用于避免碎片那就是使用ZONE的概念，新构造出一个虚拟的ZONE--ZONE_MOVABLE，所谓的虚拟就是它并不和任何物理内存区间相关联，而是可以附着在任何的物理zone上，用户可以通过命令行参数指定用于“可移动”或者“不可移动”的内存的大小。

内核中分配页框的函数入口是：alloc_pages函数

## 解读代码

例如在UMA架构下，CALL STACK是"`alloc_pages-->alloc_pages_node-->__alloc_pages_node-->__alloc_pages-->__alloc_pages_nodemask`"。

```c++
//linux\mm\page_alloc.c
/**
 * This is the 'heart' of the zoned buddy allocator.
 */
struct page *
__alloc_pages_nodemask(gfp_t gfp_mask, unsigned int order, int preferred_nid,
							nodemask_t *nodemask)
{
	struct page *page;
	unsigned int alloc_flags = ALLOC_WMARK_LOW;
...
};
```

详细代码分析见：

- 分配方面 [Linux内存管理 - zoned page frame allocator - 2](https://www.cnblogs.com/LoyenWang/p/11666939.html)；
- 水位方面 [Linux内存管理 - zoned page frame allocator - 3](https://www.cnblogs.com/LoyenWang/p/11708255.html)； 
- 碎片方面，[Linux内存管理 - zoned page frame allocator - 4](https://www.cnblogs.com/LoyenWang/p/11746357.html)；
- 回收方面，[Linux内存管理 - zoned page frame allocator - 5](https://www.cnblogs.com/LoyenWang/p/11827153.html)

# 分配器：SLAB/SLUB/SLOB

SLAB/SLUB/SLOB 分配器建立在伙伴系统分配器之上。

典型分配器buddy是以页单位进行分配。而小块内存的分配和管理是通过块分配器来实现的。目前内核中，有三种方式来实现小块内存分配：slab, slub, slob，最先有slab分配器，slub/slob分配器是改进版，slob分配器适用于小内存嵌入式设备，而slub分配器目前已逐渐成为主流块分配器。在 [Linux内存管理slub分配器](https://www.cnblogs.com/LoyenWang/p/11922887.html) 中有较详细的介绍。

SLAB分配器基本思想是将常用对象的缓存保持在初始化状态以供内核使用。如果没有基于对象的分配器，内核将花费大量时间来分配、初始化和释放同一个对象。SLAB分配器旨在缓存释放的对象，以便在使用之间保留基本结构

每个含连续多个pages的cache称为slabs，slabs被分成小块，这些小块被用于数据结构和对象的缓存管理。示意如下：

![Layout of the Slab Allocator](https://www.kernel.org/doc/gorman/html/understand/understand-html037.png)

Slab 分配器有三个主要目标：

- 分配小块内存以帮助消除可能由伙伴系统引起的内部碎片；
- 缓存常用对象，使系统不会浪费时间分配、初始化和销毁​​对象。
- 通过将对象与 L1 或 L2 缓存对齐，更好地利用硬件缓存。

在kconfig中相关配置如下,比如选择了“CONFIG_SLUB”，那相应实现就是“obj-$(CONFIG_SLUB) += slub.o”：
```
choice
	prompt "Choose SLAB allocator"
	default SLUB
	help
	   This option allows to select a slab allocator.

config SLAB
	bool "SLAB"
	select HAVE_HARDENED_USERCOPY_ALLOCATOR
	help
	  The regular slab allocator that is established and known to work
	  well in all environments. It organizes cache hot objects in
	  per cpu and per node queues.

config SLUB
	bool "SLUB (Unqueued Allocator)"
	select HAVE_HARDENED_USERCOPY_ALLOCATOR
	help
	   SLUB is a slab allocator that minimizes cache line usage
	   instead of managing queues of cached objects (SLAB approach).
	   Per cpu caching is realized using slabs of objects instead
	   of queues of objects. SLUB can use memory efficiently
	   and has enhanced diagnostics. SLUB is the default choice for
	   a slab allocator.

config SLOB
	depends on EXPERT
	bool "SLOB (Simple Allocator)"
	help
	   SLOB replaces the stock allocator with a drastically simpler
	   allocator. SLOB is generally more space efficient but
	   does not perform as well on large systems.

endchoice
```

## 关键数据结构

```c++
//linux\include\linux\slub_def.h
/*
 * Slab cache management.
 */
struct kmem_cache {
	struct kmem_cache_cpu __percpu *cpu_slab;
	/* Used for retriving partial slabs etc */
	unsigned long flags;
	unsigned long min_partial;
	int size;		/* The size of an object including meta data */
	int object_size;	/* The size of an object without meta data */
	int offset;		/* Free pointer offset. */
#ifdef CONFIG_SLUB_CPU_PARTIAL
	int cpu_partial;	/* Number of per cpu partial objects to keep around */
#endif
	struct kmem_cache_order_objects oo;

	/* Allocation and freeing of slabs */
	struct kmem_cache_order_objects max;
	struct kmem_cache_order_objects min;
	gfp_t allocflags;	/* gfp flags to use on each alloc */
	int refcount;		/* Refcount for slab cache destroy */
	void (*ctor)(void *);
	int inuse;		/* Offset to metadata */
	int align;		/* Alignment */
	int reserved;		/* Reserved bytes at the end of slabs */
	int red_left_pad;	/* Left redzone padding size */
	const char *name;	/* Name (only for display!) */
	struct list_head list;	/* List of slab caches */
#ifdef CONFIG_SYSFS
	struct kobject kobj;	/* For sysfs */
	struct work_struct kobj_remove_work;
#endif
#ifdef CONFIG_MEMCG
	struct memcg_cache_params memcg_params;
	int max_attr_size; /* for propagation, maximum size of a stored attr */
#ifdef CONFIG_SYSFS
	struct kset *memcg_kset;
#endif
#endif

#ifdef CONFIG_SLAB_FREELIST_HARDENED
	unsigned long random;
#endif

#ifdef CONFIG_NUMA
	/*
	 * Defragmentation by allocating from a remote node.
	 */
	int remote_node_defrag_ratio;
#endif

#ifdef CONFIG_SLAB_FREELIST_RANDOM
	unsigned int *random_seq;
#endif

#ifdef CONFIG_KASAN
	struct kasan_cache kasan_info;
#endif

	struct kmem_cache_node *node[MAX_NUMNODES];
};

```

## 普通驱动使用SLAB
内核中使用的典型方式：
- 在内核中通过kmem_cache_create接口来创建一个slab缓存。
- kmem_cache_alloc接口用于从slab缓存池中分配对象。
- kmem_cache_free的操作，可以看成是kmem_cache_alloc的逆过程
- kmem_cache_destroy,可以看作kmem_cache_create的逆过程

下面是一个使用kmem_cache的简单例子：
```c++
#define DEVICE_NAME "chardev"

static ssize_t chardev_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos);

static struct file_operations chardev_fops = {
        .read = chardev_read,
};

static struct kmem_cache *cachep = NULL;
static struct cdev *cdevp = NULL;
static dev_t devno = -1;

static ssize_t chardev_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
        char *p = NULL;
        unsigned long n;

        p = kmem_cache_alloc(cachep, GFP_KERNEL);
        if (p)
                memset(p, '1', 40);
        else
                goto err;

        n = copy_to_user(buff, p, 40);

        kmem_cache_free(cachep, p);

        return 40;
err:
        return -ENOMEM;
}

static int __init chardev_init(void)
{
        int ret = -1;

        cachep = kmem_cache_create("chardev_kmem", 40, 0, SLAB_HWCACHE_ALIGN, NULL, NULL);
        if (!cachep)
                goto out;

        ret = alloc_chrdev_region(&devno, 0, 1, DEVICE_NAME);
        if (ret < 0)
                goto region_err;

        cdevp = cdev_alloc();
        if (!cdevp)
                goto alloc_err;

        cdev_init(cdevp, &chardev_fops);
        ret = cdev_add(cdevp, devno, 1);
        if (!ret)
                goto out;

        cdev_del(cdevp);
        cdevp = NULL;
alloc_err:
        unregister_chrdev_region(devno, 1);
        devno = -1;
region_err:
        kmem_cache_destroy(cachep);
        cachep = NULL;
out:
        return ret;
}

static void __exit chardev_exit(void)
{
        if (cdevp)
                cdev_del(cdevp);

        if (devno != -1)
                unregister_chrdev_region(devno, 1);

        if (cachep)
                kmem_cache_destroy(cachep);
}

MODULE_LICENSE("GPL");

module_init(chardev_init);
module_exit(chardev_exit);
```

# 进程地址空间

