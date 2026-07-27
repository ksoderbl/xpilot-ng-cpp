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

#include "player.h"

void Make_thrust_sparks(player_t *pl);
void Record_shove(player_t *pl, player_t *pusher, long shove_time);
void Delta_mv(object_t *ship, object_t *obj);
void Delta_mv_elastic(object_t *obj1, object_t *obj2);
void Delta_mv_partly_elastic(object_t *obj1, object_t *obj2, double elastic);
void Obj_repel(object_t *obj1, object_t *obj2, int repel_dist);
/*void Add_fuel(pl_fuel_t *ft, double fuel);*/
void Update_tanks(pl_fuel_t *ft);
void Tank_handle_detach(player_t *pl);
void Make_debris(clpos_t pos,
                 vector_t vel,
                 int owner_id,
                 int owner_team,
                 int type,
                 double mass,
                 int status,
                 int color,
                 int radius,
                 int num_debris,
                 int min_dir, int max_dir,
                 double min_speed, double max_speed,
                 double min_life, double max_life);
void Make_wreckage(clpos_t pos,
                   vector_t vel,
                   int owner_id,
                   int owner_team,
                   double min_mass, double max_mass,
                   double total_mass,
                   int status,
                   int max_wreckage,
                   int min_dir, int max_dir,
                   double min_speed, double max_speed,
                   double min_life, double max_life);
void Explode_fighter(player_t *pl);
