#include<algorithm>
#include<filesystem>
#include<jccpp/Exception.h>
#include<jccpp/math/BaseConverter.h>

using namespace std;

int main(void)
{
	try
	{
		std::vector<uint8_t> num{ 1,0,0,9 };
		std::reverse(num.begin(), num.end());
		cout << jc::BaseConverter::ToNumber(num, 10) << endl;
		return 0;
	}
	catch (jc::Exception &e)
	{
		cout << e.what() << endl;
		throw;
	}
}
