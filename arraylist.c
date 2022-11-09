#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "cube_state_constant.c"

typedef struct array_list
{
    size_t size;
    size_t capacity;
    CubeState** prev_states;
} ArrayList;
typedef ArrayList* ArrayListPtr;


ArrayListPtr storage_create();

void storage_destroy(ArrayListPtr* listPtr);

void storage_insert(ArrayListPtr list, CubeState* cube);

void storage_resize(ArrayListPtr list);
// contains; will use a binary search to find in list.
bool storage_contains(ArrayListPtr list, CubeState* to_find);



ArrayListPtr storage_create()
{
    ArrayListPtr newList = malloc(sizeof(ArrayList));
    newList->size = 0;
    newList->capacity = 8;
    newList->prev_states = calloc(newList->capacity, sizeof(newList->prev_states));
    return newList;
}

// this is new in that I free all the previous found states when destroying the arrayList -- I don't copy them over, which saves a small amount of time
void storage_destroy(ArrayListPtr* listPtr)
{
    for (size_t i = 0; i < (*listPtr)->size; ++i)
        destroy_Cube_State((*listPtr)->prev_states[i]);
    free((*listPtr)->prev_states);
    free(*listPtr);
    *listPtr = NULL;
}

// insertion, sadly, is linear, and that's what slows down the program the most. Implementing this as a hash would have been better, but I don't have the time for that right now
void storage_insert(ArrayListPtr list, CubeState* cube)
{
    storage_resize(list);
    size_t i;

    for (i = list->size++; i > 0 && strcmp(cube->cube, list->prev_states[i - 1]->cube) < 0; --i)
        list->prev_states[i] = list->prev_states[i - 1];
    list->prev_states[i] = cube;
    printf("\n\tAdding ");
    puts(*((Cube*)cube->cube));
}

void storage_resize(ArrayListPtr list)
{
    if (list->size == list->capacity)
    {
        // puts("");
        list->capacity *= 2;
        list->prev_states = realloc(list->prev_states, list->capacity * sizeof(char*));
    }
}

// storage_contains() a binary search to find out if the current state was already found, and then returns true if we found this state through a longer path than before.
bool storage_contains(ArrayListPtr list, CubeState* to_find)
{

    // puts("");
    int left = 0;
    int right = list->size - 1;
    int mid;
    while (left <= right)
    {
        mid = (left + right) / 2;
        int compare = strcmp(to_find->cube, list->prev_states[mid]->cube);
        printf("%zu %zu %zu, %d\n", left, mid, right, compare);
        if (compare >= 1)
            left = mid + 1;
        else if (compare <= -1)
            right = mid - 1;
        else if (compare == 0)
            if (list->prev_states[mid]->depth < to_find->depth)
                return true;
            else
                return false;
    }
    return false;
}