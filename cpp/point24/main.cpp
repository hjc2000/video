#include<CLI/CLI.hpp>
#include<iostream>
#include<Point24Solver.h>
#include<stdint.h>

using namespace std;

int main(int argc, char **argv)
{
	CLI::App app { "输入 4 个数，求解 24 点。" };
	std::vector<int> cli_numbers; // 用于存储解析后的整数

	try
	{
		// 使用CLI11的callback机制来处理数值序列
		auto opt = app.add_option_function<std::vector<int>>(
			"--numbers",
			[&](std::vector<int> value)
		{
			if (value.size() < 4)
			{
				throw CLI::ValidationError { "必须输入 4 个数。" };
			}

			if (value.size() > 4)
			{
				value.resize(4);
			}

			cli_numbers = value;
		},
			"输入一系列的数字"
		);

		// 设置选项允许接受多个值（默认行为）
		opt->allow_extra_args(true);
		app.parse(argc, argv);

		// 解析完成
		std::array<uint8_t, 4> nums { };
		for (uint32_t i = 0; i < nums.size(); i++)
		{
			nums[i] = cli_numbers[i];
		}

		cout << Point24Solver::Solve(nums) << endl;

		return 0;
	}
	catch (const CLI::ParseError &e)
	{
		return app.exit(e);
	}
	catch (std::exception &e)
	{
		cout << e.what() << endl;
		throw;
	}
}
