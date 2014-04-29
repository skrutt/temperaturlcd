
//Instruktioner:
#define LCD_FUNCTION_SET		0x38 // 0b00111000
#define LCD_FUNCTION_SET_4BIT	0x28 // 0b00101000
#define LCD_DISPLAY_OFF			0x08 // 0b00001000
#define LCD_DISPLAY_ON			0x0F // 0b00001111
#define LCD_DISPLAY_CLEAR		0x01 // 0b00000001
#define LCD_ENTRY_MODE_SET		0x06 // 0b00000110
#define LCD_CURSOR_HOME			0x02 // 0b00000010
#define LCD_display_length		16	//l�ngd i tecken
#define Lcd_line2				0x40
unsigned int cursor_pos = 0;	//position f�r pekare eller vad det heter p� svenska

//funktionsdeklarationer:
void LcdSetCursorAt(unsigned char pos);
void LcdSendNibble( uint8_t nibble );
void LcdSendByte(uint8_t theByte);
void LcdSendInstruction( uint8_t theInstruction );
//forts�ttning f�ljer

void LcdSendNibble( uint8_t nibble )
{
	_delay_ms(1);
	LCD_PORT = (nibble & 0xF0) | (LCD_PORT & 0x0F); //Skriv ut nibble till D4-D7

	// Toggla E f�r att utf�ra kommandot
	LCD_PORT |= (1<<LCD_E);
	LCD_PORT &= ~(1<<LCD_E);
}

void LcdSendByte(uint8_t theByte)
{
	// Skicka upper nibble:
	LcdSendNibble(theByte);

	// Skifta v�nter till lower -> higher nibble
	theByte = theByte << 4;
	// Skica lower nibble:
	LcdSendNibble(theByte);
}

void LcdSendInstruction( uint8_t theInstruction )
{
	LCD_PORT &= ~(1<<LCD_RS); //RS = 0 f�r att skicka instruktion

	//Skicka instruktion:
	LcdSendByte(theInstruction);
}


void LcdSendCharacter(uint8_t theChar)
{
	LCD_PORT |= (1<<LCD_RS); //RS = 1 f�r att skicka text

	//Skicka instruktion:
	LcdSendByte(theChar);
	cursor_pos++;
	if (cursor_pos == LCD_display_length)	//wrappa till rad tv�
	{
		LcdSetLine2();
	}
}
void LcdSetLine2()
{
	LcdSetCursorAt(Lcd_line2);
}

void LcdInitialize(void)
{
	LCD_PORT &= ~(1<<LCD_RS);	// RS = 0
	LCD_PORT &= ~(1<<LCD_E);	// E = 0

	//Ange pinnarna som output:
	LCD_DDR = 255;
	
	//Skicka 3x init sekvens med delay:
	_delay_ms(15);
	LcdSendNibble( LCD_FUNCTION_SET );
	_delay_ms(5);
	LcdSendNibble( LCD_FUNCTION_SET );
	_delay_us(100);
	LcdSendNibble( LCD_FUNCTION_SET );

	// St�ll in displayen till 4-bit l�ge:
	LcdSendNibble( LCD_FUNCTION_SET_4BIT );

	//St�ll in �vriga instruktioner:
	//LcdSendInstruction( LCD_FUNCTION_SET_4BIT );
	LcdSendInstruction( LCD_DISPLAY_OFF );
	_delay_ms(2);
	LcdSendInstruction( LCD_DISPLAY_CLEAR );
	_delay_ms(2);
	LcdSendInstruction( LCD_ENTRY_MODE_SET );
	_delay_ms(2);
	LcdSendInstruction( LCD_DISPLAY_ON );
}
void LcdClear()
{
	LcdSendInstruction( LCD_DISPLAY_CLEAR );
	cursor_pos = 0;
}
void LcdHome()
{
	LcdSendInstruction( LCD_CURSOR_HOME );
	cursor_pos = 0;
}
void LcdSetCursorAt(unsigned char pos)
{
	pos &= 127; // clear high bit
	cursor_pos = pos;
	LcdSendInstruction( 128 + pos ); //write new pos
}

void LcdDispString(char send_string[])
{
	for(volatile int i = 0; send_string[i] != 0; i++)
	{
		/*if (send_string[i] == '�' || send_string[i] == '�')
		{
			LcdSendCharacter(239);		//�? finns ju inte!
		}
		else*/ if (send_string[i] == '�' || send_string[i] == '�')
		{
			LcdSendCharacter(225);		//�
		}else if (send_string[i] == '�' || send_string[i] == '�')
		{
			LcdSendCharacter(239);		//�	
		}
		else
		{
			LcdSendCharacter(send_string[i]);
		}			
		
	}
}

void disp_num(int num)
{
	
	//LcdSendCharacter('T');
	char charchar = 0;
	if (num >= 100)
	{
		charchar = num / 100;
		LcdSendCharacter(charchar + 48);
		num -= charchar * 100;
	}
	if (num >= 10 || charchar)
	{
		char charchar = num / 10;
		LcdSendCharacter(charchar + 48);
		num -= charchar * 10;
	}
	LcdSendCharacter(num + 48);
	
}

void disp_float(float num)
{
	//num = num / 128;	//flyttat till funktionsanrop
	char charchar = 0;
	if(num < 0)
	{
		LcdSendCharacter('-');
		num = -num;
	}
	
	if (num >= 100)
	{
		charchar = num / 100;
		LcdSendCharacter(charchar + 48);
		num -= charchar * 100;
	}
	if (num >= 10 || charchar)
	{
		char charchar = num / 10;
		LcdSendCharacter(charchar + 48);
		num -= charchar * 10;
	}
	LcdSendCharacter(num + 48);
	LcdSendCharacter(46);
	int intint = num;
	int decimals = (num - intint) * 10;
	LcdSendCharacter(decimals + 48);
	
}