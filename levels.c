/*
Lacewing
Copyright (C) 2003 Linley Henzell & Captain Pork

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public Licence as published by
    the Free Software Foundation; either version 2 of the Licence, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public Licence for more details.

    You should have received a copy of the GNU General Public Licence
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    The GPL version 2 is included in this distribution in a file called
    LICENCE.TXT. Use any text editor or the TYPE command to read it.

    You should be able to reach us by sending an email to
    captainpork@fastmail.fm.
    
File: levels.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - initialising and managing of the levels

*/

#include "allegro.h"

#include "config.h"
#include "globvars.h"



#include "stuff.h"

#include "palette.h"
#include "enemy.h"
#include "display.h"
#include "cloud.h"
#include "bullet.h"
#include "enemy.h"
#include "pickup.h"
#include "actor.h"
#include "sound.h"

void assemble_enemies(void);
int choose_enemy(int e1, int e2, int e3, int e4);

void init_duel_level(void);
void run_duel_level(void);

void start_at_level_1(void)
{
 arena.level = 0;

}

void init_level(void)
{

 if (game.type == GAME_DUEL)
 {
  init_duel_level();
  return;
 }

 arena.level ++;

 level_announcement();

 init_bullets();
 init_clouds();
 init_enemies();

 calculate_ambience();

 switch((arena.level - 1) / 4)
 {
  default:
  case 0:
   arena.colour1 = COLOUR_GREEN2;
   arena.colour2 = COLOUR_GREEN6;
   arena.colour3 = COLOUR_GREY1;
   break;
  case 1:
   arena.colour1 = COLOUR_YELLOW2;
   arena.colour2 = COLOUR_YELLOW6;
   arena.colour3 = COLOUR_GREY1;
   break;
  case 2:
   arena.colour1 = COLOUR_RED2;
   arena.colour2 = COLOUR_RED6;
   arena.colour3 = COLOUR_GREY1;
   break;
  case 3:
   arena.colour1 = COLOUR_BLUE2;
   arena.colour2 = COLOUR_BLUE6;
   arena.colour3 = COLOUR_GREY1;
   break;

 }

 if (arena.level == 16)
 {
  arena.colour1 = COLOUR_PURPLE2;
  arena.colour2 = COLOUR_PURPLE6;
  arena.colour3 = COLOUR_GREY1;
 }

 switch(arena.level)
 {
  default:
  case 1:
   arena.max_x = 594000 + (((int) (arena.level / 2)) + grand(arena.level / 3)) * 100000;
   arena.max_y = 595000 + (((int) (arena.level / 2)) + grand(arena.level / 3)) * 100000;
   if (arena.level % 4 == 0)
   {
    arena.max_x = 794000;
    arena.max_y = 795000;
   }
   if (arena.max_x > 1200000)
   {
    arena.max_x = 1194000;
   }
   if (arena.max_y > 1200000)
   {
    arena.max_y = 1195000;
   }
   arena.max_targets_in_level = 4 + arena.level / 5;
   assemble_enemies();

/*   arena.target [0] = ENEMY_FIGHTER3;
   arena.target [1] = ENEMY_SUPERJELLY;
//   arena.target [2] = ENEMY_BOUNCER3;
   arena.targets_left [0] = 4;
   arena.targets_left [1] = 5;
   arena.targets_left [2] = 4;
   arena.total_targets = arena.targets_left [0] + arena.targets_left [1] + arena.targets_left [2];
   arena.between_target_upgrades = 3;
*/
   arena.max_crawlies_in_level = 4 + arena.level / 4;
   arena.between_crawly_upgrades = 4 + arena.level / 4;
   break;
/*  case 1:
//   arena.max_x = 294000;
//   arena.max_y = 295000;
//   arena.max_x = 394000;
//   arena.max_y = 395000;
//   arena.max_x = 494000;
//   arena.max_y = 495000;
   arena.max_x = 994000;
   arena.max_y = 995000;
   arena.max_targets_in_level = 4;
   arena.total_targets = 12;
   arena.target [0] = ENEMY_DRIFTER2;
   arena.target [1] = ENEMY_STINGER3;
   arena.target [2] = ENEMY_FIGHTER4;
//   arena.target [0] = ENEMY_FIGHTER;
//   arena.target [1] = ENEMY_FIGHTER2;
//   arena.target [2] = ENEMY_FIGHTER3;
   arena.targets_left [0] = 4;
   arena.targets_left [1] = 5;
   arena.targets_left [2] = 4;
   arena.max_crawlies_in_level = 7;
   arena.between_target_upgrades = 3;
   arena.between_crawly_upgrades = 3;
   break;*/

 }
   arena.time_left = 33.333 * 180;
   if (arena.level < 8)
    arena.time_left = 33.33 * 150;
   if (arena.level < 4)
    arena.time_left = 33.33 * 120;

//   arena.time_left = 600;
//   arena.time_left = 50;
//   arena.time_left = 280;
   arena.level_finished = 0;
   arena.next_target = grand(4) + 3;
//   arena.targets_left_overall = arena.total_targets;
   arena.next_crawly = grand(5) + 1;
   arena.targets_left_total = arena.targets_left [0] + arena.targets_left [1] + arena.targets_left [2];
   arena.next_crawly_upgrade = grand(arena.between_crawly_upgrades) + 1;
   arena.hostile = 0;
   arena.crawlies_created = 0;
   init_pickups();
}


