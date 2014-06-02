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
#define update_Hz 5
#define update_delay 1000 / update_Hz
#define save_to_eeprom_every_min 10  //Nuvarande temperatur sparas till EEPROM var 10:e minut
#define eeprom_save_when 60*update_Hz*save_to_eeprom_every_min

#define buffer_len 80

//defines for memory
#define mem_slots 50
int temp_array[mem_slots+1];
int array_place = 0;
//--

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
	DDRD = 255;				//Port D som utgång
	bluetooth_reset();
	DDRC = 0;				//Port C som ingång
	PORTC = 0xFF;			//Pullups på Port C
	sei();					//enable interrupts
	usart_init(MYUBRR);		//Initiera usart-kommunikation
	SPI_init();				//initiera SPI-kommunikation med LM74
	eeprom_pointer = find_mem_place();	//letar rätt på senaste minnesplats
}


int main(void)
{
	setup();
	unsigned int counter = 0;
	while(1)
	{
		//Om ej ansluten försök att ansluta:
		if(connected == false) {
			usart_prstr("ATD0001950A6BEF\r"); //Sträng för anslutning mot displaymodulen
			
		}
		//Om ansluten skicka nuvarande temperatur:
		else {
			usart_prstr("SUPER");
			usart_putchar(0);
			usart_putchar(0);
			usart_putchar(temp >> 8);
			usart_putchar(temp & 255);
		}
		
		_delay_ms(update_delay);
		
		temp = SPI_readWord();	//Läs av temperatur			
		
		counter++;
		if (counter==eeprom_save_when)	//spara till eeprom vid efter rätt tid
		{
			counter = 0;
			save_temp_eeprom(temp);//save
			send_eeprom(); //Skicka sparade eeprom värden via usart
		}
	}	   
	   
	return 0;
}

//Variabler för hantering av mottagen data:
char strbuf[7];
char buffer[80];
int point = 0;

//Hantera inkommande data:
ISR(USART_RX_vect)
{	
	if(point == buffer_len) {
		point = 0;
	}
	
	buffer[point] = UDR0;
	
	strbuf[0] = buffer[point - 9];
	strbuf[1] = buffer[point - 8];
	strbuf[2] = buffer[point - 7];
	strbuf[3] = buffer[point - 6];
	strbuf[4] = buffer[point - 5];
	strbuf[5] = buffer[point - 4];
	strbuf[6] = buffer[point - 3];
	point++;
	
	//Om inkommande sträng är connect sätt connected till true:
	if(strncmp("CONNECT", strbuf, 7) == 0) 
	{
		connected = true;
		point = 0;
	}
	
	//Om inkommande sträng är disconnect sätt connected till false:
	if(strncmp("DIS", strbuf, 3) == 0)
	{
		connected = false;
		point= 0;
	}
}

void save_temp(int temperature)		//spara temp prototyp, sparas i sram
{
	temp_array[array_place] = temperature;
	array_place++;
	if (array_place==mem_slots)
	{
		array_place = 0;
	}
	
}

void send_eeprom()		//Skicka temperatur sparat i eeprom till displaymodulen
{	
	int * temp_array = read_mem();	//Läs eeprom
	for(int i = 0; i < eeprom_mem_slots; i++) {		//Loopa igenom och skriv ut alla temperaturvärden
		usart_prstr("SUPER");
		i++;
		usart_putchar(i >> 8);
		usart_putchar(i & 255);
		i--;
		usart_putchar(temp_array[i] >> 8);
		usart_putchar(temp_array[i] & 255);
		//_delay_ms(10);
	}
}

void bluetooth_reset() {
		PORTD |= 1<<BT_reset; 
		_delay_ms(15);
		PORTD &= ~(1<<BT_reset);
		_delay_ms(15);
		PORTD |= (1<<BT_reset);
}