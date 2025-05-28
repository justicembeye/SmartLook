//
// Created by lozio on 10/05/25.
//

#ifndef FUNC_H
#define FUNC_H

#define MAX_TASKS 6

#include <Arduino.h>


// type de fonction
typedef void (*Task_f)(void *arg);

// type de tache
typedef struct {
    // fonction a executer
    Task_f f;
    // argument de la fonction
    void *arg;
} Task;

// type de scheduler
typedef struct {
    // list des taches enregistrees
    Task tasks[MAX_TASKS];
    // nombre de taches enregistrees
    uint8_t n_tasks;
} Scheduler;



// Prototypes des fonctions
void scheduler_add(Scheduler *sch, Task_f f, void *arg);
void scheduler_run(Scheduler *sch);



#endif //FUNC_H
