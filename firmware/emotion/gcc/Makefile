# List all source files the application uses.
APPLICATION_SRCS = $(notdir $(wildcard ../*.c))  
APPLICATION_SRCS += $(notdir $(wildcard ../**/*.c))  

# SDK libraries used.
APPLICATION_SRCS += softdevice_handler.c
APPLICATION_SRCS += ble_advdata.c
APPLICATION_SRCS += ble_srv_common.c
APPLICATION_SRCS += ble_dis.c
APPLICATION_SRCS += app_timer.c
APPLICATION_SRCS += app_scheduler.c
APPLICATION_SRCS += ble_conn_params.c
APPLICATION_SRCS += ble_debug_assert_handler.c
APPLICATION_SRCS += pstorage.c
APPLICATION_SRCS += nrf_delay.c
APPLICATION_SRCS += simple_uart.c

SOURCE_PATHS +=  ../libraries/
SOURCE_PATHS +=  ../services/
LIBRARY_PATHS += ../libraries/
LIBRARY_PATHS += ../services/

PROJECT_NAME = $(shell basename "$(realpath ../)")
DEPLOY_DIR = '/USERS/jason/SkyDrive/InsideRide/'

DEVICE = NRF51
BOARD = BOARD_IRT_REV_A
#SEGGER_SERIAL = 
TARGET_DEVICE = nRF51422_xxAA

USE_SOFTDEVICE = s310

SDK_PATH = ../../nrf51_sdk/
TEMPLATE_PATH = ../../gcc/

# Enable optimizing for file size.
CFLAGS = -Os
CFLAGS += -DLOOPBACK
CFLAGS += -DBLE_NUS_ENABLED

############################################
# For debugging purposes
############################################
#CFLAGS += -DSIM_SPEED
CFLAGS += -DIRT_DEBUG
CFLAGS += -DUART
# Enable debug flags.
#CFLAGS = -g

############################################
# Define the Software Revision.
############################################
CFLAGS += -DSW_REVISION=0x06

############################################
# Define the device ID.  
# TODO: this should be done another way.
############################################
CFLAGS += -DANT_DEVICE_NUMBER=0x02

LDFLAGS += --specs=nano.specs 

GDB_PORT_NUMBER = 2331

include $(TEMPLATE_PATH)Makefile

deploy:
	cp $(BIN) $(DEPLOY_DIR)$(OUTPUT_NAME).bin
