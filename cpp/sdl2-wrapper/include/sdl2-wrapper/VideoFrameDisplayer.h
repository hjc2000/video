#pragma once
#include<ffmpeg-wrapper/wrapper/AVFrameWrapper.h>
#include<ffmpeg-wrapper/AVPixelFormatExtension.h>
#include<ffmpeg-wrapper/info-collection/IVideoStreamInfoCollection.h>
#include<ffmpeg-wrapper/pipe/interface/IFrameConsumer.h>
#include<sdl2-wrapper/SDL_EventGetter.h>
#include<sdl2-wrapper/SDL_RendererWrapper.h>
#include<sdl2-wrapper/SDL_TextureWrapper.h>
#include<sdl2-wrapper/SDL_WindowWrapper.h>

namespace video
{
	/**
	 * @brief 视频帧显示器
	*/
	class VideoFrameDisplayer :public IFrameConsumer
	{
	public:
		VideoFrameDisplayer(
			int x,
			int y,
			int width,
			int height,
			AVPixelFormat pix_format,
			std::string window_title,
			SDL_WindowFlags flags
		);

	private:
		shared_ptr<SDL_WindowWrapper> _window;
		shared_ptr<SDL_RendererWrapper> _renderer;
		shared_ptr<SDL_TextureWrapper> _texture;

	public:
		/// <summary>
		///		显示视频帧。
		/// </summary>
		/// <param name="frame">
		///		要显示的帧。送入空指针会直接返回，不会有任何影响。
		///		注意，一定要送入视频帧，不能送入音频帧。
		/// </param>
		void SendFrame(AVFrameWrapper *frame) override;
	};
}