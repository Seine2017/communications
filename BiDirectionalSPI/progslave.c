#include "spi.c"
#include <util/delay.h>

int main()
{	
	uint8_t buffer[8];	//Buffer to store data for receiving.
	uint8_t i;		//Loop variable.

	DDRD |= _BV(1);		//"Synced" connection. When 0xFF 0xFE is received, need to tell the master this happened.
	PORTD &= ~_BV(1);

	DDRD |= _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5) | _BV(6);	//Outputs to indicate what is happening.

	init_spi_slave();	//Initialise the device as the slave.
	PORTD |= _BV(2);	//Pin D2 set high to indicate successful initialisation.

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

		PORTD |= _BV(1);
		//Send 8 pieces of data.
		for(i=0 ; i<8 ; i++)
		{
			buffer[i] = spi_trx(0x30 + i);					
		}
		PORTD &= ~_BV(1);		

		PORTD |= _BV(5);	//Turn on pin D5.

		//Confirm correct data.
		if(buffer[2] == 0x56)
		{
			PORTD |= _BV(6);
		}

		_delay_ms(500);
		PORTD &= ~_BV(5);	//Turn off pin D5. If D5 toggles, SPI sending was successful.
		PORTD &= ~_BV(6);
		_delay_ms(500);
			
	}
}