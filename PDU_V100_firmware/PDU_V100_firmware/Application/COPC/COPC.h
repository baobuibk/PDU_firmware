/*
 * COPC.h
 *
 * Created: 5/18/2024 6:04:17 AM
 *  Author: Admin
 */ 


#ifndef COPC_H_
#define COPC_H_

#include "fsp.h"
#include "uart.h"
#include "POWER_IO.h"

typedef struct _COMMON_FRAME_
{
	uint8_t Cmd;	
}COMMON_FRAME;

typedef struct _PDU_SET_SINGLE_FRAME_
{
	uint8_t Cmd;            /* The command class */
	uint8_t Channel;		/* power source */
	uint8_t Status;			/* 1: ON, 0: OFF */
} PDU_SET_SINGLE_FRAME;

typedef struct _PDU_SET_ALL_FRAME_
{
	uint8_t Cmd;              /* The command class */
	uint8_t Status;		  /* 1: ON, 0: OFF */
} PDU_SET_ALL_FRAME;

typedef struct _PDU_GET_CHANNEL_FRAME_
{
	uint8_t Cmd;              /* The command class */
	uint8_t Channel;      /* power source */
} PDU_GET_CHANNEL_FRAME;

typedef struct _COMMON_RESPONSE_FRAME_
{
	uint8_t cmd;
} COMMON_RESPONSE_FRAME;

typedef struct _BUCK_SINGLE_STATUS_FRAME_
{
	uint8_t cmd;
	channel_status status;
	uint8_t VoltageMSB;
	uint8_t VoltageLSB;
	uint8_t CurrentMSB;
	uint8_t CurrentLSB;
} BUCK_SINGLE_STATUS_FRAME;

typedef struct _BUCK_MULTI_STATUS_FRAME_
{
	uint8_t cmd;
	channel_status status;
	uint8_t VoltageMSB;
	uint8_t VoltageLSB;
	uint8_t CurrentMSB;
	uint8_t CurrentLSB;
} BUCK_MULTI_STATUS_FRAME;

typedef struct _CHANNEL_STATUS_FRAME_
{
	uint8_t cmd;
	channel_status status;
	uint8_t VoltageMSB;
	uint8_t VoltageLSB;
	uint8_t CurrentMSB;
	uint8_t CurrentLSB;	
} CHANNEL_STATUS_FRAME;



// Union to encapsulate all frame types
typedef union _COPC_Sfp_Payload_ {
	COMMON_FRAME							commonFrame;
	PDU_SET_SINGLE_FRAME					powerSingleControlFrame;
	PDU_SET_ALL_FRAME						powerAllControlFrame;						
	PDU_GET_CHANNEL_FRAME					powerStatusReportFrame;
} COPC_Sfp_Payload_t;

typedef union _PDU_Sfp_Payload_ {
	COMMON_RESPONSE_FRAME					commonFrame;
	BUCK_SINGLE_STATUS_FRAME				powerSingleReportFrame;
	BUCK_MULTI_STATUS_FRAME					powerMultiReportFrame;		
	CHANNEL_STATUS_FRAME					channelReportFrame;
} PDU_Sfp_Payload_t;

void	COPC_init(void);
void	COPC_create_task(void);
void	COPC_Process();
void    Channel_Status(uint8_t channel);
void    Buck_Status(uint8_t Buck);
void	ACK_answer();
void	OK_Send(); 
#endif /* COPC_H_ */