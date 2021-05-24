#define _GNU_SOURCE
#define MAX_THREAD	1001

#include "ptask.h"

#ifdef EXTIME
extern int ex_time[6];
extern int wc_extime[6];
extern struct timespec monotime_i[6], monotime_f[6];
extern int ex_cnt[6];
extern long ex_sum[6];
#endif

struct task_par{				// parametri del task
    int arg;					// task index
    long wcet;					// in microseconds
    unsigned int period;		// in milliseconds
    unsigned int deadline;		// relative (ms)
    unsigned int priority;		// in [0-99]
    unsigned int dmiss;			// # of misses
    struct timespec at;			// next activ time
    struct timespec dl;			// abs deadline
};

struct timespec t;				// time variable
struct task_par tp[MAX_THREAD];   // task parameters
pthread_t tid[MAX_THREAD];        // thread id

// Functions definitions
int task_create(void* (*task)(void *), int i, int period, int drel, int prio){
	pthread_attr_t myatt;				// thread attributes
	struct sched_param mypar;			// scheuler parameters
	int tret;							// returned from thread creation
	//setup variables
	tp[i].arg = i;
	tp[i].period = period;
	tp[i].deadline = drel;
	tp[i].priority = prio;
	tp[i].dmiss = 0;
	//setup thread
	pthread_attr_init(&myatt);			// default are: joinable, 2MB stack
	// need root ----------------- commenta per fare prove non RT
	pthread_attr_setinheritsched(&myatt, PTHREAD_EXPLICIT_SCHED);				// use another scheduler
	pthread_attr_setschedpolicy(&myatt, SCHED_FIFO);							// scheduler fifo
	// ---------------------------
	mypar.sched_priority = tp[i].priority;										// set task priority
	pthread_attr_setschedparam(&myatt, &mypar);									// setup parameters and attributes
	tret = pthread_create(&tid[i], &myatt, task, (void*)(&tp[i]));				// create thread
	return tret;
}

int get_task_index(void* arg){
	struct task_par *tp;
	tp = (struct task_par *)arg;
	return tp->arg;
}

void set_activation(int i){
	clock_gettime(CLOCK_MONOTONIC, &t);
	time_copy(&(tp[i].at), t);
	time_copy(&(tp[i].dl), t);
	time_add_ms(&(tp[i].at), tp[i].period);
	time_add_ms(&(tp[i].dl), tp[i].deadline);
}

int deadline_miss(int i){
	struct timespec now;

	clock_gettime(CLOCK_MONOTONIC, &now);
	if (time_cmp(now, tp[i].dl) > 0) {
		tp[i].dmiss++;
		return 1;
	}
	return 0;
}

void wait_for_period(int i){
	// gestione della dl miss
	clock_nanosleep(CLOCK_MONOTONIC,
					TIMER_ABSTIME, &(tp[i].at), NULL);
	time_add_ms(&(tp[i].at), tp[i].period);
	time_add_ms(&(tp[i].dl), tp[i].period);
}

void wait_for_task_end(int i){
	pthread_join(tid[i], NULL);
}

unsigned int task_period(int i){
	return tp[i].period;
}

void time_copy(struct timespec *td, struct timespec ts){
	td->tv_sec = ts.tv_sec;
	td->tv_nsec = ts.tv_nsec;
}

void time_add_ms(struct timespec *t, int ms){
	t->tv_sec += ms/1000;
	t->tv_nsec += (ms%1000)*1e+6;
	if(t->tv_nsec > 1e+9){
		t->tv_nsec -= 1e+9;
		t->tv_sec++;
	}
}

int time_cmp(struct timespec t1, struct timespec t2){
	if(t1.tv_sec > t2.tv_sec) return 1;
	if(t1.tv_sec < t2.tv_sec) return -1;
	if(t1.tv_nsec > t2.tv_nsec) return 1;
	if(t1.tv_nsec < t2.tv_nsec) return -1;
	return 0;
}

long time_dist(struct timespec *t1, struct timespec *t2) {
    long sec = t2->tv_sec - t1->tv_sec;
    long nsec = t2->tv_nsec - t1->tv_nsec;
    return (sec * 1e9 + nsec);
}

int cpu_set(int cpu){
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    return sched_setaffinity(0, sizeof(cpuset), &cpuset);
}

void set_period(int i, unsigned int period){
    tp[i].period = period;
    tp[i].deadline = period;
}

#ifdef EXTIME

void start_extime(int i, int period){
    if (ex_cnt[i] >= 1000 / period) {
        ex_time[i] = ex_sum[i] / ex_cnt[i];
		if(ex_time[i] > wc_extime[i]){
			wc_extime[i] = ex_time[i];
		}
        ex_sum[i] = 0;
        ex_cnt[i] = 0;
    }
    ex_cnt[i]++;
    clock_gettime(CLOCK_MONOTONIC, &monotime_i[i]);
}


void stop_extime(int i){
    clock_gettime(CLOCK_MONOTONIC, &monotime_f[i]);
    ex_sum[i] = ex_sum[i] + time_dist(&monotime_i[i], &monotime_f[i])/1000;
}

#endif
