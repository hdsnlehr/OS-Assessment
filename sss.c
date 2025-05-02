

#include <stdio.h>      //For input and output functions (printf, scanf, etc.)
#include <stdlib.h>     //For memory allocation functions (malloc, free, etc.)
#include <pthread.h>    //For using POSIX threads (pthread_create, pthread_join, etc.)

#define MAX_SIZE 200    // Maximum size of the array A

// Global variables
int A[MAX_SIZE];            //Array to hold the numbers read from the file
int size = 0;               //Current size of the array A
int totalSwaps = 0;         //Total number of swaps made during sorting
int t1Swaps = 0;            //Number of swaps made by thread T1
int t2Swaps = 0;            //Number of swaps made by thread T2
int currentIteration = 0;   //Tracks the current iteration of sorting
int turn = 1;               // 1 for T1, 2 for T2 
int stopSorting = 0;        // Flag to indicate when to stop sorting

pthread_t t1, t2;           // Thread identifiers for T1 and T2
pthread_mutex_t lock;       // Mutex lock to protect shared variables
pthread_cond_t cond;        // Condition variable for signaling between threads

// Function to read integers from a file and store them in the array A
void readArrayFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");  // Open the file for reading
    if (!file) {                        // Check if the file was opened successfully
        perror("Error opening file");   // Print error message if file opening fails
        exit(EXIT_FAILURE);             // Exit the program if file opening fails
    }

    while (fscanf(file, "%d", &A[size]) == 1) { // Read integers from the file until EOF
        size++;                                 // Increment the size of the array for each integer read
    }

    fclose(file); // Close the file after reading
}

// Function to print the array A
void printArray() {
    for (int i = 0; i < size; i++) {    // Loop through each element in the array A
        printf("%d ", A[i]);            // Print the current element
    }
    printf("\n");                       // Newline after printing the array
}

// Function to sort even-indexed pairs (i and i+1 where i is even) in array A
int sortEvenPairs() {
    int swap = 0;                           // Initialize swap count to 0
    for (int i = 0; i < size - 1; i += 2) { // Loop through even indices
        if (A[i] > A[i + 1]) {              // Check if the current element is greater than the next element
            int temp = A[i];                // Store the current element in a temporary variable
            A[i] = A[i + 1];                // Swap the current element with the next element
            A[i + 1] = temp;                // Assign the temporary variable to the next element
            swap++;                         // Increment the swap count
        }
    }
    return swap;                            // Return the total number of swaps made in this iteration
}

// Function to sort odd-indexed pairs (i and i+1 where i is odd) in array A
int sortOddPairs() {
    int swap = 0;                           // Initialize swap count to 0
    for (int i = 1; i < size - 1; i += 2) { // Loop through odd indices
        if (A[i] > A[i + 1]) {              // Check if the current element is greater than the next element
            int temp = A[i];                // Store the current element in a temporary variable
            A[i] = A[i + 1];                // Swap the current element with the next element
            A[i + 1] = temp;                // Assign the temporary variable to the next element
            swap++;                         // Increment the swap count
        }
    }
    return swap;                            // Return the total number of swaps made in this iteration
}

// Thread 1 function: sorts even-indexed pairs in the array A
void* thread1Func(void* arg) {
    t1 = pthread_self();                                // Get the thread ID of T1
    while (1) {                                         // Infinite loop to keep the thread running until a break condition is met
        pthread_mutex_lock(&lock);                      // Lock the mutex to protect shared variables
        while (turn != 1 && !stopSorting) {             // Wait if it's not T1's turn and iterations not finished
            pthread_cond_wait(&cond, &lock);            // Wait for signal and release mutex during waiting
        }
        
        if (stopSorting) {                              // Check if sorting should be stopped
            pthread_mutex_unlock(&lock);                // Unlock the mutex before breaking
            break;                                      // Break the loop if sorting is stopped
        }

        printf("Iteration %d\n", currentIteration + 1);
        printf("T1: A = ");
        printArray();
        int swap = sortEvenPairs();      // Sort even-indexed pairs in the array A
        printf("Sorted: A = ");
        printArray();
        printf("Swaps: %d\n", swap);
        t1Swaps += swap;                 // Update the number of swaps made by T1
        totalSwaps += swap;              // Update the total number of swaps made
        printf("T1 completed.\n");

        int thisT1Swap = swap;           // Store the number of swaps made by T1 in this iteration for termination later
        turn = 2;                        // Set turn to 2 for T2 to run next
        pthread_cond_signal(&cond);      // Signal T2 to wake up and run
        pthread_mutex_unlock(&lock);     // Unlock the mutex to allow other threads to proceed
    }
    return NULL;                         // Return NULL to indicate thread completion
}

