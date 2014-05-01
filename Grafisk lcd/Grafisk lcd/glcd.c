
#include <stdbool.h>

//Portdefinitioner
#define dataport PORTB
#define flagport PORTD
#define x_size 320
#define y_size 240

//pins för grafisk lcd
#define CS 5
#define Rdd 6
#define Wrr 7
#define RS 4

//färger
#define red 0xe0
#define green 0x1c
#define blue 0x03
#define yellow 0xfc
#define white 0xff
#define black 0x00

bool even_tens(int num);
void LCD_write_cmd(unsigned char data);
void LCD_write_data(unsigned char data);
void LCD_write_adr(unsigned int x, unsigned int y);
void disp_all(unsigned char color);	//fyll skärmen med en bakgrundsfärg

//io funktioner för mer läsbar kod
void toggle_Wrr();
void clear_CS();
void set_CS();
void clear_Wrr();
void set_Wrr();
void clear_Rdd();
void set_Rdd();
void clear_RS();
void set_RS();

void LCD_write_char(unsigned char tecken, char color, int x, int y);	//rita ett litet tecken
void LCD_write_big_char(unsigned char tecken, char color, int x, int y, int size);	//rita ett tecken med storlek size
void LCD_write_big_char_row(char row, char color, int x, int y, int size);	//skriv én kolumn av en bokstav

void LCD_write_string(char * message, char color, int x, int y);		//skriv text i minsta storleken
void LCD_write_big_string(char * message, char color, int x, int y, int size);	//loopa ut en sträng och hantera teckenspacing och wordwrap
void LCD_draw_x_line(char color, int y, int length);			//rita en linje på höjd y med längd length
void LCD_draw_x_dotted_line(char color, int y, int length);		//rita en streckad linje på höjd y med längd length
bool even_nums(int num, int divisor);							//utför float mot int division och se om det ger samma svar
bool even_tens(int num);										//kolla efter jämna tiotal

void LCD_draw_graph(char color, int y_offset, float y_graph_size, float * arr, int x_width);		//rita en graf på skärmen 




