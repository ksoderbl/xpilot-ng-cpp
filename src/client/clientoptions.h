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

struct ClientOptions
{
    // Connection params
    ConnectParam connectParam;
};

extern ClientOptions clientOptions;
