#include <stdlib.h>

typedef struct cubeState
{
// pointer to the cube structure used
    void* cube;

    // used to store all previous states; will have variable length; will store face and rotation for each move, meaning max of 38 chars, but will let specific size be decided at creation.
    // char** prev_states;
    // used to store distance from original state
    short depth;
    // unsigned short prev_state_length;
} CubeState;

//duplicates everything but the cube, leaving it undefined
CubeState* duplicate(CubeState* original) {
    CubeState* new = malloc(sizeof(CubeState));
    new->depth = original->depth;
    // new->prev_state_length = original->prev_state_length;
    // new->prev_states = malloc(new->prev_state_length * sizeof(char));
    // for (size_t i = 0; i < new->prev_state_length; i++)
    // {
    //     new->prev_states[i] = original->prev_states[i];
    // }
}

void destroy_Cube_State(CubeState* state) {
    destroy_Cube(state->cube);
    // free(state->prev_states);
    free(state);
}