#include<base/modbus/ModbusCrc16.h>
#include<bit>
#include<format>
#include<iostream>
#include<stdint.h>

int main(void)
{
	base::ModbusCrc16 crc16;
	crc16.Add(0x01);
	crc16.Add(0x03);
	crc16.Add(0x04);
	crc16.Add(0x00);
	crc16.Add(0x00);
	crc16.Add(0x27);
	crc16.Add(0x10);
	std::cout << std::format("{:04x}", crc16.RegisterValue()) << std::endl;
}
