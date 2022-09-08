#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    int comm_sz; // number of process
    int my_rank; // process rank
    FILE* file; // my-data.txt
    long numbytes;
    char *text;

    const int MAX_STRING = 100;

    // Start up MPI
    MPI_Init(NULL, NULL);

    // Get the number of process
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        file = fopen("my-data.txt", "r");

        if (NULL == file) {
            printf("Error opening file");
            exit(0);
        }

        fseek(file, 0L, SEEK_END);
        numbytes = ftell(file);
        fseek(file, 0L, SEEK_SET);

        text = (char*)calloc(numbytes, sizeof(char));

        fread(text, sizeof(char), numbytes, file);
        fclose(file);

        printf("%s", text);
    }
    MPI_Finalize();

    return 0;
}