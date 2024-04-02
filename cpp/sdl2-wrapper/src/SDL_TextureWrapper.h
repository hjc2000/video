#pragma once
#include<AVFrameWrapper.h>
#include<SDL_Initializer.h>

namespace video
{
	using namespace video;

	class SDL_TextureWrapper :public Wrapper<SDL_Texture>
	{
	public:
		SDL_TextureWrapper(SDL_Texture *texture);
		~SDL_TextureWrapper();

	public:
		/// <summary>
		///		使用 YUV 数组更新纹理
		/// </summary>
		/// 
		/// <param name="rect">一个矩形，表示要更新纹理的哪一部分。传入空指针表示更新整个纹理</param>
		/// 
		/// <param name="Yplane">Y 数组的头指针</param>
		/// <param name="Ysize">Y数组的长度</param>
		/// 
		/// <param name="Uplane">U 数组的头指针</param>
		/// <param name="Usize">U 数组的长度</param>
		/// 
		/// <param name="Vplane">V 数组的头指针</param>
		/// <param name="Vsize">V 数组的长度</param>
		/// 
		/// <returns>成功返回 0，失败返回错误代码</returns>
		int UpdateYUVTexture(
			const SDL_Rect *rect,
			const Uint8 *Yplane, int Ysize,
			const Uint8 *Uplane, int Usize,
			const Uint8 *Vplane, int Vsize
		);

		/// <summary>
		///		使用 AVFrameWrapper 更新纹理。
		/// </summary>
		/// <param name="rect">要更新的目标矩形。如果传入空指针，表示更新整个纹理</param>
		/// <param name="frame">AVFrameWrapper 对象</param>
		/// <returns>成功返回 0，失败返回错误代码</returns>
		int UpdateYUVTexture(const SDL_Rect *rect, AVFrameWrapper &frame);
	};
}
