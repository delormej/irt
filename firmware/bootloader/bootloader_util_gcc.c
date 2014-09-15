/***********************************************************************************************************************
  GCC-spezifier Code für den Bootloader
------------------------------------------------------------------------------------------------------------------------
  \file       bootloader_util_gcc.c
  \creation   2014-02-21, Joe Merten
***********************************************************************************************************************/

#include "bootloader_util.h"
#include <stdint.h>
#include "nordic_common.h"
#include "bootloader_types.h"
#include "dfu_types.h"
#include "irt_common.h"
#include "debug.h"

/**@brief Debug logging for main module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define BL_LOG debug_log
#else
#define BL_LOG(...)
#endif // ENABLE_DEBUG_LOG

// Bootloader settings.
bootloader_settings_t  m_boot_settings __attribute__((section(".bootloader_settings_sect"))) __attribute__((used)) = {BANK_VALID_APP};

// This variable isn't used, but forces the bootloader start address to be written to the hex file.
uint32_t m_uicr_bootloader_start_address __attribute__((section(".NRF_UICR_BOOT_START_SECT"))) __attribute__((used)) = BOOTLOADER_REGION_START;

// Start the application
void bootloader_util_app_start(uint32_t start_addr)
{
        asm volatile(" LDR   R0, =0x20000          \n\t" // Assign app code address
                     " LDR   R2, [R0]              \n\t" // Get App MSP
                     " MSR   MSP, R2               \n\t" // Set the main stack pointer to the applications MSP
                     " LDR   R3, [R0, #0x00000004] \n\t" // Get application reset vector address
                     " BX    R3                    \n\t" // No return - stack code is now activated only through SVC and plain interrupts
                     " .ALIGN                      ");
}

void bootloader_util_settings_get(const bootloader_settings_t** pp_bootloader_settings)
{
    // Read only pointer to bootloader settings in flash.
	*pp_bootloader_settings = &m_boot_settings;
}
