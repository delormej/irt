# List all source files the application uses.
APPLICATION_SRCS = $(notdir $(wildcard ../*.c))  
APPLICATION_SRCS += $(notdir $(wildcard ../**/*.c))  

# SDK libraries used.
APPLICATION_SRCS += softdevice_handler.c
APPLICATION_SRCS += ble_error_log.c
APPLICATION_SRCS += ble_advdata.c
APPLICATION_SRCS += ble_srv_common.c
APPLICATION_SRCS += ble_dis.c
APPLICATION_SRCS += ble_bas.c
APPLICATION_SRCS += ble_hrs.c
APPLICATION_SRCS += app_timer.c
APPLICATION_SRCS += ble_conn_params.c
APPLICATION_SRCS += ble_bondmngr.c
APPLICATION_SRCS += ble_debug_assert_handler.c
APPLICATION_SRCS += app_gpiote.c
APPLICATION_SRCS += pstorage.c
APPLICATION_SRCS += crc16.c
APPLICATION_SRCS += twi_hw_master.c
APPLICATION_SRCS += nrf_delay.c

SOURCE_PATHS +=  ../libraries/
SOURCE_PATHS +=  ../services/
LIBRARY_PATHS += ../libraries/
LIBRARY_PATHS += ../services/

PROJECT_NAME = $(shell basename "$(realpath ../)")

DEVICE = NRF51
BOARD = BOARD_IRT_REV_A
#SEGGER_SERIAL = 
TARGET_DEVICE = nRF51422_xxAA

USE_SOFTDEVICE = s310

SDK_PATH = ../../nrf51_sdk/
TEMPLATE_PATH = ../../gcc/

CFLAGS = -Os
CFLAGS += -DLOOPBACK
CFLAGS += -DBLE_NUS_ENABLED

LDFLAGS += --specs=nano.specs 

GDB_PORT_NUMBER = 2331

include $(TEMPLATE_PATH)Makefile
