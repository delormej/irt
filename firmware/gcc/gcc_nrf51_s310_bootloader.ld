/***********************************************************************************************************************
  Linkerscript for use with the pure-gcc setup from Ole Morten Haaland
------------------------------------------------------------------------------------------------------------------------
  \file       gcc_nrf51_s310_bootloader.ld
  \creation   2014-04-04, Jason De Lorme
***********************************************************************************************************************/
MEMORY
{
  FLASH (rx) : ORIGIN = 0x00036400, LENGTH = 0x9C00 /* 39k bootloader, 1k reserved for bootloader_settings. */
  UICR (rwx) : ORIGIN = 0x10001000, LENGTH = 0x18 
  RAM  (rwx) : ORIGIN = 0x20002400, LENGTH = 0x1C00 /*  */
}
INCLUDE "gcc_nrf51_common.ld"
