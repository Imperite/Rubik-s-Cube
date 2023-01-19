#ifndef _cube_h
#define _cube_h

// generates solved cube state. Not needed, but useful for demostrating how everything's stored and rotations
void* init_Cube();

// destroys cube
void destroy_Cube(void* cube);

// With the new methods, generating a solved cube is very simple and not costly at all, but randomizing will likely take time.
void* randomize_Cube();

// will require more work to visualize effectively, but should be doable
void print_Cube(void* cube);

// returns form that will be put into the heap/radix tree to store and compare for later
char* to_String(void* cube);

// returns a new cube representing the input cube rotated on a specific face
void* rotate_Cube(void* cube, int side, int rot);

// Compares 2 cubes; used for both sorting and equality. Takes in 2 Cubes, but needed to label them as void * here to allow use with Storage.
int cmp_cubes(void* state1, void* state2);
#endif