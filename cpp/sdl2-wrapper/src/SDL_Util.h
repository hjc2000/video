#pragma once
#include<define.h>
extern "C"
{
	#include<SDL.h>
}
#include<Exception.h>
#include<Wrapper.h>
#include<cstdint>
#include<exception>
#include<iostream>
#include<string>
#include<functional>
#include<mutex>

using std::cout;
using std::endl;
using std::function;

namespace video
{
	/// <summary>
	///		选择初始化哪些模块
	/// </summary>
	enum class SDL_InitOption
	{
		Timer = SDL_INIT_TIMER,
		Audio = SDL_INIT_AUDIO,
		Video = SDL_INIT_VIDEO,
		Joystick = SDL_INIT_JOYSTICK,
		Haptic = SDL_INIT_HAPTIC,
		GameController = SDL_INIT_GAMECONTROLLER,
		Event = SDL_INIT_EVENTS,
		Sensor = SDL_INIT_SENSOR,
		Noparachute = SDL_INIT_NOPARACHUTE,
		Everything = SDL_INIT_EVERYTHING,
	};

	class SDL_Initializer
	{
	private:
		SDL_Initializer() = delete;

	public:
		/// <summary>
		///		初始化 SDL。此函数幂等且线程安全。
		///		通过本函数初始化，在进程退出后会自动执行 SDL_Quit 取消所有初始化。
		/// </summary>
		/// <param name="option"></param>
		/// <exception cref="Exception 失败会抛出异常"></exception>
		static void Initialize(SDL_InitOption option = SDL_InitOption::Everything);
	};
}