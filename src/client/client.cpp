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

#include <array>
#include <vector>
#include <cstring>
#include <new>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <ctime>
#include <cstdint>
#include <climits>

#include <sys/time.h>

#include "commonmacros.h"
#include "commonproto.h"
#include "bit.h"
#include "const.h"
#include "rules.h"
#include "clientsetup.h"
#include "shipshape.h"
#include "types.h"
#include "xperror.h"

#include "client.h"
#include "clientcommand.h"
#include "netclient.h"
#include "paint.h"
#include "talk.h"

client_data_t clData = {
    0,
};

char *geometry;
xp_args_t xpArgs;
Connect_param_t connectParam;

bool newbie;
int baseWarningType; /* Which type of base warning you prefer */
int maxCharsInNames;
int hudRadarDotSize;  /* Size for hudradar dot drawing */
double hudRadarScale; /* Scale for hudradar drawing */
double hudRadarLimit; /* Hudradar dots are not drawn if closer to
             your ship than this factor of visible
             range */
int hudSize;          /* Size for HUD drawing, depends on hudScale */

bool is_server = false; /* used in common code */

bool scoresChanged = true;
unsigned RadarHeight = 0;
unsigned RadarWidth = 256; /* radar width at the server */
bool UpdateRadar = false;  /* radar update because of polystyle changes? */

bool oldServer;
ipos_t selfPos;
ipos_t selfVel;
short heading;
short nextCheckPoint;

uint8_t numItems[NUM_ITEMS];     /* Count of currently owned items */
uint8_t lastNumItems[NUM_ITEMS]; /* Last item count shown */
int numItemsTime[NUM_ITEMS];     /* Number of frames to show this item count */
double showItemsTime;            /* How long to show changed item count for */
double scoreObjectTime;          /* How long to flash score objects */

short autopilotLight;

int showScoreDecimals;

short lock_id;   /* Id of player locked onto */
short lock_dir;  /* Direction of lock */
short lock_dist; /* Distance to player locked onto */

int eyesId;              /* Player we get frame updates for */
other_t *eyes = nullptr; /* Player we get frame updates for */
bool snooping;           /* are we snooping on someone else? */
int eyeTeam = TEAM_NOT_SET;

short selfVisible; /* Are we alive and playing? */
short damaged;     /* Damaged by ECM */
short destruct;    /* If self destructing */
short shutdown_delay;
short shutdown_count;
short thrusttime;
short thrusttimemax;
short shieldtime;
short shieldtimemax;
short phasingtime;
short phasingtimemax;

int roundDelay;    /* != 0 means we're in a delay */
int roundDelayMax; /* (not yet) used for graph of time
              remaining in delay */

int backgroundPointDist; /* spacing of navigation points */
int backgroundPointSize; /* size of navigation points */
int sparkSize;           /* size of debris and spark */
int shotSize;            /* size of shot */
int teamShotSize;        /* size of team shot */
double controlTime;      /* Display control for how long? */
uint8_t spark_rand;      /* Sparkling effect */
uint8_t old_spark_rand;  /* previous value of spark_rand */

double fuelSum;      /* Sum of fuel in all tanks */
double fuelMax;      /* How much fuel can you take? */
short fuelCurrent;   /* Number of currently used tank */
short numTanks;      /* Number of tanks */
double fuelTime;     /* Display fuel for how long? */
double fuelCritical; /* Fuel critical level */
double fuelWarning;  /* Fuel warning level */
double fuelNotify;   /* Fuel notify level */

char *shipShape = nullptr;      /* Shape of player's ship */
double power;                   /* Force of thrust */
double power_s;                 /* Saved power fiks */
double turnspeed;               /* How fast player acc-turns */
double turnspeed_s;             /* Saved turnspeed */
double turnresistance;          /* How much is lost in % */
double turnresistance_s;        /* Saved (see above) */
double displayedPower;          /* What the server is sending us */
double displayedTurnspeed;      /* What the server is sending us */
double displayedTurnresistance; /* What the server is sending us */
double sparkProb;               /* Sparkling effect user configurable */
int charsPerSecond;             /* Message output speed (configurable) */

double hud_move_fact;      /* scale the hud-movement (speed) */
double ptr_move_fact;      /* scale the speed pointer length */
instruments_t instruments; /* Instruments on screen */
char mods[MAX_CHARS];      /* Current modifiers in effect */
int packet_size;           /* Current frame update packet size */
int packet_loss;           /* lost packets per second */
int packet_drop;           /* dropped packets per second */
int packet_lag;            /* approximate lag in frames */
char *packet_measure;      /* packet measurement in a second */
long packet_loop;          /* start of measurement */

bool showUserName = false;                /* Show user name instead of nick name */
char servername[MAX_CHARS];               /* Name of server connecting to */
unsigned version;                         /* Version of the server */
bool toggle_shield;                       /* Are shields toggled by a press? */
bool shields = true;                      /* When shields are considered up */
bool auto_shield = true;                  /* shield drops for fire */
char modBankStr[NUM_MODBANKS][MAX_CHARS]; /* modifier banks */

int maxFPS; /* Max FPS player wants from server */
int oldMaxFPS = 0;
double clientFPS = 1.0; /* FPS client is drawing at */
int recordFPS = 0;      /* What FPS to record at */
time_t currentTime = 0; /* Current value of time() */
bool newSecond = false; /* Did time() increment this frame? */

int maxMouseTurnsPS = 0;
int mouseMovementInterval = 0;
int cumulativeMouseMovement = 0;

int clientPortStart = 0; /* First UDP port for clients */
int clientPortEnd = 0;   /* Last one (these are for firewalls) */

int lose_item;        /* index for dropping owned item */
int lose_item_active; /* one of the lose keys is pressed */

int num_playing_teams = 0;
long time_left = -1;
long start_loops, end_loops;

// TODO: use some sane data structure here
static fuelstation_t *Fuelstation_by_pos(int x, int y)
{
    int i, lo, hi, pos;

    lo = 0;
    hi = clMap.fuels.size() - 1;
    pos = x * Setup->y + y;
    while (lo < hi)
    {
        i = (lo + hi) >> 1;
        if (pos > clMap.fuels[i].pos)
            lo = i + 1;
        else
            hi = i;
    }
    if (lo == hi && pos == clMap.fuels[lo].pos)
        return &clMap.fuels[lo];
    warn("No fuelstation at (%d,%d)", x, y);
    return nullptr;
}

