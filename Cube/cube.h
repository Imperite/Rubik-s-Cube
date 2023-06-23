#ifndef _cube_h

#define _cube_h

#include <stddef.h>
// #include "cube_constants.c"

typedef enum faces
{
  BLANK = -1,
  WHITE = 0,
  BLUE = 1,
  ORANGE = 2,
  YELLOW = 3,
  GREEN = 4,
  RED = 5
} face;

typedef enum rotations
{
  NONE = 0,
  ROT_90 = 1,
  ROT_180 = 2,
  ROT_270 = 3
} rotation;

extern size_t CUBE_STORAGE_SIZE;

typedef struct cube Cube;


// generates solved cube state. Not needed, but useful for demostrating how everything's stored and rotations
Cube* cube_create();

// destroys cube
void cube_destroy(Cube* cube);

Cube* cube_copy(const Cube* cube);

// With the new methods, generating a solved cube is very simple and not costly at all, but randomizing will likely take time.
Cube* cube_randomized();

// will require more work to visualize effectively, but should be doable
void cube_print(const Cube* cube);

char* cube_string(const Cube* cube);

// returns form that will be put into the heap/radix tree to store and compare for later
char* cube_to_string(const Cube* cube);

// returns a new cube representing the input cube rotated on a specific face
Cube* cube_rotate(const Cube* cube, const face side, const rotation rot);

// Compares 2 cubes; used for both sorting and equality. Takes in 2 Cubes, but needed to label them as void * here to allow use with Storage.
int cube_compare(const Cube* state1, const Cube* state2);

//prints the cubes value and the binary form
void cube_print_vals(const Cube* cube);

//returns a hash of the cube
size_t cube_hash(const Cube* cube);
#endif