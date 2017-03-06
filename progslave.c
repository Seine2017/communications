#include "spi.c"
#include <util/delay.h>

int main()
{	
	DDRD |= _BV(2);	
	DDRD |= _BV(3);		
	DDRD |= _BV(4);
	DDRD |= _BV(5);		
	DDRD |= _BV(6);
	uint8_t buffer[8];	//Buffer to store data.
	uint8_t i;		//Loop variable.

	init_spi_slave();	//Initialise the device as the slave.
	PORTD |= _BV(2);	//Pin D2 set high to indicate successful initialisation.

	for(i=0 ; i<16 ; i++)
	{
		buffer[i] = 0x30 + i;	//Put in some dummy data.
	}

	while(1)
	{
		//To find the start of the sequence, the receiver waits for 0xFF, 0xFE.
		while(1)
		{
			if(spi_rx() == 0xFF)
			{
				PORTD |= _BV(3);		//0xFF received, indicated on pin D3.
				if(spi_rx() == 0xFE)
				{	
					PORTD |= _BV(4);	//0xFE received, indicated on pin D4.
					break;			//Break out of while loop to start recording the actual data.
				}
				else
				{
					_delay_ms(500);		
					PORTD &= ~_BV(3);	//0xFF received but not followed by 0xFE. Not start of sequence. Indicated on pin D3.
					_delay_ms(500);
				}	
			}
		}

		//Send 8 pieces of data.
		for(i=0 ; i<16 ; i++)
		{
			spi_tx(buffer[i]);					
		}		

		PORTD |= _BV(5);	//Turn on pin D5.
		_delay_ms(500);
		PORTD &= ~_BV(5);	//Turn off pin D5. If D5 toggles, SPI sending was successful.
		_delay_ms(500);
			
	}
}