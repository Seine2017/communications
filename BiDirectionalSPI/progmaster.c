#include "spi.c"
#include <util/delay.h>

int main()
{
	uint8_t buffer[8];	//Buffer to store data for receiving.

	DDRD |= _BV(4) | _BV(5) | _BV(6);
	init_spi_master();	//Initialise the device as the master.
	PORTD |= _BV(4);	//Set pin D4 high to indicate successful initialisation.

	while(1)
	{
		//Send 8 pieces of data repeatedly.
		//Simulates behaviour of repeatedly sending sensor data to the transceiver.

		//Send dummy data to indicate the start of a sequence.
		PORTD &= ~_BV(5);	//Set D5 low.
		while( !(PIND & _BV(1)) )
		{
			spi_tx(0xFF);		//First dummy data
			_delay_ms(5);		//Small delay in case slave needs to do something.
			spi_tx(0xFE);		//Second dummy data
			_delay_ms(5);
		}
		_delay_ms(500);		//Wait after sending dummy data so that...
		PORTD |= _BV(5);	//Set D5 high, delay to make this visible.

		//Send actual data sequence.
		buffer[0] = spi_trx(0x12);
		buffer[1] = spi_trx(0x34);
		buffer[2] = spi_trx(0x56);
		buffer[3] = spi_trx(0x78);
		buffer[4] = spi_trx(0x9A);
		buffer[5] = spi_trx(0xBC);
		buffer[6] = spi_trx(0xDE);
		buffer[7] = spi_trx(0xF0);

		//Confirm correct data.
		if((buffer[5] == 0x35))
		{
			PORTD |= _BV(6);
		}

		_delay_ms(500);
		PORTD &= ~_BV(6);
	}
}