/*
 * Grafisk_lcd.c
 *
 * Created: 2014-04-14 11:28:40
 *  Author: peol0071
 */ 
#include <stdbool.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/eeprom.h>
#include "softserial.c"

#define arr_length 105

char * float_to_str(float num);
float * make_array();
char * int_to_str(int num);
bool compare_input(char * CompareString, int length, int offset);
void bt_reset();
void send_data_CSV();	//skicka data p� soft serial i csvformat

//funktioner f�r summer
void sound_on();
void sound_off();
void sound_toggle();


#include <avr/io.h>
#define F_CPU 8000000
#include <avr/delay.h>
#include <stdio.h>
#include "glcd.c"
#include "USART.c"



#define		bufferlen  80
#define		high_button 1
#define		low_button 2
#define		inc_button 4
#define		dec_button 8
#define		send_data_button 32
#define		summer	8	//1<<4
#define		bt_reset_pin	4

static char buffer[bufferlen];		//uart buffer
int point = 0;						//pekare till uartbuffer
bool connected = false;				//flagga f�r anslutning
bool update_graph_flag = false;

static float arr[arr_length];			//array f�r graf
#define temp arr[0]

#define alarm_eeprom_adr	5


static char strbuf[6] = "S";		//buffer f�r str�ngj�mf�relse

int high_alarm_level, low_alarm_level = 0;

ISR(USART_RX_vect)
{
	if(point == bufferlen - 1)
	{
		point = 0;
	}
	buffer[point] = UDR0;
	
	point++;
	
	strbuf[0] = buffer[point-9];		//kopiera till fast minnesadress
	strbuf[1] = buffer[point-8];
	strbuf[2] = buffer[point-7];
	strbuf[3] = buffer[point-6];
	strbuf[4] = buffer[point-5];
	

	
	if(strncmp("SUPER", strbuf, 5) == 0)
	{

		signed int mem_place   = ( (signed char) buffer[point-4] ) << 8;
		mem_place			  += (unsigned char) buffer[point-3];
		
		signed int temp_value = ( (signed char ) buffer[point-2] ) << 8;
		temp_value			  += (unsigned char) buffer[point-1];
			
		if (mem_place < arr_length)
		{
			arr[mem_place] = temp_value / 128.0;
		}
		

		connected = true;
		
		point = 0;	//nollst�ll pekare
		if (mem_place > arr_length - 2)
		{
			update_graph_flag = true;
			//LCD_draw_graph(red, 135, 104, arr);
		}
		
	}

	
	
}
bool compare_input(char * CompareString, int length, int offset)
{
	int bufPos = point - length - offset; //Startposition i buffer
	
	for(int i = 0; i < length; i++) //Position i CompareString
	{
		if( buffer[bufPos] != CompareString[i] ) 
		{ //Om de tv� bokst�verna inte st�mmer �verrens
			return false;
		}		
		bufPos++;
	}
return true;
}
void setup()
{
	DDRC = 0;		//inputs
	PORTC = 255;	//inputs
	
	DDRB = 0xff;
	DDRD = 0xff;
	
	bt_reset();
	
	high_alarm_level = eeprom_read_word(alarm_eeprom_adr);		//l�s niv�er fr�n eeprom
	low_alarm_level = eeprom_read_word(alarm_eeprom_adr + 2);
	
	sei();
	usart_init(MYUBRR);	//h�rdvaru uart
	
	init_serial();		//mjukvaru uart
	
	
	disp_all(white);	//Y
	usart_prstr("AT+btscan,3,0\r");
	//usart_prstr("AT+setesc,++\r");
}
bool button_press(char Button)
{
	if (~PINC & Button)
	{
		return true;
	}
	return false;
}
bool button_press_once(char Button)
{
	static char buttons;		//spara f�rra l�gen h�r
	
	if (( ~PINC & Button ) != ( ~buttons & Button ))		//j�mf�r och kolla om det �r skillnad
	{
		buttons = PINC;
		return true;		//sant is�fall
	}
	buttons = PINC;
	
	return false;
}

