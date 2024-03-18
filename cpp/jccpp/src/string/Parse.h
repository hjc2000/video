#pragma once
#include<Exception.h>
#include<stdint.h>
#include<string>

template <typename TRet, typename TParam>
TRet Parse(TParam param)
{
	throw jc::NotImplementedException();
}

// 特化版本：从 std::string 解析为 int
template <>
int Parse<int, std::string>(std::string param)
{
	return std::stoi(param);
}

// 特化版本：从 std::string 解析为 double
template <>
double Parse<double, std::string>(std::string param)
{
	return std::stod(param);
}
