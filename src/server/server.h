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

#pragma once

#include <cstdio>
#include <cstdint>

#include "click.h"
#include "setup.h"
#include "shipshape.h"

#include "defaults.h"
#include "map.h"
#include "object.h"
#include "player.h"
// #include "polygon.h"

#include "object.h"
#include "map.h"

#include "option.h"

#include "polygon.h"

typedef struct
{
    char owner[80];
    char host[80];
} server_t;

/*
 * Global data.
 */

#define FPS options.framesPerSecond
#define NumObjs (ObjCount + 0)
#define MAX_SPECTATORS 8

extern object_t *Obj[];
extern long frame_loops;
extern long frame_loops_slow;
extern double frame_time;
extern int spectatorStart;
extern int NumPlayers;
extern int NumSpectators;
extern int NumOperators;
extern int NumPseudoPlayers;
extern int NumQueuedPlayers;
extern int ObjCount;
extern int NumAlliances;
extern int NumRobots;
extern int login_in_progress;
extern char ShutdownReason[];
extern sock_t contactSocket;
extern time_t serverStartTime;
extern server_t Server;
extern char *serverAddr;
extern uint32_t DEF_HAVE, DEF_USED, USED_KILL;
extern uint16_t KILL_OBJ_BITS;
extern int ShutdownServer, ShutdownDelay;
extern long main_loops;
extern int mapRule;
extern bool teamAssign;
extern int tagItPlayerId;
extern bool allowPlayerPasswords;
extern bool game_lock;
extern bool mute_baseless;
extern time_t gameOverTime;
extern double friction;
extern int roundtime;
extern int roundsPlayed;
extern uint32_t KILLING_SHOTS;
extern double timeStep;
extern double timePerFrame;
extern double ecmSizeFactor;
extern double coriolisCosine, coriolisSine;

extern shape_t ball_wire, wormhole_wire, filled_wire;

static inline vector_t World_gravity(world_t *world, clpos_t pos)
{
    return world->gravity[CLICK_TO_BLOCK(pos.cx)][CLICK_TO_BLOCK(pos.cy)];
}

static inline double SHOT_MULT(object_t *obj)
{
    if (Mods_get(obj->mods, ModsNuclear) && Mods_get(obj->mods, ModsCluster))
        return options.nukeClusterDamage;
    return 1.0;
}

#define APPNAME "xpilot-ng-cpp-server"

/*
 * Prototypes for cell.c
 */
void Free_cells(void);
void Alloc_cells(void);
void Cell_init_object(object_t *obj);
void Cell_add_object(object_t *obj);
void Cell_remove_object(object_t *obj);
void Cell_get_objects(clpos_t pos, int r, int max, object_t ***list, int *count);

/*
 * Prototypes for collision.c
 */
void Check_collision(void);
int IsOffensiveItem(enum Item i);
int IsDefensiveItem(enum Item i);
int CountOffensiveItems(player_t *pl);
int CountDefensiveItems(player_t *pl);

/*
 * Prototypes for id.c
 */
int peek_ID(void);
int request_ID(void);
void release_ID(int id);

/*
 * Prototypes for event.c
 */
int Handle_keyboard(player_t *pl);
void Pause_player(player_t *pl, bool on);
int Player_lock_closest(player_t *pl, bool next);
bool team_dead(int team);

/*
 * Prototypes for map.c
 */
int World_init(void);
void World_free(void);
bool Grok_map(void);
bool Grok_map_options(void);

int World_place_base(world_t *world, clpos_t pos, int dir, int team, int order);
int World_place_cannon(world_t *world, clpos_t pos, int dir, int team);
int World_place_check(world_t *world, clpos_t pos, int ind);
int World_place_fuel(world_t *world, clpos_t pos, int team);
int World_place_grav(world_t *world, clpos_t pos, double force, int type);
int World_place_target(world_t *world, clpos_t pos, int team);
int World_place_treasure(world_t *world, clpos_t pos, int team, bool empty, int ball_style);
int World_place_wormhole(world_t *world, clpos_t pos, wormtype_t type);
int World_place_item_concentrator(world_t *world, clpos_t pos);
int World_place_asteroid_concentrator(world_t *world, clpos_t pos);
int World_place_friction_area(world_t *world, clpos_t pos, double fric);

