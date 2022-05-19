/**
 * @file priority-queues.c
 * @author Edouard Desparois-Perrault (eperrault23@andover.edu)
 * @brief Implementation of priority queue using a min heap. Lower priority
 * numbers will be dequeued first. 
 * @version 0.1
 * @date 2022-04-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "./priority-queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/**
 * @brief Return the index of the parent of the provided index `i`.
 * 
 * @param i The index being searched.
 * @return int The parent of the provided index.
 */
int _parent(int i) {
    return (i - 1) / 2;
}

/**
 * @brief Return the index of the left child of the provided
 * index `i`.
 * 
 * @param i The index being searched.
 * @return int The index of the left child of the parent at
 * index `i`.
 */
int _left_child(int i) {
    return ((2 * i) + 1);
}

/**
 * @brief Return the index of the right child of the provided
 * index `i`.
 * 
 * @param i The index being searched.
 * @return int The index of the right child of the provided
 * index `i`.
 */
int _right_child(int i) {
    return ((2 * i) + 2);
}

/**
 * @brief A helper function to swap two memory addresses by making
 * use of a temporary container for the addres. `a` becomes a pointer to
 * `b` and `b` becomes a pointer to `a`.
 * 
 * @param a One node to swap.
 * @param b Another node to swap.
 */
void _swap(PQ_Node** a, PQ_Node** b) {
    PQ_Node* tmp = *a;
    *a = *b;
    *b = tmp;
}

/**
 * @brief A helper function to copy the contents of a provided
 * node `a` into a new node and return that node. 
 * 
 * @param a The node to copy into a new node.
 */
PQ_Node * _node_copy(PQ_Node * a) {
    PQ_Node* copy = malloc(sizeof(PQ_Node));
    copy->data = a->data;
    copy->priority = a->priority;
    return copy;
}

/**
 * @brief Shifts the provided element at index `i` within
 * the priority queue `q` up the tree until the heap is satisified.
 * 
 * @param i 
 * @param q 
 */
void _shift_up(int i, PQ_pq * q) {
    while (i > 0 && q->heap[_parent(i)]->priority > q->heap[i]->priority) {
        _swap(&(q->heap[_parent(i)]), &(q->heap[i]));
        i = _parent(i);
    }
}

/**
 * @brief Shifts the provided element at index `i` within
 * the priority queue `q` down the tree until the heap is satisified.
 * 
 * @param i 
 * @param q 
 */
void _shift_down(int i, PQ_pq * q) {
    int max_i = i;
    /**
     * NOTE: assumes q->current_size is still valid as no memory deallocation has 
     * taken place yet.
     * 
     */ 
    int l = _left_child(i);
    if (l <= q->current_size && q->heap[l]->priority < q->heap[max_i]->priority) max_i = l;
    int r = _right_child(i);
    if (r <= q->current_size && q->heap[r]->priority < q->heap[max_i]->priority) max_i = r;
    if (i != max_i) {
        _swap(&(q->heap[i]), &(q->heap[max_i]));
        _shift_down(max_i, q);
    }
}

/**
 * @brief A helper function to destroy all nodes in a heap. The tricky
 * part of memory management in this type of system is that the nodes
 * in the array are actually pointers to nodes stored in the heap. As such, each pointer
 * within the array must be individually deallocated.
 * 
 * @param node The pointer to the heap (i.e. array of pointers to
 * nodes stored elswhere in memory). 
 * @param heap_size The size of the provided heap.
 */
void PQ_destroy_heap_nodes(PQ_Node ** heap, int heap_size) {
    for (int i = 0; i < heap_size; i++) {
        // Free each node* in the heap.
        free(heap[i]);
    }
}

/**
 * @brief Destroy a `PQ_pq` and its associated binary heap.
 * 
 * @param q A pointer to the `PQ_pq` to destroy.
 */
void PQ_destroy(PQ_pq * q) {
    // Free each address within the heap.
    PQ_destroy_heap_nodes(q->heap, q->current_size);
    // Free the heap.
    free(q->heap);
    // Free the struct.
    free(q);
}

/**
 * @brief Checks the size of the provided priority_queue. If the size is
 * larger, it resizes the priority queue.
 * 
 * @param q The queue to check.
 */
