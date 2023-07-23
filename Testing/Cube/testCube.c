
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

  return false;
}

bool testCreate(char** name)
{
  setTestName(name, "cube_create");
  return false;
}

bool testCopy(char** name)
{
  setTestName(name, "cube_copy");
  return false;
}

bool testString(char** name)
{
  setTestName(name, "cube_string");
  return false;
}

bool testRotate(char** name)
{
  setTestName(name, "cube_rotate");
  return false;
}

bool testCompare(char** name)
{
  setTestName(name, "cube_compare");
  return false;
}

//unimplemented; ski[]
bool testHash(char** name)
{
  setTestName(name, "cube_hash");
  return true;
}


int main()
{
  Test_Function tests[] = { testSubcube, testCreate, testCopy, testString, testRotate, testCompare, testHash };
  Cube* c = malloc(sizeof(c));
  cube_create(c);
  return test(tests, 7);
}
