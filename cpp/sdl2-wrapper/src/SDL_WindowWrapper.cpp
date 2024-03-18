#include "SDL_WindowWrapper.h"

video::SDL_WindowWrapper::SDL_WindowWrapper(const char *title, int x, int y, int w, int h, SDL_WindowFlags flags)
{
	SDL_Initializer::Initialize();
	_wrapped_obj = ::SDL_CreateWindow(title, x, y, w, h, flags);
	if (!_wrapped_obj)
	{
		throw jc::Exception(SDL_GetError());
	}
}

video::SDL_WindowWrapper::~SDL_WindowWrapper()
{
	cout << "SDL_WindowWrapper 析构函数" << endl;
	::SDL_DestroyWindow(_wrapped_obj);
}

shared_ptr<video::SDL_RendererWrapper> video::SDL_WindowWrapper::CreateRenderer(int index, SDL_RendererFlags flags)
{
	auto pRender = SDL_CreateRenderer(_wrapped_obj, index, flags);
	return shared_ptr<video::SDL_RendererWrapper>{new video::SDL_RendererWrapper{ pRender }};
}

void video::SDL_WindowWrapper::SetWindowPosition(int x, int y)
{
	::SDL_SetWindowPosition(_wrapped_obj, x, y);
}
