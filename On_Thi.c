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
void pulse(int data_xung);
void send_char(unsigned char data);
void send_string(const char *s);
char get_char();
char a[5];
int angle_str;
int ecd;

int i = 0;
int dem, pwm_v;
long int xung;
float v;

void main()
{
//Pin setup
	ANSEL = ANSELH = 0; //disable all analog I/O
//Set internal clock to 2MHz. Need to set this first, then setup timer.
//	IRCF2 = 1; IRCF1 = 0; IRCF0 = 1; //Setup FOSC = 2MHz
//	SCS = 1; // Internal oscillator is used for system clock
//Timer0 setup - timer mode
	T0CS =0 ;
	PSA = 0;
	PS2 = 0; PS1 = 1; PS0 = 0;
	TMR0 = 7;
//Timer1 setup - counter mode
	TMR1ON = 1;
	T1OSCEN = 1;
	TMR1CS = 1;
	T1CKPS1 = 0; T1CKPS0 = 0; //1:1 prescale value
	T1SYNC = 1;
	TMR1GE = 0;
//Timer2 setup
	TMR2ON = 1; //Enable Timer2
	T2CKPS1 = 0; T2CKPS0 = 1; // Prescale 1:8 //PWM doesn't involve in postscaler
	TMR2 = 0; PR2 = 124; // using equation in datasheet to find PR2. T = 0.4ms                                                                                                                                                                                                                                                                                                        
// Set interrupt for Timer1
	TMR1IF = 0; GIE = 1; TMR1IE = 1; PEIE = 1;
//Set interrupt for Timer0
	T0IE = 1; T0IF = 0; GIE = 1;
//Pin setup
	TRISC2 = 0; //set pin C2 as output
	TRISC6 = 0; //set TX as output
	TRISC7 = 1; //set RX as input
	TRISE0 = 0;
	TRISE1 = 0;
// Setup
	lcd_init();
	tx_init();
	rx_init();
	PWM_init();
	br_init();
	char pwm[10];
	char n[5];
	//send_string("giao");	//Need to set in order to able to recieve data (tested)
	while(1)
	{
		for (dem = 2;dem < 5;dem++)
		{
			n[dem-2] = a[dem];
			//dem++; 
		}

		send_string(n);
		angle_str = atoi(n);
		pulse(angle_str);
		if(a[0] == 'P')
		{
			TMR2ON = 0;
		}
		else if(a[0] == 'S')
		{
			TMR2ON = 1;
		}
		else if(a[0] == 'F')
		{
			TMR2ON = 1;
			RE0 = 0;
			RE1 = 1;
		}
		else if(a[0] == 'B')
		{
			TMR2ON = 1;
			RE0 = 1;
			RE1 = 0;
		}
		v = (xung/360.0)*60.0*10.0;
		lcd_gotoxy(0,0);
		printf("hehe: %03u",angle_str);
		lcd_gotoxy(0,1);
		printf("v: %3.5f",v);
		sprintf(pwm,"%3.2f",v);

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
void pulse(int data_xung)
{
	CCPR1L = data_xung >> 2;
	DC1B1 = data_xung & 2;
	DC1B0 = data_xung & 1;
}
void interrupt Ngat()
{
	
	if(T0IF)
	{
		ecd++;
		if(ecd == 250){ecd = 0; xung = TMR1; TMR1 = 0;}
	}
	TMR0 = 7;
	T0IF = 0;
	if(RCIF)
	{
		a[i] = RCREG;
		i++;
		if(i == 5)  i = 0;

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
	SPBRGH = 0x02; // SPBRG = 51 (BRG16 = 1; SYNC = 0; BRGH = 1, FOSC = 20MHz)
	SPBRG = 0x08; // Baud rate = 9600
}
void putch(char c)
{
	lcd_putc(c);
}