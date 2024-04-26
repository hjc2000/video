#pragma once
#include<jccpp/Wrapper.h>
#include<sdl2-wrapper/SDL_Initializer.h>
#include<sdl2-wrapper/SDL_RendererWrapper.h>

namespace video
{
	/// <summary>
	///		创建渲染器时用来选择渲染器的类型
	/// </summary>
	enum class RendererFlags
	{
		/**
		 * @brief 不进行设置。
		 * 这个是我自己添加的，官方的 SDL_RendererFlags 里是没有的，
		 * 但是官方的函数是使用 uint32_t 接收 SDL_RendererFlags 类型变量的，并且允许
		 * 传入 0 或者 SDL_RendererFlags 中的一个或多个值按位相或。所以我在这里添加一个
		 * 扩展的 unset
		*/
		Unset = 0,

		/**
		 * @brief The renderer is a software fallback
		*/
		Software = ::SDL_RendererFlags::SDL_RENDERER_SOFTWARE,

		/**
		 * @brief The renderer uses hardware acceleration
		*/
		Accelerated = ::SDL_RendererFlags::SDL_RENDERER_ACCELERATED,

		/**
		 * @brief Present is synchronized with the refresh rate
		*/
		Presentvsync = ::SDL_RendererFlags::SDL_RENDERER_PRESENTVSYNC,

		/**
		 * @brief The renderer supports rendering to texture
		*/
		RendererTargetTexture = ::SDL_RendererFlags::SDL_RENDERER_TARGETTEXTURE,
	};

	class SDL_WindowWrapper : public Wrapper<SDL_Window>
	{
		SDL_Window *_wrapped_obj = nullptr;

	public:
		SDL_WindowWrapper(
			char const *title,
			int x, int y,
			int w, int h,
			SDL_WindowFlags flags
		);

		~SDL_WindowWrapper();

		int UpdateWindowSurface();
		SDL_Surface *GetWindowSurface();

		/// <summary>
		///		创建一个渲染器
		/// </summary>
		/// <param name="index"></param>
		/// <param name="flags"></param>
		/// <returns></returns>
		shared_ptr<video::SDL_RendererWrapper> CreateRenderer(
			int index,
			RendererFlags flags = RendererFlags::Unset
		);

		/// <summary>
		///		设置窗口的位置
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		void SetWindowPosition(int x, int y);

		/// <summary>
		///		获取屏幕尺寸
		/// </summary>
		/// <returns></returns>
		static SDL_Rect GetScreenSize();

		SDL_Window *&WrappedObj() override
		{
			return _wrapped_obj;
		}
		SDL_Window *WrappedObj() const override
		{
			return _wrapped_obj;
		}
	};
}
