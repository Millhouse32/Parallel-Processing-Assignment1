#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Check_for_error(int local_ok, char fname[], char message[],
        MPI_Comm comm);

void Get_dims(int* m_p, int* local_m_p, int* n_p, int* local_n_p,
        int my_rank, int comm_sz, MPI_Comm comm);
void Allocate_arrays(double** local_A_pp, double**, local_x_pp,
    double** local_y_pp, int local_m, int n, int local_n,
    MPI_Comm comm);
void Read_matrix(char prompt[], double local_A[], int m, int local_m,
    int n, int my_rank, MPI_Comm comm);
void Read_vector(char prompt[], double local_A[], int n, int local_n,
    int my_rank, MPI_Comm comm);
void Print_matrix(char title[], double local_A[], int m, int local_m,
    int n, int my_rank, MPI_Comm comm);
void Print_vector(char title[], double local_vec[], int n, int local_n, int my_rank,
    MPI_Comm comm);
void Mat_vect_mult(double local_A[], double local_x[], double local_y[],
    int local_m, int n, int local_n, MPI_Comm comm);


int main(void) {
    double* local_A;
    double* local_x;
    double* local_y;
    int m, local_m, n, local_n;
    int my_rank, comm_sz;
    MPI_Comm comm; 

    const int MAX_STRING = 100;

    // Start up MPI
    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    // Get the number of process
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    Get_dims(&m, &local_m, &n, &local_n, my_rank, comm_sz, comm);
    Allocate_arrays(&local_A, &local_x, &local_y, local_m, n, local_n, comm);
    Read_matrix("A", local_A, m, local_m, n, my_rank, comm);
    # ifdef DEBUG
    Print_matrix("A", local_A, m, local_m, n, my_rank, comm);
    # endif
    Read_vector("x", local_x, n, local_n, my_rank, comm);
    # ifdef DEBUG
    Print_vector("x", local_x, n, local_n, my_rank, comm);
    # endif

    Mat_vect_mult(local_A, local_x, local_y, local_m, n, local_n, comm);

    Print_vector("y", local_y, m, local_m, my_rank, comm);

    free(local_A);
    free(local_x);
    free(local_y);
    MPI_Finalize();
    return 0;
}    /* main */

void Check_for_error (
      int local_ok
      char fname[]
      char message[]
      MPI_Comm comm) {

    int ok;
    MPI_Allreduce(&local_ok, &ok, 1, MPI_INT, MPI_MIN, comm);
    if (ok == 0) {
        int my_rank;
        MPI_Comm_rank(comm, &my_rank);
        if (my_rank == 0) {
            fprintf(stderr, "Proc %d > In %s, %s\n", my_rank, fname, message);
            fflush(stderr);
        }
        MPI_Finialize();
        exit(-1);
    }
} // Check_for_error

void Get_dims (
    int* m_p,
    int* local_m_p
    int* n_p,
    int* local_n_p,
    int my_rank,
    int comm_sz,
    MPI_Comm comm) {

    int local_ok = 1;
    FILE* file;
    if (my_rank == 0) {
            file = fopen("my-data.txt", "r");
        if (NULL == file) {
            printf("Error opening file");
            exit(0);
        }
        fscanf(file, "%d", m_p);
        fscanf(file, "%d", n_p);
    }
    MPI_Bcast(m_p, 1, MPI_INT, 0, comm);
    MPI_Bcast(n_p, 1, MPI_INT, 0, comm);

    if (*m_p <= 0 || *n_p <= 0){
        local_ok = 0;
    }
    Check_for_error(local_ok, "Get_dims", "m and n must be positive", comm);

    *local_m_p = *m_p/comm_sz;
    *local_n_p = *n_p/comm_sz;

    fclose(file);
} // Get_dims

void Allocate_arrays(
    double** local_A_pp,
    double** local_x_pp,
    double** local_y_pp,
    int local_m,
    int n,
    int local_n,
    MPI_Comm comm) {

    int loal_ok = 1;

    *local_A_pp = malloc(local_m*n*sizeof(double));
    *local_x_pp == malloc(local_n*sizeof(double));
    *local_y_pp == malloc(local_m*sizeof(double));

    if (*local_A_pp == NULL || local_x_pp == NULL ||
        local_y_pp == NULL) {
        local_ok = 0;
    }
    Check_for_error(local_ok, "Allocate_arrays", "Can't allocate local arrays", comm);
} // Allocate_arrays

