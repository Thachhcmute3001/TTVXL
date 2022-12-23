#include<htc.h>
#include<stdio.h>
#include <math.h>
#include <stdlib.h>
#include "lcd(16).h"

#define _XTAL_FREQ 20000000
__CONFIG(FOSC_HS & WDTE_OFF & PWRTE_ON & MCLRE_OFF & CP_OFF & CPD_OFF 
 & BOREN_ON & IESO_OFF & FCMEN_OFF & LVP_OFF & DEBUG_OFF);
char a[1];
char n[1];
int i;
void putch(char c);
void tx_init();
void rx_init();
void send_char(unsigned char data);
void send_string(const char *s);
void PWM_init();
void br_init();
void pulse(unsigned int data_xung);
void main()
{
//Pin setup
	ANSEL = ANSELH = 0; //disable all analog I/O
	TRISB0 = 1; //set pin B0 as input
	TRISB1 = 1; //set pin B1 as input
	TRISC2 = 0; //set pin C2 as ouptut
	TRISD5 = 0; //set pin D5 as ouptut
	TRISD6 = 0; //set pin D6 as ouptut
	TRISD7 = 0; //set pin D7 as ouptut
//Set internal clock to 2MHz. Need to set this first, then setup timer.
//	IRCF2 = 1; IRCF1 = 0; IRCF0 = 1; //Setup FOSC = 2MHz
//	SCS = 1; // Internal oscillator is used for system clock
//Timer2 setup
	TMR2ON = 1; //Enable Timer2
	T2CKPS1 = 0; T2CKPS0 = 1; // Prescale 1:4 //PWM doesn't involve in postscaler
	TMR2 = 0;	PR2 = 124; // using equation in datasheet to find PR2. T = 10^-4s                                                                                                                                                                                                                                                                                                  
//Set interrupt for PORTB
	RBIF = 0; //PortB change interrupt flag bit (at first time doesn't occur)
	RBIE = 1; //Enable PortB change interrupt 
	PEIE = 1; //Enable all unmasked peripheral interrupts 
	GIE = 1; //Enable all unmasked interrupts
	IOCB1 = 1;//Interrupt on-change (RB1) enabled
	IOCB0 = 1;
	
// Setup
	lcd_init();
	//EUSART
	tx_init();
	rx_init();
	br_init();
	//PWM
	PWM_init();
	while(1)
	{
		for(int j = 0; j < 1; j++)
		{
			n[j] = a[j];
		}
		if(n[0] == 'R')
		{
			pulse(63);
			CCP1M1 = 0; CCP1M0 = 1; //Forward
		}
		else if(n[0] == 'L')
		{
			pulse(63);
			CCP1M1 = 1; CCP1M0 = 1; //Reverse
		}
		else if(n[0] == 'S')
		{
			TMR2ON = 1;
		}
		else if(n[0] == 'P')
		{
			TMR2ON = 0;
		}
	}	
}
void send_char(unsigned char data)
{
	while(!TXIF){}
		TXREG = data;

}
void send_string(const char *s)
{
		while(*s)
		{
			send_char(*s++);
		}
}
void interrupt Ngat()
{
	if(RBIF)
	{
		if(RB0 == 0 && RB1 == 1)
		{
			PR2 = 124; //10kHz
			pulse(113);//90% 
		}
		else if(RB1 == 0 && RB0 == 1)
		{
			PR2 = 83; //15kHz
			pulse(59);//70%
		}
	RBIF = 0;
	}
	if(RCIF)
	{
		a[i] = RCREG;
		i++;
		if(i == 1)  i = 0;
	}
	RCIF = 0;
}
void pulse(unsigned int data_xung)
{
//10bit
//	CCPR1L = data_xung >> 2; //10bit
//	DC1B1 = data_xung & 2; //10bit only
//	DC1B0 = data_xung & 1; //10bit only
//8bit
	CCPR1L = data_xung; //8bit
}
void PWM_init()
{
	//Need to turn on timer2 then be able to use PWM mode
	CCP1M3 = 1; CCP1M2 = 1; //Enable PWM mode for CCP1
}
void tx_init()
{
	SYNC = 0; // Use asynchornous mode // Only in TX setup, RX doesn't have this pin
	TX9 = 0; //8-bit reception
	BRGH = 1; //High speed
	TXEN = 1; //Transmit enabled
}
void rx_init()
{
	RX9 = 0; // 8 bit reception
	SPEN = 1; // TX/RX ports are enabled
	CREN = 1; // Enable receiver (Asynchornous mode)
	//EUSART Receive Interrupt
	RCIE = 1; RCIF = 0;
}
void br_init()
{
	//Baud rate setup
	BRG16 = 1; //Use 16-bit Baud rate 
	BRGH = 1; //High baud rate selected bit
	SPBRGH = 0x02; // SPBRH:SPBRG = 520 (BRG16 = 1; SYNC = 0; BRGH = 1, FOSC = 20MHz)
	SPBRG = 0x08; // Baud rate = 9600
}
void putch(char c)
{
	lcd_putc(c);
}