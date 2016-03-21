#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h> 
#include <unistd.h>

#define TAILLE_TAMPON 10

volatile char tampon[TAILLE_TAMPON];
volatile int ptr_entree = 0;
volatile int ptr_sorti = 0;

sem_t n_plein;
sem_t n_vide;

void* lire(void* name) {
    (void) name;
    do {
        while (1) {
            //sleep(1);
            sem_wait(&n_vide);
            tampon[ptr_entree] = getchar();
            ptr_entree++;
            ptr_entree %= TAILLE_TAMPON;
            sched_yield();
            sem_post(&n_plein);
        }
    } while (tampon[ptr_entree] != 'F');
    return NULL;
}

void* affichage(void* name) {
    (void) name;
    do {
        while (1) {
            sleep(1);
            sem_wait(&n_plein);
            printf("car = %c\n", tampon[ptr_sorti]);
            ptr_sorti++;
            ptr_sorti %= TAILLE_TAMPON;
            
            sched_yield();
            sem_post(&n_vide);
        }
    } while (tampon[ptr_sorti] != 'F');
    return NULL;
}

int main(void) {
    pthread_t filsA, filsB;

    sem_init(&n_plein, 0, 0);
    sem_init(&n_vide, 0, TAILLE_TAMPON);

    if (pthread_create(&filsA, NULL, affichage, "AA")) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&filsB, NULL, lire, "BB")) {
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
