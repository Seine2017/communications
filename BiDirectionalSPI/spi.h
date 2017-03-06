//spi.h
//Used to use the SPI interface.
//Based on code by Steve Gunn, obtained from the Year 1 C7 lab: https://secure.ecs.soton.ac.uk/notes/ellabs/1/c7/

#include <avr/io.h>
#include <avr/interrupt.h>

void init_spi_master(void);
void init_spi_slave(void);
void init_timer_and_interrupts(void);
void init_pin_change_interrupts(void);
void spi_tx(uint8_t b);
uint8_t spi_rx(void);
uint8_t spi_trx(uint8_t b);