
[how-the-arm32-kernel-starts](https://people.kernel.org/linusw/how-the-arm32-kernel-starts)

这还有另一篇解读，比较侧重关联知识 [ARM Linux 5.0 -- arch/arm/kernel/head.S](https://biscuitos.github.io/blog/ARM-SCD-kernel-head.S/)


在“linux\arch\arm\kernel\vmlinux.lds.S”中可以看到，内核（vmlinux）的start VA被定义为“PAGE_OFFSET + TEXT_OFFSET”，其中PAGE_OFFSET来自kconfig的配置， TEXT_OFFSET来自"arch/arm/Makefile"中定义的"`textofs-y`"并且通常是0x8000。

这个内核start VA在起始引导建立映射的时候很重要，所以我们在程序中有引用 KERNEL_RAM_VADDR （“KERNEL_RAM_VADDR被定义为(PAGE_OFFSET + TEXT_OFFSET)”）。

以下内容涉及linux source版本为“4.14.21”

## 1.解读入口ENTRY(stext)

```c++
//linux\arch\arm\kernel\head.S
    ...
    /**
     * "initial page table"(由swapper_pg_dir指向)将保存的地方
    */
	.globl	swapper_pg_dir
	.equ	swapper_pg_dir, KERNEL_RAM_VADDR - PG_DIR_SIZE
    ...
	__HEAD
ENTRY(stext)
    ...
    /**
     * CP15有16个register，其中和cpu-id相关的是c0
     * 访问处理器标识reg，结果保存在r9
    */
	mrc	p15, 0, r9, c0, c0		@ get processor id
    /**
     * 根据cpu-id 去查找procinfo，结果放在r5
	 * 原理是查询"`[__proc_info_begin,__proc_info_end]`"区域得到信息
	 * 该区域的内容是编译链接时填充的静态内容
    */
	bl	__lookup_processor_type		@ r5=procinfo r9=cpuid
    ...
    /**
     * 利用"lable 2"这个存放了链接地址(VA)的地方，获取得到PHYS_OFFSET并放在r8
     * 为理解可参考[PHYS_OFFSET与PAGE_OFFSET示意](https://dflund.se/~triad/images/phys-virtual-1.jpg)
	 * 对同属于相同线性映射的任意一个地址x
	 * 成立：PA_label2 -VA_label2 = PA_x -VA_x
	 * 因此: PA_x = (PA_label2 -VA_label2) + VA_x
    */
	adr	r3, 2f              @  //将label 2处物理地址(PA)值放入r3
	ldmia	r3, {r4, r8}       //将 label 2处地址(VA)放入r4， PAGE_OFFSET值放入r8
	sub	r4, r3, r4			@ (PHYS_OFFSET - PAGE_OFFSET) // 物理地址与虚拟地址差值  (PA-VA)
	add	r8, r8, r4			@ PHYS_OFFSET  //得到虚拟地址PAGE_OFFSET对应的物理地址
    ...
	/*
	 * r1 = machine no, r2 = atags or dtb,
	 * r8 = phys_offset, r9 = cpuid, r10 = procinfo
	 */
	...
    /**
     * phys/virt转换的runtime-patch，填充符号表".pv_table"内容
     * 这个runtime-patch的目的是：创建需要在具有不同内存配置的系统上启动而无需重新编译内核的内核
     * 在__fixup_a_pv_table中，核心是更新"__pv_phys_pfn_offset"
     * 
     * 注:除了这个地方，这也必须在每次内核加载模块时发生（可以看
     *    到"module_finalize-->fixup_pv_table-->__fixup_a_pv_table"），谁知道新模块是否需要在物理地址
     *    和虚拟地址之间进行转换！出于这个原因，所有模块 ELF 文件都将包含一个.pv_table具有相同类型表的
     *    部分，并且每次加载模块时也会调用完全相同的汇编循环
    */
#ifdef CONFIG_ARM_PATCH_PHYS_VIRT
	bl	__fixup_pv_table  // 在head.S此刻，__pv_phys_pfn_offset是被赋值前面计算得到的r8值
#endif
    /**
     * 创建"initial page table"(由swapper_pg_dir指向)
     * 仅处理sections of 1MB，因此根据硬件手册The 32-bit 1MB section descriptors 格式应如下：
     * 31                              2019                      2 1  0
     * +---------------------------------+------------------------+-+-+
     * | section base in physical memory | who care               |1|0|
     * +---------------------------------+------------------------+-+-+
    */
	bl	__create_page_tables

	/*
	 * The following calls CPU specific code in a position independent
	 * manner.  See arch/arm/mm/proc-*.S for details.  r10 = base of
	 * xxx_proc_info structure selected by __lookup_processor_type
	 * above.
	 *
	 * The processor init function will be called with:
	 *  r1 - machine type
	 *  r2 - boot data (atags/dt) pointer
	 *  r4 - translation table base (low word)
	 *  r5 - translation table base (high word, if LPAE)
	 *  r8 - translation table base 1 (pfn if LPAE)
	 *  r9 - cpuid
	 *  r13 - virtual address for __enable_mmu -> __turn_mmu_on
	 *
	 * On return, the CPU will be ready for the MMU to be turned on,
	 * r0 will hold the CPU control register value, r1, r2, r4, and
	 * r9 will be preserved.  r5 will also be preserved if LPAE.
	 */
    /**
     * 将__mmap_switched函数地址放入寄存器
    */
	ldr	r13, =__mmap_switched		@ address to jump to after
						@ mmu has been enabled
    /**
     * 将"b __enable_mmu"这条指令的地址放在lr中，存储调用子程序后返回的指令地址
     * We jump to __enable_mmu. r4中存放着initial page table. 它使用CP15（即TTBR0）加载该page table pointer进MMU
     * 另外，它里面会调用__turn_mmu_on
     */                        
	badr	lr, 1f				@ return (PIC) address
#ifdef CONFIG_ARM_LPAE
	mov	r5, #0				@ high TTBR0
	mov	r8, r4, lsr #12			@ TTBR1 is swapper_pg_dir pfn
#else
    /**
     * 将临时内核页表的地址放入r8
    */
	mov	r8, r4				@ set TTBR1 to swapper_pg_dir
#endif
    /**
     * 由于有"DEFINE(PROCINFO_INITFUNC,	offsetof(struct proc_info_list, __cpu_flush));"定义
     * 显然这里是将cpu对应proc info中的__cpu_flush放入r12
    */
	ldr	r12, [r10, #PROCINFO_INITFUNC]
	add	r12, r12, r10
    /**
     * 跳转到r12指定的函数去指向
     * 当该过程返回时由于前面指定了lr，所以接着跳转去__enable_mmu，又因为在"`__enable_mmu-->__turn_mmu_on`"里面
     * 指定了"ret r13"（实际代码是“`mov	r3, r13 以及ret r3`”），最终最后执行 __mmap_switched
     * 
    */
	ret	r12
1:	b	__enable_mmu
ENDPROC(stext)
	.ltorg
#ifndef CONFIG_XIP_KERNEL
2:	.long	.
	.long	PAGE_OFFSET
#endif
```



## 2.解读__fixup_pv_table

原理与head.s中求解“虚拟地址PAGE_OFFSET对应的物理地址”原理一样

```c++
/* __fixup_pv_table - patch the stub instructions with the delta between
 * PHYS_OFFSET and PAGE_OFFSET, which is assumed to be 16MiB aligned and
 * can be expressed by an immediate shifter operand. The stub instruction
 * has a form of '(add|sub) rd, rn, #imm'.
 */
	__HEAD
__fixup_pv_table:
    /**
     * 通过"lable 1"（也被称为__pv_table）赋值r3..r7 
    */
	adr	r0, 1f
	ldmia	r0, {r3-r7}
	mvn	ip, #0
    /**
     * The code uses the first value loaded into r3 to calculate the offset to physical memory, 
    */
	subs	r3, r0, r3	@ PHYS_OFFSET - PAGE_OFFSET
    /**
     * then adds that to each of the other registers so that r4 thru r7 now point directly to the 
     * physical memory contained in each of these labels. 
     * So r4 is a pointer to the physical memory storing __pv_table_begin, r5 points to __pv_table_end, 
     * r6 point to __pv_phys_pfn_offset and r7 points to __pv_offset. In C these would be u32 *, so pointers to some 32 bit numbers.
    */
	add	r4, r4, r3	@ adjust table start address          //r4 is a pointer to the physical memory storing __pv_table_begin
	add	r5, r5, r3	@ adjust table end address            //r5 points to __pv_table_end
	add	r6, r6, r3	@ adjust __pv_phys_pfn_offset address //r6 point to __pv_phys_pfn_offset
	add	r7, r7, r3	@ adjust __pv_offset address          //r7 points to __pv_offset
	mov	r0, r8, lsr #PAGE_SHIFT	@ convert to PFN   // r0 = r8 >> PAGE_SHIFT
	str	r0, [r6]	@ save computed PHYS_OFFSET to __pv_phys_pfn_offset
	strcc	ip, [r7, #HIGH_OFFSET]	@ save to __pv_offset high bits
	mov	r6, r3, lsr #24	@ constant for add/sub instructions
	teq	r3, r6, lsl #24 @ must be 16MiB aligned
THUMB(	it	ne		@ cross section branch )
	bne	__error
	str	r3, [r7, #LOW_OFFSET]	@ save to __pv_offset low bits
	b	__fixup_a_pv_table
ENDPROC(__fixup_pv_table)

	.align
1:	.long	.
	.long	__pv_table_begin
	.long	__pv_table_end
2:	.long	__pv_phys_pfn_offset
	.long	__pv_offset
```

## 3.解读__create_page_tables

```c++
/*
 * Setup the initial page tables.  We only setup the barest
 * amount which are required to get the kernel running, which
 * generally means mapping in the kernel code.
 *
 * r8 = phys_offset, r9 = cpuid, r10 = procinfo
 *
 * Returns:
 *  r0, r3, r5-r7 corrupted
 *  r4 = physical page table address
 */
__create_page_tables:
    /**
     * 等价：r4 = (phys_offset + TEXT_OFFSET) - PG_DIR_SIZE
    */
	pgtbl	r4, r8				@ page table address

	/*
	 * Clear the swapper page table
	 */
	...
	ldr	r7, [r10, #PROCINFO_MM_MMUFLAGS] @ mm_mmuflags

	/*
	 * Create identity mapping to cater for __enable_mmu.
	 * This identity mapping will be removed by paging_init().
	 */
    /**
     * 为__turn_mmu_on这段代码创建个物理地址和虚拟地址之间 1:1 映射
     * __turn_mmu_on被声明放入在一个名为 ".idmap.text"的单独section。 这里所以我们只为这段代码映射一个完整的 1MB 部分
    */
	adr	r0, __turn_mmu_on_loc //r0保存__turn_mmu_on_loc的物理地址
	ldmia	r0, {r3, r5, r6}  //取得__turn_mmu_on_loc，__turn_mmu_on，__turn_mmu_on_end 的VA，保存至r3, r5, r6中，
	sub	r0, r0, r3			@ virt->phys offset //取得VA/PA 之间的offset
	add	r5, r5, r0			@ phys __turn_mmu_on //取得__turn_mmu_on PA
	add	r6, r6, r0			@ phys __turn_mmu_on_end //取得__turn_mmu_on_end PA
	mov	r5, r5, lsr #SECTION_SHIFT  
	mov	r6, r6, lsr #SECTION_SHIFT

1:	orr	r3, r7, r5, lsl #SECTION_SHIFT	@ flags + kernel base //r3保存要存入页表的值
    /**
     * 存入页表，这里链接地址=物理地址，指定页表（页表基址r4）偏移r5（此为物理地址的高12位）左移2位（页表每一页为4字节）
    */
	str	r3, [r4, r5, lsl #PMD_ORDER]	@ identity mapping  
	cmp	r5, r6  //比较是否到达__turn_mmu_on_end
	addlo	r5, r5, #1			@ next section  //r5加上1，即1M
	blo	1b  //

	/*
	 * Map our RAM from the start to the end of the kernel .bss section.
	 */
    /**
     * 得到内核空间内存第一个32位段描述符的物理地址并保存在r0
    */
	add	r0, r4, #PAGE_OFFSET >> (SECTION_SHIFT - PMD_ORDER)
    /**
     * 符号_end是被设置为内核对象本身的.bss部分的末尾。
     * BSS 是内存中二进制内核足迹末尾部分的惯用名称，这是 C 编译器为所有运行时变量分配位置的地方。这部分的地址定义明确，但没有二进制数据
    */
	ldr	r6, =(_end - 1)
    //  PHYS_OFFSET
    /**
     * 构建搜个section descriptor的值，采用"r3= PHYS_OFFSET | r7"
    */
	orr	r3, r8, r7
    /**
     * 为我们索要映射的最后1M内存(_end),构造得到其32位段描述符的物理地址并保存在r6
    */
	add	r6, r4, r6, lsr #(SECTION_SHIFT - PMD_ORDER)
    /**
     * 得到了首/末section descriptor所在地址(r0/r6)，以及首sction desscrpter的值（r3）后，开始循环
     * 结果是创建整个内核的虚拟到物理映射，包括所有段和.bss
    */
1:	str	r3, [r0], #1 << PMD_ORDER   //post-increment the address in r0 with (1 << PMD_ORDER) which will be 4
	add	r3, r3, #1 << SECTION_SHIFT //increase the descriptor physical address portion with 1 MB (1 << SECTION_SHIFT)
	cmp	r0, r6   // check we we reached the last descriptor else loop to 1
	bls	1b      
    ... 
	/*
	 * Then map boot params address in r2 if specified.
	 * We map 2 sections in case the ATAGs/DTB crosses a section boundary.
	 */
	mov	r0, r2, lsr #SECTION_SHIFT
	movs	r0, r0, lsl #SECTION_SHIFT
	subne	r3, r0, r8
	addne	r3, r3, #PAGE_OFFSET
	addne	r3, r4, r3, lsr #(SECTION_SHIFT - PMD_ORDER)
	orrne	r6, r7, r0
	strne	r6, [r3], #1 << PMD_ORDER
	addne	r6, r6, #1 << SECTION_SHIFT
	strne	r6, [r3]

#if defined(CONFIG_ARM_LPAE) && defined(CONFIG_CPU_ENDIAN_BE8)
	sub	r4, r4, #4			@ Fixup page table pointer
						@ for 64-bit descriptors
#endif

#ifdef CONFIG_DEBUG_LL
#if !defined(CONFIG_DEBUG_ICEDCC) && !defined(CONFIG_DEBUG_SEMIHOSTING)
	/*
	 * Map in IO space for serial debugging.
	 * This allows debug messages to be output
	 * via a serial console before paging_init.
	 */
	addruart r7, r3, r0

	mov	r3, r3, lsr #SECTION_SHIFT
	mov	r3, r3, lsl #PMD_ORDER

	add	r0, r4, r3
	mov	r3, r7, lsr #SECTION_SHIFT
	ldr	r7, [r10, #PROCINFO_IO_MMUFLAGS] @ io_mmuflags
	orr	r3, r7, r3, lsl #SECTION_SHIFT
#ifdef CONFIG_ARM_LPAE
	mov	r7, #1 << (54 - 32)		@ XN
#ifdef CONFIG_CPU_ENDIAN_BE8
	str	r7, [r0], #4
	str	r3, [r0], #4
#else
	str	r3, [r0], #4
	str	r7, [r0], #4
#endif
#else
	orr	r3, r3, #PMD_SECT_XN
	str	r3, [r0], #4
#endif

#else /* CONFIG_DEBUG_ICEDCC || CONFIG_DEBUG_SEMIHOSTING */
	/* we don't need any serial debugging mappings */
	ldr	r7, [r10, #PROCINFO_IO_MMUFLAGS] @ io_mmuflags
#endif

#if defined(CONFIG_ARCH_NETWINDER) || defined(CONFIG_ARCH_CATS)
	/*
	 * If we're using the NetWinder or CATS, we also need to map
	 * in the 16550-type serial port for the debug messages
	 */
	add	r0, r4, #0xff000000 >> (SECTION_SHIFT - PMD_ORDER)
	orr	r3, r7, #0x7c000000
	str	r3, [r0]
#endif
#ifdef CONFIG_ARCH_RPC
	/*
	 * Map in screen at 0x02000000 & SCREEN2_BASE
	 * Similar reasons here - for debug.  This is
	 * only for Acorn RiscPC architectures.
	 */
	add	r0, r4, #0x02000000 >> (SECTION_SHIFT - PMD_ORDER)
	orr	r3, r7, #0x02000000
	str	r3, [r0]
	add	r0, r4, #0xd8000000 >> (SECTION_SHIFT - PMD_ORDER)
	str	r3, [r0]
#endif
#endif
#ifdef CONFIG_ARM_LPAE
	sub	r4, r4, #0x1000		@ point to the PGD table
#endif
	ret	lr
ENDPROC(__create_page_tables)
	.ltorg
	.align
__turn_mmu_on_loc:
	.long	.
	.long	__turn_mmu_on
	.long	__turn_mmu_on_end

```

## 4.解读__enable_mmu 与__turn_mmu_on

[CP15协处理器寄存器说明](https://blog.csdn.net/ooonebook/article/details/52860186)

```c++
/*
 * Setup common bits before finally enabling the MMU.  Essentially
 * this is just loading the page table pointer and domain access
 * registers.  All these registers need to be preserved by the
 * processor setup function (or set in the case of r0)
 *
 *  r0  = cp#15 control register
 *  r1  = machine ID
 *  r2  = atags or dtb pointer
 *  r4  = TTBR pointer (low word)
 *  r5  = TTBR pointer (high word if LPAE)
 *  r9  = processor ID
 *  r13 = *virtual* address to jump to upon completion
 */
__enable_mmu:
#if defined(CONFIG_ALIGNMENT_TRAP) && __LINUX_ARM_ARCH__ < 6
	orr	r0, r0, #CR_A
#else
	bic	r0, r0, #CR_A
#endif
#ifdef CONFIG_CPU_DCACHE_DISABLE
	bic	r0, r0, #CR_C
#endif
#ifdef CONFIG_CPU_BPREDICT_DISABLE
	bic	r0, r0, #CR_Z
#endif
#ifdef CONFIG_CPU_ICACHE_DISABLE
	bic	r0, r0, #CR_I
#endif
#ifdef CONFIG_ARM_LPAE
	mcrr	p15, 0, r4, r5, c2		@ load TTBR0
#else
	mov	r5, #DACR_INIT
	mcr	p15, 0, r5, c3, c0, 0		@ load domain access register
    /**
     * r4包含初始页表的地址。我们使用特殊CP15指令加载它，该指令旨在将页表指针（在物理内存中）加载到 MMU 中
    */
	mcr	p15, 0, r4, c2, c0, 0		@ load page table pointer
#endif

    /**
     * 到目前为止什么都没有发生。页表地址已在 MMU 中设置，但尚未在物理地址和虚拟地址之间进行转换。
     * 
     * 接下来我们跳转到__turn_mmu_on。这就是魔法发生的地方。  正如我们所说的__turn_mmu_on被巧妙
     * 地编译到节中.idmap.text，在之前作为PA/VA 1：1映射。这意味着它在物理和虚拟内存中具有相同的地址
    */
	b	__turn_mmu_on
ENDPROC(__enable_mmu)

```

```c++
/*
 * Enable the MMU.  This completely changes the structure of the visible
 * memory space.  You will not be able to trace execution through this.
 * If you have an enquiry about this, *please* check the linux-arm-kernel
 * mailing list archives BEFORE sending another post to the list.
 *
 *  r0  = cp#15 control register
 *  r1  = machine ID
 *  r2  = atags or dtb pointer
 *  r9  = processor ID
 *  r13 = *virtual* address to jump to upon completion
 *
 * other registers depend on the function called upon completion
 */
	.align	5
	.pushsection	.idmap.text, "ax"
ENTRY(__turn_mmu_on)
	mov	r0, r0
	instr_sync
	mcr	p15, 0, r0, c1, c0, 0		@ write control reg
	mrc	p15, 0, r3, c0, c0, 0		@ read id reg
	instr_sync
	mov	r3, r3
	mov	r3, r13
    /**
     * 该函数返回时，接着去执行r13的函数
    */
	ret	r3
__turn_mmu_on_end:
ENDPROC(__turn_mmu_on)
	.popsection
```

## 5.解读__lookup_processor_type
```c++
/*
 * Read processor ID register (CP#15, CR0), and look up in the linker-built
 * supported processor list.  Note that we can't use the absolute addresses
 * for the __proc_info lists since we aren't running with the MMU on
 * (and therefore, we are not in the correct address space).  We have to
 * calculate the offset.
 *
 *	r9 = cpuid
 * Returns:
 *	r3, r4, r6 corrupted
 *	r5 = proc_info pointer in physical address space
 *	r9 = cpuid (preserved)
 */
__lookup_processor_type:
    /**
     * r3：保存__lookup_processor_type_data PA
     * r4: 保存__lookup_processor_type_data VA
     * r5: 保存__proc_info_begin VA
     * r6: 保存__proc_info_end   VA
    */
	adr	r3, __lookup_processor_type_data
	ldmia	r3, {r4 - r6}
    /**
     * r5: 保存__proc_info_begin PA
     * r6: 保存__proc_info_end   PA
     * 此时还没有开启MMU，因为已经获得了PA，就可以访问proc info区间了
    */
	sub	r3, r3, r4			@ get offset between virt&phys
	add	r5, r5, r3			@ convert virt addresses to
	add	r6, r6, r3			@ physical address space
    /**
     * r3: 保存cpu_val, 
     * r4: 保存cpu_mask
     * 获取[__proc_info_begin,__proc_info_end]区间第一个前16个字节
     * 由于struct proc_info_list的cpu_val/cpu_mask被放在最前面16字节,因此这样可以的
    */
1:	ldmia	r5, {r3, r4}			@ value, mask
    //将cpu_val 与（cpu-id & cpu_mask）比较，如果相等则返回，此时r5存放着对应proc_info的地址
	and	r4, r4, r9			@ mask wanted bits
	teq	r3, r4
	beq	2f
    //如果不等，则将r5 指向下一个
	add	r5, r5, #PROC_INFO_SZ		@ sizeof(proc_info_list)
    // 看是否到了__proc_info_end
	cmp	r5, r6
	blo	1b
	mov	r5, #0				@ unknown processor
2:	ret	lr
ENDPROC(__lookup_processor_type)

/*
 * Look in <asm/procinfo.h> for information about the __proc_info structure.
 */
	.align	2
    /**
     * ".type伪指令定义__lookup_processor_type_data为object"
     *  根据linux\arch\arm\kernel\vmlinux.lds.S中对__proc_info_begin/__proc_info_end说明
     *  下面结果是__lookup_processor_type_data指向"*(.proc.info.init)" section的信息内容
     * 以[.section ".proc.info.init"]关键字可以知道,指向的是"arch/arm/mm/proc-*.S"中特别定义的一个全局结构变量xxx_proc_info
    */
	.type	__lookup_processor_type_data, %object
__lookup_processor_type_data:
	.long	.
	.long	__proc_info_begin
	.long	__proc_info_end
	.size	__lookup_processor_type_data, . - __lookup_processor_type_data

```

# ARM 汇编


```lds
//linux\arch\arm\kernel\vmlinux.lds.S
SECTIONS
{
	...
	.init.data : {
		INIT_DATA
		INIT_SETUP(16)
		INIT_CALLS
		CON_INITCALL
		SECURITY_INITCALL
		INIT_RAM_FS
	}
	...
}
```

```c++
...

#define INIT_CALLS_LEVEL(level)						\
		VMLINUX_SYMBOL(__initcall##level##_start) = .;		\
		KEEP(*(.initcall##level##.init))			\
		KEEP(*(.initcall##level##s.init))			\

#define INIT_CALLS							\
		VMLINUX_SYMBOL(__initcall_start) = .;			\
		KEEP(*(.initcallearly.init))				\
		INIT_CALLS_LEVEL(0)					\
		INIT_CALLS_LEVEL(1)					\
		INIT_CALLS_LEVEL(2)					\
		INIT_CALLS_LEVEL(3)					\
		INIT_CALLS_LEVEL(4)					\
		INIT_CALLS_LEVEL(5)					\
		INIT_CALLS_LEVEL(rootfs)				\
		INIT_CALLS_LEVEL(6)					\
		INIT_CALLS_LEVEL(7)					\
		VMLINUX_SYMBOL(__initcall_end) = .;
...
```


[arm汇编编程(示例)](http://www.eepw.com.cn/article/201611/317880.htm)


## LDR和STR 指令

LDR R1, =COUNT 意思是将 COUNT 变量的地址放到 R1中
LDR R1, COUNT 意思是将 COUNT 变量地址里面的内容赋给 R1

ldr (load register) 将内存内容载入通用寄存器

str (store register) 将寄存器内容写入内存

例：

"`LDR R1,[R2]; @将R2指向的存储单元的数据读出，保存在R1中R2相当于指针变量`"

"`STR R1,[R2]; @将R1的值写入到R2所指向的内存`"

"`LDR R2,[R3,#4];  @读取R3+4地址上的存储单元的内容，放入R2`"

"`LDR R2,[R3,#4]!; @读取R3+4地址上的存储单元的内容，放入R2，然后R3内的地址变为R3+4,即R3=R3+4`"

"!"表示回写



## STM??/LDR?? 压栈/出栈指令

压栈( stm** sp )

出栈( ldr** sp )

指令后缀：
   - FD: Full Descending
   - ED: Empty Descending
   - FA: Full Ascending
   - EA: Empty Ascending

