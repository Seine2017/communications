#include "transmitter.h"

//Initialise ports.
void portInit(void)
{
	DDRD &= ~_BV(2);			//nIRQ input to indicate previous transmission ended.
	DDRA |= _BV(4) | _BV(5) |  _BV(7);	//Put transmitter's SPI interface on port A.
}

uint16_t writeCommand(uint16_t cmd)
{
	//NOTE: The commands are 16 bit, SPI is 8 bit. Hence, need to do SPI manually.
 	  uint8_t i;			//for loop variable.
 	  uint16_t temp = 0;		//Received data if reading the status register.

 	  PORTA &= ~_BV(7); 			//Set SCLK low.
 	  PORTA &= ~_BV(4);			//Set slave select low to select the RFM12B module.
 	  for(i=0 ; i<16 ; i++)
	  {
 		if(cmd & 0x8000)		//If the MSB of the command is high...
		{
 			PORTA |= _BV(5);	//set MOSI high.
 		}
		else				//Else...
		{
 			PORTA &= ~_BV(5);	//set MOSI low.
 		} 
 

 		PORTA |= _BV(7);		//Set SCLK high to clock the RFM12B module.
		temp <<= 1;			//Shift temp left by 1.

		if(PINA & _BV(6))		//If MISO is high...
		{
 			temp |= 0x0001;		//set the LSB of temp to 1. Implied else means it is set to 0.
 		}
 		PORTA &= ~_BV(7); 		//Set SCLK low;
 		cmd <<= 1;			//Shift the command left by 1. The new MSB is the next bit which is used in the next loop.
 	  }
 	  PORTA |= _BV(4);			//Set slave select high to deselect the RFM12B module.
 	  return(temp);				//Return the contents of the status register if it was read.
	
}

//Initialise the RFM12B module.
void txInit(void)
{
 	writeCommand(0x80D8);	//Enable transmitter register and receiver FIFO buffer. Use 433 MHz band. Use 12.5 pF load capacitor.
 	writeCommand(0x8239);	//Enable transmitter, synthesiser and crystal oscillator.
				//Disable receiver, base band block, low battery detector, wake up timer and clock output of clock pin.
 	writeCommand(0xA640);	//Use 434 MHz frequency.
 	writeCommand(0xC647);	//Use a data rate of 4.8 kbps.
 	writeCommand(0x94A0);	//Set pin 16 to VDI output. Use 134 kHz baseband bandwidth. Set VDI response time to fast.
				//Use LNA gain of 0dBm. Use DRSSI threshold of -103dBm.
 	writeCommand(0xC2AC);	//Enable clock recovery auto-lock. Disable clock recovery fast mode. Use digital data filter.
				//Use DQD threshold of 4.
 	writeCommand(0xCA81);	//Use FIFO interrupt level of 8. Use 2DD4 as synchronisation pattern. 
				//Disable FIFO fill. Enable high sensitivity reset mode.
	writeCommand(0xCED4);	//Use 2DD4 as synchronisation pattern.
 	writeCommand(0xC483);	//Keep offset when VDI high. Do not restrict frequency range.
				//Disable AFC high accuracy mode. Enable AFC output register and function.
 	writeCommand(0x9850);	//Use 90 kHz frequency deviation. Use maximum output power (0 dBm).
 	writeCommand(0xCC17);	//Use 5 or 10 MHz (recommended) for microcontroller clock frequency.
				//Disable PLL loop dithering. Disable phase detector delay. Use higher PLL bandwidth.
 	writeCommand(0xE000);	//Unused since wake-up timer is disabled.
 	writeCommand(0xC800);	//Unused since low duty cycle mode is disabled.
 	writeCommand(0xC040);	//Use 1.66 MHz clock frequency on the CLK pin. Use 2.2V for the low battery detector threshold.
}

void sendByte(uint8_t byte)
{
 	while(PIND & _BV(2));		//Test value of pin D2 (nIRQ). Wait for previous transmission to end.
 	writeCommand(0xB800 + byte);	//Write byte to the transmitter register for transmission.
} 
