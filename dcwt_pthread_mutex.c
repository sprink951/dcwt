#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


#include "dcwt_pthread_mutex.h"

//==================================PTHREAD MUTEX=======================================
//
//======================================================================================
int dcwt_pthread_mutex_create(dcwt_pthread_mutex_t *mtx,const char *desc)
{
	int err;
	pthread_mutexattr_t attr;
	
	
	err =  pthread_mutexattr_init(&attr);
	if (err != 0)
	{
		printf("%s error(%d) %s failed\n",desc,err,"pthread_mutexattr_init");
		return -1;
	}
	
	err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
	if (err != 0)
	{
		printf("%s error(%d) %s failed\n",desc,err,"pthread_mutexattr_settype");
		return -1;
	}
		
	err = pthread_mutex_init(mtx,&attr);
	if (err != 0)
	{
		printf("%s error(%d) %s failed\n",desc,err,"pthread_mutex_init");
		return -1;
	}
	
	err =  pthread_mutexattr_destroy(&attr);
	if (err != 0)
	{
		printf("%s error(%d) %s failed\n",desc,err,"pthread_mutexattr_destroy");
		return -1;
	}
	
	 return 0;
}

int dcwt_pthread_mutex_destroy(dcwt_pthread_mutex_t *mtx,const char * desc)
{
	int err;
	err = pthread_mutex_destroy(mtx);
	if (err != 0)
	{
		printf("%s error(%d) %s falid\n",desc,err,__func__);
		return -1;
	}
	
	return 0;
}

int dcwt_pthread_mutex_lock(dcwt_pthread_mutex_t *mtx,const char * desc)
{
	int err;
	err = pthread_mutex_lock(mtx);
	if (err == 0)
	{
		return 0;
	}
	
	printf("%s error(%d) %s falid\n",desc,err,__func__);

	return -1;
}

int dcwt_pthread_mutex_unlock(dcwt_pthread_mutex_t *mtx,const char * desc)
{
	int err;
	
	err = pthread_mutex_unlock(mtx);
	if (err == 0)
	{
		return 0;
	}
	
	printf("%s error(%d) %s falid\n",desc,err,__func__);

	return -1;
}

//==================================PTHREAD COND========================================
//
//======================================================================================
int dcwt_pthread_cond_create(dcwt_pthread_cond_t *cond,const char * desc)
{
	int err;
	
	err = pthread_cond_init(cond,NULL);
	if (err == 0)
	{
		return 0;
	}
	printf("%s error(%d) %s falid\n",desc,err,__func__);
	
	return -1;
}

int dcwt_pthread_cond_destroy(dcwt_pthread_cond_t *cond,const char *desc)
{
	int err ;
	
	err = pthread_cond_destroy(cond);
	if (err == 0)
	{
		return 0;
	}
		
	printf("%s error(%d) %s falid\n",desc,err,__func__);
	return -1;
}

int dcwt_pthread_cond_signal(dcwt_pthread_cond_t *cond,const char *desc)
{
	int err;
	
	err = pthread_cond_signal(cond);
	if (err == 0)
	{
		return 0;
	}
	
	printf("%s error(%d) %s falid\n",desc,err,__func__);
	return -1;
}

int dcwt_pthread_cond_broadcast(dcwt_pthread_cond_t *cond,const char *desc)
{
	int err;
	
	err = pthread_cond_broadcast(cond);
	if (err == 0)
	{
		return 0;
	}
	
	printf("%s error(%d) %s falid\n",desc,err,__func__);
	return -1;
}

int dcwt_pthread_cond_wait(dcwt_pthread_cond_t *cond,dcwt_pthread_mutex_t *mtx, const char *desc)
{
	int err;
	
	err = pthread_cond_wait(cond,mtx);
	
	if (err == 0)
	{
		return 0;
	}
	
	printf("%s error(%d) %s falid\n",desc,err,__func__);

	return -1;
	
}


