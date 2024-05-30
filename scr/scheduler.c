#include <stdint.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "scheduler.h"
#include "tick.h"




//systick callback function
bool alarm_callback(struct repeating_timer *t)
{
    printf("numTasks %d \n", kernal.numTasks);
    for (uint8_t i =0; i <= kernal.numTasks; i++)
    {
        printf("I %d\n",i);
        printf("kernal period   %lld\n",kernal.tickPeriod);
        printf("kernal_task period   %lld\n",kernal.tasks[i].period);
        kernal.tasks[i].taskTick ++;
        printf("kernal_task tick   %lld\n",kernal.tasks[i].taskTick);
        if( kernal.tasks[i].taskTick >= (kernal.tasks[i].period / kernal.tickPeriod)) {
            kernal.tasks[i].taskTick = 0;
            kernal.tasks[i].ready = true;
            printf("not stuck %lld\n",kernal.tasks[i].taskTick);
        }
    }
}



void kernal_init(void)
{
    kernal.numTasks = 0;

}

taskId_t register_task(void(*taskEnter)(void), uint8_t priority, int64_t period) 
{
    if (kernal.numTasks < MAX_TASKS) 
    {
        
        taskId_t newTaskId;
        newTaskId.Id = kernal.numTasks;
        kernal.tasks[newTaskId.Id].period = period;
        kernal.tasks[newTaskId.Id].taskTick = 0;
        kernal.tasks[newTaskId.Id].ready = false;
        kernal.tasks[newTaskId.Id].priority = priority;
        kernal.tasks[newTaskId.Id].run = taskEnter;
        kernal.numTasks ++;
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
        if (kernal.tasks[i].ready && kernal.tasks[i].priority < highestPriority) {
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