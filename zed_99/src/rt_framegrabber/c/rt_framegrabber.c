#include "reconos_thread.h"
#include "reconos_calls.h"

#include <sensor_msgs/msg/image.h>

clock_t start, end;

#define HDMI_INPUT_WIDTH 640
#define HDMI_INPUT_HEIGHT 480
#define PIXEL_BYTE 4

THREAD_ENTRY()
{
	THREAD_INIT();

	while(1)
	{	
		uint32_t addr = mbox_get(video_cmd);
        start = clock();
		memcpy(video_image_msg_out->data.data, (void*)addr, HDMI_INPUT_HEIGHT*HDMI_INPUT_WIDTH*PIXEL_BYTE);
        end = clock();
        printf("%3.6f\n", (double)(end-start)/CLOCKS_PER_SEC);
		ROS_PUBLISH(video_pubdata, video_image_msg_out);
  
        usleep(50000);

    }
}