#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "cube.h"
// Rubik's Cube Solver, written by David Bradley using Repl.it, CodeBlocks with MinGW, and Microsoft Visual Studio. Also drew on pseudocode from wikipedia.

// Side note: here are a couple of optimizations I plan to do after the final deadline:
/*
    1. Change all rubik's simulation functions to work on the compressed state instead of the struct. this will be even harder to understand, but will minimize the amount of space free/alloced at once (maybe also switch to using only chars, no ints? no need in compressed-only to keep color_to_char or color enums)
    2. Similarly, if I switch from a arraylist to a radix tree for storing the different checked states, it should work out much more effieciently in the long run. -- Prob will implement as a multi-nided tree, and either dynamically resize the child pointers or (more likely) just declare a 6-lebgth array?
    3. Rewrite the print_cube function; it's pretty repetitive right now, and could use with a little abstraction to make it work
    4. Find a way to determine if the route taken to get to this cube is the simplest/compress moves after finding the solution.
    5. AFter all that, I'd love to work on making a simple way of plugging in the current state of the cube -- you currently would have to manually encode it in, but I'd love to change that.
    6. Finally, I'd love to create a gui for this. so that instead of using text to communicate the solution, I can visually show someone what moves to do.
    7. also, Compressing the cube down from a char per side to 3 bytes per side would mean a reduction from 50 bytes per state to 30, so could be worth it if running into space issues (but adds a lot to time cost -- now have to isloate each face for operations, making rotations even harder?)
*/

