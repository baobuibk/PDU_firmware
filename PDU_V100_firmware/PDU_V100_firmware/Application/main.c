/*
 * PDU_V100_firmware.c
 *
 * Created: 5/22/2024 12:40:54 PM
 * Author : thanh
 */ 

#include <avr/io.h>
#include "AD.h"
#include "status_led.h"
#include "uart.h"
#include "scheduler.h"
#include "COPC.h"
#include "command.h"
#include "ADC.h"
#include "POWER_IO.h"
#include "PDU_INT.h"
#include <util/delay.h>

float kkkk = 0;
int main(void)
{
	uint8_t error_flag = 0;
	status_led_init();
	usart0_init();
	
//	usart1_init();
	SCH_Initialize();
	status_led_create_task();
	COPC_init();
	COPC_create_task();
	command_init();
	command_create_task();	
	WDT_init();
	SCH_StartSchedular();
	ADC_init();

	POWER_IO_init();
	PDUINT_init();
	status_led_set_status(RUNNING); 
	PDU_Status = RUNNING;
	
	kkkk = SCALE_A;
	while (1) 
    {
//		Get_All_Status();
		SCH_HandleScheduledTask();
		for(uint8_t i = 0; i < POWER_OUTPUT; i++) {
			if(OFF_TIME[i] == 0)	{
				if(CHANNEL_STATUS_TABLE[channel_mapping_to_buck[i]] == OFF)		POWER_ACTIVATE(time_mapping_to_cmd[i],1);
				if(CHANNEL_STATUS_TABLE[cmd_to_channel_mapping[i + NUM_OF_BUCK]] == OFF)				POWER_ACTIVATE(i + NUM_OF_BUCK, 1);
				
				OFF_TIME[i] = 0xFFFF;
			}
		}
		if(ADC_data.Status == DATAREADY) {
			error_flag = 0;
 			for(uint8_t i = 0 ; i < POWERIO_CHANNEL ; i ++)
				 if(CHANNEL_STATUS_TABLE[i] == OVERCURRENT || CHANNEL_STATUS_TABLE[i] == OVERVOLTAGE)	 error_flag = 1;
				 if(error_flag)	{
					 PDU_Status = OVERVALUE;
					 status_led_set_status(OVERVALUE);
				 }
				 else {
					 PDU_Status = RUNNING;
					 status_led_set_status(RUNNING);
				 }
			ADC_data.Status = WAIT;
		}

    }
}

