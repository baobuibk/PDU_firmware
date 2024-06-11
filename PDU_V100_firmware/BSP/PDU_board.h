/*
 * PDU_board.h
 *
 * Created: 5/15/2024 11:17:18 AM
 *  Author: Admin
 */ 

#ifndef PDU_BOARD_H_
#define PDU_BOARD_H_

#define F_CPU 8000000UL

#define	PDU_HW_V1_0_0

#ifdef	PDU_HW_V1_0_0
/**************LED**************/
#define LED1_DATA_PORT		PORTD
#define LED1_DIR_PORT		DDRD
#define LED1_PIN			PIND6

#define LED2_DATA_PORT		PORTD
#define LED2_DIR_PORT		DDRD
#define LED2_PIN			PIND7
/*******************************/

/**************CAN**************/
#define SPI_DDR				DDRB
#define SPI_PORT			PORTB
#define SPI_MISO			PINB3
#define SPI_MOSI			PINB2
#define SPI_SCK				PINB1

#define MCP2515_SS_PORT		PORTB
#define MCP2515_SS_DDR		DDRB
#define MCP2515_SS_PIN		PINB0

#define MCP2515_INT_PORT	PORTB
#define MCP2515_INT_DDR		DDRB
#define MCP2515_INT_PIN		PINB4
/*******************************/

/**************WDT**************/
#define WDT_TIMEOUT			5000UL
#define TPL5010_DONE_PORT	PORTE
#define TPL5010_DONE_DDR	DDRE
#define TPL5010_DONE_PIN	PINE6

#define TPL5010_WAKE_PORT	PORTE
#define TPL5010_WAKE_DDR	DDRE
#define TPL5010_WAKE_PIN	PINE7
/*******************************/

/**************MUX**************/
#define MUX_A0_PORT			PORTE
#define MUX_A0_DDR			DDRE
#define MUX_A0_PIN			PINE2

#define MUX_A1_PORT			PORTE
#define MUX_A1_DDR			DDRE
#define MUX_A1_PIN			PINE3

#define MUX_A2_PORT			PORTE
#define MUX_A2_DDR			DDRE
#define MUX_A2_PIN			PINE5

#define MUX_A3_PORT			PORTE
#define MUX_A3_DDR			DDRE
#define MUX_A3_PIN			PINE4

#define MUX_OUT_PORT		PORTF
#define MUX_OUT_DDR			DDRF
#define MUX_OUT_PIN			PINF3
/********************************/

/************MCU_PWR*************/
#define MCU_BUCK_EN_PORT	PORTC
#define MCU_BUCK_EN_DDR		DDRC
#define MCU_BUCK_EN_PIN		PINC3

#define IOU_PWR_EN_PORT		PORTC
#define IOU_PWR_EN_DDR		DDRC
#define IOU_PWR_EN_PIN		PINC6

#define COPC_PWR_EN_PORT	PORTC
#define COPC_PWR_EN_DDR		DDRC
#define COPC_PWR_EN_PIN		PINC5
/********************************/

/**************TEC1**************/
#define TEC1_BUCK_EN_PORT	PORTA
#define TEC1_BUCK_EN_DDR	DDRA
#define TEC1_BUCK_EN_PIN	PINA0

#define TEC1_EN_PORT		PORTA
#define TEC1_EN_DDR			DDRA
#define TEC1_EN_PIN			PINA1
/********************************/

/**************TEC2**************/
#define TEC2_BUCK_EN_PORT	PORTA
#define TEC2_BUCK_EN_DDR	DDRA
#define TEC2_BUCK_EN_PIN	PINA3

#define TEC2_EN_PORT		PORTA
#define TEC2_EN_DDR			DDRA
#define TEC2_EN_PIN			PINA2
/********************************/

/**************TEC3**************/
#define TEC3_BUCK_EN_PORT	PORTA
#define TEC3_BUCK_EN_DDR	DDRA
#define TEC3_BUCK_EN_PIN	PINA5

#define TEC3_EN_PORT		PORTA
#define TEC3_EN_DDR			DDRA
#define TEC3_EN_PIN			PINA4
/********************************/

/**************TEC4**************/
#define TEC4_BUCK_EN_PORT	PORTG
#define TEC4_BUCK_EN_DDR	DDRG
#define TEC4_BUCK_EN_PIN	PING2

#define TEC4_EN_PORT		PORTA
#define TEC4_EN_DDR			DDRA
#define TEC4_EN_PIN			PINA6
/*******************************/

/************LED_PWR************/
#define LED_BUCK_EN_PORT	PORTC
#define LED_BUCK_EN_DDR		DDRC
#define LED_BUCK_EN_PIN		PINC4

#define LEDRGB_EN_PORT		PORTC
#define LEDRGB_EN_DDR		DDRC
#define LEDRGB_EN_PIN		PINC7

#define LEDIR_EN_PORT		PORTA
#define LEDIR_EN_DDR		DDRA
#define LEDIR_EN_PIN		PINA7
/*******************************/

/************12V_PWR*************/
#define CM4_BUCK_EN_PORT	PORTC			//CM4 & RADIO
#define CM4_BUCK_EN_DDR		DDRC
#define CM4_BUCK_EN_PIN		PINC0

#define CM4_PWR_EN_PORT		PORTG
#define CM4_PWR_EN_DDR		DDRG
#define CM4_PWR_EN_PIN		PING1

#define RADIO_PWR_EN_PORT	PORTG
#define RADIO_PWR_EN_DDR	DDRG
#define RADIO_PWR_EN_PIN	PING0
/********************************/

/************PDU_INT************/
#define PDU_INT_PORT		PORTD
#define PDU_INT_DDR			DDRD
#define PDU_INT_PIN			PINC5
/*******************************/

/***********POWER_GOOD**********/
#define PDU_INT_PORT		PORTD
#define PDU_INT_DDR			DDRD
#define PDU_INT_PIN			PINC5
/*******************************/

/***********HDRM_CAP**********/
#define HDRM_PORT			PORTB
#define HDRM_DDR			DDRB
#define HDRM_PIN			PINB6

#define CAPC_PORT			PORTB
#define CAPC_DDR			DDRB
#define CAPC_PIN			PINB7
/*******************************/

/*************5V_BUS************/
#define BUS_EN_PORT			PORTC
#define BUS_EN_DDR			DDRC
#define BUS_EN_PIN			PINC1

#define BUS_FAULT_PORT		PORTC
#define BUS_FAULT_DDR		DDRC
#define BUS_FAULT_PIN		PINC2
/*******************************/

#define VREF				(float)3.3
#define ADC_2V				(float)((2*1024/VREF) / 2) + 10
#define ADC_5V				(float)((5*1024/VREF) / 2) + 10
#define ADC_12V				(uint16_t)((12*1024/VREF) / 11) + 10
#define	ADC_VBUS			(uint16_t)((14.4*1024/VREF) / 11) + 10
#define ADC_1A				(uint16_t)(1*1024/3) + 10
#define ADC_2A				(uint16_t)(2*1024/3) + 10
#define ADC_2A5				(uint16_t)(2.5*1024/3) + 10
#define SCALE_5V			(float)(VREF*2/1024)
#define SCALE_12V			(float)(VREF*11/1024)
#define SCALE_VBUS			(float)(VREF*11/1024)
#define SCALE_A				(float)(0.002929)
#define ANALOG_CHANNEL		19
#define	MUX_WAIT_FOR_STABLE 3
#define POWERIO_CHANNEL		19
#define POWER_OUTPUT		10
#endif

#endif /* PDU_BOARD_H_ */
