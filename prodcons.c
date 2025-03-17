/*
 *  prodcons module
 *  Producer Consumer module
 *
 *  Implements routines for the producer consumer module based on
 *  chapter 30, section 2 of Operating Systems: Three Easy Pieces
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *
 *  Sources:
 *  - "Operating Systems: Three Easy Pieces" by Remzi H. Arpaci-Dusseau & Andrea C. Arpaci-Dusseau
 *  - University of Washington, Tacoma TCSS 422 Course Materials
 *  - Pthreads documentation: https://man7.org/linux/man-pages/man7/pthreads.7.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "counter.h"
#include "matrix.h"
#include "pcmatrix.h"
#include "prodcons.h"

// Define mutex and condition variables to synchronize access to the bounded buffer
pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER; // Mutex for buffer synchronization
pthread_cond_t buffer_not_empty = PTHREAD_COND_INITIALIZER; // Condition variable for buffer not empty
pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER; // Condition variable for buffer not full

// Shared buffer variables
Matrix **bigmatrix; // The bounded buffer storing matrices
int buffer_count = 0; // Number of matrices currently in the buffer
int buffer_head = 0; // Index of the oldest matrix in the buffer
int buffer_tail = 0; // Index of the next available spot in the buffer
int matrices_produced = 0; // Counter tracking matrices produced
int matrices_consumed = 0; // Counter tracking matrices consumed

// Function to put a matrix into the bounded buffer
int put(Matrix *value) {
    pthread_mutex_lock(&buffer_lock); // Lock the buffer to prevent race conditions
    while (buffer_count == BOUNDED_BUFFER_SIZE) { // Wait if buffer is full
        pthread_cond_wait(&buffer_not_full, &buffer_lock);
    }
    bigmatrix[buffer_tail] = value; // Store matrix in the buffer
    buffer_tail = (buffer_tail + 1) % BOUNDED_BUFFER_SIZE; // Move tail pointer circularly
    buffer_count++; // Increase buffer count
    matrices_produced++; // Track the number of matrices produced
    pthread_cond_signal(&buffer_not_empty); // Signal consumers that buffer is not empty
    pthread_mutex_unlock(&buffer_lock); // Unlock the buffer
    return 0; // Return success
}

// Function to get a matrix from the bounded buffer
Matrix *get() {
    pthread_mutex_lock(&buffer_lock); // Lock the buffer to prevent race conditions
    while (buffer_count == 0 && matrices_produced < NUMBER_OF_MATRICES) { // Wait if buffer is empty and production is ongoing
        pthread_cond_wait(&buffer_not_empty, &buffer_lock);
    }
    if (buffer_count == 0 && matrices_produced >= NUMBER_OF_MATRICES) { // If all matrices are produced and buffer is empty, return NULL
        pthread_mutex_unlock(&buffer_lock);
        return NULL;
    }
    Matrix *matrix = bigmatrix[buffer_head]; // Retrieve the oldest matrix from the buffer
    buffer_head = (buffer_head + 1) % BOUNDED_BUFFER_SIZE; // Move head pointer circularly
    buffer_count--; // Decrease buffer count
    matrices_consumed++; // Track the number of matrices consumed
    pthread_cond_signal(&buffer_not_full); // Signal producers that buffer is not full
    pthread_mutex_unlock(&buffer_lock); // Unlock the buffer
    return matrix; // Return retrieved matrix
}

// Producer thread function
void *prod_worker(void *arg) {
    ProdConsStats *stats = (ProdConsStats *)arg; // Cast argument to statistics structure
    for (int i = 0; i < NUMBER_OF_MATRICES; i++) { // Loop to produce matrices
        Matrix *mat = GenMatrixRandom(); // Generate a random matrix
        put(mat); // Place matrix in the buffer
        stats->sumtotal += SumMatrix(mat); // Update total sum of produced matrices
        stats->matrixtotal++; // Increment total produced matrix count
    }
    return (void *)stats; // Return statistics to the main thread
}

// Consumer thread function
void *cons_worker(void *arg) {
    ProdConsStats *stats = (ProdConsStats *)arg; // Cast argument to statistics structure
    while (1) {
        Matrix *m1 = get(); // Retrieve first matrix from buffer
        if (!m1) break; // If NULL is returned, exit the consumer thread
        Matrix *m2;
        while (1) {
            m2 = get(); // Retrieve second matrix
            if (!m2) break; // If NULL is returned, discard first matrix and exit
            Matrix *result = MatrixMultiply(m1, m2); // Attempt to multiply matrices
            if (result) { // If multiplication is successful
                DisplayMatrix(m1, stdout); // Print first matrix
                printf("    X\n");
                DisplayMatrix(m2, stdout); // Print second matrix
                printf("    =\n");
                DisplayMatrix(result, stdout); // Print result matrix
                printf("\n");
                stats->sumtotal += SumMatrix(m1) + SumMatrix(m2); // Update sum total
                stats->matrixtotal += 2; // Count both matrices used
                stats->multtotal++; // Count multiplication performed
                FreeMatrix(result); // Free memory of result matrix
                break;
            }
            FreeMatrix(m2); // Free second matrix if multiplication failed
        }
        FreeMatrix(m1); // Free first matrix
    }
    return (void *)stats; // Return statistics to the main thread
}

