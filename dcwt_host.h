#ifndef __DCWT_SERVER_H__
#define __DCWT_SERVER_H__
#include <sys/queue.h>
#include <pthread.h>
#include "dcwt_macro.h"
#include "dcwt_socket.h"

typedef int (* peerCallBack)(void *);

threadpool_queue_init

typedef struct _DCWTHost
{
	DCWTSocket hostSocket;
	DCWTAddress hostAddress;

	int peerUsedCount;
	TAILQ_HEAD(,_DCWTPeer) peerUsedEntry;
	int peerFreeCount;
	TAILQ_HEAD(,_DCWTPeer) peerFreeEntry;
	
	int pakcetUsedCount;
	
	int packetFreeCount;
	
}DCWT_PACKED  DCWTHost;

typedef struct _DCWTBuffer
{
	uint32_t bufferTotal;
	uint32_t bufferUsed;
	uint32_t bufferRead;
	uint32_t bufferWrite;
	char *buffer;
}DCWT_PACKED DCWTBuffer;



typedef struct _DCWTPeer
{
	TAILQ_ENTRY(_DCWTPeer) peerList;
	
	pthread_t pid;	
	DCWTHost * host;
	DCWTSocket peerSocket;
	DCWTAddress peerAddress;
	DCWTBuffer peerRecvBuffer;
	peerCallBack  peerBufferHandle;
} DCWTPeer;



DCWT_API DCWTPeer *
dcwt_get_peer(DCWTHost * host);

DCWT_API DCWTHost * 
dcwt_create_host(DCWTAddress *hostAddress, 
		int peerCount,
		peerCallBack func);
		
DCWT_API void
dcwt_free_host(DCWTHost * host);

DCWT_API void 
dcwt_free_peer(DCWTPeer * peer);
#endif
