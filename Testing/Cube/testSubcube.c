
#include <stdbool.h>
#include <stdio.h>
#include "../test.h"
#include "../../Cube/subcube.h"

#define NUMTESTCUBES 7
Subcube wbo, ygr, wb, wo, w, h;
Subcube testCubes[NUMTESTCUBES];

const int coords[][3] = { {0,0,0},{2,2,2},{0,1,0},{1,0,0},{1,1,0}, {1,0,1}, {1,1,1} };
const cubeType type[] = { CORNER, CORNER, SIDE, SIDE, CENTER, CENTER, HOLLOW };

const face colors[][3] = {
    {WHITE, BLUE, ORANGE},
    {YELLOW, GREEN, RED},
    {WHITE, BLANK, ORANGE},
    {BLANK, BLUE, ORANGE},
    {WHITE, BLANK, BLANK},
    {BLANK, BLUE, BLANK},
    {BLANK, BLANK, BLANK}
};



bool testSubcubeType(char** name)
{
  setTestName(name, "subcube_type");
  printf("%s: ", *name);

  for (int i = 0; i < NUMTESTCUBES; i++) {
    printf("  %d", i);
    if (type[i] != subcube_type(coords[i][0], coords[i][1], coords[i][2])) {
      return false;
    }
  }
  return true;
}

bool validateColors(const face expected[3], face result[3])
{
  for (int i = 0; i < 3; i++) {
    if (expected[i] != result[i] && expected[i] != BLANK) {
      printf("\nColor %d does not match: expected %c, got %c", i, faceToChar(expected[i]), faceToChar(result[i]));
      return false;
    }
  }
  return true;
}


bool testSubcubeColors(char** name)
{
  setTestName(name, "subcube_colors");
  printf("%s:", *name);

  //exclude center and hollow cubes since they are invalid
  for (int i = 0; i < NUMTESTCUBES; i++) {
    printf("  %d", i);
    face newColors[3];
    subcube_colors(testCubes[i], type[i], newColors);

    if (!validateColors(colors[i], newColors))
      return false;
  }

  return true;

}

bool testSubcubeColorAlongAxis(char** name)
{

  setTestName(name, "subcube_color_along_axis");
  printf("%s:", *name);

  for (int i = 0; i < NUMTESTCUBES - 3; i++) {
    for (int axis = 0; axis < 3; axis++) {
      printf("  %d.%d", i, axis);
      // printf("Axis %d: ", axis);
      face color = subcube_color_along_axis(testCubes[i], axis, type[i]);
      if (colors[i][axis] != color) {
        printf("\n\t%d.%d does not match: expected %c, got %c", i, axis, faceToChar(colors[i][axis]), faceToChar(color));
        return false;
      }
    }

  }

  return true;

}

int main()
{
  for (int i = 0; i < NUMTESTCUBES; i++)
    testCubes[i] = subcube_default_at(coords[i][0], coords[i][1], coords[i][2]);


  Test_Function tests[] = { testSubcubeType, testSubcubeColors, testSubcubeColorAlongAxis };
  return test(tests, sizeof(tests) / sizeof(Test_Function));
}