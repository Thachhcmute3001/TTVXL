#include<htc.h>
#include<stdio.h>
#include <math.h>
#include <stdlib.h>
#include "lcd(16).h"

#define _XTAL_FREQ 20000000
__CONFIG(FOSC_HS & WDTE_OFF & PWRTE_ON & MCLRE_OFF & CP_OFF & CPD_OFF 
 & BOREN_ON & IESO_OFF & FCMEN_OFF & LVP_OFF & DEBUG_OFF);
 
void putch(char c);
void tx_init();
void rx_init();
void PWM_init();
void br_init();
void pulse(unsigned int data_xung);
void send_char(unsigned char data);
void send_string(const char *s);
unsigned int count, count_old;
long int xung;
float distance,t;
char a[10];

void main()
{
//Pin setup
	ANSEL = ANSELH = 0; //disable all analog I/O
	TRISC1 = 1; //set pin C1 as innput - Trig //CCP2
	TRISC2 = 1; //set pin C2 as intput - Trig //CCP1
	TRISC6 = 0; //set TX as output
	TRISC7 = 1; //set RX as input
	TRISE0 = 0; //set pin E0 as output - Echo
//Set internal clock to 2MHz. Need to set this first, then setup timer. //Used for setup baud-rate
//	IRCF2 = 1; IRCF1 = 0; IRCF0 = 1; //Setup FOSC = 2MHz
//	SCS = 1; // Internal oscillator is used for system clock
//Timer1 setup
	T1SYNC = 1;
	TMR1ON = 1; //Enables Timer1
	T1OSCEN = 1; //LP oscillator is enabled for Timer1 clock
	TMR1CS = 0; //Internal clock (FOSC/4)
	T1CKPS1 = 0; T1CKPS0 = 0; //1:1 prescale value
	TMR1GE = 0; //Timer1 is always counting
	TMR1H = 0; TMR1L = 0;
//Set interrupt for CCP
	CCP1IE = 1; CCP1IF = 0; //When we set this. Do not set TMR1IE and TMR1IF. It will somehow conflict each other
	CCP2IE = 1; CCP2IF = 0;
// Set global interrupt
	GIE = 1; PEIE = 1;
// Setup
	lcd_init();
	tx_init();
	rx_init();
	PWM_init();
	br_init();
	while(1)
	{
		__delay_ms(100);
		RE0 = 1;
		__delay_ms(1);
		RE0 = 0;
		xung = abs(count - count_old);
		t = xung * (1.0/5000000.0);
		distance = 17000.0*t;
		sprintf(a,"%f",distance);
		send_string(a);
		send_string("/");
		lcd_gotoxy(0,0);
		printf("distance: %3.3f",distance);
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
	if(CCP1IF)
	{
		count = CCPR1;
	}
	if(CCP2IF)
	{
		count_old = CCPR2;
	}
	CCP1IF = 0;
	CCP2IF = 0;
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
void PWM_init()
{
	CCP2M3 = 0; CCP2M2 = 1; CCP2M1 = 0; CCP2M0 = 0; //Capture mode, every falling edge
	CCP1M3 = 0; CCP1M2 = 1; CCP1M1 = 0; CCP1M0 = 1; //Capture mode, every rising edge 
	
}
void br_init()
{
	//Baud rate setup
	BRG16 = 1; //Use 16-bit Baud rate 
	BRGH = 1; //High baud rate selected bit
	SPBRGH = 0x02; // SPBRG = 520 (BRG16 = 1; SYNC = 0; BRGH = 1, FOSC = 20MHz)
	SPBRG = 0x08; // Baud rate = 9600
}
void putch(char c)
{
	lcd_putc(c);
}