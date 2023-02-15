// Rubik's Cube Solver, written by David Bradley using Repl.it, CodeBlocks with MinGW, and Microsoft Visual Studio. Also drew on pseudocode from wikipedia.
// #include "solver.c"
#include "Cube/cube_constants.c"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cube.h"
#include "storage.h"
#include "solver.h"


void rotate(char cube[7], face face, rotation rot) {
  size_t rots[4];
  switch (face)
  {
  case WHITE:
    rots[0] = 1;
    rots[1] = 2;
    rots[2] = 4;
    rots[3] = 5;
    break;
  case BLUE:
    rots[0] = 0;
    rots[1] = 5;
    rots[2] = 3;
    rots[3] = 2;
    break;
  case ORANGE:
    rots[0] = 0;
    rots[1] = 1;
    rots[2] = 3;
    rots[3] = 4;
    break;
  case YELLOW:
    rots[0] = 1;
    rots[1] = 5;
    rots[2] = 4;
    rots[3] = 2;
    break;
  case GREEN:
    rots[0] = 0;
    rots[1] = 2;
    rots[2] = 3;
    rots[3] = 5;
    break;
  case RED:
    rots[0] = 0;
    rots[1] = 4;
    rots[2] = 3;
    rots[3] = 1;
    break;
  default:
    return;
  }

  int start = (rot == ROT_90 ? 0 : 3);
  int step = (rot == ROT_90 ? 1 : -1);

  char temp = cube[rots[start]];
  // printf("\t%c\n", temp);
  for (size_t i = 0; i < 3; i++)
  {
    // printf("\t%d: %c -> %c\n", start, cube[rots[start]], cube[rots[(start + step) % 4]]);
    cube[rots[start]] = cube[rots[(start + step) % 4]];
    start = (start + step) % 4;
  }
  // printf("\t%c -> %c\n", cube[rots[start]], temp);
  cube[rots[start]] = temp;
  // printf("%s", cube);
}

typedef struct state {
  char value[7];
  size_t depth;
} State;


void print(void* s) {
  State* st = (State*)s;
  printf("%s %zu", st->value, st->depth);
}

void print_path(void* s) {
  State* st = s;
  int depth = st->depth, count = -1;
  int changes[3];

  while (depth > 0) {
    count++;
    changes[count] = depth % 100;
    depth /= 100;
  }

  char* face[] = { "White", "Blue", "Orange" };
  char* rots[] = { "90", "180", "-90" };

  printf("%s: ", st->value);
  for (;count > -1; count--) {
    size_t f = changes[count] / 10, r = changes[count] % 10;
    printf("%s %s, ", face[f - 1], rots[r - 1]);
  }
  puts("");
}


int main()
{
  // Cube* solved = cube_create();
  // cube_print_vals(solved);

  // cube_print(solved);
  // Cube* new = cube_rotate(solved, WHITE, ROT_90);
  // cube_print(new);
  // cube_destroy(solved);
  // cube_destroy(new);
  // /*
    //Obviously, I don't have the power to test this function and see if it works fully in 15 min, but here, I've simulated the two main different types of circumstances that could occur, of 3:
      //a) the cube is some number of rotations away on the front face, in which case this the first and second examples prove that it works
      //b) the cube is some number of rotations away on a different face, in which case the third example demostrates that this works
      //c) the cube is not 1 move away, and so should be rotated again. this is proven by the 4th one, which is two different moves away from a solve.
  srand(42);
  Cube* solved = cube_create();
  Cube* rotated;

  //1st EXAMPLE:
  puts("1:To solve a solved cube:");
  solve(solved);
  puts("DONE\n");

  //2nd EXAMPLE
  rotated = cube_rotate(solved, WHITE, ROT_270);
  puts("2:To solve a front -90 rotation:");
  solve(rotated);
  cube_destroy(rotated);
  puts("DONE\n");

/*
  //3rd EXAMPLE:
  rotated = cube_rotate(solved, WHITE, ROT_90);
  puts("3:To solve a cube rotated on front face 90:");
  solve(rotated);
  cube_destroy(rotated);
  puts("DONE\n");

  //4th EXAMPLE:
  rotated = cube_rotate(solved, YELLOW, ROT_270);
  puts("4:To solve a cube rotated on Back face 270:");
  solve(rotated);
  cube_destroy(rotated);
  puts("DONE\n");

  //5th EXAMPLE:
  Cube* rotatedtmp = cube_rotate(solved, WHITE, ROT_90);
  rotated = cube_rotate(rotatedtmp, YELLOW, ROT_180);
  puts("5:To solve a cube rotated on front face 90 and the back 180:");
  cube_destroy(rotatedtmp);
  solve(rotated);
  cube_destroy(rotated);
  puts("DONE\n");

  //6th EXAMPLE:
  rotatedtmp = cube_rotate(solved, WHITE, ROT_90);
  rotated = cube_rotate(rotatedtmp, ORANGE, ROT_180);
  puts("6:To solve a cube rotated on front face 90 and the left 180:");
  cube_destroy(rotatedtmp);
  solve(rotated);
  cube_destroy(rotated);
  puts("DONE\n");
  //*/
  cube_destroy(solved);
  return 0;
}
