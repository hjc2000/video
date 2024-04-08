#pragma once
#include<Exception.h>
#include<stdint.h>

namespace jc
{
	/// <summary>
	///		计数器。计数到最大值后会归 0.
	/// </summary>
	class Counter
	{
	public:
		Counter(uint64_t max_value);

	private:
		uint64_t _max_value;
		uint64_t _count = 0;

		void IncCount();
		void DecCount();

	public:
		uint64_t operator++();
		uint64_t operator++(int);
		uint64_t operator--();
		uint64_t operator--(int);
		uint64_t operator+=(uint64_t value);
		uint64_t operator-=(uint64_t value);

		/// <summary>
		///		重置计数值。计数值归 0.
		/// </summary>
		void Reset();

		/// <summary>
		///		获取计数器的当前值。
		/// </summary>
		/// <returns></returns>
		uint64_t CurrentValue();

		/// <summary>
		///		设置计数器的当前值。
		/// 
		///		会将 value 对 (_max_value + 1) 取模，也就是说如果 value 超过最大值，
		///		会发生回绕。
		/// </summary>
		/// <param name="value"></param>
		void SetCurrentValue(uint64_t value);
	};
}