#include "stdio.h"
#include "cutest/CuTest.h"

CuSuite* cu_getsuite_ctf_power();

int main(char args[])
{
    CuSuite* suite = cu_getsuite_ctf_power();
    CuString* summary = CuStringNew();
    CuString* details = CuStringNew();
    CuSuiteSummary(suite, summary);
    CuSuiteDetails(suite, details);
    CuSuiteRun(suite);

    printf(summary->buffer);
    printf(details->buffer);
}