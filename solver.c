#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "Cube/cube.h"
#include "Storage/storage.h"

typedef struct change
{
    enum faces face;
    enum rotations degree;
} Change;

char* face_to_string[7] = { "ERROR", "front", "top", "left", "back", "bottom", "right" };
char* rotation_to_string[4] = { "ERROR", "90", "180", "-90" };

// ACTUAL SOLVING FUNCTION:
void solve(Cube* initial_state, Cube* solved);
bool solve_cube(Change moves[20], Cube* initial_state, Cube* solved);

// Solve acts as a wrapper function to print out the results of cube_solve.
void solve(Cube* initial_state, Cube* solved)
{
    Change* moves = calloc(20, sizeof(Change));

    // first it checks is a FRONT 270 rotation, so the inverse would be a FRON 90 rotation
    bool solve = solve_cube(moves, initial_state, solved);
    printf("solved: %d\n", solve);

    for (size_t i = 0; i < 20 && moves[i].degree != NONE; ++i)
    {
        printf("do a %s %s degree turn\n", face_to_string[moves[i].face + 1], rotation_to_string[moves[i].degree]);
    }
    free(moves);
}

// ACTUAL SOLVING FUNCTION:
// This has a time complexity of roughly O(n(from writing into arraylist) * n(for performing operations on each state)), assuming n represents the number of different UNIQUE states the cube can be in. without the arraylist, this would instead be just all 18^20 states within 20 moves of the cube.
// time complexity of function is O(n^2)
// Space complexity of function is O(n), for n being the number of unique states. Each new state is stored, and takes up 50 bytes per state (down from 208 originally!)
bool solve_cube(Change moves[20], Cube* initial_state, Cube* solved)
{
  /*
  plan:
    initialize storage space as static and not null
    use for loops to iterate through each rotation of cube, calling this function on it while not solved and less than 20 calls deep
      problem: how to store previous cube
        Solved: store previous change in each cube_state -- also gives way of telling if this is the original -- it'll be set to null
    we store the moves made so far in the moves array, and just overwrite previous as we elminate various leaves.
    all previous states are compressed down to the least amount of information possible and stored so we can check if we've encountered them before, while also recording the depth they were found at.
  */
  // checked_states keeps track of any states that we've checked before to reduce the number of states to check (so we're not checking duplicate states)
    static Storage checked_states = NULL;
    if (checked_states == NULL)
        checked_states = storage_create();

    static size_t depth = 0;
    printf("\ndepth: %zu", depth);

    // if this is the first level of the function, check if the original state is solved.
    if (depth == 0)
        if (cmp_cubes(initial_state, solved))
            return true;

        // go through all the possible rotations of the cube
    for (size_t face = WHITE; face <= RED; face += 8)
    {
        for (size_t rot = ROT_90; rot <= ROT_270; ++rot)
        {
          // 4-rot checks if this is the inverse of the previous rotation: 90(index 1) goes to 270(index 3), 180(2) goes to 180(2), and 270(3) goes to 90(1)
          // if (initial_state->prev_states[initial_state->prev_state_length - 2] == (face / 8) + '0' && initial_state->prev_states[initial_state->prev_state_length - 1] == 4 - rot + '0')
          //   continue;

          // find the new move, and record it
            printf("\t");
            moves[depth].face = face;
            moves[depth].degree = rot;
            Cube* new = rotate_Cube(initial_state, face, rot);

            // check if this is solved -- if so, we'll reset our static variables and attach a marker signifying we found the end before reaching the max of 20 moves.
            if (cmp_cubes(new, solved))
            {
              // if this is a solved state:
              // set the next move to be null
                if (depth < 19)
                    moves[depth + 1].degree = NONE;
                puts("Found solution!");
                destroy_Cube(new);
                if (storage_size(checked_states) > 0)
                    storage_destroy(&checked_states);
                return true;
            }

            // otherwise, check if this state has already been found, and explored further along
            if (storage_size(checked_states) >= 1 && storage_contains(checked_states, *new, cmp_cubes))
            {
                printf("\n Already checked, skipping: ");
                destroy_Cube(new);
                continue;
            }

            // record this state so we can access it later
            storage_insert(checked_states, new, cmp_cubes);

            // check the rotations from this state and see if they would solve it, as long as the depth is less or equal to the max moves of 20.
            // if this isn't the final layer
            if (depth < 20)
            {
                ++depth;
                // if there was a solution found here, return it
                if (solve_cube(moves, new, solved))
                {
                    destroy_Cube(new);
                    --depth;
                    return true;
                }
                --depth;
            }
            destroy_Cube(new);
        }
    }

    return false;
}