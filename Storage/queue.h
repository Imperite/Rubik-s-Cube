//Basic Queue interface.
//Allows me to change the underlying code (i.e. hopefully add locks to allow for concurrency) without changing the interface.

//Initializes a Queue, returning the stuct used.
void* init_Queue();

//destroys the queue
void destroy_Queue(void** queue);

//pops the top object from the queue
void* pop_Queue(void* queue);

//pushes the object onto the queue
void push_Queue(void* queue, void* obj);

//prints the queue
void print_Queue(void* queue);

void for_each_Queue(void* queue, void(*toDo)(void*));

int empty_Queue(void* queue);