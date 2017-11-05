
#include <stdint.h>
#include "cutest/CuTest.h"
#include "ctf_power.h"
#include "ctf_offset.h"

static void init_ctf_offset()
{
    // Send 10 calibration pages which sets the Offset to 590.
    static const uint8_t page[] = { 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, 0x02, 0x4E };
    for (int i = 0; i < 10; i++)
        ctf_set_calibration_page((ant_bp_ctf_calibration_t*)page);
}

static int init()
{
    init_ctf_offset();

    // Power must be calculated based on current message and a previous message.
    static const uint8_t previous_page[] = { 0x20, 0xD5, 0x00, 0xD5, 0xD6, 0x61, 0x79, 0xAE };
    static const uint8_t current_page[] = { 0x20, 0xD6, 0x00, 0xD5, 0xDC, 0x84, 0x7C, 0x36 };
    
    ctf_set_main_page((ant_bp_ctf_t*)previous_page);
    ctf_set_main_page((ant_bp_ctf_t*)current_page);
    
    return 0;
}

static void test_ctf_in_use(CuTest* tc)
{
    CuAssertTrue(tc, ctf_power_in_use());
}

static void test_ctf_get_average_power(CuTest* tc)
{
    // TODO: Add a bunch of additional power records, to create a 3??? second average??
    int16_t watts = ctf_get_average_power(3);
    CuAssertTrue(tc, watts == 89);
}

static void test_ctf_get_average_power_rollover(CuTest* tc)
{
    // This should test an event series where there is a rollover in one or more of
    // the data fields in the ctf main data page.
    CuAssertTrue(tc, 1==0);
}

static void test_ctf_get_power(CuTest* tc)
{
    int16_t watts;
    uint32_t err = ctf_get_power(&watts);
    CuAssertTrue(tc, err == CTF_SUCCESS && watts == 89);
}

static void test_ctf_get_offset(CuTest* tc)
{
    CuAssertTrue(tc, getCtfOffset()==590);
}

CuSuite* cu_getsuite_ctf_power()
{
    init();

    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_ctf_in_use);
    SUITE_ADD_TEST(suite, test_ctf_get_offset);
    SUITE_ADD_TEST(suite, test_ctf_get_power);
    SUITE_ADD_TEST(suite, test_ctf_get_average_power);
    SUITE_ADD_TEST(suite, test_ctf_get_average_power_rollover);

    return suite;
}