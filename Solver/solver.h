#ifndef solver_h
#define solver_h

#include "cubestate.h"

/**
 * Basic method for solving the cube. Prints out results to console; goes until it reaches the solved (default) state of the cube.
 * @param initial_state the starting state of the cube
 */
void solve(Cube* initial_state);

#endif