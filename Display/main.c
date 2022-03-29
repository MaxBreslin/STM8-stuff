// TCS 2022 03 16 coda factory blink 
// factory test code to blink on board led 

#include <intrinsics.h>
#include <iostm8S103f3.h>

#define OUTPIN PC_ODR_ODR3

void init_sysclk() {			//  Initialize SysClk
	CLK_ICKR = 0;			//  Reset the internal Clock Register
	CLK_ICKR_HSIEN = 1;		//  Enable the HSI
	CLK_ECKR = 0;			//  Disable the external clock
	while (CLK_ICKR_HSIRDY == 0);	//  Wait for the HSI to be ready for use
	CLK_CKDIVR = 0;			//  Ensure the clocks are running at full speed
	CLK_PCKENR1 = 0xff;		//  Enable all peripheral clocks
	CLK_PCKENR2 = 0xff;
	CLK_CCOR = 0;			//  Turn off Configurable Clock Output 
	CLK_HSITRIMR = 0;		//  Turn off any HSIU trimming
	CLK_SWIMCCR = 0;		//  Set SWIM to run at clock / 2
	CLK_SWR = 0xe1;			//  Use HSI as the clock source
	CLK_SWCR = 0;			//  Reset the clock switch control register
	CLK_SWCR_SWEN = 1;		//  Enable switching
	while (CLK_SWCR_SWBSY != 0);	//  Pause while the clock switch is busy
}

void init_tim2() {			//  Setup Timer 2 to generate an interrupt every 1mS based on a 16MHz clock.
	TIM2_PSCR = 0x04;		//  Prescaler = 32. actually 16
	TIM2_ARRH = 0x03;		//  High byte of 940. 
	TIM2_ARRL = 0xe6;		//  Low byte 
	TIM2_IER_UIE = 1;		//  Enable the update interrupts.
	TIM2_CR1_CEN = 1;		//  Finally enable the timer.
}

unsigned long systick_counter = 0;
unsigned long delay_sync_target = 0;

void delayms(unsigned long delay_for) { 	// Standard delay millis implementation 
	delay_for = delay_for - 1;
	unsigned long delay_for_target = systick_counter + delay_for;
	while(delay_for_target >= systick_counter); 		// Potential issue with overflow/wrap-around? 
}

#pragma vector = TIM2_OVR_UIF_vector // TIM2 overflow interrupt handler 
__interrupt void TIM2_UPD_OVF_IRQHandler(void) {
	systick_counter++;	//  System tick increment
	TIM2_SR1_UIF = 0;	//  Reset the interrupt
}

void init_gpio() { 
         PC_DDR_DDR3 = 1;
         PC_CR1_C13 = 1;
         PC_CR2_C23 = 1;
} 

void init_interrupt() {
	// __disable_interrupt();
	// __wait_for_interrupt();
	__enable_interrupt(); // asm("RIM"); //enable interrupts 
}

int main(void) {
	init_sysclk(); // Initialize SysClk
	init_tim2(); // Initialize TIM2 
	init_gpio(); // Initialize ports 
	init_interrupt(); // initialize interrupt controller 
	while (1) {
		OUTPIN = 1;
		delayms(100);
		OUTPIN = 0;
		delayms(400);
	}
}