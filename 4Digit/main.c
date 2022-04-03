// register select at PD3. read/write at PD2. enable at PD1. data pins [0:4] at PC7-3. data pins [5:6] at PB4-5. data pin 7 at PA3.

#include <intrinsics.h>
#include <iostm8s103f3.h>

#define D1 PB_ODR_ODR5
#define D2 PB_ODR_ODR4
#define D3 PC_ODR_ODR3
#define D4 PC_ODR_ODR4
#define OA PD_ODR_ODR3
#define OB PD_ODR_ODR1
#define OC PA_ODR_ODR2
#define OD PC_ODR_ODR6
#define OE PC_ODR_ODR7
#define OF PD_ODR_ODR2
#define OG PA_ODR_ODR3
#define ODP PC_ODR_ODR5


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

    PB_ODR = 0; // turn off all port b outputs
    PB_DDR = 0xff; // set every port b pin to output
    PB_CR1 = 0xff; // push-pull outputs
    PB_CR2 = 0xff; // maximum speed

    PA_ODR = 0; // turn off all port a outputs
    PA_DDR = 0xff; // set every port a pin to output
    PA_CR1 = 0xff; // push-pull outputs
    PA_CR2 = 0xff; // maximum speed
}

void init_tim2() {
    TIM2_CR1 = 0; // set timer 2 control register 1 to the reset state
    TIM2_IER = 0; // disable all timer 2 interrupts
    TIM2_PSCR = 0x04; // clock/(2^4) or clock/16 ; when clock is 16000hz, this is 1000hz (one pulse every millisecond)
    TIM2_ARRH = 0x03; // load 1000 to auto reset register
    TIM2_ARRL = 0xe8; // ^
    TIM2_IER_UIE = 1; // enable update interrupt
    TIM2_CR1_CEN = 1; // start the timer
}

unsigned long tim2_steps = 0; // incremented by timer 2 overflow interrupt ; counts milliseconds since timer start

#pragma vector = TIM2_OVR_UIF_vector // TIM2_OVR_UIF is the vector for timer 2 update overflow interrupt
__interrupt void TIM2_UPD_OVF_IRQHandler(void) {
    tim2_steps ++;
    TIM2_SR1_UIF = 0; // reset timer 2 interrupt
}

void delayms(unsigned long target) {
    unsigned long future_target = tim2_steps+target;
    while (tim2_steps < future_target); // wait for the desired number of milliseconds to pass
}


void display_digit(unsigned int digit, unsigned int place) {
    D1 = 1;
    D2 = 1;
    D3 = 1;
    D4 = 1;

    switch (place) {
        case 1:
            D1 = 0;
            break;
        case 2:
            D2 = 0;
            break;
        case 3:
            D3 = 0;
            break;
        case 4:
            D4 = 0;
            break;
        default:
            break;
    }
    

    switch (digit) {
        case 0:
            ODP = 0;
            OG = 0;

            OA = 1;
            OB = 1;
            OC = 1;
            OD = 1;
            OE = 1;
            OF = 1;
            break;
        case 1:
            OA = 0;
            OB = 1;
            OC = 1;
            OD = 0;
            OE = 0;
            OF = 0;
            OG = 0;
            ODP = 0;
            break;
        case 2:
            OA = 1;
            OB = 1;
            OC = 0;
            OD = 1;
            OE = 1;
            OF = 0;
            OG = 1;
            ODP = 0;
            break;
        case 3:
            OA = 1;
            OB = 1;
            OC = 1;
            OD = 1;
            OE = 0;
            OF = 0;
            OG = 1;
            ODP = 0;
            break;
        case 4:
            OA = 0;
            OB = 1;
            OC = 1;
            OD = 0;
            OE = 0;
            OF = 1;
            OG = 1;
            ODP = 0;
            break;
        case 5:
            OA = 1;
            OB = 0;
            OC = 1;
            OD = 1;
            OE = 0;
            OF = 1;
            OG = 1;
            ODP = 0;
            break;
        case 6:
            OA = 1;
            OB = 0;
            OC = 1;
            OD = 1;
            OE = 1;
            OF = 1;
            OG = 1;
            ODP = 0;
            break;
        case 7:
            OA = 1;
            OB = 1;
            OC = 1;
            OD = 0;
            OE = 0;
            OF = 1;
            OG = 0;
            ODP = 0;
            break;
        case 8:
            OA = 1;
            OB = 1;
            OC = 1;
            OD = 1;
            OE = 1;
            OF = 1;
            OG = 1;
            ODP = 0;
            break;
        case 9:
            OA = 1;
            OB = 1;
            OC = 1;
            OD = 1;
            OE = 0;
            OF = 1;
            OG = 1;
            ODP = 0;
            break;
        default:
            break;
    }
}

void display_num(unsigned int num) {
    unsigned int remainder;
    unsigned int i = 0;
    unsigned int num_cpy = num;
    if (num == 0) {
        display_digit(0,1);
    }
    num_cpy = num;
    while (num_cpy != 0) {
        i++;
        remainder = num_cpy % 10;
        display_digit(remainder,i);
        delayms(5);
        num_cpy = num_cpy / 10;
    }
}

void count(unsigned int count_to) {
    for (int i = 0; i < count_to; i++) {
        display_num(i);
    }
}


int main(void) {
    __disable_interrupt();
    init_sysclk();
    init_gpio();
    init_tim2();
    __enable_interrupt();

    while (1) {
        display_num(tim2_steps/200);
    }
}
