#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdio.h>

#define MAX_TASKS 5
#define KERNAL_PERIOD_US 10000
typedef struct 
{
    uint16_t Id;
}taskId_t;

typedef struct 
{
   taskId_t id;
   int64_t  period;
   int64_t taskTick;
   bool ready;
   uint8_t priority;
   void(*run)(void);

}Task_t;

// Define the scheduler struct
typedef struct
{  
    Task_t tasks[MAX_TASKS]; // Array of tasks
    int8_t numTasks;          // Number of tasks
    int64_t tickPeriod;       // Tick period for the scheduler
} scheduler;

  extern scheduler kernal; // Declare the kernal instance






//set the period for systick interrupts
void kernal_init(void);

taskId_t register_task(void(*taskEnter)(void), uint8_t priority, int64_t period);

void kernal_start(void);

void kernal_unregister_task(taskId_t taskId);

void kernal_notify_task(taskId_t taskId);

void kernal_block_task(taskId_t taskId);

bool alarm_callback(struct repeating_timer *t);

#endif