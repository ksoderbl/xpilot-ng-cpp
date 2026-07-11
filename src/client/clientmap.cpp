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

#include "clientmap.h"

#include <ctime>
#include <cstdio>
#include <cstring>
#include <sys/time.h>

#include "bit.h"
#include "rules.h"
#include "xperror.h"

#include "client.h"
#include "clientsetup.h"
#include "netclient.h"
#include "paint.h"

ClientMap clMap;

checkpoint_t *checks = nullptr;
int num_checks = 0;
edge_style_t *edge_styles = nullptr;
int num_edge_styles = 0;
polygon_style_t *polygon_styles = nullptr;
int num_polygon_styles = 0;

score_object_t score_objects[MAX_SCORE_OBJECTS];
int score_object = 0;

int Handle_start(long server_loops)
{
    int i;

    start_loops = server_loops;

    clMap.refuels.clear();
    clMap.connectors.clear();
    clMap.missiles.clear();
    clMap.balls.clear();
    clMap.ships.clear();
    clMap.missiles.clear();
    clMap.itemtypes.clear();
    clMap.ecms.clear();
    clMap.transporters.clear();
    clMap.pausers.clear();
    clMap.radarObjects.clear();
    clMap.vcannons.clear();
    clMap.vfuels.clear();
    clMap.vbases.clear();
    clMap.vdecors.clear();
    // for (i = 0; i < DEBRIS_TYPES; i++)
    //     num_debris[i] = 0;
    for (auto &debrisList : clMap.debrisTypes)
        debrisList.clear();

    damaged = 0;
    destruct = 0;
    shutdown_delay = 0;
    shutdown_count = -1;
    eyesId = (self != nullptr) ? self->id : 0;
    eyes = Other_by_id(eyesId);
    thrusttime = -1;
    shieldtime = -1;
    phasingtime = -1;
    return 0;
}

static void update_timing(void)
{
    static int frame_counter = 0;
    static struct timeval old_tv = {0, 0};
    struct timeval now;

    frame_counter++;
    gettimeofday(&now, nullptr);
    if (now.tv_sec != old_tv.tv_sec)
    {
        double usecs, fps;

        currentTime = time(nullptr);
        usecs = 1e6 + (now.tv_usec - old_tv.tv_usec);
        fps = (1e6 * frame_counter) / usecs;
        old_tv = now;
        newSecond = true;
        clientFPS = MAX(1.0, fps);
        frame_counter = 0;
    }
    else
        newSecond = false;
}

int Handle_end(long server_loops)
{
    end_loops = server_loops;
    snooping = (self && eyesId != self->id) ? true : false;
    update_timing();
    Paint_frame();
#ifdef SOUND
    audioUpdate();
#endif
    return 0;
}

int Handle_self_items(uint8_t *newNumItems)
{
    memcpy(numItems, newNumItems, NUM_ITEMS * sizeof(uint8_t));
    return 0;
}

static void update_status(int status)
{
    static int old_status = 0;

    if (BIT(old_status, OLD_GAME_OVER) && !BIT(status, OLD_GAME_OVER) && !BIT(status, OLD_PAUSE))
        Raise_window();

    /* Player appeared? */
    if (BIT(old_status, OLD_PLAYING | OLD_PAUSE | OLD_GAME_OVER) != OLD_PLAYING)
    {
        if (BIT(status, OLD_PLAYING | OLD_PAUSE | OLD_GAME_OVER) == OLD_PLAYING)
            Reset_shields();
    }

    old_status = status;
}

