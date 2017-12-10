#include "cutest/CuTest.h"
#include "ant_bg_scanner.h"

/*
static uint16_t device_id_get(ANT_MESSAGE* p_mesg)
{
    //67.750 {  79873671} Rx - [A4, 0x14, 0x4E, 0x00, 0x10, 0x19, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x24, 0xE0, 0x89, 0xE0, 0x11, 0x05, 0x10, 0x01, 0x6A, 0x00, 0x0C, 0xF4, 0x32]
    */
#define EXPECTED_DEVICE_ID   0x89E0
static uint16_t _device_id = 0;

static void pm_info_handler(power_meter_info_t* p_power_meter_list, uint8_t size)
{
    if (size > 0)
        _device_id = p_power_meter_lislt[0]->ant_device_id;
}

static void test_bg_scanner_handle(CuTest* tc)
{
    uint8_t buffer[] = { 0x00, 0x4E, 0x10, 0x19, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x24, 0xE0, 0x89, 0xE0, 0x11, 0x05, 0x10, 0x01, 0x6A, 0x00, 0x0C, 0xF4, 0x32 };
    ant_evt_t * p_ant_evt = (ant_evt_t*)buffer;

    ant_bg_scanner_start(pm_info_handler);
    ant_bg_scanner_rx_handle(p_ant_evt);
    CuAssertTrue(tc, _device_id == EXPECTED_DEVICE_ID);
}

CuSuite* cu_getsuite_ant_bg_scanner()
{
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_bg_scanner_handle);
    return suite;
}
