/********************************************************************          
* axi_timer.c            -- user space driver for the diff_timer    *
*                        	ip-core			                        *
*                                                                   *  
* Author(s):  Christian Lienen                                      *   
*                                                                   *   
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "difference_measurement.h"
#include "memory.h"

t_diff_measurement * diff_timer_init( uint32_t base_addr )
{
	t_diff_measurement * diff_timer;

	diff_timer = (t_diff_measurement *)mmap(0, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, base_addr);
	if (diff_timer == MAP_FAILED) {
		printf("[Difference Timer] Init could not map memory\n");
		return NULL;
	}

	return diff_timer;
}