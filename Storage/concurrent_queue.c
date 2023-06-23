#include <stdlib.h>
#include <stdatomic.h>
#include <stdio.h>

#include "queue.h"
//Based on the Michael and Scott paper,"Simple, Fast, and Practical Non-Blocking and Blocking Concurrent Queue Algorithms"
//Meant to be non-blocking for fastest operation

/** Pointer to a node */
typedef struct pointer {
    struct node* ptr;
    unsigned int count;
} NodePointer;

/** Node stored in queue*/
typedef struct node {
    _Atomic NodePointer next;
    Item value;
} Node;

/** Queue datastructure*/
typedef struct queue {
    _Atomic NodePointer head;
    _Atomic NodePointer tail;
} Queue;

void queue_create(Queue* queue)
{
    Node* node = malloc(sizeof(Node));
    *node = (Node){
        .value = NULL,
            .next = {
                .ptr = NULL,
                .count = 0
        }
    };
    *queue = (Queue){
        .head = {
            .ptr = node,
            .count = 0
        },
        .tail = {
            .ptr = node,
            .count = 0
        }
    };
}

void queue_destroy(Queue* queue)
{
    while (!queue_is_empty(queue))
        queue_pop(queue);
    free(atomic_load(&queue->head).ptr);
    free(queue);
}

void queue_push(Queue* queue, Item obj)
{
    Node* node = malloc(sizeof(Node));
    *node = (Node){
        {NULL, 0}, obj
    };

    NodePointer tail = { .ptr = NULL, .count = 0 };
    NodePointer* new = calloc(1, sizeof(NodePointer));
    new->count = 0;
    new->ptr = NULL;
    while (1) {
        tail = atomic_load(&queue->tail);
        NodePointer next = tail.ptr->next;

        if (tail.count == atomic_load(&queue->tail).count && tail.ptr == atomic_load(&queue->tail).ptr) {
            if (next.ptr == NULL) {
                *new = (NodePointer){ node, next.count + 1 };
                if (atomic_compare_exchange_strong(&tail.ptr->next, &next, *new)) {
                    break;
                }
            }
            else {
                *new = (NodePointer){ next.ptr, tail.count + 1 };
                atomic_compare_exchange_strong(&queue->tail, &tail, *new);
            }
        }
    }
    // *new = (NodePointer){ .ptr = node, .count = tail.count + 1 };
    new->ptr = node;
    new->count = tail.count + 1;
    atomic_compare_exchange_strong(&queue->tail, &tail, *new);
    free(new);
}


Item queue_pop(Queue* queue)
{
    NodePointer head;
    Item obj;
    NodePointer* new = malloc(sizeof(NodePointer));
    while (1) {
        head = atomic_load(&queue->head);
        NodePointer tail = atomic_load(&queue->tail);
        NodePointer next = head.ptr->next;
        if (head.count == atomic_load(&queue->head).count && head.ptr == atomic_load(&queue->head).ptr) {
            if (head.ptr == tail.ptr) {
                if (next.ptr == NULL) {   // only one element in queue (the extra)
                    free(new);
                    return NULL;
                }
                //help tail swap along, since tail has fallen behind
                *new = (NodePointer){ next.ptr, tail.count + 1 };
                atomic_compare_exchange_strong(&queue->tail, &tail, *new);
            }

            obj = next.ptr->value;

            *new = (NodePointer){ next.ptr, head.count + 1 };
            if (atomic_compare_exchange_strong(&queue->head, &head, *new))
                break;
        }
    }
    //This may(aka will) cause issues
    free(head.ptr);
    free(new);
    return obj;
}

//TODO: check if threadsafe
//probably not threadsafe, but I don't plan to run **during** any other threads - this is only for after the threaded operation has completed, ideally.

void queue_for_each(const Queue* queue, void(*toDo)(Item))
{
    Node* head = atomic_load(&queue->head).ptr;

    Node* curr = atomic_load(&head->next).ptr;
    while (curr->value != NULL) {
        toDo(curr->value);
        curr = atomic_load(&curr->next).ptr;
    }
}

int queue_is_empty(const Queue* queue)
{
    return atomic_load(&atomic_load(&queue->head).ptr->next).ptr == NULL;
}

int queue_size(const Queue* queue)
{
    Node* curr = atomic_load(&atomic_load(&queue->head).ptr->next).ptr;
    int size = 0;
    while (curr != NULL && curr->value != NULL) {
        curr = atomic_load(&curr->next).ptr;
        size++;
    }
    return size;

}