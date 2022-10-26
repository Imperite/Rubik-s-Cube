#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "cube.h"
#include "arraylist.c"

// Rubik's Cube Solver, written by David Bradley using Repl.it, CodeBlocks with MinGW, and Microsoft Visual Studio. Also drew on pseudocode from wikipedia.

typedef struct change
{
  enum faces face;
  enum rotations degree;
} Change;


// ACTUAL SOLVING FUNCTION:
void solve(CubeState* initial_state, CubeState* solved);
bool solve_cube(Change moves[20], CubeState* initial_state, CubeState* solved);
char* faceToWords(face face);
char* rotToWords(rotation rot);



int main()
{
  Cube* solved = init_Cube();
  print_Cube(solved);
  Cube* new = rotate_Cube(solved, WHITE, ROT_90);
  print_Cube(new);
  destroy_Cube(new);
  destroy_Cube(solved);
  /*
    //Obviously, I don't have the power to test this function and see if it works fully in 15 min, but here, I've simulated the two main different types of circumstances that could occur, of 3:
      //a) the cube is some number of rotations away on the front face, in which case this the first and second examples prove that it works
      //b) the cube is some number of rotations away on a different face, in which case the third example demostrates that this works
      //c) the cube is not 1 move away, and so should be rotated again. this is proven by the 4th one, which is two different moves away from a solve.
    srand(42);
    Cube_State *solved = init_subCubes();

    //1st EXAMPLE:
    printf("\t");
    Cube_State *rotated = rotate(solved, FRONT, ROT_90);
    //right now, the cube has stored the previous move that it just did, so I need to remove that from it's memory:
    rotated->prev_change.degree = NONE;
    puts("\nTo solve a front 90 rotation:");
    solve(rotated, solved);

    puts("DONE");

    puts("Press Enter to Continue");
    char c;
    scanf("%c", &c);

    //2nd EXAMPLE:
    printf("\n\n\t");
    rotated = rotate(rotated, FRONT, ROT_90);
    rotated->prev_change.degree = NONE;
    puts("\n\nTo solve a cube rotated on front face 180:");
    solve(rotated, solved);


    puts("Press Enter to Continue");
    scanf("%c", &c);

    //3rd EXAMPLE:
    printf("\n\n\t");
    rotated = rotate(solved, BACK, ROT_90);
    rotated->prev_change.degree = NONE;
    puts("\n\nTo solve a cube rotated on Back face 90:");
    solve(rotated, solved);

    puts("Press Enter to Continue");
    scanf("%c", &c);

    //4th EXAMPLE:
    printf("\n\n\t");
    rotated = rotate(solved, FRONT, ROT_90);
    rotated = rotate(rotated, BACK, ROT_180);
    //right now, the cube has stored the previous move that it just did, so I need to remove that from it's memory:
    rotated->prev_change.degree = NONE;
    puts("\n\nTo solve a cube rotated on front face 90 and the back 180:");
    solve(rotated, solved);


    puts("Press Enter to Continue");
    scanf("%c", &c);
    //ARRAYLIST TESTING
    ArrayListPtr storage = storage_create();
    size_t iterations = 42;
    size_t length = 2;
    for(size_t i = 0; i < iterations; ++i)
    {
      char *rand_string = malloc(sizeof(char *) * length); //need to get there to be duplicates, so I made it
      for(size_t j = 0; j<length; ++j)
        rand_string[j] = color_to_char(WHITE + (rand() % 6));
        printf("size: %zu\n", storage->size);
      if(i > 0 && !storage_contains(storage->prev_states, storage->size, rand_string))
        storage_insert(storage, rand_string);
      else if (i == 0)
        storage_insert(storage, rand_string);
      else
      {
        printf("FOUND DUPLICATE\n");
        free(rand_string);
      }
    }

    storage_destroy(&storage);
  //*/
  return 0;
}

// Solve acts as a wrapper function to print out the results of cube_solve.
void solve(CubeState* initial_state, CubeState* solved)
{
  Change* moves = calloc(20, sizeof(Change));

  bool solve = solve_cube(moves, initial_state, solved);
  printf("solved: %d\n", solve);

  for (size_t i = 0; i < 20 && moves[i].degree != NONE; ++i)
  {
    printf("Do a %s %s\n", faceToWords(moves[i].face), rotToWords(moves[i].degree));
  }
  free(moves);
}

