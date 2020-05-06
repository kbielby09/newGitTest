
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"


#define RS 1    /* BIT0 mask */
#define RW 2    /* BIT1 mask */
#define EN 4    		/* BIT2 mask */
#define ADCPRIOR 1U		//ADC0 interrupt priority
#define TPMPRIOR 1U		//TPM0 interrupt priority
#define PTDPRIOR 0U		//PTD interrupt priority


//LED prototype functions
void LED_init(void);
void LED_set(int value);

//keypad prototype functions
void keypad_init(void);
//char keypad_getkey(void); TODO delete this function if not used

//prompts for method chosen
//TODO comment these
char CharsDisplayed[] = "\r\nSelect Method:";//array acting as a buffer for displayed digits
char Method1[] = "\r\nMethod 1";
char Method2[] = "\r\nMethod 2";
char Method3[] = "\r\nMethod 3";
char Method4[] = "\r\nMethod 4";
char Method5[] = "\r\nMethod 5";

//void UpdateBuffer(int, int); TODO delete this function if not used

//LCD display function prototype
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void LCD_Write();

//Receive transmit functions for UART0
void UART0_init(void);
void UART0Tx(char c);
void UART0_puts(char* s);

//Mode Select function
int Mode_Select(int);
int mode = 1;

//ADC initialization
void ADC0_init(void);

//PWM functions
void PWM_init(void);

void delayMs(int);

char buffer[20];
uint16_t result;
int PotResult;
int PhotoResult;
int choice = 1;

/*
 * @brief   Application entry point.
 */
int main(void) {


  	/* Init board hardware. */    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    //peripheral initialization
    __disable_irq();
   // keypad_init();
    PWM_init();		//initialize PWM signal
    ADC0_init();	//Initialize ADC
    UART0_init();	//Initialize UART
    //LCD_init();		//Initialize LCD
    __enable_irq();




    //TODO Test receive function of UART might need interrupt
    //TODO get Keypad working with interrupt
    //TODO get program to accept input from terminal through Uart0
    //TODO figure out DC motor
    //TODO test LCD display

    //result = 1621;

    //UART0_puts("THis works\0");
//    choice = 0;
    //ADC0->SC1[0] = 0;

    while(1){
    	choice = 2;
    	ADC0->SC1[0] = 0x43;		//choose potentiometer as input (PTE22)
    }//end while
}

//delay function that used system clock instead of for loop
//void delayMs(int msec){
//	SysTick->LOAD = 48000 - 1;			//set systick for milliseconds
//	SysTick->CTRL = 0x5;						//enable SysTick timer and use system clock
//	for(int i = 0; i < msec; i++){
//		while((SysTick->CTRL & 1 << 16) == 0){}	//wait for clock to stop
//	}
//	SysTick->CTRL = 0;
//}

void delayMs(int n) {
    int i;
    int j;
    for(i = 0 ; i < n; i++)
        for(j = 0 ; j < 3500; j++) { }
}

//event handler for TPM0
//TODO double check if need this event handler
void TPM0_IRQHandler(void) {
	//int voltage;

	if(mode == 1){			//check if mode 1 will be used
		//voltage = result * 3300/4096;		//calculate voltage
//				//display voltage information

		choice = 1;
		ADC0->SC1[0] = 0x40;		//chose photoresistor as intput (PTE20)
//		choice = 2;
//		ADC0->SC1[0] = 0x43;		//choose potentiometer as input (PTE22)

	}//end if

//	TPM0->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK; //Clear the CHF flag for TPM0_CH1
}

//PWM initialization function
void PWM_init(void)
{
    SIM->SCGC5 |= 1 << 11;       		   	   /* enable clock to Port C*/
    PORTC->PCR[2] |= 0x0400;     		   /* PTC2 used by TPM0 */
    SIM->SCGC6 |= 0x01000000;   		   /* enable clock to TPM0 */
    SIM->SOPT2 |= 0x01000000;   		   /* use MCGFLLCLK as timer counter clock */

    TPM0->SC = 0;               		  /* disable timer */
    TPM0->CONTROLS[1].CnSC |= TPM_CnSC_MSB_MASK |TPM_CnSC_ELSB_MASK; //Enable TPM0_CH1 as edge-aligned PWM
    TPM0->CONTROLS[1].CnSC |= 0xC0;		  /* clear CHF and enable Channel Interrupt*/
    TPM0->MOD = 60000;          		  /* Set up modulo register for 50 Hz - 48.00 MHz */
    TPM0->CONTROLS[1].CnV = 1620;  		  /* Set up channel value for 2.7% duty-cycle */
    TPM0->SC |= 0x0C;            		  /* enable TPM0 with pre-scaler /16 */
    NVIC_SetPriority(TPM0_IRQn, TPMPRIOR);	//set TPM0 interrupt priority
    NVIC->ISER[0] |= 1 << 17;		  /*enable IRQ17 (bit 17 of ISER[0])*/
}

