/*
 * PDU_INT.c
 *
 * Created: 6/15/2024 9:27:33 AM
 *  Author: thanh
 */ 

#include "PDU_board.h"
#include <avr/io.h>

void PDUINT_init() {
	PDU_INT_PORT &= ~(1<<PDU_INT_PIN);
	PDU_INT_DDR |= (1<<PDU_INT_PIN);
}

void PDU_INT_En() {
	PDU_INT_PORT |= (1<<PDU_INT_PIN);
}

void PDU_INT_Dis() {
	PDU_INT_PORT &= ~(1<<PDU_INT_PIN);
}

uint8_t Get_PDU_Int() {
	return (PDU_INT_PINR & (1<< PDU_INT_PIN))>>PDU_INT_PIN;
}