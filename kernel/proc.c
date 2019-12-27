
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
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
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	
	
	PROCESS* p;

	//int	 greatest_ticks = 0;
	int count =0;
	while(1){
		if(proc_current==NR_TASKS-1){
			proc_current=0;
		}else{
			proc_current++;
		}
		count++;
		//if(count>NR_TASKS||((proc_table+proc_current)->sleep_tick==0&&(proc_table+proc_current)->block==0))break;
		if(count>NR_TASKS||(proc_table+proc_current)->sleep_tick==0&&(proc_table+proc_current)->block==0)break;
	}
	if(count<=NR_TASKS)
		p_proc_ready=proc_table+proc_current;
	else{
		for (p = proc_table; p < proc_table + NR_TASKS; p++){
			if (p->sleep_tick > 0){
				p->sleep_tick--;
			}			
		}
		schedule();
	}
	
	
	//if(p_proc_ready)disp_color_str(p_proc_ready->p_name,7);

	// while (!greatest_ticks) {
	// 	for (p = proc_table; p < proc_table+NR_TASKS; p++) {
	// 		if (!p->sleep_tick && p->ticks > greatest_ticks) {
	// 			greatest_ticks = p->ticks;
	// 			p_proc_ready = p;
	// 		}
	// 	}

	// 	if (!greatest_ticks) {
	// 		for (p = proc_table; p < proc_table+NR_TASKS; p++) {
	// 			if (!p->sleep_tick)
	// 				p->ticks = p->priority;
	// 		}
	// 	}
	// }

	//disp_color_str(p_proc_ready->p_name,3);
}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}

PUBLIC void sys_delay(int milli_seconds){
	p_proc_ready->sleep_tick = milli_seconds / TICK_TIME;
	schedule();
}

PUBLIC void sys_print(char *str){
	int color = p_proc_ready-proc_table+2;
	for(int i=0;i<80;i++)disp_str(" ");
	disp_pos-=160;
	//disp_int(ticks);
	// for(int i=0;i<NR_TASKS;i++){
	// 	str[i] = (proc_table+i)->block+'0';
	// }
	disp_color_str(str,color);
	disp_str("\n");
	//schedule();
}

PUBLIC void sys_P(SEMAPHORE* s){
	// disp_int(s->value);
	s->value--;
	// disp_int(s->value);
	if(s->value < 0){		
		if(s -> head == 0){
			s->head = p_proc_ready;
		}
		else{
			PROCESS* rear = s->head;
			while(rear->next != 0){
				rear = rear -> next;
			}
			rear->next = p_proc_ready;
		}
		p_proc_ready->block = 1;
		schedule();
	}
	//disp_int(s->value);
	// schedule();
}

PUBLIC void sys_V(SEMAPHORE* s){
	// s->value=s->value-2;
	// disp_int(s->value);
	s->value++;
	// disp_int(s->value);
	if(s->value <= 0){
		p_proc_ready = s->head;
		s->head = s-> head -> next;
		p_proc_ready->next = 0;
		p_proc_ready->block = 0;		
	}
	schedule();
}



