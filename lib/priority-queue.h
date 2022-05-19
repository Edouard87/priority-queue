/**
 * @file priority-queue.h
 * @author your name (you@domain.com)
 * @brief Type defintions for priority queues.
 * @version 0.1
 * @date 2022-05-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/* The initial size of the PQ on creation.           */
#define PQ_INITIAL_SIZE 10
/* The size of the increments of the priority queue. */
#define PQ_INCREMENT_SIZE 10

struct pq_node {
    int data;
    int priority;
};

typedef struct pq_node PQ_Node;

struct PQ_pq {
    int current_size;
    int capacity;
    PQ_Node ** heap;
};

typedef struct pq PQ;
typedef struct PQ_pq PQ_pq;

void PQ_destroy(PQ_pq * q);
void PQ_enqueue(PQ_pq * q, int data, int prioity);
PQ_pq * PQ_create();
int PQ_peek(PQ_pq* q);
int PQ_dequeue(PQ_pq* q);
void PQ_print(PQ_pq* q);
PQ_pq * PQ_heapify(PQ_Node ** arr, int size);
PQ_Node * _PQ_dequeue(PQ_pq* q);
void PQ_destroy_heap_nodes(PQ_Node ** heap, int heap_size);
void _swap(PQ_Node** a, PQ_Node** b);
PQ_Node * _node_copy(PQ_Node * a);

