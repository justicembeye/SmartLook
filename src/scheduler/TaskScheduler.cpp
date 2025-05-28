//
// Created by lozio on 20/05/25.
//

#include "TaskScheduler.h"

TaskScheduler::TaskScheduler() : task_list{}, task_count(0) {
    // Constructeur : initialise le compteur de tâches à 0
}

bool TaskScheduler::addTask(Task_f task_function) {
    if (task_count < MAX_TASKS) {
        task_list[task_count] = task_function;
        task_count++;
        return true;
    }
    return false; // impossible d'ajouter une tâche'

}

void TaskScheduler::run() {
    // Boucle à travers toutes les tâches enregistrées et les exécute
    // Chaque fonction de tâche sera responsable de vérifier si elle doit réellement
    // faire quelque chose en utilisant millis().
    for (uint8_t i = 0; i < task_count; i++) {
        task_list[i](); // Appelle la fonction de la tâche
    }
}