#pragma once
#include<string>

class ICanToString
{
public:
	virtual ~ICanToString() {}

public:
	virtual std::string ToString() = 0;

	virtual std::string ToString(char const *format_str)
	{
		return ToString();
	}
};

inline std::string ToString(int value)
{
	return std::to_string(value);
}