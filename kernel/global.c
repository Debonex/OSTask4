
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "proc.h"
#include "global.h"


PUBLIC	PROCESS			proc_table[NR_TASKS];

PUBLIC	char			task_stack[STACK_SIZE_TOTAL];

PUBLIC	TASK	task_table[NR_TASKS] = {{TestA, STACK_SIZE_TESTA, "TestA"},
					{TestB, STACK_SIZE_TESTB, "TestB"},
					{TestC, STACK_SIZE_TESTC, "TestC"},
                    {TestD, STACK_SIZE_TESTC, "TestD"},
                    {TestE, STACK_SIZE_TESTE, "TestE"},
                    {TestF, STACK_SIZE_TESTF, "TestF"}};

// PUBLIC	TASK	task_table[NR_TASKS] = {{TestE, STACK_SIZE_TESTA, "TestA"},
// 					{TestD, STACK_SIZE_TESTB, "TestB"},
// 					{TestC, STACK_SIZE_TESTC, "TestC"},
//                     {TestB, STACK_SIZE_TESTC, "TestD"},
//                     {TestA, STACK_SIZE_TESTE, "TestE"},
//                     {TestF, STACK_SIZE_TESTF, "TestF"}};

PUBLIC	irq_handler		irq_table[NR_IRQ];

PUBLIC	system_call		sys_call_table[NR_SYS_CALL] = {sys_get_ticks,sys_delay,sys_print,sys_P,sys_V};

PUBLIC  SEMAPHORE* rmutex = &(SEMAPHORE){1,0};
PUBLIC  SEMAPHORE* wmutex = &(SEMAPHORE){1,0};
PUBLIC  SEMAPHORE* rnum = &(SEMAPHORE){3,0};
PUBLIC  SEMAPHORE* S = &(SEMAPHORE){1,0};
PUBLIC  SEMAPHORE* X = &(SEMAPHORE){1,0};
PUBLIC  SEMAPHORE* Y = &(SEMAPHORE){1,0};
PUBLIC  SEMAPHORE* Z = &(SEMAPHORE){1,0};
PUBLIC  int readcount;
PUBLIC  int proc_current;
PUBLIC  int writecount;
PUBLIC  int maxreaders;

