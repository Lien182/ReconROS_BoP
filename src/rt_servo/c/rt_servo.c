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

#define VERBOSE 0

THREAD_ENTRY() {
	struct recobop_info *rb_info;


	THREAD_INIT();
	rb_info = (struct recobop_info*)GET_INIT_DATA();

	while (1) {

		if(rb_info->demo_nr == 0)
		{
			ROS_SUBSCRIBE_TAKE(servo_0_subdata, servo_0_legangle_msg);
			((uint32_t*)(rb_info->pServo))[(servo_0_legangle_msg->leg) & 7] = servo_0_legangle_msg->angle;

			
#if VERBOSE==1
			printf("[rt_servo %d] \n", rb_info->demo_nr);
#endif
		}

		else if(rb_info->demo_nr == 1)
		{
			ROS_SUBSCRIBE_TAKE(servo_1_subdata, servo_1_legangle_msg);
			((uint32_t*)(rb_info->pServo))[(servo_1_legangle_msg->leg) & 7] = servo_1_legangle_msg->angle;


#if VERBOSE==1
			printf("[rt_servo %d] \n", rb_info->demo_nr);
#endif
		}			
		else if(rb_info->demo_nr == 2)
		{
			ROS_SUBSCRIBE_TAKE(servo_2_subdata, servo_2_legangle_msg);
			((uint32_t*)(rb_info->pServo))[(servo_2_legangle_msg->leg) & 7] = servo_2_legangle_msg->angle;


#if VERBOSE==1
			printf("[rt_servo %d] \n", rb_info->demo_nr);
#endif
		}
			
		/*
		if(rb_info->demo_nr == 0)
		{
			printf("[rt_servo] ");
			a9timer_capture(a9timer, &a9cap_servo_start, A9TIMER_CAPTURE_STOP);
		}
		*/	
	}
}