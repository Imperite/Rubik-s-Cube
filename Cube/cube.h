#ifndef _cube_h
#define _cube_h

#include "cube_constants.c"

// generates solved cube state. Not needed, but useful for demostrating how everything's stored and rotations
Cube* init_Cube();
// destroys cube
void destroy_Cube(Cube* cube);
// With the new methods, generating a solved cube is very simple and not costly at all, but randomizing will likely take time.
Cube* randomize_Cube();
// will require more work to visualize effectively, but should be doable
void print_Cube(Cube* cube);
// returns form that will be put into the heap/radix tree to store and compare for later
char* to_String(Cube* cube);

Cube* rotate_Cube(Cube* cube, enum faces side, enum rotations rot);

int cmp_cubes(void* state1, void* state2);
#endif