#pragma once
#include<algorithm>
#include<filesystem>
#include<format>
#include<iostream>
#include<jccpp/Exception.h>
#include<jccpp/math/BaseConverter.h>
#include<math.h>
#include<string>

/// <summary>
///		用 operators 里的运算符对 nums 进行计算，结果如果等于 24 则返回 true，
///		否则返回 false。
/// </summary>
/// <param name="nums"></param>
/// <param name="operators"></param>
/// <returns></returns>
inline bool Calculate(std::vector<uint8_t> const &nums, std::vector<uint8_t> const &operators)
{
	std::vector<uint8_t> copyed_nums = nums;
	double result = 0;

	// 遍历 operators 中的每个运算符，本次只进行乘除运算。
	for (int i = 0; i < 3; i++)
	{
		switch (operators[i])
		{
		case 2:
			{
				result = (result + copyed_nums[i]) * copyed_nums[i + 1];
				copyed_nums[i] = 0;
				copyed_nums[i + 1] = 0;
				break;
			}
		case 3:
			{
				result = (result + copyed_nums[i]) / copyed_nums[i + 1];
				copyed_nums[i] = 0;
				copyed_nums[i + 1] = 0;
				break;
			}
		}
	}

	// 遍历 operators 中的每个运算符，本次只进行加减运算。
	for (int i = 0; i < 3; i++)
	{
		switch (operators[i])
		{
		case 0:
			{
				result = (result + copyed_nums[i]) + copyed_nums[i + 1];
				copyed_nums[i] = 0;
				copyed_nums[i + 1] = 0;
				break;
			}
		case 1:
			{
				result = (result + copyed_nums[i]) - copyed_nums[i + 1];
				copyed_nums[i] = 0;
				copyed_nums[i + 1] = 0;
				break;
			}
		}
	}

	return result == 24;
}

/// <summary>
///		对 nums 使用所有运算符组合。
///		如果有一种运算符组合使 nums 中的数的运算结果为 24，则返回 true，
///		并且此时 out_successful_operator_combinations 被赋值为成功的运算符组合。
/// </summary>
/// <param name="nums"></param>
/// <param name="out_successful_operator_combinations"></param>
/// <returns></returns>
inline bool TryAllOperatorCombinations(
	std::vector<uint8_t> const &nums,
	std::vector<uint8_t> &out_successful_operator_combinations
)
{
	// 遍历运算符的全部组合
	// 4 则运算符就像是 4 进制数。3 个运算符就像是 3 位 4 进制数
	for (uint8_t i = 0; i < std::pow(4, 3); i++)
	{
		out_successful_operator_combinations = jc::BaseConverter::ConvertBase(i, 4);
		int remain = 3 - out_successful_operator_combinations.size();
		for (int j = 0; j < remain; j++)
		{
			out_successful_operator_combinations.push_back(0);
		}

		if (Calculate(nums, out_successful_operator_combinations))
		{
			return true;
		}
	}

	return false;
}

inline bool TryAllPermutations(
	std::vector<uint8_t> const &nums,
	std::vector<uint8_t> &out_seccessful_nums_permutation,
	std::vector<uint8_t> &out_successful_operator_combinations
)
{
	out_seccessful_nums_permutation = nums;
	std::sort(out_seccessful_nums_permutation.begin(), out_seccessful_nums_permutation.end());
	do
	{
		if (TryAllOperatorCombinations(out_seccessful_nums_permutation, out_successful_operator_combinations))
		{
			return true;
		}
	} while (std::next_permutation(out_seccessful_nums_permutation.begin(), out_seccessful_nums_permutation.end()));
	return false;
}