//event handler for ADC0
void ADC0_IRQHandler(void){
	int voltage;

	if(mode == 1){			//check if mode 1 will be used
		//voltage = result * 3300/4096;		//calculate voltage
//		sprintf(buffer, "\r\nvoltage = %d mV", voltage); /* convert to string */
//		UART0_puts(buffer);			//display voltage information


		switch(choice){
		case 1:
			PhotoResult = ADC0->R[0];
			break;
		case 2:
			PotResult = ADC0->R[0];
			choice = 1;
			if(PhotoResult < 2483 && PhotoResult > 1614){
				choice = 1;
				TPM0->CONTROLS[1].CnV = (60000 * PhotoResult) / 4096;
			}
			else{
				choice = 1;
				TPM0->CONTROLS[1].CnV = (60000 * PotResult) / 4096;//1620 + PotResult*3/2;  /* Set up channel value between 2.5% and 12.5%*/
			}
			break;
		}//end switch

	}//end if
}//end function

//Port D interrupt handler
//void PORTD_IRQHandler(void){
//	int col, row;
//	const char row_select[] = {0x80, 0x40, 0x02, 0x08};/* one row is active */
//	UART0_puts("In Here\0");
//	// If a key is pressed, it gets here to find out which key.
//	// It activates one row at a time and read the input to see
//	// which column is active. */
//	/* check to see any key pressed */
//	PTD->PDDR |= 0xCA;          /* enable all rows */
//	PTD->PCOR = 0xCA;
//	delayMs(2);                 /* wait for signal return */
//	col = PTD->PDIR & 0x35;     /* read all columns */
//	PTD->PDDR = 0;              /* disable all rows */
//
//	for (row = 0; row < 4; row++)
//	{
//		PTD->PDDR = 0;                  /* disable all rows */
//		PTD->PDDR |= row_select[row];   /* enable one row */
//		PTD->PCOR = row_select[row];    /* drive the active row low */
//		delayMs(2);                     /* wait for signal to settle */
//		col = PTD->PDIR & 0x35;         /* read all columns */
//		if (col != 0x35) break;         /* if one of the input is low, some key is pressed. */
//	}
//
//	PTD->PDDR = 0;                      /* disable all rows */
//	if (row != 4){
//		/* gets here when one of the rows has key pressed, check which column it is */
//		if (col == 0x31) Mode_Select(row * 4 + 1);    /* key in column 0 */
//		if (col == 0x34) Mode_Select(row * 4 + 2);    /* key in column 1 */
//		if (col == 0x15) Mode_Select(row * 4 + 3);    /* key in column 2 */
//		if (col == 0x25) Mode_Select(row * 4 + 4);    /* key in column 3 */
//	}
//
//	PORTD->ISFR = 0;
//}
//
//int Mode_Select(int key){
//	if(key == 1){
//		UART0_puts(Method1);
//	}
//	if(key == 2){
//		UART0_puts(Method2);
//	}
//	if(key == 3){
//		UART0_puts(Method3);
//	}
//	if(key == 5){
//		UART0_puts(Method4);
//	}
//	if(key == 6){
//		UART0_puts(Method5);
//	}
//	if(key == 15){
//		UART0_puts(CharsDisplayed);
//	}
//
//	return 0;
//}