// called about every second
void run_level(void)
{

 if (game.type == GAME_DUEL)
 {
  run_duel_level();
  return;
 }

 int i;
 int current_targets = 0;
 int current_crawlies = 0;
 int target_index = -1, subtype = SUBTYPE_NONE;
 int carrying_pickup = PICKUP_NONE;

 arena.next_target --;
 arena.next_crawly --;

 if (arena.next_target != 0 && arena.next_crawly != 0)
  return;
  // Don't bother placing new enemies if now's not the time

 for (i = 0; i < NO_ENEMIES; i ++)
 {
  if (enemy[i].type == ENEMY_NONE)
   continue;
  if (enemy[i].target == TARGET_PRIMARY)
   current_targets ++;
  if (enemy[i].target == TARGET_CRAWLY)
   current_crawlies ++;
 }


 if (arena.next_target == 0)
 {
  if (arena.targets_left_total > 0 && current_targets < arena.max_targets_in_level)
  {
   if (arena.targets_left [0] > 0)
    target_index = 0;
   if (arena.targets_left [1] > 0 && grand(3) == 0)
    target_index = 1;
   if (arena.targets_left [2] > 0 && grand(4) == 0)
    target_index = 2;
   if (target_index == -1)
   {
    if (arena.targets_left [2] > 0)
     target_index = 2;
    if (arena.targets_left [1] > 0)
     target_index = 1;
   }
   if (target_index != -1)
   {
    arena.next_target_upgrade --;
    if (arena.next_target_upgrade <= 0)
    {
     arena.next_target_upgrade = arena.between_target_upgrades;
     carrying_pickup = PICKUP_UPGRADE1;
    }
     else
     {
      if (grand(6) == 0)
       carrying_pickup = PICKUP_REPAIR;
     }
     if (spawn_enemy(arena.target [target_index], subtype, carrying_pickup, TARGET_PRIMARY) != -1)
      arena.targets_left [target_index] --;
   }
  }
  arena.next_target = 5;
 }

 if (arena.next_crawly == 0)
 {
  if (current_crawlies < arena.max_crawlies_in_level)
  {
   arena.next_crawly_upgrade --;
   if (arena.next_crawly_upgrade == 0)
   {
    arena.next_crawly_upgrade = arena.between_crawly_upgrades;
    carrying_pickup = PICKUP_UPGRADE1;
   }
    else
    {
      if (grand(12) == 0)
      carrying_pickup = PICKUP_REPAIR;
//      if (grand(3) == 0)
//       carrying_pickup = PICKUP_UPGRADE2;
//      if (grand(3) == 0)
//       carrying_pickup = PICKUP_UPGRADE3;
     // other pickup types
    }
   target_index = ENEMY_CRAWLER;
   if (grand(3) == 0 && arena.level > 4)
    target_index = ENEMY_CRAWLER2;
   if (grand(3) == 0 && arena.level > 8)
    target_index = ENEMY_CRAWLER3;
   if (grand(3) == 0)
    target_index = ENEMY_FALLER;
   if (arena.hostile > 0)
    subtype = SUBTYPE_WHITE;
     else
     {
      subtype = SUBTYPE_GREEN;
      subtype += (arena.level + grand(3)) / 4 + grand(arena.crawlies_created) / 4;
      if (subtype > SUBTYPE_PURPLE)
       subtype = SUBTYPE_PURPLE;
    }
   spawn_enemy(target_index, subtype, carrying_pickup, TARGET_CRAWLY);
  }
  arena.next_crawly = 8;
 }


}


