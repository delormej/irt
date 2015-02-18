/* Linker script for the Emotion Bootloader */

/* Linker script to configure memory regions. */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x3AC00, LENGTH = 0x5400               
  RAM (rwx)  : ORIGIN = 0x20002400, LENGTH = 0x1B80		/* Was 0x1C00, but reserving 128 bytes for noninit  */
  RAM2 (rwx) : ORIGIN = 0x20003F80, LENGTH = 0x80		/* Reserved section of RAM for noinit */
  UICR (r)   : ORIGIN = 0x10001000, LENGTH = 0xFC
}
SECTIONS
{
  /* placing my named section at given address: */
  .bootloaderSettings 0x0003FC00 (NOLOAD):
  {
	
  } 
  .uicrBootStartAddress 0x10001014:
  {
	KEEP(*(.uicrBootStartAddress))
  } >UICR
  
  /* Place noinit section in RAM2 region*/
  .NoInit 0x20003F80 (NOLOAD):
  {
  } >RAM2
  /* other placements follow here... */
}


INCLUDE "gcc_nrf51_common.ld"
