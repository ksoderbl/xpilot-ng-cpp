/*
 * XPilot, a multiplayer gravity war game.  Copyright (C) 1991-2001 by
 *
 *      Bjørn Stabell
 *      Ken Ronny Schouten
 *      Bert Gijsbers
 *      Dick Balaska
 *
 * Copyright (C) 2026 Kristian Söderblom
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

#pragma once

#include <string>

#include "const.h"
#include "socklib.h"

#include "clientpack.h"
#include "netclient.h"

struct ConnectParam
{
    int contact_port = SERVER_PORT;
    int server_port = 0;
    int login_port = 0;
    char nick_name[MAX_NAME_LEN] = "";
    char user_name[MAX_NAME_LEN] = "";
    char host_name[SOCK_HOSTNAME_LENGTH] = "";
    char server_addr[MAX_HOST_LEN] = "";
    char server_name[MAX_HOST_LEN] = "";
    char disp_name[MAX_DISP_LEN] = "";
    unsigned server_version = 0;
    int team = TEAM_NOT_SET;
};

struct InstrumentsOptions
{
    bool clientRanker = false;
    bool clockAMPM = false;
    bool filledDecor = false;
    bool filledWorld = false;
    bool outlineDecor = false;
    bool outlineWorld = false;
    bool showDecor = true;
    bool showItems = true;
    bool showLivesByShip = false;
    bool showMessages = true;
    bool showMyShipShape = true;
    bool showNastyShots = true;
    bool showShipShapes = true;
    bool showShipShapesHack = false;
    bool slidingRadar = true;
    bool texturedDecor = false;
    bool texturedWalls = true;
};

struct ClientOptions
{
    // Connection params
    ConnectParam connectParam;

    // Instruments on screen
    InstrumentsOptions instruments;

    bool sound = false;

    int maxVolume = 100; // maximum volume (in percent)
};

extern ClientOptions clientOptions;
