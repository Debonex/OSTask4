
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               clock.c
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
                           clock_handler
 *======================================================================*/
PUBLIC void clock_handler(int irq)
{
	PROCESS *p;

	ticks++;
	// disp_int(ticks);
	// disp_str("\n");
	//p_proc_ready->ticks--;
	//disp_int(ticks);
	if (k_reenter != 0) {
		return;
	}

	// if (p_proc_ready->ticks > 0) {
	// 	return;
	// }
	for (p = proc_table; p < proc_table + NR_TASKS; p++){
		if (p->sleep_tick > 0){
			p->sleep_tick--;
		}			
	}

	schedule();
	
}

/*======================================================================*
                              milli_delay
 *======================================================================*/
PUBLIC void milli_delay(int milli_sec)
{
        int t = get_ticks();

        while(((get_ticks() - t) * 1000 / HZ) < milli_sec) {}
}

