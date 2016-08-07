#ifndef __DCWT_SOCKET_H__
#define __DCWT_SOCKET_H__
#include <sys/types.h>
#include <stdint.h>
#include "dcwt_macro.h"



typedef int DCWTSocket;

typedef enum _DCWTSocketType
{
   DCWT_SOCKET_TYPE_STREAM   = 1,
   DCWT_SOCKET_TYPE_DATAGRAM = 2
} DCWTSocketType;

typedef enum _DCWTSocketOption
{
	DCWT_SOCKOPT_NONBLOCK,
	DCWT_SOCKOPT_BROADCAST,
	DCWT_SOCKOPT_REUSEADDR,
	DCWT_SOCKOPT_RCVBUF,
	DCWT_SOCKOPT_SNDBUF,
	DCWT_SOCKOPT_RCVTIMEO,
	DCWT_SOCKOPT_SNDTIMEO,
	DCWT_SOCKOPT_NODELAY,
	DCWT_SOCKOPT_ERROR,
}DCWTSocketOption;

typedef enum _DCWTSocketWait
{
   DCWT_SOCKET_WAIT_NONE      = 0,
   DCWT_SOCKET_WAIT_SEND      = (1 << 0),
   DCWT_SOCKET_WAIT_RECEIVE   = (1 << 1),
   DCWT_SOCKET_WAIT_INTERRUPT = (1 << 2)
} DCWTSocketWait;

typedef struct _DCWTAddress
{
   uint32_t host;
   uint16_t port;
} DCWTAddress;


DCWT_API DCWTSocket
dcwt_socket_create(DCWTSocketType type);

DCWT_API int 
dcwt_socket_bind(DCWTSocket socket, const DCWTAddress *address);

DCWT_API int
dcwt_socket_listen(DCWTSocket socket, int backlog);

DCWT_API DCWTSocket
dcwt_socket_accept(DCWTSocket socket, DCWTAddress *address);

DCWT_API int
dcwt_socket_shutdown(DCWTSocket socket, int how);

DCWT_API void
dcwt_socket_destroy(DCWTSocket socket);

DCWT_API int 
dcwt_socket_set_option(DCWTSocket socket, DCWTSocketOption option, int Value);

DCWT_API int 
dcwt_socket_select(DCWTSocket socket ,uint32_t * condition);

DCWT_API ssize_t 
dcwt_socket_recv(DCWTSocket socket, char *buffer, int bufferLength);

DCWT_API ssize_t 
dcwt_socket_send(DCWTSocket socket, char *buffer, int bufferLength);

#endif
