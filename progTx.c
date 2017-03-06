#include "spi.c"
#include "transmitter.c"
#include <util/delay.h>

int main(void) 
{
	uint8_t i, data;
	uint8_t buffer[8];

	_delay_ms(50);

	DDRD |= _BV(4) | _BV(5) | _BV(6);	
	
	//Initialisations. NOTE: The microcontrollers and RFM12B use SEPARATE SPI PORTS.
	init_spi_master();	//Initialise SPI.	
	portInit();		//Initialise RFM12B ports.
	txInit();		//Initialise RFM12B as transmitter.

	PORTD |= _BV(4);	//Light LED to indicate successful initialisation.	

	while(1)
	{
	//SPI section.

		//Send dummy data to indicate the start of a sequence.
		PORTD |= _BV(5);	//Set D5 high.
		spi_tx(0xFF);		//First dummy data
		spi_tx(0xFE);		//Second dummy data
		_delay_ms(500);		//Wait after sending dummy data so that...
		PORTD &= ~_BV(5);	//Set D5 low again, delay to make this visible.
		
		//"Send" more data. This is just to start the transfer process so we can receive something from the slave.
		for(i=0 ; i<16 ; i++)
		{
			buffer[i] = spi_trx(0xBB);
		}
	

	//Transmitter section.

		data = writeCommand(0x0000);	//Read status register.
		
		PORTD |= _BV(6);		//Light LED at start of transmission.
		
		sendByte(0xAA); 		// PREAMBLE
		sendByte(0xAA);
		sendByte(0xAA);
		sendByte(0x2D); 		// SYNC
		sendByte(0xD4);
		for(i=0; i<16; i++) 
		{
			sendByte(buffer[i]);	// DATA
		}
		sendByte(0xAA); 		// DUMMY BYTES
		sendByte(0xAA);
		sendByte(0xAA);

		_delay_ms(500);
		PORTD &= ~_BV(6);		//Turn off LED after transmission ends.
		_delay_ms(500);
	}
}