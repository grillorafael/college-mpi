#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define LENGTH 6

#define TASK_NUMBERS (LENGTH / 2)

void displayClock(int* clock, int myRank, char* label) {
    int i = TASK_NUMBERS;

    printf("[CLOCK FOR %s][%d]", label, myRank);
    printf("[");
    for(; i < LENGTH; i++) {
        printf("%d, ", clock[i]);
    }
    printf("]");
    printf("\n");
}

void displaySet(int* set, int myRank, int before) {
    char* mode = before ? "Before" : "New";
    int i = 0;

    printf("[SET FOR %s][%d]", mode, myRank);
    printf("[");
    for(; i < (LENGTH / 2); i++) {
        printf("%d, ", set[i]);
    }
    printf("]");
    printf("\n");
}

void atualizaClock(int* setAnt, int* setRec, int myRank){
    int i = TASK_NUMBERS;
    for(; i < LENGTH; i++){
        if(setAnt[i] > setRec[i]) {
            setRec[i] = setAnt[i];
        }
    }
}

// int matrizVizinha[TASK_NUMBERS][TASK_NUMBERS] = {
//     {0, 1, 0, 1, 0, 0},
//     {1, 0, 1, 0, 1, 0},
//     {0, 1, 0, 1, 0, 1},
//     {1, 0, 1, 0, 0, 1},
//     {0, 1, 0, 0, 0, 0},
//     {0, 0, 1, 1, 0, 0}
// };
int matrizVizinha[TASK_NUMBERS][TASK_NUMBERS] = {
    {0, 1, 1},
    {1, 0, 1},
    {1, 1, 0}
};

int contaNumeroDeVizinhos(int myRank){
    int i, contador=0;

    for(i = 0; i < TASK_NUMBERS; i++) {
        if(matrizVizinha[myRank][i] == 1) {
            contador++;
        }
    }

    return contador;
}

int main(int argc, char*argv[]) {
    int i, j, contador;
    int numeroDeVizinhos;
    int myRank;
    int source;
    int tag = 50;
    int pai;
    MPI_Status status;
    int origem;
    int state; //marca o pulso atual deste processo
    int dist[TASK_NUMBERS]; //distância
    int first[TASK_NUMBERS]; //primeiro nó no caminho int set[TASK_NUMBERS];
    int* set = (int*) calloc((TASK_NUMBERS * 2), sizeof(int)); //0-5: set; 6-11: clock
    int* setAnt = (int*) calloc((TASK_NUMBERS * 2), sizeof(int)); //0-5: set; 6-11: clock
    int level[TASK_NUMBERS]; //pulso dos meus vizinhos

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    numeroDeVizinhos = contaNumeroDeVizinhos(myRank);

    // printf("iniciando...\n");
    fflush(stdout);

    for(i = 0; i < TASK_NUMBERS; i++) {
        dist[i] = TASK_NUMBERS;
        first[i] = -1;
        set[i] = 0;
        level[i] = 0;
    }

    for(i = TASK_NUMBERS; i < TASK_NUMBERS*2; i++) {
        set[i] = 0; //inicialização do relógio
    }

    dist[myRank] = 0;
    set[myRank] = 1;
    state = 0;

    set[TASK_NUMBERS + myRank]++;
    for(i = 0; i < TASK_NUMBERS; i++) {
        if(matrizVizinha[myRank][i] == 1) {
            MPI_Send(set, LENGTH, MPI_INT, i, tag, MPI_COMM_WORLD);
        }
    }
    displayClock(set, myRank, "SEND");
    set[TASK_NUMBERS + myRank]++;

    while (state < TASK_NUMBERS - 1) {
        for(i = 0; i < TASK_NUMBERS * 2; i++){
            setAnt[i] = set[i];
        }

        MPI_Recv(set, LENGTH, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
        origem = status.MPI_SOURCE;
        atualizaClock(setAnt, set, myRank);
        displayClock(set, myRank, "RECEIVE");
        set[TASK_NUMBERS + myRank]++;
        level[origem]++;
        for (i = 0; i < TASK_NUMBERS; i++) {
            if(set[i] == 1) {
                if (dist[i] > level[origem]) {
                    dist[i] = level[origem];
                    first[i] = origem;
                }
                int continua = 1;
                for(j = 0; j < TASK_NUMBERS; j++) {
                    if(matrizVizinha[myRank][j] == 1 && state >= level[j]) {
                        continua = 0;
                        break;
                    }
                }
                if(continua) {
                    state++;
                    for(j = 0; j < TASK_NUMBERS; j++) {
                        if(dist[j] == state) {
                            set[j] = 1;
                        }
                    }
                    for(j = 0; j < TASK_NUMBERS; j++) {
            if(matrizVizinha[myRank][j] == 1) {
                            MPI_Send(set, LENGTH, MPI_INT, j, tag, MPI_COMM_WORLD);
                            displayClock(set, myRank, "SEND");
                        }
                    }
                }
            }
            // set[TASK_NUMBERS + myRank]++;
        }
        fflush(stdout);
    }

    //imprimindo as distâncias
    // printf("processo %d: ", myRank);
    // for (i = 0; i < TASK_NUMBERS; i++) {
    //     printf("dist(%d)=%d, ", i, dist[i]);
    // }
    // printf("\n");
    // printf("processo %d: ", myRank);
    // for (i = 0; i < TASK_NUMBERS; i++) {
    //     printf("first(%d)=%d, ", i, first[i]);
    // }
    // printf("\n");
    fflush(stdout);
    MPI_Finalize();
}
