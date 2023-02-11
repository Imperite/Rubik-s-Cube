#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>


typedef void Item;

typedef struct array_list
{
    size_t size;
    size_t capacity;
    Item** data;
} ArrayList;
typedef ArrayList* ArrayListPtr;

// create
ArrayListPtr storage_create();
// destroy
void storage_destroy(ArrayListPtr list);
// add
void storage_insert(ArrayListPtr list, Item* obj, int(*compare)(Item*, Item*));
// resize
void resize_Storage(ArrayListPtr list);
// contains; will use a binary search to find in list.
bool storage_contains(ArrayListPtr list, Item* to_find, int(*compare)(Item*, Item*));

Item** storage_location_of(ArrayListPtr list, Item* obj, int(*compare)(Item*, Item*));

bool storage_do(ArrayListPtr list, Item* obj, int(*compare)(Item*, Item*), bool(*do_on)(ArrayListPtr, Item*, Item**));

int storage_size(ArrayListPtr list);


ArrayListPtr storage_create()
{
    ArrayListPtr newList = malloc(sizeof(ArrayList));
    *newList = (ArrayList){
        .size = 0,
        .capacity = 8,
        .data = calloc(8, sizeof(void*))
    };
    return newList;
}

//Destroys the arraylist. Assumes all values inside have already been destroyed, and so does not try to free them.
void storage_destroy(ArrayListPtr list)
{
    free((list)->data);
    free(list);
}

void storage_insert(ArrayListPtr list, Item* obj, int(*compare)(Item*, Item*))
{
    resize_Storage(list);
    size_t i;

    for (i = list->size++; i > 0 && compare(obj, list->data[i - 1]) < 0; --i)
        list->data[i] = list->data[i - 1];

    list->data[i] = obj;
}

void resize_Storage(ArrayListPtr list)
{
    if (list->size == list->capacity)
    {
        list->capacity *= 2;
        list->data = realloc(list->data, list->capacity * sizeof(Item*));
    }
}

bool storage_contains(ArrayListPtr list, Item* to_find, int(*compare)(Item*, Item*))
{
    return storage_location_of(list, to_find, compare) != NULL;
    /*
    int left = 0;
    int right = list->size - 1;
    int mid;
    while (left <= right)
    {
        mid = (left + right) / 2;
        int comp = compare(to_find, list->data[mid]);
        // printf("\t%d %d %d: %d from %s vs %s\n", left, mid, right, comp, to_find, list->data[mid]);
        if (comp >= 1)
            left = mid + 1;
        else if (comp <= -1)
            right = mid - 1;
        else if (comp == 0)
        // if (comp == 0 && list->data[mid][0] * 10 + list->data[mid][1] < to_find[0] * 10 + to_find[1])
            return true;
    }
    return false;
    */
}

void* storage_replace(ArrayListPtr list, Item* obj, int(*compare)(Item*, Item*)) {
    Item** itemLoc = storage_location_of(list, obj, compare);
    if (itemLoc == NULL)
        return NULL;

    Item* prevStored = *itemLoc;
    *itemLoc = obj;
    return prevStored;

    /*
    int left = 0;
    int right = list->size - 1;
    int mid;
    while (left <= right)
    {
        mid = (left + right) / 2;
        int comp = compare(obj, list->data[mid]);
        if (comp >= 1)
            left = mid + 1;
        else if (comp <= -1)
            right = mid - 1;
        else if (comp == 0) {
            void* toReturn = list->data[mid];
            list->data[mid] = obj;
            return toReturn;
        }
    }


    return NULL;
    */
}

Item** storage_location_of(ArrayListPtr list, Item* obj, int(*compare)(Item*, Item*)) {
    int left = 0;
    int right = list->size - 1;
    int mid;
    while (left <= right)
    {
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


bool storage_do(ArrayListPtr list, Item* obj, int(*compare)(Item*, Item*), bool(*do_on)(ArrayListPtr, Item*, Item**)) {
    return do_on(list, obj, storage_location_of(list, obj, compare));
}


void storage_for_each(ArrayListPtr list, void(*function)(Item*)) {
    for (size_t i = 0; i < list->size; i++)
    {
        function(list->data[i]);
    }

}

void storage_print(ArrayListPtr list, void(*print)(Item*)) {
    printf("[");
    storage_for_each(list, print);
    puts("]");

}

int storage_size(ArrayListPtr list) {
    return list->size;
}