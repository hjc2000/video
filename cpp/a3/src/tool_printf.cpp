#include "tool_printf.h"
#include <format>
#include <iostream>
#include<stdio.h>

void printf_chip_info(chip_info *pchip)
{
	std::cout << "-------------------------------------\n";
	std::cout << "	   Chip Information\n";
	std::cout << "-------------------------------------\n";
	std::cout << std::format(
		"{:<20} : {}\n",
		"Chip Status",
		ToString(pchip->_status)
	);
	std::cout << std::format("{:<20} : {:08x}\n", "FW Version", pchip->_version);
	std::cout << std::format("{:<20} : {:08x}\n", "Chip  ID", (uint32_t)pchip->_chip_module);
	std::cout << std::format("{:<20} : {:08x}\n", "Service", (uint32_t)pchip->_hal_service);
	std::cout << std::format("{:<20} : {:08x}\n", "Input", pchip->_input_support);
	std::cout << std::format("{:<20} : {:08x}\n", "Output", pchip->_output_support);
	std::cout << std::format("{:<20} : {:08x}\n", "Peripheral", pchip->_peripheral_en);
	std::cout << std::format("{:<20} : {}\n", "SDK Version", (uint32_t)vatek_version());
	std::cout << "=====================================\r\n";
}

void printf_app_end(void)
{
	std::cout << "=====================================\r\n" << std::endl;
	std::cout << "demo finished. press any key to quit" << std::endl;
}
