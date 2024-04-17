#include"Counter.h"
#include<iostream>

using namespace jc;
using namespace std;

void jc::TestCounter()
{
	Counter<uint8_t> counter{ 10 };
	for (int i = 0; i < 15; i++)
	{
		cout << (uint64_t)counter.CurrentValue() << endl;
		counter++;
	}
}
