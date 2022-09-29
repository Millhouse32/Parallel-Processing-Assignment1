#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mpi.h>

const int MAX_STRING = 100;

int main(void){
        // declare variables used
        int commSize;
        int myRank;
        // pointer to the input file
        FILE* inputFile;
        const int MAX_STRING = 100;

        char *testPTR = "Programming Assignment 1";
        char *text;
        char *contents;
        long numBytes;

        //Initialize MPI
        MPI_Init(NULL, NULL);

        //Determine num of processes
        MPI_Comm_size(MPI_COMM_WORLD, &commSize);
        MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

        if(myRank == 0){
                inputFile = fopen("hello.html", "r");

                if(NULL == inputFile){
                        printf("Error: Cannot open input file");
                        exit(0);
                }

                fseek(inputFile, 0L, SEEK_END);
                numBytes = ftell(inputFile);
                fseek(inputFile, 0L, SEEK_SET);

                text = (char*)calloc(numBytes, sizeof(char));

                fread(text, sizeof(char), numBytes, inputFile);
                fclose(inputFile);
                for(int thread = 1; thread < commSize; thread++){
                        MPI_Send(text, strlen(text)+1, MPI_CHAR, thread, 0, MPI_COMM_WORLD);
                }
        }
        else {
                MPI_Recv(contents, numBytes, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                char *string;
                int length = strlen(contents);
                int lengthHalfed = length/2;
                int secondaryLength = length - lengthHalfed;

                if(myRank == 1){
                        string = malloc(lengthHalfed + 1);
                        memcpy(string, contents, lengthHalfed);
                }
                else {
                        string = malloc(secondaryLength + 1);
                        memcpy(string, contents + lengthHalfed, secondaryLength);
                }
                printf("Process Number: %d\n", myRank);
                printf("%s\n", string);
        }

        MPI_Finalize();

        return 0;
}