double Fuel_by_pos(int x, int y)
{
    fuelstation_t *fuelp;

    if ((fuelp = Fuelstation_by_pos(x, y)) == nullptr)
        return 0;
    return fuelp->fuel;
}

int Target_by_index(int ind, int *xp, int *yp, int *dead_time, double *damage)
{
    if (ind < 0 || ind >= clMap.targets.size())
        return -1;
    target_t &target = clMap.targets[ind];
    *xp = target.pos / Setup->y;
    *yp = target.pos % Setup->y;
    *dead_time = target.dead_time;
    *damage = target.damage;
    return 0;
}

int Target_alive(int x, int y, double *damage)
{
    int i, lo, hi, pos;

    lo = 0;
    hi = clMap.targets.size() - 1;
    pos = x * Setup->y + y;
    while (lo < hi)
    {
        i = (lo + hi) >> 1;
        if (pos > clMap.targets[i].pos)
            lo = i + 1;
        else
            hi = i;
    }
    if (lo == hi && pos == clMap.targets[lo].pos)
    {
        *damage = clMap.targets[lo].damage;
        return clMap.targets[lo].dead_time;
    }
    warn("No targets at (%d,%d)", x, y);
    return -1;
}

int Handle_fuel(int ind, double fuel)
{
    if (ind < 0 || ind >= clMap.fuels.size())
    {
        warn("Bad fuelstation index (%d)", ind);
        return -1;
    }
    clMap.fuels[ind].fuel = fuel;
    return 0;
}

static cannontime_t *Cannon_by_pos(int x, int y)
{
    int i, lo, hi, pos;

    lo = 0;
    hi = clMap.cannons.size() - 1;
    pos = x * Setup->y + y;
    while (lo < hi)
    {
        i = (lo + hi) >> 1;
        if (pos > clMap.cannons[i].pos)
            lo = i + 1;
        else
            hi = i;
    }
    if (lo == hi && pos == clMap.cannons[lo].pos)
        return &clMap.cannons[lo];
    warn("No cannon at (%d,%d)", x, y);
    return nullptr;
}

int Cannon_dead_time_by_pos(int x, int y, int *dot)
{
    cannontime_t *cannonp;

    if ((cannonp = Cannon_by_pos(x, y)) == nullptr)
        return -1;
    *dot = cannonp->dot;
    return cannonp->dead_time;
}

int Handle_cannon(int ind, int dead_time)
{
    if (ind < 0 || ind >= clMap.cannons.size())
    {
        warn("Bad cannon index (%d)", ind);
        return 0;
    }
    clMap.cannons[ind].dead_time = dead_time;
    return 0;
}

int Handle_target(int ind, int dead_time, double damage)
{
    if (ind < 0 || ind >= clMap.targets.size())
    {
        warn("Bad target index (%d)", ind);
        return 0;
    }
    if (dead_time == 0 && (damage <= 0.0 || damage > TARGET_DAMAGE))
        warn("BUG target %d, dead %d, damage %f", ind, dead_time, damage);

    target_t &target = clMap.targets[ind];

    if (target.dead_time > 0 && dead_time == 0)
    {
        int pos = target.pos;
        Radar_show_target(pos / Setup->y, pos % Setup->y);
    }
    else if (target.dead_time == 0 && dead_time > 0)
    {
        int pos = target.pos;
        Radar_hide_target(pos / Setup->y, pos % Setup->y);
    }

    target.dead_time = dead_time;
    target.damage = damage;

    return 0;
}

static homebase_t *Homebase_by_pos(int x, int y)
{
    int i, lo, hi, pos;

    lo = 0;
    hi = clMap.bases.size() - 1;
    pos = x * Setup->y + y;
    while (lo < hi)
    {
        i = (lo + hi) >> 1;
        if (pos > clMap.bases[i].pos)
            lo = i + 1;
        else
            hi = i;
    }
    if (lo == hi && pos == clMap.bases[lo].pos)
        return &clMap.bases[lo];
    warn("No homebase at (%d,%d)", x, y);
    return nullptr;
}

int Base_info_by_pos(int x, int y, int *idp, int *teamp)
{
    homebase_t *basep;

    if ((basep = Homebase_by_pos(x, y)) == nullptr)
        return -1;
    *idp = basep->id;
    *teamp = basep->team;
    return 0;
}

int Handle_base(int id, int ind)
{
    int i;

    if (ind < 0 || ind >= clMap.bases.size())
    {
        warn("Bad homebase index (%d)", ind);
        return -1;
    }
    for (homebase_t &base : clMap.bases)
    {
        if (base.id == id)
            base.id = -1;
    }
    clMap.bases[ind].id = id;

    return 0;
}

int Check_pos_by_index(int ind, int *xp, int *yp)
{
    if (ind < 0 || ind >= num_checks)
    {
        warn("Bad checkpoint index (%d)", ind);
        *xp = 0;
        *yp = 0;
        return -1;
    }
    *xp = checks[ind].pos / Setup->y;
    *yp = checks[ind].pos % Setup->y;
    return 0;
}

int Check_index_by_pos(int x, int y)
{
    int i, pos;

    pos = x * Setup->y + y;
    for (i = 0; i < num_checks; i++)
    {
        if (pos == checks[i].pos)
            return i;
    }
    warn("Can't find checkpoint (%d,%d)", x, y);
    return 0;
}

/*
 * Convert a 'space' map block into a dot.
 */
static void Map_make_dot(uint8_t *data)
{
    if (*data == SETUP_SPACE)
        *data = SETUP_SPACE_DOT;
    else if (*data == SETUP_DECOR_FILLED)
        *data = SETUP_DECOR_DOT_FILLED;
    else if (*data == SETUP_DECOR_RU)
        *data = SETUP_DECOR_DOT_RU;
    else if (*data == SETUP_DECOR_RD)
        *data = SETUP_DECOR_DOT_RD;
    else if (*data == SETUP_DECOR_LU)
        *data = SETUP_DECOR_DOT_LU;
    else if (*data == SETUP_DECOR_LD)
        *data = SETUP_DECOR_DOT_LD;
}

