#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>


typedef void Item;

typedef struct container {
    Item* item;
    // pthread_mutex_t lock;

} Container;

//Changing to an array of Container pointers to allow for modifying of the index of items without needing to acquire a lock for the item itself.
// Works because no items are destroyed for the list during use, only modified or added.
typedef struct array_list
{
    size_t size;
    size_t capacity;
    pthread_mutex_t lock;
    Item** data;
} ArrayList;
typedef ArrayList* ArrayListPtr;

// create an empty arraylist of size 8
ArrayListPtr storage_create();
// destroy arraylist and all parts (DOES NOT DESTROY OBJECTS STORED INSIDE)
void storage_destroy(ArrayListPtr list);
// add
void storage_insert(ArrayListPtr list, Item* obj, int(*compare)(Item*, Item*));
// resize
void resize_Storage(ArrayListPtr list);

Item** storage_location_of(ArrayListPtr list, Item* obj, int(*compare)(Item*, Item*));

void* storage_do(ArrayListPtr list, Item* obj, int(*compare)(Item*, Item*), void* (*do_on)(ArrayListPtr, Item*, Item**));

int storage_size(ArrayListPtr list);


ArrayListPtr storage_create()
{
    ArrayListPtr newList = malloc(sizeof(ArrayList));
    *newList = (ArrayList){
        .size = 0,
        .capacity = 8,
        .data = calloc(8, sizeof(Container*))
    };
    pthread_mutex_init(&newList->lock, NULL);
    return newList;
}

//Destroys the arraylist. Assumes all values inside have already been destroyed, and so does not try to free them.
void storage_destroy(ArrayListPtr list)
{
    pthread_mutex_lock(&list->lock);
    // for (size_t i = 0; i < list->size; i++)
    // {
    //     // pthread_mutex_destroy(&list->data[i]->lock);
    //     free(list->data[i]);
    // }
    pthread_mutex_unlock(&list->lock);
    pthread_mutex_destroy(&list->lock);
    free(list->data);
    free(list);
}

void storage_insert(ArrayListPtr list, Item* obj, int(*compare)(Item*, Item*))
{
    pthread_mutex_lock(&list->lock);
    resize_Storage(list);
    size_t i;

    for (i = list->size++; i > 0 && compare(obj, list->data[i - 1]) < 0; --i)
        list->data[i] = list->data[i - 1];

    // Container* newContainer = malloc(sizeof(Container));
    // *newContainer = (Container){ .item = obj };
    // pthread_mutex_init(&newContainer->lock, NULL);
    list->data[i] = obj;
    pthread_mutex_unlock(&list->lock);

}

void resize_Storage(ArrayListPtr list)
{
    if (list->size == list->capacity - 1)
    {
        list->data = realloc(list->data, list->capacity * 2 * sizeof(Container*));
        list->capacity *= 2;
    }
}

Item** storage_location_of(ArrayListPtr list, Item* obj, int(*compare)(Item*, Item*)) {
    pthread_mutex_lock(&list->lock);
    int left = 0;
    int right = list->size - 1;
    int mid;
    Item** value = NULL;
    while (left <= right)
    {
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


void* storage_do(ArrayListPtr list, Item* obj, int(*compare)(Item*, Item*), void* (*do_on)(ArrayListPtr, Item*, Item**)) {
    Container* c = (Container*)storage_location_of(list, obj, compare);

    // pthread_mutex_t* lock = NULL;
    // if (c != NULL)
    //     lock = &c->lock;

    // if (lock != NULL)
    //     pthread_mutex_lock(lock);

    void* result = do_on(list, obj, (Item**)c);
    // if (lock != NULL)
    //     pthread_mutex_unlock(lock);

    return result;
}


void storage_for_each(ArrayListPtr list, void(*function)(Item*)) {
    for (size_t i = 0; i < list->size; i++)
    {
        // pthread_mutex_lock(&(list->data[i])->lock);
        function(list->data[i]);
        // pthread_mutex_unlock(&list->data[i]->lock);
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