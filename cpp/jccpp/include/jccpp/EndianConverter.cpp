#include "jccpp/EndianConverter.h"
#include<iostream>

using namespace jc;

// 辅助函数：打印数组的内容
void PrintArray(const uint8_t *arr, size_t size)
{
	for (size_t i = 0; i < size; ++i)
	{
		std::cout << std::hex << static_cast<int>(arr[i]) << " ";
	}
	std::cout << std::dec << std::endl;
}

void jc::test_EndianConverter()
{
	// 测试 ToBigEndian 方法
	uint32_t arr1[] = { 0x12345678, 0xabcdef12 }; // 2 个元素的数组

	std::cout << "原始数组 (小端序 assumed): ";
	PrintArray(reinterpret_cast<uint8_t *>(arr1), sizeof(arr1));

	EndianConverter::ToBigEndian(reinterpret_cast<uint8_t *>(arr1), sizeof(arr1), sizeof(arr1[0]));
	std::cout << "转换为大端序后的数组: ";
	PrintArray(reinterpret_cast<uint8_t *>(arr1), sizeof(arr1));
}
