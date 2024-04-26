#include<Point24Solver.h>

using namespace std;

int main(void)
{
	try
	{
		std::array<uint8_t, 4> nums{ 11,5,4,3 };
		cout << Point24Solver::Solve(nums) << endl;
		return 0;
	}
	catch (std::runtime_error &e)
	{
		cout << e.what() << endl;
		throw;
	}
}

