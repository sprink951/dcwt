#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dcwt_socket.h"
#include "dcwt_host.h"
#include "dcwt_protocol.h"
extern int 
dcwt_protocol_service(DCWTHost * host) ;

int
main(int argc , char * argv[])
{
	DCWTHost * host = NULL;
	DCWTAddress addr ;
	
	
	addr.host = (inet_addr("0.0.0.0"));
	addr.port = htons(4445);
	
	printf("addrhost=%X addrport=%d\n", addr.host,ntohs( addr.port));
	host = dcwt_create_host(&addr, 4, dcwt_buffer_handle);
	
	if (NULL == host)
	{
		return -1;
	}
	
	dcwt_protocol_service(host);
}