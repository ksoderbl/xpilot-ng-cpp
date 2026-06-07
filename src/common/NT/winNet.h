/*
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bjørn Stabell
 *      Ken Ronny Schouten
 *      Bert Gijsbers
 *      Dick Balaska
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 * <https://www.gnu.org/licenses/>.
 */

/***************************************************************************\
*  winNet.h - X11 to Windoze converter										*
*																			*
*  					*
\***************************************************************************/

#ifndef _WINNET_H_
#define _WINNET_H_

#ifdef _WINDOWS

#include <winsock.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MAXHOSTNAMELEN 64

#define EWOULDBLOCK WSAEWOULDBLOCK

    extern long alarm(long seconds, void(__cdecl *func)(int));

    extern const char *_GetWSockErrText(int error);
    extern HWND notifyWnd;
    extern BOOL *hostnameFound;

#define WM_GETHOSTNAME (WM_APP + 100)

#ifdef __cplusplus
};
#endif
// extern        long    alarm(long seconds, int);
#endif /* _WINDOWS */
#endif /* _WINNET_H_ */
