#pragma once
#include<SDL_Util.h>
#include<iostream>

namespace video
{
	class SDL_EventResult
	{
	public:
		/**
		 * @brief 获取事件的操作是否成功。如果失败了，_event 字段无效。
		*/
		bool _success = false;
		SDL_Event _event{};
	};

	/**
	 * @brief SDL 事件获取者。
	*/
	class SDL_EventGetter final
	{
	public:
		SDL_EventGetter()
		{
			SDL_Initializer::Initialize();
		}

	public:
		/**
		 * @brief 查询一次，看是否有事件发生。
		 * @return
		*/
		SDL_EventResult PollEvent()
		{
			SDL_EventResult result{};
			result._success = ::SDL_PollEvent(&result._event);
			return result;
		}

		/**
		 * @brief 阻塞，等待事件。
		 * - 此操作无法取消，且会无限时长阻塞。除非发生错误，否则不会没拿到事件就返回了。
		 *
		 * @return
		*/
		SDL_EventResult WaitEvent()
		{
			SDL_EventResult result{};
			result._success = SDL_WaitEvent(&result._event);
			if (!result._success)
			{
				cout << CODE_POS_STR << SDL_GetError() << endl;
			}

			return result;
		}

		/**
		 * @brief 轮询一次事件并检查是否是 SDL_EventType::SDL_QUIT 事件
		 * @return 成功轮询到事件，并且是 SDL_EventType::SDL_QUIT 事件则返回 true，否则返回 false。
		*/
		bool PollQuitEvent()
		{
			SDL_EventResult result = PollEvent();
			if (result._success && result._event.type == SDL_EventType::SDL_QUIT)
			{
				return true;
			}

			return false;
		}

		/**
		 * @brief 等待 SDL_EventType::SDL_QUIT 事件的发生。发生后本函数返回，否则永远不会返回。
		 *
		 * - 可以用来阻塞主线程，防止主线程退出后后台线程被杀死。
		 *
		 * - 经过测试，如果是控制台项目，没有创建 SDL 窗口的，按下 Ctrl + C 后，也会收到
		 *	 SDL_EventType::SDL_QUIT 事件。但是调试模式下，这么做会导致 visual studio 报错。
		 *
		 * - 此操作无法取消，且会无限时长阻塞。
		*/
		void WaitQuitEvent()
		{
			while (1)
			{
				SDL_EventResult result = WaitEvent();
				if (result._success && result._event.type == SDL_EventType::SDL_QUIT)
				{
					break;
				}
			}
		}
	};
}
