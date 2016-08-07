#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "dcwt_socket.h"
#include "dcwt_host.h"



static void * 
dcwt_process(void * arg)
{
	int32_t error = 0;
	uint32_t waitCondition = 0;
	int recvLength = 0;
	uint32_t isCancelled = 0;
	DCWTPeer * peer = (DCWTPeer *) arg;
	DCWTBuffer * pRecvBuf = & peer->peerRecvBuffer;

	if (NULL == peer || NULL == pRecvBuf)
	{
		printf("%s[%d]\n",__func__,__LINE__);
		return NULL;
	}
	
	if (peer->peerSocket <= 0)
	{
		printf("%s[%d]\n",__func__,__LINE__);
		return NULL;
	}
#if 1
	if (dcwt_socket_set_option(peer->peerSocket, DCWT_SOCKOPT_NONBLOCK, 1) < 0)
	{
		printf("%s[%d]\n",__func__,__LINE__);
		return NULL;
	}
#endif
	printf ("start recv from %X:%d\n",ntohl(peer->peerAddress.host),ntohs(peer->peerAddress.port));
	while(!isCancelled)
	{
		//char buffer[1500]={0};
		if (dcwt_socket_select(peer->peerSocket, &waitCondition) < 0)
		{
			isCancelled = 1;
			error = -1;
			continue;
		}
		
		if (waitCondition & DCWT_SOCKET_WAIT_RECEIVE)
		{
			if (pRecvBuf->bufferTotal <= pRecvBuf->bufferUsed)
			{
				printf("recv buffer full\n");
				if (peer->peerBufferHandle)
					peer->peerBufferHandle(peer);
				
			}
			recvLength = recv(peer->peerSocket,&pRecvBuf->buffer[pRecvBuf->bufferWrite],pRecvBuf->bufferTotal - pRecvBuf->bufferUsed,0);
			if (recvLength < 0)
			{
				if ((EAGAIN == errno) || (EWOULDBLOCK == errno) || (EINTR == errno))
				{
					continue;
				}
				else
				{
					printf("%s[%d] %s\n",__func__,__LINE__,strerror(errno));
					isCancelled = 1;
					error = -2; 
					continue;
				}
			}
			else if ( recvLength == 0)
			{
				//printf("%s[%d]\n",__func__,__LINE__);
				isCancelled = 1;
				error = -3; 
				continue;
			}
			else
			{
				 //buffer 添加
				pRecvBuf->bufferUsed += recvLength;
				pRecvBuf->bufferWrite += recvLength;
				if (peer->peerBufferHandle)
					peer->peerBufferHandle(peer); 
				
				 
			}
		}
	}

	dcwt_free_peer(peer);
	printf("%s[%d]\n",__func__,__LINE__);
	return NULL;
}

int 
dcwt_protocol_service(DCWTHost * host) 
{
	int isServiceOn = 1;


	if (NULL == host)
	{
		printf("%s[%d]\n",__func__,__LINE__);
		return -1;
	}

	host->hostSocket = dcwt_socket_create(DCWT_SOCKET_TYPE_STREAM);
	if (host->hostSocket < 0)
	{
		printf("%s[%d] socket error\n",__func__,__LINE__);
		return -2;
	}

	if (dcwt_socket_set_option(host->hostSocket, DCWT_SOCKOPT_REUSEADDR, 1) < 0)
	{
		printf("%s[%d]\n",__func__,__LINE__);
		return -3;
	}

	if (dcwt_socket_bind(host->hostSocket, &host->hostAddress) < 0)
	{
		printf("%s[%d]\n",__func__,__LINE__);
		return -4;
	}
	
	if (dcwt_socket_listen(host->hostSocket , 1024) < 0)
	{
		printf("%s[%d]\n",__func__,__LINE__);
		return -5;
	}


	while(isServiceOn)
	{
		DCWTSocket socket = -1;
		DCWTAddress clientAddr ;
		DCWTPeer * peer = NULL;
		int error = 0;	

		
		if ((socket =  dcwt_socket_accept(host->hostSocket, &clientAddr)) < 0)
		{
			printf("%s[%d] %s\n",__func__,__LINE__,strerror(errno));
			return -5;
		}
		peer = dcwt_get_peer(host);
		if (NULL == peer)
		{
			printf("%s[%d] no peer\n",__func__,__LINE__);	
			return -6;
		}
		(peer)->peerSocket = socket;
		memcpy(&(peer)->peerAddress, &clientAddr, sizeof(DCWTAddress));
		printf("%s[%d] %X:%d   connectd\n",__func__,__LINE__,clientAddr.host, ntohs(clientAddr.port));
		error = pthread_create(&(peer)->pid, NULL,  dcwt_process,peer);
		
	}
}
