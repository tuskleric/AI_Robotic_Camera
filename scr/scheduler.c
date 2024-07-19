#include <stdint.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "scheduler.h"
#include "tick.h"




//systick callback function
bool alarm_callback(struct repeating_timer *t) {
    for (uint8_t i = 0; i < kernal.numTasks; i++) {
        kernal.tasks[i].taskTick++;
        if (kernal.tasks[i].taskTick >= ((MICRO_FREQ_TO_PERIOD/(kernal.tasks[i].freq)) / kernal.tickPeriod)) {
            kernal.tasks[i].taskTick = 0;
            kernal.tasks[i].ready = true;
        }
    }
    return true; // Continue calling the callback
}



void kernal_init()
{
    kernal.numTasks = 0;

}

taskId_t register_task(void(*taskEnter)(void), uint8_t priority, int64_t freq) 
{
    if (kernal.numTasks < MAX_TASKS) 
    {
        
        taskId_t newTaskId;
        newTaskId.Id = kernal.numTasks;
        kernal.tasks[newTaskId.Id].freq = freq;
        kernal.tasks[newTaskId.Id].taskTick = 0;
        kernal.tasks[newTaskId.Id].ready = false;
        kernal.tasks[newTaskId.Id].priority = priority;
        kernal.tasks[newTaskId.Id].run = taskEnter;
        kernal.numTasks ++;
        printf("task_id %d\n", newTaskId);
        printf("task_id %d\n", kernal.tasks[newTaskId.Id].id.Id);
        printf("task_id returned %d\n", newTaskId.Id);
        return newTaskId;

    }
    else 
    {
        printf("INVALID TASK_ID");
    }
    
}

void kernal_start(void) 
{
    
    uint8_t highestPriority = UINT8_MAX;
    uint8_t taskToRun = 0xFF; // INVALID task ID
    
    for (uint8_t i = 0; i < kernal.numTasks; i++) {
        //printf("here?? %d\n", kernal.tasks[i].ready);
        if (kernal.tasks[i].ready && kernal.tasks[i].priority < highestPriority) {
            //printf("getting here\n");
            highestPriority = kernal.tasks[i].priority;
            taskToRun = i;
        }
    }
    
    //run the highest priority task (lowest priority number)
    if (taskToRun != 0xFF) {
       // printf("pre-run");
        kernal.tasks[taskToRun].run();
        //printf("post-run");
        kernal.tasks[taskToRun].ready = false;
    }
}



void kernal_unregister_task(taskId_t taskId);

void kernal_notify_task(taskId_t taskId);

void kernal_block_task(taskId_t taskId);