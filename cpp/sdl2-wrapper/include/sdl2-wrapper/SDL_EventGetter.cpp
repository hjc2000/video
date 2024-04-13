#include"sdl2-wrapper/SDL_EventGetter.h"
#include<iostream>

using namespace video;
using namespace std;

SDL_EventGetter::SDL_EventGetter()
{
	SDL_Initializer::Initialize();
}

SDL_EventResult SDL_EventGetter::PollEvent()
{
	SDL_EventResult result{};
	result._success = ::SDL_PollEvent(&result._event);
	return result;
}

SDL_EventResult SDL_EventGetter::WaitEvent()
{
	SDL_EventResult result{};
	result._success = SDL_WaitEvent(&result._event);
	if (!result._success)
	{
		cout << CODE_POS_STR << SDL_GetError() << endl;
	}

	return result;
}

bool SDL_EventGetter::PollQuitEvent()
{
	SDL_EventResult result = PollEvent();
	if (result._success && result._event.type == SDL_EventType::SDL_QUIT)
	{
		return true;
	}

	return false;
}

void SDL_EventGetter::WaitQuitEvent()
{
	while (1)
	{
		SDL_EventResult result = WaitEvent();
		if (result._success && result._event.type == SDL_EventType::SDL_QUIT)
		{
			break;
		}
	}
}