void _check_size(PQ_pq * q) {
    if (q->current_size + 1 > q->capacity) {
        q->heap = realloc(q->heap, (q->capacity + PQ_INCREMENT_SIZE) * sizeof(PQ_Node *));
        if (!q->heap) {
            perror("Error creating new memory block for heap");
            exit(1);
        }
        q->current_size = q->capacity;
        q->capacity = q->capacity + PQ_INCREMENT_SIZE;
    }
}

/**
 * @brief Enqueue an element with `data` and `priority` into the provided
 * queue `q`.
 * 
 * @param q The queue to which the value will be added.
 * @param data The data to add to the new node in `q`.
 * @param prioity The priority of the data to add to the new node in `q`.
 */
void PQ_enqueue(PQ_pq * q, int data, int prioity) {
    // Add to the end of the queue.
    PQ_Node * to_add = malloc(sizeof(PQ_Node)); // Pointer to the new node in heap.
    to_add->data = data;
    to_add->priority = prioity;
    _check_size(q);
    q->heap[q->current_size] = to_add; // `current_size` is one larger than index. 
    // Shift the node up to maintain the validity of the queue.
    _shift_up(q->current_size, q); // Shifts the node up until the tree is valid.
    q->current_size = q->current_size + 1;
}

/**
 * @brief A helper function to initialize the heap. The initialized
 * heap will be empty, and must be populated with `PQ_enqueue`.
 * 
 * @return PQ* A pointer to the first node of the created empty PQ.
 */
PQ_pq * PQ_create() {
    PQ_pq * q = malloc(sizeof(PQ_pq));
    PQ_Node ** pq_nodes = malloc(sizeof(PQ_Node*) * PQ_INITIAL_SIZE); // Create an array of node pointers
    q->current_size = 0;
    q->capacity = PQ_INITIAL_SIZE;
    q->heap = pq_nodes;
    return q;
}

/**
 * @brief Get the data of the element in the PQ with
 * the highest priority without removing the node
 * from `q`.
 * 
 * @param q The queue to search.
 * @return PQ_Node * A pointer to the node with the highest priority.
 */
int PQ_peek(PQ_pq* q) {
    return q->heap[0]->data;
}

/**
 * @brief A helper function which dequeues a node but returns a pointer to the node instead of deleting it.
 * 
 * @note This function is designed to be used for testing and debugging. As such, it will not delete the node after dequeing it.
 * 
 * @param q The PQ from which a node will be dequeued.
 */
PQ_Node * _PQ_dequeue(PQ_pq* q) {
    PQ_Node * n = q->heap[0];
    q->heap[0] = q->heap[q->current_size - 1]; // Replace the first element with the last element.
    q->current_size = q->current_size - 1;
    _shift_down(0, q);
    return n;
}

/**
 * @brief Dequeue data of the element with the largest priority
 * from the front of the PQ. This removes the element with the highest priority
 * from the current priority queue.
 * 
 * @param q A pointer to the priority queue from which elements will
 * be dequeued.
 * @return int The data of the element with the highest priority.
 */
int PQ_dequeue(PQ_pq* q) {
    PQ_Node * node = _PQ_dequeue(q);
    int data = node->data;
    free(node);
    return data;
}

/**
 * @brief Returns the tree level of the provided index. This simply
 * caculates the base-2 logarithm of the provided index and truncates
 * the decimals.
 * 
 * @note `log2(0)` is presumed to equal 0.
 * 
 * @return int The level of the index `i`.
 */
int _get_level(int i) {
    return (int) log2(i);
}

/**
 * @brief Print the provided priority queue to STDOUT.
 * 
 * @param q The priority queue to print.
 */
void PQ_print(PQ_pq* q) {
    for (int i = 0; i < q->current_size; i++) {
        printf("Pair %d, Level %d: {key: %d, data: %d}\n", i, _get_level(i), q->heap[i]->priority, q->heap[i]->data);
    }
}

/**
 * @brief Turn an array of key-value pairs into a valid heap.
 * 
 * @note The provided array will be copied into a new array. As such, the provided
 * array can be manipulated after the array is consructed.
 * 
 * @param arr An array of PQ_Node, each with data and priority.
 */
PQ_pq * PQ_heapify(PQ_Node ** arr, int size) {
    PQ_pq * pq = PQ_create();
    pq->capacity = size;
    pq->heap = malloc(sizeof(PQ_Node*) * size);
    for (int i = 0; i < size; i++) {
        PQ_enqueue(pq, arr[i]->data, arr[i]->priority);
    }
    return pq;
}