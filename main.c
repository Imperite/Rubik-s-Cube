// Rubik's Cube Solver, written by David Bradley using Repl.it, CodeBlocks with MinGW, and Microsoft Visual Studio. Also drew on pseudocode from wikipedia.
// #include "solver.c"
#include "Cube/cube_constants.c"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Storage/arraylist.c"


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

typedef struct lnode {
  struct lnode* next;
  char* value;
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
  int result = strcmp(c1, c2);
  // printf("\t cmp %s %s: %d\n", c1, c2, result);
  return result;
}


void print(void* s) {
  printf("%s", s);
}

int main()
{
  //GOAL: want to see how distant states are from the origin - thinking it should be within 3 moves, but want to double check
  //initial 'default' state, before any rotations done
  char subcube[7] = "wboYGR";
  ArrayListPtr list = storage_create();
  Queue* q = malloc(sizeof(Queue));
  LNode* start = malloc(sizeof(LNode));
  start->value = subcube;
  start->next = NULL;
  q->head = start;
  q->tail = start;
  storage_insert(list, subcube, cmp);

  while (q->head != NULL) {
    for (face f = WHITE; f <= ORANGE; f++) {
      for (rotation r = ROT_90; r <= 3; r += 2) {
        char* new = calloc(7, sizeof(char));
        strcpy(new, q->head->value);
        rotate(new, f, r);
        printf("%s on %d %d -> %s\n", q->head->value, f, r, new);
        storage_print(list, print);
        if (!storage_contains(list, new, cmp))
        {
          printf("new! %c", ' ');
          LNode* next = malloc(sizeof(LNode));
          next->next = NULL;
          next->value = new;
          q->tail->next = next;
          q->tail = next;

          storage_insert(list, new, cmp);
        }
        puts("\n");
      }
    }
    queue_print(q);
    // q->head = q->head->next;
    storage_print(list, print);
    printf("DONE ONE%c\n", '\n');
    LNode* oldHead = q->head;
    q->head = q->head->next;
    free(oldHead->value);
    free(oldHead);
  }

  free(q);
  storage_destroy(&list);

  // rotate(subcube, WHITE, ROT_90);

  // Cube* solved = init_Cube();
  // print_Cube(solved);
  // solved = rotate_Cube(solved, WHITE, ROT_90);
  // print_Cube(solved);
  // destroy_Cube(solved);
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
