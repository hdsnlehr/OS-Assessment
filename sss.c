#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define MAX_SIZE 100

int A[MAX_SIZE];
int size = 0;
int totalSwaps = 0;
int currentIteration = 0;
pthread_mutex_t lock;
pthread_cond_t cond;
int turn = 1; // 1 for T1, 2 for T2

void readArrayFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    while (fscanf(file, "%d", &A[size]) == 1) {
        size++;
    }

    fclose(file);
}

void printArray() {
    for (int i = 0; i < size; i++) {
        printf("%d ", A[i]);
    }
    printf("\n");
}

int sortEvenPairs() {
    int swaps = 0;
    for (int i = 0; i < size - 1; i += 2) {
        if (A[i] > A[i + 1]) {
            int temp = A[i];
            A[i] = A[i + 1];
            A[i + 1] = temp;
            swaps++;
        }
    }
    return swaps;
}

int sortOddPairs() {
    int swaps = 0;
    for (int i = 1; i < size - 1; i += 2) {
        if (A[i] > A[i + 1]) {
            int temp = A[i];
            A[i] = A[i + 1];
            A[i + 1] = temp;
            swaps++;
        }
    }
    return swaps;
}

void* thread1Func(void* arg) {
    while (1) {
        pthread_mutex_lock(&lock);
        while (turn != 1 && currentIteration < 5) {
            pthread_cond_wait(&cond, &lock);
        }
        if (currentIteration >= 5) {
            pthread_mutex_unlock(&lock);
            break;
        }

        printf("Iteration %d\n", currentIteration + 1);
        printf("T1: A = ");
        printArray();
        int swaps = sortEvenPairs();
        printf("Sorted: A = ");
        printArray();
        printf("Swaps: %d\n", swaps);
        totalSwaps += swaps;
        printf("T1 completed.\n");

        turn = 2;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void* thread2Func(void* arg) {
    while (1) {
        pthread_mutex_lock(&lock);
        while (turn != 2 && currentIteration < 5) {
            pthread_cond_wait(&cond, &lock);
        }
        if (currentIteration >= 5) {
            pthread_mutex_unlock(&lock);
            break;
        }

        printf("T2: A = ");
        printArray();
        int swaps = sortOddPairs();
        printf("Sorted: A = ");
        printArray();
        printf("Swaps: %d\n", swaps);
        totalSwaps += swaps;
        printf("T2 completed.\n\n");

        currentIteration++;
        turn = 1;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    readArrayFromFile("ToSort.txt");

    printf("Initial Array:\n");
    printArray();
    printf("\n");

    pthread_t t1, t2;
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&t1, NULL, thread1Func, NULL);
    pthread_create(&t2, NULL, thread2Func, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final Sorted Array:\n");
    printArray();
    printf("Total Swaps: %d\n", totalSwaps);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    return 0;
}
