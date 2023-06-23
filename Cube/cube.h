/**
 * @file cube.h
 * @author David  B (dsbradle)
 *
 * Provides an interface for modifying, storing and accessing cubes.
 *
 */
#ifndef _cube_h

#define _cube_h

#include <stddef.h>


/** Cosntant for the number of corner subcubes in a cube*/
#define NUM_CORNERS 8

/** Constant for the number of side subcubes in a cube*/
#define NUM_SIDES 12

/** Types of faces used for orienting cube*/
typedef enum faces
{
  BLANK = -1,
  WHITE = 0,
  BLUE = 1,
  ORANGE = 2,
  YELLOW = 3,
  GREEN = 4,
  RED = 5
} face;

/** Possible rotations of a side of the cube*/
typedef enum rotations
{
  NONE = 0,
  ROT_90 = 1,
  ROT_180 = 2,
  ROT_270 = 3
} rotation;

/**Different axes of for a*/
enum axis
{
  WY = 0,
  BG = 1,
  OR = 2
};

/** Size used for storage of a cube*/
extern const size_t CUBE_STORAGE_SIZE;

/** Size of the printed form of the cube*/
extern const size_t CUBE_PRINT_SIZE;

/**Struct used for storing cube*/
typedef struct cube Cube;


/**
 * Initializes a default cube.
 * @param cube the cube to initialize
 */
void cube_create(Cube* cube);

// destroys cube
/**
 * Destroys the cube, freeing its memory.
 * @param cube the cube to free
 */
void cube_free(Cube* cube);

/**
 *  Copies the cube into a new address.
 * @param original the cube to copy
 * @param copy the place to put the copy of the cube
 */
void cube_copy(const Cube* original, Cube* copy);

// With the new methods, generating a solved cube is very simple and not costly at all, but randomizing will likely take time.
/**
 * UNIMPLEMENTED
 * Creates a new cube and randomizes it.
 * @param cube the cube to randomize
 */
void cube_randomized(Cube* cube);

  // will require more work to visualize effectively, but should be doable
  /**
   * Prints the cube in a formatted fashion
   * @param cube  the cube to print
   */
void cube_print(const Cube* cube);

/**
 * Returns a string form of the cube's state
 * @param cube the cube to return the state of
 * @param dest the array to place the string in
 */
void cube_string(char dest[CUBE_PRINT_SIZE], const Cube* cube);

/**
 * returns a form that will be put into the heap/radix tree to store and compare for later
 * @param cube
 * @return char*
 */
char* cube_to_string(const Cube* cube);

// returns a new cube representing the input cube rotated on a specific face
/**
 * Creates a duplicate cube rotated on the specified side by the amount provided
 * @param cube the cube to rotate
 * @param newCube the pointer to where the new cube should be
 * @param side the side to rotate on
 * @param rot the amount to rotate
 */
void cube_rotate(const Cube* cube, Cube* newCube, const face side, const rotation rot);

/**
 * Compares cubes and returns an int representing if state1 is less (negative), equal (zero), or more (positive) than state2
 * @param state1 the first cube to compare
 * @param state2 the second cube to compare
 * @return int the difrerence between the cubes
 */
int cube_compare(const Cube* state1, const Cube* state2);

/**
 * Prints the cube's value and binary representation of each subcube
 * @param cube the cube to print
 */
void cube_print_vals(const Cube* cube);

/**
 * UNIMPLEMENTED
 * Returns a hash of the cube
 * @param cube  the cube to hash
 * @return size_t the hash of the cube
 */
size_t cube_hash(const Cube* cube);
#endif