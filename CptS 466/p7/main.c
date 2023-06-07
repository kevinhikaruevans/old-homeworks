#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "ADCSWTrigger.h"
#include "UART.h"
#include "SysTick.h"
#include "Switch.h"

#include <stdio.h>

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

unsigned long temp, light, c;
unsigned long sw;
unsigned long i = 0;

int main(void){    	
  DisableInterrupts();    
  PLL_Init();          // bus clock at 80 MHz
	UART_Init();
	SysTick_Init(8e6); // 0.1 s
	ADC_Init298();
	Switch_Init();
	
	EnableInterrupts();
	
  while(1){
		WaitForInterrupt();
  }  
}

// The decision tree created by WEKA
unsigned long Classify(unsigned long temp, unsigned long light) {
		if (temp <= 879) {
			return 0;
		} else {
			if (light <= 53) {
				if (temp <= 886) {
					return 0;
				} else {
					if (light <= 45) {
						return 0;
					}
					return 1;
				}
			} else {
				return 1;
			}
		}
}

// FOR PREDICTING
void SysTick_Handler(void) {
	char buffer[64] = {0};
	if (i++ >= 10) {
		ADC_In298(&temp, &light, &c);
		sw = Switch_In();
		i = 0;
		
		sprintf(buffer, "temp=%lu,light=%lu,prediction: %s\n", temp, light, Classify(temp, light) ? "screen" : "non-screen");
		UART_OutString(buffer);
	}
}

// FOR DATA LOGGING
void SysTick_Handler_old(void){
	char buffer[64] = {0};
	if (i++ >= 10) {
		ADC_In298(&temp, &light, &c);
		sw = Switch_In();
		i = 0;
		sprintf(buffer, "%lu,%lu,%d\n", temp, light, sw);
		UART_OutString(buffer);
	}
	//UART_OutChar('.');
	//printf(".");
}

