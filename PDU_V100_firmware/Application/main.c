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
#include <util/delay.h>


int main(void)
{
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

    while (1) 
    {
		SCH_HandleScheduledTask();
		if(ADC_data.Status == DATAREADY) {
// 			for(uint8_t i = 0 ; i < 18 ; i ++)
// 				UARTprintf("%d ", ADC_data.RAW[0][i]);
// 			UARTprintf("%d \r\n", ADC_data.RAW[0][18]);
// 			_delay_ms(1000);
			ADC_data.Status = WAIT;
		}
		if(Error_Flag)	
		{
			UARTprintf("Error detected !\r\n");
			Error_Flag = 0;
		};
			
    }
}

