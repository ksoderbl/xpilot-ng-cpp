/*
 * XPilot NG, a multiplayer space war game.
 *
 * Copyright (C) 1991-2001 by
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

#ifndef PAINTDATA_H
#define PAINTDATA_H

/* need uint8_t */
#ifndef TYPES_H
#include "types.h"
#endif

/* Define Windows Colours */
#ifdef _WINDOWS
#define MAX_COLORS 16
#define MAX_COLOR_LEN 32
#endif

extern void Rectangle_start(void);
extern void Rectangle_end(void);
extern int Rectangle_add(int color, int x, int y, int width, int height);
extern void Arc_start(void);
extern void Arc_end(void);
extern int Arc_add(int color,
				   int x, int y,
				   int width, int height,
				   int angle1, int angle2);
extern int Arc_add_rgb(unsigned long color, /* Used if fullColor is on. */
					   int fallback_color,	/* Used if fullColor is off. */
					   int x, int y,
					   int width, int height,
					   int angle1, int angle2);
extern void Segment_start(void);
extern void Segment_end(void);
extern int Segment_add(int color, int x_1, int y_1, int x_2, int y_2);

#endif
