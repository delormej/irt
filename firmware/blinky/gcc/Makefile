########################################################################################################################
# Makefile
#-----------------------------------------------------------------------------------------------------------------------
# \file       Makefile
# \creation   2014-04-04, Jason De Lorme
#-----------------------------------------------------------------------------------------------------------------------
# Makefile for the simplest program for testing purposes (pure-gcc) - has the s310 stack.
#
########################################################################################################################

# List all source files the application uses.
APPLICATION_SRCS := 

# Bootloader Application
APPLICATION_SRCS += main.c
APPLICATION_SRCS += nrf_delay.c
APPLICATION_SRCS += app_timer.c
APPLICATION_SRCS += simple_uart.c

LIBRARY_PATHS += ../../emotion

# Use shell to find name of root folder. Possible but horrible to do in make.
PROJECT_NAME := $(shell basename "$(realpath ../)")

DEVICE = NRF51
BOARD = BOARD_IRT_V2_REV_A1
#BOARD = BOARD_PCA10001
#SEGGER_SERIAL = 
#TARGET_DEVICE = nRF51422_xxAA

USE_SOFTDEVICE = s310

SDK_PATH = ../../nrf51_sdk/
TEMPLATE_PATH = ../../gcc/

LINKER_SCRIPT := $(TEMPLATE_PATH)gcc_nrf51_s310_xxaa.ld
#LINKER_SCRIPT := $(TEMPLATE_PATH)gcc_nrf51_blank_xxaa.ld

CFLAGS := -Os

# Include debug symbols
CFLAGS += -g

# Define the S310 stack requirement as this is used as a directive in the dfu_types.
CFLAGS += -DS310_STACK  

GDB_PORT_NUMBER := 2331

include $(TEMPLATE_PATH)Makefile
