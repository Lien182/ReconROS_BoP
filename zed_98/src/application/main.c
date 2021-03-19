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

volatile struct recobop_info rb_info[3];


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
	
	/*
	if(hdmi_output_init(&hdmi_output, "/dev/fb0") != 0)
	{
		printf("HDMI Output: Init error \n");
	}
	*/


	for(int i = 1; i < 2; i++)
	{
		rb_info[i].demo_nr = i;
		rb_info[i].thread_p[1] = reconos_thread_create_swt_control  ((void *)&(rb_info[i]), 73);
		rb_info[i].thread_p[2] = reconos_thread_create_swt_inverse  ((void *)&(rb_info[i]), 72);			
	}


	//rb_info[0].thread_p[2] = reconos_thread_create_swt_monitor  ((void *)hdmi_output.image, 0);

	while(1)
	{


		sleep(10);
		
		
	}

	reconos_cleanup();
	reconos_app_cleanup();
}
