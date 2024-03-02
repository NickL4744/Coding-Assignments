#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int my_rank, comm_sz;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int local_sum = my_rank; 


    for (int stride = 1; stride < comm_sz; stride *= 2) {
        if (my_rank % (2 * stride) == 0) {
            int partner = my_rank + stride;
            if (partner < comm_sz) {
                int received_sum;
                MPI_Recv(&received_sum, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_sum += received_sum;
            }
        } else {
            int partner = my_rank - stride;
            MPI_Send(&local_sum, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
            break; 
        }
    }

    if (my_rank == 0) {
        printf("Global sum: %d\n", local_sum);
    }

    MPI_Finalize();
    return 0;
}