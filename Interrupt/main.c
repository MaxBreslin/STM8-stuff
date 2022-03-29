// Switch at Port C pin 3 (pulled high with pull up resistor). Output at Port D pin 3.

#include <intrinsics.h>
#include <iostm8s103f3.h>

#define OUTPUT_PIN PD_ODR_ODR3

void init_sysclk() {
    CLK_ICKR = 0; // reset the internal clock register
    CLK_ICKR_HSIEN = 1; // select the high speed internal clock
    CLK_ECKR = 0; // disable external clock register
    while (CLK_ICKR_HSIRDY == 0); // wait for the high speed internal clock to be ready

    CLK_CKDIVR = 0; // maximum speed!
    CLK_PCKENR1 = 0xff; // enable all peripheral clocks (timers, i2c, etc.)
    CLK_PCKENR2 = 0xff; // ^
    CLK_CCOR = 0; // turn off cco
    CLK_HSITRIMR = 0; // maximum speed!
    CLK_SWIMCCR = 0; // swim is at clock/2 speed

    CLK_SWR = 0xe1; // select high speed internal as the master clock
    CLK_SWCR = 0; // clear everything in clock switch register
    CLK_SWCR_SWEN = 1; // start the switch
    while (CLK_SWCR_SWBSY == 1); // wait for switch to be completed
}

void init_gpio() {
    PD_ODR = 0; // turn off all port d outputs
    PD_DDR = 0xff; // set every port d pin to output
    PD_CR1 = 0xff; // push-pull outputs
    PD_CR2 = 0xff; // maximum speed

    PC_ODR = 0; // turn off all port c outputs
    PC_DDR = 0xff; // set every port c pin to output
    PC_CR1 = 0xff; // push-pull outputs
    PC_CR2 = 0xff; // maximum speed

    PC_DDR_DDR3 = 0; // set port c pin 3 to input
    PC_CR1_C13 = 0; // floating input
}

void enable_pc_interrupts() {
    PC_CR2_C23 = 1; // turn port c pin 3 interrupts on
}

void disable_pc_interrupts() {
    PC_CR2_C23 = 0; // turn port c pin 3 interrupts off
}

void init_tim2() {
    TIM2_CR1 = 0; // set timer 2 control register 1 to the reset state
    TIM2_IER = 0; // disable all timer 2 interrupts
    TIM2_PSCR = 0x04; // clock/(2^4) or clock/16 ; when clock is 16000hz, this is 1000hz (one pulse every millisecond)
    TIM2_ARRH = 0x03; // load 998 to auto reset register (should be 1000, but this accounts for time spent executing interrupt)
    TIM2_ARRL = 0xe6; // ^
    TIM2_IER_UIE = 1; // enable update interrupt
    TIM2_CR1_CEN = 1; // start the timer
}

void stop_tim2() {
    TIM2_CR1_CEN = 0; // stop the timer
    TIM2_SR1_UIF = 0; // reset the interrupt flag
}


unsigned long tim2_steps = 0; // incremented by timer 2 overflow interrupt ; counts milliseconds since timer start

#pragma vector = TIM2_OVR_UIF_vector
__interrupt void TIM2_UPD_OVF_IRQHandler(void) {
    tim2_steps ++;
    TIM2_SR1_UIF = 0;
}

void delayms(unsigned long target) {
    disable_pc_interrupts(); // turn off port c interrupts so they don't accumulate while timer is working
    tim2_steps = 0; // reset milliseconds since timer 2 start
    init_tim2(); // start timer 2
    while (tim2_steps < target); // wait for the desired number of milliseconds to pass
    stop_tim2(); // stop timer 2
    enable_pc_interrupts(); // turn on port c interrupts
}

void init_interrupts() {
    EXTI_CR1_PCIS = 2; // port c external interrupts are falling edge only
    EXTI_CR2_TLIS = 0; // external interrupts are falling edge only
    ITC_SPR2_VECT5SPR = 0; // use software priority register to set port c external interrupts to lower priority
                           // (allows timer 2 interrupts to run during the port c external interrupt handler)
}

#pragma vector = EXTI2_vector // EXTI2 is external interrupts for all of port c
__interrupt void EXTI_PORTC_IRQHandler(void) {
    OUTPUT_PIN = !OUTPUT_PIN; // toggle LED
    delayms(200); // disable interrupts and block execution for 200 ms to stop button bounce
}


int main(void) {
    __disable_interrupt();
    init_sysclk();
    init_gpio();
    init_interrupts();
    __enable_interrupt();

    while (1) {
        __wait_for_interrupt(); // infinitely wait for the button to be pressed
    }
}
