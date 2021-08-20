
#include "reconos_thread.h"
#include "reconos_calls.h"

#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_math.h"

#define HDMI_INPUT_WIDTH 640
#define HDMI_INPUT_HEIGHT 480
#define PIXEL_BYTE 4

clock_t start, end;

THREAD_ENTRY()
{
	uint8 ram [HDMI_INPUT_WIDTH*PIXEL_BYTE];
	int32 img_msg_data;
	{
		#pragma HLS PROTOCOL fixed
		THREAD_INIT();
		img_msg_data = GET_INIT_DATA();
	}

	
	while (1)
	{		
		uint32_t addr = MBOX_GET(video_cmd);

			

		for(int i = 0; i < HDMI_INPUT_HEIGHT;i++)
		{
			MEM_READ(addr + HDMI_INPUT_WIDTH*i*PIXEL_BYTE, ram, 			HDMI_INPUT_WIDTH*PIXEL_BYTE);
			MEM_WRITE(ram, img_msg_data+ HDMI_INPUT_WIDTH*i*PIXEL_BYTE,  	HDMI_INPUT_WIDTH*PIXEL_BYTE);
		}

		ROS_PUBLISH(video_pubdata, video_image_msg_out);
	}

}