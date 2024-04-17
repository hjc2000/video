#include<calculate_point24.h>

using namespace std;

void PrintVector(std::vector<uint8_t> vec)
{
	for (uint8_t value : vec)
	{
		cout << (int)value << ", ";
	}

	cout << endl;
}

int main(void)
{
	try
	{
		std::vector<uint8_t> nums{ 12,6,4,6 };
		std::vector<uint8_t> out_successful_nums_permutations;
		std::vector<uint8_t> out_successfull_operator_combinations;
		if (TryAllPermutations(nums,
			out_successful_nums_permutations,
			out_successfull_operator_combinations
		))
		{
			PrintVector(out_successful_nums_permutations);
			PrintVector(out_successfull_operator_combinations);
		}

		return 0;
	}
	catch (jc::Exception &e)
	{
		cout << e.what() << endl;
		throw;
	}
}

