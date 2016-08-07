#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dcwt_socket.h"
#include "dcwt_host.h"
#include "dcwt_protocol.h"

int 
dcwt_buffer_handle(void * arg)
{
	DCWTPeer * peer = (DCWTPeer *)arg;
	DCWTBuffer * pBuffer = NULL;
	DCWTProtocolHeader * header = NULL;
	DCWTBufferCode ret = DCWT_BUFFER_EMPTY;
	
	if (NULL == peer)
	{
		return DCWT_BUFFER_ERROR;
	}
	
	pBuffer  = (DCWTBuffer *)&peer->peerRecvBuffer;
	
	if (NULL == pBuffer)
	{
		return DCWT_BUFFER_ERROR;
	}
	
	
	do 
	{
		int packetLength = 0;
		char * packetStart = (char *)&pBuffer->buffer[pBuffer->bufferRead];
		//-------------------------------------------------------------
		//buffer�е����ݱ�Э��ͷС ,˵�����ݲ�����,
		//����memmove,�������Ƶ�bufferͷ��
		//-------------------------------------------------------------
		
		printf("%s[%d]pBuffer->bufferRead =%d\n",__func__,__LINE__,pBuffer->bufferRead);
		if (pBuffer->bufferUsed < sizeof(DCWTProtocolHeader))
		{
			ret = DCWT_BUFFER_INCOMPLETION;
		}
		else
		{
		//-------------------------------------------------------------
		//  ��ȡ����
		//-------------------------------------------------------------
			header = (DCWTProtocolHeader *)(packetStart);
			
			if (ntohl(header->headerMagic) != DCWT_MAGIC)
			{
				//-----------------------------------------------
				//���MAGIC��Ӧ����,��ƫ��Э��ͷ����,�����´�ѭ��
				//-----------------------------------------------
				pBuffer->bufferUsed -= sizeof(DCWTProtocolHeader);
				pBuffer->bufferRead += sizeof(DCWTProtocolHeader);
				ret = DCWT_BUFFER_MORE;
				printf("%s[%d] used = %d read = %d magic = %X\n",__func__
													 ,__LINE__
													 ,pBuffer->bufferUsed
													 ,pBuffer->bufferRead
													 ,ntohl(header->headerMagic));
				continue;
			}
			//-----------------------------------------------
			//����MAGIC,��������ܳ���
			//-----------------------------------------------
			packetLength = ntohl(header->headerLength) + sizeof(DCWTProtocolHeader);
			//-----------------------------------------------
			//buffer��������������ݴ������ݳ��� �����
			//		  ������������ݵ������ݳ��� ��buffer����
			//		  �������������С�����ݳ��� ��ȴ�һ�����ݰ�
			//-----------------------------------------------
			if ( packetLength < pBuffer->bufferUsed )
			{
				//enqueue(packetStart, pakcetLength)
				pBuffer->bufferUsed -= packetLength;
				pBuffer->bufferRead += packetLength;
				printf("%s[%d] used = %d read = %d\n",__func__,__LINE__,pBuffer->bufferUsed,pBuffer->bufferRead);

				ret = DCWT_BUFFER_MORE;
			}
			else if (packetLength == pBuffer->bufferUsed )
			{
				printf("%s[%d] used = %d read = %d\n",__func__,__LINE__,pBuffer->bufferUsed,pBuffer->bufferRead);
				
				ret= DCWT_BUFFER_EMPTY;
			}
			else
			{
				//TODO
				printf("%s[%d] used = %d read = %d\n",__func__,__LINE__,pBuffer->bufferUsed,pBuffer->bufferRead);
				ret = DCWT_BUFFER_INCOMPLETION;
			}
		}
	}while(ret == DCWT_BUFFER_MORE);
	
	//----------------------------------------------
	//��������
	//----------------------------------------------
	switch(ret)
	{
		case DCWT_BUFFER_EMPTY:
			memset(pBuffer->buffer, 0, pBuffer->bufferTotal);
			pBuffer->bufferUsed = 0;
			pBuffer->bufferRead = 0;
			pBuffer->bufferWrite = 0;
			break;
		case DCWT_BUFFER_INCOMPLETION:
			memmove(&pBuffer->buffer[0], &pBuffer->buffer[pBuffer->bufferRead], pBuffer->bufferUsed);
			pBuffer->bufferRead = 0;
			pBuffer->bufferWrite = pBuffer->bufferUsed;
			break;
		default:
			printf("%s[%d]\n",__func__,__LINE__);
			break;
	
	}
	
	return ret;
}

static int
dcwt_protocol_mu(void * arg)
{
	
	return 1;
}

static int 
dcwt_protocol_ap(void *arg)
{
	return 1;
}

int 
dcwt_protocol_handle(void * arg)
{
	DCWTPacket * pkt = (DCWTPacket *)arg;
	DCWTProtocolHeader * header = NULL;
	
	int ret = 0;
	
	if (NULL == pkt)
	{
		return -1;
	}
	
	if (NULL == pkt->packet)
	{
		return -2;
	}
		
	header = (DCWTProtocolHeader *)pkt->packet;
	
	if (ntohl(header->headerMagic) != DCWT_MAGIC)
	{
		return -3;
	}
	
	switch(header->headerCmd)
	{
		case DCWT_PROTOCOL_MU:
			ret = dcwt_protocol_mu(pkt->packet);
			break;
		case DCWT_PROTOCOL_AP:
			ret = dcwt_protocol_ap(pkt->packet);
			break;
		default:
			ret = -4;
			break;
	}
	
	return ret;
}



