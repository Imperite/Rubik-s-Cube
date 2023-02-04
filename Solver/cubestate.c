#include "../Cube/cube_constants.c"
#include <stdlib.h>
#include <stdio.h>
//Represents an edge leading away from a node in the graph of Cube states, consisting of an side to rotate on and a rotation amount..
typedef struct change {
    face face;
    rotation degree;
} Change;

//Represents a path from the starting state, storing the current state and the moves to get from the initial to the current.
typedef struct cubestate {
    Cube* cube;
    Change* moves;
    size_t depth;
} CubeState;

//cubestate helper functions

CubeState* cube_state_next(CubeState* start, face side, rotation rot);
void cube_state_destroy(void* c1);
int cube_state_compare(void* c1, void* c2);
int cube_state_compare_with_depth(void* c1, void* c2);
void cube_state_print(void* cs);

void change_print(Change* c);


int cube_state_compare(void* c1, void* c2) {
    return cube_compare(((CubeState*)c1)->cube, ((CubeState*)c2)->cube);
}

int cube_state_compare_with_depth(void* c1, void* c2) {
    int cubeCmp = cube_state_compare(c1, c2);
    if (cubeCmp != 0)
        return cubeCmp;
    return ((CubeState*)c1)->depth >= ((CubeState*)c2)->depth;
}

void cube_state_destroy(void* c1) {
    CubeState* state = ((CubeState*)c1);
    cube_destroy(state->cube);
    free(state->moves);
    free(state);
}

void cube_state_print(void* cs) {
    CubeState* cube = (CubeState*)cs;
    printf("Depth %d: ", cube->depth);
    for (size_t i = 0; i < cube->depth; i++)
    {
        change_print(&cube->moves[i]);
    }
    puts("");
    cube_print(cube->cube);
}

//Gets the CubeState for a cube rotated along face by amount rot.
CubeState* cube_state_next(CubeState* start, face side, rotation rot) {
    CubeState* cs = malloc(sizeof(CubeState));
    cs->depth = start->depth + 1;
    cs->moves = calloc(cs->depth, sizeof(Change));
    for (size_t i = 0; i < start->depth; i++)
    {
        cs->moves[i] = start->moves[i];
    }
    cs->moves[cs->depth - 1].degree = rot;
    cs->moves[cs->depth - 1].face = side;
    cs->cube = cube_rotate(start->cube, side, rot);

    return cs;

}

char* face_to_char = "Xwboygr";
char* rot_to_char = "X123";
void change_print(Change* c) {
    printf("%d %d, ", c->face, c->degree);
    // printf("%c%c, ", face_to_char[c->face + 1], rot_to_char[c->degree]);
}