#include<Point24Solver.h>

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
		std::array<uint8_t, 4> nums{ 12,6,4,6 };
		cout << Point24Solver::Solve(nums) << endl;
		return 0;
	}
	catch (jc::Exception &e)
	{
		cout << e.what() << endl;
		throw;
	}
}

