
#ifndef __VIDEO_GENERATOR_H__
#define __VIDEO_GENERATOR_H__

#include "xvtc.h"
#include "hdmi_display_bsp/ps7_cortexa9_0/include/xparameters.h"


/*
 * Function prototypes
 */
int vgen_init(XVtc *pVtc, uint32_t* base_addr, uint16_t VtcDeviceID);
int vgen_config(XVtc *pVtc, int ResolutionId, int bVerbose);

#endif // __VIDEO_GENERATOR_H__
