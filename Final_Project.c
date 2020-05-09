
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"

#define RS 1    		/* BIT0 mask */
#define mod 44999 		// 14999
#define RW 2    		/* BIT1 mask */
#define EN 0x20000		/* BIT2 mask *///TODO change this value
#define ADCPRIOR 2U		//ADC0 interrupt priority
#define TPMPRIOR 2U		//TPM0 interrupt priority
#define PTDPRIOR 0U		//PTD interrupt priority
#define UART0PRIOR 1U	//UART0 interrupt priority

//LED prototype functions
void LED_init(void);
void LED_set(int value);
void UltraLED_init(void);

//keypad prototype functions
void keypad_init(void);
//char keypad_getkey(void);

//prompts for method chosen
char CharsDisplayed[] = "\r\nSelect Method:";	//array acting as a buffer for displayed digits
char Method1[] = "\r\nMethod 1";				//prompt informing user that Mehtod 1 has been chosen
char Method2[] = "\r\nMethod 2";				//prompt informing user that Mehtod 2 has been chosen
char Method3[] = "\r\nMethod 3";				//prompt informing user that Mehtod 3 has been chosen
char Method4[] = "\r\nMethod 4";				//prompt informing user that Mehtod 4 has been chosen
char Method5[] = "\r\nMethod 5";				//prompt informing user that Mehtod 5 has been chosen

//Method 2 Speed prompt section
void Method2_Speed_Prompt(void);

//LCD display function prototype
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void LCD_Write(char*);

//initialization code for Ultrasonic Sensor
void Trigger_Timer1_init(void);
void Capture_Timer2_init(void);

//Receive transmit functions for UART0
void UART0_init(void);
void UART0Tx(char c);
void UART0_puts(char*);

//Mode Select function
void Mode_Select(int);

//ADC initialization
void ADC0_init(void);

//PWM functions
void PWM_init(void);

void delayMs(int);

//variables and arrays used by program
char buffer[20];
uint16_t result;
int PotResult;
int PhotoResult;
int choice = 1;
volatile uint32_t pulse_width;
volatile float distance;
int tmpInt1;
float tmpFrac;
int tmpInt2;
volatile uint32_t cont[2];
//short int result;
//char buffer[30];
int speed = 4;		//Scan speed of servo for mode 2
int Servo_Bound = 1;
double percentage = 2.7;
char nextChar;
int mode = 3;
int number[2]={0,0};
//char buffer[2][8] = {"Hello\r\n", "Howdy\r\n"};
//int i = 0;
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
    keypad_init();
//    UltraLED_init();	//initialize LEDs for ultrasonic sensor in Mode 2
//    Trigger_Timer1_init();                     	/* Configure PWM */
//    Capture_Timer2_init();
    PWM_init();		//initialize PWM signal
//    ADC0_init();	//Initialize ADC
    UART0_init();	//Initialize UART
//    LCD_init();		//Initialize LCD
    __enable_irq();

    //result = 1621;

