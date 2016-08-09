

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dcwt_macro.h"
#include "dcwt_pthread_pool.h"

static void
dcwt_pthread_pool_task_init(DCWTPthreadTask * task)
{
	if (task->data)
	{
		dcwt_free(task->data);
	}
	
	task->routeineCallBack = NULL;
	task->data = NULL;
}

static DCWTPthreadTask *
dcwt_pthread_pool_get_task(DCWTPthreadPool * pool)
{
	DCWTPthreadTask * task = NULL ;
	DCWTPthreadQueue * usedQueue = NULL;
	
	usedQueue = & pool->usedQueue;
	
	
	if (dcwt_pthread_mutex_lock(&usedQueue->taskMutex,"get task") < 0)
	{
		printf("%s[%d]\n",__func__,__LINE__);
		return NULL;
	}
	//已用队列为空, COND 程序挂起等待, 直到任务到来
	while (TAILQ_EMPTY(&usedQueue->taskEntry))
	{
		if(dcwt_pthread_cond_wait(&(usedQueue->taskCond), &(usedQueue->taskMutex), "get task") < 0)
		{
			if (dcwt_pthread_mutex_unlock(&usedQueue->taskMutex,"get task failed") < 0)
			{
				printf("%s[%d]\n",__func__,__LINE__);
				return NULL;
			}
			printf("%s[%d]\n",__func__,__LINE__);
			return NULL;
			
			
		}
	}
	
	if (dcwt_pthread_mutex_unlock(&usedQueue->taskMutex,"get task") < 0)
	{
		printf("%s[%d]\n",__func__,__LINE__);
		return NULL;
	}
	
	task = TAILQ_FIRST(&usedQueue->taskEntry);
	TAILQ_REMOVE(&usedQueue->taskEntry, task, taskList);
	usedQueue->taskNum -- ;
	
	return task;
}


static void *
dcwt_phtread_pool_routine(void * data)
{
	DCWTPthreadPool * pool = (DCWTPthreadPool *) data;
	DCWTPthreadQueue * freeQueue = & pool->feerQueue;
	DCWTPthreadQueue * usedQueue = & pool->usedQueue;
	DCWTPthreadTask * task = NULL;
	
	
	while(1)
	{
		task  = dcwt_pthread_pool_get_task(pool);
		if (NULL == task)
		{
			printf("%s[%d] an error has ocuured \n",__func__,__LINE__);
			break;
		}
		
		if (task->routine_cb)
		{
			task->routine_cb(task->data);
		}
		
		
		//INIT task ,put free list
		dcwt_pthread_pool_task_init(task);
		
		//Lock free task queue
		if (dcwt_pthread_mutex_lock(&freeQueue->taskMutex,"routine") < 0)
		{
			break;
		}
		
		
		TAILQ_INSERT_TAIL(&freeQueue->taskEntry, task, taskList);
		freeQueue->taskNum ++;
		
		if (freeQueue->taskNum == 1)
		{
			//TODO
			if (dcwt_pthread_cond_broadcast(&freeQueue->taskCond, "cond broadcast") < 0)
			{
				if (dcwt_pthread_mutex_unlock(&freeQueue->taskMutex, "cond failed") < 0 )
				{
					printf("%s[%d]\n",__func__,__LINE__);
				}
				break;
			}
			
		}
		
		if (dcwt_pthread_mutex_unlock(&freeQueue->taskMutex,"routine") < 0)
		{
			break;
		}
		
	}
	return NULL;
}

static int  
dcwt_pthread_pool_queue_init(DCWTPthreadQueue * queue , int queueSize)
{
	TAILQ_INIT(&queue->taskEntry);
	
	if (dcwt_pthread_mutex_create(&queue->taskMutex) < 0)
	{
		printf("%s[%d] error\n",__func__,__LINE__);
		
		return -1;
	}
	
	if (dcwt_pthread_cond_create(&queue->taskCond) < 0)
	{
		printf("%s[%d] error\n",__func__,__LINE__);
		
		return -1;
	}

	for (queue->taskNum = 0; queue->taskNum < DCWT_PTHREAD_POOL_QUEUE_SIZE; queue->taskNum++)
	{
		//插入free entry 中
		DCWTPthreadTask * task = (DCWTPthreadTask *)dcwt_malloc(sizeof(DCWTPthreadTask));
		if (NULL == task)
		{
			printf("%s[%d] error\n",__func__,__LINE__);
			dcwt_free(pool);
			return -1;
		}
		memset(task, 0, sizeof(DCWTPthreadTask));
		TAILQ_INSERT_TAIL(&queue->taskEntry, task, taskList);	
	}
	
	return 0;
}

