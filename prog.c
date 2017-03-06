#include "spi.c"
#include <util/delay.h>

int main()
{
	DDRD |= _BV(5);
	init_spi_master();	//Initialise the device as the master.
	PORTD |= _BV(5);	//Set pin D5 high to indicate successful initialisation.

	while(1)
	{
		//Send 8 pieces of data repeatedly.
		//Simulates behaviour of repeatedly sending sensor data to the transceiver.

		//Send dummy data to indicate the start of a sequence.
		PORTD &= ~_BV(5);	//Set D5 low.
		spi_tx(0xFF);		//First dummy data
		spi_tx(0xFE);		//Second dummy data
		_delay_ms(500);		//Wait after sending dummy data so that...
		PORTD |= _BV(5);	//Set D5 high again, delay to make this visible.

		//Send actual data sequence.
		spi_tx(0x12);
		spi_tx(0x34);
		spi_tx(0x56);
		spi_tx(0x78);
		spi_tx(0x9A);
		spi_tx(0xBC);
		spi_tx(0xDE);
		spi_tx(0xF0);
		_delay_ms(500);
	}
}