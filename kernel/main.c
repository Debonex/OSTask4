
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"


/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	//disp_str("-----\"kernel_main\" begins-----\n");

	TASK*		p_task		= task_table;
	PROCESS*	p_proc		= proc_table;
	char*		p_task_stack	= task_stack + STACK_SIZE_TOTAL;
	u16		selector_ldt	= SELECTOR_LDT_FIRST;
	int i;
	for (i = 0; i < NR_TASKS; i++) {
		strcpy(p_proc->p_name, p_task->name);	// name of the process
		p_proc->pid = i;			// pid

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;
		p_proc->regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK)
			| RPL_TASK;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = 0x1202; /* IF=1, IOPL=1 */

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	// proc_table[0].ticks = proc_table[0].priority = 1;
	// proc_table[1].ticks = proc_table[1].priority = 1;
	// proc_table[2].ticks = proc_table[2].priority = 1;
	// proc_table[3].ticks = proc_table[3].priority = 1;
	// proc_table[4].ticks = proc_table[4].priority = 1;
	// proc_table[5].ticks = proc_table[5].priority = 1;
	for(int i=0;i<NR_TASKS;i++){
		proc_table[i].sleep_tick=0;
		proc_table[i].block=0;
		proc_table[i].next=0;
	}
	

	k_reenter = 0;
	ticks = 0;

	p_proc_ready	= proc_table;

        /* 初始化 8253 PIT */
        out_byte(TIMER_MODE, RATE_GENERATOR);
        out_byte(TIMER0, (u8) (TIMER_FREQ/HZ) );
        out_byte(TIMER0, (u8) ((TIMER_FREQ/HZ) >> 8));

        put_irq_handler(CLOCK_IRQ, clock_handler); /* 设定时钟中断处理程序 */
        enable_irq(CLOCK_IRQ);                     /* 让8259A可以接收时钟中断 */

	//rmutex = &(SEMAPHORE){1,0};
	rmutex->value=1;
	//wmutex = &(SEMAPHORE){1,0};
	wmutex->value=1;
	S->value=1;
	X->value=1;
	Y->value=1;
	Z->value=1;
	readcount = 0;
	writecount=0;
	maxreaders=3;
	proc_current = 0;

	restart();

	while(1){}
}

