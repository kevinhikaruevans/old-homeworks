// R2R.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a single 440Hz sine wave
// Daniel Valvano, Jonathan Valvano
// September 15, 2013

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// Port B bits 2-0 have the 3-bit DAC
// Port F is onboard LaunchPad switches and LED
// Port F bit 4 is negative logic switch to play sound, SW1
// SysTick ISR: PF3 ISR heartbeat


#include "tm4c123gh6pm.h"
#include "LED.h"
#include "PLL.h"
#include "Sound.h"
#include "Switch.h"
#include "ADCSWTrigger.h"
#include "UART.h"
#include <stdio.h>

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

volatile unsigned long ADCIN = 0;
volatile unsigned int counter = 0;

void TempChange_Handler(void) {
	char buffer[64]   = {0};
	float voltage     = (3.3 * ADCIN / 4095.0);
	float temperature = 100 * (voltage - 0.5);
	
	sprintf(buffer, "Temperature is %.2f*C\n", temperature);
	UART_OutString(buffer);
	
	if (temperature >= 23) {
		SetLED(RED);
		Sound_Init(50000 / 4); // 100 Hz
	} else if (temperature < 20) {
		SetLED(GREEN);
		Sound_Init(50000); // 400 Hz
	} else {
		SetLED(YELLOW);
		Sound_Init(50000 / 2); // 200 Hz
	}
	
}

// need to generate a 100 Hz sine wave
// table size is 16, so need 100Hz*16=1.6 kHz interrupt
// bus is 80MHz, so SysTick period is 80000kHz/1.6kHz = 50000
int main(void){    
	unsigned long prevADCIN = 0;
	
  DisableInterrupts();    
  PLL_Init();          // bus clock at 80 MHz
  Switch_Init();       // Port F is onboard switches, LEDs, profiling
	UART_Init();
	ADC0_InitSWTriggerSeq3_Ch1();
  Sound_Init(50000);   // initialize SysTick timer, 100 Hz
	EnableInterrupts();
	
  while(1){ 
		if (prevADCIN != ADCIN) {
			TempChange_Handler();
			prevADCIN = ADCIN;
		}
  }  
}


// Interrupt service routine
// This happens around 16*(100 to 400 Hz), depending on the alert sound signal.
void SysTick_Handler(void){
	if (counter++ > 1000) {
		// decrease the sampling rate on the temp sensor
		ADCIN = ADC0_InSeq3();
		counter = 0;
	}
	
	SysTick_Sound_Handler();
}

