/*
 *  Simple example of a CUnit unit test.
 *
 *  This program (crudely) demonstrates a very simple "black box"
 *  test of the standard library functions fprintf() and fread().
 *  It uses suite initialization and cleanup functions to open
 *  and close a common temporary file used by the test functions.
 *  The test functions then write to and read from the temporary
 *  file in the course of testing the library functions.
 *
 *  The 2 test functions are added to a single CUnit suite, and
 *  then run using the CUnit Basic interface.  The output of the
 *  program (on CUnit version 2.0-2) is:
 *
 *           CUnit : A Unit testing framework for C.
 *           http://cunit.sourceforge.net/
 *
 *       Suite: Suite_1
 *         Test: test of fprintf() ... passed
 *         Test: test of fread() ... passed
 *
 *       --Run Summary: Type      Total     Ran  Passed  Failed
 *                      suites        1       1     n/a       0
 *                      tests         2       2       2       0
 *                      asserts       5       5       5       0

TODO: convert this to make file, compiled on unbuntu for Windows using:
jason@DESKTOP-50IAGSN:~/dev/insideride/firmware/emotion_test$ gcc -o test ./libraries/*.c ../emotion/libraries/speed_event_fifo.c ../emotion/libraries/math/*.c  ../emotion/libraries/ctf_power.c ../emotion/libraries/ctf_offset.c -iquote ../emotion/libraries/ -iquote ../emotion/libraries/math/ -iquote ./include/ -iquote ../emotion/ -iquote ../emotion/services/ -iquote ../nrf51sdk_7.2/libraries/util/ -iquote ../nrf51sdk_7.2/drivers_nrf/hal/ -lcunit -lm
 */

#include <stdio.h>
#include "CUnit/Basic.h"

// Dangerous, but effective.  This allows implicit declaration of test suite functions
// so that we do not need to create/include header definitions for each test suite.
#define ADD_CU_SUITE(test_suite_name) {\
_Pragma ("GCC diagnostic ignored \"-Wimplicit-function-declaration\"") \
    test_suite_name(); \
}

void addTests()
{
  ADD_CU_SUITE(tests_ctf_power);
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{
    CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

    addTests();

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}
