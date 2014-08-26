#include <stdio.h>
#include <mpi.h>
#include <string.h>

int numeroDeTarefas=6;
int matrizVizinhanca[6][6] = {
	{0,1,1,0,0,0},
	{1,0,1,1,1,0},
	{1,1,0,0,0,1},
	{0,1,0,0,0,0},
	{0,1,0,0,0,0},
	{0,0,1,0,0,0}
};

int contaNumeroDeVizinhos(int myRank) {
	int i;
	int contador = 0;
	for(i=0; i < numeroDeTarefas; i++) {
		contador += matrizVizinhanca[myRank][i];
	}
	return contador;
}

int main(int argc,char** argv) {
	int i;
	int numeroDeVizinhos;
	int myRank;
	int source;
	int tag=50;
	char message[100]="Oi!";
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
	numeroDeVizinhos=contaNumeroDeVizinhos(myRank);
	if(myRank == 0) {
		for(i = 0; i < numeroDeTarefas; i++) {
			if(matrizVizinhanca[myRank][i] == 1) {
				printf("0 Enviando mensagem para %d\n",i);
				MPI_Send(message, strlen(message) + 1, MPI_CHAR, i, tag, MPI_COMM_WORLD);
			}
		}
		for(i = 0; i < numeroDeVizinhos; i++) {
			MPI_Recv(message, 100, MPI_CHAR, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
			printf("0 Recebendo msg de %d\n", status.MPI_SOURCE);
		}
	}
	else {
		printf("%d Recebendo msg de 0\n", myRank);
		MPI_Recv(message,100,MPI_CHAR,MPI_ANY_SOURCE,tag,MPI_COMM_WORLD,&status);
		for(i = 0; i < numeroDeTarefas; i++) {
			if(matrizVizinhanca[myRank][i] ==1) {
				printf("%d Enviando mensagem para %d\n", myRank, i);
				MPI_Send(message, strlen(message) + 1, MPI_CHAR, i, tag, MPI_COMM_WORLD);
			}
		}
		for(i = 0; i < (numeroDeVizinhos - 1); i++) {
			printf("[%d] Esperando receber\n", myRank);
			MPI_Recv(message, 100, MPI_CHAR, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
			printf("%d Recebendo msg de %d\n", myRank, status.MPI_SOURCE);
		}
	}
	printf("Finalizando %d\n", myRank);
	MPI_Finalize();
}
