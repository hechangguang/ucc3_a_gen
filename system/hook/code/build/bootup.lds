/*
 *
 * Automatically generated file; DO NOT EDIT.
 * Bootup Configuration
 *
 */
/* Search directory for input files */
SEARCH_DIR(.)
/* No dynamic libraries */
__DYNAMIC = 0;
ENTRY(bootup_hook)
MEMORY
{
  RAM : ORIGIN = 0x02200000, LENGTH = 0x10000
}
REGION_ALIAS("REGION_TEXT", RAM);
REGION_ALIAS("REGION_DATA", RAM);
REGION_ALIAS("REGION_BSS", RAM);
SECTIONS
{
 .text :
  {
    KEEP(*(.hook_start))
    *(.text)
    *(.text.startup)
    *(.text.*)
    . = ALIGN(0x4);
    *(.eh_frame)
    . = ALIGN(0x4);
    *(.gcc_except_table)
    . = ALIGN(0x4);
    __cmdl_start = .;
    KEEP(*(.cmdl_sect))
    __cmdl_end = .;
    . = ALIGN(0x4);
    *(.lit)
    *(.srodata)
    *(.srodata.*)
    *(.rela.*)
    *(.rodata)
    *(.rodata.*)
    . = ALIGN(0x4);
    _hook_etext = .;
    __hook_text_end = .;
  } > REGION_TEXT
  .data BLOCK (0x4): AT(LOADADDR(.text) + SIZEOF(.text))
  {
    . = ALIGN(0x4);
    _hook_sdata = .;
    *(.shdata)
    *(.data)
    *(.data.*)
    *(.gcc_exc)
    *(.sdata)
    *(.sdata.*)
    . = ALIGN(0x4);
    _hook_edata = .;
  } > REGION_DATA
  .bss BLOCK (0x4) :
  {
    __hook_bss_start = . ;
    *(.shbss)
    *(.sbss)
    *(.bss)
    *(.bss.*)
    *(COMMON)
    _hook_end = ALIGN (0x8);
    __hook_end = _hook_end;
  } > REGION_BSS
}
