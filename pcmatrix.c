/*
 *  pcmatrix module
 *  Primary module providing control flow for the pcMatrix program
 *
 *  Producer consumer bounded buffer program to produce random matrices in parallel
 *  and consume them while searching for valid pairs for matrix multiplication.
 *  Matrix multiplication requires the first matrix column count equal the
 *  second matrix row count.
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
#include <assert.h>
#include <time.h>
#include "matrix.h"
#include "counter.h"
#include "prodcons.h"
#include "pcmatrix.h"

int main (int argc, char * argv[])
{
  // Process command line arguments
  int numw = NUMWORK; // Default number of worker threads
  if (argc==1)
  {
    // Use default values if no arguments are provided
    BOUNDED_BUFFER_SIZE=MAX;
    NUMBER_OF_MATRICES=LOOPS;
    MATRIX_MODE=DEFAULT_MATRIX_MODE;
    printf("USING DEFAULTS: worker_threads=%d bounded_buffer_size=%d matrices=%d matrix_mode=%d\n", numw, BOUNDED_BUFFER_SIZE, NUMBER_OF_MATRICES, MATRIX_MODE);
  }
  else
  {
    // Override defaults based on provided command-line arguments
    if (argc==2) {
      numw=atoi(argv[1]);
      BOUNDED_BUFFER_SIZE=MAX;
      NUMBER_OF_MATRICES=LOOPS;
      MATRIX_MODE=DEFAULT_MATRIX_MODE;
    }
    if (argc==3) {
      numw=atoi(argv[1]);
      BOUNDED_BUFFER_SIZE=atoi(argv[2]);
      NUMBER_OF_MATRICES=LOOPS;
      MATRIX_MODE=DEFAULT_MATRIX_MODE;
    }
    if (argc==4) {
      numw=atoi(argv[1]);
      BOUNDED_BUFFER_SIZE=atoi(argv[2]);
      NUMBER_OF_MATRICES=atoi(argv[3]);
      MATRIX_MODE=DEFAULT_MATRIX_MODE;
    }
    if (argc==5) {
      numw=atoi(argv[1]);
      BOUNDED_BUFFER_SIZE=atoi(argv[2]);
      NUMBER_OF_MATRICES=atoi(argv[3]);
      MATRIX_MODE=atoi(argv[4]);
    }
    printf("USING: worker_threads=%d bounded_buffer_size=%d matrices=%d matrix_mode=%d\n", numw, BOUNDED_BUFFER_SIZE, NUMBER_OF_MATRICES, MATRIX_MODE);
  }
  
  // Seed the random number generator with the system time to ensure randomness
  srand((unsigned) time(NULL));
  
  // Allocate memory for the bounded buffer to store matrices
  bigmatrix = (Matrix **) malloc(sizeof(Matrix *) * BOUNDED_BUFFER_SIZE);
  
  printf("Producing %d matrices in mode %d.\n", NUMBER_OF_MATRICES, MATRIX_MODE);
  printf("Using a shared buffer of size=%d\n", BOUNDED_BUFFER_SIZE);
  printf("With %d producer and consumer thread(s).\n", numw);
  printf("\n");
  
  // Create arrays for producer and consumer threads
  pthread_t producers[numw];
  pthread_t consumers[numw];
  ProdConsStats prod_stats[numw];
  ProdConsStats cons_stats[numw];
  
  // Initialize producer threads and their statistics
  for (int i = 0; i < numw; i++) {
      prod_stats[i].sumtotal = 0;
      prod_stats[i].multtotal = 0;
      prod_stats[i].matrixtotal = 0;
      pthread_create(&producers[i], NULL, prod_worker, &prod_stats[i]);
  }
  
  // Initialize consumer threads and their statistics
  for (int i = 0; i < numw; i++) {
      cons_stats[i].sumtotal = 0;
      cons_stats[i].multtotal = 0;
      cons_stats[i].matrixtotal = 0;
      pthread_create(&consumers[i], NULL, cons_worker, &cons_stats[i]);
  }
  
  // Aggregate statistics from producers and consumers
  int total_prod_sum = 0, total_cons_sum = 0;
  int total_prod_mat = 0, total_cons_mat = 0, total_mult = 0;
  
  // Join producer threads and accumulate statistics
  for (int i = 0; i < numw; i++) {
      pthread_join(producers[i], NULL);
      total_prod_sum += prod_stats[i].sumtotal;
      total_prod_mat += prod_stats[i].matrixtotal;
  }
  
  // Join consumer threads and accumulate statistics
  for (int i = 0; i < numw; i++) {
      pthread_join(consumers[i], NULL);
      total_cons_sum += cons_stats[i].sumtotal;
      total_cons_mat += cons_stats[i].matrixtotal;
      total_mult += cons_stats[i].multtotal;
  }
  
  // Print the final aggregated statistics to verify correctness
  printf("Sum of Matrix elements --> Produced=%d = Consumed=%d\n", total_prod_sum, total_cons_sum);
  printf("Matrices produced=%d consumed=%d multiplied=%d\n", total_prod_mat, total_cons_mat, total_mult);
  
  // Free allocated memory for the bounded buffer
  free(bigmatrix);
  
  return 0;
}

