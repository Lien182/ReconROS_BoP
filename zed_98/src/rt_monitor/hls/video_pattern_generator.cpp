#include "video_pattern_generator.h"
#include "reconos_thread.h"
#include "reconos_calls.h"

#include <sensor_msgs/msg/image.h>




void ros_handling(hls::stream<uint32>& osif_sw2hw,\
                                   hls::stream<uint32>& osif_hw2sw,\
                                   hls::stream<uint32>& memif_hwt2mem,\
                                   hls::stream<uint32>& memif_mem2hwt, uint32 imgbuffer[WIDTH*HEIGHT/4])
{
	static bool bInit = 1;

	uint32 msg_ptr;

	if(bInit)
	{
		THREAD_INIT();
		bInit = 0;
	}

	uint32 ret = ROS_SUBSCRIBE_TRYTAKE(video_subdata, video_image_msg_in, msg_ptr );

	if(ret == 1)
	{
		uint32 payload_addr[1];
		#pragma HLS PROTOCOL fixed
		MBOX_PUT(video_mbox_debug, 0x30 + msg_ptr);
		MEM_READ(0x30 + msg_ptr, payload_addr, 4);
		MBOX_PUT(video_mbox_debug, payload_addr[0]);
		if(payload_addr[0] != 0)
			MEM_READ(payload_addr[0], imgbuffer, WIDTH*HEIGHT);
	}
}


void video_stream(uint32 imgbuffer[WIDTH*HEIGHT/4], AXI_STREAM& m_axis_video)
{
	#pragma HLS INTERFACE axis register both port=m_axis_video

	ap_axiu<24, 1, 1, 1> video;
	uint8_t * pline = (uint8_t *)imgbuffer;

	for(int i = 0; i < HEIGHT; i++)
	{		
	#pragma HLS loop_flatten
		for(int j= 0; j < WIDTH; j++)
		{
	#pragma HLS PIPELINE II=1
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

			video.data = (ap_uint<24>)(((uint32)(pline[0]) << 16) || ((uint32)(pline[0]) << 8) || ((uint32)(pline[0]) << 0 )); 
			pline += 1;

			//Send video to stream
			m_axis_video << video;
			
		}
	}
}



//Top Level Function
void rt_imp(hls::stream<uint32> osif_sw2hw,\
            hls::stream<uint32> osif_hw2sw,\
            hls::stream<uint32> memif_hwt2mem,\
            hls::stream<uint32> memif_mem2hwt,
			AXI_STREAM& m_axis_video)
{
	//directive added from directive panel - to make this a axi4-stream interface
#pragma HLS INTERFACE axis register both port=m_axis_video

	uint32 imgbuffer[WIDTH*HEIGHT/4];

	while(1)
	{		
		#pragma HLS dependence variable=imgbuffer inter false
		#pragma HLS dependence variable=imgbuffer intra false        
		#pragma HLS dataflow
		ros_handling(osif_sw2hw,osif_hw2sw,memif_hwt2mem,memif_mem2hwt, imgbuffer);
		video_stream(imgbuffer, m_axis_video);
	}		
		
	
}

