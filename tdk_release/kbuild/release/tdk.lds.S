
/* Search directory for input files */
SEARCH_DIR(.)

/* No dynamic libraries */
__DYNAMIC  =  0;

ENTRY(_start)

#ifdef CONFIG_FIX_ROM_SIZE
#define CONFIG_CUR_ROM_SZ CONFIG_FIX_ROM_SIZE
#else
#define CONFIG_CUR_ROM_SZ CONFIG_ROM_SIZE
#endif

MEMORY
{
#if (CONFIG_ROM_BASE_ADDR != CONFIG_RAM_BASE_ADDR)
  ROM : ORIGIN = CONFIG_ROM_BASE_ADDR, LENGTH = CONFIG_CUR_ROM_SZ
  RAM : ORIGIN = CONFIG_RAM_BASE_ADDR, LENGTH = CONFIG_RAM_SIZE
#else
  RAM : ORIGIN = CONFIG_RAM_BASE_ADDR, LENGTH = CONFIG_RAM_SIZE
#endif

#if (CONFIG_ITCM_SIZE > 0)
  ITCM : ORIGIN = CONFIG_ITCM_BASE_ADDR, LENGTH = CONFIG_ITCM_SIZE
#endif

#if (CONFIG_DTCM_SIZE > 0)
  DTCM : ORIGIN = CONFIG_DTCM_BASE_ADDR, LENGTH = CONFIG_DTCM_SIZE
#endif

}

PROVIDE (gu32_stack_top	= CONFIG_RAM_BASE_ADDR + CONFIG_RAM_SIZE - 0x04);
PROVIDE (gu32_rom_size	= CONFIG_CUR_ROM_SZ);

#if (CONFIG_ROM_BASE_ADDR != CONFIG_RAM_BASE_ADDR)
REGION_ALIAS("REGION_TEXT", ROM);
#else
REGION_ALIAS("REGION_TEXT", RAM);
#endif

REGION_ALIAS("REGION_DATA", RAM);
REGION_ALIAS("REGION_BSS", RAM);

#if (CONFIG_ITCM_SIZE > 0)
REGION_ALIAS("REGION_ITCM", ITCM);
#endif

#if (CONFIG_DTCM_SIZE > 0)
REGION_ALIAS("REGION_DTCM", DTCM);
#endif

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
    _end =  ALIGN (0x8);
    __end = _end;
  } > REGION_BSS


#if (CONFIG_ITCM_SIZE > 0)
  .itcm : AT(LOADADDR(.data) + SIZEOF(.data)) 
  {
    . = ALIGN(0x4);
    _itcm_start = .;
    KEEP(*(.itcm .itcm.*))
        . = ALIGN(0x4);
    _itcm_end = .;
  } > REGION_ITCM

#endif

#if (CONFIG_DTCM_SIZE > 0)

#if (CONFIG_ITCM_SIZE > 0)
  .dtcm : AT(LOADADDR(.itcm) + SIZEOF(.itcm)) 
#else
  .dtcm : AT(LOADADDR(.data) + SIZEOF(.data)) 
#endif
  {
    . = ALIGN(0x4);
    _dtcm_start = .;
    KEEP(*(.dtcm .dtcm.*))
  . = ALIGN(0x4);
    _dtcm_end = .;
  } > REGION_DTCM

#endif


}
