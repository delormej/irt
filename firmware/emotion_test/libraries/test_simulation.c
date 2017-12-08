#include "cutest/CuTest.h"
#include "float.h"
#include "resistance.h"
#include "simulation.h"

static void test_simulation_watts(CuTest* tc)
{
    irt_resistance_state_t resistance_state;
    resistance_state.crr = DEFAULT_CRR;
    resistance_state.c = SIM_C;
    resistance_state.drafting_factor = SIM_DRAFTING_FACTOR;
    resistance_state.grade = 0.0f;

    float speed_mps = 8.8f;
    float weight_kg = 9100; //f;
    float watts = simulation_watts(speed_mps, weight_kg, &resistance_state);
    CuAssertTrue(tc, watts == 205.198761F);
}

CuSuite* cu_getsuite_simulation()
{
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_simulation_watts);
    return suite;
}
