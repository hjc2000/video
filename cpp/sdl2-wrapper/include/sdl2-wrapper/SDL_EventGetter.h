#pragma once
#include<sdl2-wrapper/SDL_Initializer.h>
#include<iostream>

namespace video
{
	/// <summary>
	///		包装原生的 SDL_Event
	/// </summary>
	class SDL_EventResult
	{
	public:
		/// <summary>
		///		获取事件的操作是否成功。如果失败了，_event 字段无效。
		/// </summary>
		bool _success = false;

		/// <summary>
		///		_success 为 true 时此字段才有效。获取到的事件被放到这里。
		/// </summary>
		SDL_Event _event{};
	};

	class SDL_EventGetter final
	{
	public:
		SDL_EventGetter();

	public:
		/// <summary>
		///		查询一次，看是否有事件发生。
		/// </summary>
		/// <returns></returns>
		SDL_EventResult PollEvent();

		/// <summary>
		///		阻塞，等待事件。
		///		此操作无法取消，且会无限时长阻塞。除非发生错误，否则不会没拿到事件就返回。
		/// </summary>
		/// <returns></returns>
		SDL_EventResult WaitEvent();

		/// <summary>
		///		轮询一次事件并检查是否是 SDL_EventType::SDL_QUIT 事件
		/// </summary>
		/// <returns>成功轮询到事件，并且是 SDL_EventType::SDL_QUIT 事件则返回 true，否则返回 false。</returns>
		bool PollQuitEvent();

		/// <summary>
		///		等待 SDL_EventType::SDL_QUIT 事件的发生。发生后本函数返回，否则永远不会返回。
		///		- 可以用来阻塞主线程，防止主线程退出后后台线程被杀死。
		///		- 经过测试，如果是控制台项目，没有创建 SDL 窗口的，按下 Ctrl + C 后，也会收到
		///		  SDL_EventType::SDL_QUIT 事件。但是调试模式下，这么做会导致 visual studio 报错。
		///		- 此操作无法取消，且会无限时长阻塞。
		/// </summary>
		void WaitQuitEvent();
	};
}
