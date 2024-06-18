/*
 * POWER_IO.c
 *
 * Created: 5/23/2024 11:38:56 PM
 *  Author: thanh
 */ 

#include <avr/io.h>

#include "ADC.h"
#include "basetypedef.h"
#include "PDU_board.h"
#include "POWER_IO.h"
#include "PDU_board.h"
#include "PDU_INT.h"
#include "command.h"
#include  <avr/pgmspace.h>
const char NAME_TABLE[POWERIO_CHANNEL][12] PROGMEM= {
	{"TEC1_BUCK"},	//0 - 7
	{"TEC2_BUCK"},	//1 - 5
	{"TEC3_BUCK"},	//2 - 2
	{"TEC4_BUCK"},	//3 - 0 
	{"MCU_BUCK "},	//4 - 10
	{"LED_BUCK "},	//5 - 13
	{"CM4_BUCK "},	//6 - 17
	{"TEC1     "},	//7 - 6
	{"TEC2     "},	//8 - 4
	{"TEC3     "},	//9 - 3
	{"TEC4     "},	//10 - 1
	{"CPOC     "},	//11 - 11
	{"IOU      "},	//12 - 12
	{"LED_RGB  "},	//13 - 14
	{"LED_IR   "},	//14 - 15
	{"CM4      "},	//15 - 8
	{"VINPUT   "},	//16 - 9
	{"SUPERCAP "},	//17 - 16
	{"VBUS     "}	//18 - 18
};
const NAME_OF_STATUS NAME_STATUS_TABLE[5] = {
	{"Off"},
	{"Ready"},
	{"Overvoltage"},
	{"Overcurrent"},
	{"On"}
};

const uint8_t  cmd_to_channel_mapping[POWERIO_CHANNEL] = {
	7, 5, 2, 0, 10, 13, 17, 6, 4, 3, 1, 11, 12, 14, 15, 8, 9, 16,18
//  0, 1, 2, 3,  4,  5,  6, 7, 8, 9,10, 11, 12, 13, 14,15,16, 17, 18
};

const uint8_t  channel_mapping_to_cmd[POWERIO_CHANNEL] = {
	3, 10, 2, 9, 8, 1, 7, 0, 15, 16, 4, 11, 12, 5, 13, 14, 17, 16, 18
};

const uint8_t channel_mapping_to_buck[POWER_OUTPUT] = {
	7, 5, 2, 0, 10, 10, 13, 13, 17
};

const uint8_t time_mapping_to_channel[POWER_OUTPUT] = {
	6, 4, 3, 1, 11, 12, 14, 15, 8
};

const uint8_t buck_mapping_to_channel[2][NUM_OF_BUCK] = {
	{  6,   4,   3,   1, 11, 14,   8},
	{255, 255, 255, 255, 12, 15, 255}	
};

const uint8_t buck_mapping_to_time[2][NUM_OF_BUCK] = {
	{  0,   1,   2,   3, 4, 6,   8},
	{255, 255, 255, 255, 5, 7, 255}
};
const uint8_t time_mapping_to_cmd[POWER_OUTPUT] = {
	0, 1, 2, 3, 4, 4, 5, 5, 6
};
uint16_t OFF_TIME_MAXIMUM[POWER_OUTPUT] = {60000, 60000, 60000, 60000, 60000, 60000, 60000, 60000, 60000};
volatile uint16_t OFF_TIME[POWER_OUTPUT] = {1000, 2000, 3000, 4000, 6000, 7000, 5000, 6000, 7000};
e_PDU_Status PDU_Status;
channel_status CHANNEL_STATUS_TABLE[POWERIO_CHANNEL];
const uint16_t (*function_array[POWERIO_CHANNEL][2])() = {
	{TEC4_Buck_Off	,	TEC4_Buck_On},  //0
	{TEC4_Off		,	TEC4_On		},  //1
	{TEC3_Buck_Off	,	TEC3_Buck_On},  //2
	{TEC3_Off		,	TEC3_On		},  //3
	{TEC2_Off		,	TEC2_On		},  //4
	{TEC2_Buck_Off	,	TEC2_Buck_On},  //5
	{TEC1_Off		,	TEC1_On		},  //6
	{TEC1_Buck_Off	,	TEC1_Buck_On},	//7
	{CM4_Off		,	CM4_On		},  //8
	{VINHigh		,	VIN_LOSS   	},  //9
	{MCU_Buck_Off	,	MCU_Buck_On	},	//10
	{COPC_Off		,	COPC_On		},  //11
	{IOU_Off		,	IOU_On		},  //12
	{LED_Buck_Off	,	LED_Buck_On	},  //13
	{LEDRGB_Off		,	LEDRGB_On	},  //14
	{LEDIR_Off		,	LEDIR_On	},  //15
	{CAPC_Off		,	CAPC_On		},  //16 ////wtfffffff
	{CM4_Buck_Off	,	CM4_Buck_On	},  //17
	{VBUS_High		,	VBUSOK      }	//18	
};

