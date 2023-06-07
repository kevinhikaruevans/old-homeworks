// Multicolor LED tolggles between red-green. 
// When SW1 pressed the LED toggles blue-green.
// Demo Code of P1
// 0.Documentation Section 
// main.c
// Lab2_HelloLaunchPad, Input from PF4, output to PF3,PF2,PF1 (LED)

// LaunchPad built-in hardware
// SW1 left switch is negative logic PF4 on the Launchpad
// SW2 right switch is negative logic PF0 on the Launchpad
// red LED connected to PF1 on the Launchpad
// blue LED connected to PF2 on the Launchpad
// green LED connected to PF3 on the Launchpad

// 1. Pre-processor Directives Section
// Constant declarations to access port registers using 
// symbolic names instead of addresses
//#include "TExaS.h"
#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))

// 2. Declarations Section
//   Global Variables
unsigned long In; // input from PF0

//   Function Prototypes
void PortF_Init(void);
void Delay(void);
void Delay_Seconds(int seconds) {
	//int s, i;
	while(seconds) {
		unsigned long volatile time;
		time = 1454480;  // 1 second
			while(time){
				time--;
		}
		seconds--;
	}
}


typedef enum color_t {
	DARK = 0,
	RED = 0x02,
	BLUE = 0x04,
	GREEN = 0x08,
	PINK = 0x06
} COLOR;

typedef enum switch_t {
	SW1 = 0x10,
	SW2 = 0x01
} SWITCH;

//void EnableInterrupts(void);


// 3. Subroutines Section
// MAIN: Mandatory for a C Program to be executable
int main(void){    
	COLOR state = DARK;
	int seconds;
  PortF_Init();        // Call initialization of port PF4 PF2    
	

  while(1){
		// read SW1 and SW2 and invert to flip from negative logic to positive logic
		In = (GPIO_PORTF_DATA_R & (SW1 | SW2)) ^ (SW1 | SW2);		
		
		switch(state) {
			
			case PINK:
				// seconds = 4;
				// wait for 4 seconds and during the loop, check if SW
				/*
				while(seconds) {
					unsigned long volatile time;
					time = 1454480;  // 1 second
					while(time){
							In = (GPIO_PORTF_DATA_R & (SW1 | SW2)) ^ (SW1 | SW2);		
							if (In == SW1) {
								state = GREEN;
								time = 0;
								seconds = 0;
								goto exit_loop;
							}
							time--;
					}
					seconds--;
				}
				state = DARK;
				*/
				Delay_Seconds(4);
				In = (GPIO_PORTF_DATA_R & (SW1 | SW2)) ^ (SW1 | SW2);		
				if (In == SW1) {
					state = GREEN;
				} else {
					state = DARK;
				}
				break;
			case RED:
				Delay_Seconds(1);
				state = DARK;
				break;
			
			case BLUE:
				Delay_Seconds(2);
				state = DARK;
				break;
			
			case GREEN:
				Delay_Seconds(1);
				state = DARK;
				break;
			
			default:
				Delay_Seconds(1);
			  state = DARK;
				break;
			
			case DARK:
				if (In == SW1) {
					state = RED;
				} else if (In == SW2) {
					state = BLUE;
				} else if (In == (SW1 | SW2)) {
					state = PINK;
				}
				break;
		}
		
	//exit_loop:
		//GPIO_PORTF_DATA_R = state;
		/*
    if(In == 0x00){ // zero means SW1 is pressed (Negative Logic, On = 0)
      GPIO_PORTF_DATA_R = 0x04;  // LED is blue (Set the third pin of portF which is connected to the blue LED)
			Delay();
		}
			
			else{GPIO_PORTF_DATA_R = 0x02; // LED is red
			}  
    
    Delay();                     // wait 0.1 sec
    GPIO_PORTF_DATA_R = 0x08;    // LED is green
    Delay();                     // wait 0.1 sec
		*/
  }
}

// Subroutine to initialize port F pins for input and output
// PF4 and PF0 are input SW1 and SW2 respectively
// PF3,PF2,PF1 are outputs to the LED
// Inputs: None
// Outputs: None
// Notes: These five pins are connected to hardware on the LaunchPad
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  //GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  //GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        
}
// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E
// pink     R-B    0x06


// Subroutine to wait 0.1 sec. 
// Inputs: None
// Outputs: None
// Notes: It is an estimation based on the clock frequency and the number of cycles per instruction.
void Delay(void){unsigned long volatile time;
  time = 145448;  // 0.1sec
  while(time){
		time--;
  }
}