//    choice = 0;
    //ADC0->SC1[0] = 0;
    //LCD_Write(Method1);
    //Mode_Select(2);

    while(1){
//    	if(mode == 1){
//    		choice = 2;
//    		ADC0->SC1[0] = 0x43;		//choose potentiometer as input (PTE22)
//    	}
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

//LED initialization for sensor
//void UltraLED_init(void){
//	 SIM->SCGC5 |= 0x400;        /* enable clock to Port B */
//	 PORTB->PCR[1] = 0x100;     /* make PTB1 pin as GPIO */
//	 PTB->PSOR |= 0x02;       /* Set PTB1 to turn off blue LED */
//	 PTB->PDDR |= 0x02;       /* make PTB1 as output pin */
//
//	 PORTB->PCR[2] = 0x100;     /* make PTB2 pin as GPIO */
//	 PTB->PSOR |= 0x04;       /* Set PTB2 to turn off red LED */
//	 PTB->PDDR |= 0x04;       /* make PTB2 as output pin */
//}
//
////Timer trigger initialization
//void Trigger_Timer1_init(void)
//{
//    SIM->SCGC5 |= 0x0400;       /* enable clock to Port B*/
//    SIM->SCGC6 |= 0x02000000;   /* enable clock to TPM1 */
//    SIM->SOPT2 |= 0x01000000;   /* use MCGFLLCLK as timer counter clock */
//    PORTB->PCR[0] = 0x0300;     /* PTB0 used by TPM1_CH0 */
//
//    TPM1->SC = 0;               /* disable timer */
//    TPM1->CONTROLS[0].CnSC  = 0x80;   	  /* clear CHF  for Channel 0*/
//    TPM1->CONTROLS[0].CnSC |= 0x20|0x08;  /* edge-aligned, pulse high MSB:MSA=10, ELSB:ELSA=10*/
//    TPM1->CONTROLS[0].CnV   = 8;  		  /* Set up channel value for >10 us*/
//	TPM1->SC |= 0x06;           		  /* set timer with prescaler /64 */
//    TPM1->MOD = mod;            		  /* Set up modulo register = 44999 */
////*************************PRE-Scaler settings **************************
//	TPM1->SC |= 0x08;           	      /* enable timer */
////***********************************************************************
//}


//void Capture_Timer2_init(void)  // Also enables the TPM2_CH1 interrupt
//{
//    SIM->SCGC5 |= 0x2000;       /* enable clock to Port E*/
//    SIM->SCGC6 |= 0x04000000;   /* enable clock to TPM2 */
//    PORTE->PCR[23] = 0x0300;    /* PTE23 used by TPM2_CH1 */
//    SIM->SOPT2 |= 0x01000000;   /* use MCGFLLCLK as timer counter clock */
//    TPM2->SC = 0;               /* disable timer */
//
//    TPM2->CONTROLS[1].CnSC = 0x80;   	/* clear CHF  for Channel 1*/
////  MSB:MSA=00, ELSB:ELSA=11 and set interrupt*/
///*  capture on both edges, MSB:MSA=00, ELSB:ELSA=11 and set interrupt*/
//    TPM2->CONTROLS[1].CnSC |= TPM_CnSC_CHIE_MASK|TPM_CnSC_ELSB_MASK|TPM_CnSC_ELSA_MASK;
//    TPM2->MOD = mod;            		  /* Set up modulo register = 44999*/
//    TPM2->CONTROLS[1].CnV = (mod+1)/2 -1; /* Set up 50% dutycycle */
//
//	TPM2->SC |= 0x80;           /* clear TOF */
//	TPM2->SC |= 0x06;           /* enable timer with prescaler /2^6 = 64 */
////*************************PRE-Scaler settings *********************************************
//	TPM2->SC |= 0x08;           /* enable timer             */
////******************************************************************************************
//    NVIC_EnableIRQ(TPM2_IRQn);  /* enable Timer2 interrupt in NVIC */
//}
//
//
//void TPM2_IRQHandler(void) {
//	while(!(TPM2->CONTROLS[1].CnSC & 0x80)) { } /* wait until the CHF is set */
//	cont[i%2] = TPM2->CONTROLS[1].CnV;
//	if(i%2 == 1){
//
//		if(cont[1] > cont[0] ){
//			pulse_width = cont[1] - cont[0];
//		}
//		    else {
//		    pulse_width = cont[1] - cont[0] + mod + 1;
//		}
//
//	//	sprintf(buffer, "Pulse width %d \r\n", pulse_width); /* convert to string */
//		UART0_puts(buffer);
//		distance = (float)(pulse_width*2/3)*0.0343;
//
//		tmpInt1 = distance;
//		tmpFrac = distance - tmpInt1;
//	    tmpInt2 = (tmpFrac * 10000);
//
//	 //   sprintf(buffer, "Distance %d.%04d cm\r\n", tmpInt1, tmpInt2); /* convert to string */
//	    UART0_puts(buffer);
//
//		distance = (float)(distance/2.54);
//
//		tmpInt1 = distance;
//		tmpFrac = distance - tmpInt1;
//		tmpInt2 = (tmpFrac * 10000);
//
//	  //  sprintf(buffer, "Distance %d.%04d inches\r\n", tmpInt1, tmpInt2); /* convert to string */
//	    UART0_puts(buffer);
//
//    	if(distance < (float)12.0)
//    	{
//    	    PTB->PCOR |= 0x02;       /* Clear PTB1 to turn on red LED */
//    	    PTB->PSOR |= 0x04;       /* Set PTB2 to turn off blue LED */
//    	}
//    	else if(distance > (float)12.0 && distance < (float)24.0)
//    	{
//    		PTB->PCOR |= 0x04;		 /* Clear PTB2 to turn on blue LED */
//    		PTB->PSOR |= 0x02;       /* Set PTB1 to turn off red LED */
//    	}
//    	else
//    	{
//    	    PTB->PSOR |= 0x02;       /* Set PTB1 to turn off red LED */
//    		PTB->PSOR |= 0x04;       /* Set PTB2 to turn off blue LED */
//    	}
//	}
//	i++;
///*---------------------------------------------------------------------*/
//    TPM2->CONTROLS[1].CnSC |= 0x80;    /* clear CHF */
//}

//Prompt user for speed
void Method2_Speed_Prompt(void){
	sprintf(buffer, "\r\nSelect Speed:");
	UART0_puts(buffer);
	sprintf(buffer, "\r\n1. %d%% per 1/2 second", 1);
	UART0_puts(buffer);
	sprintf(buffer, "\r\n2. %d%% per 1/4 second", 1);
	UART0_puts(buffer);
	sprintf(buffer, "\r\n3. %d%% per 1/8 second", 1);
	UART0_puts(buffer);
	sprintf(buffer, "\r\n4. %d%% per 1/16 second", 1);
	UART0_puts(buffer);
	sprintf(buffer,"\r\nEnter Number of Selection: ");
	UART0_puts(buffer);
}

//event handler for TPM0
void TPM0_IRQHandler(void) {
	if(mode == 1){			//check if mode 1 will be used

		choice = 1;
		ADC0->SC1[0] = 0x40;		//chose photoresistor as intput (PTE20)
	}//end if
//	else if(mode == 4){
//
//	}
//	else if(mode == 5){
//
//	}
	else if(mode == 3){
		if(speed == 1){
			if(Servo_Bound == 1){		//check if servo motor has reached +90 degrees and decrement duty cycle
				delayMs(500);
				percentage--;
				if(percentage <= 2.7){
					Servo_Bound = 0;
				}
				TPM0->CONTROLS[1].CnV = 60000 * percentage / 100;

				TPM0->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;		//clear channel flag
			}
			else if(Servo_Bound == 0){	//check if servo motor has reached -90 degrees and increment duty cycle
				delayMs(500);
				percentage++;
				if(percentage >= 12.7){
					Servo_Bound = 1;
				}
				TPM0->CONTROLS[1].CnV = 60000 * percentage / 100;

				TPM0->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;		//clear channel flag
			}
		}
		else if(speed == 2){
			if(Servo_Bound == 1){		//check if servo motor has reached +90 degrees and decrement duty cycle
				delayMs(250);
				percentage--;
				if(percentage <= 2.7){
					Servo_Bound = 0;
				}
				TPM0->CONTROLS[1].CnV = 60000 * percentage / 100;

				TPM0->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;		//clear channel flag
			}
			else if(Servo_Bound == 0){	//check if servo motor has reached -90 degrees and increment duty cycle
				delayMs(250);
				percentage++;
				if(percentage >= 12.7){
					Servo_Bound = 1;
				}
				TPM0->CONTROLS[1].CnV = 60000 * percentage / 100;

				TPM0->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;		//clear channel flag
			}
		}
		else if(speed == 3){
			if(Servo_Bound == 1){		//check if servo motor has reached +90 degrees and decrement duty cycle
				delayMs(125);
				percentage--;
				if(percentage <= 2.7){
					Servo_Bound = 0;
				}
				TPM0->CONTROLS[1].CnV = 60000 * percentage / 100;

				TPM0->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;		//clear channel flag
			}
			else if(Servo_Bound == 0){	//check if servo motor has reached -90 degrees and increment duty cycle
				delayMs(125);
				percentage++;
				if(percentage >= 12.7){
					Servo_Bound = 1;
				}
				TPM0->CONTROLS[1].CnV = 60000 * percentage / 100;

				TPM0->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;		//clear channel flag
			}
		}
		else if(speed == 4){
			if(Servo_Bound == 1){		//check if servo motor has reached +90 degrees and decrement duty cycle
				delayMs(63);
				percentage--;
				if(percentage <= 2.7){
					Servo_Bound = 0;
				}
				TPM0->CONTROLS[1].CnV = 60000 * percentage / 100;

				TPM0->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;		//clear channel flag
			}
			else if(Servo_Bound == 0){	//check if servo motor has reached -90 degrees and increment duty cycle
				delayMs(63);
				percentage++;
				if(percentage >= 12.7){
					Servo_Bound = 1;
				}
				TPM0->CONTROLS[1].CnV = 60000 * percentage / 100;

				TPM0->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;		//clear channel flag
			}
		}
	}

}//end function

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
    NVIC_SetPriority(TPM0_IRQn, TPMPRIOR);	//set UART0 interrupt priority
    NVIC->ISER[0] |= 1 << 17;		  /*enable IRQ12 (bit 17 of ISER[0])*/
}