DCWTPthreadPool * 
dcwt_pthread_pool_init(DCWTHost * host,int numOfThreads)
{
	int i = 0;
	DCWTPthreadPool * pool = NULL ;
	DCWTPthreadQueue * freeQueue = NULL;
	DCWTPthreadQueue * usedQueue = NULL;
	if ((pool = dcwt_malloc( sizeof(DCWTPthreadPool))) == NULL)
	{
		printf("%s[%d] error\n",__func__,__LINE__);
		return NULL;
	}
	
	pool->stopFlag = 0;
	pool->host = host;
	
	
	if (dcwt_pthread_pool_queue_init(& pool->freeQueue,DCWT_PTHREAD_POOL_QUEUE_SIZE) < 0)
	{
		printf("%s[%d] error\n",__func__,__LINE__);
		dcwt_free(pool);
		return NULL;
	}
	
	if (dcwt_pthread_pool_queue_init(& pool->usedQueue, 0) < 0)
	{
		printf("%s[%d] error\n",__func__,__LINE__);
		dcwt_free(pool);
		return NULL;
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
	
	return 1;
}

int dcwt_pthread_pool_add_task(DCWTPthreadPool * pool, void (* routine)(void *), void * data, int blocking)
{
	DCWTPthreadTask * task = NULL;
	DCWTPthreadQueue * freeQueue = NULL;
	DCWTPthreadQueue * usedQueue =  NULL;

	freeQueue = & pool->freeQueue;
	usedQueue = & pool->usedQueue;
	
	if (dcwt_pthread_mutex_lock(& freeQueue->taskMutex, "add task") < 0)
	{		
		printf("%s[%d] error\n",__func__,__LINE__);
		//TODO FREE POOL
		return -1;
	}
	
	while(TAILQ_EMPTY( & freeQueue->taskEntry))
	{
		if (!blocking)
		{
			if (dcwt_pthread_mutex_unlock(& freeQueue->taskMutex, "add task") < 0)
			{
				return -1;
			}
			return -2;
		}
		
		if (dcwt_pthread_cond_wait(& freeQueue->taskCond, & freeQueue->taskMutex, "add task") < 0)
		{
			if (dcwt_pthread_mutex_unlock(& freeQueue->taskMutex, "add task") < 0)
			{
				return -1;
			}
			
			return -1;
		}
			
	}
	
	task =  TAILQ_FIRST(& freeQueue->taskEntry);
	TAILQ_REMOVE(&(freeQueue->taskEntry), task, taskList);
	freeQueue->taskNum -- ;
	
	if(dcwt_pthread_mutex_unlock(&(freeQueue->taskMutex), "add task") < 0)
	{
		return -1;
	}
	
	task->routeineCallBack = routine;
	task->data = data;
	
	// ADD TASK
	if (dcwt_pthread_mutex_lock(& usedQueue->taskMutex, "add task") < 0)
	{		
		printf("%s[%d] error\n",__func__,__LINE__);
		//TODO FREE POOL
		return -1;
	}
	
	TAILQ_INSERT_TAIL(&usedQueue->taskEntry, task, taskList);
	usedQueue->taskNum ++;
	
	if (1 == usedQueue->taskNum)
	{
		if (dcwt_pthread_cond_broadcast(& usedQueue->taskCond,"add task") < 0)
		{			
			if (dcwt_pthread_mutex_unlock(& usedQueue->taskMutex, "add task") < 0)
			{		
				printf("%s[%d] error\n",__func__,__LINE__);
				//TODO FREE POOL
				return -1;
			}
		}
	}
	
	if (dcwt_pthread_mutex_unlock(& usedQueue->taskMutex, "add task") < 0)
	{		
		printf("%s[%d] error\n",__func__,__LINE__);
		//TODO FREE POOL
		return -1;
	}
	
	return 0;
}

void dcwt_pthread_pool_free(DCWTPthreadPool * pool,int blocking)
{
	//TODO
	
}