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

int storage_size(Storage storage);

void storage_print(Storage storage, void(*print)(void*));

void storage_for_each(Storage storage, void(*function)(void*));

#endif