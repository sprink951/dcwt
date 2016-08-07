#ifndef __DCWT_PTHREAD_POOL_H__
#define __DCWT_PTHREAD_POOL_H__
#include <stdint.h>
#include <sys/queue.h>
#include <pthread.h>
#include "dcwt_macro.h"

typedef struct _DCWTPthreadTask
{
	void (* routeineCallBack) (void *);
	void * data
}DCWTPthreadTask;


typedef struct _DCWTPthreadPool
{
	TAILQ_HEAD(,_DCWTPacket) taskUsedEntry;
	TAILQ_HEAD(,_DCWTPacket) taskFreeEntry;
	
	DCWTPthreadTask tasks[DCWT_PTHREAD_POOL_QUEUE_SIZE];
	uint16_t threadsNum;
	volatile uint16_t stopFlag;
	
	pthrad_t * thr_arr;
	pthread_mutex_t taskFreeMutex;
	pthread_mutex_t taskUsedMutex;
	pthread_cond_t taskFreeCond;
	pthread_cond_t taskUsedMutex;
	
	
}DCWTPhtreadPool;

#endif