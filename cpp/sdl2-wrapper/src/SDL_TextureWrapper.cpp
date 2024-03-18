#include "SDL_TextureWrapper.h"

using namespace video;

SDL_TextureWrapper::SDL_TextureWrapper(::SDL_Texture *texture)
{
	_wrapped_obj = texture;
}

SDL_TextureWrapper::~SDL_TextureWrapper()
{
	cout << "SDL_TextureWrapper 析构函数" << endl;
	SDL_DestroyTexture(_wrapped_obj);
}

int SDL_TextureWrapper::UpdateYUVTexture(
	const SDL_Rect *rect,
	const Uint8 *Yplane, int Ysize,
	const Uint8 *Uplane, int Usize,
	const Uint8 *Vplane, int Vsize)
{
	return ::SDL_UpdateYUVTexture(
		_wrapped_obj, rect,
		Yplane, Ysize,
		Uplane, Usize,
		Vplane, Vsize
	);
}

int SDL_TextureWrapper::UpdateYUVTexture(const SDL_Rect *rect, video::AVFrameWrapper &frame)
{
	return UpdateYUVTexture(
		rect,
		frame->data[0], frame->linesize[0],
		frame->data[1], frame->linesize[1],
		frame->data[2], frame->linesize[2]
	);
}
