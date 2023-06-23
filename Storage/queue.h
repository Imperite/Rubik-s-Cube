//Basic Queue interface.
//Allows me to change the underlying code (i.e. hopefully add locks to allow for concurrency) without changing the interface.


typedef struct queue Queue;

typedef void Item;

//Initializes a Queue, returning the stuct used.
Queue* queue_create();

//destroys the queue
void queue_destroy(Queue* queue);

//pops the top object from the queue
Item* queue_pop(Queue* queue);

//pushes the object onto the queue
void queue_push(Queue* queue, Item* obj);

void queue_for_each(const Queue* queue, void(*toDo)(Item*));

int queue_is_empty(const Queue* queue);

int queue_size(const Queue* queue);