// Thread 2 function: sorts odd-indexed pairs in the array A
void* thread2Func(void* arg) {
    t2 = pthread_self();                                // Get the thread ID of T2
    while (1) {                                         // Infinite loop to keep the thread running until a break condition is met
        pthread_mutex_lock(&lock);                      // Lock the mutex to protect shared variables
        while (turn != 2 && !stopSorting) {             // Wait if it's not T2's turn and iterations not finished
            pthread_cond_wait(&cond, &lock);            // Wait for signal and release mutex during waiting
        }
        
        if (stopSorting) {                              // Check if sorting should be stopped
            pthread_mutex_unlock(&lock);                // Unlock the mutex before breaking
            break;                                      // Break the loop if sorting is stopped
        }

        printf("T2: A = ");
        printArray();
        int swap = sortOddPairs();       // Sort odd-indexed pairs in the array A
        printf("Sorted: A = ");
        printArray();
        printf("Swaps: %d\n", swap);
        t2Swaps += swap;                 // Update the number of swaps made by T2
        totalSwaps += swap;              // Update the total number of swaps made
        printf("T2 completed.\n\n");

        if (swap == 0 && sortEvenPairs() == 0) {    // Check if no swaps were made in this iteration and even pairs are also sorted
            stopSorting = 1;                        // Set the stopSorting flag to indicate sorting should stop
            pthread_cond_broadcast(&cond);          // Signal all threads to wake up and finish
            pthread_mutex_unlock(&lock);            // Unlock the mutex before breaking
            break;
        }

        currentIteration++;             // Increment the current iteration count
        turn = 1;                       // Set turn to 1 for T1 to run next
        pthread_cond_signal(&cond);     // Signal T1 to wake up and run
        pthread_mutex_unlock(&lock);    // Unlock the mutex to allow other threads to proceed
    }
    return NULL;                        // Return NULL to indicate thread completion
}

// Main function: entry point of the program
int main() {
    readArrayFromFile("ToSort.txt");                    // Read the array from the file "ToSort.txt"

    printf("Initial Array:\n");
    printArray();                                       // Print the initial array before sorting
    printf("\n");

    pthread_mutex_init(&lock, NULL);                    // Initialize the mutex lock
    pthread_cond_init(&cond, NULL);                     // Initialize the condition variable

    pthread_create(&t1, NULL, thread1Func, NULL);       // Create thread T1
    pthread_create(&t2, NULL, thread2Func, NULL);       // Create thread T2

    pthread_join(t1, NULL);                             // Wait for thread T1 to finish
    pthread_join(t2, NULL);                             // Wait for thread T2 to finish
    
    printf("Thread 1 ID: %p, total number of swaps = %d\n", t1, t1Swaps); // Print the thread ID and total swaps made by T1
    printf("Thread 2 ID: %p, total number of swaps = %d\n\n", t2, t2Swaps); // Print the thread ID and total swaps made by T2


    printf("Final Sorted Array:\n");
    printf("A = ");
    printArray();                                       
    printf("Total swaps = %d\n", totalSwaps);           // Print the total number of swaps made during sorting

    pthread_mutex_destroy(&lock);                       // Destroy the mutex lock
    pthread_cond_destroy(&cond);                        // Destroy the condition variable

    return 0;                                           // Return 0 to indicate successful completion of the program
}