void Read_matrix (
    char prompt[],
    double local_A[],
    int m,
    int local_m,
    int n,
    int my_rank,
    MPI_Comm comm) {

    double* A = NULL;
    int local_ok = 1;
    int i, j;

    if (my_rank == 0) {
        A = malloc(m*n*sizeof(double));
        if (A == NULL) {
            local_ok = 0;
        }
        Check_for_error(local_ok, "Read_matrix",
            "Can't allocate temporary matrix", comm);
        
        FILE* file;
        file = fopen("my-data.txt", "r");
        if (NULL == file){
            local_ok = 0;
        }
        Check_for_error(local_ok, "Read_matrix",
            "Error opening my-data.txt file", comm);

        char ignore[1024];
        fgets(ignore, sizeof(ignore), file);
        fgets(ignore, sizeof(ignore), file);

        int num;

        for(int i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                fscanf(file, "%d", &num);
                &A[i*n+j] == num;
            }
            MPI_Scatter(A, local_m*n, MPI_DOUBLE,
                local_A, local_m*n, MPI_DOUBLE, 0, comm);
            free(A);
        }
    } else {
        Check_for_error(local_ok, "Read_matrix", 
            "Can't allocate temporary matrix", comm);
        MPI_Scatter(A, local_m*n, MPI_DOUBLE,
            local_A, local_m*n, MPI_DOUBLE, 0, comm);
    }
} // Read_matrix

void Read_vector (
    char prompt[]
    double local_vec[],
    int n,
    int local_n,
    int my_rank,
    MPI_Comm comm) {

    double* vec = NULL;
    int i, local_ok = 1;

    if (my_rank == 0) {
        vec = malloc(n*sizeof(double));
        if (vec == NULL) {
            local_ok = 0;
        }
        Check_for_error(local_ok, "Read_vector",
            "Can't allocate temporary vector", comm);

        FILE* file;
        file = fopen("my-data.txt", "r");
        if (file == NULL) {
            local_ok = 0;
        }
        Check_for_error(local_ok, "Read_matrix",
            "Error opening my-data.txt file", comm);
        char ignore[1024];
        fgets(ignore, sizeof(ignore), file);
        fgets(ignore, sizeof(ignore), file);
        fgets(ignore, sizeof(ignore), file);
        fgets(ignore, sizeof(ignore), file);

        int num;
        for (int i = 0; i < n; i++) {
            fscanf(file, "%d", &num);
            &vec[i] = num;
        }
        MPI_Scatter(vec, local_n, MPI_DOUBLE,
            local_vec, local_n, MPI_DOUBLE, 0, comm);
        free(vec);
    } else {
        Check_for_error(local_ok, "Read_vector",
            "Can't allocate temporary vector", comm);
        MPI_Scatter(vec local_n, MPI_DOUBLE,
            local_vec, local_n, MPI_DOUBLE, 0, comm);
    }
} // Read_vector

void Print_matrix (
    char title[],
    double local_A[],
    int m,
    int local_m,
    int n,
    int my_rank,
    MPI_Comm comm) {

    double *A = NULL;
    int i, j, local_ok = 1;

    if (my_rank == 0) {
        A = malloc(m*n*sizeof(double));
    
        Check_for_error(local_ok, "Print_matrix",
            "Can't allocatte temporary matrix", comm);
        MPI_Gather(local_A, local_m*n, MPI_DOUBLE,
            A, local_m*n, MPI_DOUBLE, 0, comm);
        printf("\nThe matrix %s\n", title);
        for (int i = 0; i < m; i++) {
            for (j = 0; j < n; j++) {
                printf("%f ", A[i*n+j]);
            }
            printf("\n");
        }
        printf("\n");
        free(A);
    } else {
        Check_for_error(local_ok, "Print_matrix",
            "Can't allocate temporary matrix", comm);
        MPI_Gather(local_A, local_m*n, MPI_DOUBLE,
            A, local_m*n, MPI_DOUBLE, 0, comm);
    }
} // Print_matrix

void Print_vector (
    char title[],
    double local_vec[],
    int n,
    int local_n,
    int my_rank,
    MPI_Comm comm) {

    

}

    if (my_rank == 0) {
        file = fopen("my-data.txt", "r");

        if (NULL == file) {
            printf("Error opening file");
            exit(0);
        }
        
        int lineCount = 0;
        fscanf(file, "%d", &row);
        fscanf(file, "%d", &col);

        int matrix[row][col];
        int num;
        int vector[col];
        
        for (int i = 0; i < row; i++){
            for(int j = 0; j < col; j++){
                fscanf(file, "%d", &num);
                matrix[i][j] = num;
            }
        }

        for (int i = 0; i < col; i ++){
            fscanf(file, "%d", &num);
            vector[i] = num;
        }

        fclose(file);

        // printing matrix and vector
        printf("Matrix:\n");
        for(int i = 0; i < row; i++){
            for(int j = 0; j < col; j++){
                printf("%d ", matrix[i][j]);
            }
            printf("\n");
        }
        printf("\n");
        printf("Vector: ");
        for(int i = 0; i < col; i++) {
            printf("%d ", vector[i]);
        }
        printf("\n");
        printf("Results:\n");
    }
    MPI_Finalize();

    return 0;
}