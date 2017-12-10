CC = gcc
CFLAGS = -g
TARGET = test.exe
LIB = -lm -std=gnu99

INCLUDES = -I./
INCLUDES += -I./include/
INCLUDES += -I../emotion/
INCLUDES += -I../emotion/services/
INCLUDES += -I../emotion/libraries/
INCLUDES += -I../emotion/libraries/math/
INCLUDES += -I../nrf51sdk_7.2/libraries/util/
INCLUDES += -I../nrf51sdk_7.2/drivers_nrf/hal/
INCLUDES += -I../nrf51sdk_7.2/ble/common/
INCLUDES += -I../nrf51sdk_7.2/ble/ble_services/ble_cscs/
INCLUDES += -I../nrf51sdk_7.2/softdevice/s310/headers/
INCLUDES += -I../nrf51sdk_7.2/softdevice/common/softdevice_handler/

SOURCES = ./*.c 
SOURCES += ./cutest/*.c 
SOURCES += ./libraries/*.c 
SOURCES += ./services/*.c
SOURCES += ../emotion/libraries/speed_event_fifo.c
SOURCES += ../emotion/libraries/math/*.c
SOURCES +=  ../emotion/libraries/ctf_power.c
SOURCES += ../emotion/libraries/magnet.c
SOURCES += ../emotion/libraries/ctf_offset.c
SOURCES += ../emotion/libraries/simulation.c
SOURCES += ../emotion/services/ant_bg_scanner.c 

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(INCLUDES) $(LIB) $(SOURCES)

clean:
	#$(RM) $(TARGET)
