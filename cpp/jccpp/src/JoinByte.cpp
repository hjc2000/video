#include "jccpp/JoinByte.h"

uint16_t JoinByte(uint8_t high, uint8_t low)
{
	return ((uint16_t)high << 8) | low;
}

uint32_t JoinByte(uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0)
{
	uint16_t high = JoinByte(b3, b2);
	uint16_t low = JoinByte(b1, b0);
	return JoinUint16(high, low);
}

uint32_t JoinUint16(uint16_t high, uint16_t low)
{
	return ((uint32_t)high << 16) | low;
}
