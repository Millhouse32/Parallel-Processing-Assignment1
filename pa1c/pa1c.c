// pa1c
// Author Nicholas Miller

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "timer.h"

void Get_m_n(int* m, int* n, int my_rank, int comm_sz, MPI_Comm comm);

void Create_Array(int* m, int* n, int array[], int size, int my_rank);

void Calc_Fact(int array[], int size, int my_rank, int comm_sz);

int a2[16000];

int main(void) {

	int arraySizes[] = { 1000, 4000, 8000, 16000 };

	int m, n;
	int my_rank, comm_sz;
	MPI_Comm comm;

	MPI_Init(NULL, NULL);

	comm = MPI_COMM_WORLD;
	MPI_Comm_size(comm, &comm_sz);
	MPI_Comm_rank(comm, &my_rank);

	Get_m_n(&m, &n, my_rank,  comm_sz, comm);

	//int size = 10;
	//int array[size];

	//Create_Array(&m, &n, array, size, my_rank);


	//Calc_Fact(array, size, my_rank, comm_sz);

	for (int i = 0; i < 4; i++) {
		int size = arraySizes[i];
		int array[size];

		Create_Array(&m, &n, array, size, my_rank);

		Calc_Fact(array, size, my_rank, comm_sz);

		if (my_rank == 0) {
			printf("Factorial with array size %d is done.\n", size);
		}

	}

	MPI_Finalize();

	return 0;
}

void Get_m_n(int* m, int* n, int my_rank, int comm_sz, MPI_Comm comm) {

	if (my_rank == 0) {
		printf("Enter value for M (lower bound)\n");
		scanf("%d", m);
		printf("Enter value for N (upper bound)\n");
		scanf("%d", n);

		FILE* file;
		file = fopen("performance.txt", "w+");
		fprintf(file, "Value for M (lower bound): %d\n", *m);
		fprintf(file, "Value for N (upper vound): %d\n", *n);

		fclose(file);
	}

	MPI_Bcast(m, 1, MPI_INT, 0, comm);
	MPI_Bcast(n, 1, MPI_INT, 0, comm);
}

void Create_Array(int* m, int* n, int array[], int size, int my_rank) {

	if (my_rank == 0) {

		// seed random generator
		srand ( time(NULL) );

		for (int i = 0; i < size; i++) {
			int num = (rand() %
			(*n - *m + 1)) + *m;
			array[i] =  num;
			//printf("%d, ", num);
		}

		for (int i = 0; i < size; i ++){
			//printf("%d, ", array[i]);
		}
	}
}

void Calc_Fact(int array[], int size, int my_rank, int comm_sz) {

	int elements_per_process, n_elements_received;

	MPI_Status status;

	    // master process
    if (my_rank == 0) {
        int index, i;
        elements_per_process = size / comm_sz;
  
        // check if more than 1 processes are run
        if (comm_sz > 1) {
            // distributes the portion of array
            // to child processes to calculate
            // their partial sums
            for (i = 1; i < comm_sz - 1; i++) {
                index = i * elements_per_process;
  
                MPI_Send(&elements_per_process,
                         1, MPI_INT, i, 0,
                         MPI_COMM_WORLD);
                MPI_Send(&array[index],
                         elements_per_process,
                         MPI_INT, i, 0,
                         MPI_COMM_WORLD);
            }
            // last process adds remaining elements
            index = i * elements_per_process;
            int elements_left = size - index;
  
            MPI_Send(&elements_left,
                     1, MPI_INT,
                     i, 0,
                     MPI_COMM_WORLD);
            MPI_Send(&array[index],
                     elements_left,
                     MPI_INT, i, 0,
                     MPI_COMM_WORLD);
        }
  
        // master process add its own sub array
        int sum;
        int fact = 1;
        for (i = 0; i < elements_per_process; i++){
            for (int j = array[i]; j > 1; j--)
                fact *= j;
            //printf("Factorial of %d is %d\n", array[i], fact);
            fact =1;
        }
  
        // collects partial sums from other processes
        int tmp;
        for (i = 1; i < comm_sz; i++) {
            MPI_Recv(&tmp, 1, MPI_INT,
                     MPI_ANY_SOURCE, 0,
                     MPI_COMM_WORLD,
                     &status);
            int sender = status.MPI_SOURCE;
  
            sum += tmp;
        }
  
        // prints the final sum of array
        //printf("Sum of array is : %d\n", sum);
    }
    // slave processes
    else {
        MPI_Recv(&n_elements_received,
                 1, MPI_INT, 0, 0,
                 MPI_COMM_WORLD,
                 &status);
  		// stores the received array segment
        // in local array a2
        MPI_Recv(&a2, n_elements_received,
                 MPI_INT, 0, 0,
                 MPI_COMM_WORLD,
                 &status);
  
        // calculates its partial sum
        int partial_sum = 0;
        int fact2 = 1;
        for (int i = 0; i < n_elements_received; i++){

            for (int j = a2[i]; j > 1; j--){
                fact2 *= j;
            }
            //printf("Factorial of %d is %d\n", a2[i], fact2);
            fact2 =1;
        }
  
        // sends the partial sum to the root process
        MPI_Send(&partial_sum, 1, MPI_INT,
                 0, 0, MPI_COMM_WORLD);
    }
}