//event handler for ADC0
//void ADC0_IRQHandler(void){
//	int voltage;
//
//	if(mode == 1){			//check if mode 1 will be used
//
//		switch(choice){
//		case 1:
//			PhotoResult = ADC0->R[0];
//			break;
//		case 2:
//			PotResult = ADC0->R[0];
//			choice = 1;
//			if(PhotoResult < 2483 && PhotoResult > 1614){
//				choice = 1;
//				TPM0->CONTROLS[1].CnV = (60000 * PhotoResult) / 4096;
//			}
//			else{
//				choice = 1;
//				TPM0->CONTROLS[1].CnV = (60000 * PotResult) / 4096;  /* Set up channel value between 2.5% and 12.5%*/
//			}
//			break;
//		}//end switch
//
//	}//end if
//}//end function

//Port D interrupt handler
void PORTD_IRQHandler(void){
	int col, row;
	const char row_select[] = {0x80, 0x40, 0x02, 0x08};/* one row is active */
//	sprintf(buffer, "\r\nIn Here");
//	UART0_puts(buffer);

	/* check to see any key pressed */
	PTD->PDDR |= 0xCA;          /* enable all rows */
	PTD->PCOR = 0xCA;
	delayMs(2);                 /* wait for signal return */
	col = PTD->PDIR & 0x35;     /* read all columns */
	PTD->PDDR = 0;              /* disable all rows */

	for (row = 0; row < 4; row++)
	{
		PTD->PDDR = 0;                  /* disable all rows */
		PTD->PDDR |= row_select[row];   /* enable one row */
		PTD->PCOR = row_select[row];    /* drive the active row low */
		delayMs(2);                     /* wait for signal to settle */
		col = PTD->PDIR & 0x35;         /* read all columns */
		if (col != 0x35) break;         /* if one of the input is low, some key is pressed. */
	}

//	PTD->PDDR = 0;                      /* disable all rows */
	if (row != 4){
		/* gets here when one of the rows has key pressed, check which column it is */
		if (col == 0x31){
			Mode_Select(row * 4 + 1);   /* key in column 0 */
			PORTD->ISFR = ~0x0;			//Clear interrupt flag
		}
		if (col == 0x34){
			Mode_Select(row * 4 + 2);   /* key in column 1 */
			PORTD->ISFR = ~0x0;			//Clear interrupt flag
		}
		if (col == 0x15){
			Mode_Select(row * 4 + 3);   /* key in column 2 */
			PORTD->ISFR = ~0x0;			//Clear interrupt flag
		}
	}

}

