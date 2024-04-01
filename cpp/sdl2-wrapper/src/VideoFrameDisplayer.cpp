#include "VideoFrameDisplayer.h"

using namespace video;

VideoFrameDisplayer::VideoFrameDisplayer() :
	VideoFrameDisplayer(0, 0, 1280, 720)
{

}

VideoFrameDisplayer::VideoFrameDisplayer(int x, int y, int width, int height) :
	VideoFrameDisplayer(
		x, y, width, height,
		AVPixelFormat::AV_PIX_FMT_YUV420P,
		"VideoFrameDisplayer",
		SDL_WindowFlags::SDL_WINDOW_SHOWN
	)
{

}

VideoFrameDisplayer::VideoFrameDisplayer(
	int x,
	int y,
	int width,
	int height,
	AVPixelFormat pix_format,
	std::string window_title,
	SDL_WindowFlags flags
)
{
	_window = shared_ptr<SDL_WindowWrapper>{
		new SDL_WindowWrapper{
			window_title.c_str(),
			x,
			y,
			width,
			height,
			flags
		},
	};

	_renderer = _window->CreateRenderer(-1);
	_texture = _renderer->CreateTexture(
		AVPixelFormatExtension::AVPixelFormat_to_SDL_PixelFormatEnum(pix_format),
		SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING,
		width,
		height
	);
}

void VideoFrameDisplayer::SendFrame(AVFrameWrapper *frame)
{
	if (!frame)
	{
		return;
	}

	// 更新纹理
	if (_texture->UpdateYUVTexture(nullptr, *frame) < 0)
	{
		cout << SDL_GetError() << endl;
		return;
	}

	_renderer->Clear();
	if (_renderer->RenderTexture(_texture))
	{
		cout << SDL_GetError() << endl;
		return;
	}

	_renderer->Present();
}
