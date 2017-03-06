#include <avr/io.h>

void portInit(void);
uint16_t writeCommand(uint16_t cmd);
void txInit(void);
void sendByte(uint8_t byte);