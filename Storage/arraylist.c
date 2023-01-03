#include <stdlib.h>
#include <stdbool.h>


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
void storage_destroy(ArrayListPtr* listPtr);
// add
void storage_insert(ArrayListPtr list, void* cube, int(*compare)(void*, void*));
// resize
void storage_resize(ArrayListPtr list);
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
void storage_destroy(ArrayListPtr* listPtr)
{
    for (size_t i = 0; i < (*listPtr)->size; ++i)
        free((*listPtr)->prev_states[i]);
    free((*listPtr)->prev_states);
    free(*listPtr);
    *listPtr = NULL;
}

void storage_insert(ArrayListPtr list, void* cube, int(*compare)(void*, void*))
{
    storage_resize(list);
    size_t i;

    for (i = list->size++; i > 0 && compare(cube, list->prev_states[i - 1]) < 0; --i)
        list->prev_states[i] = list->prev_states[i - 1];

    list->prev_states[i] = cube;
}

void storage_resize(ArrayListPtr list)
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
        if (comp >= 1)
            left = mid + 1;
        else if (comp <= -1)
            right = mid - 1;
        else
        // if (comp == 0 && list->prev_states[mid][0] * 10 + list->prev_states[mid][1] < to_find[0] * 10 + to_find[1])
            return true;
    }
    return false;
}

int storage_size(ArrayListPtr list) {
    return list->size;
}