###############################################################################
#
# Primary Makefile for building E-Motion firmwaree. 
# Uses slightly modified Pure GCC make from Nordic. 
#
###############################################################################

# List all source files the application uses.
APPLICATION_SRCS = $(notdir $(wildcard ../*.c))  
APPLICATION_SRCS += $(notdir $(wildcard ../**/*.c))  

############################################
# SDK libraries used.
############################################
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
DEPLOY_PATH = '../../../../../SkyDrive/InsideRide/'

############################################
# Required for pure gcc from Nordic.
############################################
PROJECT_NAME = $(shell basename "$(realpath ../)")

DEVICE = NRF51
#BOARD = BOARD_IRT_REV_A
BOARD = BOARD_DYN_BAT_N548
#SEGGER_SERIAL =	# not used, we only have 1 segger attached 
TARGET_DEVICE = nRF51422_xxAA
USE_SOFTDEVICE = s310
SDK_PATH = ../../nrf51_sdk/
TEMPLATE_PATH = ../../gcc/

############################################
# Compiler optimizations.
############################################
# Enable optimizing for file size.
#CFLAGS = -Os
LDFLAGS += --specs=nano.specs

CFLAGS += -D$(BOARD)					# Define board version being used.

############################################
# For debugging purposes
############################################
debug : CFLAGS += -DENABLE_DEBUG_LOG			# Enable debug logging.
debug : CFLAGS += -DSIMPLE_UART					# Use the Simple_UART module for logging.
#debug : CFLAGS += -DBLE_NUS_ENABLED			# Use Nordic BLE UART for logging.
debug : CFLAGS += -DENABLE_DEBUG_ASSERT			# This will block code on error and wait for debugger to attach.
debug : CFLAGS += -DSIM_SPEED					# Simulate speed.
debug : CFLAGS += -g							# Enable debug symbols.
debug : LDFLAGS += -u _printf_float				# Required to printf a float.

GDB_PORT_NUMBER = 2331					# Debugging port used by GDB.

############################################
# Define the Software Revision.
############################################
SW_REVISION_MAJ := 0
SW_REVISION_MIN := 8
SW_REVISION_BLD := 32

CFLAGS += -DSW_REVISION_MAJ=$(SW_REVISION_MAJ)
CFLAGS += -DSW_REVISION_MIN=$(SW_REVISION_MIN)
CFLAGS += -DSW_REVISION_BLD=$(SW_REVISION_BLD)
CFLAGS += -DSW_REVISION="\"$(SW_REVISION_MAJ).$(SW_REVISION_MIN).$(SW_REVISION_BLD)\""

############################################
# DEVICE specific data.  
# TODO: this should be done another way.
############################################
CFLAGS += -DANT_DEVICE_NUMBER=0x02
CFLAGS += -DSERIAL_NUMBER=0xAA55AA55

include $(TEMPLATE_PATH)Makefile

debug: all

deploy:
	cp $(BIN) $(DEPLOY_PATH)$(OUTPUT_NAME).bin
	cp $(HEX) $(DEPLOY_PATH)$(OUTPUT_NAME).hex

.PHONY: debug deploy