//array med tecken till grafisk lcd
const unsigned char TEXT[100][5] ={	
0x00, 0x00, 0x00, 0x00, 0x00, // SPACE
0x00, 0x00, 0x5F, 0x00, 0x00, // !
0x00, 0x03, 0x00, 0x03, 0x00, // "
0x14, 0x3E, 0x14, 0x3E, 0x14, // #
0x24, 0x2A, 0x7F, 0x2A, 0x12, // $
0x43, 0x33, 0x08, 0x66, 0x61, // %
0x36, 0x49, 0x55, 0x22, 0x50, // &
0x00, 0x05, 0x03, 0x00, 0x00, // '
0x00, 0x1C, 0x22, 0x41, 0x00, // (
0x00, 0x41, 0x22, 0x1C, 0x00, // )
0x14, 0x08, 0x3E, 0x08, 0x14, // *
0x08, 0x08, 0x3E, 0x08, 0x08, // +
0x00, 0x50, 0x30, 0x00, 0x00, // ,
0x08, 0x08, 0x08, 0x08, 0x08, // -
0x00, 0x60, 0x60, 0x00, 0x00, // .
0x20, 0x10, 0x08, 0x04, 0x02, // /
0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
0x04, 0x02, 0x7F, 0x00, 0x00, // 1
0x42, 0x61, 0x51, 0x49, 0x46, // 2
0x22, 0x41, 0x49, 0x49, 0x36, // 3
0x18, 0x14, 0x12, 0x7F, 0x10, // 4
0x27, 0x45, 0x45, 0x45, 0x39, // 5
0x3E, 0x49, 0x49, 0x49, 0x32, // 6
0x01, 0x01, 0x71, 0x09, 0x07, // 7
0x36, 0x49, 0x49, 0x49, 0x36, // 8
0x26, 0x49, 0x49, 0x49, 0x3E, // 9
0x00, 0x36, 0x36, 0x00, 0x00, // :
0x00, 0x56, 0x36, 0x00, 0x00, // ;
0x08, 0x14, 0x22, 0x41, 0x00, // <
0x14, 0x14, 0x14, 0x14, 0x14, // =
0x00, 0x41, 0x22, 0x14, 0x08, // >
0x02, 0x01, 0x51, 0x09, 0x06, // ?
0x3E, 0x41, 0x59, 0x55, 0x5E, // @
0x7E, 0x09, 0x09, 0x09, 0x7E, // A
0x7F, 0x49, 0x49, 0x49, 0x36, // B
0x3E, 0x41, 0x41, 0x41, 0x22, // C
0x7F, 0x41, 0x41, 0x41, 0x3E, // D
0x7F, 0x49, 0x49, 0x49, 0x41, // E
0x7F, 0x09, 0x09, 0x09, 0x01, // F
0x3E, 0x41, 0x41, 0x49, 0x3A, // G
0x7F, 0x08, 0x08, 0x08, 0x7F, // H
0x00, 0x41, 0x7F, 0x41, 0x00, // I
0x30, 0x40, 0x40, 0x40, 0x3F, // J
0x7F, 0x08, 0x14, 0x22, 0x41, // K
0x7F, 0x40, 0x40, 0x40, 0x40, // L
0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
0x7F, 0x02, 0x04, 0x08, 0x7F, // N
0x3E, 0x41, 0x41, 0x41, 0x3E, // O
0x7F, 0x09, 0x09, 0x09, 0x06, // P
0x1E, 0x21, 0x21, 0x21, 0x5E, // Q
0x7F, 0x09, 0x09, 0x09, 0x76,/*};

const unsigned char TEXT2[44][5] ={*/
0x26, 0x49, 0x49, 0x49, 0x32, // S
0x01, 0x01, 0x7F, 0x01, 0x01, // T
0x3F, 0x40, 0x40, 0x40, 0x3F, // U
0x1F, 0x20, 0x40, 0x20, 0x1F, // V
0x7F, 0x20, 0x10, 0x20, 0x7F, // W
0x41, 0x22, 0x1C, 0x22, 0x41, // X
0x07, 0x08, 0x70, 0x08, 0x07, // Y
0x61, 0x51, 0x49, 0x45, 0x43, // Z
0x00, 0x7F, 0x41, 0x00, 0x00, // [
0x02, 0x04, 0x08, 0x10, 0x20, // \//
0x00, 0x00, 0x41, 0x7F, 0x00, // ]
0x04, 0x02, 0x01, 0x02, 0x04, // ^
0x40, 0x40, 0x40, 0x40, 0x40, // _
0x00, 0x01, 0x02, 0x04, 0x00, // `
0x20, 0x54, 0x54, 0x54, 0x78, // a
0x7F, 0x44, 0x44, 0x44, 0x38, // b
0x38, 0x44, 0x44, 0x44, 0x44, // c
0x38, 0x44, 0x44, 0x44, 0x7F, // d
0x38, 0x54, 0x54, 0x54, 0x18, // e
0x04, 0x04, 0x7E, 0x05, 0x05, // f
0x08, 0x54, 0x54, 0x54, 0x3C, // g
0x7F, 0x08, 0x04, 0x04, 0x78, // h
0x00, 0x44, 0x7D, 0x40, 0x00, // i
0x20, 0x40, 0x44, 0x3D, 0x00, // j
0x7F, 0x10, 0x28, 0x44, 0x00, // k
0x00, 0x41, 0x7F, 0x40, 0x00, // l
0x7C, 0x04, 0x78, 0x04, 0x78, // m
0x7C, 0x08, 0x04, 0x04, 0x78, // n
0x38, 0x44, 0x44, 0x44, 0x38, // o
0x7C, 0x14, 0x14, 0x14, 0x08, // p
0x08, 0x14, 0x14, 0x14, 0x7C, // q
0x00, 0x7C, 0x08, 0x04, 0x04, // r
0x48, 0x54, 0x54, 0x54, 0x20, // s
0x04, 0x04, 0x3F, 0x44, 0x44, // t
0x3C, 0x40, 0x40, 0x20, 0x7C, // u
0x1C, 0x20, 0x40, 0x20, 0x1C, // v
0x3C, 0x40, 0x30, 0x40, 0x3C, // w
0x44, 0x28, 0x10, 0x28, 0x44, // x
0x0C, 0x50, 0x50, 0x50, 0x3C, // y
0x44, 0x64, 0x54, 0x4C, 0x44, // z
0x00, 0x08, 0x36, 0x41, 0x41, // {
0x00, 0x00, 0x7F, 0x00, 0x00, // |
0x41, 0x41, 0x36, 0x08, 0x00, // }
0x02, 0x01, 0x02, 0x04, 0x02,	// ~
0x20, 0x55, 0x54, 0x55, 0x78, // ä
0x20, 0x54, 0x55, 0x54, 0x78, // å
0x38, 0x45, 0x44, 0x45, 0x38 // ö

							 };


