#include <avr/io.h>

void portInit(void);
uint16_t writeCommand(uint16_t cmd);
void rxInit(void);
uint8_t receiveByte(void);
void FIFO_reset(void);