/*
 * Optimize the drawing of all blue space dots by converting
 * certain map objects into a specialised form of their type.
 */
void Map_dots(void)
{
    int i,
        x,
        y,
        start;
    uint8_t dot[256];

    /*
     * Lookup table to recognize dots.
     */
    memset(dot, 0, sizeof dot);
    dot[SETUP_SPACE_DOT] = 1;
    dot[SETUP_DECOR_DOT_FILLED] = 1;
    dot[SETUP_DECOR_DOT_RU] = 1;
    dot[SETUP_DECOR_DOT_RD] = 1;
    dot[SETUP_DECOR_DOT_LU] = 1;
    dot[SETUP_DECOR_DOT_LD] = 1;

    /*
     * Restore the map to unoptimized form.
     */
    for (i = Setup->x * Setup->y; i-- > 0;)
    {
        if (dot[Setup->map_data[i]])
        {
            if (Setup->map_data[i] == SETUP_SPACE_DOT)
                Setup->map_data[i] = SETUP_SPACE;
            else if (Setup->map_data[i] == SETUP_DECOR_DOT_FILLED)
                Setup->map_data[i] = SETUP_DECOR_FILLED;
            else if (Setup->map_data[i] == SETUP_DECOR_DOT_RU)
                Setup->map_data[i] = SETUP_DECOR_RU;
            else if (Setup->map_data[i] == SETUP_DECOR_DOT_RD)
                Setup->map_data[i] = SETUP_DECOR_RD;
            else if (Setup->map_data[i] == SETUP_DECOR_DOT_LU)
                Setup->map_data[i] = SETUP_DECOR_LU;
            else if (Setup->map_data[i] == SETUP_DECOR_DOT_LD)
                Setup->map_data[i] = SETUP_DECOR_LD;
        }
    }

    /*
     * Lookup table to test for map data which can be turned into a dot.
     */
    memset(dot, 0, sizeof dot);
    dot[SETUP_SPACE] = 1;
    if (!instruments.showDecor)
    {
        dot[SETUP_DECOR_FILLED] = 1;
        dot[SETUP_DECOR_RU] = 1;
        dot[SETUP_DECOR_RD] = 1;
        dot[SETUP_DECOR_LU] = 1;
        dot[SETUP_DECOR_LD] = 1;
    }

    /*
     * Optimize.
     */
    if (backgroundPointSize > 0)
    {
        if (BIT(Setup->mode, WRAP_PLAY))
        {
            for (x = 0; x < Setup->x; x++)
            {
                if (dot[Setup->map_data[x * Setup->y]])
                    Map_make_dot(&Setup->map_data[x * Setup->y]);
            }
            for (y = 0; y < Setup->y; y++)
            {
                if (dot[Setup->map_data[y]])
                    Map_make_dot(&Setup->map_data[y]);
            }
            start = backgroundPointDist;
        }
        else
            start = 0;

        if (backgroundPointDist > 0)
        {
            for (x = start; x < Setup->x; x += backgroundPointDist)
            {
                for (y = start; y < Setup->y; y += backgroundPointDist)
                {
                    if (dot[Setup->map_data[x * Setup->y + y]])
                        Map_make_dot(&Setup->map_data[x * Setup->y + y]);
                }
            }
        }
        for (auto &cannon : clMap.cannons)
        {
            x = cannon.pos / Setup->y;
            y = cannon.pos % Setup->y;
            if ((x == 0 || y == 0) && BIT(Setup->mode, WRAP_PLAY))
                cannon.dot = 1;
            else if (backgroundPointDist > 0 && x % backgroundPointDist == 0 && y % backgroundPointDist == 0)
                cannon.dot = 1;
            else
                cannon.dot = 0;
        }
    }
}

/*
 * Optimize the drawing of all blue map objects by converting
 * their map type to a bitmask with bits for each blue segment.
 */
void Map_restore(int startx, int starty, int width, int height)
{
    int i, j,
        x, y,
        map_index,
        type;

    /*
     * Restore an optimized map to its original unoptimized state.
     */
    x = startx;
    for (i = 0; i < width; i++, x++)
    {
        if (x < 0)
            x += Setup->x;
        else if (x >= Setup->x)
            x -= Setup->x;

        y = starty;
        for (j = 0; j < height; j++, y++)
        {
            if (y < 0)
                y += Setup->y;
            else if (y >= Setup->y)
                y -= Setup->y;

            map_index = x * Setup->y + y;

            type = Setup->map_data[map_index];
            if ((type & BLUE_BIT) == 0)
            {
                if (type == SETUP_FILLED_NO_DRAW)
                    Setup->map_data[map_index] = SETUP_FILLED;
            }
            else if ((type & BLUE_FUEL) == BLUE_FUEL)
                Setup->map_data[map_index] = SETUP_FUEL;

            else if (type & BLUE_OPEN)
            {
                if (type & BLUE_BELOW)
                    Setup->map_data[map_index] = SETUP_REC_RD;
                else
                    Setup->map_data[map_index] = SETUP_REC_LU;
            }
            else if (type & BLUE_CLOSED)
            {
                if (type & BLUE_BELOW)
                    Setup->map_data[map_index] = SETUP_REC_LD;
                else
                    Setup->map_data[map_index] = SETUP_REC_RU;
            }
            else
                Setup->map_data[map_index] = SETUP_FILLED;
        }
    }
}

