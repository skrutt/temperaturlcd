#define DDR_SPI DDRB
#define DD_MOSI 4
#define DD_SCK 5
#define CS 2
void SPI_init();
void SPI_masterTransmit(char data);
void SPI_waitTransmit();



void SPI_init()
{
	DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK)|(1<<CS); //MOSI och SCK Input
	PORTB |= 1<<CS;
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0); //enable SPI, Master, set clock rate fck/16	
}

void SPI_masterTransmit(char data) 
{
	SPDR = data;
	SPI_waitTransmit();
}

void SPI_waitTransmit() 
{
	while(~SPSR & (1<<SPIF)) {
	}
}

char SPI_read() 
{
	SPI_masterTransmit(1);
	char data = SPDR;
	return data;
}

int SPI_readWord() 
{
	PORTB &= ~(1<<CS);
	char high = SPI_read();
	char low = SPI_read();
	PORTB |= 1<<CS;
	//int sign = high & 16;

	/*if(high & 128)	//onödigt ju!! ingen effekt
	{
		//-2^13 + värde
		high &= 127;
		return -32768 + ( high * 256 ) + low ;

	}*/
	//high &= 127;
	//return low*256;
	return high*256 + (low & 248) ;
}