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

    char *str;
    int len = strlen(contents);
    int len1 = len/2;
    int len2 = len - len1;

    if (my_rank == 1){
      str = malloc(len1+1);
      memcpy(str, contents, len1);
      //str[len1] = '\0';
    }
    else {
      str = malloc(len2+1);
      memcpy(str, contents+len1, len2);
      //str[len2] = '\0';
    }

    printf("Process number :: %d\n", my_rank);
    printf("%s\n", str);
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
