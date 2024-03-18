#pragma once
#include<iostream>
#include<string>

extern "C"
{
	/// <summary>
	///		构造一个空的字符串。
	/// </summary>
	/// <returns></returns>
	std::string *CreateEmptyString();

	/// <summary>
	///		获取字符串的大小，不包括结尾的空字符。单位：字节。
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	int32_t String_GetSize(std::string *str);

	/// <summary>
	///		获取 std::string 内部的缓冲区。这个缓冲区结尾是一个空字符。
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	char const *String_GetBuffer(std::string *str);

	/// <summary>
	///		为字符串赋予新值
	/// </summary>
	/// <param name="str">要被赋予新值的字符串</param>
	/// <param name="char_arr">字符数组，结尾不包含空字符。</param>
	/// <param name="size">char_arr 的大小，单位：字节。</param>
	void String_Set(std::string *str, char const *char_arr, int32_t size);

	void FreeStdString(std::string *str);
}