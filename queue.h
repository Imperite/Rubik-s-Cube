//Basic Queue interface.
//Allows me to change the underlying code (i.e. hopefully add locks to allow for concurrency) without changing the interface.

//Initializes a Queue, returning the stuct used.
void* queue_create();

//destroys the queue
void queue_destroy(void** queue);

//pops the top object from the queue
void* queue_pop(void* queue);

//pushes the object onto the queue
void queue_push(void* queue, void* obj);

void queue_for_each(void* queue, void(*toDo)(void*));

int queue_is_empty(void* queue);