void LCD_write_cmd(unsigned char data)	//rutin för att skicka kommando
{
	clear_CS();		//Cs=0
	set_Rdd();				//R=1
	clear_RS();				//Rs=0
	dataport = data;
	toggle_Wrr();				//W=0
								//W=1
	set_CS();				//Cs=1
}
void LCD_write_data(unsigned char data)	//skriv data
{

	clear_CS();		//Cs=0
	set_Rdd();				//R=1
	set_RS();				//Rs=1
	dataport = data;
	toggle_Wrr();				//W=0
	//W=1
	set_CS();				//Cs=1
	
	
}

void LCD_write_adr(unsigned int x, unsigned int y)	//skriv en adress till LCD
{
	unsigned int xhigh, xlow = 0;
	
	xhigh = x / 256;	//filtrera ut hög och låg adress
	xlow = x % 256;
	
	LCD_write_cmd(xhigh);
	
	LCD_write_cmd(xlow);
	
	LCD_write_cmd(y);
}

void disp_all(unsigned char color)	//fyll skärmen med en bakgrundsfärg
{
	LCD_write_adr(0,0);
	unsigned char	C = color;
	for (int i = 0; i < 240; i++)		//rader
	{
		
		for (int u = 0; u < 320; u++)	//pixlar
		{	
			LCD_write_data(C);
		}
	}
}


//io funktioner för mer läsbar kod
void toggle_Wrr()
{
	clear_Wrr();
	set_Wrr();
}
void clear_CS()
{
	flagport = flagport & ~( 1 << CS );
}
void set_CS()
{
	flagport = flagport | ( 1 << CS );
}
void clear_Wrr()
{
	flagport = flagport & ~( 1 << Wrr );
}
void set_Wrr()
{
	flagport = flagport | ( 1 << Wrr );
}
void clear_Rdd()
{
	flagport = flagport & ~( 1 << Rdd );
}
void set_Rdd()
{
	flagport = flagport | ( 1 << Rdd );
}
void clear_RS()
{
	flagport = flagport & ~( 1 << RS );
}
void set_RS()
{
	flagport = flagport | ( 1 << RS );
}


void LCD_write_char(unsigned char tecken, char color, int x, int y)	//rita ett litet tecken
{
	LCD_write_big_char(tecken,color,x,y,1);
}

void LCD_write_big_char(unsigned char tecken, char color, int x, int y, int size)//rita ett tecken med storlek size
{
	
	
	if(tecken == 'ä' || tecken == 'Ä')	//filtrera lite mot skandinaviska tecken
	{
		tecken = 51+44;
	}else if(tecken == 'å' || tecken == 'Å')
	{
		tecken = 51+45;
	
	}else if(tecken == 'ö' || tecken == 'Ö')
	{
		tecken = 51+46;
		
	}else if(tecken > 31){
		
		tecken -= 32;		//32, offset mot ascii til char arrayen
	}else{
		tecken = 51+46; //ööö
	}
	
	for (int i = 0; i < 5; i++)
	{

		LCD_write_big_char_row( TEXT[tecken][i], color, x + i*size, y, size);

		//LCD_write_big_char_row( tecken, color, x + i, y+18);		//Debug pryl, binärt värde
		
	}
	LCD_write_big_char_row( 0, color, x + 5*size, y, size);			//fyll ut utrymme mellan bokstäver
}
void LCD_write_big_char_row(char row, char color, int x, int y, int size)	//skriv en kolumn av en bokstav
{
	for (int i = 0; i < 8; i++)	//loopa igenom column
	{
		
		if ((row & ( 1 << i )) != 0)		//	förgrundsfärg eller bakgrund?
		{
			for (int j = 0; j < size; j++)		//	rita en pixel i fyrkant med storleken size
			{
				LCD_write_adr(x, y + ( i * size + j ));
				for (int k = 0; k < size; k++ )
				{
					LCD_write_data(color);
				}
			}			
		}
		else
		{
			for (int j = 0; j < size; j++)	//fyll i bakgrundsfärg i rätt pixelstorlek
			{
				LCD_write_adr(x, y + ( i * size + j ));
				for (int k = 0; k < size; k++ )
				{
					LCD_write_data(white);
				}
			}
		}
	}
}

