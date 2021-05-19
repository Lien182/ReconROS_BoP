#include "reconos_thread.h"
#include "reconos_calls.h"

#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_math.h"

#include <sensor_msgs/msg/image.h>

#define HDMI_INPUT_WIDTH 640
#define HDMI_INPUT_HEIGHT 480
#define PIXEL_BYTE 4
#define HDMI_OUTPUT_WIDTH 1680


THREAD_ENTRY()
{
	uint8 ram [HDMI_INPUT_WIDTH*PIXEL_BYTE];
	int32 fb;
	{
		#pragma HLS PROTOCOL fixed
		THREAD_INIT();
		fb = GET_INIT_DATA();
	}

	
	while (1)
	{		
		uint32 pPayload[1];
		uint32 pMessage= ROS_SUBSCRIBE_TAKE(video_subdata, video_image_msg_in );
		MEM_READ(OFFSETOF(sensor_msgs__msg__Image, data.data) + pMessage, pPayload, 4);
		
		for(int i = 0; i < HDMI_INPUT_HEIGHT;i++)
		{
			MEM_READ(pPayload[0]+HDMI_INPUT_WIDTH*i*PIXEL_BYTE, ram, HDMI_INPUT_WIDTH*PIXEL_BYTE);
			MEM_WRITE(ram , fb + HDMI_OUTPUT_WIDTH*i*PIXEL_BYTE, HDMI_INPUT_WIDTH*PIXEL_BYTE);
		}

  
		
	}

}