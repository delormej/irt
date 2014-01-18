/* Linker script to configure memory regions. */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0X20000, LENGTH = 127K
  RAM (rwx) : ORIGIN = 0x20002400, LENGTH = 7K
}
INCLUDE "gcc_nrf51_common.ld"
