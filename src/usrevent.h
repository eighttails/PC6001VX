#ifndef USREVENT_H_INCLUDED
#define USREVENT_H_INCLUDED

#include <SDL.h>


/////////////////////////////////////////////////////////////////////////////
// SDL ユーザー定義イベント
/////////////////////////////////////////////////////////////////////////////
#define SENDEVENT(etype)		{	\
			SDL_Event ev;			\
			ev.type = etype;		\
			SDL_PushEvent( &ev ); }

#define SENDEVENT1(etype,data)	{	\
			SDL_Event ev;			\
			ev.type = etype;		\
			ev.user.code = data;	\
			SDL_PushEvent( &ev ); }

#define SENDEVENT2(etype,d1,d2)	{						\
			SDL_Event ev;								\
			ev.type = etype;							\
			ev.user.code = ((d1)<<16)|((d2)&0xffff);	\
			SDL_PushEvent( &ev ); }

#define SENDEVENTP(etype,pnt)	{	\
			SDL_Event ev;			\
			ev.type = etype;		\
			ev.user.data1 = pnt;	\
			SDL_PushEvent( &ev ); }


#define SDL_DRAWSCREEN	(SDL_USEREVENT)
#define SDL_FPSUPDATE	(SDL_USEREVENT+1)
#define SDL_RESTART		(SDL_USEREVENT+2)

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
#define SDL_DEBUGMODEBP	(SDL_USEREVENT+10)
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //

#endif	// USREVENT_H_INCLUDED
