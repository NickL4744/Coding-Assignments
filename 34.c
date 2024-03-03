#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    int my_rank, comm_sz;
    int n = 100;
    int* local_data;
    int* recv_data;
    int i, step;
    
    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    
    // Determine the number of elements each process handles
    int local_size = n / comm_sz;
    
    // Allocate memory for local and received data
    local_data = (int*) malloc(local_size * sizeof(int));
    recv_data = (int*) malloc(local_size * sizeof(int));
    
    // Initialize local data
    for (i = 0; i < local_size; i++) {
        local_data[i] = my_rank * local_size + i + 1;
    }
    
    // Perform reduction (butterfly algorithm)
    for (step = 1; step < comm_sz; step *= 2) {
        if (my_rank % (2 * step) == 0) {
            // Receive data from the neighbor
            MPI_Recv(recv_data, local_size, MPI_INT, my_rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // Perform local reduction
            for (i = 0; i < local_size; i++) {
                local_data[i] += recv_data[i];
            }
        } else if (my_rank % step == 0) {
            // Send local data to the neighbor
            MPI_Send(local_data, local_size, MPI_INT, my_rank - step, 0, MPI_COMM_WORLD);
        }
    }
    
    // Process 0 prints the global sum
    if (my_rank == 0) {
        int global_sum = 0;
        for (i = 0; i < local_size; i++) {
            global_sum += local_data[i];
        }
        printf("Global sum = %d\n", global_sum);
    }
    
    // Free allocated memory and finalize MPI
    free(local_data);
    free(recv_data);
    MPI_Finalize();
    return 0;
}