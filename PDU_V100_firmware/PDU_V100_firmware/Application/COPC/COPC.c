/*
 * COPC.c
 *
 * Created: 5/18/2024 6:29:56 AM
 *  Author: Admin
 */ 
#include <avr/pgmspace.h>
#include "scheduler.h"
#include "COPC.h"
#include "fsp.h"
#include "POWER_IO.h"
#include "PDU_board.h"
#include "command.h"
#include "ADC.h"
#include "PDU_INT.h"
#include <avr/interrupt.h>
static	void COPC_task_update(void);
volatile static	ringbuffer_t *p_COPCBuffer;
static	fsp_packet_t	s_COPC_FspPacket;
static	fsp_packet_t	s_PDU_FspPacket;
static  PDU_Sfp_Payload_t   *s_pPDU_Sfp_Payload;
static  COPC_Sfp_Payload_t	*s_pCOPC_Sfp_Payload;

volatile uint8_t swap_byte = 0;
typedef struct COPC_TaskContextTypedef
{
	SCH_TASK_HANDLE               taskHandle;
	SCH_TaskPropertyTypedef       taskProperty;
} COPC_TaskContextTypedef;

static COPC_TaskContextTypedef           s_COPC_task_context =
{
	SCH_INVALID_TASK_HANDLE,                 // Will be updated by Schedular
	{
		SCH_TASK_SYNC,                      // taskType;
		SCH_TASK_PRIO_0,                    // taskPriority;
		10,                                // taskPeriodInMS, call the task to check buffer every 10ms
										//with baudrate of 9600, buffer size can be less than 10 bytes
		COPC_task_update                // taskFunction;
	}
};


void	COPC_init(void)
{	
	usart0_init();
	fsp_init(FSP_ADR_PDU);
	p_COPCBuffer = uart_get_uart0_rx_buffer_address();
	s_pCOPC_Sfp_Payload = (COPC_Sfp_Payload_t *)(&s_COPC_FspPacket.payload);
	s_pPDU_Sfp_Payload = (PDU_Sfp_Payload_t *)(&s_PDU_FspPacket.payload);
}

static void COPC_task_update(void)
{
	
	char rxData;
	while (! rbuffer_empty(p_COPCBuffer))
	{
		rxData = rbuffer_remove(p_COPCBuffer);
		if(rxData == FSP_PKT_ESC)	{
			swap_byte = 1;
			break;
		}
		if(swap_byte) {
			swap_byte = 0;
			if(rxData == FSP_PKT_TSOD)	rxData = FSP_PKT_SOD;
			if(rxData == FSP_PKT_TESC)	rxData = FSP_PKT_ESC;
			if(rxData == FSP_PKT_TEOF)	rxData = FSP_PKT_EOF;
		}
		switch (fsp_decode(rxData,&s_COPC_FspPacket))
		{
			//process command
            case FSP_PKT_NOT_READY:
				break;
            case FSP_PKT_READY:
				COPC_Process();
	            break;
            case FSP_PKT_INVALID:
				
	            break;
            case FSP_PKT_WRONG_ADR:
				usart1_send_string("Wrong module adr \r\n");					
	            break;
            case FSP_PKT_ERROR:
				
	            break;
            default:

	           break;
		}
	}
}

void	ACK_answer() {
	uint8_t  encoded_frame[10];
	uint8_t  frame_len;
	fsp_gen_ack_pkt(FSP_ADR_COPC,&s_PDU_FspPacket);
	fsp_encode(&s_PDU_FspPacket,  encoded_frame,  &frame_len);
	usart0_send_array(&encoded_frame,frame_len);
}

void	COPC_create_task(void)
{
	COPC_init();
	SCH_TASK_CreateTask(&s_COPC_task_context.taskHandle, &s_COPC_task_context.taskProperty);
}

