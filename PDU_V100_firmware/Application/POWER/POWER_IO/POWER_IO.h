/*
 * POWER_IO.h
 *
 * Created: 5/23/2024 11:39:10 PM
 *  Author: thanh
 */ 


#ifndef POWER_IO_H_
#define POWER_IO_H_

const uint16_t (*function_array[POWERIO_CHANNEL][2])();
extern channel_status CHANNEL_STATUS_TABLE[POWERIO_CHANNEL];
typedef struct NAME { const char *pcName; } NAME;
typedef struct NAME_OF_STATUS { const char *pcStatus; } NAME_OF_STATUS;
extern const uint8_t cmd_to_channel_mapping[POWERIO_CHANNEL];	
extern const uint8_t  channel_mapping_to_cmd[POWERIO_CHANNEL];
extern const  char NAME_TABLE[POWERIO_CHANNEL][12];
extern const NAME_OF_STATUS NAME_STATUS_TABLE[5];
extern uint8_t Error_Flag;
void POWER_IO_init();

void TEC1_Buck_On();
void TEC1_Buck_Off();
void TEC2_Buck_On();
void TEC2_Buck_Off();
void TEC3_Buck_On();
void TEC3_Buck_Off();
void TEC4_Buck_On();
void TEC4_Buck_Off();
void LED_Buck_On();	
void LED_Buck_Off();
void CM4_Buck_On();
void CM4_Buck_Off();
void MCU_Buck_On();
void MCU_Buck_Off();

void TEC1_On();
void TEC1_Off();
void TEC2_On();	
void TEC2_Off();
void TEC3_On();
void TEC3_Off();
void TEC4_On();	
void TEC4_Off();
void IOU_On();
void IOU_Off();
void COPC_On();
void COPC_Off();
void LEDIR_On();
void LEDIR_Off();
void LEDRGB_On();
void LEDRGB_Off();
void RADIO_On();
void RADIO_Off();
void CM4_On();
void CM4_Off();
void VBUS_Low();
void VBUS_High();

void HDRM_On();
void HDRM_Off();
void CAPC_On();
void CAPC_Off();

void OVER_ERR_CUTOFF(uint8_t channel);
uint8_t POWER_ACTIVATE(uint8_t channel, uint8_t state);
#endif /* POWER_IO_H_ */