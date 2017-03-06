#include "receiver.c"
#include <util/delay.h>

int main(void) 
{
	unsigned char data, i;
	data = 0;
	
	DDRD |= _BV(5) | _BV(6);
	
	//Initialisations. Haven't initialised regular SPI here since portInit() handles it.
	portInit();		//Initialise RFM12B ports (including SPI interface).
	rxInit();		//Initialise RFM12B as receiver.

	FIFO_reset();
	
	PORTD |= _BV(5);	//Light LED to indicate successful initialisation.

	while(1) 
	{	
		for (i=0; i<16; i++) 
		{
			data = receiveByte();	//Receive data bytes. 
						//The module automatically filters out preamble, sync and dumy bytes.	
		}

		FIFO_reset();

		if(data == 0x3F)		//Flash an LED if the last data byte was correct.
		{
			PORTD |= _BV(6);
			_delay_ms(500);
			PORTD &= ~_BV(6);
			_delay_ms(500);
		}
	}
}