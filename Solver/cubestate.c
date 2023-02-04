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

CubeState* next_CubeState(CubeState* start, face side, rotation rot);
void destroy_CubeState(void* c1);
int cmp_CubeState(void* c1, void* c2);
int cmpWithDepth_CubeState(void* c1, void* c2);
void print_CubeState(void* cs);

void print_Change(Change* c);


int cmp_CubeState(void* c1, void* c2) {
    return cmp_cubes(((CubeState*)c1)->cube, ((CubeState*)c2)->cube);
}

int cmpWithDepth_CubeState(void* c1, void* c2) {
    int cubeCmp = cmp_CubeState(c1, c2);
    if (cubeCmp != 0)
        return cubeCmp;
    return ((CubeState*)c1)->depth >= ((CubeState*)c2)->depth;
}

void destroy_CubeState(void* c1) {
    CubeState* state = ((CubeState*)c1);
    destroy_Cube(state->cube);
    free(state->moves);
    free(state);
}

void print_CubeState(void* cs) {
    CubeState* cube = (CubeState*)cs;
    printf("Depth %d: ", (cube->depth));
    for (size_t i = 0; i < cube->depth; i++)
    {
        print_Change(&cube->moves[i]);
    }
    puts("");
    print_Cube(cube->cube);
}

//Gets the CubeState for a cube rotated along face by amount rot.
CubeState* next_CubeState(CubeState* start, face side, rotation rot) {
    CubeState* cs = malloc(sizeof(CubeState));
    cs->depth = start->depth + 1;
    cs->moves = calloc(cs->depth, sizeof(Change));
    for (size_t i = 0; i < start->depth; i++)
    {
        cs->moves[i] = start->moves[i];
    }
    cs->moves[cs->depth - 1].degree = rot;
    cs->moves[cs->depth - 1].face = side;
    cs->cube = rotate_Cube(start->cube, side, rot);

    return cs;

}

char* face_to_char = "Xwboygr";
char* rot_to_char = "X123";
void print_Change(Change* c) {
    printf("%c%c ", face_to_char[c->face + 1], rot_to_char[c->degree]);
}