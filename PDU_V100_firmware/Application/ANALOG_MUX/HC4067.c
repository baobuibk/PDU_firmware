/*
 * HC4067.c
 *
 * Created: 5/23/2024 1:34:00 PM
 *  Author: thanh
 */ 

#include <avr/io.h>
#include "HC4067.h"
#include "PDU_board.h"

void MUX_Init(){
	
	MUX_A0_DDR |= (1<<MUX_A0_PIN);
	MUX_A1_DDR |= (1<<MUX_A1_PIN);
	MUX_A2_DDR |= (1<<MUX_A2_PIN);
	MUX_A3_DDR |= (1<<MUX_A3_PIN);
	MUX_OUT_DDR &= ~(1<<MUX_OUT_PIN);
	
	#ifdef MUX_EN_DDR
	#waring "Using MUX_ENABLE"
		MUX_EN_DDR |= (1<<MUX_EN_PIN);
		MUX_EN_PORT &= ~(1<<MUX_EN_PIN);
	#endif
	
	MUX_A0_PORT &= ~(1<<MUX_A0_PIN);
	MUX_A1_PORT &= ~(1<<MUX_A1_PIN);
	MUX_A2_PORT &= ~(1<<MUX_A2_PIN);
	MUX_A3_PORT &= ~(1<<MUX_A3_PIN);
	MUX_OUT_PORT &= ~(1<<MUX_OUT_PIN);
}
void MUX_Set_Channel(uint8_t channel){
//	if(channel > 15)	return;
	
	#ifdef MUX_EN_PORT
		MUX_EN_PORT |= (1<<MUX_EN_PIN);
	#endif

	if(channel & 0x01)	
		MUX_A0_PORT |= (1<<MUX_A0_PIN);
	else			
		MUX_A0_PORT &= ~(1<<MUX_A0_PIN);
		
	if(channel & 0x02)
		MUX_A1_PORT |= (1<<MUX_A1_PIN);
	else
		MUX_A1_PORT &= ~(1<<MUX_A1_PIN);
		
	if(channel & 0x04)
		MUX_A2_PORT |= (1<<MUX_A2_PIN);
	else
		MUX_A2_PORT &= ~(1<<MUX_A2_PIN);
		
	if(channel & 0x08)
		MUX_A3_PORT |= (1<<MUX_A3_PIN);
	else
		MUX_A3_PORT &= ~(1<<MUX_A3_PIN);
		
	#ifdef MUX_EN_PORT
		MUX_EN_PORT &= ~(1<<MUX_EN_PIN);
	#endif

	return;					
}
