#pragma once
#include<format>

// 代码位置字符串。使用此宏，会在使用的地方展开并获取指示代码位置的字符串。
#define CODE_POS_STR    std::format("\n>> 代码位置：文件：{}，函数：{}，行号：{}。-- ", __FILE__, __func__, __LINE__).c_str()