
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"


#define RS 1    /* BIT0 mask */
#define RW 2    /* BIT1 mask */
#define EN 4    /* BIT2 mask */

//LED prototype functions
void LED_init(void);
void LED_set(int value);

//keypad prototype functions
void keypad_init(void);
char keypad_getkey(void);

//prompts for method chosen
char CharsDisplayed[] = "Method 1:";//array acting as a buffer for displayed digits
char Method1[] = "Method 1";
char Method2[] = "Method 2";
char Method3[] = "Method 3";
char Method4[] = "Method 4";
char Method5[] = "Method 5";

void UpdateBuffer(int, int);

//LCD display function prototype
void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void LCD_Write();

//Receive transmit functions
void UART0_init(void);
void UART0Tx(char c);
void UART0_puts(char* s);
//TODO include timer initialization function here
//TODO include ADC initialization function here

/*
 * @brief   Application entry point.
 */
int main(void) {

    unsigned char key;

  	/* Init board hardware. */
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();
    __disable_irq();
    keypad_init();
    __enable_irq();

    //TODO Test receive function of UART might need interrupt
    //TODO Get Keypad working with polling function
    //TODO Step 2 get Keypad working with interrupt
    //TODO Step 3 display output on UART from Keypad

 //   LCD_init();		//Initialize LCD
    UART0_init();	//Initialize UART
    ADC0_init();	//Initialize ADC

    //TODO test keypad with polling
    key = keypad_getkey();
    if(key == 4){
    	UART0_puts(Method1);
    }
    if(key == 3){
    	UART0_puts(Method2);
    }
    if(key == 2){
    	UART0_puts(Method3);
    }
    if(key == 8){
    	UART0_puts(Method4);
    }
    if(key == 7){
    	UART0_puts(Method5);
    }
    if(key == 14){
    	UART0_puts(CharsDisplayed);
    }


    while(1)
    {


    }
}

//TODO double check baud rates may need 9600 baud rate
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
    PORTA->PCR[2] = 0x0200; 	//make PTA2 UART0_Tx pin */
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

//TODO double ckeck these values and comment this function
void ADC0_init(void)
{
	uint16_t calibration;

    SIM->SCGC5 |= 0x2000;       /* clock to PORTE */
    PORTE->PCR[20] = 0;         /* PTE20 analog input */

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

}


//delay function that used system clock instead of for loop
void delayMs(int msec){
	while(msec >= 0){							//make sure user entered positive time value
		SysTick->LOAD = msec * 48000;			//set systick for milliseconds
		SysTick->CTRL = 5;						//enable SysTick timer and use system clock
		while((SysTick->CTRL & 0x1000) == 0){}	//wait for clock to stop
		SysTick->CTRL = 0;
		break;
	}

}




//Port A interrupt handler
void PORTA_IRQHandler(void){
	int col, row;
	const char row_select[] = {1 << 1, 1 << 2, 1 << 4, 1 << 5}; /* one row is active */
	while(PORTA->ISFR & (1 << 17 | 1 << 16 | 1 << 13 | 1 << 12)){
		//determine which row was pressed
		for (row = 0; row < 4; row++)
		{
			PTA->PDDR = 0;                  /* disable all rows */
			PTA->PDDR |= row_select[row];   /* enable one row */
			PTA->PCOR = row_select[row];    /* drive the active row low */
		}

		if(PORTA -> ISFR & 1 << 17){
			col = 0;
		}

		if(PORTA -> ISFR & 1 << 16){
			col = 1;
		}

		if(PORTA -> ISFR & 1 << 13){
			col = 2;
		}

		if(PORTA -> ISFR & 1 << 12){
			col = 3;
		}

		PORTA->ISFR = ~0x00000000;//clear interrupt flag
		UpdateBuffer(row, col);
	}
}

//TODO delete this function and modify for port a
char keypad_getkey(void)
{
    int row, col;
    const char row_select[] = {0x01, 0x02, 0x04, 0x08}; //TODO change these

    /* check to see any key pressed */
    PTA->PDDR |= 0x0F;          /* enable all rows */
    PTA->PCOR = 0x0F;
    delayMs(200);                 /* wait for signal return */
    col = PTA->PDIR & 0xF0;     /* read all columns */
    PTC->PDDR = 0;              /* disable all rows */
    if (col == 0xF0)
        return 0;               /* no key pressed */

    /* If a key is pressed, it gets here to find out which key.
     * It activates one row at a time and read the input to see
     * which column is active. */
    for (row = 0; row < 4; row++)
    {
        PTC->PDDR = 0;                  /* disable all rows */
        PTC->PDDR |= row_select[row];   /* enable one row */
        PTC->PCOR = row_select[row];    /* drive the active row low */
        delayUs(2);                     /* wait for signal to settle */
        col = PTC->PDIR & 0xF0;         /* read all columns */
        if (col != 0xF0) break;         /* if one of the input is low, some key is pressed. */
    }
    PTC->PDDR = 0;                      /* disable all rows */
    if (row == 4)
        return 0;                       /* if we get here, no key is pressed */

    /* gets here when one of the rows has key pressed, check which column it is */
    if (col == 0xE0) return row * 4 + 1;    /* key in column 0 */
    if (col == 0xD0) return row * 4 + 2;    /* key in column 1 */
    if (col == 0xB0) return row * 4 + 3;    /* key in column 2 */
    if (col == 0x70) return row * 4 + 4;    /* key in column 3 */

    return 0;   /* just to be safe */
}

