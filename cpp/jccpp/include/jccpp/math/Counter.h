#pragma once
#include<jccpp/Exception.h>
#include<stdint.h>

namespace jc
{
	/// <summary>
	///		计数器。计数到最大值后会归 0.
	/// </summary>
	/// <typeparam name="T">此类型必须是无符号整型</typeparam>
	template<typename T>
	class Counter
	{
	private:
		T _max_value;
		T _count = 0;

		/// <summary>
		///		递增计数。递增到最大之后，再次递增会归 0.
		/// </summary>
		void IncCount()
		{
			if (_count == _max_value)
			{
				_count = 0;
				return;
			}

			_count++;
		}

		/// <summary>
		///		递减计数。递减到 0 后再次递减会变成最大值。
		/// </summary>
		void DecCount()
		{
			if (_count == 0)
			{
				_count = _max_value;
				return;
			}

			_count--;
		}

	public:
		Counter(T max_value)
		{
			if (max_value == 0)
			{
				throw jc::ArgumentException("计数的最大值不允许 == 0");
			}

			_max_value = max_value;
		}

		#pragma region 运算符重载
		/// <summary>
		///		前缀递增
		/// </summary>
		/// <returns>返回递增后的值。</returns>
		T operator++()
		{
			IncCount();
			return _count;
		}

		/// <summary>
		///		后缀递增
		/// </summary>
		/// <param name=""></param>
		/// <returns>返回递增前的值。</returns>
		T operator++(int)
		{
			T record = _count;
			IncCount();
			return record;
		}

		/// <summary>
		///		前缀递减
		/// </summary>
		/// <returns></returns>
		T operator--()
		{
			DecCount();
			return _count;
		}

		/// <summary>
		///		后缀递减。
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		T operator--(int)
		{
			T record = _count;
			DecCount();
			return record;
		}

		/// <summary>
		///		将计数器的值增加指定的值
		/// </summary>
		/// <param name="value"></param>
		/// <returns>返回运算后的值。</returns>
		T operator+=(T value)
		{
			_count += value;
			_count %= _max_value + 1;
			return _count;
		}

		/// <summary>
		///		将计数器的值减去指定的值。
		/// </summary>
		/// <param name="value"></param>
		/// <returns>返回运算后的值。</returns>
		T operator-=(T value)
		{
			value %= _max_value + 1;
			_count += _max_value + 1 - value;
			_count %= _max_value + 1;
			return _count;
		}
		#pragma endregion

		/// <summary>
		///		重置计数值。计数值归 0.
		/// </summary>
		void Reset()
		{
			_count = 0;
		}

		/// <summary>
		///		获取计数器的当前值。
		/// </summary>
		/// <returns></returns>
		T CurrentValue() const
		{
			return _count;
		}

		/// <summary>
		///		设置计数器的当前值。
		/// 
		///		会将 value 对 (_max_value + 1) 取模，也就是说如果 value 超过最大值，
		///		会发生回绕。
		/// </summary>
		/// <param name="value"></param>
		void SetCurrentValue(T value)
		{
			_count = value % (_max_value + 1);
		}
	};
}