#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 

#define NB_PROCESS 2
#define TRUE 1
#define FALSE 0

sem_t mutex;

int request[NB_PROCESS];
int tower;

void peterson_init() {
    request[0] = FALSE;
    request[1] = FALSE;
}

void peterson_wait(int i) {
    request[i] = TRUE;
    tower = (1 - i);
    while(!(request[1 - i] == FALSE || tower == i));
}

void peterson_post(int i) {
    request[i] = FALSE;
}

void* affichage(void* name) {
    int n = atoi(name);
    for (int i = 0; i < 20; i++) {
        //sem_wait(&mutex); /* prologue */
        peterson_wait(n); /* prologue */
        for (int j = 0; j < 5; j++) printf("%d ", n);
        sched_yield(); /* pour etre sur d'avoir des problemes */
        for (int j = 0; j < 5; j++) printf("%d ", n);
        printf("\n ");
        //sem_post(&mutex); /* epilogue */
        peterson_post(n); /* epilogue */
    }

    return NULL;
}

int main(void) {
    pthread_t filsA, filsB;

    //sem_init(&mutex, 0, 1);
    peterson_init();

    if (pthread_create(&filsA, NULL, affichage, "0")) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&filsB, NULL, affichage, "1")) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    
    if (pthread_join(filsA, NULL))
        perror("pthread_join");

    if (pthread_join(filsB, NULL))
        perror("pthread_join");

    printf("Fin du pere\n");
    return (EXIT_SUCCESS);
}