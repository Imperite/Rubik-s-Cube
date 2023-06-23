#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#include "storage.h"


/** A wrapper structure for containing a lock and item together*/
typedef struct container {
    Item item;
    // pthread_mutex_t lock;

} Container;

//Changing to an array of Container pointers to allow for modifying of the index of items without needing to acquire a lock for the item itself.
// Works because no items are destroyed for the list during use, only modified or added.
/**
 * The storage implementation as a concurrent arraylist
 */
typedef struct storage
{
    size_t size;
    size_t capacity;
    pthread_mutex_t lock;
    Item* data;
} ArrayList;
typedef ArrayList* ArrayListPtr;

/**
 * Resizes the list, doubling its size if needed
 * @param list the list to resize
 */
void storage_resize(ArrayListPtr list);



void storage_create(ArrayListPtr newList)
{
    *newList = (ArrayList){
        .size = 0,
        .capacity = 8,
        .data = calloc(8, sizeof(Container*))
    };
    pthread_mutex_init(&newList->lock, NULL);
}

void storage_destroy(ArrayListPtr list)
{
    // pthread_mutex_lock(&list->lock);
    // for (size_t i = 0; i < list->size; i++)
    // {
    //     // pthread_mutex_destroy(&list->data[i]->lock);
    //     free(list->data[i]);
    // }
    // pthread_mutex_unlock(&list->lock);
    pthread_mutex_destroy(&list->lock);
    free(list->data);
    free(list);
}

void storage_insert(ArrayListPtr list, Item obj, Comparator compare)
{
    pthread_mutex_lock(&list->lock);
    storage_resize(list);
    size_t i;

    for (i = list->size++; i > 0 && compare(obj, list->data[i - 1]) < 0; --i)
        list->data[i] = list->data[i - 1];

    // Container* newContainer = malloc(sizeof(Container));
    // *newContainer = (Container){ .item = obj };
    // pthread_mutex_init(&newContainer->lock, NULL);
    list->data[i] = obj;
    pthread_mutex_unlock(&list->lock);

}

void storage_resize(ArrayListPtr list)
{
    if (list->size == list->capacity - 1) {
        list->data = realloc(list->data, list->capacity * 2 * sizeof(Container*));
        list->capacity *= 2;
    }
}

Item* storage_location_of(const ArrayListPtr list, const Item obj, Comparator compare)
{
    pthread_mutex_lock(&list->lock);
    int left = 0;
    int right = list->size - 1;
    int mid;
    Item value = NULL;
    while (left <= right) {
        mid = (left + right) / 2;
        int comp = compare(obj, list->data[mid]);
        if (comp >= 1)
            left = mid + 1;
        else if (comp <= -1)
            right = mid - 1;
        else if (comp == 0) {
            value = (list->data + mid);
            break;
        }
    }
    pthread_mutex_unlock(&list->lock);
    return value;
}


Item storage_do(ArrayListPtr list, const Item obj, Comparator compare, void* (*do_on)(ArrayListPtr, const void*, Item*))
{
    Container* c = (Container*)storage_location_of(list, obj, compare);

    // pthread_mutex_t* lock = NULL;
    // if (c != NULL)
    //     lock = &c->lock;

    // if (lock != NULL)
    //     pthread_mutex_lock(lock);

    Item result = do_on(list, obj, (Item*)c);
    // if (lock != NULL)
    //     pthread_mutex_unlock(lock);

    return result;
}


void storage_for_each(const ArrayListPtr list, Function function)
{
    for (size_t i = 0; i < list->size; i++) {
        // pthread_mutex_lock(&(list->data[i])->lock);
        function(list->data[i]);
        // pthread_mutex_unlock(&list->data[i]->lock);
    }

}

void storage_print(const ArrayListPtr list, Function print)
{
    printf("[");
    storage_for_each(list, print);
    puts("]");

}

int storage_size(const ArrayListPtr list)
{
    return list->size;
}