int Handle_self(int x, int y, int vx, int vy, int newHeading,
                double newPower, double newTurnspeed, double newTurnresistance,
                int newLockId, int newLockDist, int newLockBearing,
                int newNextCheckPoint, int newAutopilotLight,
                uint8_t *newNumItems, int newCurrentTank,
                double newFuelSum, double newFuelMax, int newPacketSize,
                int status)
{
    selfPos.x = x;
    selfPos.y = y;
    selfVel.x = vx;
    selfVel.y = vy;
    heading = newHeading;
    displayedPower = newPower;
    displayedTurnspeed = newTurnspeed;
    displayedTurnresistance = newTurnresistance;
    lock_id = newLockId;
    lock_dist = newLockDist;
    lock_dir = newLockBearing;
    nextCheckPoint = newNextCheckPoint;
    autopilotLight = newAutopilotLight;
    memcpy(numItems, newNumItems, NUM_ITEMS * sizeof(uint8_t));
    fuelCurrent = newCurrentTank;
    if (newFuelSum > fuelSum && selfVisible)
        fuelTime = FUEL_NOTIFY_TIME;
    fuelSum = newFuelSum;
    fuelMax = newFuelMax;
    selfVisible = 0;
    if (newPacketSize + 16 < packet_size)
        packet_size -= 16;
    else
        packet_size = newPacketSize;
    update_status(status);
    return 0;
}

int Handle_eyes(int id)
{
    eyesId = id;
    eyes = Other_by_id(eyesId);
    return 0;
}

int Handle_damaged(int dam)
{
    damaged = dam;
    return 0;
}

int Handle_modifiers(char *m)
{
    strlcpy(mods, m, MAX_CHARS);
    return 0;
}

int Handle_destruct(int count)
{
    destruct = count;
    return 0;
}

int Handle_shutdown(int count, int delay)
{
    shutdown_count = count;
    shutdown_delay = delay;
    return 0;
}

int Handle_thrusttime(int count, int max)
{
    thrusttime = count;
    thrusttimemax = max;
    return 0;
}

int Handle_shieldtime(int count, int max)
{
    shieldtime = count;
    shieldtimemax = max;
    return 0;
}

int Handle_phasingtime(int count, int max)
{
    phasingtime = count;
    phasingtimemax = max;
    return 0;
}

int Handle_rounddelay(int count, int max)
{
    roundDelay = count;
    roundDelayMax = max;
    return 0;
}

int Handle_refuel(int x0, int y0, int x1, int y1)
{
    refuel_t t;

    t.x0 = x0;
    t.x1 = x1;
    t.y0 = y0;
    t.y1 = y1;
    clMap.refuels.push_back(t);
    return 0;
}

int Handle_connector(int x0, int y0, int x1, int y1, int tractor)
{
    connector_t t;

    t.x0 = x0;
    t.x1 = x1;
    t.y0 = y0;
    t.y1 = y1;
    t.tractor = tractor;
    clMap.connectors.push_back(t);
    return 0;
}

int Handle_laser(int color, int x, int y, int len, int dir)
{
    laser_t t;

    t.color = color;
    t.x = x;
    t.y = y;
    t.len = len;
    t.dir = dir;
    clMap.lasers.push_back(t);
    return 0;
}

int Handle_missile(int x, int y, int len, int dir)
{
    missile_t t;

    t.x = x;
    t.y = y;
    t.dir = dir;
    t.len = len;
    clMap.missiles.push_back(t);
    return 0;
}

int Handle_ball(int x, int y, int id, int style)
{
    ball_t t;

    t.x = x;
    t.y = y;
    t.id = id;
    t.style = style;
    clMap.balls.push_back(t);
    return 0;
}

static int predict_self_dir(int received_dir)
{
    double pointer_delta = 0, dir_delta, new_dir;
    int ind = pointer_move_next - 1;
    int count = 0, int_new_dir;

    if (ind < 0)
        ind = MAX_POINTER_MOVES - 1;

    while (pointer_moves[ind].id > last_keyboard_ack && count < 50)
    {
        pointer_delta += pointer_moves[ind].movement * pointer_moves[ind].turnspeed;
        ind--;
        if (ind < 0)
            ind = MAX_POINTER_MOVES - 1;
        count++;
    }

    dir_delta = pointer_delta / (ANGLE_RESOLUTION / 2);
    new_dir = (received_dir - dir_delta);
    while (new_dir < 0)
        new_dir += ANGLE_RESOLUTION;
    while (new_dir >= ANGLE_RESOLUTION)
        new_dir -= ANGLE_RESOLUTION;
    int_new_dir = (int)(new_dir + 0.5);
    while (int_new_dir >= ANGLE_RESOLUTION)
        /* might be == ANGLE_RESOLUTION */
        int_new_dir -= ANGLE_RESOLUTION;

    return int_new_dir;
}

