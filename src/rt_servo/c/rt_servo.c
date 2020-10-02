/********************************************************************          
* rt_servo.c  -- software servo thread implementation      			*
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
	uint32_t cmd = 0;


	THREAD_INIT();
	rt = (struct reconos_thread *)GET_INIT_DATA();
	rb_info = (struct recobop_info*)rt->init_data;

	while (1) {

		ROS_SUBSCRIBE_TAKE(servo_0_subdata, servo_0_legangle_msg);
		
		//if(servo_0_legangle_msg->leg  == 0)
		{
			printf("[rt_servo] ");
			a9timer_capture(a9timer, &a9cap_servo_start, A9TIMER_CAPTURE_STOP);
		}
			

		

#if DEBUG == 1
		printf("[rt_servo %d] Got new data from inverse\n",rb_info->demo_nr);
#endif
		((uint32_t*)(rb_info->pServo))[(servo_0_legangle_msg->leg) & 7] = servo_0_legangle_msg->angle;


	}
}