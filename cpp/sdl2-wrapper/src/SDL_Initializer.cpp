#include"sdl2-wrapper/SDL_Initializer.h"
#include<iostream>

using namespace video;
using namespace std;

void SDL_Initializer::Initialize(SDL_InitOption option)
{
	using namespace std;
	static mutex lock;
	static bool initialzed = false;

	lock_guard l(lock);
	if (initialzed)
	{
		// 已经被初始化过了，直接返回
		return;
	}

	int result = SDL_Init((uint32_t)option);
	if (result)
	{
		// 发生了错误
		auto error = SDL_GetError();
		cout << error << endl;
		throw jc::Exception(error);
	}

	initialzed = true;
}
