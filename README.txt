sss.c
Parallel Sorting with Two Threads

Description
This program reads a list of integers from a file (ToSort.txt) and sorts them using two threads.
- Thread 1 (T1) sorts even-indexed pairs (0 & 1, 2 & 3 etc.).
- Thread 2 (T2) sorts odd-indexed pairs (1 & 2, 3 & 4 etc.).
The threads work across multipl iterations, using mutex locks and condition variables to synchronise.

At the end the program prints the final sorted array and the total number of swaps completed during sorting.

How to Compile
Use gcc to compile the program:
gcc -o sss sss.c -lpthread

How to Run
./sss ToSort

Author
Hudson Lehr
22564180@student.curtin.edu.au