#include "BaseConverter.h"
#include<math.h>

using namespace std;

std::vector<uint8_t> jc::BaseConverter::ConvertBase(uint64_t num, uint8_t base)
{
	std::vector<uint8_t> result;
	do
	{
		uint64_t mod = num % base;
		result.push_back(mod);
		num /= base;
	} while (num > 0);
	return result;
}

uint64_t jc::BaseConverter::ToNumber(std::vector<uint8_t> num, uint8_t base)
{
	uint64_t result = 0;
	for (uint64_t i = 0; i < num.size(); i++)
	{
		result += num[i] * std::pow(base, i);
	}

	return result;
}

std::vector<uint8_t> jc::BaseConverter::To8421BCD(uint64_t num)
{
	return ConvertBase(num, 10);
}






#include<algorithm>
#include<iostream>

void jc::TestBaseConverter()
{
	std::vector<uint8_t> num{ 1,0,0,9 };
	std::reverse(num.begin(), num.end());
	cout << jc::BaseConverter::ToNumber(num, 10) << endl;
}
