void setup() 
{
  // put your setup code here, to run once:
  
  Serial.begin(9600);       //Open serial port
  
  pinMode(9, OUTPUT);       //LED pin
  
  portInit();               //Initialise RFM12B module ports.
  rxInit();                 //Initialise module as receiver.

  FIFO_reset();
}

void loop() 
{
  // put your main code here, to run repeatedly:

  unsigned char data, i;
  data = 0;
  
  for (i=0; i<16; i++) 
  {
    //Trying to print lots of statements here makes the receiver miss bytes. So only send the data.
    Serial.print("Rx: ");
    data = receiveByte();     //Receive a byte.
    Serial.print(data, HEX);  //Print to the serial monitor.
  }

  FIFO_reset();
  
  if(data == 0x3F)            //Check if the last byte was correct, flash an LED if so.
  {
    digitalWrite(9, HIGH);
    delay(500);
    digitalWrite(9, LOW);
    delay(500);
  }
}

//Initialise ports.
void portInit(void)
{
  pinMode(2, INPUT);    //Set pin 2 to input for nIRQ.
  pinMode(4, OUTPUT);   //Set pin 4 to output for slave select.
  pinMode(5, OUTPUT);   //Set pin 5 to output for MOSI.
  pinMode(6, INPUT);    //Set pin 6 to input for MISO.
  pinMode(7, OUTPUT);   //Set pin 7 to output for SCLK.
}

//Send command to module over SPI.
unsigned int writeCommand(unsigned int cmd)
{
  //NOTE: The commands are 16 bit, SPI is 8 bit. Hence, the SPI is handled manually.
    unsigned char i;              //for loop variable.
    unsigned int temp = 0;        //Received data if reading the status register.
    digitalWrite(7, LOW);         //Set SCLK low.
    digitalWrite(4, LOW);         //Set slave select low to select the module.
    for(i=0 ; i<16 ; i++)
    {
      if(cmd & 0x8000)            //If command's MSB is high...
      {
        digitalWrite(5, HIGH);    //Set MOSI high.
      }
      else                        //Else...
      {
        digitalWrite(5, LOW);     //Set MOSI low.
      }   
 
      digitalWrite(7, HIGH);      //Set SCLK to clock the module.
      temp<<=1;                   //Shift temp left by 1.
      
      if(digitalRead(6))          //If MISO is high...
      {
        temp |= 0x0001;           //Set LSB of temp to 1. It will be shifted on the next loop.
      }
      digitalWrite(7, LOW);       //Set SCLK low.
      cmd<<=1;                    //Shift cmd left by 1. Means the new MSB is the next bit which gets sent via MOSI on the next loop.
    }
    digitalWrite(4, HIGH);        //Set slave select high again to deselect the module.
    return(temp);                 //Return the contents of the status register if it was read. 
}

//Initialise the RFM12B module.
void rxInit(void) //The only thing different between transmitter and receiver is the second command. Receiver enabled instead of transmitter.
{
  writeCommand(0x80D8); //Enable transmitter register and receiver FIFO buffer. Use 433 MHz band. Use 12.5 pF load capacitor.
  writeCommand(0x8299); //Enable receiver, low battery detector, synthesiser and crystal oscillator.
                        //Disable transmitter, base band block, wake up timer and clock output of clock pin.
  writeCommand(0xA640); //Use 434 MHz frequency.
  writeCommand(0xC647); //Use a data rate of 4.8 kbps.
  writeCommand(0x94A0); //Set pin 16 to VDI output. Use 134 kHz baseband bandwidth. Set VDI response time to fast.
                        //Use LNA gain of 0dBm. Use DRSSI threshold of -103dBm.
  writeCommand(0xC2AC); //Enable clock recovery auto-lock. Disable clock recovery fast mode. Use digital data filter.
                        //Use DQD threshold of 4.
  writeCommand(0xCA81); //Use FIFO interrupt level of 8. Use 2DD4 as synchronisation pattern. 
                        //Disable FIFO fill. Enable high sensitivity reset mode.
  writeCommand(0xCED4); //Use 2DD4 as synchronisation pattern.
  writeCommand(0xC483); //Keep offset when VDI high. Do not restrict frequency range.
                        //Disable AFC high accuracy mode. Enable AFC output register and function.
  writeCommand(0x9850); //Use 90 kHz frequency deviation. Use maximum output power (0 dBm).
  writeCommand(0xCC17); //Use 5 or 10 MHz (recommended) for microcontroller clock frequency.
                        //Disable PLL loop dithering. Disable phase detector delay. Use higher PLL bandwidth.
  writeCommand(0xE000); //Unused since wake-up timer is disabled.
  writeCommand(0xC800); //Unused since low duty cycle mode is disabled.
  writeCommand(0xC040); //Use 1.66 MHz clock frequency on the CLK pin. Use 2.2V for the low battery detector threshold.
}

//Wait for and retrieve a received byte from the FIFO buffer.
//Can replace the wait with an interrupt if needed.
uint8_t receiveByte(void)
{
  unsigned int FIFO_data, status;
  while(1)
  {
    status = writeCommand(0x0000);      //Check the status register.
    if(status & 0x8000)                 //If the MSB of the status register is set, we received something.
    {
      FIFO_data = writeCommand(0xB000); //Get received data.
      return (FIFO_data & 0x00FF);      //Extract the byte from the received data.
    }
  }  
} 

//Toggles the FIFO buffer to clear it out.
void FIFO_reset(void)
{
  writeCommand(0xCA81);
  writeCommand(0xCA83);
}
