/********************************************************************          
* rt_touch.c           -- software touch thread implementation      *
*                                                                   *
*                                                                   *  
* Author(s):  Christian Lienen                                      *   
*                                                                   *   
********************************************************************/

#include "recobop.h"
#include "reconos.h"
#include "reconos_thread.h"
#include "reconos_calls.h"
#include "utils.h"
#include "cycle_timer.h"
#include <math.h>
#include <stdio.h>

#define DEBUG 1

THREAD_ENTRY() {
	struct recobop_info *rb_info;
	struct reconos_thread * rt;
	uint32_t x_pos, y_pos;

#if DEBUG == 1
	double dd = 0.0;
#endif


	THREAD_INIT();
	rt = (struct reconos_thread *)GET_INIT_DATA();
	rb_info = (struct recobop_info*)rt->init_data;

#if DEBUG == 1
	printf("[touch thread %d] waiting for cycle timer \n", rb_info->demo_nr );
#endif
	
	while (1) {
		
		cycle_timer_wait(&cycle_timer);

#if DEBUG == 0

		x_pos = ((int32_t*)rb_info->pTouch)[0] & 0x0fff;	
		if(x_pos & 0x0800) x_pos |= 0xfffff000;
		y_pos = ((int32_t*)rb_info->pTouch)[1] & 0x0fff;
		if(y_pos & 0x0800) y_pos |= 0xfffff000;

#else
		x_pos = (int32_t)(200 * cos(dd));
		y_pos = (int32_t)(200 * sin(dd));
		dd += 0.01;
		//printf("[rt_touch] x_pos %08x, y_pos %08x \n", x_pos, y_pos);
#endif

		switch(rb_info->demo_nr)
		{
			case 0: mbox_put(touch_0_pos, 0 | ((x_pos & 0xfff) << 12) | ((y_pos & 0xfff) << 0)); break;
			case 1: mbox_put(touch_1_pos, 0 | ((x_pos & 0xfff) << 12) | ((y_pos & 0xfff) << 0)); break;
			case 2: mbox_put(touch_2_pos, 0 | ((x_pos & 0xfff) << 12) | ((y_pos & 0xfff) << 0)); break;
		}
	}
}