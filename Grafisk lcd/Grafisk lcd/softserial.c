

//dododo
#include <avr/interrupt.h>


#define port PORTC
#define ddr  DDRC
#define pin	 16

volatile char data_to_send = 0;
volatile char state = 0;
volatile bool ready = true;


void init_serial()
{
	ddr |= pin;		//make output
	port |= pin;	//set high
	
	// Turn on timer with no prescaler on the clock for fastest
	// triggering of the interrupt service routine.
	TCCR1B |= 1 ;
	TIMSK1 |= _BV(TOIE1);
	// Turn interrupts on.
	sei();
}

void send_char(char C)
{
	while (!ready)		//vänta på att data är skickad
	{	//nop
	}
	data_to_send = C;	
	ready = false;		// börja skicka
}
void send_string(char *s)  //Skriver ut en sträng
{
	while(*s)
	{
		send_char(*s);	//Skriv ut bokstaven
		s++;			//Öka char-pointern
	}
}

ISR(TIMER1_OVF_vect) 
{
	
	TCNT1 = 64720;		//sätt in ett värde i räknaren för att få rätt period
	if(ready == false)
	{
		switch(state)
		{
			
			case 0 :		//startbit
				port &= ~pin;
				state++;
				break;
				
			case 1 :		//databit ett
				
				if (data_to_send & 1)
				{
					port |= pin;
				}
				else
				{
					port &= ~pin;
				}
				state++;
				break;	
				
			case 2:		//databit ett
			
				if (data_to_send & 2)
				{
					port |= pin;
				} 
				else
				{
					port &= ~pin;
				}
				state++;
				break;
					
			case 3:		//databit ett
			
				if (data_to_send & 4)
				{
					port |= pin;
				} 
				else
				{
					port &= ~pin;
				}
				state++;
				break;
				
			case 4:		//databit ett
			
				if (data_to_send & 8)
				{
					port |= pin;
				} 
				else
				{
					port &= ~pin;
				}
				state++;
				break;
				
			case 5:		//databit ett
			
				if (data_to_send & 16)
				{
					port |= pin;
				} 
				else
				{
					port &= ~pin;
				}
				state++;
				break;
				
			case 6:		//databit fem
			
				if (data_to_send & 32)
				{
					port |= pin;
				} 
				else
				{
					port &= ~pin;
				}
				state++;
				break;
				
			case 7:		//databit sex
			
				if (data_to_send & 64)
				{
					port |= pin;
				} 
				else
				{
					port &= ~pin;
				}
				state++;
				break;
					
			case 8:		//databit sju
			
				if (data_to_send & 128)
				{
					port |= pin;
				} 
				else
				{
					port &= ~pin;
				}
				state++;
				break;
					
			case 9:		//stopbit
			
				port |= pin;
				state = 0;
				ready = true;
				break;
		}
		
	}
	
}


	
	
	