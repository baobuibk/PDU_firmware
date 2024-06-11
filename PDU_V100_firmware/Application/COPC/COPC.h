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
#define		PDU_ADDRESS	2

typedef struct _COMMON_FRAME_
{
	uint8_t Cmd;	
}COMMON_FRAME;
typedef struct _PDU_SINGLE_POWER_CONTROL_FRAME_
{
	uint8_t Cmd;              /* The command class */
	uint8_t PowerSource : 7;                   /* power source */
	uint8_t Status		:1;						/* 1: ON, 0: OFF */
} PDU_SINGLE_POWER_CONTROL_FRAME;

typedef struct _PDU_SINGLE_POWER_STATUS_REQUEST_FRAME_
{
	uint8_t Cmd;              /* The command class */
	uint8_t PowerSource;                   /* power source */

} PDU_SINGLE_POWER_STATUS_REQUEST_FRAME;

typedef struct _PDU_SINGLE_POWER_STATUS_REPORT_FRAME_
{
	uint8_t		Cmd;								/* The command class */
	uint8_t		PowerSource : 7;                   /* power source */
	uint8_t		Status		:1;						/* 1: ON, 0: OFF */
	uint16_t	Current_Val;
}	PDU_SINGLE_POWER_STATUS_REPORT_FRAME;

// Union to encapsulate all frame types

typedef union _COPC_Sfp_Payload_ {
	COMMON_FRAME							commonFrame;
	PDU_SINGLE_POWER_CONTROL_FRAME			powerControlFrame;
	PDU_SINGLE_POWER_STATUS_REQUEST_FRAME	powerStatusRequestFrame;
	PDU_SINGLE_POWER_STATUS_REPORT_FRAME	powerStatusReportFrame;
} COPC_Sfp_Payload_t;
void	COPC_init(void);
void	COPC_create_task(void);

#endif /* COPC_H_ */