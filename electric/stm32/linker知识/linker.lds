MEMORY
{
    rom (rx)  : ORIGIN = 0x500, LENGTH = 256k
    ram (!rx) : org = 0x400, l = 4M
}
SECTIONS  
{  
    .text 0x500  :  AT(0)  {  *(.text) }  > ram
    .data        :         {  *(.data) }  > rom
}  

