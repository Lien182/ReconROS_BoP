/********************************************************************          
* main.c           -       main function							*
*                        				                            *
*                                                                   *  
* Author(s): Christoph Rueting, Christian Lienen                    *   
*                                                                   *   
********************************************************************/
#include "recobop.h"

#include "reconos.h"
#include "reconos_app.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>

#include "a9timer.h"
#include "memory.h"
#include "cycle_timer.h"
#include "axi_touch.h"
#include "axi_servo.h"
#include "hdmi.h"


#include "hdmi_driver/zed_hdmi_display.h"
#include "hdmi_driver/hdmi_display_bsp/ps7_cortexa9_0/include/xparameters.h"

#define FB_SIZE 1920*1080*4

zed_hdmi_display_t demo;


#define BOP_0_TOUCH_BASE_ADDR 0x43C10000
#define BOP_0_SERVO_BASE_ADDR 0x43C00000
#define BOP_1_TOUCH_BASE_ADDR 0x43C30000
#define BOP_1_SERVO_BASE_ADDR 0x43C70000
#define BOP_2_TOUCH_BASE_ADDR 0x43C50000
#define BOP_2_SERVO_BASE_ADDR 0x43C60000


volatile struct recobop_info rb_info[3];

t_hdmi video_info;


static void exit_signal(int sig) 
{
	reconos_cleanup();
	printf("[recobop] aborted\n");
	exit(0);
}


int main(int argc, char **argv) {
	reconos_init();
	reconos_app_init();

	if(memory_init() < 0)
	{
		printf("Error while allocating memory \n");
		return -1;
	}



	uint32_t* fb = malloc(FB_SIZE);
	memset(fb, 0xaa, FB_SIZE); 

	demo.uBaseAddr_IIC_HdmiOut = mmap(0, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, memfd, XPAR_ZED_HDMI_IIC_0_BASEADDR);
    demo.uDeviceId_VTC_HdmioGenerator = XPAR_ZED_HDMI_DISPLAY_V_TC_0_DEVICE_ID;
    demo.uBaseAddr_VTC_HdmioGenerator = mmap(0, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, memfd, XPAR_ZED_HDMI_DISPLAY_V_TC_0_BASEADDR);
	demo.uDeviceId_VDMA_HdmiDisplay = XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_DEVICE_ID;
	demo.uBaseAddr_VDMA_HdmiDisplay = mmap(0, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, memfd, XPAR_ZED_HDMI_DISPLAY_AXI_VDMA_0_BASEADDR);
    demo.uBaseAddr_MEM_HdmiDisplay = (fb);
    demo.uNumFrames_HdmiDisplay = XPAR_AXIVDMA_0_NUM_FSTORES;



	uint32_t * framedata = malloc(480 * 640 * 4);
	rb_info[0].thread_p[1] = reconos_thread_create_hwt_monitor  (framedata);

	//cycle_timer_start(&cycle_timer);




	//video_info.thread_p = reconos_thread_create_hwt_color2bw((void*)(&(video_info.rc_flag)));


	zed_hdmi_display_init( &demo );
	
	reconos_cleanup();
	reconos_app_cleanup();
}
