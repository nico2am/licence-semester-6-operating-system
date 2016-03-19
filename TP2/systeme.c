
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "cpu.h"
#include "systeme.h"

struct {
    PSW cpu; /* mot d'etat du processeur */
    int state; /* etat du processus        */
    time_t wakeup;
}
process[MAX_PROCESS]; /* table des processus      */

int current_process = -1; /* nu du processus courant  */
int counter_getchar_process = 0;

char buffer = '\0';

/**********************************************************
 ** Demarrage du systeme
 ***********************************************************/

static PSW systeme_init(void) {
    init_proccess();
    PSW cpu;

    printf("Booting.\n");
    /*** creation d'un programme ***/
    /*make_inst(0, INST_SUB, 1, 1, 0);
    make_inst(1, INST_SUB, 2, 2, -1000);
    make_inst(2, INST_SUB, 3, 3, -10);
    make_inst(3, INST_CMP, 1, 2, 0);
    make_inst(4, INST_IFGT, RI_EMPTY, RI_EMPTY, 11);
    make_inst(5, INST_NOP, RI_EMPTY, RI_EMPTY, ARG_EMPTY);
    make_inst(6, INST_NOP, RI_EMPTY, RI_EMPTY, ARG_EMPTY);
    make_inst(7, INST_NOP, RI_EMPTY, RI_EMPTY, ARG_EMPTY);
    make_inst(8, INST_ADD, 1, 3, 0);
    make_inst(9, INST_SYSC, 1, 0, SYSC_PUTI);
    make_inst(10, INST_JUMP, RI_EMPTY, RI_EMPTY, 3);
    make_inst(11, INST_HALT, RI_EMPTY, RI_EMPTY, ARG_EMPTY);*/

    /*make_inst(0, INST_SYSC, 1, 1, SYSC_NEW_THREAD);
    make_inst(1, INST_IFGT, RI_EMPTY, RI_EMPTY, 6);

    make_inst(2, INST_LOAD, 2, 0, 10);
    make_inst(3, INST_SUB, 2, 2, -10);
    make_inst(4, INST_STORE, 2, 0, 10);
    make_inst(5, INST_JUMP, RI_EMPTY, RI_EMPTY, 2);

    make_inst(6, INST_LOAD, 2, 0, 10);
    make_inst(7, INST_SYSC, 2, RI_EMPTY, SYSC_PUTI);
    make_inst(8, INST_JUMP, 0, 0, 6);*/

    /*make_inst(0, INST_SUB, 2, 2, 0);
    make_inst(1, INST_STORE, 2, 0, 17);
    make_inst(2, INST_SYSC, 1, 1, SYSC_NEW_THREAD);
    make_inst(3, INST_IFGT, RI_EMPTY, RI_EMPTY, 10);

    make_inst(4, INST_LOAD, 2, 0, 17);
    make_inst(5, INST_ADD, 2, 0, 10);
    make_inst(6, INST_SYSC, 2, RI_EMPTY, SYSC_PUTI);
    make_inst(7, INST_SUB, 3, 3, -3);
    make_inst(8, INST_SYSC, 3, RI_EMPTY, SYSC_SLEEP);
    make_inst(9, INST_JUMP, RI_EMPTY, RI_EMPTY, 5);

    make_inst(10, INST_SUB, 4, 4, -1);
    make_inst(11, INST_SYSC, 4, RI_EMPTY, SYSC_PUTI);
    make_inst(12, INST_SUB, 3, 3, -3);
    make_inst(13, INST_SYSC, 3, RI_EMPTY, SYSC_SLEEP);
    make_inst(14, INST_JUMP, RI_EMPTY, RI_EMPTY, 10);*/

    /*make_inst(0, INST_SYSC, 1, 0, SYSC_GETCHAR);
    make_inst(1, INST_SYSC, 1, RI_EMPTY, SYSC_PUTI);
    make_inst(2, INST_SUB, 2, 2, -1);
    make_inst(3, INST_SYSC, 2, RI_EMPTY, SYSC_SLEEP);
    make_inst(4, INST_JUMP, RI_EMPTY, RI_EMPTY, 0);*/

    make_inst(0, INST_SYSC, 1, 1, SYSC_FORK);
    make_inst(1, INST_IFGT, RI_EMPTY, RI_EMPTY, 8);
    
    make_inst(2, INST_SUB, 1, 1, -1);
    make_inst(3, INST_SYSC, 1, 0, SYSC_PUTI);
    make_inst(4, INST_SYSC, 1, 0, SYSC_PUTI);
    make_inst(5, INST_SUB, 2, 2, -4);
    make_inst(6, INST_SYSC, 2, 0, SYSC_SLEEP);
    make_inst(7, INST_SYSC, 0, 0, SYSC_EXIT);

    make_inst(8, INST_SUB, 2, 2, -4);
    make_inst(9, INST_SYSC, 2, 0, SYSC_SLEEP); 
    make_inst(10, INST_JUMP, 0, 0, 0);

    /*** valeur initiale du PSW ***/
    memset(&cpu, 0, sizeof (cpu));
    cpu.PC = 0;
    cpu.SB = 0;
    cpu.SS = CPU_SS;

    int index = search_free_process();
    process[index].cpu = cpu;
    process[index].state = READY;

    return ordonnanceur(cpu);
}

/**********************************************************
 ** Simulation du systeme (mode systeme)
 ***********************************************************/


