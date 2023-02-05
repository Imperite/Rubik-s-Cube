#include <stdlib.h>
#include <stdatomic.h>
//Based on the Michael and Scott paper,"Simple, Fast, and Practical Non-Blocking and Blocking Concurrent Queue Algorithms"/

typedef void Item;


typedef struct pointer {
    struct node* ptr;
    unsigned int count;
} NodePointer;

typedef struct node {
    NodePointer next;
    Item* value;
} Node;

typedef struct queue {
    NodePointer head;
    NodePointer tail;
} Queue;

Queue* queue_create();
void queue_destroy(Queue* queue);
void queue_push(Queue* queue, Item* obj);
Item* queue_pop(Queue* queue);
void queue_for_each(Queue* queue, void(*toDo)(Item*));
int queue_is_empty(Queue* queue);


Queue* queue_create() {
    Node* node = malloc(sizeof(Node));
    node->next.ptr = NULL;

    Queue* queue = malloc(sizeof(Queue));
    queue->head.ptr = node;
    queue->tail.ptr = node;
    return queue;
}

//assumes all operations are done, clearing the queue;
void queue_destroy(Queue* queue) {
    while (!queue_is_empty(queue))
        queue_pop(queue);
    free(queue);
}

//same as enqueue
void queue_push(Queue* queue, Item* obj) {
    Node* node = malloc(sizeof(Node));
    *node = (Node){
        .value = obj,
        .next = {.ptr = NULL, .count = 0}
    };

    NodePointer tail;
    while (1) {
        tail = queue->tail;
        NodePointer next = tail.ptr->next;

        if (tail.count == queue->tail.count && tail.ptr == queue->tail.ptr) {
            if (next.ptr == NULL) {
                if (atomic_compare_exchange_strong(&tail.ptr->next, &next, ((NodePointer) { node, next.count + 1 }))) {
                    break;
                }
            }
            else {
                atomic_compare_exchange_strong(&queue->tail, &tail, ((NodePointer) { next.ptr, tail.count + 1 }));
            }
        }
    }
    atomic_compare_exchange_strong(&queue->tail, &tail, ((NodePointer) { node, tail.count + 1 }));
}


Item* queue_pop(Queue* queue) {
    NodePointer head;
    Item* obj;
    while (1) {
        head = queue->head;
        NodePointer tail = queue->tail;
        NodePointer next = head.ptr->next;
        if (head.count == queue->head.count && head.ptr == queue->head.ptr) {
            if (head.ptr == tail.ptr) {
                if (next.ptr == NULL)
                    return NULL;

                atomic_compare_exchange_strong(&queue->tail, &tail, ((NodePointer){ next.ptr, tail.count + 1 }));
            }
            obj = next.ptr->value;

            if (atomic_compare_exchange_strong(&queue->head, &head, ((NodePointer) {next.ptr, head.count + 1 })))
                break;
        }
    }
    free(head.ptr);
    //TODO: do something to free the head
    return obj;
}

//TODO: check if threadsafe
//probably not threadsafe, but I don't plan to run **during** any other threads - this is only for after the threaded operation has completed, ideally.
void queue_for_each(Queue* queue, void(*toDo)(Item*)) {
    Node* curr = queue->head.ptr;
    while (curr != NULL) {
        toDo(curr->value);
        curr = curr->next.ptr;
    }
}

int queue_is_empty(Queue* queue) {
    return queue->head.ptr->next.ptr == NULL;
}