/*
 * sched.h
 *
 *  Created on: Oct 30, 2021
 *      Author: hello
 */

#ifndef INC_SCHED_H_
#define INC_SCHED_H_
#include "main.h"

typedef unsigned char tByte;
typedef unsigned int  tWord;

typedef struct {
	void ( * pTask)(void);
	tWord Delay;
	tWord Period;
	tByte RunMe;
	tWord TaskID;
} sTask;

void SCH_Init(void);
void SCH_Update(void);
void  SCH_Dispatch_Tasks(void);
tByte SCH_Add_Task(void (* pFunction)(), tWord DELAY, tWord PERIOD);
tByte SCH_Delete_Task(const tByte TASK_INDEX);
void  SCH_Report_Status(void);

#endif /* INC_SCHED_H_ */