void Wormhole_line_init(void);

void Compute_gravity(void);
double World_wrap_findDir(world_t *world, double dx, double dy);
double World_wrap_cfindDir(world_t *world, int dx, int dy);
double World_wrap_length(world_t *world, int dx, int dy);
int Find_closest_team(clpos_t pos);

/*
 * Prototypes for xpmap.c
 */
void Create_blockmap_from_polygons(void);
setup_t *Xpmap_init_setup(void);
void Xpmap_print(void);
void Xpmap_grok_map_data(void);
void Xpmap_allocate_checks(void);
void Xpmap_tags_to_internal_data(void);
void Xpmap_find_map_object_teams(void);
void Xpmap_find_base_direction(void);
void Xpmap_blocks_to_polygons(void);

/*
 * Prototypes for xp2map.c
 */
bool isXp2MapFile(FILE *ifile);
bool parseXp2MapFile(char *fname, optOrigin opt_origin);

/*
 * Prototypes for cmdline.c
 */
void tuner_none(void);
void tuner_dummy(void);
void Check_playerlimit(void);
void Timing_setup(void);
bool Init_options(void);
void Free_options(void);

/*
 * Prototypes for player.c
 */
void Item_damage(player_t *pl, double prob);

void Add_fuel(pl_fuel_t *ft, double fuel);

static inline void Player_add_fuel(player_t *pl, double amount)
{
    Add_fuel(&(pl->fuel), amount);
}

void Place_item(player_t *pl, int type);
int Choose_random_item(void);
void Tractor_beam(player_t *pl);
void General_tractor_beam(int id, clpos_t pos,
                          int items, player_t *victim, bool pressor);
void Place_mine(player_t *pl);
void Place_moving_mine(player_t *pl);
void Place_general_mine(int id, int team, int status,
                        clpos_t pos, vector_t vel, modifiers_t mods);
void Detonate_mines(player_t *pl);
char *Describe_shot(int type, int status, modifiers_t mods, int hit);
void Fire_ecm(player_t *pl);
void Fire_general_ecm(int id, int team, clpos_t pos);
void Update_connector_force(ballobject_t *ball);
void Fire_shot(player_t *pl, int type, int dir);
void Fire_general_shot(int id, int team, bool cannon,
                       clpos_t pos, int type, int dir,
                       modifiers_t mods, int target_id);
void Fire_normal_shots(player_t *pl);
void Fire_main_shot(player_t *pl, int type, int dir);
void Fire_left_shot(player_t *pl, int type, int dir, int gun);
void Fire_right_shot(player_t *pl, int type, int dir, int gun);
void Fire_left_rshot(player_t *pl, int type, int dir, int gun);
void Fire_right_rshot(player_t *pl, int type, int dir, int gun);

bool Friction_area_hitfunc(group_t *groupptr, const move_t *move);

void Team_immunity_init(void);
void Hitmasks_init(void);

void Delete_shot(int ind);
void Do_deflector(player_t *pl);
void Do_transporter(player_t *pl);
void Do_general_transporter(int id, clpos_t pos,
                            player_t *victim, int *item, double *amount);
void do_lose_item(player_t *pl);
void Update_torpedo(torpobject_t *torp);
void Update_missile(missileobject_t *shot);
void Update_mine(mineobject_t *mine);
void Make_item(clpos_t pos,
               vector_t vel,
               int item, int num_per_pack, int status);
void Throw_items(player_t *pl);
void Detonate_items(player_t *pl);
void add_temp_wormholes(int xin, int yin, int xout, int yout);
void remove_temp_wormhole(int ind);

/*
 * Prototypes for server.cpp
 */
void End_game(void);
int Pick_team(int pick_for_type);
void Server_info(char *str, size_t max_size);
void Log_game(const char *heading);
const char *Describe_game_status(void);
void Game_Over(void);
void Server_shutdown(const char *user_name, int delay, const char *reason);
void Server_log_admin_message(player_t *pl, const char *str);
int plock_server(bool on);
void Main_loop(void);