char* systeme_int_string(WORD i) {
    switch (i) {
        case INT_INIT: return "Init";
        case INT_SEGV: return "Segv";
        case INT_INST: return "Inst";
        case INT_TRACE: return "Trace";
        case INT_CLOCK: return "Clock";
        case INT_SYSC: return "Sysc";
        default: return "Unknown";
    }
}

void systeme_int_print(WORD i) {
    printf("%s\n", systeme_int_string(i));
}

PSW systeme_int_trace(PSW m) {
    systeme_int_print(INT_TRACE);
    printf("\t\033[32mPC: %d\033[0m\n", m.PC);
    for (int i = 0; i < NB_REGISTERS; i++)
        printf("\t\033[32mDR[%d]: %d\033[0m\n", i, m.DR[i]);

    m.IN = INT_EMPTY;

    return m;
}

PSW sysc_exit(PSW m) {
    printf("EXIT\n");
    process[current_process].state = EMPTY;
    current_process = -1;
    return ordonnanceur(m);
}

PSW sysc_new_thread(PSW father) {
    PSW son = father;
    int index = search_free_process();

    father.DR[father.RI.i] = index;
    father.AC = index;

    son.DR[father.RI.i] = 0;
    son.AC = 0;

    process[index].state = READY;
    process[index].cpu = son;

    return father;
}

PSW sysc_sleep(PSW m) {
    time_t wakeup_date = time(NULL) + m.DR[m.RI.i];

    process[current_process].state = SLEEPING;
    process[current_process].wakeup = wakeup_date;
    process[current_process].cpu = m;

    return ordonnanceur(m);
}

PSW sysc_getchar(PSW m) {
    if (buffer == '\0') {
        counter_getchar_process++;
        process[current_process].state = GETCHAR;
        return ordonnanceur(m);
    }

    m.DR[m.RI.i] = buffer;

    return m;
}

PSW sysc_fork(PSW father) {
    PSW son = father;
    int index = search_free_process();

    son.SB = index * CPU_SS;

    for (int i = 0; i < CPU_SS; i++)
        mem[i + son.SB] = mem[i + father.SB];

    father.DR[father.RI.i] = index;
    father.AC = index;

    son.DR[father.RI.i] = 0;
    son.AC = 0;

    process[index].state = READY;
    process[index].cpu = son;

    return father;
}

PSW systeme_int_sysc(PSW m) {
    m.IN = INT_EMPTY;

    switch (m.RI.ARG) {
        case SYSC_EXIT:
            m = sysc_exit(m);
            break;
        case SYSC_PUTI:
            printf("puti: %d\n", m.DR[m.RI.i]);
            break;
        case SYSC_NEW_THREAD:
            m = sysc_new_thread(m);
            break;
        case SYSC_SLEEP:
            m = sysc_sleep(m);
            break;
        case SYSC_GETCHAR:
            m = sysc_getchar(m);
            break;
        case SYSC_FORK:
            m = sysc_fork(m);
            break;
    }

    return m;
}


void simul_write_char() {
    static time_t last_write = 0;
    static int nb_letters = 5;
    static char letters[] = {'a', 'b', 'c', 'd', 'e'};

    char letter = letters[rand() % nb_letters];
    
    if (time(NULL) - 3 > last_write) {
        if (counter_getchar_process == 0)
            buffer = letter;
        else {
            for (int i = 0; i < MAX_PROCESS; i++) {
                if (process[i].state == GETCHAR) {
                    process[i].cpu.DR[process[i].cpu.RI.i] = letter;
                    process[i].state = READY;
                    counter_getchar_process--;
                    break;
                }
            }
        }
        last_write = time(NULL);
    }
}

PSW systeme(PSW m) {
    //printf("Current process : %d\n", current_process);

    simul_write_char();

    switch (m.IN) {
        case INT_INIT:
            systeme_int_print(INT_INIT);
            return (systeme_init());
        case INT_SEGV:
            systeme_int_print(INT_SEGV);
            exit(1);
        case INT_TRACE:
            m = systeme_int_trace(m);
            break;
        case INT_INST:
            systeme_int_print(INT_INST);
            exit(1);
        case INT_CLOCK:
            //systeme_int_print(INT_CLOCK);
            m = ordonnanceur(m);
            break;
        case INT_SYSC:
            m = systeme_int_sysc(m);
            break;
    }

    return m;
}

/**********************************************************
 ** proccess
 ***********************************************************/

void init_proccess() {
    for (int i = 0; i < MAX_PROCESS; i++) {
        process[i].state = EMPTY;
    }
}

int search_free_process() {
    for (int i = 0; i < MAX_PROCESS; i++)
        if (process[i].state == EMPTY)
            return i;

    printf("Plus de threads disponible\n");
    exit(1);
}

void wakeup_process() {
    for (int i = 0; i < MAX_PROCESS; i++)
        if (process[i].state == SLEEPING && process[i].wakeup <= time(NULL))
            process[i].state = READY;
}

PSW ordonnanceur(PSW m) {

    if (current_process >= 0 && current_process < MAX_PROCESS && process[current_process].state == READY)
        process[current_process].cpu = m;

    do {
        current_process = (current_process + 1) % MAX_PROCESS;
        wakeup_process();
    } while (process[current_process].state != READY);

    return process[current_process].cpu;
}
