#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dcwt_socket.h"
#include "dcwt_host.h"
//==========================================
//	free 链表中取出一个peer,放到used 链表中
//==========================================
DCWTPeer *
dcwt_get_peer(DCWTHost * host)
{
	DCWTPeer * peer = NULL;
	
	// Get From  Free List	
	peer = TAILQ_FIRST(& host->peerFreeEntry);
	
	if (NULL == peer)
	{
		return NULL;
	}
	

	TAILQ_REMOVE(&host->peerFreeEntry, peer, peerList);
	host->peerFreeCount --;
	// Set To Used List;
	TAILQ_INSERT_TAIL(&host->peerUsedEntry, peer, peerList);	
	host->peerUsedCount ++;
	printf("0x%lX %p\n",(unsigned long)(peer),peer);
	return peer;
}
//==========================================
//	peer连接断开之后,初始化之后,
//	从used 链表中取出,放到free 链表中,
//  等待下一次调用
//==========================================
void 
dcwt_free_peer(DCWTPeer * peer)
{
	DCWTHost * host = peer->host;
	DCWTPacket * packet = NULL;
	DCWTPacket * next = NULL;
	
	TAILQ_REMOVE(&host->peerUsedEntry, peer, peerList);
	host->peerUsedCount--;
	
	TAILQ_INSERT_TAIL(&host->peerFreeEntry, peer, peerList);	
	host->peerFreeCount++;
	
	if(peer->peerSocket > 0)
	{
		close(peer->peerSocket);
		peer->peerSocket = -1;
	}
	
	if (peer->peerRecvBuffer.buffer)
	{
		dcwt_free (peer->peerRecvBuffer.buffer);
		peer->peerRecvBuffer.bufferTotal = 0;
		peer->peerRecvBuffer.bufferUsed = 0;
		peer->peerRecvBuffer.bufferRead = 0;
		peer->peerRecvBuffer.bufferWrite = 0;
	}
	
	TAILQ_FOREACH_SAFE(packet, &peer->peerPacketEntry, packetList, next)
	{
		TAILQ_REMOVE(&peer->peerPacketEntry, packet, packetList);
		dcwt_free(packet);
	}
	
}

DCWTHost * 
dcwt_create_host(DCWTAddress *hostAddress, 
		int peerCount,
		peerCallBack func)
{
	int i = 0;
		
	DCWTHost * host = NULL;
	DCWTPeer * peer = NULL;
	//Create Host

	host = (DCWTHost *) dcwt_malloc(sizeof(DCWTHost));
	if (NULL == host)
	{
		return NULL;
	}
	host->hostAddress.host = hostAddress->host;
	host->hostAddress.port = hostAddress->port;
	
	TAILQ_INIT(& host->peerUsedEntry);
	TAILQ_INIT(& host->peerFreeEntry);
	

	//Create Peer
	for (i = 0;i < peerCount; i++)
	{
		peer = (DCWTPeer *) dcwt_malloc(sizeof(DCWTPeer));
		if (NULL == peer)
		{
			printf("%s[%d] peer create failed\n",__func__,__LINE__);
			return NULL;
		}
		printf("0x%lX %p\n",(unsigned long)(peer),peer);
		//Set Peer To Free TAILQ
		memset(peer, 0, sizeof(DCWTPeer));

		peer->host = host;
		peer->peerRecvBuffer.buffer = (char *)dcwt_malloc(DCWT_RECV_LEN);
		if (peer->peerRecvBuffer.buffer == NULL)
		{
			printf("%s[%d]\n",__func__,__LINE__);
			return NULL;
		}
		
		peer->peerRecvBuffer.bufferTotal = DCWT_RECV_LEN;
		peer->peerRecvBuffer.bufferUsed = 0;
		peer->peerRecvBuffer.bufferRead = 0;
		peer->peerRecvBuffer.bufferWrite = 0;
		peer->peerBufferHandle = func;
		TAILQ_INSERT_TAIL(&host->peerFreeEntry, peer, peerList);
		
		
		peer = NULL;
	}
	host->peerUsedCount = 0;
	host->peerFreeCount = peerCount;
	return host;
}



void
dcwt_free_host(DCWTHost * host)
{
	DCWTPeer * peer = NULL;
	DCWTPeer * next = NULL;
	TAILQ_FOREACH_SAFE(peer, &host->peerFreeEntry, peerList, next)
	{
		TAILQ_REMOVE(&host->peerFreeEntry, peer, peerList);
		dcwt_free(peer)	;
	}

	TAILQ_FOREACH_SAFE(peer, &host->peerUsedEntry, peerList, next)
	{
		TAILQ_REMOVE(&host->peerUsedEntry, peer, peerList);
		dcwt_free(peer);
	}

	dcwt_free(host);
}



