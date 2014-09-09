#include <stdio.h>
#include <mpi.h>
#include <math.h>

float f(float x){
	return log(x) + 2*x - x*x;
}

float calcula (float local_a, float local_b, float h){
	float integral, x, i;

	integral = (f(local_a) + f(local_b))/2.0;

	x = local_a;
	integral *= h;
	return integral;
}

int main(int argc, char** argv) {
	int my_rank;
	int p;
	int n = 22;
	int source;
	int dest = 0;
	int tag = 200;
	float a = 1.0;
	float b = 10.0;
	float h;
	float local_a;
	float local_b;
	float local_n; // não remove senão para de funcionar
	float integral;
	float total;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	h = (b-a) / n;

	int j;
	for(j = my_rank; j < n; j += p) {
		local_a = a + j * h;
		local_b = local_a + h;
		printf("Rank %d calculando entre %f e %f\n", my_rank, local_a, local_b);
		integral += calcula(local_a, local_b, h);
	}

	if(my_rank == 0) {
		total = integral;
		for(source = 1; source < p; source++) {
			MPI_Recv(&integral, 1, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &status);
			total += integral;
		}
	}
	else{
		MPI_Send(&integral, 1, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
	}

	if(my_rank == 0) {
        printf("Resultado: %f\n", total);
    }
	MPI_Finalize();
}
