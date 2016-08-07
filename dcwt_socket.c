#include <stdio.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "dcwt_socket.h"

DCWTSocket
dcwt_socket_create(DCWTSocketType type)
{
	return socket(PF_INET,
				  (type == DCWT_SOCKET_TYPE_STREAM) ? SOCK_STREAM : SOCK_DGRAM, IPPROTO_TCP);
}

int 
dcwt_socket_bind(DCWTSocket socket, const DCWTAddress *address)
{
	struct sockaddr_in sin;
	
	memset(&sin, 0, sizeof(struct sockaddr_in));
	
	sin.sin_family = AF_INET;
	
	if (NULL != address)
	{
		sin.sin_port = (address->port);
		sin.sin_addr.s_addr = (address->host);
		printf("%X:%d\n",sin.sin_addr.s_addr, sin.sin_port);
	}
	else
	{
		return -1;
	}
	
	return bind(socket, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
}

int
dcwt_socket_listen(DCWTSocket socket, int backlog)
{
	return listen(socket, backlog);
}

DCWTSocket
dcwt_socket_accept(DCWTSocket socket, DCWTAddress *address)
{
	int result ;
	struct sockaddr_in sin;
	socklen_t sinLength = sizeof(struct sockaddr_in);
	
	result = accept(socket,
			address != NULL ? (struct sockaddr *)&sin : NULL,
			address != NULL ? &sinLength : NULL);
	
	if (address != NULL)
	{
		address -> host = (uint32_t) sin.sin_addr.s_addr;
		address -> port = sin.sin_port;
	}
	
	return result;
}

int
dcwt_socket_shutdown(DCWTSocket socket, int how)
{
	return shutdown(socket, how);
}

void
dcwt_socket_destroy(DCWTSocket socket)
{
	if (socket != -1)
		close(socket);
}

int 
dcwt_socket_set_option(DCWTSocket socket, DCWTSocketOption option, int value)
{
	int result = -1;
	
	switch(option)
	{
		case DCWT_SOCKOPT_NONBLOCK:
			result = fcntl(socket, F_SETFL, (value ? O_NONBLOCK : 0) | (fcntl(socket, F_GETFL) & ~O_NONBLOCK));
			break;
		case DCWT_SOCKOPT_BROADCAST:
			result = setsockopt(socket, SOL_SOCKET, SO_BROADCAST, (char *)&value, sizeof(int));
			break;
		case DCWT_SOCKOPT_REUSEADDR:
			result = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&value, sizeof(int));
			break;
		case DCWT_SOCKOPT_RCVBUF:
			result = setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char *)&value, sizeof(int));
			break;
		case DCWT_SOCKOPT_SNDBUF:
			result = setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char *)&value, sizeof(int));
			break;
		case DCWT_SOCKOPT_RCVTIMEO:
		{
			struct timeval timeVal;
			timeVal.tv_sec = value / 1000;
			timeVal.tv_usec = (value % 1000) * 1000;
			result = setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeVal, sizeof(struct timeval));
			break;
		}
		case DCWT_SOCKOPT_SNDTIMEO:
		{
			struct timeval timeVal;
			timeVal.tv_sec = value / 1000;
			timeVal.tv_usec = (value % 1000) * 1000;
			result = setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeVal, sizeof(struct timeval));
			break;
		}
		case DCWT_SOCKOPT_NODELAY:
		{
			result = setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char *)&value, sizeof(int));
			break;
		}
		default:
			break;
		
		
	}
	
	return result == -1 ? -1 :0;
}

int 
dcwt_socket_select(DCWTSocket socket ,uint32_t * condition)
{
	struct timeval tv;
	fd_set readSet;
	int selectCount;
	
	
	
	tv.tv_sec = 1;
	tv.tv_usec = 0;
		
	FD_ZERO(&readSet);
	FD_SET(socket,&readSet);
		
	selectCount = select(socket + 1, &readSet, NULL, NULL, &tv);
		
	if (selectCount < 0)
	{
		if((EINTR == errno) && (*condition & DCWT_SOCKET_WAIT_INTERRUPT))
		{
			*condition = DCWT_SOCKET_WAIT_INTERRUPT;
			return 0;
		}
		return -1;
	}
	
	
	* condition = DCWT_SOCKET_WAIT_NONE;
	if (selectCount == 0)
	{
		return 0;
	}
	
	if (FD_ISSET(socket, &readSet))
		*condition |= DCWT_SOCKET_WAIT_RECEIVE;
	
	return 0;
}

ssize_t
dcwt_socket_recv(DCWTSocket socket , char *buffer, int bufferLength)
{
	return recv(socket, buffer, bufferLength, MSG_DONTWAIT);
}

ssize_t 
dcwt_socket_send(DCWTSocket socket, char *buffer, int bufferLength)
{
	return send(socket, buffer, bufferLength, MSG_DONTWAIT);
}
