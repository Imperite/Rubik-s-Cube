
#include <stdio.h>
#include <string.h>
#include "test.h"

int test(Test_Function tests[], size_t numTests)
{
  int testsFailed = 0;
  for (size_t i = 0; i < numTests; i++) {
    char* c;
    bool result = tests[i](&c);
    if (!result) testsFailed++;
    printf("%s: %d\n", c, result);
  }
  return testsFailed;
}

void setTestName(char** namePlace, const char* desiredName)
{
  *namePlace = malloc(strlen(desiredName) * sizeof(char));
  strcpy(*namePlace, desiredName);
}