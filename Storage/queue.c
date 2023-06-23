#include <stdlib.h>
#include <stdio.h>

#include "queue.h"

/** A node in the queue*/
typedef struct node {
    struct node* next;
    Item value;
} Node;

/** the Queue datastructure */
typedef struct queue {
    Node* tail, * head;
} Queue;


Queue* queue_create()
{
    Queue* queue = malloc(sizeof(Queue));
    queue->tail = NULL;
    queue->head = NULL;
    return queue;
}

void queue_destroy(Queue* queue)
{
    while (!queue_is_empty(queue)) {
        queue_pop(queue);
    }
    free(queue);
}

void queue_push(Queue* queue, Item obj)
{
    Node* n = malloc(sizeof(Node));
    n->next = NULL;
    n->value = obj;

    if (queue_is_empty(queue)) {
        queue->head = n;
    }
    else {
        queue->tail->next = n;
    }
    queue->tail = n;
}

Item queue_pop(Queue* queue)
{
    if (queue_is_empty(queue))
        return NULL;

    Node* head = queue->head;

    // Node* next = n->next;
    if (head->next == NULL)
        queue->tail = NULL;

    queue->head = head->next;

    Item obj = head->value;
    free(head);
    return obj;
}


void queue_for_each(const Queue* queue, void(*toDo)(Item))
{
    Node* curr = queue->head;
    while (curr != NULL) {
        toDo(curr->value);
        curr = curr->next;
    }
}

int queue_is_empty(const Queue* queue)
{
    return queue->head == NULL;
}

int queue_size(const Queue* queue)
{
    int items = 0;
    Node* curr = queue->head;
    while (curr != NULL) {
        items++;
        curr = curr->next;
    }
    return items;
}
