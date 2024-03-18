#include"string_helper.h"

using namespace std;

std::string *CreateEmptyString()
{
	return new string{};
}

int32_t String_GetSize(std::string *str)
{
	return (int32_t)str->size();
}

char const *String_GetBuffer(std::string *str)
{
	return str->c_str();
}

void String_Set(std::string *str, char const *char_arr, int32_t size)
{
	*str = string{ char_arr, (size_t)size };
}

void FreeStdString(std::string *str)
{
	delete str;
}
