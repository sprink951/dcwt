#ifndef __DCWT_PROTOCOL_H__
#define __DCWT_PROTOCOL_H__
#include <stdint.h>
#include "dcwt_macro.h"

typedef enum _DCWTBufferCode
{
	DCWT_BUFFER_ERROR = -1,
	DCWT_BUFFER_EMPTY,
	DCWT_BUFFER_INCOMPLETION,
	DCWT_BUFFER_MORE,
}DCWTBufferCode;

typedef enum _DCWTProtocolCmd
{
	DCWT_PROTOCOL_MU = 0x84,
	DCWT_PROTOCOL_AP = 0x8E,
}DCWTProtocolCmd;


typedef struct _DCWTProtocolHeader
{
	uint32_t headerMagic;
	uint32_t headerSeq;
	uint8_t  headerCmd;
	uint32_t headerLength;
}DCWT_PACKED DCWTProtocolHeader;

typedef struct _DCWTProtocolMU
{
	DCWTProtocolHeader muHeader;
	
	uint8_t muMac[DCWT_ADDR_LEN];
	uint8_t muDst[DCWT_ADDR_LEN];
	uint8_t muBssid[DCWT_ADDR_LEN];
	
	uint8_t muChannel;
	int8_t  muRssi;
	int8_t  muRate;
	uint8_t muType;
	uint8_t muSubtype;
	uint8_t muEncry;
	uint8_t muCrc;
	uint8_t muSsidLen;
	uint8_t muSsid[DCWT_SSID_MAX];
	uint8_t muInfoFlag;
	uint8_t muInfo[1];
}DCWT_PACKED DCWTProtocolMU;

typedef struct _DCWTProtocolAP
{
	DCWTProtocolHeader	apHeader;
	
	uint8_t apMac[DCWT_ADDR_LEN];
	uint8_t apDst[DCWT_ADDR_LEN];
	uint8_t apBssid[DCWT_ADDR_LEN];
	
	uint8_t apChannel;
	int8_t  apRssi;
	int8_t  apRate;
	uint8_t apEncry;
	uint8_t apCrc;
	uint8_t apSsidLen;
	uint8_t apSsid[1];
}DCWT_PACKED DCWTProtocolAP;


DCWT_API int 
dcwt_buffer_handle(void * arg);

DCWT_API int 
dcwt_protocol_handle(void * arg);
#endif