/********************************************************************          
* main.c           -       main function							*
*                        				                            *
*                                                                   *  
* Author(s): Christoph Rueting, Christian Lienen                    *   
*                                                                   *   
********************************************************************/
#include "recobop.h"

#include "reconos.h"
#include "reconos_app.h"
#include "reconos_calls.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>

#include "a9timer.h"
#include "memory.h"
#include "cycle_timer.h"
#include "axi_touch.h"
#include "axi_servo.h"
#include "hdmi.h"
#include "difference_measurement.h"


#define BOP_0_TOUCH_BASE_ADDR 0x43C10000
#define BOP_0_SERVO_BASE_ADDR 0x43C00000
#define BOP_1_TOUCH_BASE_ADDR 0x43C30000
#define BOP_1_SERVO_BASE_ADDR 0x43C70000
#define BOP_2_TOUCH_BASE_ADDR 0x43C50000
#define BOP_2_SERVO_BASE_ADDR 0x43C60000

#define DIFFERENCE_MEASUREMENT_ADDR 0x43C80000


#define HDMI_INPUT_WIDTH 640
#define HDMI_INPUT_HEIGHT 480
#define PIXEL_BYTE 4
#define HDMI_OUTPUT_WIDTH 1680



volatile struct recobop_info rb_info[3];

t_hdmi_input	hdmi_input;

#define OFFSETOF(type, member) ((uint32_t)(intptr_t)&(((type *)(void*)0)->member) )

static void exit_signal(int sig) 
{
	reconos_cleanup();
	printf("[recobop] aborted\n");
	exit(0);
}

void framegrabber_init(void)
{
	uint8_t * fb = malloc(HDMI_INPUT_WIDTH*HDMI_INPUT_HEIGHT*PIXEL_BYTE);
    video_image_msg_out->height = HDMI_INPUT_HEIGHT;
    video_image_msg_out->width = HDMI_INPUT_WIDTH;
    video_image_msg_out->encoding.data = "rgba8";
    video_image_msg_out->encoding.size = 6;
    video_image_msg_out->encoding.capacity = 7;
    video_image_msg_out->data.data = fb;
    video_image_msg_out->data.size = HDMI_INPUT_WIDTH*HDMI_INPUT_HEIGHT*PIXEL_BYTE;
    video_image_msg_out->data.capacity = HDMI_INPUT_WIDTH*HDMI_INPUT_HEIGHT*PIXEL_BYTE+1;
    video_image_msg_out->step = HDMI_INPUT_WIDTH*PIXEL_BYTE;

}


int main(int argc, char **argv) {
	reconos_init();
	reconos_app_init();

	if(memory_init() < 0)
	{
		printf("Error while allocating memory \n");
		return -1;
	}


	diff_measurement = diff_timer_init(DIFFERENCE_MEASUREMENT_ADDR);
/*
	if(hdmi_input_init(&(hdmi_input), "/dev/video0", video_cmd) != 0)
	{
		printf("HDMI Input: Init error \n");
		return -2;
	}

	if(hdmi_output_init(&hdmi_output, "/dev/fb0") != 0)
	{
		printf("HDMI Output: Init error \n");
	}
	else
		printf("HDMI Output: Width %d, Height %d \n", hdmi_output.width, hdmi_output.height);

*/
	cycle_timer_init(&cycle_timer, 20, cycle_timer_cmd_mutex, cycle_timer_cmd_cond);

	a9timer = a9timer_init();


	rb_info[0].timerregister = &(a9timer->TMR_CNT_REG_L);
	rb_info[0].rc_flag_control = 0UL;
	rb_info[0].rc_flag_inverse = 0UL;
	rb_info[0].pServo = (uint32_t*)axi_servo_init(BOP_0_SERVO_BASE_ADDR);
	rb_info[0].pTouch = (uint32_t*)axi_touch_init( BOP_0_TOUCH_BASE_ADDR);
	rb_info[0].demo_nr = 0UL;
	rb_info[0].rc_flag_touch = 0UL;
	rb_info[0].rc_flag_servo = 0UL;
	
	rb_info[1].timerregister = &(a9timer->TMR_CNT_REG_L);
	rb_info[1].rc_flag_control = 0UL;
	rb_info[1].rc_flag_inverse = 0UL;
	rb_info[1].pServo = (uint32_t*)axi_servo_init(BOP_1_SERVO_BASE_ADDR);
	rb_info[1].pTouch = (uint32_t*)axi_touch_init(BOP_1_TOUCH_BASE_ADDR);
	rb_info[1].demo_nr = 1UL;
	rb_info[1].rc_flag_touch = 0UL;
	rb_info[1].rc_flag_servo = 0UL;

	rb_info[2].timerregister = &(a9timer->TMR_CNT_REG_L);
	rb_info[2].rc_flag_control = 0UL;
	rb_info[2].rc_flag_inverse = 0UL;
	rb_info[2].pServo = (uint32_t*)axi_servo_init(BOP_2_SERVO_BASE_ADDR);
	rb_info[2].pTouch = (uint32_t*)axi_touch_init(BOP_2_TOUCH_BASE_ADDR);
	rb_info[2].demo_nr = 2UL;

	rb_info[2].rc_flag_touch = 0UL;
	rb_info[2].rc_flag_servo = 0UL;


	signal(SIGINT, exit_signal);
	signal(SIGTERM, exit_signal);
	signal(SIGABRT, exit_signal);	

	//framegrabber_init();

	//video_threads[0] =  reconos_thread_create_hwt_monitor  ((void *)hdmi_output.image);
	
	//video_threads[0] =  reconos_thread_create_hwt_monitor  ((void *)hdmi_output.image);

	//video_threads[0] =  reconos_thread_create_hwt_framegrabber  (video_image_msg_out->data.data);
	
	
	for(int i = 0; i < 3; i++)
	{
		printf("Init Data on %x \n", (uint32_t)&(rb_info[i]));
		printf("Servo %x, Touch %x \n", (uint32_t)rb_info[i].pServo, (uint32_t)rb_info[i].pTouch);
		printf("Init Servo data: 0: %x, 1: %x \n",((uint32_t*)rb_info[i].pServo)[0] & 0x0fff,((uint32_t*)rb_info[i].pServo)[1] & 0x0fff);
		printf("Init Touch data: 0: %x, 1: %x \n",((uint32_t*)rb_info[i].pTouch)[0] & 0x0fff,((uint32_t*)rb_info[i].pTouch)[1] & 0x0fff);
	
		
	
		if( i == 0)
		{
			rb_info[i].thread_p[0] = reconos_thread_create_hwt_touch    ((void *)&(rb_info[i]));
			rb_info[i].thread_p[3] = reconos_thread_create_swt_servo    ((void *)&(rb_info[i]), 71);		
		}
		else if(i == 1)
		{
			rb_info[i].thread_p[0] = reconos_thread_create_hwt_touch    ((void *)&(rb_info[i]));
			rb_info[i].thread_p[3] = reconos_thread_create_swt_servo    ((void *)&(rb_info[i]), 71);
			rb_info[i].thread_p[2] = reconos_thread_create_swt_inverse  ((void *)&(rb_info[i]), 72);
		}	
		else if(i == 2)
		{
			rb_info[i].thread_p[0] = reconos_thread_create_swt_touch    ((void *)&(rb_info[i]), 71);
			rb_info[i].thread_p[3] = reconos_thread_create_hwt_servo    ((void *)&(rb_info[i]));
		}


		
	}
	
	
	cycle_timer_start(&cycle_timer);
	
	while(1)
	{
		sleep(1);
	}
		
	reconos_cleanup();
	reconos_app_cleanup();
}
