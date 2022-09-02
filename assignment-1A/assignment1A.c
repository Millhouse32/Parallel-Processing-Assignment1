#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAX_STRING = 100;

int main(void){
  int comm_sz;  // number of process
  int my_rank; // process rank
  FILE* file; // hello.html

  const int MAX_STRING = 100;

  char *test = "hello world";
  char *text;
  char *contents;
  long numbytes;

  // Start up MPI
  MPI_Init(NULL, NULL);

  // Get the number of processes
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  if (my_rank != 0) {
    MPI_Recv(contents, numbytes, MPI_CHAR, 0, 0, MPI_COMM_WORLD,
      MPI_STATUS_IGNORE);
    printf("Process number :: %d \n", my_rank);
    printf("%s", contents);
  }

  else {
    file = fopen("hello.html", "r");

    if (NULL == file){
      printf("Error opening file");
      exit(0);
    }

    fseek(file, 0L, SEEK_END);
    numbytes = ftell(file);
    fseek(file, 0L, SEEK_SET);

    text = (char*)calloc(numbytes, sizeof(char));

    fread(text, sizeof(char), numbytes, file);
    fclose(file);
    for (int dest = 1; dest < comm_sz; dest++) {
      MPI_Send(text, strlen(text)+1, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
    }
  }

  MPI_Finalize();

  return 0;
}
