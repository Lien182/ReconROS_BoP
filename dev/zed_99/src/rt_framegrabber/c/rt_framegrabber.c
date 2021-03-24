#include "recobop.h"
#include "reconos.h"

#include "reconos_thread.h"
#include "reconos_calls.h"
#include "utils.h"
#include "hdmi_input.h"
#include "hdmi_output.h"

#include <math.h>
#include <stdio.h>


#define HDMI_INPUT_WIDTH 640
#define HDMI_INPUT_HEIGHT 480
#define PIXEL_BYTE 4

THREAD_ENTRY()
{
	THREAD_INIT();

	
	//struct reconos_thread * rt = GET_INIT_DATA();


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


	while(1)
	{
		//printf("HMDI Input: width=%d, height=%d \n", hdmi_input.width, hdmi_input.height);
		
		uint32_t addr = mbox_get(video_cmd);

		memcpy(fb, (void*)addr, HDMI_INPUT_HEIGHT*HDMI_INPUT_WIDTH*PIXEL_BYTE);

		ROS_PUBLISH(video_pubdata, video_image_msg_out);
    
        usleep(50000);

    }
}