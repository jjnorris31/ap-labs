#define _GNU_SOURCE
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "logger.h"

long *readMatrix(char *filename); // read matrix file and returns an long type array with content of matrix
long *getColumn(int col, long *matrix); // returns the specified array that represents a column in the matrix
long *getRow(int row, long *matrix); // returns the specified array that represents a row in the matrix array
int getLock(void); // Search for an available buffer, if so it returns the available lock id which is the same buffer id, otherwise returns -1
int releaseLock(int lock); // Releases a buffer and unlock the mutex. Returns 0 for successful unlock, otherwise -1
long dotProduct(long *vec1, long *vec2); // Given 2 arrays of 2000 lenght, it calculates the dot product and returns it as a long type value
long *multiply(long *matA, long *matB); // Here's where all the magic happens. Explained in Matrix multiplication section
int saveResultMatrix(long *result); // Saves result matrix into a new result.dat file, return 0 for a successful operation, otherwise it will return -1
void *threadHandler(void *arg);

int NUM_BUFFERS; // Number of temporal buffers for vectors that will be used for each dot product
long **buffers; // An array of NUM_BUFFERS pointers to the available buffers that will serve as temporal vectors for dot product operations
pthread_mutex_t *mutexes; // Mutexes that will help to know which buffer is available
long *result; // Will store the result of matA and matB multiplication
pthread_t threads[2000];
int N_NUMBER = 2000; // 2000 constant

int main(int argc, char **argv) {
    if (argc != 3) {
        errorf("multiplier: USAGE ./multiplier -n NUMBER\n");
        exit(EXIT_FAILURE);
    }

    NUM_BUFFERS = strtol(argv[2], NULL, 10);
    if (NUM_BUFFERS < 8){
        errorf("NUM_BUFFERS should be > 8\n");
        exit(EXIT_FAILURE);
    }

    infof("NUM_BUFFERS = %d\n", NUM_BUFFERS);

    buffers = (long **)malloc(NUM_BUFFERS * sizeof(long *));
    mutexes = (pthread_mutex_t *) malloc(NUM_BUFFERS * sizeof(pthread_mutex_t));

    for (int i = 0; i < NUM_BUFFERS; i++) {
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
        mutexes[i] = mutex;
        pthread_mutex_init(&mutexes[i], NULL);
    }

    infof("All pthreads created\n");

    long *matrixA, *matrixB;
    matrixA = readMatrix("matA.dat");
    matrixB = readMatrix("matB.dat");

    infof("Processing matrix\n");

    result = multiply(matrixA, matrixB);
    infof("Saving result...\n");
    saveResultMatrix(result);
    infof("Matrix processed\n");

    free(matrixA);
    free(matrixB);
    free(buffers);
    free(mutexes);
    free(result);
    return 0;
}

long *readMatrix(char *filename) {
    FILE *file;
    char *line = NULL;
    size_t len = 0;
    if ((file = fopen(filename, "r")) == NULL) {
        panicf("Can't open file: %s\n", filename);
        return NULL;
    }

    long *matrix;
    matrix = (long *)malloc((N_NUMBER * N_NUMBER) * sizeof(long));
    int i = 0;

    while (getline(&line, &len, file) != -1) {
        matrix[i] = strtol(line, NULL, 10);
        i++;
    }

    free(line);
    return matrix;
}

long *getColumn(int col, long *matrix) {
    if (col < 0 || col > N_NUMBER) {
        panicf("Col must be 0 > col > 2000\n");
        exit(EXIT_FAILURE);
    }

    size_t offset = col - 1;
    long *mycol;
    mycol = (long *)malloc(N_NUMBER * sizeof(long));

    for (int i = 0; i < N_NUMBER; i++) {
        mycol[i] = matrix[offset];
        offset += N_NUMBER;
    }

    return mycol;
}

long *getRow(int row, long *matrix) {
    if (row < 0 || row > N_NUMBER) {
        panicf("Row must be something like 0 > row > 2000\n");
        exit(EXIT_FAILURE);
    }
    size_t offset = ((2 * row) - 2) * 1000;
    long *myrow;
    myrow = (long *)malloc(N_NUMBER * sizeof(long));

    for (int i = 0; i < N_NUMBER; i++) {
        myrow[i] = matrix[offset++];
    }
    return myrow;
}

int getLock(void) {
    for (int i = 0; i < NUM_BUFFERS; i++) {
        if (pthread_mutex_trylock(&mutexes[i]) == 0)
            return i;
    }
    return -1;
}

int releaseLock(int lock) {
    if (pthread_mutex_unlock(&mutexes[lock]) == 0) {
        return 0;
    }
    return -1;
}

long dotProduct(long *vec1, long *vec2) {
    long result = 0;
    for (int i = 0; i < N_NUMBER; i++)
        result += vec1[i] * vec2[i];
    return result;
}

struct MatrixStruct {
    long *matA;
    long *matB;
    size_t rowPosition;
    size_t colPosition;
    long *result;
};


long *multiply(long *matA, long *matB) {
    result = (long *)malloc((N_NUMBER * N_NUMBER) * sizeof(long));

    for (size_t i = 0; i < N_NUMBER; i++) {
        for (size_t j = 0; j < N_NUMBER; j++) {
            struct MatrixStruct *currentStruct = (struct MatrixStruct *) malloc(sizeof(struct MatrixStruct));
            currentStruct->matA = matA;
            currentStruct->matB = matB;
            currentStruct->rowPosition = i + 1;
            currentStruct->colPosition = j + 1;
            currentStruct->result = result;
            pthread_create(&threads[j], NULL, threadHandler, (void *)currentStruct);
        }

        for (size_t j = 0; j < N_NUMBER; j++) {
            pthread_join(threads[j], NULL);
        }
        printf("\r%ld%%", (i * 100) / 1999);
        fflush(stdout);
    }

    printf("\n");
    return result;
}

int saveResultMatrix(long *result) {
    FILE *newFile;
    if ((newFile = fopen("results.dat", "w")) == NULL) {
        errorf("error: can't create results.dat\n");
        return -1;
    }

    for (size_t i = 0; i < (N_NUMBER * N_NUMBER); i++)
        fprintf(newFile, "%ld\n", result[i]);

    fclose(newFile);
    return 0;
}

void freeBuffer(long *bufferRow, long *bufferCol, void *arg) {
    free(bufferRow);
    free(bufferCol);
    free(arg);
}

void *threadHandler(void *currentStruct) {
    struct MatrixStruct *data = (struct MatrixStruct *)currentStruct;
    long index;

    int lockRow;
    int lockColumn;
    while ((lockRow = getLock()) == -1);
    while ((lockColumn = getLock()) == -1);
    buffers[lockRow] = getRow(data->rowPosition, data->matA);
    buffers[lockColumn] = getColumn(data->colPosition, data->matB);

    index = ((((data->rowPosition - 1) * N_NUMBER) + data->colPosition) - 1);
    data->result[index] = dotProduct(buffers[lockRow], buffers[lockColumn]);

    freeBuffer(buffers[lockRow], buffers[lockColumn], currentStruct);
    while (releaseLock(lockRow) != 0);
    while (releaseLock(lockColumn) != 0);
}