void Channel_Status(uint8_t channel) {
	uint8_t  frame_len = 0;
	uint8_t  payload[20];
	memset((void*)payload,0,sizeof(payload));
	float	volt_tmp;
	float	amp_tmp;
	uint16_t vl_tmp;

	cli();
	amp_tmp = (float)(ADC_data.RAW[0][time_mapping_to_channel[channel]]);
	volt_tmp = (float)(ADC_data.RAW[0][channel_mapping_to_buck[channel]]);
	sei();
	
	if(channel < 8)
		volt_tmp *= SCALE_5V;
	else 
		volt_tmp *= SCALE_12V;
		
	amp_tmp *= SCALE_A;
	payload[frame_len++] =  channel;
	payload[frame_len++] =  CHANNEL_STATUS_TABLE[cmd_to_channel_mapping[channel]];
	vl_tmp = (uint16_t)(100*volt_tmp);
	payload[frame_len++] = (uint8_t)(vl_tmp >> 8);
	payload[frame_len++] = (uint8_t)(vl_tmp);
	
	vl_tmp = (uint16_t)(100*amp_tmp);
	payload[frame_len++] = (uint8_t)(vl_tmp >> 8);
	payload[frame_len++] = (uint8_t)(vl_tmp);	
	fsp_gen_cmd_w_data_pkt(s_pCOPC_Sfp_Payload->commonFrame.Cmd, payload, frame_len, FSP_ADR_COPC, FSP_PKT_WITHOUT_ACK, &s_PDU_FspPacket);
	fsp_encode(&s_PDU_FspPacket,  payload,  &frame_len);
	usart0_send_array(&payload,frame_len);
/*
	uint8_t  frame_len = 6;
	float	 volt_tmp;
	float	 amp_tmp;
	uint16_t vl_tmp;
	uint8_t  payload[20];
	s_pCOPC_Sfp_Payload->commonFrame.Cmd = 0xAD;
	cli();
	amp_tmp = (float)(ADC_data.RAW[0][cmd_to_channel_mapping[channel]]);
	volt_tmp = (float)(ADC_data.RAW[0][channel_mapping_to_buck[channel]]);
	sei();	
	if(channel < 8)
	volt_tmp *= SCALE_5V;
	else
	volt_tmp *= SCALE_12V;
	
	amp_tmp *= SCALE_A;	
	vl_tmp = 100 * (uint16_t)(volt_tmp);
	s_pPDU_Sfp_Payload->channelReportFrame.VoltageMSB = (uint8_t)(vl_tmp >> 8);
	s_pPDU_Sfp_Payload->channelReportFrame.VoltageLSB = (uint8_t)(vl_tmp);
	
	vl_tmp = 100 * (uint16_t)(amp_tmp);
	s_pPDU_Sfp_Payload->channelReportFrame.CurrentMSB = (uint8_t)(vl_tmp >> 8);
	s_pPDU_Sfp_Payload->channelReportFrame.CurrentLSB = (uint8_t)(vl_tmp);
		
	s_pPDU_Sfp_Payload->channelReportFrame.cmd = s_pCOPC_Sfp_Payload->commonFrame.Cmd;
	s_pPDU_Sfp_Payload->channelReportFrame.status = CHANNEL_STATUS_TABLE[channel];
	s_PDU_FspPacket.sod        = FSP_PKT_SOD;
	s_PDU_FspPacket.src_adr    = fsp_my_adr;
	s_PDU_FspPacket.dst_adr    = FSP_ADR_COPC;
	s_PDU_FspPacket.length     = frame_len;
	s_PDU_FspPacket.type       = FSP_PKT_TYPE_DATA;
	s_PDU_FspPacket.eof		   = FSP_PKT_EOF;
	s_PDU_FspPacket.crc16	   = crc16_CCITT(FSP_CRC16_INITIAL_VALUE, &s_PDU_FspPacket.src_adr, s_PDU_FspPacket.length + 4);
	fsp_encode(&s_PDU_FspPacket,  payload,  &frame_len);
	//usart0_send_array(payload,frame_len);
	usart1_send_array(payload,frame_len);
	*/
}


void Buck_Status(uint8_t Buck) {
	float	volt_tmp = 0;
	float	amp_tmp = 0;
	uint16_t vl_tmp = 0;
	uint8_t  frame_len = 0;
	uint8_t  payload[20];
	memset((void*)payload,0,sizeof(payload));

	cli();
	volt_tmp = (float)(ADC_data.RAW[0][cmd_to_channel_mapping[Buck]]);
	amp_tmp = (float)(ADC_data.RAW[0][buck_mapping_to_channel[0][Buck]]);
	if(buck_mapping_to_channel[1][Buck] != 255)	
	amp_tmp += (float)(ADC_data.RAW[0][buck_mapping_to_channel[1][Buck]]);
	sei();
	
	if(Buck < 6)
		volt_tmp *= SCALE_5V;
	else
		volt_tmp *= SCALE_12V;
	
	amp_tmp *= SCALE_A;
	payload[frame_len++] =  Buck;
	payload[frame_len++] =  CHANNEL_STATUS_TABLE[cmd_to_channel_mapping[Buck]];
	vl_tmp =  (uint16_t)(100 *volt_tmp);
	payload[frame_len++] = (uint8_t)(vl_tmp >> 8);
	payload[frame_len++] = (uint8_t)(vl_tmp);
	
	vl_tmp = (uint16_t)(100 *amp_tmp);
	payload[frame_len++] = (uint8_t)(vl_tmp >> 8);
	payload[frame_len++] = (uint8_t)(vl_tmp);
	fsp_gen_cmd_w_data_pkt(s_pCOPC_Sfp_Payload->commonFrame.Cmd, payload, frame_len, FSP_ADR_COPC, FSP_PKT_WITHOUT_ACK, &s_PDU_FspPacket);
	fsp_encode(&s_PDU_FspPacket,  payload,  &frame_len);
	usart0_send_array(&payload,frame_len);
}