/*
  Notation:
    XYZ
      X = C, S, or (W B O R G Y)
        C = Corner cube
        S = Side cube
        (W B O R G Y) = center cube; one face and constant, so can be used for orientation

      Y = 0-a
        Denotes the index of this cube on the face

      z = (w b o r g y) = (0 1 2 3 4 5)?
        Represents the directional face of each cube by the center facing the same direction

    zY
      z = (w b o r g y) = (0 1 2 3 4 5)?
        Represents a side of the cube
      Y = 0-7
        Represents what part of that side this block is: 0-3 are

  Normal Cube state: (original orientation, used in array-struct form)
    L1: C0 S0 C1    L2: S4 B1 S5    L3: C4 S8 C5
        S1 W0 S2        O2 -- R3        S9 Y5 Sa
        C2 S3 C3        S6 G4 S7        C6 Sb C7

  (NOT USED) Standard method of storing compressed: (just showing cubes, not specific faces)
            Center Cubes                  Side cubes
    C0 C1 C2 C3 C4 C5 C6 C7 C8 | S0 S1 S2 S3 S4 S5 S6 S7 S8 S9 Sa Sb | <DEPTH INT>

    Requires storing each cube as a set of 2-3 chars describing faces of cube; would need to factor in rotations in changing cubes, making it more challenging

  Expanded form: (shows faces -- set up for simple rotations?)
      White Corners |  Blue Corners   |  Orange Corners | Yellow Corners  |  Green Corners  |  Red Corners    |         < Group
    C0w C1w C3w C2w | C4b C5b C1b C0b | C4o C0o C2o C6o | C6y C7y C5y C4y | C2g C3g C7g C6g | C1r C5r C7r C3r | . . .   < Original Face
    w0  w1  w2  w3  | b0  b1  b2  b3  | o0  o1  o2  o3  | y0  y1  y2  y3  | g0  g1  g2  g3  | r0  r1  r2  r3  |         < New Terminology

      White Sides   |   Blue Sides    |   Orange Sides  |   Yellow Sides  |   Green Sides   |   Red Sides     |
    S0w S2w S3w S1w | S8b S5b S0b S4b | S4o S1o S6o S9o | Sby Say S8y S9y | S3g S7g Sbg S6g | S5r Sar S7r S2r | <DEPTH INT>
    w4  w5  w6  w7  | b4  b5  b6  b7  | o4  o5  o6  o7  | y4  y5  y6  y7  | g4 g5  g6  g7   | r4  r5  r6  r7  |

  Rotations:
    Needed:
      Must describe completely how to switch values of different positions
        Could use array as in prev version
        Possibility of finding formula?

    white rotates:
      90:
        white corners:
          w0 > w1 > w2 > w3 >
        white sides:
          w4 > w5 > w6 > w7 >
        adjacent corners:
          b2 > r3 > g0 > o1 >
          b3 > r0 > g1 > o2 >
        adjacent sides:
          b6 > r7 > g4 > o5 >

      180:
        white corners:
          w0 > w2 >
          w1 > w3 >
        white sides:
          w4 > w6 >
          w5 > w7 >
        adjacent corners:
          b2 > g0 >
          b3 > g1 >
          r3 > o1 >
          r0 > o2 >
        adjacent sides:
          b6 > g4 >
          o7 > r5 >
        Again, if I store white face side progression, there's a pattern that continues into a 270:

      270 (-90)
        white corners:
          w0 > w3 > w2 > w1 >
        white sides:
          w4 > w7 > w6 > w5 >
        adjacent corners:
          b2 > o1 > g0 > r3 >
          b3 > o2 > g1 > r0 >
        adjacent sides:
          b6 > o7 > g4 > r5 >
        index + 3 % 4, or index-1 % 4 works
      Algorithm: Rot white face mod 4 for corners and sides, shift index by 1/-1 for adj corners/sides
      Pattern:  b r g o = 1 5 4 2 = -2 -1
    blue rotates:
      90:
        blue corners:
          b0 > b1 > b2 > b3 >
        blue sides:
          b4 > b5 > b6 > b7 >
        adjacent corners:
          w0 > o0 > y2 > r0 >
          w1 > o1 > y3 > r1 >
        adjacent sides:
          w4 > o4 > y6 > r4 >

      180:
        blue corners:
          b0 > b2 >
          b1 > b3 >
        blue sides:
          b4 > b6 >
          b5 > b7 >
        adjacent corners:
          w0 > y2 >
          w1 > y3 >
          o0 > r0 >
          o1 > r1 >
        adjacent sides:
          w4 > y6 >
          o4 > r4 >

      270 (-90):
        blue corners:
          b0 > b3 > b2 > b1 >
        blue sides:
          b4 > b7 > b6 > b5 >
        adjacent corners:
          w0 > r0 > y2 > o0 >
          w1 > r1 > y3 > o1 >
        adjacent sides:
          w4 > r4 > y6 > o4 >

      Diff algorithm (face changes but position remains same, except for in yellow) than white face for adj. rots, same for face rots.
      Pattern: y r w o = 3 5 0 2 = +2 +1
    orange rotates:
      90:
        orange corners:
          o0 > o1 > o2 > o3 >
        orange sides:
          o4 > o5 > o6 > o7 >
        adjacent corners:
          w0 > g0 > y0 > b0 >
          w1 > g1 > y1 > b1 >
        adjacent sides:
          w7 > g7 > y7 > b7 >
      180:
        orange corners:
          o0 > o2 >
          o1 > o3 >
        orange sides:
          o4 > o6 >
          o5 > o7 >
        adjacent corners:
          w0 > y0 >
          g0 > b0 >
        adjacent sides:
          w7 > y7 >
          g7 > b7 >
      270:
        orange corners:
          o0 > o3 > o2 > o1 >
        orange sides:
          o4 > o7 > o6 > o5 >
        adjacent corners:
          w0 > b0 > y0 > g0 >
          w1 > b1 > y1 > g1 >
        adjacent sides:
          w7 > b7 > y7 > g7 >
      Truly simple rotation: All are just incrementing the index as they progress
      Pattern: w g y b = 0 4 3 1 = -2 -1

    yellow rotates:
      90:
        yellow corners:
          y0 > y1 > y2 > y3 >
        yellow sides:
          y4 > y5 > y6 > y7 >
        adjacent corners:
          b0 > o3 > g2 > r1 >
          b1 > o0 > g3 > r2 >
        adjacent sides:
          b4 > o7 > g6 > r5 >

      180:
        yellow corners:
          y0 > y2 >
          y1 > y3 >
        yellow sides:
          y4 > y6 >
          y5 > y7 >
        adjacent corners:
          b0 > g2 >
          b1 > g3 >
          o3 > r1 >
          o0 > r2 >
        adjacent sides:
          b4 > o6 >
          o7 > r5 >

      270 (-90):
        yellow corners:
          y0 > y3 > y2 > y1 >
        yellow sides:
          y4 > y7 > y6 > y5 >
        adjacent corners:
          b0 > r1 > g2 > o3 >
          b1 > r2 > g3 > o0 >
        adjacent sides:
          b4 > r5 > g6 > o7 >
      same as white, just with different base pattern:
      Pattern: b o g r = 1 2 4 5

  Should work above ^^

  Formula:
    90 degrees means  shift = 1, 180 degrees means shift = 2, 270 degrees means shift = 3 (or -1?)
    White/Yellow:
      store  patttern that represents the order of sides to access w/ lowest value first
        White:  4 2 1 5 = g o b r
        Yellow: 1 2 4 5 = b o g r
      face corners and sides:
        next_loc = prev_loc + shift % 4
      adj corners:
        next_loc_0 = prev_loc + shift % 4
        next_loc_1 = prev_loc - shift % 4
      adj sides:
        next_loc = prev_loc + shift % 4
    Blue/Green:
      store  patttern that represents the order of sides to access w/ lowest value first
        Blue:   3 5 0 2 = y r w o
        Green:  3 2 0 5 = y o w r
      face corners and sides:
        next_loc = prev_loc + shift % 4
      adj corners and sides:
        next_loc = (face =! yellow ? prev_loc : prev_loc + 2 )
    Orange/Red:
      store  patttern that represents the order of sides to access w/ lowest value first
        Orange: 0 4 3 1 w g y b
        Red:    0 1 3 4 w b y g
      face corners and sides:
        next_loc = prev_loc + shift % 4
      adj corners:
        next_loc = prev_loc
      adj sides:
        next_loc = prev_loc
*/

