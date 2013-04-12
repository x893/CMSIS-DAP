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

#define TIMEOUT_DELAY	200000

#include <stdio.h>

#include <RTL.h>
#include <rl_usb.h>
#include <stm32f10x.h>                         /* STM32F10x Definitions      */

#define  __NO_USB_LIB_C
#include "usb_config.c"

#include "DAP_config.h"
#include "..\DAP.h"

#include "usbd_user_cdc_acm.h"

uint8_t usbd_hid_process(void);
void CheckUserApplication(void);
void LedConnectedOn(void);
void LedConnectedOff(void);
void LedConnectedToggle(void);
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

#if (USBD_CDC_ACM_ENABLE == 1)
	int32_t usb_rx_ch;
	int32_t usb_tx_ch;
#endif
uint32_t led_count;
uint32_t led_timeout;

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

void BoardInit(void)
{
	const GPIO_InitTypeDef pins_A_init = {
		~(GPIO_Pin_13 | GPIO_Pin_14),
		GPIO_Speed_2MHz,
		GPIO_Mode_IN_FLOATING
	};
	const GPIO_InitTypeDef pins_BC_init = {
		GPIO_Pin_All,
		GPIO_Speed_2MHz,
		GPIO_Mode_IN_FLOATING
	};
	const GPIO_InitTypeDef pins_B34_init = {
		GPIO_Pin_3 | GPIO_Pin_4,
		GPIO_Speed_2MHz,
		GPIO_Mode_IPD
	};
	const GPIO_InitTypeDef pins_A15_init = {
		GPIO_Pin_15,
		GPIO_Speed_2MHz,
		GPIO_Mode_IPD
	};

	RCC->APB2ENR |= (RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	GPIO_INIT(GPIOA, pins_A_init);
	GPIO_INIT(GPIOA, pins_A15_init);

	GPIO_INIT(GPIOB, pins_BC_init);
	GPIO_INIT(GPIOB, pins_B34_init);

	LEDS_SETUP();
}

void USBD_Error_Event(void)
{
	LedConnectedOn();
	LedRunningOn();

	usbd_connect(__FALSE);
	usbd_reset_core();

	while(1);
}

int main (void)
{
	BoardInit();
	LedConnectedOn();
	if (UserAppDescriptor.UserInit != NULL)
	{
		pUserAppDescriptor = &UserAppDescriptor;
		pUserAppDescriptor->UserInit((CoreDescriptor_t *)&CoreDescriptor);
	}

	LedConnectedOff();
	led_count = 0;
	// Check for USB connected
	while ((GPIOA->IDR & GPIO_Pin_11) != 0)
	{
		if (led_count++ == 0)
			LedConnectedOn();
		else if (led_count == 5)
			LedConnectedOff();
		else if (led_count == 25)
			led_count = 0;
		Delay_ms(10);
	}
	LedConnectedOff();

	// USB Device Initialization and connect
	usbd_init();
	usbd_connect(__TRUE);

	led_count = 0;
	while (!usbd_configured())	// Wait for USB Device to configure
	{
		if (led_count++ == 0)
			LedConnectedOn();
		else if (led_count == 5)
			LedConnectedOff();
		else if (led_count == 100)
			led_count = 0;
		Delay_ms(10);
	}
	LedConnectedOff();
	Delay_ms(100);			// Wait for 100ms

	led_count = 0;
	led_timeout = TIMEOUT_DELAY;
	usb_rx_ch = -1;
	usb_tx_ch = -1;

	while (1)
	{
		if (pUserAppDescriptor == NULL)
		{	// No user application
			if (led_count++ == 1000000)
			{
				led_count = 0;
				LedConnectedToggle();
			}
			usbd_hid_process();
		}
		else if (!usbd_hid_process())
		{
			// No packet processing
			if (led_timeout == 500)
			{
				LedConnectedOn();
			}
			else if (led_timeout == 0)
			{
				LedConnectedOff();
				led_timeout = TIMEOUT_DELAY;
			}
			led_timeout--;
			
		}
		else
		{
			led_timeout = TIMEOUT_DELAY;
		}

#if (USBD_CDC_ACM_ENABLE == 1)

		NotifyOnStatusChange();

		// USB -> UART
		if (usb_rx_ch == -1)
			usb_rx_ch = USBD_CDC_ACM_GetChar ();

		if (usb_rx_ch != -1)
		{
			if (UART_PutChar (usb_rx_ch) == usb_rx_ch)
				usb_rx_ch = -1;
		}

		// UART -> USB
		if (usb_tx_ch == -1)
			usb_tx_ch = UART_GetChar ();

		if (usb_tx_ch != -1)
		{
			if (USBD_CDC_ACM_PutChar (usb_tx_ch) == usb_tx_ch)
				usb_tx_ch = -1;
		}
#endif
	}
}

void LedConnectedOn(void)		{	LED_CONNECTED_PORT->BSRR = LED_CONNECTED_PIN;	}
void LedConnectedOff(void)		{	LED_CONNECTED_PORT->BRR  = LED_CONNECTED_PIN;	}
void LedConnectedToggle(void)	{	LED_CONNECTED_PORT->ODR ^= LED_CONNECTED_PIN;	}
void LedRunningOn(void)			{	LED_RUNNING_PORT->BSRR   = LED_RUNNING_PIN;		}
void LedRunningOff(void)		{	LED_RUNNING_PORT->BRR    = LED_RUNNING_PIN;		}
void LedRunningToggle(void)		{	LED_RUNNING_PORT->ODR   ^= LED_RUNNING_PIN;		}

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
	delay *= (CPU_CLOCK / 1000 + (DELAY_SLOW_CYCLES - 1)) / (2 * DELAY_SLOW_CYCLES);
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
