#pragma once
#include<stdint.h>
#include<vector>

namespace jc
{
	/// <summary>
	///		进制转换器
	/// </summary>
	class BaseConverter
	{
	private:
		BaseConverter() = delete;

	public:
		/// <summary>
		///		将 num 转为 base 进制，每个位储存到 std::vector<uint8_t> 的一个单独的字节中。
		///		第 0 字节表示最低位。
		/// </summary>
		/// <param name="num"></param>
		/// <param name="base"></param>
		/// <returns></returns>
		static std::vector<uint8_t> ConvertBase(uint64_t num, uint8_t base);

		/// <summary>
		///		num 向量里存放的是 base 进制数。每一位占一个字节。第 0 字节表示最低位。
		/// </summary>
		/// <param name="num"></param>
		/// <param name="base"></param>
		/// <returns></returns>
		static uint64_t ToNumber(std::vector<uint8_t> const &num, uint8_t base);

		/// <summary>
		///		将整型转为 8421 BCD 码。每个码都独立放到一个字节中。虽然说每个码只需要 4 位，
		///		不需要 8 位。
		///		在返回的 std::vector<uint8_t> 中，第 0 个字节表示最低的 10 进制位。
		/// </summary>
		/// <param name="num"></param>
		/// <returns></returns>
		static std::vector<uint8_t> To8421BCD(uint64_t num);
	};

	void TestBaseConverter();
}