void Map_blue(int startx, int starty, int width, int height)
{
    int i, j,
        x, y,
        map_index,
        type,
        newtype;
    uint8_t blue[256];
    bool outline = false;

    if (instruments.outlineWorld ||
        instruments.filledWorld ||
        instruments.texturedWalls)
        outline = true;
    /*
     * Optimize the map for blue.
     */
    memset(blue, 0, sizeof blue);
    blue[SETUP_FILLED] = BLUE_LEFT | BLUE_UP | BLUE_RIGHT | BLUE_DOWN;
    blue[SETUP_FILLED_NO_DRAW] = blue[SETUP_FILLED];
    blue[SETUP_FUEL] = blue[SETUP_FILLED];
    blue[SETUP_REC_RU] = BLUE_RIGHT | BLUE_UP;
    blue[SETUP_REC_RD] = BLUE_RIGHT | BLUE_DOWN;
    blue[SETUP_REC_LU] = BLUE_LEFT | BLUE_UP;
    blue[SETUP_REC_LD] = BLUE_LEFT | BLUE_DOWN;
    blue[BLUE_BIT | BLUE_OPEN] =
        blue[BLUE_BIT | BLUE_OPEN | BLUE_LEFT] =
            blue[BLUE_BIT | BLUE_OPEN | BLUE_UP] =
                blue[BLUE_BIT | BLUE_OPEN | BLUE_LEFT | BLUE_UP] =
                    blue[SETUP_REC_LU];
    blue[BLUE_BIT | BLUE_OPEN | BLUE_BELOW] =
        blue[BLUE_BIT | BLUE_OPEN | BLUE_BELOW | BLUE_RIGHT] =
            blue[BLUE_BIT | BLUE_OPEN | BLUE_BELOW | BLUE_DOWN] =
                blue[BLUE_BIT | BLUE_OPEN | BLUE_BELOW | BLUE_RIGHT | BLUE_DOWN] =
                    blue[SETUP_REC_RD];
    blue[BLUE_BIT | BLUE_CLOSED] =
        blue[BLUE_BIT | BLUE_CLOSED | BLUE_RIGHT] =
            blue[BLUE_BIT | BLUE_CLOSED | BLUE_UP] =
                blue[BLUE_BIT | BLUE_CLOSED | BLUE_RIGHT | BLUE_UP] =
                    blue[SETUP_REC_RU];
    blue[BLUE_BIT | BLUE_CLOSED | BLUE_BELOW] =
        blue[BLUE_BIT | BLUE_CLOSED | BLUE_BELOW | BLUE_LEFT] =
            blue[BLUE_BIT | BLUE_CLOSED | BLUE_BELOW | BLUE_DOWN] =
                blue[BLUE_BIT | BLUE_CLOSED | BLUE_BELOW | BLUE_LEFT | BLUE_DOWN] =
                    blue[SETUP_REC_LD];
    for (i = BLUE_BIT; i < (int)(sizeof blue); i++)
    {
        if ((i & BLUE_FUEL) == BLUE_FUEL || (i & (BLUE_OPEN | BLUE_CLOSED)) == 0)
            blue[i] = blue[SETUP_FILLED];
    }

    x = startx;
    for (i = 0; i < width; i++, x++)
    {
        if (x < 0)
            x += Setup->x;
        else if (x >= Setup->x)
            x -= Setup->x;

        y = starty;
        for (j = 0; j < height; j++, y++)
        {
            if (y < 0)
                y += Setup->y;
            else if (y >= Setup->y)
                y -= Setup->y;

            map_index = x * Setup->y + y;

            type = Setup->map_data[map_index];
            newtype = 0;
            switch (type)
            {
            case SETUP_FILLED:
            case SETUP_FILLED_NO_DRAW:
            case SETUP_FUEL:
                newtype = BLUE_BIT;
                if (type == SETUP_FUEL)
                {
                    newtype |= BLUE_FUEL;
                }
                if ((x == 0)
                        ? (!BIT(Setup->mode, WRAP_PLAY) ||
                           !(blue[Setup->map_data[(Setup->x - 1) * Setup->y + y]] & BLUE_RIGHT))
                        : !(blue[Setup->map_data[(x - 1) * Setup->y + y]] & BLUE_RIGHT))
                    newtype |= BLUE_LEFT;
                if ((y == 0)
                        ? (!BIT(Setup->mode, WRAP_PLAY) ||
                           !(blue[Setup->map_data[x * Setup->y + Setup->y - 1]] & BLUE_UP))
                        : !(blue[Setup->map_data[x * Setup->y + (y - 1)]] & BLUE_UP))
                    newtype |= BLUE_DOWN;
                if (!outline || ((x == Setup->x - 1)
                                     ? (!BIT(Setup->mode, WRAP_PLAY) || !(blue[Setup->map_data[y]] & BLUE_LEFT))
                                     : !(blue[Setup->map_data[(x + 1) * Setup->y + y]] & BLUE_LEFT)))
                    newtype |= BLUE_RIGHT;
                if (!outline || ((y == Setup->y - 1)
                                     ? (!BIT(Setup->mode, WRAP_PLAY) || !(blue[Setup->map_data[x * Setup->y]] & BLUE_DOWN))
                                     : !(blue[Setup->map_data[x * Setup->y + (y + 1)]] & BLUE_DOWN)))
                    newtype |= BLUE_UP;
                break;

            case SETUP_REC_LU:
                newtype = BLUE_BIT | BLUE_OPEN;
                if (x == 0
                        ? (!BIT(Setup->mode, WRAP_PLAY) ||
                           !(blue[Setup->map_data[(Setup->x - 1) * Setup->y + y]] & BLUE_RIGHT))
                        : !(blue[Setup->map_data[(x - 1) * Setup->y + y]] & BLUE_RIGHT))
                    newtype |= BLUE_LEFT;
                if (!outline || ((y == Setup->y - 1)
                                     ? (!BIT(Setup->mode, WRAP_PLAY) || !(blue[Setup->map_data[x * Setup->y]] & BLUE_DOWN))
                                     : !(blue[Setup->map_data[x * Setup->y + (y + 1)]] & BLUE_DOWN)))
                    newtype |= BLUE_UP;
                break;

            case SETUP_REC_RU:
                newtype = BLUE_BIT | BLUE_CLOSED;
                if (!outline || ((x == Setup->x - 1)
                                     ? (!BIT(Setup->mode, WRAP_PLAY) || !(blue[Setup->map_data[y]] & BLUE_LEFT))
                                     : !(blue[Setup->map_data[(x + 1) * Setup->y + y]] & BLUE_LEFT)))
                    newtype |= BLUE_RIGHT;
                if (!outline || ((y == Setup->y - 1)
                                     ? (!BIT(Setup->mode, WRAP_PLAY) || !(blue[Setup->map_data[x * Setup->y]] & BLUE_DOWN))
                                     : !(blue[Setup->map_data[x * Setup->y + (y + 1)]] & BLUE_DOWN)))
                    newtype |= BLUE_UP;
                break;

            case SETUP_REC_LD:
                newtype = BLUE_BIT | BLUE_BELOW | BLUE_CLOSED;
                if ((x == 0)
                        ? (!BIT(Setup->mode, WRAP_PLAY) ||
                           !(blue[Setup->map_data[(Setup->x - 1) * Setup->y + y]] & BLUE_RIGHT))
                        : !(blue[Setup->map_data[(x - 1) * Setup->y + y]] & BLUE_RIGHT))
                    newtype |= BLUE_LEFT;
                if ((y == 0)
                        ? (!BIT(Setup->mode, WRAP_PLAY) ||
                           !(blue[Setup->map_data[x * Setup->y + Setup->y - 1]] & BLUE_UP))
                        : !(blue[Setup->map_data[x * Setup->y + (y - 1)]] & BLUE_UP))
                    newtype |= BLUE_DOWN;
                break;

            case SETUP_REC_RD:
                newtype = BLUE_BIT | BLUE_BELOW | BLUE_OPEN;
                if (!outline || ((x == Setup->x - 1)
                                     ? (!BIT(Setup->mode, WRAP_PLAY) || !(blue[Setup->map_data[y]] & BLUE_LEFT))
                                     : !(blue[Setup->map_data[(x + 1) * Setup->y + y]] & BLUE_LEFT)))
                    newtype |= BLUE_RIGHT;
                if ((y == 0)
                        ? (!BIT(Setup->mode, WRAP_PLAY) ||
                           !(blue[Setup->map_data[x * Setup->y + Setup->y - 1]] & BLUE_UP))
                        : !(blue[Setup->map_data[x * Setup->y + (y - 1)]] & BLUE_UP))
                    newtype |= BLUE_DOWN;
                break;

            default:
                continue;
            }
            if (newtype != 0)
            {
                if (newtype == BLUE_BIT)
                    newtype = SETUP_FILLED_NO_DRAW;
                Setup->map_data[map_index] = newtype;
            }
        }
    }
}

