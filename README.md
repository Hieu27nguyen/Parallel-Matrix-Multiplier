Overview
This project implements a multi-threaded matrix multiplier using POSIX threads (pthreads) and a bounded buffer for inter-thread communication. The goal is to synchronize multiple producer and consumer threads that generate and multiply matrices concurrently, ensuring efficient parallel execution.

Features
Multi-threaded Execution: Uses multiple producer and consumer threads for parallel computation.
Bounded Buffer Synchronization: Implements a synchronized shared buffer using mutex locks and condition variables.
Random Matrix Generation: Matrices are dynamically generated with random dimensions (1x1 to 4x4).
Matrix Multiplication: Valid matrices are retrieved and multiplied, while invalid matrices are discarded.
Thread-safe Memory Management: Ensures proper allocation and deallocation of matrices to prevent memory leaks.
Implementation Details
Producer Threads:

Generate random matrices.
Store them in a bounded buffer.
Use mutexes and condition variables to manage access to the buffer.
Consumer Threads:

Retrieve matrices from the bounded buffer.
Perform matrix multiplication using MatrixMultiply().
Discard invalid matrices and free memory.
Print results using DisplayMatrix().
Synchronization Mechanisms:

Mutex Locks: Ensure exclusive access to shared resources.
Condition Variables: Allow threads to wait for available matrices or buffer space
