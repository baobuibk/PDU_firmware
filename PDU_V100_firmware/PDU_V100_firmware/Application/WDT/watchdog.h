/*
 * watchdog.h
 *
 * Created: 5/20/2024 2:05:19 PM
 *  Author: Admin
 */ 


#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#include <avr/io.h>
#include "scheduler.h"
#include "PDU_board.h"
#include "basetypedef.h"

static uint8_t WDT_Current; 
void WDT_init(void);
void status_wdt_update(void);
#endif /* WATCHDOG_H_ */