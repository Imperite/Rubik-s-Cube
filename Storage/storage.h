#ifndef _storage_h_
#define _storage_h_

#include <stdbool.h>

typedef void* Storage;

// create
Storage storage_create();
// destroy
void storage_destroy(Storage* storagePtr);
// add
void storage_insert(Storage storage, void* cube, int(*compare)(void*, void*));
// contains; will use a binary search to find in storage.
bool storage_contains(Storage storage, void* to_find, int (*compare)(void*, void*));

int storage_size(Storage storage);

void storage_print(Storage storage, void(*print)(void*));

#endif