//spi.c
//Used to use the SPI interface.
//Author: Kiran Patel
//
//Contains code by Steve Gunn, obtained from the Year 1 C7 lab: https://secure.ecs.soton.ac.uk/notes/ellabs/1/c7/

#include "spi.h"

void init_spi_master(void)
{
	DDRB = _BV(PB4) | _BV(PB5) | _BV(PB7);	  //Set Slave Select (/SS), MOSI and SCLK to outputs.
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);  //Enable SPI, configure this device as master, set F_SCK = F_CPU/16 = 750 kHz.
	//init_timer_and_interrupts();
}

void init_spi_slave(void)
{
	DDRB = _BV(PB6);	 		  //Set MISO to output. 
	SPCR = _BV(SPE);  		 	  //Enable SPI.
	//init_pin_change_interrupts();
}

void init_timer_and_interrupts(void)
{
 	TCCR1A = 0;
 	TCCR1B = _BV(WGM12);
 
 	TCCR1B |= _BV(CS12) | _BV(CS10);	  //Use 500ms as the interrupt period.
 	OCR1A = 5859;				  //ie 20 Hz interrupt frequency.

 	TIMSK1 |= _BV(OCIE1A);			  //Enable timer interrupt.
 	sei();					  //Enable global interrupts.
}

void init_pin_change_interrupts(void)
{
	EICRA |= _BV(ISC01);			  //Set to trigger on falling edge.
	EIMSK |= _BV(INT0);			  //Enable pin change interrupt on D0.
	sei();					  //Enable global interrupts.
}

void spi_tx(uint8_t b)
{
	SPDR = b;				  //Write data to the SPI data register. Initiates transmission.
	while(!(SPSR & _BV(SPIF)));		  //Wait for SPIF to go high, indicates transfer complete.
}
	
uint8_t spi_rx(void) 
{
	while(!(SPSR & _BV(SPIF)));		  //Wait for SPIF to go high, indicates transfer complete.
	return SPDR;				  //Read received data from SPI data register, return it to be used.
}

uint8_t spi_trx(uint8_t b)
{
	SPDR = b;				  //Write data to the SPI data register. Initiates transmission.
	while(!(SPSR & _BV(SPIF)));		  //Wait for SPIF to go high, indicates transfer complete.
	return SPDR;				  //Read received data from SPI data register, return it to be used.
}