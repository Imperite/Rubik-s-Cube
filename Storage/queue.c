#include <stdlib.h>
#include <stdio.h>

typedef struct node {
    struct node* next;
    void* value;
} Node;

typedef struct queue {
    Node* tail, * head;
} Queue;

Queue* init_Queue();
void destroy_Queue(Queue* queue);
void push_Queue(Queue* queue, void* obj);
void* pop_Queue(Queue* queue);
void print_Queue(Queue* q);
void forEach_Queue(Queue* queue, void(*toDo)(void*));
int empty_Queue(Queue* queue);


Queue* init_Queue() {
    Queue* queue = malloc(sizeof(Queue));
    queue->tail = NULL;
    queue->head = NULL;
}

void destroy_Queue(Queue* queue) {
    while (!empty_Queue(queue)) {
        pop_Queue(queue);
    }
    free(queue);
}

void push_Queue(Queue* queue, void* obj) {
    Node* n = malloc(sizeof(Node));
    n->next = NULL;
    n->value = obj;

    if (queue->head == NULL) {
        queue->head = n;
        queue->tail = n;
        return;
    }

    queue->tail->next = n;
    queue->tail = n;
}

void* pop_Queue(Queue* queue) {
    if (queue->head = NULL)
        return NULL;

    Node* n = queue->head;

    Node* next = n->next;
    if (next == NULL)
        queue->tail = NULL;
    queue->head = next;

    void* obj = n->value;
    free(n);
    return obj;
}


void forEach_Queue(Queue* queue, void(*toDo)(void*)) {
    Node* curr = queue->head;
    while (curr != NULL) {
        toDo(curr->value);
        curr = curr->next;
    }
}

int empty_Queue(Queue* queue) {
    return queue->head == NULL;
}