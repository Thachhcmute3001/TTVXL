#include<htc.h>
#include<stdio.h>
#include <math.h>
#include <stdlib.h>
#include "lcd(16).h"

#define _XTAL_FREQ 20000000
__CONFIG(FOSC_INTRC_NOCLKOUT & WDTE_OFF & PWRTE_ON & MCLRE_OFF & CP_OFF & CPD_OFF 
 & BOREN_ON & IESO_OFF & FCMEN_OFF & LVP_OFF & DEBUG_OFF);
 
void putch(char c);
void tx_init();
void rx_init();
void PWM_init();
void br_init();
void angle(unsigned int angle);
void send_char(unsigned char data);
void send_string(const char *s);
char get_char();
unsigned int xung;
char a[5];
int angle_str;

int i = 0;
int dem;

void main()
{
//Pin setup
	ANSEL = ANSELH = 0; //disable all analog I/O
//Set internal clock to 2MHz. Need to set this first, then setup timer.
	IRCF2 = 1; IRCF1 = 0; IRCF0 = 1; //Setup FOSC = 2MHz
	SCS = 1; // Internal oscillator is used for system clock
//Timer2 setup
	TMR2ON = 1; //Enable Timer2
	T2CKPS1 = 1; T2CKPS0 = 1; // Prescale 1:16 //PWM doesn't involve in postscaler
	TMR2 = 0; PR2 = 624; // using equation in datasheet to find PR2
// Set interrupt for Timer2
	TMR2IF = 0; GIE = 1; TMR2IE = 0; PEIE = 1;
//Pin setup
	TRISC2 = 0; //set pin C2 as output
	TRISC6 = 0; //set TX as output
	TRISC7 = 1; //set RX as input
	TRISE0 = 0;
	RE0 = 0;
// Setup
	lcd_init();
	tx_init();
	rx_init();
	PWM_init();
	br_init();
	char pwm[5];
	//send_string("Giao tiep UART da duoc khoi tao");	//Need to set in order to able to recieve data (tested)
	while(1)
	{
		for (dem = 2;dem < 5;dem++)
		{
			pwm[dem-2] = a[dem];
			//dem++; 
		}
		send_string(pwm);
		angle_str = atoi(pwm);
		angle(angle_str);
		lcd_gotoxy(0,0);
		printf("%d",angle_str);
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
void angle(unsigned int angle)
{
	xung = 125 + (angle * 125) / 180;
	CCPR1L = xung >> 2;
	DC1B1 = xung & 2;
	DC1B0 = xung & 1;
	
}
void interrupt Ngat()
{
	if(RCIF)
	{
		a[i] = RCREG;
		i++;
		if(i == 5) i=0;
	}
	RCIF = 0;
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
	CCP1M3 = 1; CCP1M2 = 1; //Enable PWM mode for CCP1
}
void br_init()
{
	//Baud rate setup
	BRG16 = 1; //Use 16-bit Baud rate 
	BRGH = 1; //High baud rate selected bit
	SPBRGH = 0b00000000; // SPBRG = 51 (BRG16 = 1; SYNC = 0; BRGH = 1, FOSC = 2MHz)
	SPBRG = 0b00110011; // Baud rate = 9600
}
void putch(char c)
{
	lcd_putc(c);
}