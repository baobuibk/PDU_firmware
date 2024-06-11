/*
 * status_led.h
 *
 * Created: 5/15/2024 2:22:27 PM
 *  Author: Admin
 */ 


#ifndef STATUS_LED_H_
#define STATUS_LED_H_

typedef	enum Led_DisplayStateTypedef
{POWERUP=0, NORMAL, POWERING_SUB}Led_DisplayStateTypedef;
void	status_led_init(void);
void	status_led_set_status(Led_DisplayStateTypedef status);
void	status_led_create_task(void);

#endif /* STATUS_LED_H_ */