typedef struct change
{
  enum faces face;
  enum rotations degree;
} Change;

// generates solved cube state. Not needed, but useful for demostrating how everything's stored and rotations
Cube *init_Cube();
// destroys cube
void destroy_Cube(Cube *cube);
// With the new methods, generating a solved cube is very simple and not costly at all, but randomizing will likely take time.
Cube *randomize_Cube();
// will require more work to visualize effectively, but should be doable
void print_Cube(Cube *cube);
// returns form that will be put into the heap/radix tree to store and compare for later
char *to_String(Cube *cube);

Cube *rotate_Cube(Cube *cube, enum faces side, enum rotations rot);

// No need for isEqual anymore, as strcmp works just fine

// IF I HAVE TIME: look into changing base storage and rotations to be only string-based, not number-based
// actual Search function:
// use a tree with 18 children -- generate each of the children, then generate their children?
// create the first child, then recurse into it and repeat until level 20 has been reached or the cube is solved
// have pointer to previous node, so that once the solved state is found we can return the moves needed. Each index of the connected_states will store one specific move.
// could I store previous states to reduce time?
// to do so:
// need to compress (shrinks storage space from 1664 bits to 54) -- DONE
//  need a way to compare two rubik's Cube_States/compressed form of two rubik's cube_states -- DONE, use strcmp()
// need to write an arraylist to store and new vals and check if any old vals exist

// This'll allow me to store all the states efficiently, letting me do a depth-first search while keeping previous states.

// store a list of pointers to all created rubik's cube_states

typedef struct array_list
{
  size_t size;
  size_t capacity;
  char **prev_states;
} ArrayList;
typedef ArrayList *ArrayListPtr;

bool cmp_cubes(char *state1, char *state2);
// create
ArrayListPtr storage_list_create();
// destroy
void storage_list_destroy(ArrayListPtr *listPtr);
// add
void storage_list_insert(ArrayListPtr list, Cube *cube);
// resize
void storage_list_resize(ArrayListPtr list);
// contains; will use a binary search to find in list.
bool storage_list_contains(char **prev_states, size_t end, char *to_find);

// TODO: Delete; unused
typedef struct tree_node
{
  // stores all other nodes that are one rotation away from this current Cube_State
  struct tree_node *connected_states[18];
  // stores the index of the previous node pointer inside of the connected_states array
  size_t prev_index;

  Cube current;
} Tree_Node;
typedef Tree_Node *Tree_NodePtr;

// ACTUAL SOLVING FUNCTION:
void solve(Cube *initial_state, Cube *solved);
bool solve_cube(Change moves[20], Cube *initial_state, Cube *solved);

