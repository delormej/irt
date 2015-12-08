###############################################################################
#
# Primary Makefile for building E-Motion BOOTLOADER for Softdevice v2. 
# Uses slightly modified Pure GCC make from Nordic. 
#
###############################################################################

PROJECT_NAME = IRT_BOOTLOADER

SDK_PATH = ../nrf51sdk_7.2/
TEMPLATE_PATH = $(SDK_PATH)toolchain/gcc/

# List all source files the application uses.
C_SOURCE_FILES = $(notdir $(wildcard ./*.c))  

############################################
# SDK libraries used.
############################################
C_SOURCE_FILES += ble_advdata.c
C_SOURCE_FILES += ble_conn_params.c
C_SOURCE_FILES += ble_srv_common.c
C_SOURCE_FILES += ble_dfu.c
C_SOURCE_FILES += bootloader_util_gcc.c
C_SOURCE_FILES += nrf_delay.c
C_SOURCE_FILES += pstorage.c
C_SOURCE_FILES += app_error.c
C_SOURCE_FILES += app_scheduler.c
C_SOURCE_FILES += app_timer.c
C_SOURCE_FILES += bootloader.c
C_SOURCE_FILES += crc16.c
C_SOURCE_FILES += dfu_single_bank.c
C_SOURCE_FILES += dfu_init_template.c
C_SOURCE_FILES += dfu_transport_ble.c
C_SOURCE_FILES += hci_mem_pool.c
C_SOURCE_FILES += nrf_assert.c
C_SOURCE_FILES += softdevice_handler.c

C_SOURCE_PATHS += $(SDK_PATH)ble/common/
C_SOURCE_PATHS += $(SDK_PATH)ble/ble_services/ble_dfu/
C_SOURCE_PATHS += $(SDK_PATH)libraries/bootloader_dfu/
C_SOURCE_PATHS += $(SDK_PATH)drivers_nrf/hal/
C_SOURCE_PATHS += $(SDK_PATH)drivers_nrf/pstorage/
C_SOURCE_PATHS += $(SDK_PATH)libraries/util/
C_SOURCE_PATHS += $(SDK_PATH)libraries/scheduler/
C_SOURCE_PATHS += $(SDK_PATH)libraries/timer/
C_SOURCE_PATHS += $(SDK_PATH)libraries/crc16/
C_SOURCE_PATHS += $(SDK_PATH)libraries/hci/
C_SOURCE_PATHS += $(SDK_PATH)softdevice/common/softdevice_handler/
C_SOURCE_PATHS += $(SDK_PATH)ble/ble_services/ble_dis/

INCLUDE_PATHS +=  ./include
INCLUDE_PATHS +=  $(SDK_PATH)libraries/crc16
INCLUDE_PATHS +=  $(SDK_PATH)libraries/bootloader_dfu/ble_transport
INCLUDE_PATHS +=  $(SDK_PATH)ble/ble_services/ble_dfu
INCLUDE_PATHS +=  $(SDK_PATH)libraries/hci
INCLUDE_PATHS +=  $(SDK_PATH)libraries/hci/config
INCLUDE_PATHS +=  $(SDK_PATH)bsp
INCLUDE_PATHS +=  $(SDK_PATH)ble/common
INCLUDE_PATHS +=  $(SDK_PATH)libraries/bootloader_dfu
INCLUDE_PATHS +=  $(SDK_PATH)libraries/timer
INCLUDE_PATHS +=  $(SDK_PATH)drivers_nrf/hal
INCLUDE_PATHS +=  $(SDK_PATH)drivers_nrf/pstorage
INCLUDE_PATHS +=  $(SDK_PATH)drivers_nrf/pstorage/config
INCLUDE_PATHS +=  $(SDK_PATH)libraries/scheduler
INCLUDE_PATHS +=  $(SDK_PATH)libraries/util
INCLUDE_PATHS +=  $(SDK_PATH)softdevice/common/softdevice_handler

DEPLOY_PATH = './../../../../OneDrive/InsideRide/Builds/'

DEVICE_VARIANT = xxAA
TARGET_DEVICE = nRF51422_xxAA
USE_SOFTDEVICE = s310
SOFTDEVICE = $(SDK_PATH)/s310_nrf51422_2.0.1_softdevice.hex

# Use a different linker script because the FLASH location and bootloader settings are defined here.
LINKER_SCRIPT ?= gcc_s310_xxaa_bootloader.ld

BOARD = BOARD_IRT_V2_REV_A1

############################################
# For debugging purposes
############################################
debug : CFLAGS += -DENABLE_DEBUG_LOG			# Enable debug logging.
debug : CFLAGS += -DSIMPLE_UART					# Use the Simple_UART module for logging.
debug : CFLAGS += -DENABLE_DEBUG_ASSERT			# This will block code on error and wait for debugger to attach.
debug : CFLAGS += -g							# Enable debug symbols.
debug : CFLAGS += -Wno-format					# Disable warnings on printf formating
debug : LDFLAGS += -u _printf_float				# Required to printf a float.

############################################
# Define the Software Revision.
############################################
SW_REVISION_MAJ := 1
SW_REVISION_MIN := 1
SW_REVISION_BLD := 0

# Update project name to include the build.
PROJECT_NAME := $(PROJECT_NAME)_$(SW_REVISION_MAJ)_$(SW_REVISION_MIN)_$(SW_REVISION_BLD)

include $(TEMPLATE_PATH)Makefile.common

DEPLOY_SCRIPT = ..\..\python\deploy\deploy.py 

debug: all 

###########
# 8/25/2015
# Deployment script will have to change to be able to support writing bootloader.hex and system.dat
# files rather than the application.hex/.dat as the current deploy.py does. 
#
#deploy:
#	python $(DEPLOY_SCRIPT) $(HEX) 
#	cp $(OUTPUT_PATH)$(OUTPUT_NAME).zip $(DEPLOY_PATH)$(OUTPUT_NAME).zip

name:
	@echo $(PROJECT_NAME)

# Set to 'Valid App'
# Set 0 for CRC value
# Set factory features (0x10001000) to use charger (64) or for no charger (0)
prep:
	$(NRFJPROG) --memwr 0x3fc00 --val 1		
	$(NRFJPROG) --memwr 0x3fc04 --val 0		
	$(NRFJPROG) --memwr 0x10001080 --val 64	

# Does a complete install to a device, wipe clean, new softdevice, bootloader & app.
install: | flash-softdevice flash prep 
	make -C ../emotion --no-print-directory --makefile=../emotion/Makefile flash

.PHONY: debug deploy name prep install
