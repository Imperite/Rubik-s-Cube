#ifndef _storage_h_
#define _storage_h_

#include <stdbool.h>

typedef void* Storage;

// create
Storage storage_create();
// destroy
void storage_destroy(Storage storage);
// add
void storage_insert(Storage storage, void* obj, int(*compare)(void*, void*));
// contains; will use a binary search to find in storage.
bool storage_contains(Storage storage, void* to_find, int (*compare)(void*, void*));

//removes the object from the storage and returns it
void* storage_replace(Storage storage, void* obj, int(*compare)(void*, void*));

//returns a pointer to the location of obj in memory using binary search.
void** storage_location_of(Storage storage, void* obj, int(*compare)(void*, void*));

// calls function do_on on the location in memory provided. Does not guarantee location is not null.
bool storage_do(Storage storage, void* obj, int(*compare)(void*, void*), bool(*do_on)(Storage, void*, void**));

int storage_size(Storage storage);

void storage_print(Storage storage, void(*print)(void*));

void storage_for_each(Storage storage, void(*function)(void*));

#endif