void POWER_IO_init(){

	//CAP INIT
	HDRM_PORT			&= ~(1<<HDRM_PIN);
	CAPC_PORT			&= ~(1<<CAPC_PIN);
	HDRM_DDR			|= (1<<HDRM_PIN);
	CAPC_DDR			|= (1<<CAPC_PIN);

	//7 BUCK ENABLE PORT
	TEC1_BUCK_EN_PORT	&= ~(1<<TEC1_BUCK_EN_PIN);
	TEC2_BUCK_EN_PORT	&= ~(1<<TEC2_BUCK_EN_PIN);
	TEC3_BUCK_EN_PORT	&= ~(1<<TEC3_BUCK_EN_PIN);
	TEC4_BUCK_EN_PORT	&= ~(1<<TEC4_BUCK_EN_PIN);
	LED_BUCK_EN_PORT	&= ~(1<<LED_BUCK_EN_PIN);
	CM4_BUCK_EN_PORT	&= ~(1<<CM4_BUCK_EN_PIN);
	MCU_BUCK_EN_PORT	&= ~(1<<MCU_BUCK_EN_PIN);
	
	//10 POWER ENABLE PORT
	TEC1_EN_PORT		&= ~(1<<TEC1_EN_PIN);
	TEC2_EN_PORT		&= ~(1<<TEC2_EN_PIN);
	TEC3_EN_PORT		&= ~(1<<TEC3_EN_PIN);
	TEC4_EN_PORT		&= ~(1<<TEC4_EN_PIN);
	IOU_PWR_EN_PORT		&= ~(1<<IOU_PWR_EN_PIN);
	COPC_PWR_EN_PORT	&= ~(1<<COPC_PWR_EN_PIN);
	LEDIR_EN_PORT		&= ~(1<<LEDIR_EN_PIN);
	LEDRGB_EN_PORT		&= ~(1<<LEDRGB_EN_PIN);
	CM4_PWR_EN_PORT		&= ~(1<<CM4_PWR_EN_PIN);
//	RADIO_PWR_EN_PORT	&= ~(1<<RADIO_PWR_EN_PIN);
	
	//7 BUCK ENABLE DDR
	TEC1_BUCK_EN_DDR	|= (1<<TEC1_BUCK_EN_PIN);
	TEC2_BUCK_EN_DDR	|= (1<<TEC2_BUCK_EN_PIN);
	TEC3_BUCK_EN_DDR	|= (1<<TEC3_BUCK_EN_PIN);
	TEC4_BUCK_EN_DDR	|= (1<<TEC4_BUCK_EN_PIN);
	LED_BUCK_EN_DDR		|= (1<<LED_BUCK_EN_PIN);
	CM4_BUCK_EN_DDR		|= (1<<CM4_BUCK_EN_PIN);
	MCU_BUCK_EN_DDR		|= (1<<MCU_BUCK_EN_PIN);
		
	//10 POWER ENABLE DDR	
	TEC1_EN_DDR			|= (1<<TEC1_EN_PIN);
	TEC2_EN_DDR			|= (1<<TEC2_EN_PIN);
	TEC3_EN_DDR			|= (1<<TEC3_EN_PIN);
	TEC4_EN_DDR			|= (1<<TEC4_EN_PIN);
	IOU_PWR_EN_DDR		|= (1<<IOU_PWR_EN_PIN);
	COPC_PWR_EN_DDR		|= (1<<COPC_PWR_EN_PIN);
	LEDIR_EN_DDR		|= (1<<LEDIR_EN_PIN);
	LEDRGB_EN_DDR		|= (1<<LEDRGB_EN_PIN);
	CM4_PWR_EN_DDR		|= (1<<CM4_PWR_EN_PIN);
//	RADIO_PWR_EN_DDR	|= (1<<RADIO_PWR_EN_PIN);
	for(uint8_t i = 0 ; i < POWERIO_CHANNEL ; i++)
		CHANNEL_STATUS_TABLE[i] = OFF;
	
}

