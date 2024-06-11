/*
 * command.c
 *
 * Created: 5/19/2024 8:37:53 PM
 *  Author: Admin
 */ 
#include "scheduler.h"
#include "command.h"
#include "uart.h"
#include "ADC.h"
#include <stdlib.h>
#include "cmdline.h"
#include "POWER_IO.h"
#include <avr/pgmspace.h>

/* Private typedef -----------------------------------------------------------*/
typedef struct _Command_TaskContextTypedef_
{
	SCH_TASK_HANDLE               taskHandle;
	SCH_TaskPropertyTypedef       taskProperty;
} Command_TaskContextTypedef;


//typedef enum{OK = 0, CMDLINE_BAD_CMD, CMDLINE_TOO_MANY_ARGS, CMDLINE_TOO_FEW_ARGS, CMDLINE_INVALID_ARG} command_error_code_t;
const char * ErrorCode[5] = {"OK\r\n", "CMDLINE_BAD_CMD\r\n", "CMDLINE_TOO_MANY_ARGS\r\n",
"CMDLINE_TOO_FEW_ARGS\r\n", "CMDLINE_INVALID_ARG\r\n" };

static	void	command_task_update(void);
tCmdLineEntry g_psCmdTable[] = { 
	{ "show_all", show_data," : Show all channel parameters " },
	{ "help", Cmd_help," : Display list of commands, format: help" },								
	{"set_channel", Cmd_set_channel , " : Set channel N Off/On, format: set_channel N 0/1 (N < 10)", },
	{"set_buck", Cmd_set_buck , " : Set buck N Off/On, format: set_buck N 0/1 (N < 7)", },
	{"set_all", Cmd_set_all, " : Turn Off/On all channel, format: set_all 0/1"},
//	{"", Cmd_TEC_disable, " : disable TEC channel, format: tec_dis channel"}	,
//	{"tec_dis_auto", Cmd_TEC_disable_auto_control, " : disable auto control, format tec_dis_auto channel"},
//	{"tec_ena_auto", Cmd_set_all_auto_control, " :  enable auto control,  format: tec_ena_auto channel"},
//	{"tec_set_output", Cmd_TEC_set_output, " :  set the output manually,  format: tec_set_output channel heat_cool(0:COOL, 1: HEAT) voltage(150 mean 1.5)"},
	{0,0,0}
};

volatile static	ringbuffer_t *p_CommandRingBuffer;
static	char s_commandBuffer[COMMAND_MAX_LENGTH];
static uint8_t	s_commandBufferIndex = 0;


static Command_TaskContextTypedef           s_CommandTaskContext =
{
	SCH_INVALID_TASK_HANDLE,                 // Will be updated by Schedular
	{
		SCH_TASK_SYNC,                      // taskType;
		SCH_TASK_PRIO_0,                    // taskPriority;
		10,                                // taskPeriodInMS;
		command_task_update                // taskFunction;
	}
};

void	command_init(void)
{
	usart1_init();
	
	p_CommandRingBuffer = uart_get_uart1_rx_buffer_address();
	memset((void *)s_commandBuffer, 0, sizeof(s_commandBuffer));
	s_commandBufferIndex = 0;
	usart1_send_string("PDU FIRMWARE V1.0.0 \r\n");
	usart1_send_string(">");
	command_send_splash();
}

static	void	command_task_update(void)
{
	char rxData;
	int8_t	ret_val;
	while (! rbuffer_empty(p_CommandRingBuffer))
	{
		rxData = rbuffer_remove(p_CommandRingBuffer);
		usart1_send_char(rxData);
		if ((rxData == '\r') || (rxData == '\n'))		//got a return or new line
		{
			if (s_commandBufferIndex > 0)		//if we got the CR or LF at the begining, discard	
			{
				s_commandBuffer[s_commandBufferIndex] = 0;
				s_commandBufferIndex++;
				ret_val = CmdLineProcess(s_commandBuffer);		
				s_commandBufferIndex = 0;		
				usart1_send_string(ErrorCode[ret_val]);
				usart1_send_string("> ");
			}	
			else usart1_send_string("\r\n> ");
		}
		else if ((rxData == 8) || (rxData == 127))	
		{
			if (s_commandBufferIndex > 0) s_commandBufferIndex--;
		}
		else
		{
			s_commandBuffer[s_commandBufferIndex] = rxData;
			s_commandBufferIndex ++;
			if (s_commandBufferIndex > COMMAND_MAX_LENGTH) s_commandBufferIndex= 0;
		}
	}	
}