/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{
	while (1) {
		if(readcount>=maxreaders)continue;
		sys_P(rmutex);
		if(readcount==0)
			sys_P(wmutex);
		readcount++;
		syscall_print("TestA start reading");	
		sys_V(rmutex);	

		for(int i=0;i<2;i++){
			syscall_print("TestA reading");
			syscall_delay(TICK_TIME);
		}
		

		sys_P(rmutex);
		readcount--;
		syscall_print("TestA end reading");
		if(readcount==0)sys_V(wmutex);	
		sys_V(rmutex);
	}

	// while (1) {
	// 	if(readcount>=maxreaders)continue;
	// 	sys_P(Z);
	// 	sys_P(rmutex);
	// 	sys_P(X);
	// 	readcount++;
	// 	if(readcount==1)sys_P(wmutex);
	// 	syscall_print("TestA start reading");
	// 	sys_V(X);
	// 	sys_V(rmutex);
	// 	sys_V(Z);

	// 	for(int i=0;i<2;i++){
	// 		syscall_print("TestA reading");
	// 		syscall_delay(TICK_TIME);
	// 	}
		

	// 	sys_P(X);
	// 	readcount--;
	// 	syscall_print("TestA end reading");
	// 	if(readcount==0)sys_V(wmutex);	
	// 	sys_V(X);
	// }
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestB()
{
	while(1){
		if(readcount>=maxreaders)continue;
		sys_P(rmutex);
		if(readcount==0)
			sys_P(wmutex);
		readcount++;
		syscall_print("TestB start reading");		
		sys_V(rmutex);	

		for(int i=0;i<3;i++){
			syscall_print("TestB reading");
			syscall_delay(TICK_TIME);
		}


		sys_P(rmutex);
		readcount--;
		syscall_print("TestB end reading");
		if(readcount==0)sys_V(wmutex);	
		sys_V(rmutex);
	}

	// while (1) {
	// 	if(readcount>=maxreaders)continue;
	// 	sys_P(Z);
	// 	sys_P(rmutex);
	// 	sys_P(X);
	// 	readcount++;
	// 	if(readcount==1)sys_P(wmutex);
	// 	syscall_print("TestB start reading");
	// 	sys_V(X);
	// 	sys_V(rmutex);
	// 	sys_V(Z);

	// 	for(int i=0;i<2;i++){
	// 		syscall_print("TestB reading");
	// 		syscall_delay(TICK_TIME);
	// 	}
		

	// 	sys_P(X);
	// 	readcount--;
	// 	syscall_print("TestB end reading");
	// 	if(readcount==0)sys_V(wmutex);	
	// 	sys_V(X);
	// }
}

/*======================================================================*
                               TestC
 *======================================================================*/
void TestC()
{
	while(1){
		if(readcount>=maxreaders)continue;
		sys_P(rmutex);
		if(readcount==0)
			sys_P(wmutex);
		readcount++;
		syscall_print("TestC start reading");	
		sys_V(rmutex);

		for(int i=0;i<3;i++){
			syscall_print("TestC reading");
			syscall_delay(TICK_TIME);
		}

		sys_P(rmutex);
		readcount--;
		syscall_print("TestC end reading");
		if(readcount==0)sys_V(wmutex);	
		sys_V(rmutex);
	}

	// while (1) {
	// 	if(readcount>=maxreaders)continue;
	// 	sys_P(Z);
	// 	sys_P(rmutex);
	// 	sys_P(X);
	// 	readcount++;
	// 	if(readcount==1)sys_P(wmutex);
	// 	syscall_print("TestC start reading");
	// 	sys_V(X);
	// 	sys_V(rmutex);
	// 	sys_V(Z);

	// 	for(int i=0;i<2;i++){
	// 		syscall_print("TestC reading");
	// 		syscall_delay(TICK_TIME);
	// 	}
		

	// 	sys_P(X);
	// 	readcount--;
	// 	syscall_print("TestC end reading");
	// 	if(readcount==0)sys_V(wmutex);	
	// 	sys_V(X);
	// }
}

/*======================================================================*
                               TestD
 *======================================================================*/
void TestD()
{
	while(1){
		syscall_P(wmutex);
		writecount++;
		syscall_print("TestD start writing");
		for(int i=0;i<3;i++){
			syscall_print("TestD is writing");
			syscall_delay(TICK_TIME);
		}
		writecount--;
		syscall_print("TestD end writing");
		syscall_V(wmutex);
	}

	// while(1){
	// 	sys_P(Y);
	// 	writecount++;
	// 	syscall_print("TestD start writing");
	// 	if(writecount==1)sys_P(rmutex);
	// 	sys_V(Y);
	// 	sys_P(wmutex);		
	// 	for(int i=0;i<3;i++){
	// 		syscall_print("TestD is writing");
	// 		syscall_delay(TICK_TIME);
	// 	}
	// 	sys_V(wmutex);
	// 	sys_P(Y);
	// 	writecount--;
	// 	syscall_print("TestD end writing");
	// 	if(writecount==0)sys_V(rmutex);
	// 	sys_V(Y);
	// }
}

/*======================================================================*
                               TestE
 *======================================================================*/
void TestE()
{
	while(1){
		syscall_P(wmutex);
		writecount++;
		syscall_print("TestE start writing");
		for(int i=0;i<4;i++){
			syscall_print("TestE is writing");
			syscall_delay(TICK_TIME);
		}
		writecount--;
		syscall_print("TestE end writing");
		syscall_V(wmutex);
	}

	// while(1){
	// 	sys_P(Y);
	// 	writecount++;
	// 	syscall_print("TestE start writing");
	// 	if(writecount==1)sys_P(rmutex);
	// 	sys_V(Y);

	// 	sys_P(wmutex);		
	// 	for(int i=0;i<3;i++){
	// 		syscall_print("TestE is writing");
	// 		syscall_delay(TICK_TIME);
	// 	}
	// 	sys_V(wmutex);

	// 	sys_P(Y);
	// 	writecount--;
	// 	syscall_print("TestE end writing");
	// 	if(writecount==0)sys_V(rmutex);
	// 	sys_V(Y);
	// }
}

/*======================================================================*
                               TestF
 *======================================================================*/
void TestF()
{
	while(1){
		char *output = "  readers is reading.  writers is writing.";
		*(output) = readcount+'0';
		*(output+21)=writecount+'0';
		syscall_print(output);
		// disp_int(readcount);
		// disp_str(" readers is reading.");
		// disp_str(" 0 writers is writing.");
		// disp_str("\n");
		syscall_delay(TICK_TIME);
	}
}