//Initialize keypad functionality
//columns are inputs and interrupts
void keypad_init(void)
{
    SIM->SCGC5 |= 1 << 12;       /* enable clock to Port D */
    //enable pins as GPIO
    PORTD->PCR[7] |= 0x103;     ///* make PTA17 pin as GPIO and enable pullup
    PORTD->PCR[6] |= 0x103;     ///* make PTA16 pin as GPIO and enable pullup
    PORTD->PCR[1] |= 0x103;		///* make PTA13 pin as GPIO and enable pullup
    PORTD->PCR[3] |= 0x103;     ///* make PTA12 pin as GPIO and enable pullup
    PORTD->PCR[2] |= 0x103;      ///* make PTA5 pin as GPIO and enable pullup
    PORTD->PCR[0] |= 0x103;      ///* make PTA4 pin as GPIO and enable pullup
    PORTD->PCR[5] |= 0x103;      ///* make PTA2 pin as GPIO and enable pullup
    PORTD->PCR[4] |= 0x103;      ///* make PTA1 pin as GPIO and enable pullup

    PTD->PDDR = 0xCA;//0x35;	 //make pins 5,4,2,0 as input pins

    //Clear interrupt selections
    PORTD->PCR[5] &= ~0xF0000;
    PORTD->PCR[4] &= ~0xF0000;
    PORTD->PCR[2] &= ~0xF0000;
    PORTD->PCR[0] &= ~0xF0000;

    //enable falling and rising edge interrupt
    PORTD->PCR[5] |= 0xA0000;
    PORTD->PCR[4] |= 0xA0000;
    PORTD->PCR[2] |= 0xA0000;
    PORTD->PCR[0] |= 0xA0000;

	NVIC_SetPriority(PORTD_IRQn, PTDPRIOR);		//set interrupt priority for PORTDj as highest priority
    NVIC->ISER[0] |= 1 << 31;	//enable interrupt for PORTD
}

