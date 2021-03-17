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


#define BOP_0_TOUCH_BASE_ADDR 0x43C10000
#define BOP_0_SERVO_BASE_ADDR 0x43C00000
#define BOP_1_TOUCH_BASE_ADDR 0x43C30000
#define BOP_1_SERVO_BASE_ADDR 0x43C70000
#define BOP_2_TOUCH_BASE_ADDR 0x43C50000
#define BOP_2_SERVO_BASE_ADDR 0x43C60000


volatile struct recobop_info rb_info[3];

t_hdmi_input	hdmi_input;

#define OFFSETOF(type, member) ((uint32_t)(intptr_t)&(((type *)(void*)0)->member) )

static void exit_signal(int sig) 
{
	reconos_cleanup();
	printf("[recobop] aborted\n");
	exit(0);
}


int main(int argc, char **argv) {
	reconos_init();
	reconos_app_init();

	if(memory_init() < 0)
	{
		printf("Error while allocating memory \n");
		return -1;
	}

	if(hdmi_input_init(&(hdmi_input), "/dev/video0", video_cmd) != 0)
	{
		printf("HDMI Input: Init error \n");
		return -2;
	}

	rb_info[0].thread_p[2] = reconos_thread_create_swt_framegrabber  ((void *)&(rb_info[0]), 0);


	while(1)
	{
		sleep(1);
	}
		
	reconos_cleanup();
	reconos_app_cleanup();
}
