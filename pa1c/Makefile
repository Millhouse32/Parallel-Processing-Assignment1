all: pa1c

pa1c:
	mpicc pa1c.c -o pa1c

run1:
	mpirun -np 1 pa1c

run2: 
	mpirun -np 2 pa1c

run4:
	mpirun -np 4 pa1c

run8:
	mpirun -np 8 pa1c

clean:
	rm -f pa1c