

## 常见解读

```c
/*设置Reset_Handler符号所属段是“.text”*/
.section  .text.Reset_Handler
/*.weak 如果这个符号名没有被定义，那么就创建它*/  
.weak  Reset_Handler
/*设置Reset_Handler符号的类型为函数,也就是说Reset_Handler是函数的名字*/
.type  Reset_Handler, %function
Reset_Handler:
  cpsid i
  ldr   sp, =_estack       /* set stack pointer */
...

  /*指示符号Reset_Handler的大小是". - Reset_Handler"*/
.size  Reset_Handler, .-Reset_Handler

```

```c
  /*指示以下部分都是属于name为“.isr_vector”的section，并且该section的type是progbits*/
   .section  .isr_vector,"a",%progbits
   /*指示符号g_pfnVectors的类型是object.  %object是数据类型   %function 函数类型*/
  .type  g_pfnVectors, %object
  /*指示符号g_pfnVectors的大小是". - g_pfnVectors"*/
  .size  g_pfnVectors, .-g_pfnVectors
```

```c++
/*
*****************************************************************************
**  File        : LinkerScript.ld
**  Abstract    : Linker script for STM32F407ZGT6 Device with
**                1024KByte FLASH, 192KByte RAM,  64KByte CCM DATA RAM
*****************************************************************************
*/

/* Entry Point */
/*
  设置入口有几种方式，具体有以下几种方式，优先级依次降低:

  - the ‘-e’ entry command-line option;
  - the ENTRY(symbol) command in a linker script;
  - the value of a target-specific symbol, if it is defined; For many targets this is start, 
  - but PE- and BeOS-based systems   for example check a list of possible entry symbols, 
  - matching the first one found.
  - the address of the first byte of the ‘.text’ section, if present;
  - The address 0.
*/
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x20020000;    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

/* Specify the memory areas */
/*
语法如下：
MEMORY
  {
    name [(attr)] : ORIGIN = origin, LENGTH = len
    …
  }
  Once you define a memory region, you can direct the linker to place specific 
  output sections into that memory region by using the ‘>region’ output section attribute
  
  - The origin is an numerical expression for the start address of the memory region.
  - The len is an expression for the size in bytes of the memory region.
*/
MEMORY
{
FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K
RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
CCMRAM (rw)     : ORIGIN = 0x10000000, LENGTH = 64K
}

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    __text_start = .;
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
    __text_end = _etext;
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    __rodata_start = .;

    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
    __rodata_end = .;
  } >FLASH

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* used by the startup to initialize liteos vector */
  _si_liteos_vector_data = LOADADDR(.vector_ram);

  /* Initialized liteos vector sections goes into RAM, load LMA copy after code */
  .vector_ram :
  {
    . = ORIGIN(RAM);
    _s_liteos_vector = .;
    *(.data.vector)    /* liteos vector in ram */
    _e_liteos_vector = .;
  } > RAM AT> FLASH

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data ALIGN(0x1000):
  {
    __ram_data_start = _sdata;
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    KEEP(*( SORT (.liteos.table.*)));

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
    __ram_data_end = _edata;
  } >RAM AT> FLASH


  _siccmram = LOADADDR(.ccmram);

  /* CCM-RAM section
  *
  * IMPORTANT NOTE!
  * If initialized variables will be placed in this section,
  * the startup code needs to be modified to copy the init-values.
  */
  .ccmram :
  {
    . = ALIGN(4);
    _sccmram = .;       /* create a global symbol at ccmram start */
    *(.ccmram)
    *(.ccmram*)

    . = ALIGN(4);
    _eccmram = .;       /* create a global symbol at ccmram end */
  } >CCMRAM AT> FLASH


  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    __bss_start = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
    __bss_end = _ebss;
  } >RAM

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM

  . = ALIGN(8);
  __los_heap_addr_start__ = .;
  __los_heap_addr_end__ = ORIGIN(RAM) + LENGTH(RAM) - _Min_Stack_Size - 1;

  __fast_end = .;

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}



```