

#include <avr/eeprom.h>
#define mem_pointer_sign 0x8000
#define mem_length 1 //behövs tydligen bara ett? använder eeprom write adress x2? 

unsigned int find_mem_place()
{
	//leta mem_pointer_sign, minnespekare
	for (unsigned int * i = eeprom_start; i < eeprom_stop; i++)
	{
		if (eeprom_read_word(i) == mem_pointer_sign)
		{
			return i;
		}
		
	}
	return eeprom_start;		//returnera en minnesstart om vi ej hittat nån
}

void write_eeprom(int word, unsigned int * adr)
{
	eeprom_write_word(adr,word);		//skriver tydligen low byte först
	//skriv till adress
}

void save_temp_eeprom(int word)
{
	unsigned int ff_pointer = eeprom_pointer + mem_length; //öka ett steg, pekare till nästa slot
	if (ff_pointer >= eeprom_stop)
	{
		ff_pointer = eeprom_start;				//slå runt minnet
	}
	eeprom_write_word(ff_pointer, mem_pointer_sign);		//skriv alltid en mem_pointer_sign först för att garantera minnesplats
	eeprom_write_word(eeprom_pointer, word);		//skriv till eeprom
	eeprom_pointer = ff_pointer;	//avancera ett steg
	
}
  
int * read_mem()
/**läs minnet och skicka tillbaka pekare till en kronologisk array*/
{
	static int return_mem[ eeprom_mem_slots ];	//reservera minne
	unsigned int array_pointer = 0;
	unsigned int i = eeprom_pointer;
	
	if (i == eeprom_start)		//wrap around memory
	{
		i = eeprom_stop - 2;	//sätt till sista minnesplats
	}else
	{
		i--;
		i--;		//två byte på varje plats
	}
	
	
	for(; i != eeprom_pointer; i -=2 )	//läs två byte åt gången och loopa bakåt
	{
		
		return_mem[array_pointer] = eeprom_read_word(i);
		array_pointer++;
		
		if (i <= eeprom_start)		//wrap around memory
		{
			i = eeprom_stop ;	//sätt till sista minnesplats
		}
	}
	return return_mem;
}