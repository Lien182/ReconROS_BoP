#include "recobop.h"
#include "reconos.h"
#include "reconos_thread.h"
#include "reconos_calls.h"
#include "utils.h"
#include "cycle_timer.h"
#include <math.h>
#include <stdio.h>

//Mode: Cycle or constant Position
#define REFERENCE_CYCLE 0
#define REFERENCE_CONST 1

//Debug optioons
//For tests, the motors can be turned off
#define VERBOSE 0
#define MOTOR_ON 1
#define DIRECTAXIACCESS 0
#define DEBUG_OUTPUT_KALMAN 0
#define FILE_LOG 0

#define ROS 1

//Konstants
#define KP -0.0004f
#define KD -0.0003f
#define SIGMAINV 0.02f
#define Q 2.0f
#define alpha 0.1f


void kalman(float Ta, float* u, float* y, float* x, float * P)
{
	int i;
	float tmp;
	float Kk[4];	
	float xk[4];	
	float Pk[16];	
	float* Pp = P; 
	float* xp = x;



	if(fabsf(xp[0]) < 300 && fabsf(xp[2]) < 300  )
	{
		//prediction
		xk[0] = xp[0] + Ta * xp[1];
		xk[1] = xp[1] - 210.21f * u[0];
		xk[2] = xp[2] + Ta * xp[3];
		xk[3] = xp[3] - 210.21f * u[1];
	}
	else
	{
		xk[0] = xp[0];
		xk[1] = xp[1];
		xk[2] = xp[2];
		xk[3] = xp[3];
	}
	


	Pk[0]  = Pp[0] 	+ Ta * Pp[4] + (Pp[1] + Ta * Pp[5]) * Ta + Q;
	Pk[1]  = Pp[4] 	+ Pp[5] * Ta;
	Pk[2]  = Pp[8] 	+ Ta * Pp[12] + (Pp[9] + Pp[13] * Ta) * Ta;
	Pk[3]  = Pp[12] + Ta * Pp[13];
	Pk[4]  = Pp[1] * Ta  * Pp[5] ;
	Pk[5]  = Pp[5] + Q;
	Pk[6]  = Pp[9] 	+ Pp[13] * Ta;
	Pk[7]  = Pp[13];

	Pk[8]  = Pp[2] + Ta * Pp[6] + (Pp[3] + Ta * Pp[7]) * Ta;
	Pk[9]  = Pp[6] + Pp[7] * Ta;
	Pk[10] = Pp[10] + Ta * Pp[14] + (Pp[11] + Pp[15] * Ta) * Ta + Q;
	Pk[11] = Pp[14] + Ta * Pp[15];
	Pk[12] = Pp[3] * Ta * Pp[7];
	Pk[13] = Pp[7];
	Pk[14] = Pp[11] + Pp[15] * Ta;
	Pk[15] = Pp[15] + Q;

	//Korrection
	xp[0] = xk[0];
	xp[1] = xk[1];
	xp[2] = xk[2];
	xp[3] = xk[3];

//	if( (fabsf(y[0] - xp[0]) < 100 ) && (fabsf(y[1] - xp[2]) < 100 ) )
	{
		for (i = 0; i < 2; i++)
		{
			Kk[0] = Pk[0  + 2*i] * SIGMAINV;
			Kk[1] = Pk[4  + 2*i] * SIGMAINV;
			Kk[2] = Pk[8  + 2*i] * SIGMAINV;
			Kk[3] = Pk[12 + 2*i] * SIGMAINV;

			tmp = (y[i] - xp[i * 2]);

	#if DEBUG_OUTPUT_KALMAN == 1
			printf("KK: (%3.5f,%3.5f,%3.5f,%3.5f) \n",Kk[0],Kk[1],Kk[2],Kk[3] );
			printf("Difference: %3.5f v1: %3.5f \n", tmp,xp[1]);		
	#endif

			xp[0] += Kk[0] * tmp;
			xp[1] += Kk[1] * tmp;
			xp[2] += Kk[2] * tmp;
			xp[3] += Kk[3] * tmp;

			memcpy(Pp, Pk, sizeof(float) * 16);

			Pp[0] -= Kk[0] * Pk[0 + 8 * i];
			Pp[1] -= Kk[0] * Pk[1 + 8 * i];
			Pp[2] -= Kk[0] * Pk[2 + 8 * i];
			Pp[3] -= Kk[0] * Pk[3 + 8 * i];

			Pp[4] -= Kk[1] * Pk[0 + 8 * i];
			Pp[5] -= Kk[1] * Pk[1 + 8 * i];
			Pp[6] -= Kk[1] * Pk[2 + 8 * i];
			Pp[7] -= Kk[1] * Pk[3 + 8 * i];

			Pp[8]  -= Kk[2] * Pk[0 + 8 * i];
			Pp[9]  -= Kk[2] * Pk[1 + 8 * i];
			Pp[10] -= Kk[2] * Pk[2 + 8 * i];
			Pp[11] -= Kk[2] * Pk[3 + 8 * i];

			Pp[12] -= Kk[3] * Pk[0 + 8 * i];
			Pp[13] -= Kk[3] * Pk[1 + 8 * i];
			Pp[14] -= Kk[3] * Pk[2 + 8 * i];
			Pp[15] -= Kk[3] * Pk[3 + 8 * i];

			memcpy(Pk, Pp, sizeof(float) * 16);
		}
	}

	
}


