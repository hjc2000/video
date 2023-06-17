#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Person
{
public:
	Person()
	{
		cout << "Person 构造函数" << endl;
	}
	~Person()
	{
		cout << "析构函数" << endl;
	}
	Person(const Person& person)
	{
		cout << "拷贝构造函数" << endl;
	}
};

int main(void)
{
	// test_dic();
	Person p;
	Person p1{ p };
	return 0;
}
