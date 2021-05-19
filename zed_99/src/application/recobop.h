/********************************************************************          
* recobop.h           -- header file for the ball on plate          *
*                        demonstrator                               *
*                                                                   *  
* Author(s): Christoph Rueting, Christian Lienen                    *   
*                                                                   *   
********************************************************************/
#ifndef RECOBOP_H
	#define RECOBOP_H

	#include "reconos.h"
	#include "log.h"
	#include "a9timer.h"
	#include "hdmi_input.h"
	#include "hdmi_output.h"
	#include "cycle_timer.h"

	#include <math.h>
	#include <stdint.h>
	#include <string.h>



	struct recobop_info {
		volatile uint32_t* pTouch;				// Offset +00
		volatile uint32_t* pServo;				// Offset +04
		volatile uint32_t  demo_nr;				// Offset +08
		volatile uint32_t* timerregister;		// Offset +12
		volatile uint32_t* stackaddr_control;	// Offset +16
		volatile uint32_t  rc_flag_control; 	// Offset +20
		volatile uint32_t  threadid_control;	// Offset +24
		volatile uint32_t* stackaddr_inverse;   // Offset +28
		volatile uint32_t  rc_flag_inverse; 	// Offset +32
		volatile uint32_t  threadid_inverse;	// Offset +36

		volatile uint32_t  rc_flag_touch; 		// Offset +40
		volatile uint32_t  threadid_touch;		// Offset +44
		volatile uint32_t  rc_flag_servo; 		// Offset +48
		volatile uint32_t  threadid_servo;		// Offset +52

		volatile struct 	reconos_thread *thread_p[4];
	};

	t_hdmi_output	hdmi_output;

	volatile struct 	reconos_thread *video_threads[4];	

	static inline uint32_t fltofi(float f, int n, int dn) {
		int i;
		int wn = n - dn;

		int d;
		float w;

		uint32_t df, wf;
		uint32_t m = 0xffffffff;

		if (f > 0) {
			d = floor(f);
			w = f - d;
		} else {
			d = floor(f);
			w = f - d;
		}

		df = 0 | (d << wn);

		wf = 0;
		for (i = -1; i >= -wn; i--) {
			if (w >= pow(2, i)) {
				wf |= 1 << (wn + i);
				w -= pow(2, i);
			}
		}

		return (df | wf) & (m >> (32 - n));
	}

	static inline float fitofl(uint32_t f, int n, int dn) {
		int i;
		int wn = n - dn;

		int d;
		float w;

		uint32_t m = 0xffffffff;

		if (((f >> (n - 1)) & 0x1) == 1) {
			d = (m << dn) | (f >> wn);
		} else {
			d = 0 | (f >> wn);
		}

		w = 0;
		for (i = -1; i >= -wn; i--) {
			if (((f >> (wn + i)) & 0x1) == 1) {
				w += pow(2, i);
			}
		}

		return d + w;
	}


	t_a9timer * a9timer;
	t_cycle_timer cycle_timer;

	t_a9timer_capture a9cap_touch_start;
	t_a9timer_capture a9cap_touch_end;
	t_a9timer_capture a9cap_control_start;
	t_a9timer_capture a9cap_control_end;
	t_a9timer_capture a9cap_inverse_start;
	t_a9timer_capture a9cap_inverse_end;
	t_a9timer_capture a9cap_servo_start;
	
	
	t_a9timer_capture a9cap_cycle_time;

#else
	extern t_a9timer * a9timer;
	extern t_cycletimer cycle_timer;


	extern t_a9timer_capture a9cap_touch_start;
	extern t_a9timer_capture a9cap_touch_end;
	extern t_a9timer_capture a9cap_control_start;
	extern t_a9timer_capture a9cap_control_end;
	extern t_a9timer_capture a9cap_inverse_start;
	extern t_a9timer_capture a9cap_inverse_end;
	extern t_a9timer_capture a9cap_servo_start;
	extern t_a9timer_capture a9cap_servo_end;

#endif /* RECOBOP_H */