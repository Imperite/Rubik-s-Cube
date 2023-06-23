#include <stdlib.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
// #include "../cube.h"
#include "storage.h"

const float FILL_RATIO = .6;

typedef struct storage {
    size_t accessors;
    size_t size;
    size_t capacity;
    atomic_bool global_lock;
    size_t(*hash)(Item* toHash);
    Item** data;
} Hashtable;
typedef Hashtable* HashPtr;

HashPtr storage_create();

void storage_destroy(HashPtr hash);

void storage_insert(HashPtr hash, Item* obj, Comparator compare);

void storage_resize(HashPtr hash);

Item** storage_location_of(const HashPtr hash, const Item* obj, Comparator compare);

Item* storage_do(HashPtr hash, const Item* obj, Comparator compare, Item* (*do_on)(HashPtr, const Item*, Item*));

int storage_size(HashPtr hash);

void storage_access_start(HashPtr hash);

void storage_access_finish(HashPtr hash);

void storage_for_each(HashPtr hash, void(*function)(Item*));

void storage_print(HashPtr hash, void(*print)(Item*));

size_t hash_cube(Item* toHash);

HashPtr storage_create()
{
    HashPtr hash = malloc(sizeof(Hashtable));
    *hash = (Hashtable){
        .accessors = 0,
        .size = 0,
        .capacity = 64,
        .global_lock = false,
        .hash = hash_cube,
        .data = calloc(64, sizeof(Item*))
    };
    return hash;
}

void storage_destroy(HashPtr hash)
{
    free(hash->data);
    free(hash);
}

int storage_size(HashPtr hash)
{
    return hash->size;
}

void storage_access_start(HashPtr hash)
{
    while (true) {
        while (hash->global_lock);
        size_t prev = hash->accessors;
        if (atomic_compare_exchange_strong(&(hash->accessors), &prev, prev + 1))
            break;
    }
    // printf("added: %zu\n", hash->accessors);
}

void storage_access_finish(HashPtr hash)
{
    while (true) {
        size_t prev = hash->accessors;
        if (atomic_compare_exchange_strong(&(hash->accessors), &prev, prev - 1))
            break;
    }
    // printf("removed: %zu\n", hash->accessors);
}

size_t hash_cube(Item* toHash)
{
    return cube_hash(toHash);
}

Item** storage_location_of(const HashPtr hash, const Item* obj, Comparator compare)
{

    size_t initialIndex = hash->hash(obj);
    size_t actualIndex = initialIndex;
    while (hash->data[actualIndex % hash->capacity] != 0 && initialIndex != hash->hash(hash->data[actualIndex % hash->capacity])) actualIndex++;
    if (hash->data[actualIndex % hash->capacity] == 0) return NULL;
    return (hash->data + (actualIndex % hash->capacity));
}

Item* storage_do(HashPtr hash, const Item* obj, Comparator compare, Item* (*do_on)(HashPtr, const Item*, Item*))
{
    storage_access_start(hash);

    Item** item = storage_location_of(hash, obj, compare);
    Item* result = do_on(hash, obj, item);

    storage_access_finish(hash);
    return result;
}

void storage_insert(HashPtr hash, Item* obj, Comparator compare)
{
    if (hash->size * 1.0 / hash->capacity >= FILL_RATIO)
        storage_resize(hash);

    storage_access_start(hash);

    size_t initialIndex = hash->hash(obj);
    size_t actualIndex = initialIndex;
    while (true) {
        while (hash->data[actualIndex % hash->capacity] != 0)
            actualIndex++;
        size_t zero = 0;

        if (atomic_compare_exchange_strong((hash->data + (actualIndex % hash->capacity)), &zero, obj))
            break;
    }

    storage_access_finish(hash);
}

void storage_resize(HashPtr hash)
{
// while (true) {
//     atomic_bool prev = hash->global_lock;
//     atomic_compare_exchange_strong(&(hash->global_lock), &prev, true);
// }
    puts("resizing, acquiring lock\n");
    size_t prev_capacity = hash->capacity;
    bool expected = false;
    //spin until CAS successful, or exit if the expansion has already happened
    while (!atomic_compare_exchange_strong(&hash->global_lock, &expected, true)) {
        while (hash->global_lock);
        if (hash->capacity > prev_capacity) return;
        prev_capacity = hash->capacity;
    }
    hash->global_lock = true;
    while (hash->accessors > 1);
    puts("got lock\n");
    Item** newData = realloc(hash->data, hash->capacity * 2 * sizeof(Item*));

    hash->capacity *= 2;
    for (size_t i = 0; i < prev_capacity; i++) {
        if (hash->data[i] != 0) {
            newData[hash->hash(hash->data[i]) % (hash->capacity * 2)] = hash->data[i];
        }
    }
    free(hash->data);
    hash->data = newData;

    hash->global_lock = false;
}

void storage_for_each(HashPtr hash, void(*function)(Item*))
{
    for (size_t i = 0; i < hash->capacity; i++) {
        if (hash->data[i] == NULL) continue;
        function(hash->data[i]);
    }

}

void storage_print(HashPtr hash, void(*print)(Item*))
{
    printf("[");
    storage_for_each(hash, print);
    puts("]");

}