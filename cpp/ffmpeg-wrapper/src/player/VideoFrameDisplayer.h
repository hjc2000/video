#pragma once
#include<AVFrameWrapper.h>
#include<AVPixelFormatExtension.h>
#include<IFrameConsumer.h>
#include<IVideoStreamInfoCollection.h>
#include<SDL_EventGetter.h>
#include<SDL_RendererWrapper.h>
#include<SDL_TextureWrapper.h>
#include<SDL_WindowWrapper.h>

namespace video
{
	/**
	 * @brief 视频帧显示器
	*/
	class VideoFrameDisplayer :public IFrameConsumer
	{
		#pragma region 选项
	public:
		class Options :public IVideoFrameInfoCollection
		{
		public:
			Options() {}

			Options(IVideoFrameInfoCollection &infos)
			{
				IVideoFrameInfoCollection::operator=(infos);
			}

			Options &operator=(IVideoFrameInfoCollection &infos)
			{
				IVideoFrameInfoCollection::operator=(infos);
				return *this;
			}

		public:
			int _x = 0;
			int _y = 0;
			int _width = 1280;
			int _height = 720;
			AVPixelFormat _pix_format = AVPixelFormat::AV_PIX_FMT_YUV420P;
			std::string _window_title = "VideoFrameDisplayer";
			SDL_WindowFlags _flags = SDL_WindowFlags::SDL_WINDOW_SHOWN;

		public:
			#pragma region IVideoFrameInfoCollection 属性
			virtual int width() override
			{
				return _width;
			}
			virtual void set_width(int value) override
			{
				_width = value;
			}

			virtual int height() override
			{
				return _height;
			}
			virtual void set_height(int value) override
			{
				_height = value;
			}

			virtual AVPixelFormat pixel_format() override
			{
				return _pix_format;
			}
			virtual void set_pixel_format(AVPixelFormat value) override
			{
				_pix_format = value;
			}
			#pragma endregion
		};
		#pragma endregion

	public:
		VideoFrameDisplayer(Options options);

	private:
		shared_ptr<SDL_WindowWrapper> _window;
		shared_ptr<SDL_RendererWrapper> _renderer;
		shared_ptr<SDL_TextureWrapper> _texture;

	public:
		/**
		 * @brief 显示视频帧。
		 * @param frame 要显示的帧。送入空指针会直接返回，不会有任何影响。
		 * - 注意，一定要送入视频帧，不能送入音频帧。
		*/
		void SendFrame(AVFrameWrapper *frame) override;
	};
}