#ifndef __DCWT_PTHREAD_MUTEX_H__
#define __DCWT_PTHREAD_MUTEX_H__
#include <pthread.h>
#include "dcwt_macro.h"

typedef pthread_mutex_t  dcwt_pthread_mutex_t;
typedef pthread_cond_t  dcwt_pthread_cond_t;

DCWT_API int 
dcwt_pthread_mutex_create(dcwt_pthread_mutex_t * mtx, const char * desc);

DCWT_API int
dcwt_pthread_mutex_destroy(dcwt_pthread_mutex_t * mtx, const char * desc);

DCWT_API int 
dcwt_pthread_mutex_lock(dcwt_pthread_mutex_t *mtx,const char * desc);

DCWT_API int 
dcwt_pthread_mutex_unlock(dcwt_pthread_mutex_t *mtx,const char * desc);

DCWT_API int 
dcwt_pthread_cond_create(dcwt_pthread_cond_t *cond,const char * desc);

DCWTAPI int
dcwt_pthread_cond_destroy(dcwt_pthread_cond_t *cond,const char *desc);

DCWTAPI int
dcwt_pthread_cond_signal(dcwt_pthread_cond_t *cond,const char *desc);

DCWTAPI int 
dcwt_pthread_cond_broadcast(dcwt_pthread_cond_t *cond,const char *desc);

DCWTAPI int 
dcwt_pthread_cond_wait(dcwt_pthread_cond_t *cond,dcwt_pthread_mutex_t *mtx, const char *desc)


#endif