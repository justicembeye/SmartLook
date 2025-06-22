//
// Created by lozio on 20/05/25.
//

#ifndef TASKSCHEDULER_H
#define TASKSCHEDULER_H

#include "Task.h" // Per il TaskFunction
#include <Arduino.h>

const byte MAX_TASKS = 10;

class TaskScheduler {
    public:
        TaskScheduler();
        bool addTask(Task_f task_function);
        void run(); // Esegue tutti i task al loro turno


    private:
        Task_f task_list[MAX_TASKS];
        byte task_count;
};


#endif //TASKSCHEDULER_H
