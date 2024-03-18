#pragma once
#include<SDL_Util.h>
#include<SDL_TextureWrapper.h>

namespace video
{
	class SDL_RendererWrapper : public Wrapper<SDL_Renderer>
	{
	public://生命周期
		SDL_RendererWrapper(SDL_Renderer *pRenderer);
		~SDL_RendererWrapper();

	public:
		/// <summary>
		///		设置用来绘制的颜色
		/// </summary>
		/// <param name="r">红色通道数值</param>
		/// <param name="g">绿色通道数值</param>
		/// <param name="b">蓝色通道数值</param>
		/// <param name="a">透明通道数值</param>
		/// <returns>成功返回 0，失败返回错误代码</returns>
		int SetDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

		/// <summary>
		///		在缓冲区中绘制矩形
		/// </summary>
		/// <param name="rect"></param>
		/// <returns>成功返回 0，失败返回错误代码</returns>
		int DrawRect(const SDL_Rect *rect);

		/// <summary>
		///		绘制一个实心的矩形。矩形内部也会被充满画笔颜色
		/// </summary>
		/// <param name="rect"></param>
		/// <returns>成功返回 0，失败返回错误代码</returns>
		int DrawFillRect(const SDL_Rect *rect);

		/// <summary>
		///		设置混合模式。
		///		透明通道需要设置混合模式为 SDL_BlendMode::SDL_BLENDMODE_BLEND 才有用
		/// </summary>
		/// <param name="mode">混合模式</param>
		/// <returns>成功返回 0，失败返回错误代码</returns>
		int SetBlendMode(SDL_BlendMode mode);

		/// <summary>
		///		清除画布
		///		会用当前画笔的颜色涂满画布。画笔颜色通过 SetDrawColor 方法设置。
		/// </summary>
		/// <returns>成功返回 0，失败返回错误代码</returns>
		int Clear();

		/// <summary>
		///		将纹理复制到本渲染器进行渲染。
		/// </summary>
		/// <param name="texture">要渲染的纹理</param>
		/// <param name="srcrect">
		///		选择 texture 中的一块矩形区域作为输入，传入空指针表示将整个 texture 作为输入
		/// </param>
		/// <param name="dstrect">
		///		选择渲染目标的一块矩形区域作为输出，传入空指针表示将整个渲染目标作为输出
		/// </param>
		/// <returns>成功返回 0，失败返回错误代码</returns>
		int RenderTexture(shared_ptr<SDL_TextureWrapper> texture, const SDL_Rect *srcrect, const SDL_Rect *dstrect);

		/// <summary>
		///		将整个纹理复制到本渲染器的整个矩形中进行渲染。如果 texture
		///		的大小与本渲染器的矩形大小不匹配，会缩放 texture 以适应。
		/// </summary>
		/// <param name="texture">要渲染的纹理</param>
		/// <returns>成功返回 0，失败返回错误代码</returns>
		int RenderTexture(shared_ptr<SDL_TextureWrapper> texture);

		/// <summary>
		///		创建纹理
		/// </summary>
		/// <param name="format"></param>
		/// <param name="access"></param>
		/// <param name="w"></param>
		/// <param name="h"></param>
		/// <returns></returns>
		shared_ptr<SDL_TextureWrapper> CreateTexture(
			SDL_PixelFormatEnum format,
			SDL_TextureAccess access, int w, int h);

		/// <summary>
		///		要调用这个函数才能让渲染器将缓冲区中的 RGBA 渲染到屏幕中，不然
		///		再怎么编辑缓冲区，数据都只是停留在缓冲区而已，不会呈现到屏幕上。
		/// </summary>
		void Present();
	};
}
