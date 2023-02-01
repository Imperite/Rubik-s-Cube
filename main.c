// Rubik's Cube Solver, written by David Bradley using Repl.it, CodeBlocks with MinGW, and Microsoft Visual Studio. Also drew on pseudocode from wikipedia.
// #include "solver.c"
#include "Cube/cube_constants.c"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Cube/cube.h"
#include "Storage/storage.h"


void rotate(char cube[7], face face, rotation rot) {
  size_t rots[4];
  switch (face)
  {
  case WHITE:
    rots[0] = 1;
    rots[1] = 2;
    rots[2] = 4;
    rots[3] = 5;
    break;
  case BLUE:
    rots[0] = 0;
    rots[1] = 5;
    rots[2] = 3;
    rots[3] = 2;
    break;
  case ORANGE:
    rots[0] = 0;
    rots[1] = 1;
    rots[2] = 3;
    rots[3] = 4;
    break;
  case YELLOW:
    rots[0] = 1;
    rots[1] = 5;
    rots[2] = 4;
    rots[3] = 2;
    break;
  case GREEN:
    rots[0] = 0;
    rots[1] = 2;
    rots[2] = 3;
    rots[3] = 5;
    break;
  case RED:
    rots[0] = 0;
    rots[1] = 4;
    rots[2] = 3;
    rots[3] = 1;
    break;
  default:
    return;
  }

  int start = (rot == ROT_90 ? 0 : 3);
  int step = (rot == ROT_90 ? 1 : -1);

  char temp = cube[rots[start]];
  // printf("\t%c\n", temp);
  for (size_t i = 0; i < 3; i++)
  {
    // printf("\t%d: %c -> %c\n", start, cube[rots[start]], cube[rots[(start + step) % 4]]);
    cube[rots[start]] = cube[rots[(start + step) % 4]];
    start = (start + step) % 4;
  }
  // printf("\t%c -> %c\n", cube[rots[start]], temp);
  cube[rots[start]] = temp;
  // printf("%s", cube);
}

typedef struct state {
  char value[7];
  size_t depth;
} State;

typedef struct lnode {
  struct lnode* next;
  State* value;
} LNode;

typedef struct queue {
  LNode* tail, * head;
} Queue;

void queue_print(Queue* q) {
  LNode* curr = q->head;
  while (curr != NULL) {
    printf("%s, ", curr->value);
    curr = curr->next;
  }
  puts("");
}


int cmp(void* c1, void* c2) {
  int result = strcmp(((State*)c1)->value, ((State*)c2)->value);
  // printf("\t cmp %s %s: %d\n", c1, c2, result);
  return result;
}

void print(void* s) {
  State* st = (State*)s;
  printf("%s %d", st->value, st->depth);
}

void print_path(void* s) {
  State* st = s;
  int depth = st->depth, count = -1;
  int changes[3];

  while (depth > 0) {
    count++;
    changes[count] = depth % 100;
    depth /= 100;
  }

  char* face[] = { "White", "Blue", "Orange" };
  char* rots[] = { "90", "180", "-90" };

  printf("%s: ", st->value);
  for (;count > -1; count--) {
    size_t f = changes[count] / 10, r = changes[count] % 10;
    printf("%s %s, ", face[f - 1], rots[r - 1]);
  }
  puts("");
}


int main()
{
  Cube* solved = init_Cube();
  // print_cube_vals(solved);

  print_Cube(solved);
  // Cube* new = rotate_Cube(solved, WHITE, ROT_90);
  // print_Cube(new);
  destroy_Cube(solved);
  // destroy_Cube(new);
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
    ArrayListPtr storage = storage_create();
    size_t iterations = 42;
    size_t length = 2;
    for(size_t i = 0; i < iterations; ++i)
    {
      char *rand_string = malloc(sizeof(char *) * length); //need to get there to be duplicates, so I made it
      for(size_t j = 0; j<length; ++j)
        rand_string[j] = color_to_char(WHITE + (rand() % 6));
        printf("size: %zu\n", storage->size);
      if(i > 0 && !storage_contains(storage->prev_states, storage->size, rand_string))
        storage_insert(storage, rand_string);
      else if (i == 0)
        storage_insert(storage, rand_string);
      else
      {
        printf("FOUND DUPLICATE\n");
        free(rand_string);
      }
    }

    storage_destroy(&storage);
  //*/
  return 0;
}
