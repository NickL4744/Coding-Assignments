#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NUM_BINS 5
#define DATA_SIZE 20

// Function to calculate histogram for a portion of data
void calculate_histogram(int data[], int histogram[], int size, int num_bins) {
    for (int i = 0; i < size; i++) {
        // Calculate the bin index for the current data point
        int bin_index = data[i] * num_bins / 5;
        histogram[bin_index]++;
    }
}

// Function to print histogram
void print_histogram(int histogram[], int num_bins) {
    printf("Histogram:\n");
    for (int i = 0; i < num_bins; i++) {
        printf("%d: %d\n", i, histogram[i]);
    }
}

int main(int argc, char **argv) {
    int rank, size;
    int data[DATA_SIZE] = {1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3, 4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9};
    int histogram[NUM_BINS] = {0};

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Calculate chunk size for each process
    int chunk_size = DATA_SIZE / size;
    int *recv_counts = malloc(size * sizeof(int));
    int *displs = malloc(size * sizeof(int));

    // Calculate the counts and displacements for scatter operation
    for (int i = 0; i < size; i++) {
        recv_counts[i] = chunk_size;
        displs[i] = i * chunk_size;
    }

    // Scatter the data to all processes
    int local_data[chunk_size];
    MPI_Scatterv(data, recv_counts, displs, MPI_INT, local_data, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate histogram for local data
    calculate_histogram(local_data, histogram, chunk_size, NUM_BINS);

    // Reduce histograms from all processes to obtain the final histogram
    int final_histogram[NUM_BINS] = {0};
    MPI_Reduce(histogram, final_histogram, NUM_BINS, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    // Print final histogram on root process
    if (rank == 0) {
        print_histogram(final_histogram, NUM_BINS);
    }

    // Finalize MPI
    MPI_Finalize();
    return 0;
}