/********************************************************************          
* cycle_timer.c            -- periodic software cycle timer         *
*                             other threads can use it through      *
*                             cycle_timer_wait()                    *
*                                                                   *  
* Author(s):  Christian Lienen                                      *   
*                                                                   *   
********************************************************************/

#include "cycle_timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <limits.h>

#include "a9timer.h"
#include "recobop.h"


void * cycle_timer_thread(void* arg)
{
    t_cycle_timer * cycle_timer;
    cycle_timer = (t_cycle_timer*)arg;

    struct timespec tim;
    tim.tv_sec = (cycle_timer->period * 1000000) / 1000000000;
    tim.tv_nsec = (cycle_timer->period * 1000000) % 1000000000;

    printf("[CYCLE TIMER] period set to %d s, %d ns\n",(uint32_t)tim.tv_sec,(uint32_t)tim.tv_nsec);

    while(!cycle_timer->bStart)
        nanosleep(&tim , NULL);

    while(1)
    {
        pthread_mutex_lock(cycle_timer->mutex);
        pthread_cond_broadcast(cycle_timer->cond);
        pthread_mutex_unlock(cycle_timer->mutex);

        printf("[CYCLE TIMER] ");
        nanosleep(&tim , NULL);
    }
}


//Starts the software cycle timer, period is given in ms; priority is 79
void cycle_timer_init(t_cycle_timer * cycle_timer, uint64_t period, pthread_mutex_t* mutex, pthread_cond_t* cond )
{

    cycle_timer->mutex  = mutex;
    cycle_timer->cond   = cond;
    cycle_timer->period = period;

    cycle_timer->bStart = 0;

    struct sched_param param;
    pthread_attr_t attr;
    int ret;

    /* Lock memory */
    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
            printf("[CYCLE TIMER] mlockall failed: %m\n");
            exit(-2);
    }

    /* Initialize pthread attributes (default values) */
    ret = pthread_attr_init(&attr);
    if (ret) {
            printf("[CYCLE TIMER] init pthread attributes failed\n");
            return;
    }

    /* Set a specific stack size  */
    ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    if (ret) {
        printf("[CYCLE TIMER] pthread setstacksize failed\n");
        return;
    }

    /* Set scheduler policy and priority of pthread */
    ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    if (ret) {
            printf("[CYCLE TIMER] pthread setschedpolicy failed\n");
            return;
    }
    param.sched_priority = 79;
    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret) {
            printf("[CYCLE TIMER] pthread setschedparam failed\n");
            return;
    }
    /* Use scheduling parameters of attr */
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
            printf("[CYCLE TIMER] pthread setinheritsched failed\n");
            return;
    }
    
    pthread_create(&cycle_timer->timer_thread, &attr, &cycle_timer_thread, (void*)cycle_timer);
	
}

void cycle_timer_wait(t_cycle_timer * cycle_timer)
{
        pthread_mutex_lock(cycle_timer->mutex);
        pthread_cond_wait(cycle_timer->cond, cycle_timer->mutex);
        pthread_mutex_unlock(cycle_timer->mutex); 
}


void cycle_timer_start(t_cycle_timer * cycle_timer)
{
        cycle_timer->bStart = 1UL;
}


void cycle_timer_stop(t_cycle_timer * cycle_timer)
{
        cycle_timer->bStart = 0UL;
}