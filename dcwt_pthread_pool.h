#ifndef __DCWT_PTHREAD_POOL_H__
#define __DCWT_PTHREAD_POOL_H__
#include <stdint.h>
#include <sys/queue.h>
#include "dcwt_macro.h"
#incldue "dcwt_pthread_mutex.h"



typedef struct _DCWTPthreadTask
{
	TAILQ_ENTRY(,_DCWTPthreadTask) taskList;
	void (* routeineCallBack) (void *);
	void * data
}DCWTPthreadTask;

typedef struct _DCWTPthreadPool
{
	TAILQ_HEAD(,_DCWTPthreadTask) taskUsedEntry;
	TAILQ_HEAD(,_DCWTPthreadTask) taskFreeEntry;
	
	DCWTHost * host;
	//DCWTPthreadTask tasks[DCWT_PTHREAD_POOL_QUEUE_SIZE];
	uint16_t threadsNum;
	volatile uint16_t stopFlag;
	
	pthread_t * threadsPID;
	dcwt_pthread_mutex_t taskFreeMutex;
	dcwt_pthread_mutex_t taskUsedMutex;
	dcwt_pthread_cond_t taskFreeCond;
	dcwt_pthread_cond_t taskUsedMutex;
}DCWTPhtreadPool;

#endif