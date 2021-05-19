#include "reconos_thread.h"
#include "reconos_calls.h"

#include <math.h>
#include <stdio.h>


#define HDMI_INPUT_WIDTH 640
#define HDMI_INPUT_HEIGHT 480
#define HDMI_OUTPUT_WIDTH 1680
#define PIXEL_BYTE 4

clock_t start, end;

THREAD_ENTRY()
{
	int j = 1;
	THREAD_INIT();

    uint32_t * hdmi_out = (uint32_t *)GET_INIT_DATA();

	while(1)
	{
		end = clock();
		if(j==1)
		{
			j=0;
			
		}
		else
			printf("%3.6f\n", (double)(end-start)/CLOCKS_PER_SEC);
		
        ROS_SUBSCRIBE_TAKE(video_subdata, video_image_msg_in);
		start = clock();
        
		for(int i = 0; i < HDMI_INPUT_HEIGHT;i++)
		{
			memcpy(&(hdmi_out[HDMI_OUTPUT_WIDTH*i]), &((uint32_t*)video_image_msg_in->data.data)[HDMI_INPUT_WIDTH*i], HDMI_INPUT_WIDTH*4);
		
		}

    }
}		
        
        