void LCD_write_string(char * message, char color, int x, int y)		//skriv text i minsta storleken
{
	LCD_write_big_string(message,color,x,y,1);	
}
void LCD_write_big_string(char * message, char color, int x, int y, int size)	//loopa ut en sträng och hantera teckenspacing och wordwrap
{
	for (int i = 0; message[i] != 0; i++)
	{
		//wordwrap
		if (x + (size * 6) * (i + 1) >= x_size )
		{
			x = - ( i * (6 * size) );
			y += ( 8 * size );
		}
		LCD_write_big_char(message[i], color, x + ( size * 6 * i ), y, size);
	}
}

void LCD_draw_x_line(char color, int y, int length)	//rita en linje på höjd y med längd length
{

	for (int i = 0; i < length; i++)
	{
		LCD_write_adr(i, y);
		LCD_write_data(color);
	}
}
void LCD_draw_x_dotted_line(char color, int y, int length) //rita en streckad linje på höjd y med längd length
{
	
	for (int i = 0; i < length; i++)
	{
		if (even_tens(i))	//int mot float division
		{
			LCD_write_adr(i, y);
			LCD_write_data(color);
		}
		
	}
}
bool even_nums(int num, int divisor)	//utför float mot int division och se om det ger samma svar
{
	float in_float = divisor;
	
	if (num/divisor == num/in_float)	//sant om lika
		return true;
	return false;
}
bool even_tens(int num)		//kolla efter jämna tiotal
{
	return even_nums(num,10); 
}

void LCD_draw_graph(char color, int y_offset, float y_graph_size, float * arr, int x_width)		//rita en graf på skärmen 
{
	int bottom =  y_offset + y_graph_size;
	float topvalue = arr[0], bottomvalue = arr[0];
	
	#define top_bottom_offset 2	//offset mot top och botten för snyggare graf
	
	//char graph_width;
	#define graph_width x_width * 2		//en grej som ger finare graph, i väntan på standardisering
	
	LCD_draw_x_line(0x00, y_offset, graph_width );		//rita outline
	LCD_draw_x_line(0x00, bottom, graph_width );	
	
	
	//loop för att hitta topp och bottenvärden
	for (int i = 0; i < x_width; i++)
	{
		float temp = arr[i];
		if (temp > topvalue)
		{	
			topvalue = temp;
		}
		if (temp < bottomvalue)
		{
			bottomvalue = temp;
		}
	}
	topvalue += top_bottom_offset;		//lägg till en offset för snyggare graf
	bottomvalue -= top_bottom_offset;
	
	// välj range att använda	
	float range = topvalue - bottomvalue;
	int grid_scale = range / 3;	//skala grid mot range, rita tre linjer eller fyra
	float scale;
		
	//rita graf
	for (int i = 0; i < x_width; i++)
	{
		for (int j = 1; j < y_graph_size; j++)
		{
			LCD_write_adr(i*2, bottom - j);	//sätt position för datapunkt
			LCD_write_data(white);				//skriv bakgrund
			LCD_write_data(white);		
		}
		
		
		scale =  ( y_graph_size / range) * (arr[i] - bottomvalue);	//skala datapunkt
		
		LCD_write_adr(i*2, bottom - scale);	//sätt position för datapunkt
		
		LCD_write_data(color);	//skriv punkt
		LCD_write_data(color);	//skriv punkt
	}
	
	//skriv ut top och bottenvärde
	LCD_write_string(float_to_str(topvalue), color, graph_width , y_offset + 1);		//*2 som fulhack för att få bättre läsbarhet
	LCD_write_string(float_to_str(bottomvalue), color, graph_width , bottom - 8);

	//loop för grid
	for (int i = bottomvalue; i < topvalue; i++)
	{
		
		if (even_nums(i, grid_scale))	//kolla om vi ska rita en linje, leta jämna tal utifrån grid skalning
		{
			int temp = (i - bottomvalue) * ( (y_graph_size) / range);
			if (i != 0)
			{
				LCD_draw_x_dotted_line(black, bottom - temp, graph_width);
			} 
			else
			{
				LCD_draw_x_line(black, bottom - temp, graph_width);		//rita nolla om den är med
			}
			
			
			i += grid_scale - 1;		//hoppa jämna tiotal
		}

	}

}
