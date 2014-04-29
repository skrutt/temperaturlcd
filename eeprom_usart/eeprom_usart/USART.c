/*
 * USART.c
 *
 * Created: 03/04/2014 20:28:38
 *  Author: jimmy
 */ 

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

#define FOSC 8000000
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

void usart_init(uint16_t ubrr);
char usart_getchar(void) ; 
void usart_putchar(char data);
void usart_prstr(char *s);


void usart_init(unsigned int ubrr)
{
	UBRR0H = (unsigned char) (ubrr>>8); //Sätt rätt baudrate utifrån den definierade MYUBRR
	UBRR0L = (unsigned char) ubrr;
	
	UCSR0B = (1<<RXEN0) | (1<<TXEN0)| (0<<TXCIE0)| (1<<RXCIE0); //enable transmitter och receiver
	UCSR0C = (1<<USBS0) | (1<<UCSZ00) | (1<<UCSZ01); //8 data bitar 1 stop bit
}

void usart_putchar(char data)
{
	while(!(UCSR0A & (1<<UDRE0))) ; //Kollar transmission buffer empty flag UDRE0 i UCSR0A
	UDR0 = data;	
}

char usart_getchar(void)
{
	while(!(UCSR0A & (1<<RXC0))) ; //Kollar receive complete bit
	return UDR0;
}

void usart_prstr(char *s)  //Skriver ut en sträng
{
	while(*s)
	{
		usart_putchar(*s); //Skriv ut bokstaven
		s++; //Öka char-pointern
	}
}