void assemble_enemies(void)
{

 switch(arena.level)
 {
  case 1:
   arena.target [0] = ENEMY_STINGER;
   arena.target [1] = ENEMY_BOUNCER4;
   arena.target [2] = ENEMY_FIREBASE;
   arena.targets_left [0] = 8;
   arena.targets_left [1] = 0;
   arena.targets_left [2] = 0;
/*   arena.target [0] = ENEMY_STINGER;
   arena.targets_left [0] = 8;
   arena.targets_left [1] = 0;
   arena.targets_left [2] = 0;*/
//   arena.next_target_upgrade = 1;
   arena.total_targets = arena.targets_left [0] + arena.targets_left [1] + arena.targets_left [2];
   arena.between_target_upgrades = arena.total_targets / 4;
   arena.next_target_upgrade = arena.total_targets % 4;
   return; // NOTE return, not break
  case 4:
   arena.target [0] = ENEMY_BOSS1;
   arena.targets_left [0] = 1;
   arena.targets_left [1] = 0;
   arena.targets_left [2] = 0;
   arena.next_target_upgrade = 1;
   arena.total_targets = 1;
   return; // NOTE return, not break
  case 8:
   arena.target [0] = ENEMY_BOSS2;
   arena.targets_left [0] = 1;
   arena.targets_left [1] = 0;
   arena.targets_left [2] = 0;
   arena.total_targets = 1;
   arena.next_target_upgrade = 1;
   return; // NOTE return, not break
  case 12:
   arena.target [0] = ENEMY_BOSS3;
   arena.targets_left [0] = 1;
   arena.targets_left [1] = 0;
   arena.targets_left [2] = 0;
   arena.total_targets = 1;
   arena.next_target_upgrade = 1;
   return; // NOTE return, not break
  case 16:
   arena.target [0] = ENEMY_BOSS4;
   arena.targets_left [0] = 1;
   arena.targets_left [1] = 0;
   arena.targets_left [2] = 0;
   arena.total_targets = 1;
   arena.next_target_upgrade = 1;
   return; // NOTE return, not break
// Remember: no firebases on boss levels
 }


 int muster [400];

 int i, j, m, k;

 m = 0;

 for (k = 1; k < 3; k ++)
 {
  m = 0;
  for (i = 0; i < NO_ENEMY_TYPES; i ++)
  {
   if (eclass[i].common == 0)
    continue;
//   if (k == 0 && eclass[i].common < 3)
//    continue; // rarer types shouldn't be main enemies on any level
   if (abs(eclass[i].level - (arena.level + k)) < eclass[i].common)
//       && abs(eclass[i].level - (arena.level + k)) > 0)
   {
    for (j = 0; j < eclass[i].common - abs(eclass[i].level - arena.level); j ++)
    {
     muster [m] = i;
     m ++;
    }
   }
  }
//   if (m == 0)
//    exit(k);
   arena.target [k] = muster [grand(m)];
 }

 switch(arena.level)
 {
  case 1:
   arena.target [0] = ENEMY_STINGER;
   break;
  case 2:
   arena.target [0] = choose_enemy(ENEMY_STINGER, ENEMY_STINGER, ENEMY_DRIFTER, ENEMY_DRIFTER);
   break;
  case 3:
   arena.target [0] = choose_enemy(ENEMY_STINGER2, ENEMY_STINGER2, ENEMY_DRIFTER, ENEMY_DRIFTER);
   break;
// No 4
  case 5:
   arena.target [0] = choose_enemy(ENEMY_DRIFTER, ENEMY_STINGER2, ENEMY_JELLY, ENEMY_STINGER2);
   break;
  case 6:
   arena.target [0] = choose_enemy(ENEMY_STINGER2, ENEMY_STINGER2, ENEMY_JELLY, ENEMY_JELLY);
   break;
  case 7:
   arena.target [0] = choose_enemy(ENEMY_DRIFTER2, ENEMY_JELLY, ENEMY_FIGHTER, ENEMY_WANDERER);
   break;
// No 8
  case 9:
   arena.target [0] = choose_enemy(ENEMY_STINGER3, ENEMY_FIGHTER, ENEMY_FIGHTER2, ENEMY_STINGER3);
   break;
  case 10:
   arena.target [0] = choose_enemy(ENEMY_STINGER3, ENEMY_BOLTER, ENEMY_FIGHTER2, ENEMY_STINGER3);
   break;
  case 11:
   arena.target [0] = choose_enemy(ENEMY_BOLTER, ENEMY_STINGER3, ENEMY_FIGHTER3, ENEMY_DRIFTER3);
   break;
// No 12
  case 13:
   arena.target [0] = choose_enemy(ENEMY_STINGER4, ENEMY_FLAKKER, ENEMY_TORPER2, ENEMY_BOLTER2);
   break;
  case 14:
   arena.target [0] = choose_enemy(ENEMY_STINGER4, ENEMY_FLAKKER2, ENEMY_BOLTER2, ENEMY_BLASTER);
   break;
  case 15:
   arena.target [0] = choose_enemy(ENEMY_CURVER, ENEMY_FLAKKER2, ENEMY_FIGHTER3, ENEMY_BLASTER);
   break;
 }

 arena.targets_left [0] = 4;// + arena.level / 8;
/* if (arena.targets_left [0] > 10)
  arena.targets_left [0] = 10;*/
 arena.targets_left [1] = 2 + arena.level / 10;
 arena.targets_left [2] = 2;// + arena.level / 13;

 if (arena.level > 6 && grand(6) == 0)
 {
  arena.targets_left [2] = 0; // will still get same # of upgrades - see below
  i = ENEMY_FIREBASE;
  if (grand(arena.level) > 5 || arena.level > 10)
   i = ENEMY_FIREBASE2;
  if (grand(arena.level) > 7 || arena.level > 13)
   i = ENEMY_FIREBASE3;
  if (grand(arena.level) > 9)
   i = ENEMY_HIVE;
  j = PICKUP_UPGRADE1 + grand(3);
  if (grand(4) == 0)
   j = PICKUP_SHIP;
  spawn_enemy(i, SUBTYPE_NONE, j, TARGET_NO);
 }
 
 arena.total_targets = arena.targets_left [0] + arena.targets_left [1] + arena.targets_left [2];
 arena.between_target_upgrades = arena.total_targets / 4;
 arena.next_target_upgrade = arena.total_targets % 4;

 
}

