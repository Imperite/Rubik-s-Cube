// Rubik's Cube Solver, written by David Bradley using Repl.it, CodeBlocks with MinGW, and Microsoft Visual Studio. Also drew on pseudocode from wikipedia.
// #include "solver.c"
#include "Cube/cube_constants.c"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdatomic.h>

#include "cube.h"
#include "storage.h"
#include "solver.h"

void print_solved() {

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

  /*
  //1st EXAMPLE:
  Cube* rotated = cube_create();
  puts("1:To solve a solved cube:");
  solve(rotated);
  puts("DONE\n");
  // */


  //2nd EXAMPLE
  // solved = cube_create();
  rotated = cube_rotate(solved, WHITE, ROT_270);
  Cube* rotated2 = cube_rotate(rotated, RED, ROT_180);
  cube_destroy(rotated);
  rotated = cube_rotate(rotated2, GREEN, ROT_90);
  cube_destroy(rotated2);
  rotated2 = cube_rotate(rotated, RED, ROT_270);
  cube_destroy(rotated);
  // rotated = cube_rotate(rotated2, WHITE, ROT_180);
  // cube_destroy(rotated2);

  puts("2:To solve a front -90 rotation:");
  solve(rotated2);
  // cube_destroy(rotated);
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
  puts("DONE\n");

  //5th EXAMPLE:
  Cube* rotatedtmp = cube_rotate(solved, WHITE, ROT_90);
  rotated = cube_rotate(rotatedtmp, YELLOW, ROT_180);
  puts("5:To solve a cube rotated on front face 90 and the back 180:");
  cube_destroy(rotatedtmp);
  solve(rotated);
  puts("DONE\n");

  //6th EXAMPLE:
  Cube* rotatedtmp = cube_rotate(solved, WHITE, ROT_90);
  rotated = cube_rotate(rotatedtmp, ORANGE, ROT_180);
  puts("6:To solve a cube rotated on front face 90 and the left 180:");
  cube_destroy(rotatedtmp);
  solve(rotated);
  puts("DONE\n");
  //*/
  cube_destroy(solved);
  return 0;
}