void Mode_Select(int key){
	if(key == 1){
		sprintf(buffer, "\r\nMethod %d", 1);
		UART0_puts(buffer);
		delayMs(500);
		mode = 1;
	}
	if(key == 2){
		sprintf(buffer, "\r\nMethod %d", 2);
		UART0_puts(buffer);
		delayMs(500);
		mode = 2;
	}
	if(key == 3){
		sprintf(buffer, "\r\nMethod %d", 3);
		UART0_puts(buffer);
		Method2_Speed_Prompt();
		mode = 3;
	}
	if(key == 5){
		UART0_puts(Method4);
		mode = 5;
	}
	if(key == 6){
		UART0_puts(Method5);
		mode = 5;
	}
	if(key == 15){
		UART0_puts(CharsDisplayed);
	}

}

//interrupt handler for UART0
void UART0_IRQHandler(void){
	char c;
	c = UART0->D;
	switch(c){
	case '1':
		speed = 1;
		break;
	case '2':
		speed = 2;
		break;
	case '3':
		speed = 3;
		break;
	case '4':
		speed = 4;
		break;
	default:
		sprintf(buffer, "\r\nInvalid Entry");
		UART0_puts(buffer);
		break;
	}//end switch
}


///* initialize UART0 to transmit at 115200 Baud */
void UART0_init(void) {
    SIM->SCGC4 |= 0x0400;   					/* enable clock for UART0 */
    SIM->SOPT2 |= 1 << 26;						/* use FLL output for UART Baud rate generator */
    UART0->C2 = 0;								/* turn off UART0 while changing configurations */
    UART0->BDH = 0x00;
    UART0->BDL = 0x1A;							/* 115200 Baud */
    UART0->C4 = 0x0F;       					/* Over Sampling Ratio 16 */
    UART0->C1 = 0x00;       					/* 8-bit data */
    UART0->C2 = 0x2C;			//enable transmit and receive and receive interrupt

    NVIC_SetPriority(UART0_IRQn, UART0PRIOR);	//set UART0 interrupt priority
    NVIC->ISER[0] |= 1<<12;						/*enable IRQ15 (bit 12 of ISER[0])(ADC)*/
    SIM->SCGC5 |= 0x0200;   					//enable clock for PORTA */
    PORTA->PCR[2] |= 0x0200; 					//make PTA2 UART0_Tx pin */
    PORTA->PCR[1] |= 1 << 9;					//mate PTA1 UART0_Rx pin
}

void UART0Tx(char c) {
    while(!(UART0->S1 & 0x80)) {
    }   /* wait for transmit buffer empty */
    UART0->D = c; /* send a char */
}