//Initialize keypad functionality
//columns are inputs and interrupts
//TODO update comments
//void keypad_init(void)
//{
//    SIM->SCGC5 |= 1 << 12;       /* enable clock to Port D */
//    //enable pins as GPIO
//    PORTD->PCR[7] |= 0x103;     ///* make PTA17 pin as GPIO and enable pullup
//    PORTD->PCR[6] |= 0x103;     ///* make PTA16 pin as GPIO and enable pullup
//    PORTD->PCR[1] |= 0x103;		///* make PTA13 pin as GPIO and enable pullup
//    PORTD->PCR[3] |= 0x103;     ///* make PTA12 pin as GPIO and enable pullup
//    PORTD->PCR[2] |= 0x103;      ///* make PTA5 pin as GPIO and enable pullup
//    PORTD->PCR[0] |= 0x103;      ///* make PTA4 pin as GPIO and enable pullup
//    PORTD->PCR[5] |= 0x103;      ///* make PTA2 pin as GPIO and enable pullup
//    PORTD->PCR[4] |= 0x103;      ///* make PTA1 pin as GPIO and enable pullup
//
//    PTD->PDDR &= ~0x35;			//make pins 5,4,2,0 as input pins
//
//    //Clear interrupt selections
//    PORTD->PCR[5] &= ~0xF0000;
//    PORTD->PCR[4] &= ~0xF0000;
//    PORTD->PCR[2] &= ~0xF0000;
//    PORTD->PCR[0] &= ~0xF0000;
//
//    //enable falling and rising edge interrupt
//    PORTD->PCR[5] |= 0xB0000;
//    PORTD->PCR[4] |= 0xB0000;
//    PORTD->PCR[2] |= 0xB0000;
//    PORTD->PCR[0] |= 0xB0000;
//
//	  NVIC_SetPriority(PORTD_IRQn, PTDPRIOR);	//set interrupt priority for PORTD
//    NVIC->ISER[0] |= 1 << 31;	//enable interrupt for PORTD
//}

