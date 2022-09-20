// pa1c 
// Author Nicholas Miller

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <timer.h>

void Get_M_N(int* m, int* n, int* local_m, int* local_n, int my_rank,
int comm_sz, MPI_Comm comm);

int main(void) {
	int m, local_m, n, local_n;
	int my_rank, comm_sz;
	MPI_Comm comm;

	MPI_Init(NULL, NULL);
	comm = MPI_COMM_WORLD;
	MPI_Comm_size(comm, &comm_sz);
	MPI_Comm_rank(comm, &my_rank);

	Get_M_N(&m, &local_m, &n, &local_n, my_rank, comm_sz, comm);
}

Get_M_N(int* m, int* n, int* local_m, int* local_n, int my_rank,
int comm_sz, MPI_Comm comm) {

	if (my_rank == 0) {
		printf("Enter value for M\n");
		scanf("%d", m);
		printf("Enter value for N\n");
		scanf("%d", n);

		MPI_Bcast(m, 1, MPI_INT, 0, comm);
		MPI_Bcast(n, 1, MPI_INT, 0, comm);
	}
}