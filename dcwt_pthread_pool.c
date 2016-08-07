

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dcwt_macro.h"
#include "dcwt_pthread_pool.h"


static void 
dcwt_pthread_pool_entry_init(DCWTPthreadPool * pool)
{
	TAILQ_INIT(pool);
}


