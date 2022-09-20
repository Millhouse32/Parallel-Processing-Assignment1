// pa1c 
// Author Nicholas Miller

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "timer.h"

void Get_M_N(int* m, int* n, int my_rank, int comm_sz, MPI_Comm comm);

void Create_Array(int *m, int *n, int array[], int size);

int main(void) {
	int m, local_m, n, local_n;
	int my_rank, comm_sz;
	MPI_Comm comm;
	int array[1000];

	MPI_Init(NULL, NULL);
	comm = MPI_COMM_WORLD;
	MPI_Comm_size(comm, &comm_sz);
	MPI_Comm_rank(comm, &my_rank);

	Get_M_N(&m, &n, my_rank, comm_sz, comm);

	printf("M :: %d\n", m);
	printf("N :: %d\n", n);

	Create_Array(&m, &n, array, 1000);

	MPI_Finalize();
}

void Get_M_N(int* m, int* n, int my_rank, int comm_sz, MPI_Comm comm) {

	if (my_rank == 0) {
		printf("Enter value for M\n");
		scanf("%d", m);
		printf("Enter value for N\n");
		scanf("%d", n);

		FILE* file;
		file = fopen("performance.txt", "w+");
		fprintf(file,"Value for M: %d\n", *m);
		fprintf(file,"Value for N: %d\n", *n);

		fclose(file);

		MPI_Bcast(m, 1, MPI_INT, 0, comm);
		MPI_Bcast(n, 1, MPI_INT, 0, comm);

	}
}

void Create_Array(int* m, int* n, int array[], int size) {
	
	if (my_rank == 0) {
		for (int i = 0; i < size; i++){
			int num = (rand() %
	        (*n - *m + 1)) + *m;
	        array[i] = num;
		}

		for (int i = 0; i < size; i++) {
			printf("%d, ", array[i]);
		}
	}
}