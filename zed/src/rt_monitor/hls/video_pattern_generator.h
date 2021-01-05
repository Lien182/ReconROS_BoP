#ifndef _VPG_H_
#define _VPG_H_
	
#include <hls_video.h>

#define HEIGHT 	600
#define WIDTH  	800
	
typedef hls::stream<ap_axiu<24,1,1,1> >		AXI_STREAM;

void video_pattern_generator(AXI_STREAM& m_axis_video);

#endif
