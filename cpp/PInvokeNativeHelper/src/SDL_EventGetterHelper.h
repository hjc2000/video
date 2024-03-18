#pragma once
#include<SDL_EventGetter.h>

using namespace video;

extern "C"
{
	SDL_EventGetter *CreateSDL_EventGetter();
	void FreeSDL_EventGetter(SDL_EventGetter *event_getter);

	uint8_t SDL_EventGetter_PollQuitEvent(SDL_EventGetter *event_getter);
}