void UART0_puts(char* s) {
    while (*s != 0){         /* if not end of string */
//        nextChar = *s++;
    	UART0Tx(*s++);      /* send the character through UART0 */
    }
}
//
////ADC0 initialization function
//void ADC0_init(void)
//{
//	uint16_t calibration;
//
//    SIM->SCGC5 |= 0x2000;       /* clock to PORTE */
//    PORTE->PCR[20] = 0;         /* PTE20 analog input */
//    PORTE->PCR[22] = 0;			//PTE22 as analog input
//
//    SIM->SCGC6 |= 0x8000000;    /* clock to ADC0 */
//    ADC0->SC2 &= ~0x40;         /* software trigger */
//    /* clock div by 4, long sample time, single ended 12 bit, bus clock */
//    ADC0->CFG1 = 0x40 | 0x10 | 0x04 | 0x00;
//    ADC0->SC1[0] |= 1 << 6;		/*enable ADC0 interrupt*/
//    //Start Calibration
//    ADC0->SC3 |= ADC_SC3_CAL_MASK;
//	while (ADC0->SC3 & ADC_SC3_CAL_MASK) {
//	// Wait for calibration to complete
//	}
//	// Finish off the calibration
//	// Initialize a 16-bit variable in RAM
//	calibration = 0x0;
//	// Add the plus-side calibration results to the variable
//	calibration += ADC0->CLP0;
//	calibration += ADC0->CLP1;
//	calibration += ADC0->CLP2;
//	calibration += ADC0->CLP3;
//	calibration += ADC0->CLP4;
//	calibration += ADC0->CLPS;
//	// Divide by two
//	calibration /= 2;
//	// Set the MSB of the variable
//	calibration |= 0x8000;
//	// Store the value in the plus-side gain calibration register
//	ADC0->PG = calibration;
//	// Repeat the procedure for the minus-side calibration value
//	calibration = 0x0000;
//	calibration += ADC0->CLM0;
//	calibration += ADC0->CLM1;
//	calibration += ADC0->CLM2;
//	calibration += ADC0->CLM3;
//	calibration += ADC0->CLM4;
//	calibration += ADC0->CLMS;
//	calibration /= 2;
//	calibration |= 0x8000;
//	ADC0->MG = calibration;
//    //Done Calibration
//
//	/* Reconfigure ADC0*/
//    /* clock div by 4, long sample time, single ended 12 bit, bus clock */
//    ADC0->CFG1 = 0x40 | 0x10 | 0x04 | 0x00;
//    NVIC_SetPriority(ADC0_IRQn, ADCPRIOR);		//set ADC0 interrupt priority
//    NVIC->ISER[0] |= 1<<15;//0x8000;		  /*enable IRQ15 (bit 15 of ISER[0])(ADC)*/
//
//}


//Initialization function for LCD
void LCD_init(void)
{
    SIM->SCGC5 |= 1 << 11;       /* enable clock to Port C */
    PORTC->PCR[10] = 0x100;      /* make PTC10 pin as GPIO */
    PORTC->PCR[11] = 0x100;      /* make PTC11 pin as GPIO */
    PORTC->PCR[12] = 0x100;      /* make PTC12 pin as GPIO */
    PORTC->PCR[13] = 0x100;      /* make PTC13 pin as GPIO */
    PORTC->PCR[16] = 0x100;      /* make PTC16 pin as GPIO */
    PORTC->PCR[17] = 0x100;      /* make PTC17 pin as GPIO */
    PTC->PDDR |= 0x33C00;          /* make PTC 10,output pins */

    delayMs(30);                /* initialization sequence */
    LCD_nibble_write(0x30, 0);
    delayMs(10);
    LCD_nibble_write(0x30, 0);
    delayMs(1);
    LCD_nibble_write(0x30, 0);
    delayMs(1);
    LCD_nibble_write(0x20, 0);  /* use 4-bit data mode */
    delayMs(1);

    LCD_command(0x28);          /* set 4-bit data, 2-line, 5x7 font */
    LCD_command(0x06);          /* move cursor right */
    LCD_command(0x01);          /* clear screen, move cursor to home */
    LCD_command(0x0F);          /* turn on display, cursor blinking */
}

void LCD_nibble_write(unsigned char data, unsigned char control)
{
    data &= 0xF0;       /* clear lower nibble for control */
    control &= 0x0F;    /* clear upper nibble for data */
    PTC->PDOR = (data << 6)| (control << 6);       /* RS = 0, R/W = 0 */
    PTC->PDOR = ((data << 6)| (control << 7) | EN);  /* pulse E */
    delayMs(0);
    PTC->PDOR = data << 6;
    PTC->PDOR = 0;
}

void LCD_command(unsigned char command)
{
    LCD_nibble_write(command & 0xF0, 0);   /* upper nibble first */
    LCD_nibble_write((command << 4) & 0xF0, 0);     /* then lower nibble */

    if (command < 4)
        delayMs(4);         /* commands 1 and 2 need up to 1.64ms */
    else
        delayMs(1);         /* all others 40 us */
}

void LCD_data(unsigned char data)
{
    LCD_nibble_write(data & 0xF0, RS);    /* upper nibble first */
    LCD_nibble_write((data << 4) & 0xF0, RS);      /* then lower nibble  */

    delayMs(1);
}
