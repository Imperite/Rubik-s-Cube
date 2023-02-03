#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../Cube/cube.h"
#include "../Cube/cube_constants.c"
#include "../Storage/storage.h"
#include "../Storage/queue.h"

typedef struct change
{
    face face;
    rotation degree;
} Change;

typedef struct cubestate {
    Cube* cube;
    Change* moves;
    size_t depth;
} CubeState;

char* face_to_string[7] = { "ERROR", "front", "top", "left", "back", "bottom", "right" };
char* rotation_to_string[4] = { "ERROR", "90", "180", "-90" };

// ACTUAL SOLVING FUNCTION:
void solve(Cube* initial_state);
bool check_state(CubeState* to_check, Storage storage, void* queue, Cube* solved);

//cubestate helper functions
CubeState* nextCubeState(CubeState* start, face side, rotation rot);
void destroyCubeState(void* c1);
int cmpCubeState(void* c1, void* c2);
int cmpCubeStateWithDepth(void* c1, void* c2);



// Solve acts as a wrapper function to print out the results of cube_solve.
void solve(Cube* initial_state)
{
    printf("test %d", 1);
    Cube* solved = init_Cube();

    void* queue = init_Queue();

    Storage storage = storage_create();

    CubeState* current = malloc(sizeof(CubeState));
    current->cube = initial_state;
    current->depth = 0;
    current->moves = NULL;
    storage_insert(storage, current, cmpCubeState);
    bool isSolved = check_state(current, storage, queue, solved);
    // printf("test %d", 2);

    while (!empty_Queue(queue)) {
        current = pop_Queue(queue);
        check_state(current, storage, queue, solved);
    }

    // first it checks is a FRONT 270 rotation, so the inverse would be a FRONT 90 rotation
    printf("solved: %d\n", isSolved);

    for (size_t i = 0; i < current->depth; ++i)
    {
        printf("do a %s %s degree turn\n", face_to_string[current->moves[i].face + 1], rotation_to_string[current->moves[i].degree]);
    }

    //TODO: something here causes segfault
    // for_each_Queue(queue, destroyCubeState);
    // destroy_Queue(queue);

    // storage_forEach(storage, destroyCubeState);
    // storage_destroy(storage);
}

int cmpCubeState(void* c1, void* c2) {
    return cmp_cubes(((CubeState*)c1)->cube, ((CubeState*)c2)->cube);
}

int cmpCubeStateWithDepth(void* c1, void* c2) {
    int cubeCmp = cmpCubeState(c1, c2);
    if (cubeCmp != 0)
        return cubeCmp;
    return ((CubeState*)c1)->depth >= ((CubeState*)c2)->depth;
}

void destroyCubeState(void* c1) {
    CubeState* state = ((CubeState*)c1);
    destroy_Cube(state->cube);
    free(state->moves);
    free(state);
}

CubeState* nextCubeState(CubeState* start, face side, rotation rot) {
    CubeState* cs = malloc(sizeof(CubeState));
    cs->depth = start->depth + 1;
    cs->moves = calloc(cs->depth, sizeof(Change));
    for (size_t i = 0; i < start->depth; i++)
    {
        cs->moves[i] = start->moves[i];
    }

    cs->cube = rotate_Cube(start->cube, side, rot);

    return cs;

}

/*
Checks if this current state is the solved state. If not, adds all (non preveiously reached) states to the queue.
Checks all permutations from this state; if they are not in storage, adds them to queue.
Returns true if this is the solved state, and false otherwise.

Notably, this should be thread-safe, as long as the Storage and Queue used are also such.
*/
bool check_state(CubeState* to_check, Storage storage, void* queue, Cube* solved) {
    if (cmp_cubes(to_check->cube, solved) == 0)
        return true;

    for (size_t side = WHITE; side <= RED; side++) {
        for (size_t rot = ROT_90;rot <= ROT_270; rot++) {
            CubeState* new = nextCubeState(to_check, side, rot);
            if (storage_contains(storage, new, cmpCubeStateWithDepth))
                continue;

            void* previous = storage_replace(storage, new, cmpCubeState);
            if (previous != NULL)
                destroyCubeState(previous);
            push_Queue(queue, new);
        }
    }
    return false;
}
