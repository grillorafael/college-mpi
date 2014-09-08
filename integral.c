#include <stdio.h>
#include <mpi.h>
#include <math.h>


int main(int argc, char** argv){
	int my_rank, p, local_n, n = 1024, source, dest = 0, tag = 200;
	float a = 1.0, b = 5.0, h, local_a, local_b, integral, total;
	MPI_Status status;

	float calcula (float local_a, float local_b, int local_n, float h);

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	h = (b-a) / n;

	int j;
	for(j = my_rank; j < n; j += p) {
		local_a = a + j * h;
		local_b = local_a + h;
		integral += calcula(local_a, local_b, local_n, h);
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

float calcula (float local_a, float local_b, int local_n, float h){
	float integral, x, i;

	float f(float x);

	integral = (f(local_a) + f(local_b))/2.0;

	x = local_a;
	for(i = 1; i <= local_n - 1; i++){
		x+=h;
		integral += f(x);
	}
	integral *= h;
	return integral;
}

float f(float x){
	return log(x);
}
