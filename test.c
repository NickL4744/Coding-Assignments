#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int rank, comm_sz;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int local_sum = rank; // Initialize local sum with rank

    // Perform tree-structured global sum
    int d = 1;
    while (d < comm_sz) {
        int source = rank + d;
        int dest = rank - d;
        if (source < comm_sz) {
            int received_sum;
            MPI_Recv(&received_sum, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            local_sum += received_sum;
        }
        if (dest >= 0) {
            MPI_Send(&local_sum, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            break;
        }
        d *= 2;
    }

    // Process 0 prints the final result
    if (rank == 0) {
        printf("Global sum: %d\n", local_sum);
    }

    MPI_Finalize();
    return 0;
}