void TEC1_Buck_On()		{ TEC1_BUCK_EN_PORT |= (1<<TEC1_BUCK_EN_PIN);	}
void TEC1_Buck_Off()	{ TEC1_BUCK_EN_PORT &= ~(1<<TEC1_BUCK_EN_PIN);	}
void TEC2_Buck_On()		{ TEC2_BUCK_EN_PORT |= (1<<TEC2_BUCK_EN_PIN);	}
void TEC2_Buck_Off()	{ TEC2_BUCK_EN_PORT &= ~(1<<TEC2_BUCK_EN_PIN);	}
void TEC3_Buck_On()		{ TEC3_BUCK_EN_PORT |= (1<<TEC3_BUCK_EN_PIN);	}
void TEC3_Buck_Off()	{ TEC3_BUCK_EN_PORT &= ~(1<<TEC3_BUCK_EN_PIN);	}
void TEC4_Buck_On()		{ TEC4_BUCK_EN_PORT |= (1<<TEC4_BUCK_EN_PIN);	}
void TEC4_Buck_Off()	{ TEC4_BUCK_EN_PORT &= ~(1<<TEC4_BUCK_EN_PIN);	}
void LED_Buck_On()		{ LED_BUCK_EN_PORT |= (1<<LED_BUCK_EN_PIN);		}
void LED_Buck_Off()		{ LED_BUCK_EN_PORT &= ~(1<<LED_BUCK_EN_PIN);	}
void CM4_Buck_On()		{ CM4_BUCK_EN_PORT |= (1<<CM4_BUCK_EN_PIN);		}
void CM4_Buck_Off()		{ CM4_BUCK_EN_PORT &= ~(1<<CM4_BUCK_EN_PIN);	}
void MCU_Buck_On()		{ MCU_BUCK_EN_PORT |= (1<<MCU_BUCK_EN_PIN);		}
void MCU_Buck_Off()		{ MCU_BUCK_EN_PORT &= ~(1<<MCU_BUCK_EN_PIN);	}
						
void TEC1_On()			{ TEC1_EN_PORT |= (1<<TEC1_EN_PIN);				}
void TEC1_Off()			{ TEC1_EN_PORT &= ~(1<<TEC1_EN_PIN);			}
void TEC2_On()			{ TEC2_EN_PORT |= (1<<TEC2_EN_PIN);				}
void TEC2_Off()			{ TEC2_EN_PORT &= ~(1<<TEC2_EN_PIN);			}
void TEC3_On()			{ TEC3_EN_PORT |= (1<<TEC3_EN_PIN);				}
void TEC3_Off()			{ TEC3_EN_PORT &= ~(1<<TEC3_EN_PIN);			}
void TEC4_On()			{ TEC4_EN_PORT |= (1<<TEC4_EN_PIN);				}
void TEC4_Off()			{ TEC4_EN_PORT &= ~(1<<TEC4_EN_PIN);			}
void IOU_On()			{ IOU_PWR_EN_PORT |= (1<<IOU_PWR_EN_PIN);		}
void IOU_Off()			{ IOU_PWR_EN_PORT &= ~(1<<IOU_PWR_EN_PIN);		}
void COPC_On()			{ COPC_PWR_EN_PORT |= (1<<COPC_PWR_EN_PIN);		}
void COPC_Off()			{ COPC_PWR_EN_PORT &= ~(1<<COPC_PWR_EN_PIN);	}
void LEDIR_On()			{ LEDIR_EN_PORT |= (1<<LEDIR_EN_PIN);			}
void LEDIR_Off()		{ LEDIR_EN_PORT &= ~(1<<LEDIR_EN_PIN);			}
void LEDRGB_On()		{ LEDRGB_EN_PORT |= (1<<LEDRGB_EN_PIN);			}
void LEDRGB_Off()		{ LEDRGB_EN_PORT &= ~(1<<LEDRGB_EN_PIN);		}
//void RADIO_On()			{ RADIO_PWR_EN_PORT |= (1<<RADIO_PWR_EN_PIN);	}	//removed in ver 1.2.0
//void RADIO_Off()		{ RADIO_PWR_EN_PORT &= ~(1<<RADIO_PWR_EN_PIN);	}	//removed in ver 1.2.0
void CM4_On()			{ CM4_PWR_EN_PORT |= (1<<CM4_PWR_EN_PIN);		}
void CM4_Off()			{ CM4_PWR_EN_PORT &= ~(1<<CM4_PWR_EN_PIN);		}	

