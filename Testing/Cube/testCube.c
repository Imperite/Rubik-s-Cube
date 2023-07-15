
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../test.h"

#include "../../Cube/cube.h"
#include "../../Cube/subcube.h"


bool testSubcube(char** name)
{
  setTestName(name, "subcube");

  // face colors[] = subuc

  return true;
}

int main()
{
  Test_Function tests[] = { testSubcube };
  Cube* c = malloc(sizeof(c));
  cube_create(c);
  int failed = test(tests, 1);
  printf("Failed: %d\n", failed);
  if (failed)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
