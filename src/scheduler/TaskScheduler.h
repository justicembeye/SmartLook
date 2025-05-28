//
// Created by lozio on 20/05/25.
//

#ifndef TASKSCHEDULER_H
#define TASKSCHEDULER_H

#include "Task.h" // Pour TaskFunction
#include <Arduino.h>

const byte MAX_TASKS = 10;

class TaskScheduler {
    public:
        TaskScheduler();
        bool addTask(Task_f task_function);
        void run(); // Exécute toutes les tâches dont c'est le tour

    private:
        Task_f task_list[MAX_TASKS];
        byte task_count;
};


#endif //TASKSCHEDULER_H