int Handle_ship(int x, int y, int id, int dir, int shield, int cloak,
                int eshield, int phased, int deflector)
{
    ship_t t;

    t.x = x;
    t.y = y;
    t.id = id;
    if (dirPrediction && self && self->id == id)
        t.dir = predict_self_dir(dir);
    else
        t.dir = dir;
    t.shield = shield;
    t.cloak = cloak;
    t.eshield = eshield;
    t.phased = phased;
    t.deflector = deflector;
    clMap.ships.push_back(t);

    /* if we see a ship in the center of the display, we may be watching
     * it, especially if it's us!  consider any ship there to be our eyes
     * until we see a ship that really is us.
     * BG: XXX there was a bug here.  self was dereferenced at "self->id"
     * while self could be nullptr here.
     */
    if (!selfVisible && ((x == selfPos.x && y == selfPos.y) || (self && id == self->id)))
    {

        eyesId = id;
        eyes = Other_by_id(eyesId);
        if (eyes != nullptr)
            eyeTeam = eyes->team;
        selfVisible = (self && (id == self->id));
        return Handle_radar(x, y, 3);
    }

    return 0;
}

int Handle_mine(int x, int y, int teammine, int id)
{
    mine_t t;

    t.x = x;
    t.y = y;
    t.teammine = teammine;
    t.id = id;
    clMap.mines.push_back(t);
    return 0;
}

int Handle_item(int x, int y, int type)
{
    itemtype_t t;

    t.x = x;
    t.y = y;
    t.type = type;
    clMap.itemtypes.push_back(t);
    return 0;
}

template <typename ShotT, std::size_t N>
static int Handle_shot_vector(std::array<std::vector<ShotT>, N> &shotTypes,
                              int type,
                              uint8_t *p,
                              int n,
                              const char *name)
{
    if (type < 0 || type >= static_cast<int>(N))
    {
        error("Invalid %s type %d", name, type);
        return -1;
    }

    auto &shotList = shotTypes[static_cast<std::size_t>(type)];

    if (n <= 0)
    {
        if (n < 0)
            printf("%s %d < 0\n", name, n);

        shotList.clear();
        return 0;
    }

    try
    {
        shotList.resize(static_cast<std::size_t>(n));
    }
    catch (const std::bad_alloc &)
    {
        error("No memory for %s", name);
        shotList.clear();
        return -1;
    }

    std::memcpy(shotList.data(),
                p,
                static_cast<std::size_t>(n) * sizeof(ShotT));

    return 0;
}

int Handle_fastshot(int type, uint8_t *p, int n)
{
    // warn("Handle_fastshot: type %d, n %d", type, n);
    return Handle_shot_vector(clMap.fastshotTypes, type, p, n, "fastshot");
}

int Handle_teamshot(int type, uint8_t *p, int n)
{
    // warn("Handle_teamshot: type %d, n %d", type, n);
    return Handle_shot_vector(clMap.teamshotTypes, type, p, n, "teamshot");
}

int Handle_debris(int type, uint8_t *p, int n)
{
    // warn("Handle_debris: type %d, n %d", type, n);

    if (type < 0 || type >= static_cast<int>(DEBRIS_TYPES))
    {
        error("Invalid debris type %d", type);
        return -1;
    }

    if (n <= 0)
    {
        if (n < 0)
            printf("debris %d < 0\n", n);

        clMap.debrisTypes[type].clear();
        return 0;
    }

    auto &debrisList = clMap.debrisTypes[type];

    try
    {
        debrisList.resize(static_cast<std::size_t>(n));
    }
    catch (const std::bad_alloc &)
    {
        error("No memory for debris");
        debrisList.clear();
        return -1;
    }

    std::memcpy(debrisList.data(), p, static_cast<std::size_t>(n) * sizeof(debris_t));

    return 0;
}

