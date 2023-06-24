#ifndef test_h
#define test_h

#include <stdbool.h>
#include <stdlib.h>


typedef bool (*Test_Function)(char** name);


/**
 * Runs the provided tests and returns how many have failed.
 * @param tests an array of test functions
 * @param numTests the number of tests to run
 * @return int the number of failing tests
 */
int test(Test_Function tests[], size_t numTests);

/**
 * Sets the test name for a test function.
 * @param namePlace The location that the name should be placed at
 * @param desiredName the name that should be set
 */
void setTestName(char** namePlace, const char* desiredName);

#endif