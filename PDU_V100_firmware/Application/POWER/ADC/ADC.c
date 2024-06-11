/*
 * ADC.c
 *
 * Created: 5/23/2024 2:20:12 PM
 *  Author: thanh
 *	NOTE: OVERVOLTAGE -> Turn OFF BUCK + OUT
		: OVERCURRENT -> Turn OFF OUT
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "ADC.h"
#include "HC4067.h"
#include "POWER_IO.h"

const channel_status ADC_to_Erro_Mapping[ANALOG_CHANNEL] = {
	OVERVOLTAGE,		//Ch0 - TEC4_VSEN
	OVERCURRENT,		//Ch1 - TEC4_ISEN
	OVERVOLTAGE,		//Ch2 - TEC3_VSEN
	OVERCURRENT,		//Ch3 - TEC3_ISEN
	OVERCURRENT,		//Ch4 - TEC2_ISEN
	OVERVOLTAGE,		//Ch5 - TEC2_VSEN
	OVERCURRENT,		//Ch6 - TEC1_ISEN
	OVERVOLTAGE,		//Ch7 - TEC1_VSEN
	OVERCURRENT,		//Ch8 - CM4_ISEN
	OVERCURRENT,		//Ch9 - RADIO_ISEN
	OVERVOLTAGE,		//Ch10 - MCU_VSEN
	OVERCURRENT,		//Ch11 - COPC_ISEN
	OVERCURRENT,		//Ch12 - IOU_ISEN
	OVERVOLTAGE,		//Ch13 - LED_VSEN
	OVERCURRENT,		//Ch14 - LEDRGB_ISEN
	OVERCURRENT,		//Ch15 - LEDIR_ISEN
	OVERVOLTAGE,		//Ch16 - CAP_VSEN
	OVERVOLTAGE,		//Ch17 - CM4_VSEN
	OVERVOLTAGE			//Ch18 - VBUS_VSEN
};

volatile ADC_block	ADC_data;

uint16_t get_raw(channel_name channel) {
	if(channel > ANALOG_CHANNEL)	return 0xFFFF;
	uint16_t tmp;
	ADC_Interrupt_Dis();
		tmp = ADC_data.RAW[0][channel];
	ADC_Interrupt_En();
	return tmp;
}

void write_threshold(uint8_t channel, uint16_t value) {
	if(channel > ANALOG_CHANNEL) return;
	ADC_data.RAW[1][channel] = value;
}

void ADC_init(void) {
	cli();
	ADCSRA = 0x87;
	ADMUX = 0x03;
	ADC_Interrupt_En();
	ADC_data.Status = WAIT;
	for(uint8_t i = 0 ; i < ANALOG_CHANNEL ; i++)
		ADC_data.RAW_Status[i] = IDLE;
	write_threshold(0,ADC_5V);
	write_threshold(1,ADC_2A5);
	write_threshold(2,ADC_5V);
	write_threshold(3,ADC_2A5);
	write_threshold(4,ADC_2A5);		
	write_threshold(5,ADC_5V);
	write_threshold(6,ADC_2A5);
	write_threshold(7,ADC_5V);
	write_threshold(8,ADC_1A);
	write_threshold(9,ADC_1A);
	write_threshold(10,ADC_5V);
	write_threshold(11,ADC_1A);
	write_threshold(12,ADC_1A);
	write_threshold(13,ADC_5V);
	write_threshold(14,ADC_5V);
	write_threshold(15,ADC_1A);
	write_threshold(16,800);
	
	write_threshold(17,ADC_12V);
	write_threshold(18,ADC_VBUS);
	MUX_Init();
	sei();
}	

inline void ADC_Interrupt_En(void) {
	ADCSRA |= (1<<ADIE);
}

inline void ADC_Interrupt_Dis(void) {
	ADCSRA &= ~(1<<ADIE);
}

inline void ADC_Start_Convert(void) {
	ADCSRA |= (1 < ADSC);
}
uint8_t kk;
ISR(ADC_vect)
{
	uint8_t adc_temp = ADC_data.Channel_to_read;
	if(ADC_data.RAW[1][adc_temp] < ADCW) {	//OVERLOAD, OVERVOLTAGE, OVERCURRENT {
//		ADC_data.RAW_Status[adc_temp] = ADC_to_Erro_Mapping[adc_temp];
		ADC_data.Error_Counter[adc_temp]++;
		if(ADC_data.Error_Counter[adc_temp] > 10)
		{
			OVER_ERR_CUTOFF(adc_temp);
			ADC_data.Error_Counter[adc_temp] = 0;
		}
	} else {
		ADC_data.RAW_Status[adc_temp] = READY;
		ADC_data.Error_Counter[adc_temp] = 0;
	}
	ADC_data.RAW[0][adc_temp] = ADCW;
	
	adc_temp++;
	kk = adc_temp;
	if(adc_temp > (ANALOG_CHANNEL - 1)) {
		ADMUX = 0x03;
		MUX_Set_Channel(0);
		adc_temp = 0;
		ADC_data.Status = DATAREADY;
	} else {
		if(adc_temp < 16) {
			MUX_Set_Channel(adc_temp);
			ADMUX = 0x03;
		}
		else if(adc_temp == 16)	
			ADMUX = 0x00;
		else if(adc_temp == 17)
			ADMUX = 0x01;	
		else if(adc_temp == 18)
			ADMUX = 0x02;	
		
		ADC_data.Status = WAIT;
	}

	ADC_data.Channel_to_read = adc_temp;
	ADC_data.Wait = MUX_WAIT_FOR_STABLE;
}