/* Get signed short and advance ptr */
static int get_short(char **ptr)
{
    *ptr += 2;
    return ((signed char)*(*ptr - 2) << 8) + (uint8_t)(*(*ptr - 1));
}

/* Unsigned version */
static unsigned int get_ushort(char **ptr)
{
    *ptr += 2;
    return ((uint8_t)*(*ptr - 2) << 8) + (uint8_t)*(*ptr - 1);
}

static int get_32bit(char **ptr)
{
    int res;

    res = get_ushort(ptr) << 16;
    return res + get_ushort(ptr);
}

static void parse_styles(char **callptr)
{
    int i, num_bmaps;
    char *ptr;

    ptr = *callptr;
    num_polygon_styles = *ptr++ & 0xff;
    num_edge_styles = *ptr++ & 0xff;
    num_bmaps = *ptr++ & 0xff;

    polygon_styles = XMALLOC(polygon_style_t, MAX(1, num_polygon_styles));
    if (polygon_styles == nullptr)
    {
        error("no memory for polygon styles");
        exit(1);
    }

    edge_styles = XMALLOC(edge_style_t, MAX(1, num_edge_styles));
    if (edge_styles == nullptr)
    {
        error("no memory for edge styles");
        exit(1);
    }

    for (i = 0; i < num_polygon_styles; i++)
    {
        polygon_styles[i].rgb = get_32bit(&ptr);
        polygon_styles[i].texture = *ptr++ & 0xff;
        polygon_styles[i].def_edge_style = *ptr++ & 0xff;
        polygon_styles[i].flags = *ptr++ & 0xff;
    }

    if (num_polygon_styles == 0)
    {
        /* default polygon style */
        polygon_styles[0].flags = 0;
        polygon_styles[0].def_edge_style = 0;
        num_polygon_styles = 1;
    }

    for (i = 0; i < num_edge_styles; i++)
    {
        edge_styles[i].width = *ptr++; /* -1 means hidden */
        edge_styles[i].rgb = get_32bit(&ptr);
        /* kps - what the **** is this ? */
        /* baron - it's line style from XSetLineAttributes */
        /* 0 = LineSolid, 1 = LineOnOffDash, 2 = LineDoubleDash */
        edge_styles[i].style =
            (*ptr == 1) ? 1 : (*ptr == 2) ? 2
                                          : 0;
        ptr++;
    }

    for (i = 0; i < num_bmaps; i++)
    {
        char fname[30];
        int flags;

        strlcpy(fname, ptr, 30);
        ptr += strlen(fname) + 1;
        flags = *ptr++ & 0xff;
        Bitmap_add(fname, 1, flags);
    }
    *callptr = ptr;
}