THREAD_ENTRY() {
	struct recobop_info *rb_info;
	int i;

	THREAD_INIT();
	rb_info = (struct recobop_info*)GET_INIT_DATA();

    float error_x_diff = 0,  error_x_last = 0;
	float error_y_diff = 0,  error_y_last = 0;

	float ctrl_x, ctrl_y;
#if REFERENCE_CYCLE == 1
	float dd = 0;
#endif
	float target_x, target_y;

	uint32_t bFirst = 1;

	float e[2];
	float u[2];
	float Ta = 0.02;
	float x[4];
	float P[16];
	float y[2];

	float y_old[2] = {0.0, 0.0};

	float t_p2b_ra_x = 0.0f;
	float t_p2b_ra_y = 0.0f;

	memset(y,0,2  * sizeof(float));
	memset(u,0,2  * sizeof(float));
	memset(x,0,4  * sizeof(float));
	memset(P,0,16 * sizeof(float));

	for(i = 0; i < 4; i++)
		P[i*4+i] = 1.0f;
	

#if FILE_LOG == 1
	fd = fopen("control_log.csv", "w+");
	if(fd == NULL)
	{
		printf("Control: Open control_log.csv failed \n");
	}
	fprintf(fd, "y[0], y[1], x[0], x[2], x[1], x[3], error_x, error_y, u[0], u[1]\n");
#endif 

#if DEBUG_OUTPUT_KALMAN == 1
	printf("y_1 %4.5f\t,y_2 %4.5f\t,x_1 %4.5f\t,x_2 %4.5f\t,v_1 %4.3f\t,v_2 %4.3f\t,e_1 %4.3f\t,e_2 %4.3f\t,u_1 %4.3f\t,u_2 %4.3f\n", y[0], y[1], x[0], x[2], x[1], x[3], e[0], e[1], u[0], u[1]);
#endif 

	while (1) {
		volatile uint32_t pos;

#if DIRECTAXIACCESS == 0
	
		if(rb_info->demo_nr == 0)
		{				
			ROS_SUBSCRIBE_TAKE(control_0_subdata, control_0_position_msg);
			pos = (control_0_position_msg->y & 0x00fff) | ((control_0_position_msg->x & 0x00fff ) << 12);
#if VERBOSE==1
			printf("[rt_control %d] \n", rb_info->demo_nr);
#endif
		}
		else if(rb_info->demo_nr == 1)
		{				
			ROS_SUBSCRIBE_TAKE(control_1_subdata, control_1_position_msg);
			pos = (control_1_position_msg->y & 0x00fff) | ((control_1_position_msg->x & 0x00fff ) << 12);
#if VERBOSE==1
			printf("[rt_control %d] \n", rb_info->demo_nr);
#endif
		}
		else if(rb_info->demo_nr == 2)
		{				
			ROS_SUBSCRIBE_TAKE(control_2_subdata, control_2_position_msg);
			pos = (control_2_position_msg->y & 0x00fff) | ((control_2_position_msg->x & 0x00fff ) << 12);
#if VERBOSE==1
			printf("[rt_control %d] \n", rb_info->demo_nr);
#endif
		}


		
#else
		cycle_timer_wait(&cycle_timer);	
		printf("[rt_control] ");
		a9timer_capture(a9timer, &a9cap_control_start, A9TIMER_CAPTURE_STOP);
		pos = (((uint32_t*)rb_info->pTouch)[1] & 0x00fff) | (((uint32_t*)rb_info->pTouch)[0] & 0x00fff )<< 12;
#endif

		
		
		int p_p_b_x = (pos >> 12) & 0xfff;
		if (((p_p_b_x >> 11) & 0x1) == 1) {
			p_p_b_x |= 0xfffff000;
		}
		int p_p_b_y = (pos >> 0) & 0xfff;
		if (((p_p_b_y >> 11) & 0x1) == 1) {
			p_p_b_y |= 0xfffff000;
		}


#if REFERENCE_CYCLE == 1
if(rb_info->demo_nr == 0)
{
	dd += 0.04;
	target_x = 30 * cosf(dd) ;
	target_y = 30 * sinf(dd) -60;
}
else if(rb_info->demo_nr == 1)
{
	dd += 0.02;
	target_x = 30 * cosf(dd) + 60;
	target_y = 30 * sinf(dd) - 60;
}
else
{	
	dd += 0.04;
	target_x = 30 * cosf(dd) + 60;
	target_y = 30 * sinf(dd) + 60;
}

#endif

#if REFERENCE_CONST == 1

		target_x = 0;
		target_y = 0;
#endif


		//printf("[rt_control] x_pos %08x, y_pos %08x \n", p_p_b_x, p_p_b_y);

		y[0] = (float)p_p_b_x * 116.67e-3 ;
		y[1] = (float)p_p_b_y * 116.67e-3 ;

		u[0] = t_p2b_ra_x / 180.0f * 3.14f;
		u[1] = t_p2b_ra_y / 180.0f * 3.14f;

		if(bFirst == 1)
		{
			bFirst = 0;
			x[0] = y[0];
			x[2] = y[1];
		}


		y[0] = y_old[0] * alpha + y[0] * (1.0-alpha);
		y_old[0] = y[0];
	
		y[1] = y_old[1] * alpha + y[1] * (1.0-alpha);
		y_old[1] = y[1];


		kalman(Ta, u, y, x, P);

		// calculate errors
		e[0] = x[0] - target_x;
		e[1] = x[2] - target_y;



		// implement PD controller for x
		error_x_diff = (e[0] - error_x_last) / Ta;
		error_x_last = e[0];
		

		float ctrl_x_p = KP * e[0];
		float ctrl_x_d = KD * error_x_diff;
		ctrl_x = ctrl_x_p + ctrl_x_d;


		// implement 
		error_y_diff = (e[1] - error_y_last) / Ta;
		error_y_last = e[1];


		float ctrl_y_p = KP * e[1];
		float ctrl_y_d = KD * error_y_diff;
		ctrl_y = ctrl_y_p +  ctrl_y_d;

		t_p2b_ra_x = -ctrl_y * 180.0f / 3.14f;		
		t_p2b_ra_y = ctrl_x  * 180.0f / 3.14f;
		
		if (t_p2b_ra_x > 11.0f)
			t_p2b_ra_x = 11.0f;
		else if(t_p2b_ra_x < -11.0f)
			t_p2b_ra_x = -11.0f;

		
		if (t_p2b_ra_y > 11.0f)
			t_p2b_ra_y = 11.0f;
		else if(t_p2b_ra_y < -11.0f)
			t_p2b_ra_y = -11.0f;

#if MOTOR_ON == 0
		t_p2b_ra_x = 0;
		t_p2b_ra_y = 0;
#endif

		// write command to inverse threads
		uint32_t cmd_x = fltofi(t_p2b_ra_x, 14, 6);
		uint32_t cmd_y = fltofi(t_p2b_ra_y, 14, 6);

#if FILE_LOG == 1
		fprintf(fd,"%4.3f\t,%4.3f\t,%4.3f\t,%4.3f\t,%4.3f\t,%4.3f\t,%4.3f\t,%4.3f\t,%4.3f\t,%4.3f\n", y[0], y[1], x[0], x[2], x[1], x[3], e[0], e[1], u[0], u[1]);
#endif

#if DEBUG_OUTPUT_KALMAN == 1
		printf("y_1 %4.3f\t,y_2 %4.3f\t,x_1 %4.3f\t,x_2 %4.3f\t,v_1 %4.3f\t,v_2 %4.3f\t,e_1 %4.3f\t,e_2 %4.3f\t,u_1 %4.3f\t,u_2 %4.3f\n", y[0], y[1], x[0], x[2], x[1], x[3], e[0], e[1], u[0], u[1]);
#endif


#ifdef ROS

		if(rb_info->demo_nr == 0)
		{				
			for (i = 0; i < 6; i++)
			{
				control_0_rotation_msg->cmd_x = cmd_x;
				control_0_rotation_msg->cmd_y = cmd_y;
				control_0_rotation_msg->leg = i;
				ROS_PUBLISH(control_0_pubdata, control_0_rotation_msg);
			}
		}
		else if(rb_info->demo_nr == 1)
		{				
			for (i = 0; i < 6; i++)
			{
				control_1_rotation_msg->cmd_x = cmd_x;
				control_1_rotation_msg->cmd_y = cmd_y;
				control_1_rotation_msg->leg = i;
				ROS_PUBLISH(control_1_pubdata, control_1_rotation_msg);
			}
		}
		else if(rb_info->demo_nr == 2)
		{				
			for (i = 0; i < 6; i++)
			{
				control_2_rotation_msg->cmd_x = cmd_x;
				control_2_rotation_msg->cmd_y = cmd_y;
				control_2_rotation_msg->leg = i;
				ROS_PUBLISH(control_2_pubdata, control_2_rotation_msg);
			}
		}

		

#else
		for (i = 0; i < 6; i++) MBOX_PUT(legacy_0_inverse_0_cmd, ((cmd_x << 17) | (cmd_y << 3) | (i << 0)));
#endif	

	}
}
