

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dcwt_macro.h"
#include "dcwt_pthread_pool.h"



static DCWTPthreadTask *
dcwt_pthread_pool_get_task(DCWTPhtreadPool * pool)
{
	DCWTPthreadTask * task = NULL ;
	
	if (dcwt_pthread_mutex_lock(&pool->taskUsedMutex,"get task") < 0)
	{
		printf("%s[%d]\n",__func__,__LINE__);
		return NULL;
	}
	//已用队列为空
	if (TAILQ_EMPTY(&pool->taskUsedEntry))
	{
		if(dcwt_pthread_cond_wait(&(pool->taskUsedCond), &(pool->taskUsedMutex), "get task") < 0)
		{
			if (dcwt_pthread_mutex_unlock(&pool->taskUsedMutex,"get task failed") < 0)
			{
				printf("%s[%d]\n",__func__,__LINE__);
				return NULL;
			}
			printf("%s[%d]\n",__func__,__LINE__);
			return NULL;
			
			
		}
	}
	
	if (dcwt_pthread_mutex_unlock(&pool->taskUsedMutex,"get task") < 0)
	{
		printf("%s[%d]\n",__func__,__LINE__);
		return NULL;
	}
	
	TAILQ_REMOVE(&pool->taskUsedEntry, task, taskList);
	
	return task;
	
}


static void *
dcwt_phtread_pool_routine(void * data)
{
	DCWTPthreadPool * pool = (DCWTPthreadPool *) data;
	DCWTPthreadTask * task = NULL;
	
	while(1)
	{
		//TODO
	}
	return NULL;
}

DCWTPthreadPool * 
dcwt_pthread_pool_init(DCWTHost * host,int numOfThreads)
{
	int i = 0;
	DCWTPthreadPool * pool = NULL ;
	
	if ((pool = dcwt_malloc( sizeof(DCWTPthreadPool))) == NULL)
	{
		printf("%s[%d] error\n",__func__,__LINE__);
		return NULL;
	}
	
	pool->stopFlag = 0;
	pool->host = host;
	if(dcwt_pthread_mutex_create(&pool->taskFreeMutex) < 0)
	{
		printf("%s[%d] error\n",__func__,__LINE__);
		dcwt_free(pool);
		return NULL;
	}
	
	if (dcwt_pthread_mutex_create(&pool->taskUsedMutex) < 0)
	{
		printf("%s[%d] error\n",__func__,__LINE__);
		dcwt_free(pool);
		return NULL;
	}
	
	if (dcwt_pthread_cond_create(&pool->taskFreeCond) < 0)
	{
		printf("%s[%d] error\n",__func__,__LINE__);
		dcwt_free(pool);
		return NULL;
	}
	
	if (dcwt_pthread_cond_create(&pool->taskUsedCond) < 0)
	{
		printf("%s[%d] error\n",__func__,__LINE__);
		dcwt_free(pool);
		return NULL;
	}
	
	TAILQ_INIT(pool->taskUsedEntry);
	TAILQ_INIT(pool->taskFreeEntry);
	
	for (i = 0; i < DCWT_PTHREAD_POOL_QUEUE_SIZE; i++)
	{
		//插入free entry 中
		DCWTPthreadTask * task = (DCWTPthreadTask *)dcwt_malloc(sizeof(DCWTPthreadTask));
		if (NULL == task)
		{
			printf("%s[%d] error\n",__func__,__LINE__);
			dcwt_free(pool);
			return NULL;
		}
		memset(task, 0, sizeof(DCWTPthreadTask));
		TAILQ_INSERT_TAIL(&pool->taskFreeEntry, task, taskList);	
	}
	
	if ((pool->threadsPID = dcwt_malloc(sizeof(pthread_t) * numOfThreads)) == NULL)
	{
		printf("%s[%d] error\n",__func__,__LINE__);
		dcwt_free(pool);
		return NULL;
	}
	
	for (pool->threadsNum; pool->threadsNum <  numOfThreads; (pool->threadsNum)++)
	{
		if (pthread_create(&(pool->threadsPID[pool->threadsNum]),NULL, xxxxxxxx,pool))
		{
			printf("%s[%d] error\n",__func__,__LINE__);
			//TODO FREE POOL
			return NULL;
		}
	}
	
	
}


