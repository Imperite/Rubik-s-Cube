#ifndef cubestate_h
#define cubestate_h

#include "../Cube/cube.h"

//Represents an edge leading away from a node in the graph of Cube states, consisting of an side to rotate on and a rotation amount..
typedef struct change {
  size_t change_id;
} Change;

//Represents a path from the starting state, storing the current state and the moves to get from the initial to the current.
typedef struct cubestate {
  Cube* cube;
  Change* moves;
  size_t depth;
} CubeState;

extern char* id_to_str[];

//cubestate helper functions

CubeState* cube_state_next(CubeState* start, face side, rotation rot);
void cube_state_destroy(void* c1);
int cube_state_compare(const void* c1, const void* c2);
int cube_state_compare_with_depth(const void* c1, const void* c2);
void cube_state_print(void* cs);

void change_print(Change* c);
char* change_array_string(Change changes[], size_t length);


#endif