int main()
{
  Cube *solved = init_Cube();
  print_Cube(solved);

  destroy_Cube(solved);
  /*
    //Obviously, I don't have the power to test this function and see if it works fully in 15 min, but here, I've simulated the two main different types of circumstances that could occur, of 3:
      //a) the cube is some number of rotations away on the front face, in which case this the first and second examples prove that it works
      //b) the cube is some number of rotations away on a different face, in which case the third example demostrates that this works
      //c) the cube is not 1 move away, and so should be rotated again. this is proven by the 4th one, which is two different moves away from a solve.
    srand(42);
    Cube_State *solved = init_subCubes();

    //1st EXAMPLE:
    printf("\t");
    Cube_State *rotated = rotate(solved, FRONT, ROT_90);
    //right now, the cube has stored the previous move that it just did, so I need to remove that from it's memory:
    rotated->prev_change.degree = NONE;
    puts("\nTo solve a front 90 rotation:");
    solve(rotated, solved);

    puts("DONE");

    puts("Press Enter to Continue");
    char c;
    scanf("%c", &c);

    //2nd EXAMPLE:
    printf("\n\n\t");
    rotated = rotate(rotated, FRONT, ROT_90);
    rotated->prev_change.degree = NONE;
    puts("\n\nTo solve a cube rotated on front face 180:");
    solve(rotated, solved);


    puts("Press Enter to Continue");
    scanf("%c", &c);

    //3rd EXAMPLE:
    printf("\n\n\t");
    rotated = rotate(solved, BACK, ROT_90);
    rotated->prev_change.degree = NONE;
    puts("\n\nTo solve a cube rotated on Back face 90:");
    solve(rotated, solved);

    puts("Press Enter to Continue");
    scanf("%c", &c);

    //4th EXAMPLE:
    printf("\n\n\t");
    rotated = rotate(solved, FRONT, ROT_90);
    rotated = rotate(rotated, BACK, ROT_180);
    //right now, the cube has stored the previous move that it just did, so I need to remove that from it's memory:
    rotated->prev_change.degree = NONE;
    puts("\n\nTo solve a cube rotated on front face 90 and the back 180:");
    solve(rotated, solved);


    puts("Press Enter to Continue");
    scanf("%c", &c);
    //ARRAYLIST TESTING
    ArrayListPtr storage = storage_list_create();
    size_t iterations = 42;
    size_t length = 2;
    for(size_t i = 0; i < iterations; ++i)
    {
      char *rand_string = malloc(sizeof(char *) * length); //need to get there to be duplicates, so I made it
      for(size_t j = 0; j<length; ++j)
        rand_string[j] = color_to_char(WHITE + (rand() % 6));
        printf("size: %zu\n", storage->size);
      if(i > 0 && !storage_list_contains(storage->prev_states, storage->size, rand_string))
        storage_list_insert(storage, rand_string);
      else if (i == 0)
        storage_list_insert(storage, rand_string);
      else
      {
        printf("FOUND DUPLICATE\n");
        free(rand_string);
      }
    }

    storage_list_destroy(&storage);
  //*/
  return 0;
}

// Solve acts as a wrapper function to print out the results of cube_solve.
void solve(Cube *initial_state, Cube *solved)
{
  Change *moves = calloc(20, sizeof(Change));

  // first it checks is a FRONT 270 rotation, so the inverse would be a FRON 90 rotation
  bool solve = solve_cube(moves, initial_state, solved);
  printf("solved: %d\n", solve);

  for (size_t i = 0; i < 20 && moves[i].degree != NONE; ++i)
  {
    printf("do a ");
    switch (moves[i].face)
    {
    case WHITE:
      printf("front ");
      break;
    case YELLOW:
      printf("back ");
      break;
    case ORANGE:
      printf("left ");
      break;
    case RED:
      printf("right ");
      break;
    case BLUE:
      printf("top ");
      break;
    case GREEN:
      printf("bottom ");
      break;
    default:
      printf("ERROR");
    }

    switch (moves[i].degree)
    {
    case ROT_270:
      printf(" 270 degree turn");
      break;
    case ROT_180:
      printf(" 180 degree turn");
      break;
    case ROT_90:
      printf(" 90 degree turn");
      break;
    case NONE:
      printf(" ERROR");
      break;
    }
    puts("");
  }
  free(moves);
}

