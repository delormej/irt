#include <stdio.h>
#include <stdint.h>
#include "cutest/CuTest.h"

CuSuite* cu_getsuite_ctf_power();
CuSuite* cu_getsuite_magnet();
CuSuite* cu_getsuite_simulation();
//CuSuite* cu_getsuite_ant_bg_scanner();
CuSuite* cu_getsuite_ant_fec();
static CuString* output;

static void RunAllTests()
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

    CuSuiteAddSuite(suite, cu_getsuite_ctf_power());
    CuSuiteAddSuite(suite, cu_getsuite_magnet());
    CuSuiteAddSuite(suite, cu_getsuite_simulation());
    //CuSuiteAddSuite(suite, cu_getsuite_ant_bg_scanner());
	CuSuiteAddSuite(suite, cu_getsuite_ant_fec());

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
}

int main(char args[])
{
    RunAllTests();
}