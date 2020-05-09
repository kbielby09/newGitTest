/* p6_4.c UART0 Receive using interrupt (modified) */
#include <MKL25Z4.H>
#include<stdio.h>
void UART0_init(void);
void UART0Tx(char c);
void UART0_puts(char* s);;
void delayMs(int n);
char buffer[2][8] = {"Hello\r\n", "Howdy\r\n"};
int mode;
int number[2]={0,0};

int i=0;

int main (void) {
    __disable_irq();        /* global disable IRQs */
    UART0_init();
    __enable_irq();         /* global enable IRQs */
    mode = 0;

    while (1) {
        /* UART0 receive is moved to UART0 interrupt handler*/
    	UART0_puts(buffer[0]);
    	UART0_puts(buffer[1]);

    	delayMs(1000);
    }
}

/* UART0 interrupt handler */
void UART0_IRQHandler(void) {
    char c;
    c = UART0->D;           /* read the char received */
    buffer[mode][i%5] = c;
    number[mode] = number[mode]*10 + c - 0x30;
    i++;
}

/* initialize UART0 to receive at 115200 Baud */
void UART0_init(void) {
    SIM->SCGC4 |= 0x0400;    /* enable clock for UART0 */
    SIM->SOPT2 |= 0x04000000;   /* use FLL output for UART Baud rate generator */
    UART0->C2 = 0;          /* turn off UART0 while changing configurations */
    UART0->BDH = 0x00;
    UART0->BDL = 0x0C;      /* 115200 Baud - Using 20.9715 MHz clock*/
    UART0->C4 = 0x0F;       /* Over Sampling Ratio 16 */
    UART0->C1 = 0x00;       /* 8-bit data */
    UART0->C2 = 0x2C;       /* enable receive, transmit and receive interrupt*/
    NVIC->ISER[0] |= 0x00001000;    /* enable INT12 (bit 12 of ISER[0]) */
    SIM->SCGC5 |= 0x0200;    /* enable clock for PORTA */
    PORTA->PCR[1] = 0x0200; /* make PTA1 UART0_Rx pin */
    PORTA->PCR[2] = 0x0200; /* make PTA1 UART0_Tx pin */
}

void UART0Tx(char c) {
    while(!(UART0->S1 & 0x80)) {
    }   /* wait for transmit buffer empty */
    UART0->D = c; /* send a char */
}

void UART0_puts(char* s) {
    while (*s != 0)         /* if not end of string */
        UART0Tx(*s++);      /* send the character through UART0 */
}

/* Delay n milliseconds
 * The CPU core clock is set to MCGFLLCLK at 41.94 MHz in SystemInit().
 */
void delayMs(int n) {
    int i;
    int j;
    for(i = 0 ; i < n; i++)
        for (j = 0; j < 3500; j++) {}
}

