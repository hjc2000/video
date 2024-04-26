#include "SDL_RendererWrapper.h"
#include<iostream>

using namespace video;
using namespace std;

SDL_RendererWrapper::SDL_RendererWrapper(SDL_Renderer *pRenderer)
{
	_wrapped_obj = pRenderer;
}

SDL_RendererWrapper::~SDL_RendererWrapper()
{
	cout << "SDL_RendererWrapper 析构函数" << endl;
	SDL_DestroyRenderer(_wrapped_obj);
}

int SDL_RendererWrapper::SetDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return SDL_SetRenderDrawColor(_wrapped_obj, r, g, b, a);
}

int SDL_RendererWrapper::DrawRect(const SDL_Rect *rect)
{
	return SDL_RenderDrawRect(_wrapped_obj, rect);
}

int SDL_RendererWrapper::DrawFillRect(const SDL_Rect *rect)
{
	return SDL_RenderFillRect(_wrapped_obj, rect);
}

int SDL_RendererWrapper::SetBlendMode(SDL_BlendMode mode)
{
	return SDL_SetRenderDrawBlendMode(_wrapped_obj, mode);
}

int SDL_RendererWrapper::Clear()
{
	return SDL_RenderClear(_wrapped_obj);
}

int SDL_RendererWrapper::RenderTexture(
	shared_ptr<SDL_TextureWrapper> texture,
	const SDL_Rect *srcrect,
	const SDL_Rect *dstrect)
{
	return SDL_RenderCopy(_wrapped_obj, *texture, srcrect, dstrect);
}

int SDL_RendererWrapper::RenderTexture(shared_ptr<SDL_TextureWrapper> texture)
{
	return SDL_RenderCopy(_wrapped_obj, *texture, nullptr, nullptr);
}

shared_ptr<SDL_TextureWrapper> SDL_RendererWrapper::CreateTexture(
	SDL_PixelFormatEnum format,
	SDL_TextureAccess access, int w, int h)
{
	SDL_Texture *tex = SDL_CreateTexture(_wrapped_obj, format, access, w, h);
	if (tex == nullptr)
	{
		auto error = SDL_GetError();
		cout << error << endl;
		throw std::runtime_error(error);
	}

	return shared_ptr<SDL_TextureWrapper>{ new SDL_TextureWrapper{ tex }};
}


void SDL_RendererWrapper::Present()
{
	SDL_RenderPresent(_wrapped_obj);
}