static int init_polymap(void)
{
    int i, j, startx, starty, ecount, edgechange, current_estyle;
    int dx, dy, cx, cy, pc;
    int *styles;
    xp_polygon_t *poly;
    ipos_t *points, min, max;
    char *ptr, *edgeptr;

    oldServer = false;
    ptr = (char *)Setup->map_data;

    parse_styles(&ptr);

    int num_polygons = get_ushort(&ptr);
    // polygons = XMALLOC(xp_polygon_t, num_polygons);
    // if (polygons == nullptr)
    // {
    //     error("no memory for polygons");
    //     exit(1);
    // }
    warn("init_polymap: num_polygons: %d", num_polygons);
    clMap.polygons.resize(num_polygons);

    for (auto &poly : clMap.polygons)
    {
        poly.style = *ptr++ & 0xff;
        current_estyle = polygon_styles[poly.style].def_edge_style;
        dx = 0;
        dy = 0;
        ecount = get_ushort(&ptr);
        edgeptr = ptr;
        if (ecount)
            edgechange = get_ushort(&edgeptr);
        else
            edgechange = INT_MAX;
        ptr += ecount * 2;
        pc = get_ushort(&ptr);
        if ((points = XMALLOC(ipos_t, pc)) == nullptr)
        {
            error("no memory for points");
            exit(1);
        }
        if (ecount)
        {
            if ((styles = XMALLOC(int, pc)) == nullptr)
            {
                error("no memory for special edges");
                exit(1);
            }
        }
        else
            styles = nullptr;
        startx = get_ushort(&ptr);
        starty = get_ushort(&ptr);
        points[0].x = cx = min.x = max.x = startx;
        points[0].y = cy = min.y = max.y = starty;

        if (!edgechange)
        {
            current_estyle = get_ushort(&edgeptr);
            ecount--;
            if (ecount)
                edgechange = get_ushort(&edgeptr);
        }
        if (styles)
            styles[0] = current_estyle;

        for (j = 1; j < pc; j++)
        {
            dx = get_short(&ptr);
            dy = get_short(&ptr);
            cx += dx;
            cy += dy;
            if (min.x > cx)
                min.x = cx;
            if (min.y > cy)
                min.y = cy;
            if (max.x < cx)
                max.x = cx;
            if (max.y < cy)
                max.y = cy;
            points[j].x = dx;
            points[j].y = dy;

            if (edgechange == j)
            {
                current_estyle = get_ushort(&edgeptr);
                ecount--;
                if (ecount)
                    edgechange = get_ushort(&edgeptr);
            }
            if (styles)
                styles[j] = current_estyle;
        }
        poly.points = points;
        poly.edge_styles = styles;
        poly.num_points = pc;
        poly.bounds.x = min.x;
        poly.bounds.y = min.y;
        poly.bounds.w = max.x - min.x;
        poly.bounds.h = max.y - min.y;
    }
    int num_bases = *ptr++ & 0xff;
    for (i = 0; i < num_bases; i++)
    {
        homebase_t base;
        /* base.pos is not used */
        base.id = -1;
        base.team = *ptr++ & 0xff;
        cx = get_ushort(&ptr);
        cy = get_ushort(&ptr);
        base.bounds.x = cx - BLOCK_SZ / 2;
        base.bounds.y = cy - BLOCK_SZ / 2;
        base.bounds.w = BLOCK_SZ;
        base.bounds.h = BLOCK_SZ;
        if (*ptr < 16)
            base.type = SETUP_BASE_RIGHT;
        else if (*ptr < 48)
            base.type = SETUP_BASE_UP;
        else if (*ptr < 80)
            base.type = SETUP_BASE_LEFT;
        else if (*ptr < 112)
            base.type = SETUP_BASE_DOWN;
        else
            base.type = SETUP_BASE_RIGHT;
        base.appeartime = 0;
        clMap.bases.push_back(base);
        ptr++;
    }
    int num_fuels = get_ushort(&ptr);
    for (i = 0; i < num_fuels; i++)
    {
        fuelstation_t fs;
        cx = get_ushort(&ptr);
        cy = get_ushort(&ptr);
        fs.fuel = MAX_STATION_FUEL;
        fs.bounds.x = cx - BLOCK_SZ / 2;
        fs.bounds.y = cy - BLOCK_SZ / 2;
        fs.bounds.w = BLOCK_SZ;
        fs.bounds.h = BLOCK_SZ;
    }
    num_checks = *ptr++ & 0xff;
    if (num_checks != 0)
    {

        checks = XMALLOC(checkpoint_t, num_checks);
        if (checks == nullptr)
        {
            error("No memory for checkpoints (%d)", num_checks);
            exit(1);
        }
    }
    for (i = 0; i < num_checks; i++)
    {
        cx = get_ushort(&ptr);
        cy = get_ushort(&ptr);
        checks[i].bounds.x = cx - BLOCK_SZ / 2;
        checks[i].bounds.y = cy - BLOCK_SZ / 2;
        checks[i].bounds.w = BLOCK_SZ;
        checks[i].bounds.h = BLOCK_SZ;
    }

    /*
     * kps - hack.
     * Player can disable downloading of textures by having texturedWalls off.
     */
    if (instruments.texturedWalls && Setup->data_url[0])
        Mapdata_setup(Setup->data_url);
    Colors_init_style_colors();

    return 0;
}

static int init_blockmap(void)
{
    int i,
        max,
        type;
    uint8_t types[256];

    num_checks = 0;
    clMap.fuels.clear();
    clMap.bases.clear();
    clMap.cannons.clear();
    clMap.targets.clear();
    checks = nullptr;
    memset(types, 0, sizeof types);
    types[SETUP_FUEL] = 1;
    types[SETUP_CANNON_UP] = 2;
    types[SETUP_CANNON_RIGHT] = 2;
    types[SETUP_CANNON_DOWN] = 2;
    types[SETUP_CANNON_LEFT] = 2;
    for (i = SETUP_TARGET; i < SETUP_TARGET + 10; i++)
        types[i] = 3;
    for (i = SETUP_BASE_LOWEST; i <= SETUP_BASE_HIGHEST; i++)
        types[i] = 4;
    for (i = 0; i < OLD_MAX_CHECKS; i++)
        types[SETUP_CHECK + i] = 5;
    max = Setup->x * Setup->y;
    for (i = 0; i < max; i++)
    {
        switch (types[Setup->map_data[i]])
        {
        case 5:
            num_checks++;
            break;
        default:
            break;
        }
    }
    if (num_checks != 0)
    {
        checks = XMALLOC(checkpoint_t, num_checks);
        if (checks == nullptr)
        {
            error("No memory for Map checks (%d)", num_checks);
            return -1;
        }
        num_checks = 0;
    }

    for (i = 0; i < max; i++)
    {
        type = Setup->map_data[i];
        switch (types[type])
        {
        case 1:
            fuelstation_t fs;
            fs.pos = i;
            fs.fuel = MAX_STATION_FUEL;
            clMap.fuels.push_back(fs);
            break;
        case 2:
            cannontime_t cannon;
            cannon.pos = i;
            cannon.dead_time = 0;
            cannon.dot = 0;
            clMap.cannons.push_back(cannon);
            break;
        case 3:
            target_t target;
            target.pos = i;
            target.dead_time = 0;
            target.damage = TARGET_DAMAGE;
            clMap.targets.push_back(target);
            break;
        case 4:
            homebase_t base;
            base.pos = i;
            base.id = -1;
            base.team = type % 10;
            base.type = type - (type % 10);
            base.appeartime = 0;
            clMap.bases.push_back(base);
            Setup->map_data[i] = type - (type % 10);
            break;
        case 5:
            checks[type - SETUP_CHECK].pos = i;
            num_checks++;
            Setup->map_data[i] = SETUP_CHECK;
            break;
        default:
            break;
        }
    }
    return 0;
}

