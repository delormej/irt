#include "stdio.h"
#include <stdint.h>
#include "float.h"
#include "cutest/CuTest.h"
#include "app_util.h"

CuSuite* cu_getsuite_ctf_power();
CuSuite* cu_getsuite_magnet();
CuSuite* cu_getsuite_simulation();
static CuString* output;

static float track_grade_get(uint8_t* buffer)
{
    // Simulated Grade (%) = (Raw Grade Value x 0.01%) – 200.00%
    uint16_t raw_grade = uint16_decode(buffer);
    float grade = (raw_grade * 0.01f) - 200.0f;    
    
    // Internally we store as 0.06 for example, so move the decimal. 
    return grade / 100.0f;
}

static void RunAllTests()
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

    CuSuiteAddSuite(suite, cu_getsuite_ctf_power());
    CuSuiteAddSuite(suite, cu_getsuite_magnet());
    CuSuiteAddSuite(suite, cu_getsuite_simulation());

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);

    // uint8_t buffer[] = { 0x2F, 0x4E };
    // float grade = track_grade_get(buffer);
    // printf("grade: %.6f\r\n", grade);
}

int main(char args[])
{
    RunAllTests();
}