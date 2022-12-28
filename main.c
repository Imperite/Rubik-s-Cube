#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "Cube/cube.h"
// Rubik's Cube Solver, written by David Bradley using Repl.it, CodeBlocks with MinGW, and Microsoft Visual Studio. Also drew on pseudocode from wikipedia.

typedef struct change
{
  enum faces face;
  enum rotations degree;
} Change;

// generates solved cube state. Not needed, but useful for demostrating how everything's stored and rotations
Cube* init_Cube();
// destroys cube
void destroy_Cube(Cube* cube);
// With the new methods, generating a solved cube is very simple and not costly at all, but randomizing will likely take time.
Cube* randomize_Cube();
// will require more work to visualize effectively, but should be doable
void print_Cube(Cube* cube);
// returns form that will be put into the heap/radix tree to store and compare for later
char* to_String(Cube* cube);

Cube* rotate_Cube(Cube* cube, enum faces side, enum rotations rot);

typedef struct array_list
{
  size_t size;
  size_t capacity;
  char** prev_states;
} ArrayList;
typedef ArrayList* ArrayListPtr;

bool cmp_cubes(char* state1, char* state2);
// create
ArrayListPtr storage_list_create();
// destroy
void storage_list_destroy(ArrayListPtr* listPtr);
// add
void storage_list_insert(ArrayListPtr list, Cube* cube);
// resize
void storage_list_resize(ArrayListPtr list);
// contains; will use a binary search to find in list.
bool storage_list_contains(char** prev_states, size_t end, char* to_find);

// TODO: Delete; unused
typedef struct tree_node
{
  // stores all other nodes that are one rotation away from this current Cube_State
  struct tree_node* connected_states[18];
  // stores the index of the previous node pointer inside of the connected_states array
  size_t prev_index;

  Cube current;
} Tree_Node;
typedef Tree_Node* Tree_NodePtr;

// ACTUAL SOLVING FUNCTION:
void solve(Cube* initial_state, Cube* solved);
bool solve_cube(Change moves[20], Cube* initial_state, Cube* solved);

int main()
{
  Cube* solved = init_Cube();
  print_Cube(solved);
  solved = rotate_Cube(solved, WHITE, ROT_90);
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
void solve(Cube* initial_state, Cube* solved)
{
  Change* moves = calloc(20, sizeof(Change));

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
  static ArrayListPtr checked_states = NULL;
  if (checked_states == NULL)
    checked_states = storage_list_create();

  static size_t depth = 0;
  printf("\ndepth: %zu", depth);

  // if this is the first level of the function, check if the original state is solved.
  if (depth == 0)
    if (cmp_cubes(*initial_state, *solved))
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
      if (cmp_cubes(*new, *solved))
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
      if (checked_states->size >= 1 && storage_list_contains(checked_states->prev_states, checked_states->size, *new))
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

bool cmp_cubes(char* state1, char* state2)
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
void storage_list_destroy(ArrayListPtr* listPtr)
{
  for (size_t i = 0; i < (*listPtr)->size; ++i)
    free((*listPtr)->prev_states[i]);
  free((*listPtr)->prev_states);
  free(*listPtr);
  *listPtr = NULL;
}

// insertion, sadly, is linear, and that's what slows down the program the most. Implementing this as a hash would have been better, but I don't have the time for that right now
void storage_list_insert(ArrayListPtr list, Cube* cube)
{
  // complete the char storage by updating the depth
  // cube->cube[48] = cube->depth / 10 + '0';
  // cube->cube[49] = cube->depth % 10 + '0';

  storage_list_resize(list);
  size_t i;

  for (i = list->size++; i > 0 && strcmp(*cube, list->prev_states[i - 1]) < 0; --i)
    list->prev_states[i] = list->prev_states[i - 1];

  list->prev_states[i] = *cube;
  printf("\n\tAdding ");
  puts(*cube);
}

void storage_list_resize(ArrayListPtr list)
{
  if (list->size == list->capacity)
  {
    puts("");
    list->capacity *= 2;
    list->prev_states = realloc(list->prev_states, list->capacity * sizeof(char*));
  }
}

// BASED ON PSEUDOCODE FROM WIKIPEDIA; was having trouble getting it to work recursively based on the code I made in class, so gave up and looked up how to do it on wikipedia.
// storage_list_contains() a binary search to find out if the current state was already found, and then returns true if we found this state through a longer path than before.
bool storage_list_contains(char** prev_states, size_t size, char* to_find)
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