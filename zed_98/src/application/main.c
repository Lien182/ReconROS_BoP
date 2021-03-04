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


#define HDMI_INPUT_WIDTH 640
#define HDMI_INPUT_HEIGHT 480
#define HDMI_OUTPUT_WIDTH 1680
#define PIXEL_BYTE 4


#define BOP_0_TOUCH_BASE_ADDR 0x43C10000
#define BOP_0_SERVO_BASE_ADDR 0x43C00000
#define BOP_1_TOUCH_BASE_ADDR 0x43C30000
#define BOP_1_SERVO_BASE_ADDR 0x43C70000
#define BOP_2_TOUCH_BASE_ADDR 0x43C50000
#define BOP_2_SERVO_BASE_ADDR 0x43C60000


volatile struct recobop_info rb_info[3];

t_hdmi_output	hdmi_output;

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
	if(hdmi_output_init(&hdmi_output, "/dev/fb0") != 0)
	{
		printf("HDMI Output: Init error \n");
	}


	while(1)
	{

		ROS_SUBSCRIBE_TAKE(video_subdata, video_image_msg_in);

		for(int i = 0; i < HDMI_INPUT_HEIGHT;i++)
		{
			memcpy(&(hdmi_output.image[HDMI_OUTPUT_WIDTH*i]), &((uint32_t*)video_image_msg_in->data.data)[HDMI_INPUT_WIDTH*i], HDMI_INPUT_WIDTH*4);
		}
		//sleep(1);
		
		/*
		if(ros_subscriber_message_try_take(video_subdata, video_image_msg_in))
		{
			printf("MSG= 0x%x \n", video_image_msg_in);
			printf("Data =  x%x \n", OFFSETOF(sensor_msgs__msg__Image, data.data) + video_image_msg_in);
			printf("Data =  x%x \n", *((uint32_t*)(((uint32_t)OFFSETOF(sensor_msgs__msg__Image, data.data)) + ((uint32_t)video_image_msg_in))));
		}*/

		//printf("mbox: 0x%x \n", mbox_get(video_mbox_debug));
		
		
	}

	reconos_cleanup();
	reconos_app_cleanup();
}