// ACTUAL SOLVING FUNCTION:
// This has a time complexity of roughly O(n(from writing into arraylist) * n(for performing operations on each state)), assuming n represents the number of different UNIQUE states the cube can be in. without the arraylist, this would instead be just all 18^20 states within 20 moves of the cube.
// time complexity of function is O(n^2)
// Space complexity of function is O(n), for n being the number of unique states. Each new state is stored, and takes up 50 bytes per state (down from 208 originally!)
bool solve_cube(Change moves[20], Cube *initial_state, Cube *solved)
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
  static ArrayListPtr checked_states = NULL;
  if (checked_states == NULL)
    checked_states = storage_list_create();

  static size_t depth = 0;
  printf("\ndepth: %zu", depth);

  // if this is the first level of the function, check if the original state is solved.
  if (depth == 0)
    if (cmp_cubes(initial_state->cube, solved->cube))
      return true;

  // go through all the possible rotations of the cube
  for (size_t face = WHITE; face <= RED; face += 8)
  {
    for (size_t rot = ROT_90; rot <= ROT_270; ++rot)
    {
      // 4-rot checks if this is the inverse of the previous rotation: 90(index 1) goes to 270(index 3), 180(2) goes to 180(2), and 270(3) goes to 90(1)
      if (initial_state->prev_states[initial_state->prev_state_length - 2] == (face / 8) + '0' && initial_state->prev_states[initial_state->prev_state_length - 1] == 4 - rot + '0')
        continue;

      // find the new move, and record it
      printf("\t");
      moves[depth].face = face;
      moves[depth].degree = rot;
      Cube *new = rotate_Cube(initial_state, face, rot);

      // check if this is solved -- if so, we'll reset our static variables and attach a marker signifying we found the end before reaching the max of 20 moves.
      if (cmp_cubes(new->cube, solved->cube))
      {
        // if this is a solved state:
        // set the next move to be null
        if (depth < 19)
          moves[depth + 1].degree = NONE;
        puts("Found solution!");
        destroy_Cube(new);
        if (checked_states->size > 0)
          storage_list_destroy(&checked_states);
        return true;
      }

      // otherwise, check if this state has already been found, and explored further along
      if (checked_states->size >= 1 && storage_list_contains(checked_states->prev_states, checked_states->size, new->cube))
      {
        printf("\n Already checked, skipping: ");
        destroy_Cube(new);
        continue;
      }

      // record this state so we can access it later
      storage_list_insert(checked_states, new);

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

bool cmp_cubes(char *state1, char *state2)
{
  for (size_t i = 0; i <= 48; ++i)
    if (state1[i] != state2[i])
      return false;
  return true;
}

// ARRAYLIST FUNCTIONS: most of this will be copied over from the array_list functions we wrote in class, but compare will be of my own design --anything new will be commented
ArrayListPtr storage_list_create()
{
  ArrayListPtr newList = malloc(sizeof(ArrayList));
  newList->size = 0;
  newList->capacity = 8;
  newList->prev_states = calloc(newList->capacity, sizeof(newList->prev_states));
  return newList;
}

// this is new in that I free all the previous found states when destroying the arrayList -- I don't copy them over, which saves a small amount of time
void storage_list_destroy(ArrayListPtr *listPtr)
{
  for (size_t i = 0; i < (*listPtr)->size; ++i)
    free((*listPtr)->prev_states[i]);
  free((*listPtr)->prev_states);
  free(*listPtr);
  *listPtr = NULL;
}

// insertion, sadly, is linear, and that's what slows down the program the most. Implementing this as a hash would have been better, but I don't have the time for that right now
void storage_list_insert(ArrayListPtr list, Cube *cube)
{
  // complete the char storage by updating the depth
  cube->cube[48] = cube->depth / 10 + '0';
  cube->cube[49] = cube->depth % 10 + '0';

  storage_list_resize(list);
  size_t i;

  for (i = list->size++; i > 0 && strcmp(cube->cube, list->prev_states[i - 1]) < 0; --i)
    list->prev_states[i] = list->prev_states[i - 1];

  list->prev_states[i] = cube->cube;
  printf("\n\tAdding ");
  puts(cube->cube);
}

void storage_list_resize(ArrayListPtr list)
{
  if (list->size == list->capacity)
  {
    puts("");
    list->capacity *= 2;
    list->prev_states = realloc(list->prev_states, list->capacity * sizeof(char *));
  }
}

// BASED ON PSEUDOCODE FROM WIKIPEDIA; was having trouble getting it to work recursively based on the code I made in class, so gave up and looked up how to do it on wikipedia.
// storage_list_contains() a binary search to find out if the current state was already found, and then returns true if we found this state through a longer path than before.
bool storage_list_contains(char **prev_states, size_t size, char *to_find)
{
  puts("");
  int left = 0;
  int right = size - 1;
  int mid;
  while (left <= right)
  {
    mid = (left + right) / 2;
    int compare = strcmp((to_find + 2), (prev_states[mid] + 2));
    if (compare == 1)
      left = mid + 1;
    else if (compare == -1)
      right = mid - 1;
    else if (compare == 0 && prev_states[mid][0] * 10 + prev_states[mid][1] < to_find[0] * 10 + to_find[1])
      return true;
  }
  return false;
}