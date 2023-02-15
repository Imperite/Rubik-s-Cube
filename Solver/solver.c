#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../cube.h"
#include "../storage.h"
#include "../queue.h"
#include "cubestate.c"


char* face_to_string[7] = { "ERROR", "front", "top", "left", "back", "bottom", "right" };
char* rotation_to_string[4] = { "ERROR", "90", "180", "-90" };

void solve(Cube* initial_state);
bool check_state(CubeState* to_check, Storage storage, void* queue, Cube* solved);

//returns true if the cube has been updated/inserted inside the storage, otherwise returns false.
bool solver_update_cubestate(Storage storage, void* new, void** loc);


// Solve acts as a wrapper function to print out the results of cube_solve. Creates a Storage and Queue in order to remember traversed nodes and boundary nodes.
// Currently only works in one thread, but programmatically is ready for multi-threading (though storage needs locks)
void solve(Cube* initial_state)
{
    Cube* solved = cube_create();

    Queue queue = queue_create();
    Storage storage = storage_create();

    CubeState* current = malloc(sizeof(CubeState));
    current->cube = initial_state;
    current->depth = 0;
    current->moves = NULL;
    storage_insert(storage, current, cube_state_compare);
    bool isSolved = check_state(current, storage, queue, solved);

    while (!queue_is_empty(queue) && !isSolved) {
        current = queue_pop(queue);
        isSolved = check_state(current, storage, queue, solved);
    }

    printf("solved: %d\n", isSolved);
    // cube_state_print(current);
    for (size_t i = 0; i < current->depth; ++i) {
        printf("do a %s %s degree turn\n", face_to_string[current->moves[i].face + 1], rotation_to_string[current->moves[i].degree]);
    }

    // puts("Queue:");
    // queue_for_each(queue, cube_state_print);
    // puts("\nStorage:");
    // storage_print(storage, cube_state_print);

    printf("SIZE: %d\n", storage_size(storage));
    queue_for_each(queue, cube_state_destroy);
    queue_destroy(queue);

    storage_for_each(storage, cube_state_destroy);
    storage_destroy(storage);
    free(solved);
}

/*
Checks if this current state is the solved state. If not, adds all (non preveiously reached) states to the queue.
Checks all permutations from this state; if they are not in storage, adds them to queue.
Returns true if this is the solved state, and false otherwise.

Notably, this should be thread-safe, as long as the Storage and Queue used are also such.
*/
bool check_state(CubeState* to_check, Storage storage, Queue queue, Cube* solved) {
    if (cube_compare(to_check->cube, solved) == 0)
        return true;

    for (size_t side = WHITE; side <= RED; side++) {
        for (size_t rot = ROT_90;rot <= ROT_270; rot++) {
            CubeState* new = cube_state_next(to_check, side, (rot % 3) + 1); //shift the rotation to check the 180 rotation first
            // storage_print(storage, cube_state_print);

            if (storage_do(storage, new, cube_state_compare, solver_update_cubestate)) { // try to add/modify existing version in storage
                queue_push(queue, new);

            }
            else //delete state since already checked
            {
                cube_state_destroy(new);
            }
        }
    }
    return false;
}


bool solver_update_cubestate(Storage storage, void* new, void** loc) {
    CubeState* newState = (CubeState*) new;

    CubeState** storage_loc = (CubeState**)loc;
    if (storage_loc != NULL && (*storage_loc)->depth <= newState->depth) { // failure case: storage has this but has already traversed this state
        // printf("\treturning bc null or traversed\n");
        return false;
    }

    if (storage_loc != NULL) {// Found match in storage
        cube_state_destroy(*storage_loc);
        // printf("\treplacing\n");

        **storage_loc = *newState;
    }
    else {
        // printf("\tinserting\n");
        storage_insert(storage, newState, cube_state_compare);
    }
    return true;
}