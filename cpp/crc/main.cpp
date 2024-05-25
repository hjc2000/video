#include<bit>
#include<format>
#include<iostream>
#include<jccpp/communicate/ModbusCrc16.h>
#include<stdint.h>

int main(void)
{
	jccpp::ModbusCrc16 crc16;
	crc16.Add(0x01);
	crc16.Add(0x03);
	crc16.Add(0x04);
	crc16.Add(0x00);
	crc16.Add(0x00);
	crc16.Add(0x27);
	crc16.Add(0x10);
	std::cout << std::format("{:04x}", crc16.RegisterValue()) << std::endl;
}