int Cmd_help(int argc, char *argv[]) {
	tCmdLineEntry *pEntry;

	usart1_send_string("\nAvailable commands\r\n");
	usart1_send_string("------------------\r\n");

	// Point at the beginning of the command table.
	pEntry = &g_psCmdTable[0];

	// Enter a loop to read each entry from the command table.  The
	// end of the table has been reached when the command name is NULL.
	while (pEntry->pcCmd) {
		// Print the command name and the brief description.
		usart1_send_string(pEntry->pcCmd);
		usart1_send_string(pEntry->pcHelp);
		usart1_send_string("\r\n");

		// Advance to the next entry in the table.
		pEntry++;

	}
	// Return success.
	return (CMDLINE_OK);
}
//*****************************************************************************
//
// Format: show_data
//
//*****************************************************************************
float tmp = 0;
float scale = 0;
int
show_data(int argc) 
{
	if (argc > 1) return CMDLINE_TOO_MANY_ARGS;
//	char tmp_vol_char[10];
	char tmp_cur_char[10];
	usart1_send_string("\r\n");
	for(uint8_t i = 0 ; i < POWERIO_CHANNEL ; i++)
	{
		usart1_send_string_P(NAME_TABLE[i]);
		usart1_send_string(" : ");
		usart1_send_string(NAME_STATUS_TABLE[CHANNEL_STATUS_TABLE[cmd_to_channel_mapping[i]]].pcStatus);
//		sprintf(tmp_vol_char," %2.1f",ADC_data.RAW[0][1]);
		tmp = (float)(ADC_data.RAW[0][cmd_to_channel_mapping[i]]);
		scale = SCALE_A;
		if(i < 6) 
			tmp *= SCALE_5V;
		else if(i == 6)	
			tmp *= SCALE_12V;
		else if(i < 17)	
			tmp *= SCALE_A;
		else if(i == 17)	
			tmp *= 1;
		else if(i == 18)
			tmp *= SCALE_12V;
		if(i > 6 && i < 17)	tmp_cur_char[7] = 'A';
		else				tmp_cur_char[7] = 'V';
		sprintf(tmp_cur_char," %2.2f",tmp);
		usart1_send_string(tmp_cur_char);
		usart1_send_char(tmp_cur_char[7]);
//		UARTprintf(" V: %f, I: %f",i,1);
		usart1_send_string("\r\n");
	}
	return CMDLINE_OK;
}
//*****************************************************************************
//
// Format: TEC_dis channel 
//
//*****************************************************************************
int
Cmd_TEC_disable(int argc, char *argv[])
{
		  if (argc < 2) return CMDLINE_TOO_FEW_ARGS;
		  if (argc > 2) return CMDLINE_TOO_MANY_ARGS;
		  uint8_t channel = atoi(argv[1]);
		  if (channel > 4)	return CMDLINE_INVALID_ARG;
//		  temperature_disable_TEC(channel);
		  return CMDLINE_OK;
}


//*****************************************************************************
//
// Format: TEC_dis_control channel 
//
//*****************************************************************************
int
Cmd_TEC_disable_auto_control(int argc, char *argv[])
{
		  if (argc < 2) return CMDLINE_TOO_FEW_ARGS;
		  if (argc >2) return CMDLINE_TOO_MANY_ARGS;
		  uint8_t channel = atoi(argv[1]);
		  if (channel > 4)	return CMDLINE_INVALID_ARG;
//		  temperature_disable_auto_control_TEC(channel);
		  return CMDLINE_OK;
}

