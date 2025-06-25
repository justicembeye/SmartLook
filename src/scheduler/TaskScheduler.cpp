//
// Created by lozio on 20/05/25.
//

#include "TaskScheduler.h"

TaskScheduler::TaskScheduler() : task_list{}, task_count(0) {
    // Costruttore: inizializza il contatore dei task a 0

}

bool TaskScheduler::addTask(Task_f task_function) {
    if (task_count < MAX_TASKS) {
        task_list[task_count] = task_function;
        task_count++;
        return true;
    }
    return false; // impossibile aggiungere un task

}

void TaskScheduler::run() {
    // Cicla attraverso tutti i task registrati ed eseguili
    // Ogni funzione di task sarà responsabile di verificare se deve effettivamente
    // fare qualcosa usando millis().
    for (uint8_t i = 0; i < task_count; i++) {
        task_list[i](); //Chiama la funzione del task
    }
}