int main(void)
{
	setup();		//anropa setuprutin
	
    while(1)
    {

		LCD_write_string(buffer,black,0,100);
	
		//send_char(~(128+32+8+2));
		
		if ( button_press(high_button) )		//styr h�g larmniv�
		{
			LCD_write_big_string("H�g larmniv�:              " , red, 0, 0, 2);
			
			LCD_write_big_string(float_to_str(high_alarm_level), red, 0, 30, 7);
			
			if (button_press(inc_button))
			{
				high_alarm_level++;
				eeprom_write_word(alarm_eeprom_adr,high_alarm_level);
			}
			else if (button_press(dec_button))
			{
				high_alarm_level--;
				eeprom_write_word(alarm_eeprom_adr,high_alarm_level);
			}
		}
		else if ( button_press(low_button) )		//styr inmatning av l�g larmniv�
		{
			LCD_write_big_string("L�g larmniv�:               " , red, 0, 0, 2);
		
			LCD_write_big_string(float_to_str(low_alarm_level), blue, 0, 30, 7);
			if (button_press(inc_button))
			{
				low_alarm_level++;
				eeprom_write_word(alarm_eeprom_adr+2,low_alarm_level);
			}
			else if (button_press(dec_button))
			{
				low_alarm_level--;
				eeprom_write_word(alarm_eeprom_adr+2,low_alarm_level);
			}
		}		
		else if ( button_press(send_data_button) )		//Skicka data om knappen �r tryckt!
		{
			send_data_CSV();
		} 
		else
		{
			if (temp > high_alarm_level)		//kontrollera temperaturgr�nser f�r larm
			{
				LCD_write_big_string("Larm?", red, 0, 30, 7);
				sound_toggle();
			}
			else if (temp < low_alarm_level)
			{
				LCD_write_big_string("Larm?", blue, 0, 30, 7);
				sound_toggle();
			}
			else
			{
				LCD_write_big_string("Nuvarande temperatur �r:" , red, 0, 0, 2);		//uppdatera sk�rm
				LCD_write_big_string(float_to_str(temp), black, 0, 30, 7);
				sound_off();
				LCD_write_string(float_to_str(high_alarm_level) , red, 280, 20);
				LCD_write_string(float_to_str(low_alarm_level) , blue, 280, 30);			
			}


			
			if (update_graph_flag)
			{
				LCD_draw_graph(red, 135, 104, arr, arr_length);
			/*	usart_prstr("+++\r");
				_delay_ms(120);
				usart_prstr("AT+BTRSSI,1\r");		//saker f�r att l�sa signalstyrka
				_delay_ms(2000);
				usart_prstr("AT+BTRSSI,0\r");
				usart_prstr("ATO\r");*/
				
				update_graph_flag = false;
			}
				
		}	
    }
}
void sound_on(){
	PORTD |= summer;
}	
void sound_off(){
	PORTD &= ~summer;
}
void sound_toggle(){
	PORTD = ( PORTD ^ summer);
}

void bt_reset()
{
	PORTD &= ~bt_reset_pin;
	_delay_ms(10);
	PORTD |= bt_reset_pin;
}

void send_data_CSV()	//skicka data p� soft serial i csvformat
{
	//disable other interrupts
	UCSR0B &= ~(1 << RXCIE0); 
	//Visa notis p� sk�rmen
	LCD_write_string("Skickar data.", black, 240, 50);
	
	//startsignal
	send_string("\n\rstart\n\r");	
	//loop data
	for (int i = 0; i < arr_length; i++)
	{
		send_string("Jimmy vill ha mera utfyllnad!  Mycket data");		//precist konfigurerad data att skicka
		send_string(float_to_str(i));
		send_char(';');
		send_string(float_to_str(arr[i]));
		send_string("\n\r");
	}
	//stopsignal
	send_string("stop\n\r");
	//ta bort notis p� sk�rmen
	LCD_write_string("             ", black, 240, 50);
	//restore
	UCSR0B |= (1 << RXCIE0); 
}

char * float_to_str(float num)
{
	static char buffer[9];
	int intnum = num*10;	//konvertera till int
	int insert_zero = 100;	//n�r ska vi skriva nollor efter tal?
	
	int i = 0;
	if (intnum < 0)		//l�gg till minustecken
	{
		buffer[i] = '-';
		i++;
		intnum = -intnum;
	}
	
	for (int j = 10000;j >= 1; j /= 10.0 )	//loopa igenom tal och skapa nummer
	{
		
		if (intnum >= j || j < insert_zero)	
		{
			if (j == 1)
			{
				buffer[i] = ',';
				i++;
			}
			buffer[i] = intnum / j  + 48;
			i++;
			
			intnum = intnum % j ;
			insert_zero = j;	//efter vi skrivit en siffra skriver vi alltid nollor efter
			
		}
	}
	buffer[i++] = ' ';	//ett mellanslag f�r att kompensera f�r skiftande l�ngd p� str�ngen
	buffer[i] = 0; // skriv en nolla till buffer f�r att markera slut p� str�ngen
	
	return buffer;
	
}
