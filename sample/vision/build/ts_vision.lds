/*
 *
 * Automatically generated file; DO NOT EDIT.
 * TDK OS MiddleWare Configuration
 *
 */
/* Search directory for input files */
SEARCH_DIR(.)
/* No dynamic libraries */
__DYNAMIC = 0;
ENTRY(_start)
MEMORY
{
  ROM : ORIGIN = 0x20000000, LENGTH = 0x100000
  RAM : ORIGIN = 0x02200000, LENGTH = 0x80000
  ITCM : ORIGIN = 0x01000000, LENGTH = 0x8000
  DTCM : ORIGIN = 0x01080000, LENGTH = 0x8000
}
PROVIDE (gu32_stack_top = 0x02200000 + 0x80000 - 0x04);
PROVIDE (gu32_rom_size = 0x100000);
REGION_ALIAS("REGION_TEXT", ROM);
REGION_ALIAS("REGION_DATA", RAM);
REGION_ALIAS("REGION_BSS", RAM);
REGION_ALIAS("REGION_ITCM", ITCM);
REGION_ALIAS("REGION_DTCM", DTCM);
SECTIONS
{
 .text :
  {
    *startup*.o*(.text)
    *(.text)
    *(.text.startup)
    *(.text.*)
    . = ALIGN(0x4);
    *(.eh_frame)
    . = ALIGN(0x4);
    *(.gcc_except_table)
    *(.lit)
    *(.srodata)
    *(.srodata.*)
    *(.rela.*)
    *(.rodata)
    . = ALIGN(0x4) ;
    __dtb_start = .;
    KEEP(*dtb.o)
    __dtb_end = .;
 . = ALIGN(0x4);
 __cfg_start = .;
    KEEP(*T10ISP*.o)
    __cfg_end = .;
  . = ALIGN(0x4);
    __ispcfg_start = .;
    KEEP(*isp_config*.o)
    __ispcfg_end = .;
 . = ALIGN(0x4);
 __npu_cfg_start = .;
    KEEP(*fn_*_cfg*.o)
 __npu_cfg_end = .;
 . = ALIGN(0x4);
 __scpu_app_start = .;
    KEEP(*scpu_app*.o)
    __scpu_app_end = .;
 . = ALIGN(0x4);
 __osd_start = .;
    KEEP(*libosd*.o)
    __osd_end = .;
    *(.rodata.*)
    . = ALIGN(0x4);
    __kmod_start = .;
    KEEP(*(.kmod))
    __kmod_end = .;
    __init_start = .;
    KEEP(*(.init))
    __init_end = .;
    __insmod_start = .;
    KEEP(*(.insmod))
    __insmod_end = .;
  __modparam_start = .;
    KEEP(*(.modparam))
    __modparam_end = .;
    . = ALIGN(0x4);
  __cmd_tbl_start = .;
    KEEP(*(.cmd_tbls))
    __cmd_tbl_end = .;
    __snr_start = .;
    KEEP(*(.snr))
    __snr_end = .;
    _etext = .;
    __text_end = .;
  } > REGION_TEXT
  .data : AT(LOADADDR(.text) + SIZEOF(.text))
  {
      . = ALIGN(0x4);
      _sdata = .;
    *(.shdata)
    *(.data .data.*)
    PROVIDE( __tls_base = . );
    *(.gcc_exc)
    PROVIDE( __global_pointer$ = . + 0x800 );
    *(.sdata .sdata.* .sdata*)
    . = ALIGN(0x4);
    _edata = .;
  } > REGION_DATA
  .bss BLOCK (0x4) :
  {
    __bss_start = . ;
    *(.shbss)
    *(.sbss .sbss.* .sbss*)
    *(.bss .bss.* .bss*)
    _end = ALIGN (0x8);
    __end = _end;
  } > REGION_BSS
  .itcm : AT(LOADADDR(.data) + SIZEOF(.data))
  {
    . = ALIGN(0x4);
    _itcm_start = .;
    KEEP(*(.itcm .itcm.*))
        . = ALIGN(0x4);
    _itcm_end = .;
  } > REGION_ITCM
  .dtcm : AT(LOADADDR(.itcm) + SIZEOF(.itcm))
  {
    . = ALIGN(0x4);
    _dtcm_start = .;
    KEEP(*(.dtcm .dtcm.*))
  . = ALIGN(0x4);
    _dtcm_end = .;
  } > REGION_DTCM
}
