#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>


typedef struct array_list
{
    size_t size;
    size_t capacity;
    void** prev_states;
} ArrayList;
typedef ArrayList* ArrayListPtr;

// create
ArrayListPtr storage_create();
// destroy
void storage_destroy(ArrayListPtr list);
// add
void storage_insert(ArrayListPtr list, void* cube, int(*compare)(void*, void*));
// resize
void resize_Storage(ArrayListPtr list);
// contains; will use a binary search to find in list.
bool storage_contains(ArrayListPtr list, void* to_find, int(*compare)(void*, void*));

int storage_size(ArrayListPtr list);


ArrayListPtr storage_create()
{
    ArrayListPtr newList = malloc(sizeof(ArrayList));
    newList->size = 0;
    newList->capacity = 8;
    newList->prev_states = calloc(newList->capacity, sizeof(void*));
    return newList;
}

// this is new in that I free all the previous found states when destroying the arrayList -- I don't copy them over, which saves a small amount of time

void storage_destroy(ArrayListPtr list)
{
    // for (size_t i = 0; i < (*listPtr)->size; ++i)
    //     free((*listPtr)->prev_states[i]);
    free((list)->prev_states);
    free(list);
}

void storage_insert(ArrayListPtr list, void* cube, int(*compare)(void*, void*))
{
    resize_Storage(list);
    size_t i;

    for (i = list->size++; i > 0 && compare(cube, list->prev_states[i - 1]) < 0; --i)
        list->prev_states[i] = list->prev_states[i - 1];

    list->prev_states[i] = cube;
}

void resize_Storage(ArrayListPtr list)
{
    if (list->size == list->capacity)
    {
        list->capacity *= 2;
        list->prev_states = realloc(list->prev_states, list->capacity * sizeof(char*));
    }
}

bool storage_contains(ArrayListPtr list, void* to_find, int(*compare)(void*, void*))
{
    int left = 0;
    int right = list->size - 1;
    int mid;
    while (left <= right)
    {
        mid = (left + right) / 2;
        int comp = compare(to_find, list->prev_states[mid]);
        // printf("\t%d %d %d: %d from %s vs %s\n", left, mid, right, comp, to_find, list->prev_states[mid]);
        if (comp >= 1)
            left = mid + 1;
        else if (comp <= -1)
            right = mid - 1;
        else if (comp == 0)
        // if (comp == 0 && list->prev_states[mid][0] * 10 + list->prev_states[mid][1] < to_find[0] * 10 + to_find[1])
            return true;
    }
    return false;
}

void* storage_replace(ArrayListPtr list, void* obj, int(*compare)(void*, void*)) {
    int left = 0;
    int right = list->size - 1;
    int mid;
    while (left <= right)
    {
        mid = (left + right) / 2;
        int comp = compare(obj, list->prev_states[mid]);
        if (comp >= 1)
            left = mid + 1;
        else if (comp <= -1)
            right = mid - 1;
        else if (comp == 0) {
            void* toReturn = list->prev_states[mid];
            list->prev_states[mid] = obj;
            return toReturn;
        }
    }


    return NULL;
}

void storage_for_each(ArrayListPtr list, void(*function)(void*)) {
    for (size_t i = 0; i < list->size; i++)
    {
        function(list->prev_states[i]);
    }

}

void storage_print(ArrayListPtr list, void(*print)(void*)) {
    printf("[");
    storage_for_each(list, print);
    puts("]");

}

int storage_size(ArrayListPtr list) {
    return list->size;
}