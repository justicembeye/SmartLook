//
// Created by lozio on 10/05/25.
//

#include <Arduino.h>

#include "func.h"


// Scheduler functions
void scheduler_add(Scheduler *sch, Task_f f, void *arg) {
    if (sch->n_tasks < MAX_TASKS) {
        sch->tasks[sch->n_tasks].f = f;
        sch->tasks[sch->n_tasks].arg = arg;
        sch->n_tasks++;
    }
}

void scheduler_run(Scheduler *sch) {
    for (uint8_t i = 0; i < sch->n_tasks; ++i) {
        sch->tasks[i].f(sch->tasks[i].arg);
    }
}