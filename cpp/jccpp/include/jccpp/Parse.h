#pragma once
#include<stdexcept>
#include<stdint.h>
#include<string>

template <typename TRet, typename TParam>
TRet Parse(TParam param)
{
	throw std::runtime_error{ "此重载未实现" };
}

/// <summary>
///		特化版本：从 std::string 解析为 int
/// </summary>
/// <param name="param"></param>
/// <returns></returns>
template <>
int Parse<int, std::string>(std::string param)
{
	return std::stoi(param);
}

/// <summary>
///		特化版本：从 std::string 解析为 double
/// </summary>
/// <param name="param"></param>
/// <returns></returns>
template <>
double Parse<double, std::string>(std::string param)
{
	return std::stod(param);
}
