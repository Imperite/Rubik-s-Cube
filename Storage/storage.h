#ifndef storage_h
#define storage_h

#include <stdbool.h>

/** Abstract type of storage */
typedef struct storage* Storage;

/** Representation of items stored in the datastructure */
typedef void* Item;

/** A comparator that compares to items stored in the queue and returns a value of their difference*/
typedef int(*Comparator)(const void*, const void*);

/** A function that operates on the item without returning anything*/
typedef void(*Function)(Item);

/**
 * Creates a default storage object
 * @param storage the object to initialize
 */
void storage_create(Storage storage);

/**
 * Destroy the storage object. DOES NOT FREE VALUES STORED INSIDE.
 * @param storage the storage to destroy
 */
void storage_destroy(Storage storage);

/**
 * Inserts the object into the storage
 * @param storage the storage to insert into
 * @param obj the object to insert
 * @param compare the comparator to use for comparing objects
 */
void storage_insert(Storage storage, Item obj, Comparator compare);

/**
 * Checks if the object is in storage.
 * @param storage the storage to search
 * @param to_find the object to search for
 * @param compare  the comparison function
 * @return if the object is in storage
 */
bool storage_contains(const Storage storage, const Item to_find, Comparator compare);


/**
 * Replaces the object from the storage and returns it
 * @param storage the storage to serach
 * @param obj the object to replace
 * @param compare the comparison function
 * @return Item the item previously at this location
 */
Item storage_replace(Storage storage, const Item obj, Comparator compare);


/**
 * Returns a pointer to the location of obj in memory using binary search.
 * @param storage the storage to serarch
 * @param obj the object to find
 * @param compare the comparison function
 * @return Item* the pointer to the item's location in storage
 */
Item* storage_location_of(const Storage storage, const Item obj, Comparator compare);

/**
 * Calls function do_on on the location in memory provided. Does not guarantee location is not null.
 * @param storage the storage to search
 * @param obj the object to find the memory location of
 * @param compare the comparison function
 * @param do_on the fucntion to call on the location of
 * @return Item the result of calling do_on on the location
 */
Item storage_do(Storage storage, const Item obj, Comparator compare, Item(*do_on)(Storage, const void*, Item*));

/**
 * Returns the size of the storage
 * @param storage the storage to get the size of
 * @return int the size of the storage
 */
int storage_size(const Storage storage);

/**
 * Prints out all elements in storage
 * @param storage the storage to print
 * @param print the function for printing Items
 */
void storage_print(const Storage storage, Function print);

/**
 * Calls the function on each element in storage
 * @param storage the storage to iterate over
 * @param function the function to call
 */
void storage_for_each(const Storage storage, Function function);

#endif