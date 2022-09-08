#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    int comm_sz; // number of process
    int my_rank; // process rank
    FILE* file; // my-data.txt
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int row;
    int col;

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
        
        int lineCount = 0;
        fscanf(file, "%d", &row);
        fscanf(file, "%d", &col);


        printf("Num Row: %d",row);
        printf("\n");
        printf("Num Col: %d", col);
        printf("\n");
        int matrix[row][col];
        int num;
        int vector[];
        
        for (int i = 0; i < row; i++){
            for(int j = 0; j < col; j++){
                fscanf(file, "%d", &num);
                matrix[i][j] = num;
            }
        }

        fclose(file);

        // printing matrix
        for(int i = 0; i < row; i++){
            for(int j = 0; j < col; j++){
                printf("%d ", matrix[i][j]);
            }
            printf("\n");
        }
    }
    MPI_Finalize();

    return 0;
}