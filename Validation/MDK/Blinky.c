/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: Freescale MK20DX128xxx5 LED Flasher
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <MK20D5.H>

#define LED_NUM     2                   /* Number of user LEDs                */
//const uint32_t led_mask[] = {1UL << 9, 1UL << 10};
const unsigned long led_mask[] = { 1UL<<28, 1UL<<29, 1UL<<31, 1UL<< 2,
                                   1UL<< 3, 1UL<< 4, 1UL<< 5, 1UL<< 6 };

volatile uint32_t msTicks;                            /* counts 1ms timeTicks */


// temporary varuiables for debugging purposes
unsigned long tmp_mask;
uint32_t pin1_val;
volatile uint32_t* pin1_addr;
uint32_t pin2_val;
volatile uint32_t* pin2_addr;

int reg_test = 0;
int test_success = 0;
char mem_write_success = 0;

int bpTestCounter = 0;

int test_array1[256] = {0}; 
int test_array2[256] = {0};


/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;                        /* increment counter necessary in Delay() */
}

/*------------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *------------------------------------------------------------------------------*/
__INLINE static void Delay (uint32_t dlyTicks) {
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

/*------------------------------------------------------------------------------
  configer LED pins
 *------------------------------------------------------------------------------*/
static void LED_Config(void) {
  SIM->SCGC5   |= (1UL << 11);        /* Enable Clock to Port C */ 
  PORTC->PCR[9] = (1UL <<  8);        /* Pin is GPIO */
  PORTC->PCR[10] = (1UL <<  8);       /* Pin is GPIO */

  PTC->PDOR = (led_mask[0] | 
               led_mask[1] );          /* switch LEDs off  */
  PTC->PDDR = (led_mask[0] | 
               led_mask[1] );          /* enable Output */
}

/*------------------------------------------------------------------------------
  Switch on LEDs
 *------------------------------------------------------------------------------*/
__INLINE static void LED_On (uint32_t led) {

  PTC->PCOR   = led_mask[led];
}


/*------------------------------------------------------------------------------
  Switch off LEDs
 *------------------------------------------------------------------------------*/
__INLINE static void LED_Off (uint32_t led) {

  PTC->PSOR   = led_mask[led];
}


void BP_Test()
{
  int i, j;
  
  for(i=0; i<10;  i++) {  
    bpTestCounter ++; // increment counter so we know on which iteration breakpoint is hit
    j = bpTestCounter;
  }
}



void Test() 
{
  int i, n;
// we can get here only if test_completed is set to one from outside the applicatin (from script)
  int success;  // use register variable, we will set it to 1 from script 
// execute this block only once
  static int done = 0;
  if(done)
    return;
  done = 1;
  
  success = test_success; // use register variable, we will set it to 1 from script 

  n = 0xA0; // initialize n with 0xA0, we will override this via register from debugger 		done = 1;
  success--;
  test_success = success;
  // copy test_array1 into test_array2 for future comparison 
  // test_array1 should already be modified by debugger and n is set to 256
  mem_write_success = 1; // assume all values were written correctly
  for( i = 0; i < n; i++)
  {
    if(test_array1[i] != 0x1000+ i)
      mem_write_success = 0;
    test_array2[i] = test_array1[i];
  }

  test_success = (success == 1);
}




/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
  int num     = -1; 
  int dir     =  1;

  SystemCoreClockUpdate();                      /* Get Core Clock Frequency */
  SysTick_Config(SystemCoreClock/1000);         /* Generate interrupt each 1 ms    */
  
  BP_Test();
  Test();
  
  while(1);
  
#if 0
  LED_Config();                             
 
  while(1) {
    /* Calculate 'num': 0,1,...,LED_NUM-1,LED_NUM-1,...,1,0,0,...             */
    num += dir;
    if (num == LED_NUM) { dir = -1; num =  LED_NUM-1; } 
    else if   (num < 0) { dir =  1; num =  0;         }
    LED_On (num);
    Delay(250);
    LED_Off(num);
    Delay(250);
  }
#endif  
}