static int Map_init(void)
{
    return oldServer ? init_blockmap() : init_polymap();
}

static int Map_cleanup(void)
{
    clMap.bases.clear();
    clMap.fuels.clear();
    clMap.targets.clear();
    clMap.cannons.clear();
    return 0;
}

homebase_t *Homebase_by_id(int id)
{
    int i;

    if (id != -1)
    {
        for (homebase_t &base : clMap.bases)
        {
            if (base.id == id)
                return &base;
        }
    }
    return nullptr;
}

int Handle_leave(int id)
{
    other_t *other;
    char msg[MSG_LEN];

    if ((other = Other_by_id(id)) != nullptr)
    {
        if (other == self)
        {
            warn("Self left?!");
            self = nullptr;
        }
        Free_ship_shape(other->ship);
        other->ship = nullptr;
        /*
         * Silent about tanks and robots.
         */
        if (other->mychar != 'T' && other->mychar != 'R')
        {
            sprintf(msg, "%s left this world.", other->nick_name);
            Add_message(msg);
        }
        num_others--;
        while (other < &Others[num_others])
        {
            *other = other[1];
            other++;
        }
        scoresChanged = true;
    }
    return 0;
}

int Handle_player(int id, int player_team, int mychar,
                  char *nick_name, char *user_name, char *host_name,
                  char *shape, int myself)
{
    other_t *other;

    if (BIT(Setup->mode, TEAM_PLAY) && (player_team < 0 || player_team >= MAX_TEAMS))
    {
        warn("Illegal team %d for received player, setting to 0", player_team);
        player_team = 0;
    }
    if ((other = Other_by_id(id)) == nullptr)
    {
        if (num_others >= max_others)
        {
            max_others += 5;
            if (num_others == 0)
                Others = XMALLOC(other_t, max_others);
            else
                Others = XREALLOC(other_t, Others, max_others);
            if (Others == nullptr)
                fatal("Not enough memory for player info");
            if (self != nullptr)
                /* We've made 'self' the first member of Others[]. */
                self = &Others[0];
        }
        other = &Others[num_others++];
    }
    if (self == nullptr && (myself || (version < 0x4F10 && strcmp(connectParam.nick_name, nick_name) == 0)))
    {
        if (other != &Others[0])
        {
            /* Make 'self' the first member of Others[]. */
            *other = Others[0];
            other = &Others[0];
        }
        self = other;
    }
    memset(other, 0, sizeof(other_t));
    other->id = id;
    other->team = player_team;
    other->mychar = mychar;
    strlcpy(other->nick_name, nick_name, sizeof(other->nick_name));
    strlcpy(other->user_name, user_name, sizeof(other->user_name));
    strlcpy(other->host_name, host_name, sizeof(other->host_name));
    strlcpy(other->id_string, nick_name, sizeof(other->id_string));
    other->max_chars_in_names = -1;
    scoresChanged = true;
    other->ship = Convert_shape_str(shape);
    Calculate_shield_radius(other->ship);

    return 0;
}

int Handle_team(int id, int pl_team)
{
    other_t *other;

    other = Other_by_id(id);
    if (other == nullptr)
    {
        warn("Received packet to change team for nonexistent id %d", id);
        return 0;
    }
    if (BIT(Setup->mode, TEAM_PLAY) && (pl_team < 0 || pl_team >= MAX_TEAMS))
    {
        warn("Illegal team %d received for player id %d", pl_team, id);
        return 0;
    }
    other->team = pl_team;
    scoresChanged = true;

    return 0;
}

int Handle_score(int id, double score, int life, int mychar, int alliance)
{
    other_t *other;

    if ((other = Other_by_id(id)) == nullptr)
    {
        warn("Can't update score for non-existing player %d,%.2f,%d",
             id, score, life);
        return 0;
    }
    else if (other->score != score || other->life != life || other->mychar != mychar || other->alliance != alliance)
    {
        other->score = score;
        other->life = life;
        other->mychar = mychar;
        other->alliance = alliance;
        scoresChanged = true;
    }

    return 0;
}

int Handle_team_score(int team, double score)
{
    warn("Handle_team_score: team: %d, score: %f", team, score);
    return 0;
}

int Handle_timing(int id, int check, int round, long tloops)
{
    other_t *other;

    if ((other = Other_by_id(id)) == nullptr)
    {
        warn("Can't update timing for non-existing player %d,%d,%d",
             id, check, round);
        return 0;
    }
    else if (other->check != check || other->round != round)
    {
        other->check = check;
        other->round = round;
        other->timing = round * num_checks + check;
        other->timing_loops = tloops;
        scoresChanged = true;
    }

    return 0;
}

int Handle_score_object(double score, int x, int y, char *msg)
{
    score_object_t *sobj = &score_objects[score_object];

    sobj->score = score;
    sobj->x = x;
    sobj->y = y;
    sobj->life_time = scoreObjectTime;

    /* Initialize sobj->hud_msg (is shown on the HUD) */
    if (msg[0] != '\0')
    {
        if (Using_score_decimals())
            sprintf(sobj->hud_msg, "%s %.*f", msg, showScoreDecimals, score);
        else
        {
            int sc = (int)(score >= 0.0 ? score + 0.5 : score - 0.5);
            sprintf(sobj->hud_msg, "%s %d", msg, sc);
        }
        sobj->hud_msg_len = strlen(sobj->hud_msg);
        sobj->hud_msg_width = -1;
    }
    else
        sobj->hud_msg_len = 0;

    /* Initialize sobj->msg data (is shown on game area) */
    if (Using_score_decimals())
        sprintf(sobj->msg, "%.*f", showScoreDecimals, score);
    else
    {
        int sc = (int)(score >= 0.0 ? score + 0.5 : score - 0.5);
        sprintf(sobj->msg, "%d", sc);
    }
    sobj->msg_len = strlen(sobj->msg);
    sobj->msg_width = -1;

    /* Update global index variable */
    score_object = (score_object + 1) % MAX_SCORE_OBJECTS;

    return 0;
}

