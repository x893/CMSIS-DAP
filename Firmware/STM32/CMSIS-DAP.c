/*------------------------------------------------------------------------------
 *      RL-ARM - USB
 *------------------------------------------------------------------------------
 *      Name:    USBD_Demo.c
 *      Purpose: USB Device Demonstration
 *      Rev.:    V4.70
 *------------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include <RTL.h>
#include <rl_usb.h>
#include <stm32f10x.h>                         /* STM32F10x Definitions      */

#define  __NO_USB_LIB_C
#include "usb_config.c"

#include "DAP_config.h"
#include "..\DAP.h"

#include "usbd_user_cdc_acm.h"

void usbd_hid_process(void);
void CheckUserApplication(void);
void LedConnectedOn(void);
void LedConnectedOff(void);
void LedRunningOn(void);
void LedRunningOff(void);
void LedRunningToggle(void);
void LedConnectedOut(uint16_t bit);
void LedRunningOut(uint16_t bit);
void Delay_ms(uint32_t delay);

extern const UserAppDescriptor_t UserAppDescriptor;

UserAppDescriptor_t * pUserAppDescriptor = NULL;

const CoreDescriptor_t CoreDescriptor = {
	&LedConnectedOut,
	&LedRunningOut,
};

volatile uint32_t led_count = 0;

#if (USBD_CDC_ACM_ENABLE == 1)

/* Check if status has changed and if so, send notify to USB Host on Int EP   */
void NotifyOnStatusChange (void)
{
	static int32_t old_notify = -1;
	int32_t stat, com_err, notify;

	stat    = UART_GetStatusLineState();
	com_err = UART_GetCommunicationErrorStatus();
	notify = (((com_err & UART_OVERRUN_ERROR_Msk) == UART_OVERRUN_ERROR_Msk)  << CDC_SERIAL_STATE_OVERRUN_Pos)    |
			(((com_err & UART_PARITY_ERROR_Msk ) == UART_PARITY_ERROR_Msk )   << CDC_SERIAL_STATE_PARITY_Pos )    |
			(((com_err & UART_FRAMING_ERROR_Msk) == UART_FRAMING_ERROR_Msk)   << CDC_SERIAL_STATE_FRAMING_Pos)    |
			(((stat & UART_STATUS_LINE_RI_Msk )  == UART_STATUS_LINE_RI_Msk)  << CDC_SERIAL_STATE_RING_Pos   )    |
			(UART_GetBreak() << CDC_SERIAL_STATE_BREAK_Pos)                                                   |
			(((stat & UART_STATUS_LINE_DSR_Msk)  == UART_STATUS_LINE_DSR_Msk) << CDC_SERIAL_STATE_TX_CARRIER_Pos) |
			(((stat & UART_STATUS_LINE_DCD_Msk)  == UART_STATUS_LINE_DCD_Msk) << CDC_SERIAL_STATE_RX_CARRIER_Pos) ;
	if (notify ^ old_notify)
	{	// If notify changed
		if (USBD_CDC_ACM_Notify (notify))   // Send new notification
			old_notify = notify;
	}
}

#endif

int main (void)
{
#if (USBD_CDC_ACM_ENABLE == 1)
	int32_t usb_rx_ch = -1;
	int32_t usb_tx_ch = -1;
#endif

	LEDS_SETUP();
	LedRunningOn();				// Turn on LEDs
	LedConnectedOn();			// while not device configured

	if (UserAppDescriptor.UserInit != NULL)
	{
		pUserAppDescriptor = &UserAppDescriptor;
		pUserAppDescriptor->UserInit((CoreDescriptor_t *)&CoreDescriptor);
	}

	usbd_init();                          /* USB Device Initialization          */
	usbd_connect(__TRUE);                 /* USB Device Connect                 */

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
	Delay_ms(10);				// Wait for 100ms
	LedRunningOff();			// Turn off LEDs
	LedConnectedOff();
	led_count = 0;

	while (1)
	{
		usbd_hid_process();

		if (pUserAppDescriptor == NULL
		&&	led_count++ == 50000)
		{
			led_count = 0;
			LedRunningToggle();
		}

#if (USBD_CDC_ACM_ENABLE == 1)
		NotifyOnStatusChange();

		// USB -> UART
		if (usb_rx_ch == -1)
		{
			usb_rx_ch = USBD_CDC_ACM_GetChar ();
		}
		if (usb_rx_ch != -1)
		{
			if (UART_PutChar (usb_rx_ch) == usb_rx_ch)
			{
				usb_rx_ch = -1;
			}
		}

		// UART -> USB
		if (usb_tx_ch == -1)
		{
			usb_tx_ch = UART_GetChar ();
		}
		if (usb_tx_ch != -1)
		{
			if (USBD_CDC_ACM_PutChar (usb_tx_ch) == usb_tx_ch)
			{
				usb_tx_ch = -1;
			}
		}
#endif
	}
}

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
