#define LIGHT                   (*((volatile unsigned long *)0x400050FC)) //bits 5-0
#define GPIO_PORTB_OUT          (*((volatile unsigned long *)0x400050FC)) // bits 5-0
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
#define GPIO_PORTE_IN           (*((volatile unsigned long *)0x4002400C)) // bits 1-0
#define PedSwitch               (*((volatile unsigned long *)0x40024004))

#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOE      0x00000010  // port E Clock Gating Control
#define SYSCTL_RCGC2_GPIOB      0x00000002  // port B Clock Gating Control

//Initialize PLL and configure the clock frequency using SYSDIV2
void PLL_Init(void);
// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void);
// Time delay using busy wait. This assumes 80 MHz system clock.
void SysTick_Wait10ms(unsigned long delay);

void GoS_f(void){	
LIGHT = 0x11;
SysTick_Wait10ms(50);	
};
void waitS_f(void){	
unsigned short i = 0;	
	LIGHT = 0x10;	
while(i<10){
	SysTick_Wait10ms(50);
	LIGHT ^= 0x02;
	i++;
}	
}
void stopS_f(void){
LIGHT = 0x12;
SysTick_Wait10ms(100);
}
void goP_f(void){
LIGHT = 0x0c;	
SysTick_Wait10ms(500);
}

void slowP_f(void){
unsigned short i = 0;	
	LIGHT = 0x04;	
while(i<10){
	SysTick_Wait10ms(50);
	LIGHT ^= 0x10;
	i++;
}
}

void startS_f(void){
unsigned short i = 0;	
	LIGHT = 0x10;	
while(i<10){
	SysTick_Wait10ms(50);
	LIGHT ^= 0x02;
	i++;
}
}
// Linked data structure
struct State {
	void (*CmdPt)(void);   // output function 
  unsigned long Next[2];}; 
typedef const struct State STyp;
#define goS 	  0
#define waitS 	1
#define stopS   2
#define goP 		3
#define slowP 	4	
#define startS 	5	
STyp FSM[6]={
 {&GoS_f,{goS,waitS}}, 
 {&waitS_f, {stopS,stopS}},
 {&stopS_f,{goP,goP}},
 {&goP_f, {slowP,slowP}},
 {&slowP_f, {startS,startS}},
 {&waitS_f, {goS,goS}}};

unsigned long S;  // index to the current state 
unsigned long Input; 
int main(void){ volatile unsigned long delay;
  PLL_Init();       // 80 MHz, Program 10.1
  SysTick_Init();   // Program 10.2
  SYSCTL_RCGC2_R |= 0x12;      // 1) B E
  delay = SYSCTL_RCGC2_R;      // 2) no need to unlock
  GPIO_PORTE_AMSEL_R &= ~0x03; // 3) disable analog function on PE1-0
  GPIO_PORTE_PCTL_R &= ~0x000000FF; // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x03;   // 5) inputs on PE1-0
  GPIO_PORTE_AFSEL_R &= ~0x03; // 6) regular function on PE1-0
  GPIO_PORTE_DEN_R |= 0x03;    // 7) enable digital on PE1-0
  GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;    // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0
  S = goS;  
  while(1){
		(FSM[S].CmdPt)(); // output + delay
    Input = PedSwitch;     // read sensors
    S = FSM[S].Next[Input]; // next states 
  }
}

