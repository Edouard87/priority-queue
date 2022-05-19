/**
 * @file tester.c
 * @author Edouard Desparois-Perrault (eperrault23@andover.edu)
 * @brief Testing methods for the binary heap implementation of priority queues.
 * @version 0.1
 * @date 2022-05-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "../lib/priority-queue.h"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <regex.h>
#include <unistd.h>
#include <assert.h>
#define RANDOM_NUM_SIZE 4294967296

struct _random_pq {
    PQ_pq * pq;
    PQ_Node ** nodes_random;
    PQ_Node ** nodes_ordered;
};

typedef struct _random_pq _Random_pq;

/* 
 * ****************
 * HELPER FUNCTIONS
 * **************** 
 */ 

/**
 * @brief Implementation of Fisher-Yates shuffle for truly
 * random number distribution. See https://stackoverflow.com/questions/42321370/fisher-yates-shuffling-algorithm-in-c.
 * REUSED from previous project on Binary Search Trees.
 * 
 * @param arr The array to shuffle.
 * @param n The size of the array to shuffle.
 */
void _random_shuffle(PQ_Node ** arr, int n) {
    int i, j;
    PQ_Node * tmp;
    for (i = n - 1; i > 0; i--) {
        j = rand() % (i+1);
        tmp = arr[j];
        arr[j] = arr[i];
        arr[i] = tmp;
    }
}

/**
 * @brief Compare two singular nodes.
 * 
 * @param node_a The first node to compare.
 * @param node_b The second node to compare.
 * @return int 0 if they are not equal, 1 if they are equal.
 */
