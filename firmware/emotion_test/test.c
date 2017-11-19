#include "stdio.h"
#include <stdint.h>
#include "float.h"
#include "cutest/CuTest.h"
#include "app_util.h"

CuSuite* cu_getsuite_ctf_power();
CuSuite* cu_getsuite_magnet();
static CuString* output;

static float track_grade_get(uint8_t* buffer)
{
    // Simulated Grade (%) = (Raw Grade Value x 0.01%) – 200.00%
    uint16_t raw_grade = uint16_decode(buffer);
    float grade = (raw_grade * 0.01f) - 200.0f;    
    
    // Internally we store as 0.06 for example, so move the decimal. 
    return grade / 100.0f;
}

static void RunSuite(const char* name, CuSuite* suite)
{
    printf("Running %s\r\n:", name);   
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    CuSuiteRun(suite);
}

static void RunAllTests()
{
    output = CuStringNew();
    RunSuite("ctf_power", cu_getsuite_ctf_power());
    RunSuite("magnet", cu_getsuite_magnet());
    printf(output->buffer);

    // uint8_t buffer[] = { 0x2F, 0x4E };
    // float grade = track_grade_get(buffer);
    // printf("grade: %.6f\r\n", grade);

}

int main(char args[])
{
    RunAllTests();
}