int choose_enemy(int e1, int e2, int e3, int e4)
{
 switch(grand(4))
 {
  case 0: return e1;
  case 1: return e2;
  case 2: return e3;
  case 3: return e4;
 }

 return e1;

}

void finish_level(void)
{
   arena.level_finished = 1;
   arena.seconds_left_when_finished = 0;
   arena.time_bonus = 0;
   if (arena.hostile == 0)
   {
    arena.seconds_left_when_finished = arena.time_left / 33.3333;
    arena.time_bonus = (arena.seconds_left_when_finished) * arena.level * 5;
    if (game.users == 1)
     gain_score(game.single_player, arena.time_bonus);
      else
      {
       gain_score(0, arena.time_bonus);
       gain_score(1, arena.time_bonus);
      }
   }
   arena.time_left = 166;
   arena.colour1 = COLOUR_GREY2;
   arena.colour2 = COLOUR_GREY6;
   if (arena.hostile > 0)
   {
    arena.hostile = 0;
    arena.colour3 = COLOUR_GREY1;
   }

 play_sound(WAV_LEVEL_END);
}









void init_duel_level(void)
{

 init_bullets();
 init_clouds();
 init_enemies();
 player[0].duel_score = 0;
 player[1].duel_score = 0;

   arena.colour1 = COLOUR_YELLOW2;
   arena.colour2 = COLOUR_YELLOW6;
   arena.colour3 = COLOUR_GREY1;

   arena.max_x = 794000 + grand(6) * 100000;
   arena.max_y = 795000 + grand(6) * 100000;
   if (arena.max_x > 1200000)
   {
    arena.max_x = 1194000;
   }
   if (arena.max_y > 1200000)
   {
    arena.max_y = 1195000;
   }
   switch(game.duel_crawlies)
   {
    case 0:
     arena.max_crawlies_in_level = 0;
     arena.between_crawly_upgrades = 0;
     break;
    case 1:
     arena.max_crawlies_in_level = 3;
     arena.between_crawly_upgrades = 2;
     break;
    case 2:
     arena.max_crawlies_in_level = 6;
     arena.between_crawly_upgrades = 2;
     break;
    case 3:
     arena.max_crawlies_in_level = 9;
     arena.between_crawly_upgrades = 2;
     break;
   }
   arena.time_left = 33.333 * 180;
   if (game.duel_mode == DUEL_10_MINUTES)
    arena.time_left = 33.333 * 600;

   arena.level_finished = 0;
   arena.next_crawly = grand(5) + 1;
   arena.next_crawly_upgrade = grand(arena.between_crawly_upgrades) + 1;
   arena.hostile = 0;
   arena.crawlies_created = 0;
   init_pickups();
}



