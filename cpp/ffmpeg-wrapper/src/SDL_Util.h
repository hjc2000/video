#pragma once
#include<define.h>
extern "C"
{
	// 如果不定义这个宏，且 main 函数中没有接收命令行，会报错
	#define SDL_MAIN_HANDLED
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
	/**
	 * @brief 选择初始化哪些模块
	 */
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

		/**
		 * @brief SDL 反初始化器。在析构的时候会取消所有 SDL 的初始化。
		*/
		class SDL_Uninitializer
		{
		public:
			~SDL_Uninitializer()
			{
				cout << "SDL_Quit" << endl;
				SDL_Quit();
			}
		};

	public:
		/// <summary>
		///		初始化 SDL。此函数幂等且线程安全。
		///		通过本函数初始化，在进程退出后会自动执行 SDL_Quit 取消所有初始化。
		/// </summary>
		/// <param name="option"></param>
		/// <exception cref="Exception 失败会抛出异常"></exception>
		static void Initialize(SDL_InitOption option = SDL_InitOption::Everything)
		{
			using namespace std;
			static mutex lock;
			static shared_ptr<SDL_Uninitializer> uninitializer;

			lock_guard l(lock);
			if (uninitializer)
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

			// 初始化成功
			// 构造初始化器，在进程退出时会自动析构，从而反初始化。
			uninitializer = shared_ptr<SDL_Uninitializer>{ new SDL_Uninitializer{} };
		}
	};
}