void OK_Send() {
	uint8_t  frame_len = 0;
	uint8_t  payload[15];
	memset((void* ) payload,0,sizeof(payload));
	payload[0] = s_pCOPC_Sfp_Payload->commonFrame.Cmd;
	fsp_gen_cmd_w_data_pkt(FSP_CMD_RESPONSE_DONE,  payload,  1, FSP_ADR_COPC, FSP_PKT_WITHOUT_ACK,  &s_PDU_FspPacket);
	fsp_encode(&s_PDU_FspPacket,  payload,  &frame_len);
	usart0_send_array(payload,frame_len);	
}

void Get_All_Status() {
	uint8_t  frame_len = 0;
	uint8_t  payload[70];
	memset((void*)payload,0,sizeof(payload));
	uint16_t	ADC_tmp[ANALOG_CHANNEL];
	float	flo_tmp = 0;
	uint16_t vl_tmp = 0;
	
	cli();
	for(uint8_t i = 0 ; i < ANALOG_CHANNEL ; i++)
		ADC_tmp[i] = ADC_data.RAW[0][i];
	sei();
	for(uint8_t i = 0 ; i < POWERIO_CHANNEL ; i++)
	{
		if(i == 17)
			continue;
		payload[frame_len++] = CHANNEL_STATUS_TABLE[cmd_to_channel_mapping[i]];
		flo_tmp = (float)(ADC_tmp[cmd_to_channel_mapping[i]]);
		if(i < 6)
			flo_tmp *= SCALE_5V;
		else if(i == 6)
			flo_tmp *= SCALE_12V;
		else if(i == 16)
			flo_tmp *= SCALE_12V;
		else if(i < 17 && i != 16)
			flo_tmp *= SCALE_A;
		else if(i == 17)
			break;
		else if(i == 18)
			flo_tmp *= SCALE_12V;
		vl_tmp =  (uint16_t)(100 *flo_tmp);

		payload[frame_len++] = (uint8_t)(vl_tmp >> 8);
		payload[frame_len++] = (uint8_t)(vl_tmp);
	}
	fsp_gen_cmd_w_data_pkt(s_pCOPC_Sfp_Payload->commonFrame.Cmd, payload, frame_len, FSP_ADR_COPC, FSP_PKT_WITHOUT_ACK, &s_PDU_FspPacket);
	fsp_encode(&s_PDU_FspPacket,  payload,  &frame_len);
	usart0_send_array(&payload,frame_len);	
}

void COPC_Process() {
	uint8_t channel;
	uint8_t status;
	
//	status = s_COPC_FspPacket.type;
//	if((status == FSP_PKT_TYPE_ACK) || (status == FSP_PKT_TYPE_DATA_WITH_ACK) || (status == FSP_PKT_TYPE_CMD_WITH_ACK) || (status == FSP_PKT_TYPE_CMD_W_DATA_ACK))
//		ACK_answer();
	switch (s_pCOPC_Sfp_Payload->commonFrame.Cmd)
	{
		case	FSP_CMD_PDU_SET_CHANNEL:
			usart1_send_string("Received set channel command\r\n");
			channel = s_pCOPC_Sfp_Payload->powerSingleControlFrame.Channel;
			status = s_pCOPC_Sfp_Payload->powerSingleControlFrame.Status;
			if (channel > NUM_OF_CHANNEL - 1)	break;
			if(status > 1)					break;
			POWER_ACTIVATE(channel + NUM_OF_BUCK, status);
			OK_Send();
		break;
		
		case	FSP_CMD_PDU_SET_BUCK:
			usart1_send_string("Received set buck command\r\n");
			channel = s_pCOPC_Sfp_Payload->powerSingleControlFrame.Channel;
			status = s_pCOPC_Sfp_Payload->powerSingleControlFrame.Status;
			if (channel > NUM_OF_BUCK - 1)	break;
			if(status > 1)					break;
			POWER_ACTIVATE(channel, status);
			OK_Send();
		break;
		
		case	FSP_CMD_PDU_SET_ALL:
			usart1_send_string("Received set all command\r\n");
			status = s_pCOPC_Sfp_Payload->powerAllControlFrame.Status;
			if(status > 1)					break;
			for(uint8_t i = 0 ; i < (NUM_OF_BUCK + NUM_OF_CHANNEL) ; i++)
			POWER_ACTIVATE(i,status);
			OK_Send();
		break;
		
		case	FSP_CMD_PDU_GET_CHANNEL:
			usart1_send_string("Received get channel command\r\n");
			channel = s_pCOPC_Sfp_Payload->powerStatusReportFrame.Channel;
			if (channel > NUM_OF_CHANNEL - 1)	break;
			Channel_Status(channel);
		break;
		
		case	FSP_CMD_PDU_GET_BUCK:
			usart1_send_string("Received get buck command\r\n");
			channel = s_pCOPC_Sfp_Payload->powerStatusReportFrame.Channel;
			if (channel > NUM_OF_BUCK - 1)	break;
			Buck_Status(channel);
		break;
		
		case	FSP_CMD_PDU_GET_ALL:
			usart1_send_string("Received get all command\r\n");
			PDU_INT_Dis();
			Get_All_Status();
		break;
	}
}