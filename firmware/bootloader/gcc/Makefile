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
APPLICATION_SRCS += dfu_dual_bank.c # Image too big for dual bank right now.
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

# Use shell to find name of root folder. Possible but horrible to do in make.
PROJECT_NAME := $(shell basename "$(realpath ../)")

DEVICE = NRF51
BOARD = BOARD_IRT_REV_A
#SEGGER_SERIAL = 
TARGET_DEVICE = nRF51422_xxAA

USE_SOFTDEVICE = s310

SDK_PATH = ../../nrf51_sdk/
TEMPLATE_PATH = ../../gcc/

LINKER_SCRIPT := $(TEMPLATE_PATH)gcc_nrf51_s310_bootloader.ld

LIBRARY_PATHS += ../include
LIBRARY_PATHS += ../include/ble_transport
LIBRARY_PATHS += ../include/hci_transport
#SOURCE_PATHS += ...

CFLAGS := -Os
# Dead Code Elemination - but if there was no dead code, it blows up the codesize a little
#CFLAGS += -ffunction-sections -fdata-sections
#LDFLAGS += -Wl,--gc-sections

# Include debug symbols
CFLAGS = -g

# Define the S310 stack requirement as this is used as a directive in the dfu_types.
CFLAGS += -DS310_STACK  

GDB_PORT_NUMBER := 2331

include $(TEMPLATE_PATH)Makefile
