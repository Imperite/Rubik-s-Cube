#include <stddef.h>
#ifndef _cube_h
#define _cube_h

// generates solved cube state. Not needed, but useful for demostrating how everything's stored and rotations
void* cube_create();

// destroys cube
void cube_destroy(void* cube);

// With the new methods, generating a solved cube is very simple and not costly at all, but randomizing will likely take time.
void* cube_randomized();

// will require more work to visualize effectively, but should be doable
void cube_print(void* cube);

char* cube_string(void* cube);

// returns form that will be put into the heap/radix tree to store and compare for later
char* cube_to_string(void* cube);

// returns a new cube representing the input cube rotated on a specific face
void* cube_rotate(void* cube, int side, int rot);

// Compares 2 cubes; used for both sorting and equality. Takes in 2 Cubes, but needed to label them as void * here to allow use with Storage.
int cube_compare(void* state1, void* state2);

//prints the cubes value and the binary form
void cube_print_vals(void* cube);

//returns a hash of the cube
size_t cube_hash(void* cube);
#endif