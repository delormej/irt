#include "irt_common.h"
#include "cutest/CuTest.h"
#include "magnet.h"
#define NULL '\0'

static mag_calibration_factors_t default_factors;

static void test_get_default_factors(CuTest* tc)
{
    default_factors = magnet_get_default_factors();
    magnet_init(&default_factors);
    CuAssertTrue(tc, default_factors.low_factors[3] == -645.523540742f);
}

static void test_magnet_watts(CuTest* tc)
{
    //float speed_mps, uint16_t position
    float watts = magnet_watts(7.0f, 1600);
    CuAssertTrue(tc, watts == 28.4103069F );
}

static void test_magnet_position(CuTest* tc)
{
    target_power_e target;
    uint16_t position = magnet_position(7.0F, 28.4F, &target);
    CuAssertTrue(tc, position == 1325);
    //CuAssertTrue(target == TARGET_AT_POWER);
}

CuSuite* cu_getsuite_magnet()
{
    // init();
    
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_get_default_factors);
    SUITE_ADD_TEST(suite, test_magnet_watts);
    SUITE_ADD_TEST(suite, test_magnet_position);
}