void HDRM_On()			{ HDRM_PORT |= (1<<HDRM_PIN);					}
void HDRM_Off()			{ HDRM_PORT &= ~(1<<HDRM_PIN);					}
void CAPC_On()			{ CAPC_PORT |= (1<<CAPC_PIN);					}
void CAPC_Off()			{ CAPC_PORT &= ~(1<<CAPC_PIN);					}
			
void VINHigh() { 
	usart1_send_string("VIN IS TOO HIGH\r\n");	    
//	for(uint8_t i = 0 ; i < 16 ; i++)
//		if(CHANNEL_STATUS_TABLE[cmd_to_channel_mapping[i]] != OFF)	POWER_ACTIVATE(i,0);
//	usart1_send_string("Shutting all power down... \r\n");
}	
void VIN_LOSS() { 
	if(!Get_PDU_Int()) {
		PDU_INT_En();
		usart1_send_string("LOSSING VIN\r\n");  
	}
}
void VBUSOK()			{ return;		}
void VBUS_High()		{ usart1_send_string("VBUS IS TOO HIGH\r\n");		}
	
uint8_t POWER_ACTIVATE(uint8_t channel, uint8_t state) {
	if(channel > (POWERIO_CHANNEL - 1))	return 0;
	if(state)
		CHANNEL_STATUS_TABLE[cmd_to_channel_mapping[channel]] = ON;
	else
		CHANNEL_STATUS_TABLE[cmd_to_channel_mapping[channel]] = OFF;
		
	function_array[cmd_to_channel_mapping[channel]][state]();
	if(!state)
		if(channel > NUM_OF_BUCK - 1)
			OFF_TIME[channel - NUM_OF_BUCK] = OFF_TIME_MAXIMUM[channel - NUM_OF_BUCK];
		else {
			if(buck_mapping_to_time[0][channel] != 255)
				OFF_TIME[buck_mapping_to_time[0][channel]] = OFF_TIME_MAXIMUM[buck_mapping_to_time[0][channel]];
			if(buck_mapping_to_time[1][channel] != 255)
				OFF_TIME[buck_mapping_to_time[1][channel]] = OFF_TIME_MAXIMUM[buck_mapping_to_time[1][channel]];
		}

	if(state)	state = 4;
	
	usart1_send_string("Turn ");
	usart1_send_string(NAME_STATUS_TABLE[state].pcStatus);
	usart1_send_char(' ');
	usart1_send_string_P(NAME_TABLE[channel]);
	usart1_send_string("\r\n");
	return 1;
}

void OVER_ERR_CUTOFF(uint8_t channel) {
	if(channel > (POWERIO_CHANNEL - 1))	return;
	function_array[channel][0]();
	CHANNEL_STATUS_TABLE[channel] = ADC_to_Erro_Mapping[channel];
	PDU_INT_En();
	usart1_send_string("\r\n");
	usart1_send_string(NAME_STATUS_TABLE[ADC_to_Erro_Mapping[channel]].pcStatus);
	usart1_send_string(" on line ");
	usart1_send_string_P(NAME_TABLE[channel_mapping_to_cmd[channel]]);
	usart1_send_string("\r\n");
	
}