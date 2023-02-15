//Basic Queue interface.
//Allows me to change the underlying code (i.e. hopefully add locks to allow for concurrency) without changing the interface.

typedef void* Queue;

//Initializes a Queue, returning the stuct used.
Queue queue_create();

//destroys the queue
void queue_destroy(Queue* queue);

//pops the top object from the queue
void* queue_pop(Queue queue);

//pushes the object onto the queue
void queue_push(Queue queue, void* obj);

void queue_for_each(Queue queue, void(*toDo)(void*));

int queue_is_empty(Queue queue);

int queue_size(Queue queue);