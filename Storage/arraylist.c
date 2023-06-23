#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "storage.h"


/** Arraylist implementation*/
typedef struct storage
{
    size_t size;
    size_t capacity;
    Item* data;
} ArrayList;
typedef ArrayList* ArrayListPtr;

/**
 * Resizes the arraylist if needed, doubling its size.
 * @param list the list to possibly resize
 */
void storage_resize(ArrayListPtr list);


void storage_create(ArrayListPtr newList)
{
    *newList = (ArrayList){
        .size = 0,
        .capacity = 8,
        .data = calloc(8, sizeof(void*))
    };
}

//Destroys the arraylist. Assumes all values inside have already been destroyed, and so does not try to free them.
void storage_destroy(ArrayListPtr list)
{
    free((list)->data);
    free(list);
}

void storage_insert(ArrayListPtr list, Item obj, Comparator compare)
{
    storage_resize(list);
    size_t i;

    for (i = list->size++; i > 0 && compare(obj, list->data[i - 1]) < 0; --i)
        list->data[i] = list->data[i - 1];

    list->data[i] = obj;
}

void storage_resize(ArrayListPtr list)
{
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->data = realloc(list->data, list->capacity * sizeof(Item*));
    }
}

bool storage_contains(const ArrayListPtr list, const Item to_find, Comparator compare)
{
    return storage_location_of(list, to_find, compare) != NULL;
}

Item storage_replace(ArrayListPtr list, const Item obj, Comparator compare)
{
    Item* itemLoc = storage_location_of(list, obj, compare);
    if (itemLoc == NULL)
        return NULL;

    Item prevStored = *itemLoc;
    *itemLoc = obj;
    return prevStored;
}

Item* storage_location_of(const ArrayListPtr list, const Item obj, Comparator compare)
{
    int left = 0;
    int right = list->size - 1;
    int mid;
    while (left <= right) {
        mid = (left + right) / 2;
        int comp = compare(obj, list->data[mid]);
        if (comp >= 1)
            left = mid + 1;
        else if (comp <= -1)
            right = mid - 1;
        else if (comp == 0) {
            return (list->data + mid);
        }
    }
    return NULL;
}


Item storage_do(ArrayListPtr list, const Item obj, Comparator compare, Item(*do_on)(ArrayListPtr, const void*, Item*))
{
    return do_on(list, obj, storage_location_of(list, obj, compare));
}


void storage_for_each(ArrayListPtr list, Function function)
{
    for (size_t i = 0; i < list->size; i++) {
        function(list->data[i]);
    }

}

void storage_print(ArrayListPtr list, Function print)
{
    printf("[");
    storage_for_each(list, print);
    puts("]");

}

int storage_size(ArrayListPtr list)
{
    return list->size;
}