int _compare_single_nodes(PQ_Node * node_a, PQ_Node * node_b) {
    if (node_a->priority != node_b->priority || node_a->data != node_b->data) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief Compare two heaps of nodes.
 * 
 * @param nodes_a The first heap of nodes.
 * @param nodes_b The second heap of nodes.
 * @param n The total amount of nodes in both heaps.
 * @return int 0 if they're not equal, 1 if they're equal.
 */
int _compare_nodes(PQ_Node ** nodes_a, PQ_Node ** nodes_b, int n) {
    for (int i = 0; i < n; i++) {
        if (!_compare_single_nodes(nodes_a[i], nodes_b[i])) return 0;
    }
    return 1;
}

/**
 * @brief Compare two Priority Queues.
 * 
 * @return int 0 if different, 1 if same.
 * 
 */
int _compare_pq(PQ_pq * pq_a, PQ_pq * pq_b) {
    assert(pq_a->current_size == pq_b->current_size); // They must be the same size.
    return _compare_nodes(pq_a->heap, pq_b->heap, pq_a->current_size);
}

/**
 * @brief Creates a random priority queue struct from the provided size parameter.
 * 
 * @param size The size of the priority queue to create.
 * @return _Random_pq* A pointer to the random priority queue structure that contains the nodes within the queue
 * and the queue itself.
 */
_Random_pq * _random_n_pq(int size) {
    PQ_Node ** nodes = malloc(sizeof(PQ_Node*) * size); // Nodes to be in random order.
    PQ_Node ** ordered_nodes = malloc(sizeof(PQ_Node*) * size); // Nodes to be in increasing order.
    PQ_pq * pq = PQ_create();
    // Create an array of random nodes in increasing order of priority, as this is a min-heap.
    for (int i = 0; i < size; i++) {
        PQ_Node * node = malloc(sizeof(PQ_Node));
        node->data = rand() % RANDOM_NUM_SIZE;
        node->priority = i;
        nodes[i] = node;
        ordered_nodes[i] = node;
    }
    // Shuffle the nodes.
    _random_shuffle(nodes, size);
    // Enqueue the nodes.
    for (int i = 0; i < size; i++) {
        PQ_enqueue(pq, nodes[i]->data, nodes[i]->priority);
    }
    _Random_pq * rand = malloc(sizeof(_Random_pq));
    rand->nodes_ordered = ordered_nodes;
    rand->nodes_random = nodes;
    rand->pq = pq;
    return rand;
}

/**
 * @brief A function to generate a PQ of a definite
 * size. This is useful for debugging tests as it ensures that the PQ
 * produced has nodes with the same priorities. This also makes
 * use of the `_Random_PQ` struct for compatability
 * purposes (so it can be used as a drop-in replacement for
 * `_random_n_pq` or `_random_pq`).
 * 
 * @note There is no reason to make a random list of nodes as this
 * functionality has already been extensively tested. As such, this has
 * not been added. `random_nodes` will be the same as ordered_nodes for 
 * compatability.
 * 
 * @warning There is no possible size configuration for this
 * helper function. As such, it may fail in some circumstances.
 * 
 * @return _Random_PQ* The created PQ of definite size.
 */
_Random_pq * _definite_pq() {
    PQ_pq * pq = PQ_create();
    const int SIZE = 4;
    int priorities[SIZE];
    priorities[0] = 2;
    priorities[1] = 4;
    priorities[2] = 5;
    priorities[3] = 8;
    PQ_Node ** nodes = malloc(SIZE * sizeof(PQ_Node*));
    for (int i = 0; i < SIZE; i++) {
        PQ_Node * node = malloc(sizeof(PQ_Node));
        node->priority = priorities[i];
        node->data = i;
        nodes[i] = node;
        PQ_enqueue(pq, nodes[i]->data, nodes[i]->priority);
    }
    _Random_pq * rand = malloc(sizeof(_Random_pq));
    rand->nodes_ordered = nodes;
    rand->nodes_random = nodes;
    rand->pq = pq;
    return rand;
}

/**
 * @brief Create a random PQ as large as the configured
 * initial size of the heap.
 * 
 * @note This function is included for compatability
 * with legacy functions. Configuring the size as a parameter
 * using `random_n_pq` is preferred.
 * 
 * @return _Random_pq* The created random PQ. It has an array 
 * of random unordered nodes and their associated data. 
 */
_Random_pq * _random_pq() {
    return _random_n_pq(PQ_INITIAL_SIZE);
}

/**
 * @brief Helper function to safely destroy a `_Random_pq`.
 * 
 * @param r_pq The random pq to destroy.
 */
void _destroy_random_pq(_Random_pq * r_pq) {
    PQ_destroy_heap_nodes(r_pq->nodes_ordered, r_pq->pq->current_size);
    free(r_pq->nodes_ordered);
    /* NOTE: deleting the nodes in the ordered array deletes the nodes in the
     * unordered array because they point to the same objects. */
    PQ_destroy(r_pq->pq); // Destroy last as we access the `current_size`.
    free(r_pq);
}

/* 
 * ***********
 * BEGIN TESTS
 * ***********
 */

/**
 * @brief Test enqeue and dequeue on an empty heap.
 * 
 * @return int 0 if fail, 1 if pass.
 */
int test_enqueue_dequeue(void) {
    PQ_pq * pq = PQ_create();
    PQ_Node node;
    node.data = rand() % RANDOM_NUM_SIZE;
    node.priority = rand() % 100;
    PQ_enqueue(pq, node.data, node.priority);
    PQ_Node * dequeued_node = _PQ_dequeue(pq);
    CU_ASSERT(dequeued_node->data == node.data);
    CU_ASSERT(dequeued_node->priority == node.priority);
    PQ_destroy(pq);
    /* NOTE: `dequeued_node` must be freed as its memory address is
     * no longer within the range of accessible values within the heap.
     * Freeing the heap will not free the `dequeued_node`. 
     */
    free(dequeued_node);
}

/**
 * @brief Uses random values to extensively test enqueing
 * and dequeuing.
 * 
 * @return int 0 if fail, 1 if pass.
 */
int test_enqueue_dequeue_extensive(void) {
    _Random_pq * r_pq = _random_pq();
    PQ_Node * nodes[PQ_INITIAL_SIZE];
    for (int i = 0; i < PQ_INITIAL_SIZE; i++) {
        nodes[i] = _PQ_dequeue(r_pq->pq);
    }
    CU_ASSERT_TRUE(_compare_nodes(nodes, r_pq->nodes_ordered, PQ_INITIAL_SIZE));
    /* NOTE: As previously stated, dequeued nodes must be destroyed seperately. */
    PQ_destroy_heap_nodes(nodes, PQ_INITIAL_SIZE);
    _destroy_random_pq(r_pq);
}

/**
 * @brief Test the heapify method using a randomized priority queue.
 * 
 * @return int 0 if fail, 1 if pass.
 */
int test_heapify(void) {
    _Random_pq * r_pq = _random_pq(); // not implemented using `PQ_Heapify`.
    PQ_pq * pq = PQ_heapify(r_pq->nodes_random, r_pq->pq->current_size); // using the nodes in random order.
    CU_ASSERT(_compare_pq(r_pq->pq, pq));
    _destroy_random_pq(r_pq);
    PQ_destroy(pq);
}

/**
 * @brief Test enqueue and dequeue when the array must be resized.
 * 
 * @return int 0 if fail, 1 if pass.
 */
int test_large(void) {
    const int NUM_TEST = 30;
    /* Tests enqueuing and resizing */
    _Random_pq * r_pq = _random_n_pq(NUM_TEST);
    PQ_Node * nodes[NUM_TEST];
    /* Ensure that dequeue continues to work */
    for (int i = 0; i < NUM_TEST; i++) {
        nodes[i] = _PQ_dequeue(r_pq->pq);
    }
    CU_ASSERT(_compare_nodes(r_pq->pq->heap, nodes, NUM_TEST));
    _destroy_random_pq(r_pq);
}

/**
 * @brief Testing the peek method.
 * 
 * @return int 0 if fail, 1 if success.
 */
int test_peek(void) {
    const int SIZE = 10;
    _Random_pq * r_pq = _random_n_pq(SIZE);
    for (int i = 0; i < SIZE; i++) {
        int peeked = PQ_peek(r_pq->pq);
        CU_ASSERT(peeked == r_pq->nodes_ordered[i]->data);
        CU_ASSERT(_compare_single_nodes(_PQ_dequeue(r_pq->pq), r_pq->nodes_ordered[i])); 
    }
    _destroy_random_pq(r_pq);
}

/**
 * @brief The pevious functions extensively test the internal dequeing
 * function that returns a node pointer. However, in application, `PQ_dequeue` returns
 * just the data, as the priorty is not really relevant. This method tests the high-level
 * `PQ_dequeue` function.
 * 
 * @return int 0 if fail, 1 if succeed.
 */
int test_high_level_dequeue(void) {
    const int SIZE = 10;
    _Random_pq * r_pq = _random_n_pq(SIZE);
    for (int i = 0; i < SIZE; i++) {
        CU_ASSERT(r_pq->nodes_ordered[i]->data == PQ_dequeue(r_pq->pq));
    }
    /**
     * NOTE: The `PQ_dequeue` function automatically handles
     * deleting the dequeued node.
     * 
     */
    _destroy_random_pq(r_pq);
}

/**
 * @brief Test swapping of memory addresses using the internal `_swap` function.
 * Ensure that no memory leaks are caused.
 * 
 * @return int 0 if fail, 1 if pass.
 */
int test_swap(void) {
    const int runs = 10;
    for (int i = 0; i < runs; i++) {
        PQ_Node * a = malloc(sizeof(PQ_Node));
        a->data = rand() % RANDOM_NUM_SIZE;
        a->priority = rand() % 10;
        PQ_Node * a_original = _node_copy(a);
        PQ_Node * b = malloc(sizeof(PQ_Node));
        b->data = rand() % RANDOM_NUM_SIZE;
        b->priority = rand() % 10;
        PQ_Node * b_original = _node_copy(b);
        _swap(&a, &b);
        CU_ASSERT(_compare_single_nodes(a, b_original));
        CU_ASSERT(_compare_single_nodes(b, a_original));
        free(a);
        free(a_original);
        free(b);
        free(b_original);
    }
}

int main() {
    /* Seed randomness */
    time_t t;
    srand((unsigned) time(&t));
    /* Setup tests */
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("general tests", NULL, NULL);
    CU_add_test(suite, "Basic Test", (void*) test_enqueue_dequeue);
    CU_add_test(suite, "Test the swap helper method", (void*) test_swap);
    CU_add_test(suite, "Basic Test", (void*) test_enqueue_dequeue_extensive);
    CU_add_test(suite, "Heapify Test", (void*) test_heapify);
    CU_add_test(suite, "Larger test for dequeuing and resizing", (void*) test_large);
    CU_add_test(suite, "Test high level dequeing interface", (void*) test_high_level_dequeue);
    CU_add_test(suite, "Test peeking for dequeue", (void*) test_peek);

    CU_basic_run_tests();
    CU_cleanup_registry();
}