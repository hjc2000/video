#include "VideoFrameDisplayer.h"

using namespace video;

VideoFrameDisplayer::VideoFrameDisplayer(Options options)
{
	_window = shared_ptr<SDL_WindowWrapper>{
		new SDL_WindowWrapper{
			options._window_title.c_str(),
			options._x,
			options._y,
			options._width,
			options._height,
			options._flags
		},
	};

	_renderer = _window->CreateRenderer(-1);
	_texture = _renderer->CreateTexture(
		AVPixelFormatExtension::AVPixelFormat_to_SDL_PixelFormatEnum(options._pix_format),
		SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING,
		options._width,
		options._height
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
