// pa1c 
// Author Nicholas Miller

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "timer.h"

void Get_M_N(int* m, int* n, int my_rank, int comm_sz, MPI_Comm comm);

void Create_Array(int *m, int *n, int array[], int size, int my_rank);

void Calculate_Fact(int array[], int size, int my_rank);

int main(void) {

	int arraySizes[] = { 1000, 4000, 8000, 16000 };

	MPI_Init(NULL, NULL);

	int m, local_m, n, local_n;
	int my_rank, comm_sz;
	MPI_Comm comm;

	comm = MPI_COMM_WORLD;
	MPI_Comm_size(comm, &comm_sz);
	MPI_Comm_rank(comm, &my_rank);

	Get_M_N(&m, &n, my_rank, comm_sz, comm);

	for (int i = 0; i < 4; i++){
		int size = arraySizes[i];
		int array[size];

		Create_Array(&m, &n, array, size, my_rank);

		Calculate_Fact(array, size, my_rank);

		if (my_rank ==0)
			printf("Factorial with array size %d is done.\n", size);
	}

	MPI_Finalize();
}

void Get_M_N(int* m, int* n, int my_rank, int comm_sz, MPI_Comm comm) {

	if (my_rank == 0) {
		printf("Enter value for M (lower bound)\n");
		scanf("%d", m);
		printf("Enter value for N (upper bound)\n");
		scanf("%d", n);

		FILE* file;
		file = fopen("performance.txt", "w+");
		fprintf(file,"Value for M: %d\n", *m);
		fprintf(file,"Value for N: %d\n", *n);

		fclose(file);
	}
		MPI_Bcast(m, 1, MPI_INT, 0, comm);
		MPI_Bcast(n, 1, MPI_INT, 0, comm);
}

void Create_Array(int* m, int* n, int array[], int size, int my_rank) {
	if (my_rank == 0)
	{
		// seed random generator
		srand ( time(NULL) );

		for (int i = 0; i < size; i++){
			int num = (rand() %
	        (*n - *m + 1)) + *m;
	        array[i] = num;
		}

		// for (int i = 0; i < size; i ++){
		// 	printf("%d, ",array[i]);
		// }
	}
}

void Calculate_Fact(int array[], int size, int my_rank) {
	
	int tag = 100;

	if (my_rank == 0) {
		int fact[size];
		MPI_Send(array, size, MPI_INT, 1, tag, MPI_COMM_WORLD);
		MPI_Recv(fact, size, MPI_INT, 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for (int i = 0; i < size; i++) {
			//printf("Process %d, Result=%d\n", my_rank, fact[i]);
		}
	}
	else {
		int fact[size];
		MPI_Recv(array, size, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		// calc factorial
		for (int i = 0; i < size; i++) {
			int f = 1;
			for (int j = 1; j <= array[i]; ++j) {
				f *= j;
			}
			fact[i] = f;
		}
		MPI_Send(fact,size, MPI_INT,0,tag, MPI_COMM_WORLD);
		MPI_Comm_size (MPI_COMM_WORLD, &size);
	}
}