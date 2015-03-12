/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x0001D000, LENGTH = 0x23000 
  RAM (rwx)  : ORIGIN = 0x20002400, LENGTH = 0x1B80		/* Was 0x1C00, but reserving 128 bytes for noninit  */
  RAM2 (rwx) : ORIGIN = 0x20003F80, LENGTH = 0x80		/* Reserved section of RAM for noinit */
}

SECTIONS
{
   /* Place noinit section in RAM2 region*/
  .NoInit 0x20003F80 (NOLOAD):
  {
  } >RAM2
  /* other placements follow here... */
}

INCLUDE "gcc_nrf51_common.ld"
