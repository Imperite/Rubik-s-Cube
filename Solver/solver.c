#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../Cube/cube.h"
// #include "../Cube/cube_constants.c"
#include "../Storage/storage.h"
#include "../Storage/queue.h"
#include "cubestate.c"


char* face_to_string[7] = { "ERROR", "front", "top", "left", "back", "bottom", "right" };
char* rotation_to_string[4] = { "ERROR", "90", "180", "-90" };

void solve(Cube* initial_state);
bool check_state(CubeState* to_check, Storage storage, void* queue, Cube* solved);

// Solve acts as a wrapper function to print out the results of cube_solve. Creates a Storage and Queue in order to remember traversed nodes and boundary nodes.
// Currently only works in one thread, but programmatically is ready for multi-threading (though storage needs locks)
void solve(Cube* initial_state)
{
    Cube* solved = init_Cube();

    void* queue = init_Queue();
    Storage storage = init_Storage();

    CubeState* current = malloc(sizeof(CubeState));
    current->cube = initial_state;
    current->depth = 0;
    current->moves = NULL;
    insert_Storage(storage, current, cmp_CubeState);
    bool isSolved = check_state(current, storage, queue, solved);
    // printf("test %d", 1);

    while (!empty_Queue(queue) && !isSolved) {
        // forEach_Queue(queue, print_CubeState);
        current = pop_Queue(queue);
        // print_CubeState(current);
        isSolved = check_state(current, storage, queue, solved);
    }

    printf("solved: %d\n", isSolved);

    for (size_t i = 0; i < current->depth; ++i)
    {
        printf("do a %s %s degree turn\n", face_to_string[current->moves[i].face + 1], rotation_to_string[current->moves[i].degree]);
    }

    // forEach_Queue(queue, print_CubeState);
    // puts("");
    // print_Storage(storage, print_CubeState);

    // forEach_Queue(queue, print_CubeState);
    destroy_Queue(queue);

    forEach_Storage(storage, destroy_CubeState);
    destroy_Storage(storage);
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
            CubeState* new = next_CubeState(to_check, side, rot);
            if (contains_Storage(storage, new, cmpWithDepth_CubeState))
                continue;

            void* previous = replace_Storage(storage, new, cmp_CubeState);
            if (previous != NULL)
                destroy_CubeState(previous);
            push_Queue(queue, new);
        }
    }
    return false;
}
