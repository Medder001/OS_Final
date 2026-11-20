MEMORY {
  dram_space (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS
{
  .text 0x80100000: { 
    boot.o(.text)
    *(.text)
  } > dram_space

  .data : {
    *(.data)
  } > dram_space

  .bss : {
    *(.bss)
  } > dram_space

  .rodata : {
    *(.rodata)
  } > dram_space
}

