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

#define DEBUG 0

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

		printf("[rt_touch] ");
		a9timer_capture(a9timer, &a9cap_touch_start, A9TIMER_CAPTURE_STOP);
		

#if DEBUG == 0

		x_pos = ((int32_t*)rb_info->pTouch)[0] & 0x0fff;	
		if(x_pos & 0x0800) x_pos |= 0xfffff000;
		y_pos = ((int32_t*)rb_info->pTouch)[1] & 0x0fff;
		if(y_pos & 0x0800) y_pos |= 0xfffff000;

		//printf("[rt_touch] x_pos %08x, y_pos %08x \n", x_pos, y_pos);

#else
		x_pos = (int32_t)(200 * cos(dd));
		y_pos = (int32_t)(200 * sin(dd));
		dd += 0.01;
		//printf("[rt_touch] x_pos %08x, y_pos %08x \n", x_pos, y_pos);
#endif

		if(rb_info->demo_nr == 0)
		{
			touch_0_position_msg->x = x_pos;
			touch_0_position_msg->y = y_pos;
		
			printf("[rt_touch] ");
			a9timer_capture(a9timer, &a9cap_touch_end, A9TIMER_CAPTURE_STOP);
			ROS_PUBLISH(touch_0_pubdata, touch_0_position_msg);
		}
		else if(rb_info->demo_nr == 1)
		{
			touch_1_position_msg->x = x_pos;
			touch_1_position_msg->y = y_pos;
		
			printf("[rt_touch] ");
			a9timer_capture(a9timer, &a9cap_touch_end, A9TIMER_CAPTURE_STOP);
			ROS_PUBLISH(touch_1_pubdata, touch_1_position_msg);
		}
		else if(rb_info->demo_nr == 2)
		{
			touch_2_position_msg->x = x_pos;
			touch_2_position_msg->y = y_pos;
		
			printf("[rt_touch] ");
			a9timer_capture(a9timer, &a9cap_touch_end, A9TIMER_CAPTURE_STOP);
			ROS_PUBLISH(touch_2_pubdata, touch_2_position_msg);
		}

	}
}