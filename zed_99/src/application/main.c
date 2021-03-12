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
#define PIXEL_BYTE 4


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



	uint8_t * fb = malloc(HDMI_INPUT_WIDTH*HDMI_INPUT_HEIGHT*PIXEL_BYTE);

	while(1)
	{
		printf("HMDI Input: width=%d, height=%d \n", hdmi_input.width, hdmi_input.height);
		
		uint32_t addr = mbox_get(video_cmd);

		memcpy(fb, (void*)addr, HDMI_INPUT_HEIGHT*HDMI_INPUT_WIDTH*PIXEL_BYTE);

		video_image_msg_out->height = HDMI_INPUT_HEIGHT;
		video_image_msg_out->width = HDMI_INPUT_WIDTH;
		video_image_msg_out->encoding.data = "rgba8";
		video_image_msg_out->encoding.size = 6;
		video_image_msg_out->encoding.capacity = 7;
		video_image_msg_out->data.data = fb;
		video_image_msg_out->data.size = HDMI_INPUT_WIDTH*HDMI_INPUT_HEIGHT*PIXEL_BYTE;
		video_image_msg_out->data.capacity = HDMI_INPUT_WIDTH*HDMI_INPUT_HEIGHT*PIXEL_BYTE+1;
		video_image_msg_out->step = HDMI_INPUT_WIDTH*PIXEL_BYTE;
		

		ROS_PUBLISH(video_pubdata, video_image_msg_out);
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
