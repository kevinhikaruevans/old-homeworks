// Sound.c, derived from SysTickInts.c

// Runs on LM4F120 or TM4C123, 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// September 15, 2013
// Port B bits 2-0 have the 3-bit DAC
// Port F is onboard LaunchPad switches and LED
// Port F bit 4 is negative logic switch to play sound, SW1
// SysTick ISR: PF3 ISR heartbeat
#include "tm4c123gh6pm.h"

unsigned char Index;  

// 3-bit 16-element sine wave
//const unsigned char SineWave[16] = {4,5,6,7,7,7,6,5,4,3,2,1,1,1,2,3};
const unsigned short SineWave[16] = {
	8,
	11,
	13,
	14,
	15,
	14,
	13,
	11,
	8,
	5,
	3,
	2,
	1,
	2,
	3,
	5
};
// **************DAC_Init*********************
// Initialize 3-bit DAC 
// Input: none
// Output: none
void DAC_Init(void){unsigned long volatile delay;
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD; // activate port C
  delay = SYSCTL_RCGC2_R;    // allow time to finish activating
  GPIO_PORTD_AMSEL_R &= ~0x0F;      // no analog 
  GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // regular function
  GPIO_PORTD_DIR_R |= 0x0F;      // make PC3-0 out
  GPIO_PORTD_AFSEL_R &= ~0x0F;   // disable alt funct on PBC-0
  GPIO_PORTD_DEN_R |= 0x0F;      // enable digital I/O on PBC-0
}

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Input: interrupt period
//        Units of period are 12.5ns
//        Maximum is 2^24-1
//        Minimum is determined by lenght of ISR
// Output: none
void Sound_Init(unsigned long period){
  DAC_Init();          // Port B is DAC
  Index = 0;
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1      
  NVIC_ST_CTRL_R = 0x0007;  // enable SysTick with core clock and interrupts
}

// **************DAC_Out*********************
// output to DAC
// Input: 3-bit data, 0 to 7 
// Output: none
void DAC_Out(unsigned long data){
  GPIO_PORTD_DATA_R = data;
}

void SysTick_Sound_Handler(void) {
	// GPIO_PORTF_DATA_R ^= 0x08;     // toggle PF3, debugging
  Index = (Index+1)&0x0F;  
  DAC_Out(SineWave[Index]); 
}