int Handle_message(char *msg)
{
    int i;
    char ignoree[MAX_CHARS];
    other_t *other;

    if (msg[strlen(msg) - 1] == ']')
    {
        for (i = strlen(msg) - 1; i > 0; i--)
        {
            if (msg[i - 1] == ' ' && msg[i] == '[')
                break;
        }

        if (i == 0)
        { /* Odd, but let it pass */
            Add_message(msg);
            return 0;
        }

        strcpy(ignoree, &msg[i + 1]);

        for (i = 0; i < (int)strlen(ignoree); i++)
        {
            if (ignoree[i] == ']')
                break;
        }
        ignoree[i] = '\0';

        other = Other_by_name(ignoree, false);

        if (other == nullptr)
        { /* Not in list, probably servermessage */
            Add_message(msg);
            return 0;
        }

        if (other->ignorelevel <= 0)
        {
            Add_message(msg);
            return 0;
        }

        if (other->ignorelevel >= 2)
            return 0;

        /* ignorelevel must be 1 */

        crippleTalk(msg);
        Add_message(msg);
    }
    else
        Add_message(msg);
    return 0;
}

int Handle_time_left(long sec)
{
    if (sec >= 0 && sec < 10 && (time_left > sec || sec == 0))
        Play_beep();
    time_left = (sec >= 0) ? sec : 0;
    return 0;
}

bool Using_score_decimals(void)
{
    if (showScoreDecimals > 0 && version >= 0x4500 && (version < 0x4F09 || version >= 0x4F11))
        return true;
    return false;
}

int Client_init(char *server, unsigned server_version)
{
    version = server_version;
    if (server_version < 0x4F09)
        oldServer = true;
    else
        oldServer = false;

    Make_table();

    if (Paint_init() == -1)
        return -1;

    strlcpy(servername, server, sizeof(servername));

    return 0;
}

int Client_setup(void)
{
    if (Map_init() == -1)
        return -1;

    if (oldServer)
    {
        Map_dots();
        Map_restore(0, 0, Setup->x, Setup->y);
        Map_blue(0, 0, Setup->x, Setup->y);
        /* kps -remove this, you shouldn't change options this way */
        /* No one wants this on old-style maps anyway, so turn it off.
         * I do, so turn it on.
         * This allows people to turn it on in their .xpilotrc for new maps
         * without affecting old ones. It's still possible to turn in on
         * from the config menu during play for old maps.
         * -- But doesn't seem to work anyway if turned on? Well who cares */
        instruments.texturedWalls = false;
    }

    RadarHeight = (RadarWidth * Setup->height) / Setup->width;

    if (Init_playing_windows() == -1)
        return -1;

    if (Alloc_msgs() == -1)
        return -1;

    if (Alloc_history() == -1)
        return -1;

    return 0;
}

int Client_fps_request(void)
{
    LIMIT(maxFPS, 1, MAX_SUPPORTED_FPS);
    oldMaxFPS = maxFPS;
    return Send_fps_request(maxFPS);
}

int Check_client_fps(void)
{
    if (oldMaxFPS != maxFPS)
        return Client_fps_request();
    return 0;
}

int Client_power(void)
{
    int i;

    if (Send_power(power) == -1 ||
        Send_power_s(power_s) == -1 ||
        Send_turnspeed(turnspeed) == -1 ||
        Send_turnspeed_s(turnspeed_s) == -1 ||
        Send_turnresistance(turnresistance) == -1 ||
        Send_turnresistance_s(turnresistance_s) == -1)
        return -1;

    if (Check_view_dimensions() == -1)
        return -1;

    for (i = 0; i < NUM_MODBANKS; i++)
    {
        if (Send_modifier_bank(i) == -1)
            return -1;
    }

    return 0;
}

int Client_start(void)
{
    Key_init();

    return 0;
}

void Client_cleanup(void)
{
    int i;

    Pointer_control_set_state(false);
    Platform_specific_cleanup();
    Free_selectionAndHistory();
    Free_msgs();
    if (max_others > 0)
    {
        for (i = 0; i < num_others; i++)
        {
            other_t *other = &Others[i];
            Free_ship_shape(other->ship);
        }
        free(Others);
        num_others = 0;
        max_others = 0;
    }

    clMap.refuels.clear();
    clMap.connectors.clear();
    clMap.lasers.clear();
    clMap.missiles.clear();
    clMap.balls.clear();
    clMap.ships.clear();
    clMap.mines.clear();
    clMap.ecms.clear();
    clMap.transporters.clear();
    clMap.pausers.clear();
    clMap.appearers.clear();
    clMap.radarObjects.clear();
    clMap.vcannons.clear();
    clMap.vfuels.clear();
    clMap.vbases.clear();
    clMap.vdecors.clear();
    clMap.itemtypes.clear();
    clMap.wreckages.clear();
    clMap.asteroids.clear();
    clMap.wormholes.clear();
    Map_cleanup();
    Paint_cleanup();
}

int Client_pointer_move(int movement)
{
    if (maxMouseTurnsPS == 0)
        return Send_pointer_move(movement);

    /*
     * maxMouseTurnsPS is not 0: player wants to limit amount
     * of pointer move packets sent to server.
     */
    cumulativeMouseMovement += movement;

    return 0;
}

/*
 * Check if there is any pointer move we need to send to server.
 * Returns how many microseconds to wait in select().
 */
int Client_check_pointer_move_interval(void)
{
    struct timeval now;
    static int last_send_interval_num = -1;
    int interval_num; /* 0 ... maxMouseTurnsPS - 1 */
    int next_interval_start;

    assert(maxMouseTurnsPS > 0);

    /*
     * Let's see if we've sent any pointer move this interval,
     * if not and there is something to send, do that now.
     */
    gettimeofday(&now, nullptr);
    interval_num = ((int)now.tv_usec) / mouseMovementInterval;
    if (interval_num != last_send_interval_num && cumulativeMouseMovement != 0)
    {
        Send_pointer_move(cumulativeMouseMovement);
        cumulativeMouseMovement = 0;
        last_send_interval_num = interval_num;
    }

    if (cumulativeMouseMovement != 0)
    {
        /* calculate how long to wait to next interval */
        next_interval_start = (interval_num + 1) * mouseMovementInterval;
        return next_interval_start - (int)now.tv_usec;
    }

    return 1000000;
}

/*
 * Exit the entire client.
 */
void Client_exit(int status)
{
    Net_cleanup();
    Client_cleanup();
    exit(status);
}
