#include <RTL.h>
#include <rl_usb.h>
#include "DAP_config.h"
#include "..\DAP.h"

extern void usbd_hid_process (void);
extern const UserAppDescriptor_t UserAppDescriptor;

void LedConnectedOn(void)	{	LED_CONNECTED_PORT->BSRR = LED_CONNECTED_PIN;	}
void LedConnectedOff(void)	{	LED_CONNECTED_PORT->BRR  = LED_CONNECTED_PIN;	}
void LedRunningOn(void)		{	LED_RUNNING_PORT->BSRR   = LED_RUNNING_PIN;		}
void LedRunningOff(void)	{	LED_RUNNING_PORT->BRR    = LED_RUNNING_PIN;		}
void LedRunningToggle(void)
{
	if ((LED_RUNNING_PORT->ODR & LED_RUNNING_PIN) == 0)
		LedRunningOn();
	else
		LedRunningOff();
}

void LedConnectedOut(uint16_t bit)
{
	if (bit & 1)	LedConnectedOn();
	else			LedConnectedOff();
}
void LedRunningOut(uint16_t bit)
{
	if (bit & 1)	LedRunningOn();
	else			LedRunningOff();
}

const CoreDescriptor_t CoreDescriptor = {
	&LedConnectedOut,
	&LedRunningOut,
};


UserAppDescriptor_t * pUserAppDescriptor = NULL;

// Delay for specified time
//    delay:  delay time in ms
void Delay_ms(uint32_t delay)
{
	delay *= (CPU_CLOCK / 1000 + (DELAY_SLOW_CYCLES - 1)) / DELAY_SLOW_CYCLES;
	PIN_DELAY_SLOW(delay);
}

void HardFault_Handler(void);
void NMI_Handler(void)			__attribute((alias("HardFault_Handler")));
void MemManage_Handler(void)	__attribute((alias("HardFault_Handler")));
void BusFault_Handler(void)		__attribute((alias("HardFault_Handler")));
void UsageFault_Handler(void)	__attribute((alias("HardFault_Handler")));
void SVC_Handler(void)			__attribute((alias("HardFault_Handler")));
void DebugMon_Handler(void)		__attribute((alias("HardFault_Handler")));
void PendSV_Handler(void)		__attribute((alias("HardFault_Handler")));
void HardFault_Handler(void)
{
	__disable_irq();
	LEDS_SETUP();
	while(1)
	{
		LedRunningOn();
		Delay_ms(250);
		LedRunningOff();
		LedConnectedOn();
		Delay_ms(250);
		LedConnectedOff();
		Delay_ms(500);		// Wait for 500ms
	}
}

void CheckUserApplication(void);
// void Reset_Handler(void)		__attribute((alias("main")));

volatile uint32_t led_count = 0;

int main (void)
{
	if (UserAppDescriptor.UserInit != NULL)
	{
		pUserAppDescriptor = &UserAppDescriptor;
		pUserAppDescriptor->UserInit((CoreDescriptor_t *)&CoreDescriptor);
	}

	LEDS_SETUP();
	LedRunningOn();				// Turn on LEDs
	LedConnectedOn();			// while not device configured

	usbd_init();
	usbd_connect(__TRUE);
	led_count = 0;
	while (!usbd_configured())	// Wait for USB Device to configure
	{
		if (led_count++ == 1000000)
		{
			usbd_connect(__FALSE);
			Delay_ms(100);
			NVIC_SystemReset();
		}
	}

	Delay_ms(100);				// Wait for 100ms
	LedRunningOff();			// Turn off LEDs
	LedConnectedOff();
	led_count = 0;
	while (1)
	{
		usbd_hid_process();		// Process USB HID Data
		if (pUserAppDescriptor == NULL
		&&	led_count++ == 50000)
		{
			led_count = 0;
			LedRunningToggle();
		}
	}
}

/******************************************************************************
 * @file     main.c
 * @brief    CMSIS-DAP Main module
 * @version  V1.00
 * @date     31. May 2012
 *
 * @note
 * Copyright (C) 2012 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M
 * processor based microcontrollers.
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/
