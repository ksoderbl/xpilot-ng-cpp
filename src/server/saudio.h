/*
 * XPilot NG CPP, a multiplayer space war game.
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

/* This piece of code was provided by Greg Renda (greg@ncd.com). */

#ifndef SAUDIO_H
#define SAUDIO_H

#define SDBG(x) /*#x*/

#include "audio.h"

int sound_player_init(Player *);
void sound_player_on(Player *pl, int on);
void sound_play_player(Player *, int);
void sound_play_all(int);
void sound_play_sensors(clpos_t, int);
void sound_play_queued(Player *pl);
void sound_close(Player *pl);

#endif /* SAUDIO_H */
