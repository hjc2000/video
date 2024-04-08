#include"jccpp/Counter.h"

using namespace std;
using namespace jc;

Counter::Counter(uint64_t max_value)
{
	if (max_value == 0)
	{
		throw jc::ArgumentException("计数的最大值不允许 == 0");
	}

	_max_value = max_value;
}

/// <summary>
///		递增计数。递增到最大之后，再次递增会归 0.
/// </summary>
void Counter::IncCount()
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
void Counter::DecCount()
{
	if (_count == 0)
	{
		_count = _max_value;
		return;
	}

	_count--;
}

/// <summary>
///		前缀递增
/// </summary>
/// <returns>返回递增后的值。</returns>
uint64_t Counter::operator++()
{
	IncCount();
	return _count;
}

/// <summary>
///		后缀递增
/// </summary>
/// <param name=""></param>
/// <returns>返回递增前的值。</returns>
uint64_t Counter::operator++(int)
{
	uint64_t record = _count;
	IncCount();
	return record;
}

/// <summary>
///		前缀递减
/// </summary>
/// <returns></returns>
uint64_t Counter::operator--()
{
	DecCount();
	return _count;
}

/// <summary>
///		后缀递减。
/// </summary>
/// <param name=""></param>
/// <returns></returns>
uint64_t Counter::operator--(int)
{
	uint64_t record = _count;
	DecCount();
	return record;
}

/// <summary>
///		将计数器的值增加指定的值
/// </summary>
/// <param name="value"></param>
/// <returns>返回运算后的值。</returns>
uint64_t Counter::operator+=(uint64_t value)
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
uint64_t Counter::operator-=(uint64_t value)
{
	value %= _max_value + 1;
	_count += _max_value + 1 - value;
	_count %= _max_value + 1;
	return _count;
}

void Counter::Reset()
{
	_count = 0;
}

uint64_t Counter::CurrentValue()
{
	return _count;
}

void Counter::SetCurrentValue(uint64_t value)
{
	_count = value % (_max_value + 1);
}
