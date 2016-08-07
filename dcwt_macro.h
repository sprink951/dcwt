#ifndef __DCWT_MACRO_H__
#define __DCWT_MACRO_H__
#include <stdlib.h>
#define DCWT_PACKED __attribute__ ((packed))
#define DCWT_API	extern
#define DCWT_DEBUG printf
#define DCWT_RECV_LEN (1500 * 20)

#define DCWT_ADDR_LEN (6)
#define DCWT_SSID_MAX (33)

#define DCWT_MAGIC (0x1337835)

#define DCWT_PTHREAD_POOL_QUEUE_SIZE (5000)

#define dcwt_malloc malloc
#define dcwt_free	free


#ifndef TAILQ_FOREACH_SAFE
#define TAILQ_FOREACH_SAFE(var, head, field, tvar)                      \
        for ((var) = TAILQ_FIRST((head));                               \
            (var) && ((tvar) = TAILQ_NEXT((var), field), 1);            \
            (var) = (tvar)) 
#endif
#endif