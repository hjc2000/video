#pragma once
#include<cstdint>
#include<algorithm>
#include<bit>
#include<Exception.h>
#include <execution>

namespace jc
{
	using namespace std;

	/// <summary>
	///		转换大小端序
	/// </summary>
	class EndianConverter
	{
	private:
		EndianConverter() = delete;

	public:
		/// <summary>
		///		将数组中的逐个单元转为大端序。
		/// 
		///		* 如果本机已经是大端序了，则不会进行转换。如果本机是小端序，则会将数组中的每个单元的字节序反转。
		/// 
		///		* 使用本函数代表你确定 buf 中的数据是小端序的。因为本函数无法判断你的数据是不是小端序。如果你的
		///		机器是小端序，则每次调用本函数都会将每个单元的字节进行反转。如果你连续对 buf 调用 2 此本函数，每次
		///		传入的参数都相同，则 buf 中的每个单元的字节序又恢复成原来的样子了。
		/// </summary>
		/// <param name="buf"></param>
		/// <param name="buf_size"></param>
		/// <param name="element_size"></param>
		static void ToBigEndian(uint8_t *buf, size_t buf_size, size_t element_size)
		{
			if (std::endian::native == std::endian::big)
			{
				return;
			}

			ReverseEachElements(buf, buf_size, element_size);
		}

		/// <summary>
		///		将一个数组中的每个单元分别反转。
		/// 
		///		* 第一个单元仍是第一个单元，因为反转的不是单元的顺序，而是每个单元内的字节。
		///		即每个单元的第一个字节会变成这个单元的最后一个字节。
		/// </summary>
		/// 
		/// <param name="buf">一个包含了整数个单元的数组。</param>
		/// <param name="buf_size">buf 的尺寸。单位为字节。</param>
		/// <param name="element_size">每个单元的尺寸。单位为字节。</param>
		/// 
		/// <exception cref="jc::ArgumentException"></exception>
		static void ReverseEachElements(uint8_t *buf, size_t buf_size, size_t element_size)
		{
			#pragma region 检查参数合法性
			if (buf_size == 0)
			{
				return;
			}

			if (element_size == 0)
			{
				throw jc::ArgumentException();
			}

			if (buf_size < element_size)
			{
				// 整体尺寸不能比单个单元尺寸还小。
				throw jc::ArgumentException();
			}

			if (buf_size % element_size != 0)
			{
				// 数组中必须包含整数个单元。
				throw jc::ArgumentException();
			}
			#pragma endregion

			size_t element_num = buf_size / element_size;
			for (size_t element_pos = 0; element_pos < element_num; element_pos++)
			{
				ReverseArray((uint8_t *)(buf + element_pos * element_size), element_size);
			}
		}

		/// <summary>
		///		将一个数组所有字节反向。
		/// </summary>
		/// <param name="buf"></param>
		/// <param name="buf_size"></param>
		static void ReverseArray(uint8_t *buf, size_t size)
		{
			reverse(buf, buf + size);
		}
	};

	void test_EndianConverter();
}