/* initialize UART0 to transmit at 115200 Baud */
void UART0_init(void) {
    SIM->SCGC4 |= 0x0400;   	/* enable clock for UART0 */
    SIM->SOPT2 |= 1 << 26;		/* use FLL output for UART Baud rate generator */
    UART0->C2 = 0;				/* turn off UART0 while changing configurations */
    UART0->BDH = 0x00;
    UART0->BDL = 0x1A;			/* 115200 Baud */
    UART0->C4 = 0x0F;       	/* Over Sampling Ratio 16 */
    UART0->C1 = 0x00;       	/* 8-bit data */
    UART0->C2 = 0x08 | 1 << 2;	//enable transmit and receive

    SIM->SCGC5 |= 0x0200;   	//enable clock for PORTA */
    PORTA->PCR[2] |= 0x0200; 	//make PTA2 UART0_Tx pin */
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

//TODO double check these values and comment this function
void ADC0_init(void)
{
	uint16_t calibration;

    SIM->SCGC5 |= 0x2000;       /* clock to PORTE */
    PORTE->PCR[20] = 0;         /* PTE20 analog input */
    PORTE->PCR[22] = 0;			//PTE22 as analog input

    SIM->SCGC6 |= 0x8000000;    /* clock to ADC0 */
    ADC0->SC2 &= ~0x40;         /* software trigger */
    /* clock div by 4, long sample time, single ended 12 bit, bus clock */
    ADC0->CFG1 = 0x40 | 0x10 | 0x04 | 0x00;
    ADC0->SC1[0] |= 1 << 6;		/*enable ADC0 interrupt*/
    //Start Calibration
    ADC0->SC3 |= ADC_SC3_CAL_MASK;
	while (ADC0->SC3 & ADC_SC3_CAL_MASK) {
	// Wait for calibration to complete
	}
	// Finish off the calibration
	// Initialize a 16-bit variable in RAM
	calibration = 0x0;
	// Add the plus-side calibration results to the variable
	calibration += ADC0->CLP0;
	calibration += ADC0->CLP1;
	calibration += ADC0->CLP2;
	calibration += ADC0->CLP3;
	calibration += ADC0->CLP4;
	calibration += ADC0->CLPS;
	// Divide by two
	calibration /= 2;
	// Set the MSB of the variable
	calibration |= 0x8000;
	// Store the value in the plus-side gain calibration register
	ADC0->PG = calibration;
	// Repeat the procedure for the minus-side calibration value
	calibration = 0x0000;
	calibration += ADC0->CLM0;
	calibration += ADC0->CLM1;
	calibration += ADC0->CLM2;
	calibration += ADC0->CLM3;
	calibration += ADC0->CLM4;
	calibration += ADC0->CLMS;
	calibration /= 2;
	calibration |= 0x8000;
	ADC0->MG = calibration;
    //Done Calibration

	/* Reconfigure ADC0*/
    /* clock div by 4, long sample time, single ended 12 bit, bus clock */
    ADC0->CFG1 = 0x40 | 0x10 | 0x04 | 0x00;
    NVIC_SetPriority(ADC0_IRQn, ADCPRIOR);		//set ADC0 interrupt priority
    NVIC->ISER[0] |= 1<<15;//0x8000;		  /*enable IRQ15 (bit 15 of ISER[0])(ADC)*/

}



//
//void LCD_Write(){
//	LCD_command(1);         /* clear display */
//	delayMs(500);
//	LCD_command(0x85);      /* set cursor at first line */
//
//	for(int i = 0; i < 9; i++){
//		LCD_data(CharsDisplayed[i]);
//	}
//}


//TODO modify this code and change comments
//void LCD_init(void)
//{
//    SIM->SCGC5 |= 1 <<11;       /* enable clock to Port D *///TODO change this
//    PORTC->PCR[10] = 0x100;      /* make PTD0 pin as GPIO */
//    PORTC->PCR[11] = 0x100;      /* make PTD1 pin as GPIO */
//    PORTC->PCR[12] = 0x100;      /* make PTD2 pin as GPIO */
//    PORTC->PCR[13] = 0x100;      /* make PTD4 pin as GPIO */
//    PORTC->PCR[16] = 0x100;      /* make PTD6 pin as GPIO */
//    PORTC->PCR[17] = 0x100;      /* make PTD7 pin as GPIO */
//    PTC->PDDR |= 0x33C00;          /* make PTD7-4, 2, 1, 0 as output pins *///TODO Change this
//
//    delayMs(30);                /* initialization sequence */
//    LCD_nibble_write(0x30, 0);
//    delayMs(10);
//    LCD_nibble_write(0x30, 0);
//    delayMs(1);
//    LCD_nibble_write(0x30, 0);
//    delayMs(1);
//    LCD_nibble_write(0x20, 0);  /* use 4-bit data mode */
//    delayMs(1);
//
//    LCD_command(0x28);          /* set 4-bit data, 2-line, 5x7 font */
//    LCD_command(0x06);          /* move cursor right */
//    LCD_command(0x01);          /* clear screen, move cursor to home */
//    LCD_command(0x0F);          /* turn on display, cursor blinking */
//}
//
//
////void LCD_init(void)
////{
////    SIM->SCGC5 |= 0x1000;       /* enable clock to Port D *///TODO change this
////    PORTC->PCR[0] = 0x100;      /* make PTD0 pin as GPIO */
////    PORTC->PCR[1] = 0x100;      /* make PTD1 pin as GPIO */
////    PORTC->PCR[2] = 0x100;      /* make PTD2 pin as GPIO */
////    PORTC->PCR[4] = 0x100;      /* make PTD4 pin as GPIO */
////    PORTC->PCR[5] = 0x100;      /* make PTD5 pin as GPIO */
////    PORTC->PCR[6] = 0x100;      /* make PTD6 pin as GPIO */
////    PORTC->PCR[7] = 0x100;      /* make PTD7 pin as GPIO */
////    PTC->PDDR |= 0xF7;          /* make PTD7-4, 2, 1, 0 as output pins *///TODO Change this
////
////    delayMs(30);                /* initialization sequence */
////    LCD_nibble_write(0x30, 0);
////    delayMs(10);
////    LCD_nibble_write(0x30, 0);
////    delayMs(1);
////    LCD_nibble_write(0x30, 0);
////    delayMs(1);
////    LCD_nibble_write(0x20, 0);  /* use 4-bit data mode */
////    delayMs(1);
////
////    LCD_command(0x28);          /* set 4-bit data, 2-line, 5x7 font */
////    LCD_command(0x06);          /* move cursor right */
////    LCD_command(0x01);          /* clear screen, move cursor to home */
////    LCD_command(0x0F);          /* turn on display, cursor blinking */
////}
//
//void LCD_nibble_write(unsigned char data, unsigned char control)
//{
//    data &= 0xF0;       /* clear lower nibble for control */
//    control &= 0x0F;    /* clear upper nibble for data */
//    PTC->PDOR = (data | control) << 6;       /* RS = 0, R/W = 0 */
//    PTC->PDOR = (data | control | EN) << 6;  /* pulse E */
//    delayMs(0);
//    PTC->PDOR = data << 6;
//    PTC->PDOR = 0;
//}
//
//void LCD_command(unsigned char command)
//{
//    LCD_nibble_write(command & 0xF0, 0);   /* upper nibble first */
//    LCD_nibble_write(command << 4, 0);     /* then lower nibble */
//
//    if (command < 4)
//        delayMs(4);         /* commands 1 and 2 need up to 1.64ms */
//    else
//        delayMs(1);         /* all others 40 us */
//}
//
//void LCD_data(unsigned char data)
//{
//    LCD_nibble_write(data & 0xF0, RS);    /* upper nibble first */
//    LCD_nibble_write(data << 4, RS);      /* then lower nibble  */
//
//    delayMs(1);
//}
