#pragma once
#include<algorithm>
#include<array>
#include<filesystem>
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

public:
	static double CalculateExpression(std::string const &expr);

	/// <summary>
	///		
	/// </summary>
	/// <param name="nums"></param>
	/// <returns></returns>
	static std::string Solve(std::array<uint8_t, 4> const &nums)
	{
		std::array<uint8_t, 4> copyed_nums = nums;
		std::sort(copyed_nums.begin(), copyed_nums.end());

		do
		{
			std::string expr = TryAllOperatorCombinations(copyed_nums);
			if (expr != "")
			{
				return expr;
			}
		} while (std::next_permutation(copyed_nums.begin(), copyed_nums.end()));
		return std::string("找不到解");
	}
};
