/*
 * sched.c
 *
 *  Created on: Oct 30, 2021
 *      Author: hello
 */
#include "main.h"

#define RETURN_NORMAL (0)
#define RETURN_ERROR (1)

#define ERROR_SCH_TOO_MANY_TASKS (2)
#define ERROR_SCH_CANNOT_DELETE_TASK (3)

#define SCH_MAX_TASKS 40
#define	NO_TASK_ID 0

sTask SCH_tasks_G[SCH_MAX_TASKS];

tByte Error_code_G;

void SCH_Dispatch_Tasks(void)
{
   tByte Index;
   // Dispatches (runs) the next task (if one is ready)
   for (Index = 0; Index < SCH_MAX_TASKS; Index++)
   {
      if (SCH_tasks_G[Index].RunMe > 0)
         {
         (*SCH_tasks_G[Index].pTask)();  // Run the task

         SCH_tasks_G[Index].RunMe -= 1;   // Reset / reduce RunMe flag

         // Periodic tasks will automatically run again
         // - if this is a 'one shot' task, remove it from the array
         if (SCH_tasks_G[Index].Period == 0)
            {
            SCH_Delete_Task(Index);
          }
       }
   }
   Error_code_G = 0;
   SCH_Report_Status();
}

void SCH_Init(void) {
    unsigned char i;
    for (i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_Delete_Task(i);
    }
    Error_code_G = 0;
}

void SCH_Update(void){
    unsigned char Index;
    for (Index = 0; Index < SCH_MAX_TASKS; Index++) {
        if (SCH_tasks_G[Index].pTask){
            if (SCH_tasks_G[Index].Delay == 0) {
                SCH_tasks_G[Index].RunMe += 1;
                if (SCH_tasks_G[Index].Period) {
                    SCH_tasks_G[Index].Delay = SCH_tasks_G[Index].Period;
                }
            } else {
                SCH_tasks_G[Index].Delay -= 1;
            }
        }
    }
}

tByte SCH_Add_Task(void (* pFunction)(), tWord DELAY, tWord PERIOD)
{
	tByte Index = 0;
	while ((SCH_tasks_G[Index].pTask != 0) && (Index < SCH_MAX_TASKS))
	{
	      Index++;
	}

	// Have we reached the end of the list?
	if (Index == SCH_MAX_TASKS)
	{
		// Task list is full
	    //
	    // Set the global error variable
	    Error_code_G = ERROR_SCH_TOO_MANY_TASKS;

	    // Also return an error code
	    return SCH_MAX_TASKS;
	}
    SCH_tasks_G[Index].pTask = pFunction;
    SCH_tasks_G[Index].Delay = DELAY;
    SCH_tasks_G[Index].Period = PERIOD;
    SCH_tasks_G[Index].RunMe = 0;
    return Index;
}


tByte SCH_Delete_Task(const tByte TASK_INDEX)
{
	tByte Return_code;

   if (SCH_tasks_G[TASK_INDEX].pTask == 0)
   {
      // No task at this location...
      //
      // Set the global error variable
      Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
      // ...also return an error code
      Return_code = RETURN_ERROR;
   }
   else
   {
      Return_code = RETURN_NORMAL;
   }

   SCH_tasks_G[TASK_INDEX].pTask   = 0x0000;
   SCH_tasks_G[TASK_INDEX].Delay   = 0;
   SCH_tasks_G[TASK_INDEX].Period  = 0;
   SCH_tasks_G[TASK_INDEX].RunMe   = 0;

   return Return_code;
}

void SCH_Report_Status(void)
{
#ifdef SCH_REPORT_ERRORS
   // ONLY APPLIES IF WE ARE REPORTING ERRORS
   // Check for a new error code
   if (Error_code_G != Last_error_code_G)
   {
      // Negative logic on LEDs assumed
      Error_port = 255 - Error_code_G;
      Last_error_code_G = Error_code_G;
      if (Error_code_G != 0)
      {
    	   Error_tick_count_G = 60000;
      }
      else
      {
         Error_tick_count_G = 0;
      }
   }
   else
   {
      if (Error_tick_count_G != 0)
      {
         if (--Error_tick_count_G == 0)
         {
            Error_code_G = 0; // Reset error code
         }
       }
   }
#endif
}
