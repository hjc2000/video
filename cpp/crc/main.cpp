#include<bit>
#include<iostream>
#include<jccpp/Bit.h>
#include<jccpp/math/ModbusCrc16.h>
#include<stdint.h>

int main(void)
{
	jccpp::ModbusCrc16 crc16;
	crc16.Add(0x01);
	crc16.Add(0x10);
	crc16.Add(0x40);
	crc16.Add(0x00);
	crc16.Add(0x00);
	crc16.Add(0x02);
	crc16.Add(0x04);
	crc16.Add(0x00);
	crc16.Add(0x00);
	crc16.Add(0x00);
	crc16.Add(0x01);
	std::cout << crc16.RegisterValue() << std::endl;
}
