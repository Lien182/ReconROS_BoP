

#ifndef __VIDEO_RESOLUTION_H__
#define __VIDEO_RESOLUTION_H__

//#include "xbasic_types.h"
#include <stdint.h>

// Video Pattern Generator - Video Resolution values
#define VIDEO_RESOLUTION_VGA       0
#define VIDEO_RESOLUTION_480P      1
#define VIDEO_RESOLUTION_576P      2
#define VIDEO_RESOLUTION_SVGA      3
#define VIDEO_RESOLUTION_XGA       4
#define VIDEO_RESOLUTION_720P      5
#define VIDEO_RESOLUTION_SXGA      6
#define VIDEO_RESOLUTION_1080P     7
#define VIDEO_RESOLUTION_UXGA      8
#define NUM_VIDEO_RESOLUTIONS      9

struct struct_vres_timing_t
{
	char *pName;
	uint32_t VActiveVideo;
	uint32_t VFrontPorch;
	uint32_t VSyncWidth;
	uint32_t VBackPorch;
	uint32_t VSyncPolarity;
	uint32_t HActiveVideo;
	uint32_t HFrontPorch;
	uint32_t HSyncWidth;
	uint32_t HBackPorch;
	uint32_t HSyncPolarity;
};
typedef struct struct_vres_timing_t vres_timing_t;

char *  vres_get_name(uint32_t resolutionId);
uint32_t vres_get_width(uint32_t resolutionId);
uint32_t vres_get_height(uint32_t resolutionId);
uint32_t vres_get_timing(uint32_t resolutionId, vres_timing_t *pTiming );

uint32_t vres_detect( uint32_t width, uint32_t height );


#endif // __VIDEO_RESOLUTION_H__
