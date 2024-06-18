/*
 * ADC.h
 *
 * Created: 5/23/2024 2:21:24 PM
 *  Author: thanh
 */ 


#ifndef ADC_H_
#define ADC_H_
#include "PDU_board.h"
#include "basetypedef.h"

	typedef enum strADC_state	{
		IDLE = 0,			//READY TO RUN
		RUN,
		WAIT,			//WAIT FOR TRIGGED OR SOMETHING ELSE		IDLE - WAIT - RUN - DONE - DATAREADY
		OK,
		DONE,			//FOR EACH CHANNEL
		DATAREADY,		//FOR ALL CHANNEL
	} strADC_state;
	typedef enum channel_name {
		TEC1 = 0,
		TEC2,
		TEC3,
		TEC4,
		IOU5V,
		COPC5V,
		LEDIR5V,
		LEDRGB5V,
		RADIO12V,
		CM412V
	} channel_name;
	typedef struct ADC_block {
		strADC_state	Status;
		channel_status	RAW_Status[ANALOG_CHANNEL];
		uint8_t			Channel_to_read;
		uint8_t			Wait;
		uint8_t			Error_Counter[ANALOG_CHANNEL];
		uint16_t		RAW[2][ANALOG_CHANNEL];
		
	} ADC_block;
	
	extern const channel_status ADC_to_Erro_Mapping[ANALOG_CHANNEL];
	extern volatile ADC_block ADC_data;

	uint16_t get_raw(channel_name channel);
	void write_threshold(uint8_t channel, uint16_t value);
	void ADC_init(void);
	void ADC_Interrupt_En(void);
	void ADC_Interrupt_Dis(void);
	void ADC_Start_Convert(void);
#endif /* ADC_H_ */