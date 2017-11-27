###############################################################################
#
# Primary Makefile for building E-Motion firmwaree. 
# Uses slightly modified Pure GCC make from Nordic. 
#
###############################################################################

SDK_PATH = ../nrf51sdk_7.2/
TEMPLATE_PATH = $(SDK_PATH)toolchain/gcc/

# List all source files the application uses.
C_SOURCE_FILES = $(notdir $(wildcard ./*.c))  
C_SOURCE_FILES += $(notdir $(wildcard ./**/*.c))
C_SOURCE_FILES += $(notdir $(wildcard ./**/**/*.c))    

############################################
# SDK libraries used.
############################################
C_SOURCE_FILES += softdevice_handler.c
C_SOURCE_FILES += ble_advdata.c
C_SOURCE_FILES += ble_srv_common.c
C_SOURCE_FILES += ble_dis.c
C_SOURCE_FILES += app_timer.c
C_SOURCE_FILES += app_scheduler.c
C_SOURCE_FILES += ble_conn_params.c
C_SOURCE_FILES += ble_debug_assert_handler.c
C_SOURCE_FILES += pstorage.c
C_SOURCE_FILES += nrf_delay.c
C_SOURCE_FILES += simple_uart.c
#C_SOURCE_FILES += app_button.c
C_SOURCE_FILES += app_fifo.c

C_SOURCE_PATHS +=  ./libraries/
C_SOURCE_PATHS +=  ./services/
C_SOURCE_PATHS += ./libraries/math/
C_SOURCE_PATHS += $(SDK_PATH)ble/ble_services/ble_dis/

INCLUDE_PATHS +=  .
INCLUDE_PATHS +=  ./libraries/
INCLUDE_PATHS +=  ./libraries/math/
INCLUDE_PATHS +=  ./services/

INCLUDE_PATHS += $(SDK_PATH)softdevice/common/softdevice_handler/
INCLUDE_PATHS += $(SDK_PATH)drivers_nrf/simple_uart/
INCLUDE_PATHS += $(SDK_PATH)drivers_nrf/hal/
INCLUDE_PATHS += $(SDK_PATH)drivers_nrf/pstorage/
INCLUDE_PATHS += $(SDK_PATH)drivers_nrf/pstorage/config/
INCLUDE_PATHS += $(SDK_PATH)drivers_nrf/twi_master/
INCLUDE_PATHS += $(SDK_PATH)drivers_nrf/ble_flash/
INCLUDE_PATHS += $(SDK_PATH)libraries/timer/
INCLUDE_PATHS += $(SDK_PATH)libraries/scheduler/
INCLUDE_PATHS += $(SDK_PATH)libraries/gpiote
#INCLUDE_PATHS += $(SDK_PATH)libraries/button/
INCLUDE_PATHS += $(SDK_PATH)libraries/fifo/
INCLUDE_PATHS += $(SDK_PATH)ble/common/
INCLUDE_PATHS += $(SDK_PATH)ble/ble_services/ble_dis/
INCLUDE_PATHS += $(SDK_PATH)ble/ble_services/ble_cscs/
INCLUDE_PATHS += $(SDK_PATH)ble/ble_error_log/

DEPLOY_PATH = './../../../../Google Drive/Firmware/'
DEPLOY_PATH2 = './../../../../OneDrive/InsideRide/Builds/'

DEVICE_VARIANT = xxAA
USE_SOFTDEVICE = s310
SOFTDEVICE = $(SDK_PATH)/s310_nrf51422_2.0.1_softdevice.hex
TARGET_DEVICE = nRF51422_xxAA
CFLAGS += -DS310_V2								# Define SoftDevice V2

BOARD = BOARD_IRT_V2_REV_A1
#BOARD = BOARD_IRT_V2_REV_A
#BOARD = BOARD_IRT_REV_A
#debug : BOARD = BOARD_DYN_BAT_N548
#SEGGER_SERIAL =	# not used, we only have 1 segger attached 

############################################
# For debugging purposes
############################################
debug : CFLAGS += -DENABLE_DEBUG_LOG			# Enable debug logging.
debug : CFLAGS += -DSIMPLE_UART					# Use the Simple_UART module for logging.
#debug : CFLAGS += -DBLE_NUS_ENABLED			# Use Nordic BLE UART for logging.
debug : CFLAGS += -DENABLE_DEBUG_ASSERT			# This will block code on error and wait for debugger to attach.
#debug : CFLAGS += -DSIM_SPEED					# Simulate speed.
debug : CFLAGS += -g							# Enable debug symbols.
debug : CFLAGS += -Wno-format					# Disable warnings on printf formating
debug : LDFLAGS += -u _printf_float				# Required to printf a float.
#debug : CFLAGS += -DSETTINGS=2					# Sets the device so the accelerometer doesn't turn off

############################################
# Define the Software Revision.
############################################
SW_REVISION_MAJ := 1
SW_REVISION_MIN := 9
SW_REVISION_BLD := 4

CFLAGS += -DSW_REVISION_MAJ=$(SW_REVISION_MAJ)
CFLAGS += -DSW_REVISION_MIN=$(SW_REVISION_MIN)
CFLAGS += -DSW_REVISION_BLD=$(SW_REVISION_BLD)
CFLAGS += -DSW_REVISION="\"$(SW_REVISION_MAJ).$(SW_REVISION_MIN).$(SW_REVISION_BLD)\""

############################################
# DEVICE specific data.  
############################################
#CFLAGS += -DUSE_BATTERY_READ_PIN				# If this device is built to require enabling batt voltage read by charging a capacitor.  You can still read voltage on boards without this. 
CFLAGS += -DUSE_BATTERY_CHARGER					# If this device is enabled to control battery charger.
												# USE_BATTERY_CHARGER also dictates if there is control of the
												# power regulator, since only battery charge enabled boards have 
												# the power regulator.   
#CFLAGS += -DSMALL_MAG							# Small magnet

# Add linker flag to link to the math library.
LDFLAGS += -lm

############################################
# Concat name used for project and binary 
# based on build variants.
############################################
PROJECT_NAME = $(subst BOARD_,,$(BOARD))_

ifeq (USE_BATTERY_READ_PIN, $(findstring USE_BATTERY_READ_PIN,$(CFLAGS)))
	PROJECT_NAME := $(PROJECT_NAME)P
endif

ifeq (USE_BATTERY_CHARGER, $(findstring USE_BATTERY_CHARGER,$(CFLAGS)))
	PROJECT_NAME := $(PROJECT_NAME)C
endif

PROJECT_NAME := $(PROJECT_NAME)_$(SW_REVISION_MAJ)_$(SW_REVISION_MIN)_$(SW_REVISION_BLD)

include $(TEMPLATE_PATH)Makefile.common

DEPLOY_SCRIPT = ..\..\python\deploy\deploy.py 

debug: all 

deploy:
	python $(DEPLOY_SCRIPT) $(BIN) 
	cp $(OUTPUT_PATH)$(OUTPUT_NAME).zip $(DEPLOY_PATH2)$(OUTPUT_NAME).zip

############################################
# Flashes both the bootloader & app.
############################################
flash-all: 
	$(NRFJPROG) --program $(HEX) ./../bootloader/gcc/_build/bootloader.hex --verify --reset

name:
	@echo $(PROJECT_NAME)

.PHONY: debug deploy name prep
