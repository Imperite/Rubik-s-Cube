/**
 * @file queue.h
 * @author David B(dsbradle)
 *
 * Basic Queue interface.
 * Allows me to change the underlying code (i.e. hopefully add locks to allow for concurrency) without changing the interface.
 */
#ifndef queue_h
#define queue_h

/** Queue datastructure used by methods*/
typedef struct queue Queue;

/** Abstract type representing an item stored in the queue. Not stored directly, but always stored as a pointer. */
typedef void* Item;


/**
 * Initializes a Queue, returning the stuct used.
 * @param queue the queue to initalize
 */
void queue_create(Queue* queue);

/**
 * Destroys the queue and anything nodes in it. DOES NOT FREE ITEMS IN QUEUE.
 * @param queue the queue to free
 */
void queue_destroy(Queue* queue);

/**
 * Pops the top item from the queue
 * @param queue the queue to pop from
 * @return Item the item that was at the top of the queue.
 */
Item queue_pop(Queue* queue);


/**
 * Pushes the object onto the queue.
 * @param queue the queue to push onto
 * @param obj the object to add
 */
void queue_push(Queue* queue, Item obj);

/**
 * Iterates over every element in the queue, calling toDo on them.
 * @param queue the queue to call the function on every element of
 * @param toDo the function to call
 */
void queue_for_each(const Queue* queue, void(*toDo)(Item));

/**
 * Returns if the queue is empty.
 * @param queue  the queue to check
 * @return int 0 if the the queue is empty, or 1 otherwise
 */
int queue_is_empty(const Queue* queue);

/**
 * returns the size of the queue.
 * @param queue the queue to check
 * @return int the size of the queue
 */
int queue_size(const Queue* queue);

#endif