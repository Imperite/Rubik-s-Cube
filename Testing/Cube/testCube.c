
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../test.h"

#include "../../Cube/cube.h"


bool test1(char** name)
{
  setTestName(name, "test 1");

  return true;
}

int main()
{
  Test_Function tests[] = { test1 };
  Cube* c = malloc(sizeof(c));
  cube_create(c);
  int failed = test(tests, 1);
  printf("Failed: %d\n", failed);
  if (failed)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
