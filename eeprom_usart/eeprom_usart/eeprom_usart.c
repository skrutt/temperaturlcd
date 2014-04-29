#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>

#define BT_reset	  7

#define eeprom_start 200
#define eeprom_stop 420
#define eeprom_mem_slots ( eeprom_stop - eeprom_start ) / 2 - 1 // en slot går till nuvarande plats
#define update_Hz 2
#define update_delay 1000 / update_Hz
#define save_to_eeprom_every_min 5
#define eeprom_save_when 60*update_Hz*save_to_eeprom_every_min

unsigned int * eeprom_pointer = 0;

int temp;


#include "SPI.c"
#include "eeprom.c"
#include "USART.c"

bool connected = false;

int charPos = 0;


char message[] = "Nuvarande temperatur:";
void setup()
{
	DDRD = 255;
	PORTD |= 1<<BT_reset; 
	DDRC = 0;
	PORTC = 0xFF;
	//LcdInitialize();
	sei();
	usart_init(MYUBRR);
	SPI_init();
	eeprom_pointer = find_mem_place();	//letar rätt på senaste minnesplats
	
	/*unsigned int * adr = 4;
	write_eeprom(0, adr);*/
	//save_temp_eeprom(5);
	//save_temp_eeprom(6);
	bluetooth_reset();
}


int main(void)
{
	setup();
	unsigned int counter = 0;
	while(1)
	{
		if(connected == false) {
			usart_prstr("ATD0001950A6BEF\r");
			
		}
		else {

			usart_prstr("SUPER");
			usart_putchar(temp >> 8);
			usart_putchar(temp & 255);
			
		}
		
		_delay_ms(update_delay);
		
		

		temp = SPI_readWord();
		
		
		//float floatfloat = temp;
		//disp_float(floatfloat / 128);
		//save_temp(temp);
		
		
		counter++;
		if (counter==eeprom_save_when)	//spara till eeprom vid efter rätt tid
		{
			counter = 0;
			save_temp_eeprom(temp);//save
		}
		
		//disp_num( eeprom_pointer );	//visa nuvarande eepromplats
		
		if (~PINC & 1)
		{
			//LcdSendCharacter('S');		//Skickar ut tempdata på skärmen
			disp_mem_usart();
		}
	}	   
	   
	return 0;
}


ISR(USART_RX_vect)
{	
	char testString[7] = "CONNECT";
	
	char input = UDR0;
	if(UDR0 == testString[charPos]) {
		charPos++;
	}
	else {
		charPos = 0;
	}
	if(charPos == 7) {
		usart_prstr("JA");
		connected = true;
	}
	
}


//defines for memory
#define mem_slots 50
int temp_array[mem_slots+1];
int array_place = 0;

void save_temp(int temperature)		//spara temp prototyp, sparas i sram
{
	temp_array[array_place] = temperature;
	array_place++;
	if (array_place==mem_slots)
	{
		array_place = 0;
	}
	
}




void disp_mem_usart()
{
	usart_prstr("Sparade temps: \n\r");
	int * temp_array = read_mem();
	for( int i = 0; i < eeprom_mem_slots; i++)
	{
		
		float floatfloat = temp_array[i];
		usart_float(floatfloat);
		usart_putchar('\n');
		usart_putchar('\r');
		
	}

}

void usart_float(float num)
{
	num = num / 128;
	if(num < 0)
	{
		usart_putchar('-');
		num = -num;
	}
	
	if (num >= 100)
	{
		char charchar = num / 100;
		usart_putchar(charchar + 48);
		num -= charchar * 100;
	}
	if (num >= 10)
	{
		char charchar = num / 10;
		usart_putchar(charchar + 48);
		num -= charchar * 10;
	}
	usart_putchar(num + 48);
	usart_putchar(46);
	int intint = num;
	int decimals = (num - intint) * 10;
	usart_putchar(decimals + 48);
	
}

void bluetooth_reset() {
		PORTD &= ~(1<<BT_reset);
		PORTD |= (1<<BT_reset);
}