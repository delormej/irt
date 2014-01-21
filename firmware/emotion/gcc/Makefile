TARGET_CHIP := NRF51422_QFAA_CA
BOARD := BOARD_IRT_REV_A

CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DANT_STACK_SUPPORT_REQD
CFLAGS += -DLOOPBACK
CFLAGS += -DBLE_NUS_ENABLED

LDFLAGS += --specs=nano.specs 

C_SOURCE_FILES += main.c
C_SOURCE_FILES += nrf_pwm.c
C_SOURCE_FILES += irt_peripheral.c
C_SOURCE_FILES += softdevice_handler.c
C_SOURCE_FILES += speed.c
C_SOURCE_FILES += resistance.c
C_SOURCE_FILES += power.c
C_SOURCE_FILES += ble_dis.c
C_SOURCE_FILES += ble_bas.c
C_SOURCE_FILES += ble_hrs.c
C_SOURCE_FILES += ble_nus.c
C_SOURCE_FILES += ble_cps.c
C_SOURCE_FILES += ant_bike_power.c
C_SOURCE_FILES += ble_ant.c
C_SOURCE_FILES += ble_error_log.c
C_SOURCE_FILES += ble_advdata.c
C_SOURCE_FILES += ble_srv_common.c
C_SOURCE_FILES += app_timer.c
C_SOURCE_FILES += ble_conn_params.c
C_SOURCE_FILES += ble_bondmngr.c
C_SOURCE_FILES += ble_debug_assert_handler.c
C_SOURCE_FILES += app_gpiote.c
C_SOURCE_FILES += pstorage.c
C_SOURCE_FILES += crc16.c

C_SOURCE_PATHS += ../
C_SOURCE_PATHS += ../services/
C_SOURCE_PATHS += ../libraries/
C_SOURCE_PATHS += ../../nrf51_sdk/Source/sd_common
C_SOURCE_PATHS += ../../nrf51_sdk/Source/app_common
C_SOURCE_PATHS += ../../nrf51_sdk/Source/ble

INCLUDEPATHS += -I".."
INCLUDEPATHS += -I"../../nrf51_sdk/Include"
INCLUDEPATHS += -I"../../nrf51_sdk/Include/sd_common"
INCLUDEPATHS += -I"../../nrf51_sdk/Include/app_common"
INCLUDEPATHS += -I"../../nrf51_sdk/Include/ble"
INCLUDEPATHS += -I"../../nrf51_sdk/Include/ble/ble_services"
INCLUDEPATHS += -I"../../nrf51_sdk/Include/s310"
INCLUDEPATHS += -I"../services"
INCLUDEPATHS += -I"../libraries"

# This works to program.
# but you might be able to just use "make flash" command.
#C:\Users\jasondel\dev\scratch\ble_ant_emotion>"C:\Program Files (x86)\Nordic Semiconductor\nrf51\bin\nrfjprog.exe" --reset --program _build/blinky_gcc_s310_xxaa.hex

SDK_PATH = ../../nrf51_sdk/
OUTPUT_FILENAME := emotion

DEVICE_VARIANT := xxaa

USE_SOFTDEVICE := S310

PATH := $(SDK_PATH)/bin:$(PATH)
ECHO $(PATH)
include $(SDK_PATH)Source/templates/gcc/Makefile.common

