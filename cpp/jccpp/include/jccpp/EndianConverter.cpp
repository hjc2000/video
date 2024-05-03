#include"EndianConverter.h"
#include<algorithm>
#include<bit>
#include<cstdint>
#include<execution>

using namespace jc;

void jc::EndianConverter::ReverseArray(uint8_t *buf, int32_t size)
{
	std::reverse(buf, buf + size);
}

void jc::EndianConverter::ReverseEachElements(uint8_t *buf, int32_t buf_size, int32_t element_size)
{
	#pragma region 检查参数合法性
	if (buf_size < 0)
	{
		throw std::invalid_argument{ "buf_size 不能小于 0" };
	}

	if (element_size <= 0)
	{
		throw std::invalid_argument{ "element_size 不能小于等于 0" };
	}

	if (buf_size < element_size)
	{
		throw std::invalid_argument{ "buf_size 不能小于 element_size。" };
	}

	if (buf_size % element_size != 0)
	{
		throw std::invalid_argument{ "buf_size 必须是 element_size 的整数倍。" };
	}
	#pragma endregion

	int32_t element_num = buf_size / element_size;
	for (int32_t element_pos = 0; element_pos < element_num; element_pos++)
	{
		ReverseArray((uint8_t *)(buf + element_pos * element_size), element_size);
	}
}

void jc::EndianConverter::ToBigEndian(uint8_t *buffer, int32_t buf_size, int32_t element_size)
{
	if (std::endian::native == std::endian::big)
	{
		return;
	}

	ReverseEachElements(buffer, buf_size, element_size);
}


#include<iostream>

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

	EndianConverter::ToBigEndian(
		reinterpret_cast<uint8_t *>(arr1),
		sizeof(arr1),
		sizeof(arr1[0])
	);

	std::cout << "转换为大端序后的数组: ";
	PrintArray(reinterpret_cast<uint8_t *>(arr1), sizeof(arr1));
}