//TODO delete this function
void keypad_init(void)
{
    SIM->SCGC5   |= 1 << 9;       /* enable clock to Port A */
    //enable pins as GPIO
    PORTA->PCR[17] = 0x103;     /* make PTA17 pin as GPIO and enable pullup and enable rising edge interrupt*/
    PORTA->PCR[16] = 0x103;     /* make PTA16 pin as GPIO and enable pullup and enable rising edge interrupt*/
    PORTA->PCR[13] = 0x103;		/* make PTA13 pin as GPIO and enable pullup and enable rising edge interrupt*/
    PORTA->PCR[12] = 0x103;     /* make PTA12 pin as GPIO and enable pullup and enable rising edge interrupt*/
    PORTA->PCR[5] = 0x103;      /* make PTA5 pin as GPIO and enable pullup*/
    PORTA->PCR[4] = 0x103;      /* make PTA4 pin as GPIO and enable pullup*/
    PORTA->PCR[2] = 0x103;      /* make PTA2 pin as GPIO and enable pullup*/
    PORTA->PCR[1] = 0x103;      /* make PTA1 pin as GPIO and enable pullup*/
    PTA->PDDR |= 1 << 5 | 1 << 4 | 1 << 2 | 1 << 1; //make PTA5,4,2,1 as output pins
}

//TODO uncomment this code for interrupt keypad and check functionality
/* this function initializes PortC that is connected to the keypad.
 * All pins are configured as GPIO input pin with pull-up enabled.
 */
//void keypad_init(void)
//{
//    SIM->SCGC5   |= 1 << 9;       /* enable clock to Port A */
//    //enable pins as GPIO
//    PORTA->PCR[17] = 0x103 | 0x90000;      /* make PTA17 pin as GPIO and enable pullup and enable rising edge interrupt*/
//    PORTA->PCR[16] = 0x103 | 0x90000;      /* make PTA16 pin as GPIO and enable pullup and enable rising edge interrupt*/
//    PORTA->PCR[13] = 0x103 | 0x90000;      /* make PTA13 pin as GPIO and enable pullup and enable rising edge interrupt*/
//    PORTA->PCR[12] = 0x103 | 0x90000;      /* make PTA12 pin as GPIO and enable pullup and enable rising edge interrupt*/
//    PORTA->PCR[5] = 0x103;      /* make PTA5 pin as GPIO and enable pullup*/
//    PORTA->PCR[4] = 0x103;      /* make PTA4 pin as GPIO and enable pullup*/
//    PORTA->PCR[2] = 0x103;      /* make PTA2 pin as GPIO and enable pullup*/
//    PORTA->PCR[1] = 0x103;      /* make PTA1 pin as GPIO and enable pullup*/
//    PTA->PDDR |= 1 << 5 | 1 << 4 | 1 << 2 | 1 << 1; //make PTA5,4,2,1 as output pins
//}

/*This function adds the characters pressed to the character buffer*/
void UpdateBuffer(int row, int col){
	if(row == 0){
		if (col == 1){
			*CharsDisplayed = "Method 1";
		}
		if (col == 2){
			*CharsDisplayed = "Method 2";
		}
		if (col == 3){
			*CharsDisplayed = "Method 3";
		}

	}
	if(row == 1){
		if (col == 1){
			*CharsDisplayed = "Method 4";
		}
		if (col == 2){
			*CharsDisplayed = "Method 5";
		}
	}
	if(row == 3){
		*CharsDisplayed = "Select Method";
	}

	LCD_Write();
}

void LCD_Write(){
	LCD_command(1);         /* clear display */
	delayMs(500);
	LCD_command(0x85);      /* set cursor at first line */

	for(int i = 0; i < 9; i++){
		LCD_data(CharsDisplayed[i]);
	}
}

void LCD_init(void)
{
    SIM->SCGC5 |= 0x1000;       /* enable clock to Port D */
    PORTD->PCR[0] = 0x100;      /* make PTD0 pin as GPIO */
    PORTD->PCR[1] = 0x100;      /* make PTD1 pin as GPIO */
    PORTD->PCR[2] = 0x100;      /* make PTD2 pin as GPIO */
    PORTD->PCR[4] = 0x100;      /* make PTD4 pin as GPIO */
    PORTD->PCR[5] = 0x100;      /* make PTD5 pin as GPIO */
    PORTD->PCR[6] = 0x100;      /* make PTD6 pin as GPIO */
    PORTD->PCR[7] = 0x100;      /* make PTD7 pin as GPIO */
    PTD->PDDR |= 0xF7;          /* make PTD7-4, 2, 1, 0 as output pins */

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
    PTD->PDOR = data | control;       /* RS = 0, R/W = 0 */
    PTD->PDOR = data | control | EN;  /* pulse E */
    delayMs(0);
    PTD->PDOR = data;
    PTD->PDOR = 0;
}

void LCD_command(unsigned char command)
{
    LCD_nibble_write(command & 0xF0, 0);   /* upper nibble first */
    LCD_nibble_write(command << 4, 0);     /* then lower nibble */

    if (command < 4)
        delayMs(4);         /* commands 1 and 2 need up to 1.64ms */
    else
        delayMs(1);         /* all others 40 us */
}

void LCD_data(unsigned char data)
{
    LCD_nibble_write(data & 0xF0, RS);    /* upper nibble first */
    LCD_nibble_write(data << 4, RS);      /* then lower nibble  */

    delayMs(1);
}






