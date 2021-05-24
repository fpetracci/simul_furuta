#ifndef PTASK_H
	#define PTASK_H

#include <time.h>
#include <sched.h>
#include <pthread.h>
#include "condiviso.h"

// Function declaration
int task_create(
				void* (*task)(void *), 		// function as void pointer
				int i,						// index to identify the task
				int period,					// period of task (ms)
				int drel,					// relative deadline (ms)
				int prio					// priority [0-99]
				);
int get_task_index(void* arg);				// return task index
void set_activation(int i);					// set startup time for period and deadlines
int deadline_miss(int i);					// return 1 if deadline missed
void wait_for_period(int i);				// thread sleeps untill next period bein
void wait_for_task_end(int i);				// wait untill task i terminate his execution
unsigned int task_period(int i);			// return task period

// time variables management
void time_copy(	struct timespec 	*td,
				struct timespec 	ts);
void time_add_ms(struct timespec *t, int ms);
int time_cmp(	struct timespec t1,
				struct timespec t2);
long time_dist(	struct timespec *t1, 		// return time distance in nanoseconds, negative if t2<t1
				struct timespec *t2);
int cpu_set(int cpu);
void set_period(int i, unsigned int period);

#ifdef EXTIME
void start_extime(int i, int period);
void stop_extime(int i);

#endif
	
#endif