//*****************************************************************************
//
// Format: set_channel N 0/1
// Set channel N OFF/ON
//
//*****************************************************************************
int Cmd_set_channel(int argc, char *argv[])
{
	if (argc < 3) return CMDLINE_TOO_FEW_ARGS;
	if (argc > 3) return CMDLINE_TOO_MANY_ARGS;
	uint8_t channel = atoi(argv[1]);
	uint8_t state = atoi(argv[2]);
	if (channel > NUM_OF_CHANNEL - 1)	return CMDLINE_INVALID_ARG;
	if(state > 1)					return CMDLINE_INVALID_ARG;
	POWER_ACTIVATE(channel + NUM_OF_BUCK,state);
	return CMDLINE_OK;
}

//*****************************************************************************
//
// Format: set_buck N 0/1
// Set channel N OFF/ON
//
//*****************************************************************************
int Cmd_set_buck(int argc, char *argv[])
{
	if (argc < 3) return CMDLINE_TOO_FEW_ARGS;
	if (argc > 3) return CMDLINE_TOO_MANY_ARGS;
	uint8_t channel = atoi(argv[1]);
	uint8_t state = atoi(argv[2]);
	if (channel > NUM_OF_BUCK - 1)		return CMDLINE_INVALID_ARG;
	if(state > 1)					return CMDLINE_INVALID_ARG;
	POWER_ACTIVATE(channel,state);
	return CMDLINE_OK;
}

//*****************************************************************************
//
// Format: set_all N 0/1
// Set all N OFF/ON
//
//*****************************************************************************
int Cmd_set_all(int argc, char *argv[])
{
	if (argc < 2) return CMDLINE_TOO_FEW_ARGS;
	if (argc > 2) return CMDLINE_TOO_MANY_ARGS;
	uint8_t state = atoi(argv[1]);
	if(state > 1)					return CMDLINE_INVALID_ARG;
	for(uint8_t i = 0 ; i < (NUM_OF_BUCK + NUM_OF_CHANNEL) ; i++)
		POWER_ACTIVATE(i,state);
	return CMDLINE_OK;
}
void	command_create_task(void)
{
	SCH_TASK_CreateTask(&s_CommandTaskContext.taskHandle, &s_CommandTaskContext.taskProperty);
};


const char SPLASH[][65] PROGMEM = {
	{"\r\n"},
	{".......................................................\r\n"},
	{".......................................................\r\n"},
	{"..     ____                       _     _ _          ..\r\n"},
	{"..    / ___| _ __   __ _  ___ ___| |   (_|_)_ ___    ..\r\n"},
	{"..    \\___ \\| '_ \\ / _` |/ __/ _ \\ |   | | | '_  \\   ..\r\n"},
	{"..     ___) | |_) | (_| | (_|  __/ |___| | | | | |   ..\r\n"},
	{"..    |____/| .__/ \\__,_|\\___\\___|_____|_|_|_| |_|   ..\r\n"},
	{"..          |_|    ____  ____  _   _                 ..\r\n"},
	{"..                |  _ \\|  _ \\| | | |                ..\r\n"},
	{"..                | |_) | | | | | | |                ..\r\n"},
	{"..                |  __/| |_| | |_| |                ..\r\n"},
	{"..                |_|   |____/ \\___/                 ..\r\n"},
	{"..              __     ___   ___   ___               ..\r\n"},
	{"..              \\ \\   / / | / _ \\ / _ \\              ..\r\n"},
	{"..               \\ \\ / /| || | | | | | |             ..\r\n"},
	{"..                \\ V / | || |_| | |_| |             ..\r\n"},
	{"..                 \\_/  |_(_)___(_)___/              ..\r\n"},
	{"..                                                   ..\r\n"},
	{".......................................................\r\n"},
	{".......................................................\r\n"},
};
	
void	command_send_splash(void)
{
	for(uint8_t i = 0 ; i < 21 ; i++) {
		usart1_send_string_P(&SPLASH[i][0]);
	}
	usart1_send_char('>');
}
