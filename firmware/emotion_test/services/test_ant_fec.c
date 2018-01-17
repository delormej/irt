#include "cutest/CuTest.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <float.h>

static void print_seq(uint8_t count) {
    char message[32];
    uint8_t page = 0;
    
    if (count == 128 || count == 129)
    {
        page = 0x51;
        strcpy(message, "ant_product_page");
    }
    else if (count == 64 || count == 65)
    {
        page = 0x50;
        strcpy(message, "ant_manufacturer_page");
    }
    else if (count % 61 == 0) 
    {
        page = 82;
        strcpy(message, "battery_status");
    }
    else if (  (~(0b10 ^ count) & 3) == 3  )
    {
        page = 25;
        strcpy(message, "SpecificTrainerDataPage");
    }
    else if (  (~(0b111 ^ count) & 7) == 7  )
    {
        page = 241;
        strcpy(message, "ManufacturerSpecificPage");
    }       
    else if (  (~(0b11 ^ count) & 3) == 3  )
    {
        page = 17;
        strcpy(message, "GeneralSettingsPage");
    }     
    else 
    {
        page = 16;
        strcpy(message, "GeneralFEDataPage");    
    }
    printf("%i\t[%.2x], %s\n", count, page, message);
}

static uint8_t resistance_pct_get(uint16_t position)
{
    const uint16_t MAGNET_POSITION_OFF = 2000;
    const uint16_t MAGNET_POSITION_MIN_RESISTANCE = 1600;
    const uint16_t MAGNET_POSITION_MAX_RESISTANCE = 800;

	float resistance_pct = 0.0f;
    
    if (position < MAGNET_POSITION_MIN_RESISTANCE)
    {
        // Subtract position from min position, divide by full range,
        // multiply by 200 to remove the decimal and represent units of 0.5%.
        resistance_pct = 200 * (  
            (float)((MAGNET_POSITION_MIN_RESISTANCE - position) /
            (float)(MAGNET_POSITION_MIN_RESISTANCE - MAGNET_POSITION_MAX_RESISTANCE)));
    }	
    
	return (uint8_t)resistance_pct;
}

static void test_resistance_pct_get(CuTest* tc) {
    uint16_t position = 800;
    uint8_t pct = 0;
    for (;position <= 2000; position+=10) {
        pct = resistance_pct_get(position);
        printf("position:%i == %i\r\n", position, pct);
    }
}

static void test_message_sequence(CuTest* tc)
{
    for (uint8_t i = 0; i < 255; i++) {
        print_seq(i);
    }
    CuAssertTrue(tc, 0 == 0);
}

CuSuite* cu_getsuite_ant_fec()
{
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_message_sequence);
    SUITE_ADD_TEST(suite, test_resistance_pct_get);
    return suite;
}
