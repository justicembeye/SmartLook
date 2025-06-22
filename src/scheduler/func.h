//
// Created by lozio on 10/05/25.
//

#ifndef FUNC_H
#define FUNC_H

#define MAX_TASKS 6

#include <Arduino.h>


// tipo di funzioni
typedef void (*Task_f)(void *arg);

// tipo di task
typedef struct {
    // funzione da eseguire
    Task_f f;
    // argomento della funzione
    void *arg;
} Task;

// tipo di scheduler
typedef struct {
    // lista delle task registrate
    Task tasks[MAX_TASKS];
    // numero di task registrate
    uint8_t n_tasks;
} Scheduler;



// prototipo delle funzioni
void scheduler_add(Scheduler *sch, Task_f f, void *arg);
void scheduler_run(Scheduler *sch);



#endif //FUNC_H
