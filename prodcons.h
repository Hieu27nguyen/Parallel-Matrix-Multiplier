/*
 *  prodcons header
 *  Function prototypes, data, and constants for producer/consumer module
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *
 *  Implements the producer-consumer synchronization model using pthreads,
 *  condition variables, and mutex locks. This module is based on:
 *  - OSTEP Chapter 30: "Concurrency and Synchronization" 
 *    (Operating Systems: Three Easy Pieces by Remzi H. Arpaci-Dusseau)
 *  - Lecture examples on multithreading and bounded buffer implementation
 */

#ifndef PRODCONS_H
#define PRODCONS_H

#include <pthread.h>
#include <stdio.h>
#include "matrix.h"
extern Matrix **bigmatrix;

// Declare global variables (Defined in pcmatrix.c)
extern int BOUNDED_BUFFER_SIZE;
extern int NUMBER_OF_MATRICES;
extern int MATRIX_MODE;

// Shared Bounded Buffer
extern Matrix **bigmatrix;  
extern int buffer_head;
extern int buffer_tail;
extern int buffer_count;
extern int buffer_out;

// Synchronization variables
extern pthread_mutex_t buffer_mutex;
extern pthread_mutex_t buffer_lock;
extern pthread_cond_t buffer_not_full;
extern pthread_cond_t buffer_not_empty;
// Declare global variables
extern int produced_count;  // Declare produced_count (not define)
// Structure to track production/consumption stats
typedef struct prodcons {
    int sumtotal;    
    int multtotal;   
    int matrixtotal;
} ProdConsStats;

// Function prototypes
void *prod_worker(void *arg);
void *cons_worker(void *arg);
int put(Matrix *value);   
Matrix *get();            

#endif // PRODCONS_H

