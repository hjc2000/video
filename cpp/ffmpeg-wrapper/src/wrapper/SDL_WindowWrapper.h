#pragma once
#include<SDL_Util.h>
#include<SDL_RendererWrapper.h>
#include<Exception.h>
#include<Wrapper.h>

namespace video
{
	/// <summary>
	/// 创建渲染器时用来选择渲染器的类型
	/// </summary>
	enum SDL_RendererFlags
	{
		/**
		 * @brief 不进行设置。
		 * 这个是我自己添加的，官方的 SDL_RendererFlags 里是没有的，
		 * 但是官方的函数是使用 uint32_t 接收 SDL_RendererFlags 类型变量的，并且允许
		 * 传入 0 或者 SDL_RendererFlags 中的一个或多个值按位相或。所以我在这里添加一个
		 * 扩展的 unset
		*/
		unset = 0,

		/**
		 * @brief The renderer is a software fallback
		*/
		software = ::SDL_RendererFlags::SDL_RENDERER_SOFTWARE,

		/**
		 * @brief The renderer uses hardware acceleration
		*/
		accelerated = ::SDL_RendererFlags::SDL_RENDERER_ACCELERATED,

		/**
		 * @brief Present is synchronized with the refresh rate
		*/
		presentvsync = ::SDL_RendererFlags::SDL_RENDERER_PRESENTVSYNC,

		/**
		 * @brief The renderer supports rendering to texture
		*/
		SDL_RENDERER_TARGETTEXTURE = ::SDL_RendererFlags::SDL_RENDERER_TARGETTEXTURE,
	};

	class SDL_WindowWrapper : public Wrapper<::SDL_Window>
	{
	public:
		SDL_WindowWrapper(const char *title, int x, int y, int w, int h, SDL_WindowFlags flags);
		~SDL_WindowWrapper();

	public:
		int UpdateWindowSurface()
		{
			return ::SDL_UpdateWindowSurface(_wrapped_obj);
		}

		::SDL_Surface *GetWindowSurface()
		{
			return ::SDL_GetWindowSurface(_wrapped_obj);
		}

		/**
		 * @brief Create a Renderer object创建一个渲染器
		 *
		 * @param index
		 * @param flags
		 * @return shared_ptr<video::SDL_RendererWrapper>
		 */
		shared_ptr<video::SDL_RendererWrapper> CreateRenderer(int index, SDL_RendererFlags flags = SDL_RendererFlags::unset);

		/**
		 * @brief 设置窗口的位置
		 *
		 * @param x
		 * @param y
		 */
		void SetWindowPosition(int x, int y);

		/**
		 * @brief 获取屏幕尺寸
		 *
		 * @return SDL_Rect
		 */
		static SDL_Rect GetScreenSize()
		{
			SDL_Rect screenRect;
			int ret = ::SDL_GetDisplayBounds(0, &screenRect);
			if (ret < 0)
				throw jc::Exception(SDL_GetError());
			return screenRect;
		}
	};
}
