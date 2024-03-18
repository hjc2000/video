#include"SDL_EventGetterHelper.h"

SDL_EventGetter *CreateSDL_EventGetter()
{
	return new SDL_EventGetter{};
}

void FreeSDL_EventGetter(SDL_EventGetter *event_getter)
{
	delete event_getter;
}

uint8_t SDL_EventGetter_PollQuitEvent(SDL_EventGetter *event_getter)
{
	return event_getter->PollQuitEvent();
}
