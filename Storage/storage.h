#ifndef _storage_h_
#define _storage_h_

#include <stdbool.h>

typedef void* Storage;

// create
Storage init_Storage();
// destroy
void destroy_Storage(Storage storage);
// add
void insert_Storage(Storage storage, void* obj, int(*compare)(void*, void*));
// contains; will use a binary search to find in storage.
bool contains_Storage(Storage storage, void* to_find, int (*compare)(void*, void*));

//removes the object from the storage and returns it
void* replace_Storage(Storage storage, void* obj, int(*compare)(void*, void*));

int size_Storage(Storage storage);

void print_Storage(Storage storage, void(*print)(void*));

void forEach_Storage(Storage storage, void(*function)(void*));

#endif