// called about every second
void run_duel_level(void)
{

 int i;
 int current_crawlies = 0;
 int subtype = SUBTYPE_NONE;
 int carrying_pickup = PICKUP_NONE;
 int target_index = -1;


 arena.next_crawly --;

 if (arena.next_crawly != 0)
  return;

 for (i = 0; i < NO_ENEMIES; i ++)
 {
  if (enemy[i].type == ENEMY_NONE)
   continue;
  if (enemy[i].target == TARGET_CRAWLY)
   current_crawlies ++;
 }


 if (arena.next_crawly == 0)
 {
  if (current_crawlies < arena.max_crawlies_in_level)
  {
   arena.next_crawly_upgrade --;
   if (arena.next_crawly_upgrade == 0)
   {
    arena.next_crawly_upgrade = grand(arena.between_crawly_upgrades) + 1;
    carrying_pickup = PICKUP_UPGRADE1;
   }
    else
    {
      if (grand(12) == 0)
      carrying_pickup = PICKUP_REPAIR;
    }
   target_index = ENEMY_CRAWLER;
   if (grand(3) == 0 && arena.crawlies_created > 16)
    target_index = ENEMY_CRAWLER3;
   if (grand(3) == 0 && arena.crawlies_created > 8)
    target_index = ENEMY_CRAWLER2;
   if (grand(3) == 0)
    target_index = ENEMY_FALLER;
    
   subtype = SUBTYPE_GREEN;
   subtype += grand(arena.crawlies_created) / 8;
   if (subtype > SUBTYPE_PURPLE)
    subtype = SUBTYPE_PURPLE;
   spawn_enemy(target_index, subtype, carrying_pickup, TARGET_CRAWLY);
  }
  switch(game.duel_crawlies)
  {
   case 0:
    arena.next_crawly = 0;
    break;
   case 1:
    arena.next_crawly = 24;
    break;
   case 2:
    arena.next_crawly = 12;
    break;
   case 3:
    arena.next_crawly = 6;
    break;
  }
 }


}