int Handle_wreckage(int x, int y, int wrecktype, int size, int rotation)
{
    wreckage_t t;

    t.x = x;
    t.y = y;
    t.wrecktype = wrecktype;
    t.size = size;
    t.rotation = rotation;
    clMap.wreckages.push_back(t);
    return 0;
}

int Handle_asteroid(int x, int y, int type, int size, int rotation)
{
    asteroid_t t;

    t.x = x;
    t.y = y;
    t.type = type;
    t.size = size;
    t.rotation = rotation;
    clMap.asteroids.push_back(t);
    return 0;
}

int Handle_wormhole(int x, int y)
{
    wormhole_t t;

    t.x = x - BLOCK_SZ / 2;
    t.y = y - BLOCK_SZ / 2;
    clMap.wormholes.push_back(t);
    return 0;
}

int Handle_polystyle(int polyind, int newstyle)
{
    xp_polygon_t *poly;

    if (polyind < 0 || polyind > clMap.polygons.size())
    {
        return -1;
    }

    poly = &clMap.polygons[polyind];
    poly->style = newstyle;
    /*warn("polygon %d style set to %d", polyind, newstyle);*/
    UpdateRadar = true;
    return 0;
}

int Handle_ecm(int x, int y, int size)
{
    ecm_t t;

    t.x = x;
    t.y = y;
    t.size = size;
    clMap.ecms.push_back(t);
    return 0;
}

int Handle_trans(int x1, int y1, int x2, int y2)
{
    trans_t t;

    t.x1 = x1;
    t.y1 = y1;
    t.x2 = x2;
    t.y2 = y2;
    clMap.transporters.push_back(t);
    return 0;
}

int Handle_paused(int x, int y, int count)
{
    paused_t t;

    t.x = x;
    t.y = y;
    t.count = count;
    clMap.pausers.push_back(t);
    return 0;
}

int Handle_appearing(int x, int y, int id, int count)
{
    appearing_t t;

    t.x = x;
    t.y = y;
    t.id = id;
    t.count = count;
    clMap.appearers.push_back(t);
    return 0;
}

int Handle_fastradar(int x, int y, int size)
{
    radar_t t;

    t.x = x;
    t.y = y;
    t.type = RadarEnemy;

    if ((size & 0x80) != 0)
    {
        t.type = RadarFriend;
        size &= ~0x80;
    }

    t.size = size;
    clMap.radarObjects.push_back(t);
    return 0;
}

int Handle_radar(int x, int y, int size)
{
    return Handle_fastradar((int)((double)(x * RadarWidth) / Setup->width + 0.5),
                            (int)((double)(y * RadarHeight) / Setup->height + 0.5),
                            size);
}

int Handle_vcannon(int x, int y, int type)
{
    vcannon_t t;

    t.x = x;
    t.y = y;
    t.type = type;
    clMap.vcannons.push_back(t);
    return 0;
}

int Handle_vfuel(int x, int y, double fuel)
{
    vfuel_t t;

    t.x = x;
    t.y = y;
    t.fuel = fuel;
    clMap.vfuels.push_back(t);
    return 0;
}

int Handle_vbase(int x, int y, int xi, int yi, int type)
{
    vbase_t t;

    t.x = x;
    t.y = y;
    t.xi = xi;
    t.yi = yi;
    t.type = type;
    clMap.vbases.push_back(t);
    return 0;
}

int Handle_vdecor(int x, int y, int xi, int yi, int type)
{
    vdecor_t t;

    t.x = x;
    t.y = y;
    t.xi = xi;
    t.yi = yi;
    t.type = type;
    clMap.vdecors.push_back(t);
    return 0;
}
