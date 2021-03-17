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
#define HDMI_OUTPUT_WIDTH 1680
#define PIXEL_BYTE 4

THREAD_ENTRY()
{
	THREAD_INIT();

    uint32_t * hdmi_out = GET_INIT_DATA();

	while(1)
	{
        ROS_SUBSCRIBE_TAKE(video_subdata, video_image_msg_in);
        //printf("Image received! Pointer=%x; ImagePointer=%x, Width = %d, Height = %d \n", hdmi_out, video_image_msg_in->data.data, video_image_msg_in->width, video_image_msg_in->height);

		for(int i = 0; i < HDMI_INPUT_HEIGHT;i++)
		{
			memcpy(&(hdmi_output.image[HDMI_OUTPUT_WIDTH*i]), &((uint32_t*)video_image_msg_in->data.data)[HDMI_INPUT_WIDTH*i], HDMI_INPUT_WIDTH*4);
		}

    }
}		
        
        
