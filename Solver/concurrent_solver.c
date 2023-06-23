
#include "../cube.h"
#include "../storage.h"
#include "../queue.h"
#include "cubestate.c"
#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>

char* face_to_string[7] = { "ERROR", "front", "top", "left", "back", "bottom", "right" };
char* rotation_to_string[4] = { "ERROR", "90", "180", "-90" };
size_t THREADS = 10;

typedef struct threadInfo {
    bool* isSolved;
    Storage storage;
    Queue queue;
    Cube* solved;
    size_t id;
} threadInfo;


void solve(Cube* initial_state);

CubeState* startThreads(bool* isSolved, Storage storage, Queue queue, CubeState* start, Cube* target);

void* thread_solve(void* info);

bool check_state(CubeState* to_check, Storage storage, void* queue, Cube* solved);

//returns true if the cube has been updated/inserted inside the storage, otherwise returns false.
void* solver_update_cubestate(Storage storage, void* new, void** loc);


void solve(Cube* initial_state) {
    Cube* solved = cube_create();

    Queue queue = queue_create();
    Storage storage = storage_create();

    CubeState* current = malloc(sizeof(CubeState));
    *current = (CubeState){
        .depth = 0,
        .moves = NULL
    };
    for (size_t i = 0; i < 20; i++)
    {
        current->cube[i] = (*initial_state)[i];
    }


    CubeState* finalState;

    storage_insert(storage, current, cube_state_compare);
    bool* isSolved = malloc(sizeof(bool));
    if (check_state(current, storage, queue, solved)) {
        finalState = current;
        *isSolved = true;
    }
    else {
        *isSolved = false;
        finalState = startThreads(isSolved, storage, queue, current, solved);

        printf("solved: %d\n", *isSolved);
    }

    for (size_t i = 0; *isSolved && i < finalState->depth; ++i) {
        printf("do a %s turn\n", id_to_str[finalState->moves[i].change_id]);
        // printf("do a %s %s degree turn\n", face_to_string[finalState->moves[i].face + 1], rotation_to_string[finalState->moves[i].degree]);
    }
    free(isSolved);

// free(finalState);
// free(result);
// puts("Queue:");
// queue_for_each(queue, cube_state_print);
// puts("\nStorage:");
// storage_print(storage, cube_state_print);

// printf("STORAGE SIZE: %d\n", storage_size(storage));
// printf("QUEUE SIZE: %d\n", queue_size(queue));

    queue_destroy(queue);

    storage_for_each(storage, cube_state_destroy);
    storage_destroy(storage);

    cube_destroy(solved);


}

CubeState* startThreads(bool* isSolved, Storage storage, Queue queue, CubeState* start, Cube* target) {
    pthread_t* threads = calloc(THREADS, sizeof(pthread_t));
    threadInfo* tinfos = calloc(THREADS, sizeof(threadInfo));

    for (size_t i = 0; i < THREADS; i++)
    {
        tinfos[i] = (threadInfo){
            isSolved,
            storage,
            queue,
            target,
            i
        };
        pthread_create(&threads[i], NULL, thread_solve, &tinfos[i]);
    }


//set up wait on isSolved
    CubeState* finalState = NULL;
    for (size_t i = 0; i < THREADS; i++)
    {
        if (finalState == NULL)
            pthread_join(threads[i], (void**)&finalState);
        else
            pthread_join(threads[i], NULL);
    }


    free(threads);
    free(tinfos);

    return finalState;

}

void* thread_solve(void* info) {
    threadInfo* tInfo = (threadInfo*)info;
    bool* isSolved = tInfo->isSolved;
    Queue queue = tInfo->queue;
    Storage storage = tInfo->storage;

    while (!queue_is_empty(queue) && !*isSolved) {
        CubeState* current = queue_pop(queue);
        printf("Thread %zu: depth %zu\n", tInfo->id, current->depth);
        if (check_state(current, storage, queue, tInfo->solved)) {
            *isSolved = true;
            return current;
        }
    }
    return NULL;
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
            CubeState* new = cube_state_next(to_check, side, rot);
            //shift the rotation to check the 180 rotation first
            // storage_print(storage, cube_state_print);
            CubeState* result = storage_do(storage, new, cube_state_compare, solver_update_cubestate);
            if (result == new)
                storage_insert(storage, new, cube_state_compare);
            if (result != NULL) { // try to add/modify existing version in storage
                queue_push(queue, result);
            }
            else //delete state since already checked
            {
                cube_state_destroy(new);
            }
        }
    }
    return false;
}


void* solver_update_cubestate(Storage storage, void* new, void** loc) {
    CubeState* newState = (CubeState*) new;

    CubeState** storage_loc = (CubeState**)loc;
    // if (storage_loc != NULL && (*storage_loc)->depth <= newState->depth) { // failure case: storage has this but has already traversed this state
    //     // printf("\treturning bc null or traversed\n");
    //     return false;
    // }

    if (storage_loc != NULL) {// Found match in storage
        // cube_state_destroy(*storage_loc);
        while (1) {
            size_t depth = (*storage_loc)->depth;
            if (depth <= newState->depth)
                return NULL;
            atomic_compare_exchange_strong(&(*storage_loc)->depth, &depth, newState->depth);
        }
        cube_state_destroy(newState);
        return *storage_loc;
        // **storage_loc = *newState;
    }
    else {
        // printf("\tinserting\n");
        // storage_insert(storage, newState, cube_state_compare);
        return newState;
    }
}