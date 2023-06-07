#define NULL                    0
#define GPIO_PORTB_OUT          (*((volatile unsigned long *)0x400050FC)) // bits 5-0
#define LIGHT                   GPIO_PORTB_OUT

#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
#define GPIO_PORTE_IN           (*((volatile unsigned long *)0x4002401C)) // bits 2-0
#define INPUTS                  GPIO_PORTE_IN

#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
	
#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define PED_LIGHT               GPIO_PORTF_DATA_R
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOE      0x00000010  // port E Clock Gating Control
#define SYSCTL_RCGC2_GPIOB      0x00000002  // port B Clock Gating Control

//Initialize PLL and configure the clock frequency using SYSDIV2
void PLL_Init(void);
// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void);
// Time delay using busy wait. This assumes 80 MHz system clock.
void SysTick_Wait10ms(unsigned long delay);

void PortF_Init(void);
void PortB_Init(void);
void PortE_Init(void);
void flashPed(void);

// Linked data structure
struct State {
	void (*CmdPtr)(void);   // output function
	unsigned long Delay;
	unsigned long OutputStreet;
	unsigned long OutputPed;
  unsigned long Next[8];
}; 
typedef const struct State STyp;
	
enum StateNames {
	goMain,
	waitMain,
	goSpring,
	waitSpring,
	goPed,
	hurryPed
};

enum StreetLight {
	mainRed      = 1 << 5,
	mainYellow   = 1 << 4,
	mainGreen    = 1 << 3,
	springRed    = 1 << 2,
	springYellow = 1 << 1,
	springGreen  = 1 << 0
};

enum PedLight {
	pedOff   = 0x0,
	pedGreen = 0x8,
	pedRed   = 0x2
};

unsigned long S;  // index to the current state  
unsigned long Input; 

STyp FSM[9] = {
	{NULL,      100, mainGreen  | springRed,    pedRed,   {goMain, goMain, waitMain, waitMain, waitMain, waitMain, waitMain, waitMain}},
	{NULL,      500, mainYellow | springRed,    pedRed,   {goSpring, goSpring, goSpring, goSpring, goPed, goPed, goPed, goPed}},
	{NULL,      100, mainRed    | springGreen,  pedRed,   {goSpring, waitSpring, goSpring, waitSpring, waitSpring, waitSpring, waitSpring, waitSpring}},
	{NULL,      500, mainRed    | springYellow, pedRed,   {goMain, goMain, goMain, goMain, goPed, goPed, goPed, goPed}},
	{NULL,      50,  mainRed    | springRed,    pedGreen, {hurryPed, hurryPed, hurryPed, hurryPed, goPed, goPed, goPed, goPed}},
	{&flashPed, 50,  mainRed    | springRed,    pedRed,   {goMain, goMain, goSpring, goSpring, goMain, goMain, goSpring, goSpring}}
};

int main(void){ volatile unsigned long delay;
  PLL_Init();       // 80 MHz, Program 10.1
  SysTick_Init();   // Program 10.2
	
  SYSCTL_RCGC2_R |= 0x32;      // 1) B E F
  delay = SYSCTL_RCGC2_R;      // 2) unlock
	
	PortF_Init();
	PortB_Init();
	PortE_Init();
	
  S = goMain;  
	
  while(1){
		if (FSM[S].CmdPtr) {
			(FSM[S].CmdPtr)(); // output + delay
		} else {
			LIGHT = FSM[S].OutputStreet;
			PED_LIGHT = FSM[S].OutputPed;
			
			SysTick_Wait10ms(FSM[S].Delay);
		}
		
    Input = INPUTS;         // read sensors
    S = FSM[S].Next[Input]; // next states 
  }
}

void flashPed(void) {
	unsigned short i = 0;
	PED_LIGHT = pedRed;
	
	while(i < 10) {
		SysTick_Wait10ms(50);
		PED_LIGHT ^= pedRed;
		i++;
	}
}

// Subroutine to initialize port F pins for input and output
// PF4 and PF0 are input SW1 and SW2 respectively
// PF3,PF2,PF1 are outputs to the LED
// Inputs: None
// Outputs: None
// Notes: These five pins are connected to hardware on the LaunchPad
void PortF_Init(void){
  //SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  //delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  //GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  //GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        
}
void PortB_Init(void) {
	GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;    // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0
	
}
void PortE_Init(void) {
	GPIO_PORTE_AMSEL_R &= ~0x07; // 3) disable analog function on PE2-0
  GPIO_PORTE_PCTL_R &= ~0x000000FF; // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x07;   // 5) inputs on PE2-0
  GPIO_PORTE_AFSEL_R &= ~0x07; // 6) regular function on PE2-0
  GPIO_PORTE_DEN_R |= 0x07;    // 7) enable digital on PE2-0
}
