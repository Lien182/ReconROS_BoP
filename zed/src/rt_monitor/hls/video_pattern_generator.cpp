#include "video_pattern_generator.h"
#include "reconos_thread.h"
#include "reconos_calls.h"

#include <sensor_msgs/msg/image.h>

//Top Level Function
void rt_imp(hls::stream<uint32> osif_sw2hw,\
                                   hls::stream<uint32> osif_hw2sw,\
                                   hls::stream<uint32> memif_hwt2mem,\
                                   hls::stream<uint32> memif_mem2hwt,
								   AXI_STREAM& m_axis_video)
{
	//directive added from directive panel - to make this a axi4-stream interface
#pragma HLS INTERFACE axis register both port=m_axis_video

	ap_axiu<24, 1, 1, 1> video;
//	hls::rgb_8 pixel;
	int cbar;

	uint32 output_buffer_addr;

	{
		#pragma HLS PROTOCOL fixed
		THREAD_INIT();
		output_buffer_addr = GET_INIT_DATA();

		uint32 payload_addr[1];
		uint32 msg_ptr = ROS_SUBSCRIBE_TAKE(video_subdata, video_image_msg_in );
		MEM_READ(OFFSETOF(sensor_msgs__msg__Image, data.data) + msg_ptr, payload_addr, 4);
		MEM_WRITE(payload_addr, OFFSETOF(sensor_msgs__msg__Image, data.data) + msg_ptr, 4);
	}
	
	while(1)
	{

		//MEM_READ( payload_addr[0], ram, 800*600);

	//Add code for output video generation here
		for(int i = 0; i < HEIGHT; i++)
		{
	#pragma HLS PIPELINE
			for(int j= 0; j < WIDTH; j++)
			{
				// Start of frame, assert tuser
				if((i==0)&&(j==0))
					video.user=1;
				else
					video.user=0;

				//End of line, assert tlast
				if(j==WIDTH-1)
					video.last = 1;
				else
					video.last = 0;

				//colour bar generation
				cbar = j / 100; // color bar = 0..7
				switch ( cbar )
				{
					case 0: video.data = (ap_uint<24>)0x00000000; break; // Black
					case 1: video.data = (ap_uint<24>)0x00FF0000; break; // Red
					case 2: video.data = (ap_uint<24>)0x0000FF00; break; // Green
					case 3: video.data = (ap_uint<24>)0x000000FF; break; // Blue
					case 4: video.data = (ap_uint<24>)0x0000FFFF; break; // Cyan
					case 5: video.data = (ap_uint<24>)0x00FF00FF; break; // Purple
					case 6: video.data = (ap_uint<24>)0x00FFFF00; break; // Yellow
					case 7: video.data = (ap_uint<24>)0x00FFFFFF; break; // White
				}

				//Send video to stream
				m_axis_video << video;
			}
		}
	}
	
}

