########################################################################################################################
# Makefile
#-----------------------------------------------------------------------------------------------------------------------
# \file       Makefile
# \creation   2014-04-04, Jason De Lorme
#-----------------------------------------------------------------------------------------------------------------------
# Makefile for bootloader (pure-gcc).
#
########################################################################################################################

# List all source files the application uses.
APPLICATION_SRCS := 

# Bootloader Application
APPLICATION_SRCS += main.c
APPLICATION_SRCS += bootloader.c
APPLICATION_SRCS += bootloader_util_gcc.c
APPLICATION_SRCS += dfu_dual_bank.c 
#APPLICATION_SRCS += dfu_single_bank.c

# Services
APPLICATION_SRCS += ble_dfu.c

# Libraries
APPLICATION_SRCS += app_timer.c
APPLICATION_SRCS += app_scheduler.c
APPLICATION_SRCS += app_gpiote.c
APPLICATION_SRCS += crc16.c
APPLICATION_SRCS += softdevice_handler.c
APPLICATION_SRCS += pstorage.c
APPLICATION_SRCS += simple_uart.c
APPLICATION_SRCS += debug.c

# HCI Transport
#APPLICATION_SRCS += hci_transport.c
#APPLICATION_SRCS += hci_slip.c
APPLICATION_SRCS += hci_mem_pool.c
#APPLICATION_SRCS += app_uart.c

# BLE Transport
APPLICATION_SRCS += dfu_transport_ble.c
APPLICATION_SRCS += ble_conn_params.c
APPLICATION_SRCS += ble_advdata.c
APPLICATION_SRCS += ble_srv_common.c

DEVICE = NRF51
BOARD = BOARD_IRT_V2_REV_A
#BOARD = BOARD_IRT_REV_A
#SEGGER_SERIAL = 
TARGET_DEVICE = nRF51422_xxAA

USE_SOFTDEVICE = s310
SOFTDEVICE = ../../s310_nrf51422_1.0.0_softdevice.hex

SDK_PATH = ../../nrf51_sdk/
TEMPLATE_PATH = ../../gcc/

LINKER_SCRIPT := $(TEMPLATE_PATH)gcc_nrf51_s310_bootloader.ld

LIBRARY_PATHS += ../include
# See this for why ble and not hci: <https://devzone.nordicsemi.com/blogs/12/nrf51-sdk-v600-bug-report-and-temporary-workaround/
LIBRARY_PATHS += ../include/ble_transport
#LIBRARY_PATHS += ../include/hci_transport
LIBRARY_PATHS +=  ../../emotion
LIBRARY_PATHS +=  ../../emotion/libraries
SOURCE_PATHS  += ../../emotion/libraries

CFLAGS = -Os -flto -ffunction-sections -fdata-sections -fno-builtin
LDFLAGS += --specs=nano.specs -lc -lnosys
LDFLAGS += -Wl,--gc-sections

# Define the S310 stack requirement as this is used as a directive in the dfu_types.
CFLAGS += -DS310_STACK  

############################################
# For debug builds
############################################
debug: CFLAGS += -DSIMPLE_UART
debug: CFLAGS += -g
debug: CFLAGS += -DENABLE_DEBUG_LOG
GDB_PORT_NUMBER := 2331

# Use shell to find name of root folder. Possible but horrible to do in make.
PROJECT_NAME = $(subst BOARD_,,$(BOARD))_
PROJECT_NAME :=  $(PROJECT_NAME)_$(shell basename "$(realpath ../)")

include $(TEMPLATE_PATH)Makefile

debug: all

.PHONY: debug 