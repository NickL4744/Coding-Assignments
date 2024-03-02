#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MATRIX_SIZE 4

void printMatrix(double *mat, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%f ", mat[i * cols + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void matrixVectorMultiplication(double *matrixBlock, double *vector, double *result, int blockSize) {
    for (int i = 0; i < blockSize; i++) {
        result[i] = 0.0;
        for (int j = 0; j < blockSize; j++) {
            result[i] += matrixBlock[i * blockSize + j] * vector[j];
        }
    }
}

int main(int argc, char **argv) {
    int rank, comm_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    if (rank == 0) {
        double matrix[MATRIX_SIZE][MATRIX_SIZE] = {
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 10, 11, 12},
            {13, 14, 15, 16}
        };
        double vector[MATRIX_SIZE] = {1, 2, 3, 4};

        int blockSize = MATRIX_SIZE / comm_size;

        for (int dest = 1; dest < comm_size; dest++) {
            MPI_Send(&matrix[0][dest * blockSize], MATRIX_SIZE * blockSize, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
            MPI_Send(vector, MATRIX_SIZE, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
        }

        double result[blockSize];
        matrixVectorMultiplication(matrix[0], vector, result, blockSize);

        for (int i = 0; i < blockSize; i++) {
            printf("Process 0 Result %f\n", result[i]);
        }

        MPI_Status status;
        for (int source = 1; source < comm_size; source++) {
            MPI_Recv(result, blockSize, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
            for (int i = 0; i < blockSize; i++) {
                printf("Process %d Result %f\n", source, result[i]);
            }
        }
    } else {
        int blockSize = MATRIX_SIZE / comm_size;

        double matrixBlock[MATRIX_SIZE * blockSize];
        double vector[MATRIX_SIZE];
        MPI_Recv(matrixBlock, MATRIX_SIZE * blockSize, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(vector, MATRIX_SIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        double result[blockSize];
        matrixVectorMultiplication(matrixBlock, vector, result, blockSize);

        MPI_Send(result, blockSize, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}