// ACTUAL SOLVING FUNCTION:
// This has a time complexity of roughly O(n(from writing into arraylist) * n(for performing operations on each state)), assuming n represents the number of different UNIQUE states the cube can be in. without the arraylist, this would instead be just all 18^20 states within 20 moves of the cube.
// time complexity of function is O(n^2)
// Space complexity of function is O(n), for n being the number of unique states. Each new state is stored, and takes up 50 bytes per state (down from 208 originally!)
bool solve_cube(Change moves[20], CubeState* initial_state, CubeState* solved)
{
  /*
  plan:
    initialize storage space as static and not null
    use for loops to iterate through each rotation of cube, calling this function on it while not solved and less than 20 calls deep
      problem: how to store previous cube
        Solved: store previous change in each cube_state -- also gives way of telling if this is the original -- it'll be set to null
    we store the moves made so far in the moves array, and just overwrite previous as we elminate various leaves.
    all previous states are compressed down to the least amount of information possible and stored so we can check if we've encountered them before, while also recording the depth they were found at.
  */
  // checked_states keeps track of any states that we've checked before to reduce the number of states to check (so we're not checking duplicate states)
  static ArrayListPtr checked_states = NULL;
  if (checked_states == NULL)
    checked_states = storage_create();

  static size_t depth = 0;
  printf("\ndepth: %zu", depth);

  // if this is the first level of the function, check if the original state is solved.
  if (depth == 0)
    if (strcmp(initial_state->cube, solved->cube) == 0)
      return true;

  // go through all the possible rotations of the cube
  for (size_t face = WHITE; face <= RED; face += 8)
  {
    for (size_t rot = ROT_90; rot <= ROT_270; ++rot)
    {
      // 4-rot checks if this is the inverse of the previous rotation: 90(index 1) goes to 270(index 3), 180(2) goes to 180(2), and 270(3) goes to 90(1)
      // if (initial_state->prev_states[initial_state->prev_state_length - 2] == (face / 8) + '0' && initial_state->prev_states[initial_state->prev_state_length - 1] == 4 - rot + '0')
      //   continue;

      // find the new move, and record it
      printf("\t");
      moves[depth].face = face;
      moves[depth].degree = rot;
      CubeState* new = duplicate(initial_state);
      new->cube = rotate_Cube(initial_state->cube, face, rot);

      // check if this is solved -- if so, we'll reset our static variables and attach a marker signifying we found the end before reaching the max of 20 moves.
      if (strcmp(new->cube, solved->cube) == 0)
      {
        // if this is a solved state:
        // set the next move to be null
        if (depth < 19)
          moves[depth + 1].degree = NONE;
        puts("Found solution!");
        destroy_Cube(new->cube);
        if (checked_states->size > 0)
          storage_destroy(&checked_states);
        return true;
      }

      // otherwise, check if this state has already been found, and explored further along
      if (checked_states->size >= 1 && storage_contains(checked_states, new))
      {
        printf("\n Already checked, skipping: ");
        //TODO: re-implement for cubeState instead of just cube; otherwise will cause memory leak
        destroy_Cube(new->cube);
        continue;
      }

      // record this state so we can access it later
      storage_insert(checked_states, new);

      // check the rotations from this state and see if they would solve it, as long as the depth is less or equal to the max moves of 20.
      // if this isn't the final layer
      if (depth < 20)
      {
        ++depth;
        // if there was a solution found here, return it
        if (solve_cube(moves, new, solved))
        {
          destroy_Cube(new->cube);
          --depth;
          return true;
        }
        --depth;
      }
      destroy_Cube(new->cube);
    }
  }

  return false;
}


char* faceToWords(face face) {
  switch (face)
  {
  case WHITE:
    return "front ";
  case YELLOW:
    return "back ";
  case ORANGE:
    return "left ";
  case RED:
    return "right ";
  case BLUE:
    return "top ";
  case GREEN:
    return "bottom ";
  default:
    return "ERROR ";
  }
}

char* rotToWords(rotation rot) {
  switch (rot)
  {
  case ROT_90:
    return "90 degree turn ";
  case ROT_180:
    return "180 degree turn ";
  case ROT_270:
    return "270 degree turn ";
  default:
    return "ERROR ";
  }
}