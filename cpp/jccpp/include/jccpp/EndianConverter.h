#pragma once
#include<stdint.h>

namespace jc
{
	/// <summary>
	///		转换大小端序
	/// </summary>
	class EndianConverter
	{
	private:
		EndianConverter() = delete;

		/// <summary>
		///		将一个数组所有字节反向。
		/// </summary>
		/// <param name="buffer"></param>
		/// <param name="buf_size"></param>
		static void ReverseArray(uint8_t *buf, int32_t size);

		/// <summary>
		///		将一个数组中的每个单元分别反转。
		/// 
		///		* 第一个单元仍是第一个单元，因为反转的不是单元的顺序，而是每个单元内的字节。
		///		即每个单元的第一个字节会变成这个单元的最后一个字节。
		/// </summary>
		/// 
		/// <param name="buffer">一个包含了整数个单元的数组。</param>
		/// <param name="buf_size">buffer 的尺寸。单位为字节。</param>
		/// <param name="element_size">每个单元的尺寸。单位为字节。</param>
		/// 
		/// <exception cref="jc::ArgumentException"></exception>
		static void ReverseEachElements(uint8_t *buf, int32_t buf_size, int32_t element_size);

	public:
		/// <summary>
		///		将数组中的逐个单元转为大端序。
		///		* 如果本机已经是大端序了，则不会进行转换。如果本机是小端序，则会将数组中的每个单元的字节序反转。
		///		* 使用本函数代表你确定 buffer 中的数据是小端序的。因为本函数无法判断你的数据是不是小端序。
		/// </summary>
		/// <param name="buffer"></param>
		/// <param name="buf_size"></param>
		/// <param name="element_size"></param>
		static void ToBigEndian(uint8_t *buffer, int32_t buf_size, int32_t element_size);
	};

	void test_EndianConverter();
}