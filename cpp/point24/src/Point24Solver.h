#pragma once
#include<algorithm>
#include<array>
#include<format>
#include<iostream>
#include<jccpp/Exception.h>
#include<jccpp/math/BaseConverter.h>
#include<math.h>
#include<string>

class Point24Solver
{
	static std::string OperatorToString(uint8_t op);
	static std::string GetExpression(std::array<uint8_t, 4> const &nums, std::vector<uint8_t> const &operators);
	static std::string TryAllOperatorCombinations(std::array<uint8_t, 4> const &nums);
	static double CalculateExpression(std::string const &expr);

public:
	/// <summary>
	///		
	/// </summary>
	/// <param name="nums"></param>
	/// <returns></returns>
	static std::string Solve(std::array<uint8_t, 4> const &nums);
};
