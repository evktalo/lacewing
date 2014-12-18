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
    
File: enemy.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - most functions relating to enemies
This is a really badly written file. There's no excuse at all for large
parts of it, in particular all of those move_<whatever> functions. I'm
reasonably pleased with most of the Lacewing code (at least, compared to
some of my earlier crimes against C), but for some reason I can't be
bothered fixing up this tangled, redundancy-filled mess. Oh well.

*/

#include "allegro.h"

#include <math.h>

#include "config.h"
#include "globvars.h"

#include "stuff.h"
#include "cloud.h"
#include "palette.h"
#include "bullet.h"
#include "pickup.h"
#include "sound.h"
#include "actor.h"
#include "levels.h"

#define HIVE_MIN_RANGE 200000
#define HIVE_MAX_RANGE 500000

/*
NOTE:

Firebases & Hives should always be enemy[0], so that they never overwrite
another enemy on the screen

*/


// int add_enemy - adds an appropriate enemy

int create_enemy(int type, int subtype, int x, int y,
 int x_speed, int y_speed, int angle, int carrying_pickup, int special [10],
 int target);

int create_turret(int type, int base, int turret_counter);

void manage_enemy(int e);
int move_enemy(int e);
int detect_collision(int e);//, int things [2]);
void enemy_explodes(int e);
void register_destroyed(int e, int cause);

void destroy_enemy(int e);
int move_crawler(int e);
int move_faller(int e);
int move_bouncers(int e);
int move_stinger(int e);
int move_stinger2(int e);
int move_stinger3(int e);
int move_stinger4(int e);
int move_hurler(int e);
int move_hurler2(int e);
int move_hurler3(int e);
int move_swerver(int e);
int move_swerver3(int e);
int move_drifter(int e);
int move_hive(int e);
int move_bee(int e);
int move_curver(int e);
int move_wanderer(int e);
int move_minethrower(int e);
int move_minethrower2(int e);
int move_dead_wanderer(int e);
int move_dead_fighter(int e);
int move_jelly(int e);
int move_cloaker(int e);
int move_superjelly(int e);
int move_flakker(int e);
int move_bolter(int e);
int move_torper(int e);
int move_fighter(int e);
int move_fighter4(int e);
int move_fighter4_turret(int e);
int move_torper2(int e);
int move_firebase(int e);
int move_firebase_turret(int e);
int move_firebase2(int e);
int move_firebase2_turret1(int e);
int move_firebase2_turret2(int e);
int move_firebase3(int e);
int move_firebase3_turret(int e);
int move_blaster_turret(int e);
int move_blaster(int e);
int move_boss1(int e);
int move_boss1_turret(int e); // turret1 & 2
int move_boss2(int e);
int move_boss2_turret(int e);
int move_boss3(int e);
int move_boss3_turret(int e);
int move_boss4(int e);
int move_boss4_turret(int e);
int metamorphosis(int e);

//int turn_towards_angle(int angle, int tangle, int turning);
//int turn_towards_xy(int x1, int y1, int x2, int y2, int angle, int turning);
void accelerate_enemy(int e, int angle, int impulse);
void drag_enemy(int e, float drag_amount);
int track_target(int e, int attacking, int turning);
void standard_attack(int e, int angle);
int near_angle(int angle1, int angle2);

void find_firebase_start_point(int location [2]);
void find_crawly_start_point(int location [2]);
void find_target_start_point(int location [2]);
int spawn_enemy(int type, int subtype, int carrying_pickup, int target);
void move_sun(int e);
int hurt_enemy(int e, int hurty, int source, char pulse);
void score_kill(int etype, int subtype, int p);
int check_for_targets(void);


enum
{
ESOUND_NONE,
ESOUND_EXPLODE1,
ESOUND_EXPLODE2,
ESOUND_WOBBLE,
ESOUND_LONG_WOBBLE,
ESOUND_WARP_IN,
ESOUND_STING,
ESOUND_HARD_ZAP,
ESOUND_BOSS2,
ESOUND_BLORT,
ESOUND_STING4,
ESOUND_BLAST,
ESOUND_ZAPNOTE1,
ESOUND_ZAPNOTE2,
ESOUND_MINE,
ESOUND_SHORTZAP,
ESOUND_SHORTZAP2,
ESOUND_BLAT,
ESOUND_SUN,
ESOUND_BOSS1,
ESOUND_CROAK,
ESOUND_GREENSEEKER1,
ESOUND_GREENSEEKER2,
ESOUND_TORPEDO,
ESOUND_BOSS3_1,
ESOUND_BOSS3_2,
ESOUND_BOSS3_3,
ESOUND_PLASMA,
ESOUND_SEEKMINE,
ESOUND_ORBITAL,
ESOUND_MISSILE,
ESOUND_MINEBANG,
ESOUND_MUTATE

};

void enemy_sound(int e, int sound);
void enemy_soundf(int e, int sound, int freq);

void init_enemies(void)
{

 int e;

 for (e = 0; e < NO_ENEMIES; e++)
 {
  enemy[e].type = ENEMY_NONE;
  enemy[e].target = TARGET_NO;
 }

}


int create_enemy(int type, int subtype, int x, int y,
 int x_speed, int y_speed, int angle, int carrying_pickup, int special [10],
 int target)
{

   int e = 0;

   for (e = 0; e < NO_ENEMIES; e++)
   {
    if (e == NO_ENEMIES - 1) return -1;
    if (enemy[e].type == ENEMY_NONE) break;
   }

   enemy[e].type = type;
   enemy[e].x = x;
   enemy[e].y = y;
   enemy[e].x_speed = x_speed;
   enemy[e].y_speed = y_speed;
   enemy[e].angle = grand(ANGLE_FULL); //angle;
   enemy[e].moving_angle = angle;
   enemy[e].armour = eclass[type].max_armour;
   enemy[e].hurt_pulse = 0;
   enemy[e].radius = eclass[type].radius;
   enemy[e].edge_radius = eclass[type].edge_radius;
   enemy[e].shield = eclass[type].max_shield;
   enemy[e].mass = eclass[type].mass;
   enemy[e].drag_amount = 0;
   enemy[e].subtype = subtype;
   enemy[e].attacking = ATTACK_NONE;
   enemy[e].counter = 256 - counter;
   enemy[e].recycle = 50;
//   enemy[e].attacking = 0;
   enemy[e].just_collided = 0;
   enemy[e].carrying_pickup = carrying_pickup;
   enemy[e].target = target;
   enemy[e].burst_fire = 0;

// REMEMBER: enemies are also created in create_turret()

   switch(subtype)
   {
    case SUBTYPE_NONE:
     switch(type)
     {
      default:
      case ENEMY_STINGER:
      case ENEMY_BOSS1:
       enemy[e].colours [0] = COLOUR_GREEN6;
       enemy[e].colours [1] = COLOUR_GREEN1;
       break;
      case ENEMY_MINETHROWER:
      case ENEMY_SUN:
      case ENEMY_FIGHTER3:
      case ENEMY_DEAD_FIGHTER3:
      case ENEMY_FIREBASE2:
      case ENEMY_DRIFTER2:
      case ENEMY_FLAKKER:
      case ENEMY_FLAKKER2:
      case ENEMY_BOLTER:
      case ENEMY_HURLER3:
       enemy[e].colours [0] = COLOUR_RED6;
       enemy[e].colours [1] = COLOUR_RED1;
       break;
      case ENEMY_SUN_GREEN:
      case ENEMY_FIGHTER:
      case ENEMY_DEAD_FIGHTER:
       enemy[e].colours [0] = COLOUR_GREEN6;
       enemy[e].colours [1] = COLOUR_GREEN1;
       break;
      case ENEMY_MINETHROWER2:
      case ENEMY_SUN_BLUE:
      case ENEMY_CURVER:
      case ENEMY_STINGER3:
      case ENEMY_FIGHTER4:
      case ENEMY_BOLTER2:
      case ENEMY_BOSS3:
      case ENEMY_BOSS3_TURRET1:
      case ENEMY_BOSS3_TURRET2:
      case ENEMY_TORPER2:
       enemy[e].colours [0] = COLOUR_BLUE6;
       enemy[e].colours [1] = COLOUR_BLUE2;
       break;
      case ENEMY_WANDERER:
      case ENEMY_DRIFTER:
      case ENEMY_JELLY:
      case ENEMY_STINGER2:
      case ENEMY_FIREBASE:
      case ENEMY_BEE:
      case ENEMY_HIVE:
      case ENEMY_TORPER:
      case ENEMY_HURLER:
       enemy[e].colours [0] = COLOUR_YELLOW6;
       enemy[e].colours [1] = COLOUR_YELLOW1;
       break;
      case ENEMY_CLOAKER:
       enemy[e].colours [0] = COLOUR_YELLOW1;
       enemy[e].colours [1] = COLOUR_YELLOW1;
       break;
      case ENEMY_CLOAKER2:
       enemy[e].colours [0] = COLOUR_PURPLE1;
       enemy[e].colours [1] = COLOUR_PURPLE1;
       break;
      case ENEMY_STINGER4:
       enemy[e].colours [0] = COLOUR_PURPLE6;
       enemy[e].colours [1] = COLOUR_PURPLE1;
       break;
      case ENEMY_BLASTER:
      case ENEMY_FIREBASE3:
      case ENEMY_FIREBASE3_TURRET:
      case ENEMY_SUPERJELLY:
      case ENEMY_BOSS4:
      case ENEMY_BOSS4_TURRET:
       enemy[e].colours [0] = COLOUR_GREY6;
       enemy[e].colours [1] = COLOUR_GREY2;
       break;
/*      case ENEMY_DEAD_FIGHTER:
       enemy[e].colours [0] = COLOUR_GREY4;
       enemy[e].colours [1] = COLOUR_GREY1;
       break;*/
      case ENEMY_FIGHTER2:
      case ENEMY_DEAD_FIGHTER2:
      case ENEMY_BOSS2:
      case ENEMY_BOSS2_TURRET:
      case ENEMY_HURLER2:
       enemy[e].colours [0] = COLOUR_ORANGE6;
       enemy[e].colours [1] = COLOUR_ORANGE1;
       break;
      case ENEMY_DRIFTER3:
       enemy[e].colours [0] = COLOUR_PURPLE6;
       enemy[e].colours [1] = COLOUR_PURPLE1;
       break;
     }
     break;
    case SUBTYPE_GREEN:
     enemy[e].colours [0] = COLOUR_GREEN6;
     enemy[e].colours [1] = COLOUR_GREEN1;
     break;
    case SUBTYPE_YELLOW:
     enemy[e].colours [0] = COLOUR_YELLOW6;
     enemy[e].colours [1] = COLOUR_YELLOW1;
     enemy[e].armour *= 15; enemy[e].armour /= 10;
     break;
    case SUBTYPE_ORANGE:
     enemy[e].colours [0] = COLOUR_ORANGE6;
     enemy[e].colours [1] = COLOUR_ORANGE1;
     enemy[e].armour *= 25; enemy[e].armour /= 10;
     break;
    case SUBTYPE_RED:
     enemy[e].colours [0] = COLOUR_RED6;
     enemy[e].colours [1] = COLOUR_RED1;
     enemy[e].armour *= 30; enemy[e].armour /= 10;
     break;
    case SUBTYPE_BLUE:
     enemy[e].colours [0] = COLOUR_BLUE6;
     enemy[e].colours [1] = COLOUR_BLUE1;
     enemy[e].armour *= 35; enemy[e].armour /= 10;
     break;
    case SUBTYPE_PURPLE:
     enemy[e].colours [0] = COLOUR_PURPLE6;
     enemy[e].colours [1] = COLOUR_PURPLE1;
     enemy[e].armour *= 40; enemy[e].armour /= 10;
     break;
    case SUBTYPE_WHITE:
     enemy[e].colours [0] = COLOUR_GREY6;
     enemy[e].colours [1] = COLOUR_GREY2;
     enemy[e].armour *= 50; enemy[e].armour /= 10;
     break;
   }

   enemy[e].hurt_pulse_colour1 = enemy[e].colours [0];
   enemy[e].hurt_pulse_colour2 = enemy[e].colours [1];

   int i, j;

   for (i = 0; i < NO_ENEMY_ATTRIBUTES; i ++)
   {
    enemy[e].attribute [i] = eclass[type].default_attributes [i];
   }

   
   switch(enemy[e].type)
   {
    case ENEMY_CRAWLER:
    case ENEMY_CRAWLER2:
    case ENEMY_CRAWLER3:
    case ENEMY_CRAWLER4:
     enemy[e].attribute [ATTRIB_CRAWLER_AXIS] = 1;
     enemy[e].attribute [ATTRIB_CRAWLER_SPEED] = 805 + subtype * 250;
      // crawler speed shouldn't be a multiple of GRAIN
     if (enemy[e].attribute [ATTRIB_CRAWLER_SPEED] % GRAIN == 0)
      enemy[e].attribute [ATTRIB_CRAWLER_SPEED] += 5;
     break;
    case ENEMY_FALLER:
     enemy[e].attribute [ATTRIB_FALLER_SPEED] = 1105 + subtype * 200;
//     enemy[e].attribute [ATTRIB_FALLER_SECTIONS] = 4; //grand(4);
     enemy[e].attribute [ATTRIB_FALLER_SECTIONS] = grand(arena.level / 2); //grand(4);
     if (game.type == GAME_DUEL)
      enemy[e].attribute [ATTRIB_FALLER_SECTIONS] = grand(arena.crawlies_created) / 5;
     if (enemy[e].attribute [ATTRIB_FALLER_SECTIONS] > 4)
      enemy[e].attribute [ATTRIB_FALLER_SECTIONS] = 4;
     enemy[e].attribute [ATTRIB_FALLER_ARMOUR] = 300 + subtype * 150;
     break;
    case ENEMY_SWERVER:
     enemy[e].attribute [ATTRIB_SWERVER_ANGLE] = ANGLE_QUARTER;  // MUST be divisible into ANGLE_FULL
     enemy[e].attribute [ATTRIB_SWERVER_TURN] = 16; // MUST be divisible into ANGLE_FULL AND whatever SWERVER_ANGLE defaults to
     enemy[e].attribute [ATTRIB_SWERVER_SPEED] = 1600 + subtype * 350;
     break;
    case ENEMY_SWERVER2:
     enemy[e].attribute [ATTRIB_SWERVER_ANGLE] = ANGLE_QUARTER;  // MUST be divisible into ANGLE_FULL
     enemy[e].attribute [ATTRIB_SWERVER_TURN] = 16; // MUST be divisible into ANGLE_FULL AND whatever SWERVER_ANGLE defaults to
     enemy[e].attribute [ATTRIB_SWERVER_SPEED] = 1800 + subtype * 350;
     break;
    case ENEMY_SWERVER3:
     enemy[e].attribute [ATTRIB_SWERVER_ANGLE] = ANGLE_QUARTER;  // MUST be divisible into ANGLE_FULL
     enemy[e].attribute [ATTRIB_SWERVER_TURN] = 16; // MUST be divisible into ANGLE_FULL AND whatever SWERVER_ANGLE defaults to
     enemy[e].attribute [ATTRIB_SWERVER_SPEED] = 1800 + subtype * 350;
     enemy[e].attribute [ATTRIB_SWERVER3_ANGLE2] = 0;
     enemy[e].attribute [ATTRIB_SWERVER3_ATTACKING] = ATTACK_NONE;
     break;
    case ENEMY_SUN:
     enemy[e].hurt_pulse_colour1 = COLOUR_YELLOW6;
     enemy[e].hurt_pulse_colour2 = COLOUR_YELLOW3;
     enemy[e].attribute [ATTRIB_SUN_COUNTER] = 6000;
     enemy[e].attribute [ATTRIB_SUN_COUNT_SPEED] = 0;
     enemy[e].attribute [ATTRIB_SUN_COUNT_START] = 6000;
     break;
    case ENEMY_SUN_GREEN:
     enemy[e].hurt_pulse_colour1 = COLOUR_YELLOW6;
     enemy[e].hurt_pulse_colour2 = COLOUR_YELLOW3;
     enemy[e].attribute [ATTRIB_SUN_COUNTER] = 5000;
     enemy[e].attribute [ATTRIB_SUN_COUNT_SPEED] = 0;
     enemy[e].attribute [ATTRIB_SUN_COUNT_START] = 5000;
     break;
    case ENEMY_SUN_BLUE:
     enemy[e].hurt_pulse_colour1 = COLOUR_GREY6;
     enemy[e].hurt_pulse_colour2 = COLOUR_GREY3;
     enemy[e].attribute [ATTRIB_SUN_COUNTER] = 4000;
     enemy[e].attribute [ATTRIB_SUN_COUNT_SPEED] = 0;
     enemy[e].attribute [ATTRIB_SUN_COUNT_START] = 4000;
     break;
    case ENEMY_WANDERER:
     enemy[e].attribute [ATTRIB_WANDERER_NEXT_MINE] = 250;
     enemy[e].attribute [ATTRIB_WANDERER_X_DEST] = grand(arena.max_x - 40000) + 20000;
     enemy[e].attribute [ATTRIB_WANDERER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
     enemy[e].burst_fire = 3 + grand(4);
     enemy[e].angle = grand(ANGLE_FULL);
     break;
    case ENEMY_FIGHTER:
     enemy[e].attribute [ATTRIB_FIGHTER_THRUST] = 100;
     enemy[e].attribute [ATTRIB_FIGHTER_MODE] = FIGHTER_MOVE;
     enemy[e].attribute [ATTRIB_FIGHTER_TURN] = 16;
     enemy[e].attribute [ATTRIB_FIGHTER_X_DEST] = grand(arena.max_x - 40000) + 20000;
     enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
     enemy[e].burst_fire = 0;
     enemy[e].angle = grand(ANGLE_FULL);
     break;
    case ENEMY_FIGHTER2:
     enemy[e].attribute [ATTRIB_FIGHTER_THRUST] = 150;
     enemy[e].attribute [ATTRIB_FIGHTER_MODE] = FIGHTER_MOVE;
     enemy[e].attribute [ATTRIB_FIGHTER_TURN] = 20;
     enemy[e].attribute [ATTRIB_FIGHTER_X_DEST] = grand(arena.max_x - 40000) + 20000;
     enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
     enemy[e].burst_fire = 3 + grand(4);
     enemy[e].angle = grand(ANGLE_FULL);
     break;
    case ENEMY_FIGHTER3:
     enemy[e].attribute [ATTRIB_FIGHTER_THRUST] = 200;
     enemy[e].attribute [ATTRIB_FIGHTER_MODE] = FIGHTER_MOVE;
     enemy[e].attribute [ATTRIB_FIGHTER_TURN] = 24;
     enemy[e].attribute [ATTRIB_FIGHTER_X_DEST] = grand(arena.max_x - 40000) + 20000;
     enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
     enemy[e].burst_fire = 0;
     enemy[e].angle = grand(ANGLE_FULL);
     break;
    case ENEMY_BLASTER:
     j = create_turret(ENEMY_BLASTER_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BLASTER_TURRET1] = j;
     if (j != -1) // sanity check
     {
      enemy[j].x = enemy[e].x - 15*GRAIN;
      enemy[j].y = enemy[e].y - 10*GRAIN;
     }
     j = create_turret(ENEMY_BLASTER_TURRET, e, 256 - counter - 50);
     enemy[e].attribute [ATTRIB_BLASTER_TURRET2] = j;
     if (j != -1) // sanity check
     {
      enemy[j].x = enemy[e].x - 15*GRAIN;
      enemy[j].y = enemy[e].y - 10*GRAIN;
     }
     enemy[e].angle = grand(ANGLE_FULL);
     break;
    case ENEMY_FIGHTER4:
     enemy[e].attribute [ATTRIB_FIGHTER_THRUST] = 80;
     enemy[e].attribute [ATTRIB_FIGHTER_MODE] = FIGHTER_MOVE;
     enemy[e].attribute [ATTRIB_FIGHTER_TURN] = 8;
     enemy[e].attribute [ATTRIB_FIGHTER_X_DEST] = grand(arena.max_x - 40000) + 20000;
     enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
     j = create_turret(ENEMY_FIGHTER4_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_FIGHTER4_TURRET1] = j;
     if (j != -1) // sanity check
     {
      enemy[j].x = enemy[e].x - 15*GRAIN;
      enemy[j].y = enemy[e].y - 10*GRAIN;
     }
     j = create_turret(ENEMY_FIGHTER4_TURRET, e, 256 - counter - 50);
     enemy[e].attribute [ATTRIB_FIGHTER4_TURRET2] = j;
     if (j != -1) // sanity check
     {
      enemy[j].x = enemy[e].x - 15*GRAIN;
      enemy[j].y = enemy[e].y - 10*GRAIN;
     }
     enemy[e].angle = grand(ANGLE_FULL);
     break;
    case ENEMY_FIREBASE:
     j = create_turret(ENEMY_FIREBASE_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_FIREBASE_TURRET1] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x - 51*GRAIN;
      enemy[j].y = enemy[e].y - 0*GRAIN;
     }
     j = create_turret(ENEMY_FIREBASE_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_FIREBASE_TURRET2] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x + 51*GRAIN;
      enemy[j].y = enemy[e].y - 0*GRAIN;
     }
     j = create_turret(ENEMY_FIREBASE_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_FIREBASE_TURRET3] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x - 0*GRAIN;
      enemy[j].y = enemy[e].y - 51*GRAIN;
     }
     j = create_turret(ENEMY_FIREBASE_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_FIREBASE_TURRET4] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x - 0*GRAIN;
      enemy[j].y = enemy[e].y + 51*GRAIN;
     }
     enemy[e].angle = grand(ANGLE_FULL);
     break;
    case ENEMY_BOSS1:
     j = create_turret(ENEMY_BOSS1_TURRET2, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS1_TURRET3] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x - 35*GRAIN;
      enemy[j].y = enemy[e].y - 35*GRAIN;
     }
/*     j = create_turret(ENEMY_BOSS1_TURRET1, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS1_TURRET4] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x + 35*GRAIN;
      enemy[j].y = enemy[e].y - 35*GRAIN;
     }*/
     j = create_turret(ENEMY_BOSS1_TURRET2, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS1_TURRET1] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x - 51*GRAIN;
      enemy[j].y = enemy[e].y + 50*GRAIN;
     }
     j = create_turret(ENEMY_BOSS1_TURRET2, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS1_TURRET2] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x + 51*GRAIN;
      enemy[j].y = enemy[e].y + 50*GRAIN;
     }
     enemy[e].angle = grand(ANGLE_FULL);
     enemy[e].attribute [ATTRIB_BOSS1_X_DEST] = grand(arena.max_x - 300000) + 150000;
     enemy[e].attribute [ATTRIB_BOSS1_Y_DEST] = grand(arena.max_y - 300000) + 150000;
     break;
    case ENEMY_BOSS2:
     j = create_turret(ENEMY_BOSS2_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS2_TURRET1] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x - 51*GRAIN;
      enemy[j].y = enemy[e].y + 50*GRAIN;
     }
     j = create_turret(ENEMY_BOSS2_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS2_TURRET2] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x + 51*GRAIN;
      enemy[j].y = enemy[e].y + 50*GRAIN;
     }
     j = create_turret(ENEMY_BOSS2_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS2_TURRET3] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x - 35*GRAIN;
      enemy[j].y = enemy[e].y - 35*GRAIN;
     }
     j = create_turret(ENEMY_BOSS2_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS2_TURRET4] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x + 35*GRAIN;
      enemy[j].y = enemy[e].y - 35*GRAIN;
     }
     enemy[e].angle = grand(ANGLE_FULL);
     enemy[e].attribute [ATTRIB_BOSS2_DELTA_SPIN] = 1;
     enemy[e].attribute [ATTRIB_BOSS2_TURRETS_LEFT] = 4;
     enemy[e].attribute [ATTRIB_BOSS2_X_DEST] = grand(arena.max_x - 300000) + 150000;
     enemy[e].attribute [ATTRIB_BOSS2_Y_DEST] = grand(arena.max_y - 300000) + 150000;
     break;
    case ENEMY_BOSS3:
     j = create_turret(ENEMY_BOSS3_TURRET1, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS3_TURRET1] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x - 51*GRAIN;
      enemy[j].y = enemy[e].y + 50*GRAIN;
     }
     j = create_turret(ENEMY_BOSS3_TURRET1, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS3_TURRET2] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x + 51*GRAIN;
      enemy[j].y = enemy[e].y + 50*GRAIN;
     }
     j = create_turret(ENEMY_BOSS3_TURRET1, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS3_TURRET3] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x - 35*GRAIN;
      enemy[j].y = enemy[e].y - 35*GRAIN;
     }
     j = create_turret(ENEMY_BOSS3_TURRET2, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS3_TURRET4] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x + 35*GRAIN;
      enemy[j].y = enemy[e].y - 35*GRAIN;
     }
     j = create_turret(ENEMY_BOSS3_TURRET2, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS3_TURRET5] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x + 35*GRAIN;
      enemy[j].y = enemy[e].y - 35*GRAIN;
     }
     enemy[e].angle = grand(ANGLE_FULL);
//     enemy[e].attribute [ATTRIB_BOSS2_DELTA_SPIN] = 1;
//     enemy[e].attribute [ATTRIB_BOSS2_TURRETS_LEFT] = 4;
//     enemy[e].attribute [ATTRIB_BOSS2_X_DEST] = grand(arena.max_x - 300000) + 150000;
//     enemy[e].attribute [ATTRIB_BOSS2_Y_DEST] = grand(arena.max_y - 300000) + 150000;
     break;
    case ENEMY_BOSS4:
     j = create_turret(ENEMY_BOSS4_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS4_TURRET1] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x - 51*GRAIN;
      enemy[j].y = enemy[e].y + 50*GRAIN;
     }
     j = create_turret(ENEMY_BOSS4_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS4_TURRET2] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x + 51*GRAIN;
      enemy[j].y = enemy[e].y + 50*GRAIN;
     }
     j = create_turret(ENEMY_BOSS4_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS4_TURRET3] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x - 35*GRAIN;
      enemy[j].y = enemy[e].y - 35*GRAIN;
     }
     j = create_turret(ENEMY_BOSS4_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_BOSS4_TURRET4] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x + 35*GRAIN;
      enemy[j].y = enemy[e].y - 35*GRAIN;
     }
     enemy[e].angle = grand(ANGLE_FULL);
     enemy[e].attribute [ATTRIB_BOSS4_X_DEST] = grand(arena.max_x - 300000) + 150000;
     enemy[e].attribute [ATTRIB_BOSS4_Y_DEST] = grand(arena.max_y - 300000) + 150000;
//     enemy[e].attribute [ATTRIB_BOSS2_DELTA_SPIN] = 1;
//     enemy[e].attribute [ATTRIB_BOSS2_TURRETS_LEFT] = 4;
//     enemy[e].attribute [ATTRIB_BOSS2_X_DEST] = grand(arena.max_x - 300000) + 150000;
//     enemy[e].attribute [ATTRIB_BOSS2_Y_DEST] = grand(arena.max_y - 300000) + 150000;
     break;
    case ENEMY_FIREBASE3:
     j = create_turret(ENEMY_FIREBASE3_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_FIREBASE3_TURRET1] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x - 35*GRAIN;
      enemy[j].y = enemy[e].y - 36*GRAIN;
     }
     j = create_turret(ENEMY_FIREBASE3_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_FIREBASE3_TURRET2] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x + 35*GRAIN;
      enemy[j].y = enemy[e].y - 36*GRAIN;
     }
     j = create_turret(ENEMY_FIREBASE3_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_FIREBASE3_TURRET3] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x + 35*GRAIN;
      enemy[j].y = enemy[e].y + 36*GRAIN;
     }
     j = create_turret(ENEMY_FIREBASE3_TURRET, e, 256 - counter);
     enemy[e].attribute [ATTRIB_FIREBASE3_TURRET4] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x - 35*GRAIN;
      enemy[j].y = enemy[e].y + 36*GRAIN;
     }
     enemy[e].angle = grand(ANGLE_FULL);
     break;
    case ENEMY_FIREBASE2:
     j = create_turret(ENEMY_FIREBASE2_TURRET1, e, 256 - counter);
     enemy[e].attribute [ATTRIB_FIREBASE2_TURRET1] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x - 0*GRAIN;
      enemy[j].y = enemy[e].y - 25*GRAIN;
     }
     j = create_turret(ENEMY_FIREBASE2_TURRET2, e, 256 - counter);
     enemy[e].attribute [ATTRIB_FIREBASE2_TURRET2] = j;
     if (j != -1)
     {
      enemy[j].x = enemy[e].x + 0*GRAIN;
      enemy[j].y = enemy[e].y + 25*GRAIN;
     }
     enemy[e].angle = grand(ANGLE_FULL);
     break;
    case ENEMY_DRIFTER:
     enemy[e].attribute [ATTRIB_DRIFTER_RANGE] = 100000;
     enemy[e].attribute [ATTRIB_DRIFTER_ACCEL] = 180;
     enemy[e].angle = grand(ANGLE_FULL);
     enemy[e].attribute [ATTRIB_DRIFTER_X_DEST] = grand(arena.max_x - 300000) + 150000;
     enemy[e].attribute [ATTRIB_DRIFTER_Y_DEST] = grand(arena.max_y - 300000) + 150000;
     break;
    case ENEMY_HIVE:
     enemy[e].attribute [ATTRIB_HIVE_ATTACK13] = ATTACK_NONE;
     enemy[e].attribute [ATTRIB_HIVE_ATTACK24] = ATTACK_NONE;
/*     enemy[e].attribute [ATTRIB_HIVE_BEE1] = -1;
     enemy[e].attribute [ATTRIB_HIVE_BEE2] = -1;
     enemy[e].attribute [ATTRIB_HIVE_BEE3] = -1;
     enemy[e].attribute [ATTRIB_HIVE_BEE4] = -1;
     enemy[e].angle = grand(ANGLE_FULL);*/
     break;
    case ENEMY_BEE:
     enemy[e].attribute [ATTRIB_BEE_HIVE] = special [0];
     enemy[e].attribute [ATTRIB_BEE_MODE] = BEE_RESTING;
     enemy[e].attribute [ATTRIB_BEE_NUMBER] = special [1];
     enemy[e].attribute [ATTRIB_BEE_RANGE] = 200;
     break;
    case ENEMY_DRIFTER2:
     enemy[e].attribute [ATTRIB_DRIFTER_RANGE] = 125000;
     enemy[e].attribute [ATTRIB_DRIFTER_ACCEL] = 185;
     enemy[e].angle = grand(ANGLE_FULL);
     enemy[e].attribute [ATTRIB_DRIFTER_X_DEST] = grand(arena.max_x - 300000) + 150000;
     enemy[e].attribute [ATTRIB_DRIFTER_Y_DEST] = grand(arena.max_y - 300000) + 150000;
     break;
    case ENEMY_DRIFTER3:
     enemy[e].attribute [ATTRIB_DRIFTER_RANGE] = 150000;
     enemy[e].attribute [ATTRIB_DRIFTER_ACCEL] = 190;
     enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL_DIR] = 32;
     if (grand(2) == 0)
      enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL_DIR] = -32;
     enemy[e].angle = grand(ANGLE_FULL);
     enemy[e].attribute [ATTRIB_DRIFTER_X_DEST] = grand(arena.max_x - 300000) + 150000;
     enemy[e].attribute [ATTRIB_DRIFTER_Y_DEST] = grand(arena.max_y - 300000) + 150000;
     break;
    case ENEMY_CLOAKER:
     enemy[e].attribute [ATTRIB_CLOAKER_ACCEL] = 150;
     enemy[e].angle = grand(ANGLE_FULL);
     enemy[e].attribute [ATTRIB_CLOAKER_X_DEST] = grand(arena.max_x - 300000) + 150000;
     enemy[e].attribute [ATTRIB_CLOAKER_Y_DEST] = grand(arena.max_y - 300000) + 150000;
     break;
    case ENEMY_CLOAKER2:
     enemy[e].attribute [ATTRIB_CLOAKER_ACCEL] = 210;
     enemy[e].angle = grand(ANGLE_FULL);
     enemy[e].attribute [ATTRIB_CLOAKER_X_DEST] = grand(arena.max_x - 300000) + 150000;
     enemy[e].attribute [ATTRIB_CLOAKER_Y_DEST] = grand(arena.max_y - 300000) + 150000;
     break;
    case ENEMY_BOLTER:
    case ENEMY_BOLTER2:
     enemy[e].attribute [ATTRIB_BOLTER_SPIN] = 1;
     enemy[e].attribute [ATTRIB_BOLTER_DELTA_SPIN] = 1;
     break;
    case ENEMY_TORPER2:
     enemy[e].attribute [ATTRIB_TORPER2_X_DEST] = grand(arena.max_x - 300000) + 150000;
     enemy[e].attribute [ATTRIB_TORPER2_Y_DEST] = grand(arena.max_y - 300000) + 150000;
     break;
    default:
//     enemy[e].angle = grand(ANGLE_FULL);
     break;
/*    case ENEMY_BOUNCER2:
    case ENEMY_BOUNCER3:
    case ENEMY_BOUNCER4:
     break;*/
   }
   
   return e;

}

int create_turret(int type, int base, int turret_counter)
{

   int e = 0;

   for (e = 0; e < NO_ENEMIES; e++)
   {
    if (e == NO_ENEMIES - 1) return -1;
    if (enemy[e].type == ENEMY_NONE) break;
   }

   enemy[e].type = type;
   enemy[e].x = enemy[base].x;
   enemy[e].y = enemy[base].y;
   enemy[e].x_speed = enemy[base].x_speed;
   enemy[e].y_speed = enemy[base].y_speed;
   enemy[e].angle = enemy[base].angle;
   enemy[e].moving_angle = enemy[base].angle;
   enemy[e].armour = eclass[type].max_armour;
   enemy[e].hurt_pulse = 0;
   enemy[e].radius = eclass[type].radius;
   enemy[e].edge_radius = eclass[type].edge_radius;
   enemy[e].shield = eclass[type].max_shield;
   enemy[e].mass = eclass[type].mass;
   enemy[e].drag_amount = 0;
   enemy[e].subtype = 0;
   enemy[e].attacking = ATTACK_NONE;
   enemy[e].counter = 256 - counter + turret_counter;
   enemy[e].recycle = enemy[e].counter;
//   enemy[e].attacking = 0;
   enemy[e].just_collided = 0;
   enemy[e].carrying_pickup = 0;
   enemy[e].target = TARGET_NO;
   enemy[e].burst_fire = 0;

   int i;

   for (i = 0; i < NO_ENEMY_ATTRIBUTES; i ++)
   {
    enemy[e].attribute [i] = eclass[type].default_attributes [i];
   }

   switch(type)
   {
    default:
    case ENEMY_FIGHTER4_TURRET:
     enemy[e].colours [0] = COLOUR_BLUE6;
     enemy[e].colours [1] = COLOUR_BLUE1;
     enemy[e].attribute [ATTRIB_FI4TURRET_MAIN] = base;
     enemy[e].attribute [ATTRIB_FI4TURRET_MAIN_DEAD] = 0;
     break;
    case ENEMY_BLASTER_TURRET:
     enemy[e].colours [0] = COLOUR_BLUE6;
     enemy[e].colours [1] = COLOUR_BLUE1;
     enemy[e].attribute [ATTRIB_BTURRET_MAIN] = base;
     enemy[e].attribute [ATTRIB_BTURRET_MAIN_DEAD] = 0;
     break;
    case ENEMY_FIREBASE_TURRET:
     enemy[e].colours [0] = COLOUR_YELLOW6;
     enemy[e].colours [1] = COLOUR_YELLOW1;
     enemy[e].attribute [ATTRIB_FBTURRET_MAIN] = base;
     enemy[e].attribute [ATTRIB_FBTURRET_MAIN_DEAD] = 0;
     break;
    case ENEMY_BOSS2_TURRET:
     enemy[e].colours [0] = COLOUR_YELLOW6;
     enemy[e].colours [1] = COLOUR_YELLOW1;
     enemy[e].attribute [ATTRIB_BO2TURRET_MAIN] = base;
     enemy[e].attribute [ATTRIB_BO2TURRET_MAIN_DEAD] = 0;
     break;
    case ENEMY_BOSS3_TURRET1:
    case ENEMY_BOSS3_TURRET2:
     enemy[e].colours [0] = COLOUR_BLUE6;
     enemy[e].colours [1] = COLOUR_BLUE1;
     enemy[e].attribute [ATTRIB_BO3TURRET_MAIN] = base;
     enemy[e].attribute [ATTRIB_BO3TURRET_MAIN_DEAD] = 0;
     break;
    case ENEMY_BOSS4_TURRET:
     enemy[e].colours [0] = COLOUR_GREY6;
     enemy[e].colours [1] = COLOUR_GREY1;
     enemy[e].attribute [ATTRIB_BO4TURRET_MAIN] = base;
     enemy[e].attribute [ATTRIB_BO4TURRET_MAIN_DEAD] = 0;
     break;
    case ENEMY_FIREBASE2_TURRET1:
    case ENEMY_FIREBASE2_TURRET2:
     enemy[e].colours [0] = COLOUR_RED6;
     enemy[e].colours [1] = COLOUR_RED1;
     enemy[e].attribute [ATTRIB_FB2TURRET_MAIN] = base;
     enemy[e].attribute [ATTRIB_FB2TURRET_MAIN_DEAD] = 0;
     break;
    case ENEMY_FIREBASE3_TURRET:
     enemy[e].colours [0] = COLOUR_GREY6;
     enemy[e].colours [1] = COLOUR_GREY1;
     enemy[e].attribute [ATTRIB_FB3TURRET_MAIN] = base;
     enemy[e].attribute [ATTRIB_FB3TURRET_MAIN_DEAD] = 0;
     break;
    case ENEMY_BOSS1_TURRET1:
    case ENEMY_BOSS1_TURRET2:
     enemy[e].colours [0] = COLOUR_GREEN6;
     enemy[e].colours [1] = COLOUR_GREEN1;
     enemy[e].attribute [ATTRIB_BO1TURRET_MAIN] = base;
     enemy[e].attribute [ATTRIB_BO1TURRET_MAIN_DEAD] = 0;
     break;
   }

   enemy[e].hurt_pulse_colour1 = enemy[e].colours [0];
   enemy[e].hurt_pulse_colour2 = enemy[e].colours [1];

   enemy[e].angle = grand(ANGLE_FULL);

   return e;

}



void run_enemies(void)
{

 int e;

 for (e = 0; e < NO_ENEMIES; e++)
 {
  if (enemy[e].type != ENEMY_NONE)
   manage_enemy(e);
 }

}


void manage_enemy(int e)
{

   if (enemy[e].hurt_pulse > 0)
    enemy[e].hurt_pulse --;
   if (enemy[e].recycle > 0)
    enemy[e].recycle --;
   if (enemy[e].just_collided > 0)
    enemy[e].just_collided --;

   enemy[e].counter ++; // assumed: this wraps at 256 (unsigned char)

   char need_move_enemy = 1;

 switch(enemy[e].type)
 {
  case ENEMY_CRAWLER:
  case ENEMY_CRAWLER2:
  case ENEMY_CRAWLER3:
  case ENEMY_CRAWLER4:
   move_crawler(e);
   need_move_enemy = 0;
   break;
  case ENEMY_FALLER:
   move_faller(e);
   need_move_enemy = 0;
   break;
  case ENEMY_STINGER:
   move_stinger(e);
   break;
  case ENEMY_STINGER2:
   move_stinger2(e);
   break;
  case ENEMY_STINGER3:
   move_stinger3(e);
   break;
  case ENEMY_STINGER4:
   move_stinger4(e);
   break;
  case ENEMY_HURLER:
  case ENEMY_HURLER2:
   move_hurler(e);
   break;
  case ENEMY_HURLER3:
   move_hurler3(e);
   break;
  case ENEMY_SWERVER:
  case ENEMY_SWERVER2:
   move_swerver(e);
   need_move_enemy = 0;
   break;
  case ENEMY_SWERVER3:
   move_swerver3(e);
   need_move_enemy = 0;
   break;
  case ENEMY_DRIFTER:
  case ENEMY_DRIFTER2:
  case ENEMY_DRIFTER3:
   move_drifter(e);
   break;
  case ENEMY_TORPER2:
   move_torper2(e);
   break;
  case ENEMY_CURVER:
   move_curver(e);
   break;
  case ENEMY_BLASTER:
   move_blaster(e);
   need_move_enemy = 0;
   break;
  case ENEMY_BLASTER_TURRET:
   move_blaster_turret(e);
   need_move_enemy = 0;
   break;
  case ENEMY_FIREBASE:
   move_firebase(e);
   need_move_enemy = 0;
   break;
  case ENEMY_FIREBASE_TURRET:
   move_firebase_turret(e);
   need_move_enemy = 0;
   break;
  case ENEMY_FIREBASE2:
   move_firebase2(e);
   need_move_enemy = 0;
   break;
  case ENEMY_FIREBASE2_TURRET1:
   move_firebase2_turret1(e);
   need_move_enemy = 0;
   break;
  case ENEMY_FIREBASE2_TURRET2:
   move_firebase2_turret2(e);
   need_move_enemy = 0;
   break;
  case ENEMY_FIREBASE3:
   move_firebase3(e);
   need_move_enemy = 0;
   break;
  case ENEMY_FIREBASE3_TURRET:
   move_firebase3_turret(e);
   need_move_enemy = 0;
   break;
  case ENEMY_FIGHTER4:
   move_fighter4(e);
   need_move_enemy = 0;
   break;
  case ENEMY_FIGHTER4_TURRET:
   move_fighter4_turret(e);
   need_move_enemy = 0;
   break;
  case ENEMY_HIVE:
   move_hive(e);
   need_move_enemy = 0;
   break;
  case ENEMY_BOSS1:
   move_boss1(e);
   need_move_enemy = 0;
   break;
  case ENEMY_BOSS1_TURRET1:
  case ENEMY_BOSS1_TURRET2:
   move_boss1_turret(e);
   need_move_enemy = 0;
   break;
  case ENEMY_BOSS2:
   move_boss2(e);
   need_move_enemy = 0;
   break;
  case ENEMY_BOSS2_TURRET:
   move_boss2_turret(e);
   need_move_enemy = 0;
   break;
  case ENEMY_BOSS3:
   move_boss3(e);
   need_move_enemy = 0;
   break;
  case ENEMY_BOSS3_TURRET1:
  case ENEMY_BOSS3_TURRET2:
   move_boss3_turret(e);
   need_move_enemy = 0;
   break;
  case ENEMY_BOSS4:
   move_boss4(e);
   need_move_enemy = 0;
   break;
  case ENEMY_BOSS4_TURRET:
   move_boss4_turret(e);
   need_move_enemy = 0;
   break;
  case ENEMY_BEE:
   move_bee(e);
   break;
  case ENEMY_JELLY:
   move_jelly(e);
   break;
  case ENEMY_SUPERJELLY:
   move_superjelly(e);
   break;
  case ENEMY_CLOAKER:
  case ENEMY_CLOAKER2:
   move_cloaker(e);
   break;
  case ENEMY_FLAKKER:
  case ENEMY_FLAKKER2:
   move_flakker(e);
   break;
  case ENEMY_BOLTER:
  case ENEMY_BOLTER2:
   move_bolter(e);
   break;
  case ENEMY_TORPER:
   move_torper(e);
   break;
  case ENEMY_WANDERER:
   move_wanderer(e);
   break;
  case ENEMY_MINETHROWER:
   move_minethrower(e);
   break;
  case ENEMY_MINETHROWER2:
   move_minethrower2(e);
   break;
  case ENEMY_DEAD_FIGHTER:
  case ENEMY_DEAD_FIGHTER2:
  case ENEMY_DEAD_FIGHTER3:
   move_dead_fighter(e);
   break;
  case ENEMY_FIGHTER:
  case ENEMY_FIGHTER2:
  case ENEMY_FIGHTER3:
   move_fighter(e);
   break;
  case ENEMY_DEAD_WANDERER:
   move_dead_wanderer(e);
   break;
  case ENEMY_SUN_GREEN:
  case ENEMY_SUN_BLUE:
  case ENEMY_SUN:
   move_sun(e);
   break;
  case ENEMY_BOUNCER2:
  case ENEMY_BOUNCER3:
  case ENEMY_BOUNCER4:
  case ENEMY_BOUNCER5:
   move_bouncers(e);
   break; // normal bouncers don't need this.
//  default:
//   if (move_enemy(e) == 0)
//    break;
 }
//   int things [2];
//   int k = detect_collision(e, things);

// NOTE: some (eg turrets, or crawlers/fallers/swervers) won't get this far
 if (enemy[e].type != ENEMY_NONE && need_move_enemy == 1)
   move_enemy(e);

}


int hurt_enemy(int e, int hurty, int source, char pulse)
{

 int k;

 switch(enemy[e].type)
 {
  case ENEMY_HIVE:
   if (enemy[e].attribute [ATTRIB_HIVE_ATTACK13] == ATTACK_NONE)
    enemy[e].attribute [ATTRIB_HIVE_ATTACK13] = source;
   if (enemy[e].attribute [ATTRIB_HIVE_ATTACK24] == ATTACK_NONE)
    enemy[e].attribute [ATTRIB_HIVE_ATTACK24] = source;
    break;
  case ENEMY_BEE:
   k = enemy[e].attribute [ATTRIB_BEE_HIVE];
   if (k != -1)
   {
    if (enemy[k].attribute [ATTRIB_HIVE_ATTACK13] == ATTACK_NONE)
     enemy[k].attribute [ATTRIB_HIVE_ATTACK13] = source;
    if (enemy[k].attribute [ATTRIB_HIVE_ATTACK24] == ATTACK_NONE)
     enemy[k].attribute [ATTRIB_HIVE_ATTACK24] = source;
   }
   break;
 }

// shields go here...

 int passing_colours [5];

 if (hurty >= enemy[e].armour)
 {
  switch(enemy[e].type)
  {
   case ENEMY_FALLER:
    if (enemy[e].attribute [ATTRIB_FALLER_SECTIONS] > 0)
    {
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
     place_explosion(enemy[e].x, enemy[e].y - (enemy[e].attribute [ATTRIB_FALLER_SECTIONS] * 7 * GRAIN), 0, 0,
     100 + crandom(100), passing_colours);
     enemy[e].attribute [ATTRIB_FALLER_SECTIONS] --;
     enemy[e].armour = enemy[e].attribute [ATTRIB_FALLER_ARMOUR];
     enemy[e].hurt_pulse = 8;
     enemy_sound(e, ESOUND_MINEBANG);
     return 1;
    }
    break;
  }
  register_destroyed(e, source);
  enemy_explodes(e);
  return 0;
 }

 enemy[e].armour -= hurty;
 if (pulse)
  enemy[e].hurt_pulse = 8;
 return 1;

}


void enemy_explodes(int e)
{
 int j;
// int xs = enemy[e].x_speed;
// int ys = enemy[e].y_speed;

 int passing_colours [5];
 int bullet_colours [4];
 int special [10];

 int standard_sound = ESOUND_EXPLODE1;

 switch(enemy[e].type)
 {
/*  case ENEMY_BOUNCER:
  case ENEMY_SWERVER:
  case ENEMY_SWERVER3:
  case ENEMY_BOUNCER4:
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_YELLOW;
//   place_explosion(enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed,
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
   600 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 1000, 1500,
    4, passing_colours);
   blast(enemy[e].x, enemy[e].y, 30000 + (enemy[e].subtype * 2000), 350 + (enemy[e].subtype * 30), 5000, OWNER_ENEMY);
   break;*/
  case ENEMY_BOUNCER2:
  case ENEMY_BOUNCER3:
  case ENEMY_BOUNCER5:
  case ENEMY_SWERVER2:
   standard_attack(e, grand(ANGLE_FULL));
// Fall through:
  case ENEMY_BOUNCER:
  case ENEMY_SWERVER:
  case ENEMY_SWERVER3:
  case ENEMY_BOUNCER4:
//   switch(enemy[e].subtype)
//   {
//    default:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
//     break;
//   }
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    600 + crandom(250) + (enemy[e].subtype * 100), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 1000, 1500,
    4, passing_colours);
   passing_colours [3] = TRANS_WHITE2;
   passing_colours [2] = TRANS_LGREY;
   passing_colours [1] = TRANS_GREY;
   passing_colours [0] = TRANS_DGREY;
      create_cloud(CLOUD_SHOCKWAVE,
       enemy[e].x, enemy[e].y,
       0, 0, 0, 0,
       200 + crandom(50) + (enemy[e].subtype * 40),15,0, 0, 250 + grand(100) + (enemy[e].subtype * 40), 0, passing_colours);
   blast(enemy[e].x, enemy[e].y, 30000 + (enemy[e].subtype * 2000), 350 + (enemy[e].subtype * 30), 5000, OWNER_ENEMY);
   break;
  case ENEMY_CRAWLER:
  case ENEMY_CRAWLER2:
  case ENEMY_CRAWLER3:
  case ENEMY_CRAWLER4:
  case ENEMY_FALLER:
  case ENEMY_DEAD_FIGHTER:
  case ENEMY_DEAD_FIGHTER2:
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_YELLOW;
//   place_explosion(enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed,
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    400 + crandom(250), passing_colours);
// passing_colours already set:
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 1000, 1500,
    4, passing_colours);
   break;
  case ENEMY_CLOAKER:
  case ENEMY_JELLY:
  case ENEMY_FLAKKER:
  case ENEMY_FLAKKER2:
  case ENEMY_BOLTER:
  case ENEMY_BOLTER2:
  case ENEMY_DEAD_WANDERER:
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_YELLOW;
//   place_explosion(enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed,
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    1200 + crandom(250), passing_colours);
// passing_colours already set:
//   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 1500, 1500,
//    5, passing_colours);
//   passing_colours[0] = COLOUR_ORANGE8;
//   passing_colours[1] = COLOUR_ORANGE7;
//   passing_colours[2] = COLOUR_ORANGE6;
//   passing_colours[3] = COLOUR_ORANGE4;
//   passing_colours[4] = COLOUR_ORANGE2;
//   place_speck_burst(enemy[e].x, enemy[e].y, 0, 0, 15 + grand(10), 20, 20, 1000, passing_colours);
   break;
  case ENEMY_STINGER4:
   enemy_sound(e, ESOUND_BLAST);
   standard_sound = ESOUND_NONE;
  case ENEMY_DRIFTER:
  case ENEMY_DRIFTER2:
  case ENEMY_STINGER:
  case ENEMY_STINGER2:
  case ENEMY_STINGER3:
  case ENEMY_HURLER:
  case ENEMY_TORPER2:
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_YELLOW;
//   place_explosion(enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed,
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    700 + crandom(250), passing_colours);
// passing_colours already set:
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 1500, 1500,
    5, passing_colours);
   break;
  case ENEMY_WANDERER:
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    700 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 1500, 1500,
    5, passing_colours);
//   passing_colours[0] = COLOUR_ORANGE8;
//   passing_colours[1] = COLOUR_ORANGE7;
//   passing_colours[2] = COLOUR_ORANGE6;
//   passing_colours[3] = COLOUR_ORANGE4;
//   passing_colours[4] = COLOUR_ORANGE2;
//   place_speck_burst(enemy[e].x, enemy[e].y, 0, 0, 15 + grand(10), 20, 20, 1000, passing_colours);
   create_enemy(ENEMY_DEAD_WANDERER, SUBTYPE_NONE, enemy[e].x, enemy[e].y,
    enemy[e].x_speed, enemy[e].y_speed, enemy[e].angle, enemy[e].carrying_pickup, special,
    TARGET_NO);
// there's an exception to prevent wanderers dropping pickups when destroyed
   break;
  case ENEMY_FIGHTER:
   enemy_sound(e, ESOUND_EXPLODE2);
   standard_sound = ESOUND_NONE;
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_YELLOW;
//   place_explosion(enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed,
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    400 + crandom(250), passing_colours);
// passing_colours already set:
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 1000, 1500,
    4, passing_colours);
   create_enemy(ENEMY_DEAD_FIGHTER, SUBTYPE_NONE, enemy[e].x, enemy[e].y,
    enemy[e].x_speed, enemy[e].y_speed, enemy[e].angle, enemy[e].carrying_pickup, special,
    TARGET_NO);
// there's an exception to prevent fighters dropping pickups when shot down
   break;
  case ENEMY_FIGHTER2:
   enemy_sound(e, ESOUND_EXPLODE2);
   standard_sound = ESOUND_NONE;
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_YELLOW;
//   place_explosion(enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed,
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    400 + crandom(250), passing_colours);
// passing_colours already set:
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 1000, 1500,
    4, passing_colours);
   create_enemy(ENEMY_DEAD_FIGHTER2, SUBTYPE_NONE, enemy[e].x, enemy[e].y,
    enemy[e].x_speed, enemy[e].y_speed, enemy[e].angle, enemy[e].carrying_pickup, special,
    TARGET_NO);
// there's an exception to prevent fighters dropping pickups when shot down
   break;
  case ENEMY_FIGHTER3:
   enemy_sound(e, ESOUND_EXPLODE2);
   standard_sound = ESOUND_NONE;
   create_enemy(ENEMY_DEAD_FIGHTER3, SUBTYPE_NONE, enemy[e].x, enemy[e].y,
    enemy[e].x_speed, enemy[e].y_speed, enemy[e].angle, enemy[e].carrying_pickup, special,
    TARGET_NO);
  case ENEMY_DEAD_FIGHTER3:
   passing_colours[0] = TRANS_DGREEN;
   passing_colours[1] = TRANS_LGREEN;
   passing_colours[2] = TRANS_YELLOW;
//   place_explosion(enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed,
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    400 + crandom(250), passing_colours);
// passing_colours already set:
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 1000, 1500,
    4, passing_colours);
// there's an exception to prevent fighters dropping pickups when shot down
   break;
  case ENEMY_SUN:
   passing_colours [0] = TRANS_LORANGE;
   passing_colours [1] = COLOUR_ORANGE8;
   place_line_burst(enemy[e].x, enemy[e].y, 0, 0, 20, 15 + grand(10), 15, 1200, 300, passing_colours);
   passing_colours [0] = TRANS_DORANGE;
   passing_colours [1] = TRANS_LORANGE;
   passing_colours [2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0, 1200 + grand(250), passing_colours);
   break;
  case ENEMY_SUN_GREEN:
   passing_colours [0] = TRANS_LGREEN;
   passing_colours [1] = COLOUR_GREEN8;
   place_line_burst(enemy[e].x, enemy[e].y, 0, 0, 20, 15 + grand(10), 15, 1200, 300, passing_colours);
   passing_colours [0] = TRANS_DGREEN;
   passing_colours [1] = TRANS_LGREEN;
   passing_colours [2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0, 1200 + grand(250), passing_colours);
   break;
  case ENEMY_SUN_BLUE:
   passing_colours [0] = TRANS_LBLUE;
   passing_colours [1] = COLOUR_BLUE8;
   place_line_burst(enemy[e].x, enemy[e].y, 0, 0, 20, 15 + grand(10), 15, 1200, 300, passing_colours);
   passing_colours [0] = TRANS_DBLUE;
   passing_colours [1] = TRANS_LBLUE;
   passing_colours [2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0, 1200 + grand(250), passing_colours);
   break;
  case ENEMY_MINETHROWER:
   for (j = 0; j < 6; j ++)
   {
    create_bullet(BULLET_PREMINE, enemy[e].x, enemy[e].y,
     enemy[e].x_speed + grand(15001) - 7500, enemy[e].y_speed + grand(15001) - 7500,
     OWNER_ENEMY, 100, 50 + grand(20), 100, 0,
     0, 0, bullet_colours, 1,0,0,0,0,0);
   }
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    1200 + crandom(250), passing_colours);
   break;
  case ENEMY_MINETHROWER2:
   for (j = 0; j < 6; j ++)
   {
    create_bullet(BULLET_PRESEEKMINE, enemy[e].x, enemy[e].y,
     enemy[e].x_speed + grand(15001) - 7500, enemy[e].y_speed + grand(15001) - 7500,
     OWNER_ENEMY, 100, 50 + grand(20), 100, 0,
     0, 0, bullet_colours, 1,0,0,0,0,0);
   }
   passing_colours[0] = TRANS_DBLUE;
   passing_colours[1] = TRANS_LBLUE;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    1200 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   break;
  case ENEMY_BLASTER:
   passing_colours[0] = TRANS_DBLUE;
   passing_colours[1] = TRANS_LBLUE;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    1200 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   if (enemy[e].attribute [ATTRIB_BLASTER_TURRET1] != -1)
   enemy[enemy[e].attribute [ATTRIB_BLASTER_TURRET1]].attribute [ATTRIB_BTURRET_MAIN_DEAD] = 1;
//    hurt_enemy(enemy[e].attribute [ATTRIB_BLASTER_TURRET1], 99999, OWNER_ENEMY);
   if (enemy[e].attribute [ATTRIB_BLASTER_TURRET2] != -1)
    enemy[enemy[e].attribute [ATTRIB_BLASTER_TURRET2]].attribute [ATTRIB_BTURRET_MAIN_DEAD] = 1;
//    hurt_enemy(enemy[e].attribute [ATTRIB_BLASTER_TURRET2], 99999, OWNER_ENEMY);
   break;
  case ENEMY_BLASTER_TURRET:
   passing_colours[0] = TRANS_DBLUE;
   passing_colours[1] = TRANS_LBLUE;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    500 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   break;
  case ENEMY_BOSS1:
   passing_colours[0] = TRANS_DGREEN;
   passing_colours[1] = TRANS_LGREEN;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    5000 + crandom(250), passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, 0, 0,
    99,1,0, 0, 1300, 0, passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   if (enemy[e].attribute [ATTRIB_BOSS1_TURRET1] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS1_TURRET1]].attribute [ATTRIB_BO1TURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_BOSS1_TURRET2] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS1_TURRET2]].attribute [ATTRIB_BO1TURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_BOSS1_TURRET3] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS1_TURRET3]].attribute [ATTRIB_BO1TURRET_MAIN_DEAD] = 1;
/*   if (enemy[e].attribute [ATTRIB_BOSS1_TURRET4] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS1_TURRET4]].attribute [ATTRIB_BO1TURRET_MAIN_DEAD] = 1;*/
   break;
//  case ENEMY_BOSS1_TURRET1:
  case ENEMY_BOSS1_TURRET2:
  case ENEMY_HURLER3:
   passing_colours[0] = TRANS_DGREEN;
   passing_colours[1] = TRANS_LGREEN;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    1500 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   break;
  case ENEMY_BOSS2:
   passing_colours[0] = TRANS_DORANGE;
   passing_colours[1] = TRANS_LORANGE;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    5000 + crandom(250), passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, 0, 0,
    120,1,0, 0, 1500, 0, passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   if (enemy[e].attribute [ATTRIB_BOSS2_TURRET1] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS2_TURRET1]].attribute [ATTRIB_BO2TURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_BOSS2_TURRET2] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS2_TURRET2]].attribute [ATTRIB_BO2TURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_BOSS2_TURRET3] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS2_TURRET3]].attribute [ATTRIB_BO2TURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_BOSS2_TURRET4] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS2_TURRET4]].attribute [ATTRIB_BO2TURRET_MAIN_DEAD] = 1;
   break;
  case ENEMY_BOSS2_TURRET:
  case ENEMY_HURLER2:
//  case ENEMY_HURLER3:
   passing_colours[0] = TRANS_DORANGE;
   passing_colours[1] = TRANS_LORANGE;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    1500 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   break;
  case ENEMY_TORPER:
   passing_colours[0] = TRANS_DORANGE;
   passing_colours[1] = TRANS_LORANGE;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    1500 + crandom(250), passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, 0, 0,
    50 + crandom(10),1,0, 0, 500, 0, passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   break;
  case ENEMY_BOSS3:
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_LORANGE;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    5000 + crandom(250), passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, 0, 0,
    135,1,0, 0, 1100, 0, passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   if (enemy[e].attribute [ATTRIB_BOSS3_TURRET1] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS3_TURRET1]].attribute [ATTRIB_BO3TURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_BOSS3_TURRET2] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS3_TURRET2]].attribute [ATTRIB_BO3TURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_BOSS3_TURRET3] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS3_TURRET3]].attribute [ATTRIB_BO3TURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_BOSS3_TURRET4] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS3_TURRET4]].attribute [ATTRIB_BO3TURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_BOSS3_TURRET5] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS3_TURRET5]].attribute [ATTRIB_BO3TURRET_MAIN_DEAD] = 1;
   break;
  case ENEMY_BOSS3_TURRET1:
  case ENEMY_BOSS3_TURRET2:
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_LORANGE;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    1500 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   break;
  case ENEMY_BOSS4:
   passing_colours[0] = TRANS_DBLUE;
   passing_colours[1] = TRANS_LBLUE;
   passing_colours[2] = TRANS_PURPLE;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    5000 + crandom(250), passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, 0, 0,
    500,1,0, 0, 1300, 0, passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   if (enemy[e].attribute [ATTRIB_BOSS4_TURRET1] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS4_TURRET1]].attribute [ATTRIB_BO4TURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_BOSS4_TURRET2] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS4_TURRET2]].attribute [ATTRIB_BO4TURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_BOSS4_TURRET3] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS4_TURRET3]].attribute [ATTRIB_BO4TURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_BOSS4_TURRET4] != -1)
    enemy[enemy[e].attribute [ATTRIB_BOSS4_TURRET4]].attribute [ATTRIB_BO4TURRET_MAIN_DEAD] = 1;
   break;
  case ENEMY_BOSS4_TURRET:
   passing_colours[0] = TRANS_LBLUE;
   passing_colours[1] = TRANS_PURPLE;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    1500 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   break;
  case ENEMY_FIGHTER4:
   passing_colours[0] = TRANS_DBLUE;
   passing_colours[1] = TRANS_LBLUE;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    1200 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   if (enemy[e].attribute [ATTRIB_FIGHTER4_TURRET1] != -1)
   enemy[enemy[e].attribute [ATTRIB_FIGHTER4_TURRET1]].attribute [ATTRIB_FI4TURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_FIGHTER4_TURRET2] != -1)
    enemy[enemy[e].attribute [ATTRIB_FIGHTER4_TURRET2]].attribute [ATTRIB_FI4TURRET_MAIN_DEAD] = 1;
   break;
  case ENEMY_FIGHTER4_TURRET:
//   passing_colours[0] = TRANS_DGREEN;
//   passing_colours[1] = TRANS_LGREEN;
   passing_colours[0] = TRANS_DBLUE;
   passing_colours[1] = TRANS_LBLUE;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    500 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   break;
  case ENEMY_FIREBASE:
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    2500 + crandom(250), passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, 0, 0,
    50 + crandom(10),1,0, 0, 700, 0, passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   if (enemy[e].attribute [ATTRIB_FIREBASE_TURRET1] != -1)
   enemy[enemy[e].attribute [ATTRIB_FIREBASE_TURRET1]].attribute [ATTRIB_FBTURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_FIREBASE_TURRET2] != -1)
   enemy[enemy[e].attribute [ATTRIB_FIREBASE_TURRET2]].attribute [ATTRIB_FBTURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_FIREBASE_TURRET3] != -1)
   enemy[enemy[e].attribute [ATTRIB_FIREBASE_TURRET3]].attribute [ATTRIB_FBTURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_FIREBASE_TURRET4] != -1)
   enemy[enemy[e].attribute [ATTRIB_FIREBASE_TURRET4]].attribute [ATTRIB_FBTURRET_MAIN_DEAD] = 1;
   break;
  case ENEMY_HIVE:
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    2500 + crandom(250), passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, 0, 0,
    70 + crandom(20),1,0, 0, 800, 0, passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   for (j = 0; j < NO_ENEMIES; j ++)
   {
    if (enemy[j].type == ENEMY_BEE)
     enemy[j].attribute [ATTRIB_BEE_HIVE] = -1;
   }
   break;
  case ENEMY_BEE:
   passing_colours[0] = TRANS_DORANGE;
   passing_colours[1] = TRANS_LORANGE;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    100 + crandom(150), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 2 + grand(3), 2, 6, 1500, 1500,
    5, passing_colours);
   break;
  case ENEMY_FIREBASE_TURRET:
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    800 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   break;
  case ENEMY_FIREBASE2:
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    2500 + crandom(250), passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, 0, 0,
    60 + crandom(10),1,0, 0, 600, 0, passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   if (enemy[e].attribute [ATTRIB_FIREBASE_TURRET1] != -1)
   enemy[enemy[e].attribute [ATTRIB_FIREBASE_TURRET1]].attribute [ATTRIB_FBTURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_FIREBASE_TURRET2] != -1)
   enemy[enemy[e].attribute [ATTRIB_FIREBASE_TURRET2]].attribute [ATTRIB_FBTURRET_MAIN_DEAD] = 1;
   break;
  case ENEMY_FIREBASE2_TURRET1:
  case ENEMY_FIREBASE2_TURRET2:
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    800 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   break;
  case ENEMY_FIREBASE3:
   passing_colours[0] = TRANS_DBLUE;
   passing_colours[1] = TRANS_LBLUE;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    2500 + crandom(250), passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, 0, 0,
    70 + crandom(20),1,0, 0, 900, 0, passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   if (enemy[e].attribute [ATTRIB_FIREBASE3_TURRET1] != -1)
   enemy[enemy[e].attribute [ATTRIB_FIREBASE3_TURRET1]].attribute [ATTRIB_FB3TURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_FIREBASE3_TURRET2] != -1)
   enemy[enemy[e].attribute [ATTRIB_FIREBASE3_TURRET2]].attribute [ATTRIB_FB3TURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_FIREBASE3_TURRET3] != -1)
   enemy[enemy[e].attribute [ATTRIB_FIREBASE3_TURRET3]].attribute [ATTRIB_FB3TURRET_MAIN_DEAD] = 1;
   if (enemy[e].attribute [ATTRIB_FIREBASE3_TURRET4] != -1)
   enemy[enemy[e].attribute [ATTRIB_FIREBASE3_TURRET4]].attribute [ATTRIB_FB3TURRET_MAIN_DEAD] = 1;
   break;
  case ENEMY_FIREBASE3_TURRET:
   passing_colours[0] = TRANS_DBLUE;
   passing_colours[1] = TRANS_LBLUE;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    800 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   break;
  case ENEMY_DRIFTER3:
  case ENEMY_CURVER:
  case ENEMY_SUPERJELLY:
  case ENEMY_CLOAKER2:
   passing_colours[0] = TRANS_DBLUE;
   passing_colours[1] = TRANS_PURPLE;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    1200 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 3 + grand(3), 2, 6, 1500, 1500,
    6, passing_colours);
   break;
  

 }

 if (standard_sound != ESOUND_NONE)
  enemy_sound(e, standard_sound);



 if (enemy[e].carrying_pickup != PICKUP_NONE
     && enemy[e].type != ENEMY_WANDERER && enemy[e].type != ENEMY_FIGHTER
     && enemy[e].type != ENEMY_FIGHTER2 && enemy[e].type != ENEMY_FIGHTER3)
 {
  create_pickup(enemy[e].carrying_pickup, enemy[e].x, enemy[e].y, 333);
 }

 destroy_enemy(e);

}

void destroy_enemy(int e)
{
 enemy[e].type = ENEMY_NONE;
 calculate_ambience();
}


// REMEMBER this can be called from collision detection in actor.c
int move_enemy(int e)
{

 switch(enemy[e].type)
 {
  case ENEMY_CRAWLER:
  case ENEMY_CRAWLER2:
  case ENEMY_CRAWLER3:
  case ENEMY_CRAWLER4:
  case ENEMY_FALLER:
  case ENEMY_SWERVER:
  case ENEMY_SWERVER2:
  case ENEMY_SWERVER3:
   return 0;
 }

if (enemy[e].x_speed == 0 && enemy[e].y_speed == 0) return 1;

  if (enemy[e].x + enemy[e].x_speed <= enemy[e].edge_radius
      || enemy[e].x + enemy[e].x_speed >= arena.max_x - enemy[e].edge_radius - 4000)
  {
   enemy[e].x_speed *= -1;
  }
  if (enemy[e].y + enemy[e].y_speed <= enemy[e].edge_radius
      || enemy[e].y + enemy[e].y_speed >= arena.max_y - enemy[e].edge_radius - 5000)
  {
   enemy[e].y_speed *= -1;
  }


//enemy[mbull].x2 += bullet[mbull].x_speed;
//bullet[mbull].y2 += bullet[mbull].y_speed;

enemy[e].x += enemy[e].x_speed;
enemy[e].y += enemy[e].y_speed;

// if an enemy is subject to drag, this happens in its own function

return 1;


}


int move_faller(int e)
{

  if (enemy[e].y + enemy[e].attribute [ATTRIB_FALLER_SPEED] > arena.max_y)
  {
   metamorphosis(e);
   return 0;
  }

  enemy[e].y += enemy[e].attribute [ATTRIB_FALLER_SPEED];
  enemy[e].x_speed = 0;
  enemy[e].y_speed = enemy[e].attribute [ATTRIB_FALLER_SPEED];

 return 1;


}



int move_crawler(int e)
{
 // speed of crawler must not be divisible by GRID_WIDTH

 int ex, ey;

 int sp = enemy[e].attribute [ATTRIB_CRAWLER_SPEED];

 if (enemy[e].attribute [ATTRIB_CRAWLER_AXIS] == 0)
 {
  enemy[e].x += sp * enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X];
  ex = enemy[e].x + 4 * GRAIN;
//  if ((enemy[e].x / GRAIN) / GRID_WIDTH
//   != ((enemy[e].x + (995 * enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X])) / GRAIN) / GRID_WIDTH
//      || grand(3) == 0
  if (((enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X] == 1
    && ((ex / GRAIN) % GRID_WIDTH >
       ((ex + sp) / GRAIN) % GRID_WIDTH))
  ||
  (enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X] == -1
    && ((ex / GRAIN) % GRID_WIDTH <
       ((ex - sp) / GRAIN) % GRID_WIDTH)))
   &&
   (grand(4) == 0
      || (ex >= arena.max_x - (3 * GRID_WIDTH * GRAIN)
          && enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X] == 1)
      || (ex <= (3 * GRID_WIDTH * GRAIN)
          && enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X] == -1)))
  {
//   enemy[e].x = (enemy[e].x / GRAIN) % GRID_WIDTH + (enemy[e].x / GRAIN) / GRID_WIDTH;
   enemy[e].attribute [ATTRIB_CRAWLER_AXIS] = 1;
  }
  enemy[e].x_speed = sp * enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X];
  enemy[e].y_speed = 0;
  return 1;
 }

  if (enemy[e].y + sp >= arena.max_y - 8 * GRAIN)
  {
   metamorphosis(e);
   return 0;
  }

  enemy[e].y += sp;

  enemy[e].x_speed = 0;
  enemy[e].y_speed = sp;
  // need to give speed values for collisions


  ey = enemy[e].y + 4 * GRAIN;
  ex = enemy[e].x + 4 * GRAIN;

  if ((ey / GRAIN) / GRID_WIDTH
   != (((ey + sp) / GRAIN)) / GRID_WIDTH)
  {
//   enemy[e].y = (enemy[e].y / GRAIN) % GRID_WIDTH + (enemy[e].y / GRAIN) / GRID_WIDTH;
   enemy[e].attribute [ATTRIB_CRAWLER_AXIS] = 0;
   enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X] = -1;
   if (ex < 3 * GRID_WIDTH * GRAIN
    || (ex < arena.max_x - (2 * GRID_WIDTH * GRAIN) && grand(4) == 0))
     enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X] = 1;
  }

 return 1;
 
/*
 enemy[e].x_speed = 0;
 enemy[e].y_speed = 0;

 int ex;

 int sp = enemy[e].attribute [ATTRIB_CRAWLER_SPEED];

 if (enemy[e].attribute [ATTRIB_CRAWLER_AXIS] == 0)
 {
  enemy[e].x += sp * enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X];
//  if ((enemy[e].x / GRAIN) / GRID_WIDTH
//   != ((enemy[e].x + (995 * enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X])) / GRAIN) / GRID_WIDTH
//      || grand(3) == 0
  if (((enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X] == 1
    && ((enemy[e].x / GRAIN) % GRID_WIDTH >
       ((enemy[e].x + sp) / GRAIN) % GRID_WIDTH))
  ||
  (enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X] == -1
    && ((enemy[e].x / GRAIN) % GRID_WIDTH <
       ((enemy[e].x - sp) / GRAIN) % GRID_WIDTH)))
   &&
   (grand(4) == 0
      || (enemy[e].x >= arena.max_x - (3 * GRID_WIDTH * GRAIN)
          && enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X] == 1)
      || (enemy[e].x <= (3 * GRID_WIDTH * GRAIN)
          && enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X] == -1)))
  {
//   enemy[e].x = (enemy[e].x / GRAIN) % GRID_WIDTH + (enemy[e].x / GRAIN) / GRID_WIDTH;
   enemy[e].attribute [ATTRIB_CRAWLER_AXIS] = 1;
  }
  return 1;
 }

  if (enemy[e].y + sp >= arena.max_y - 80 * GRAIN)
  {
   metamorphosis(e);
   return 0;
  }

  enemy[e].y += sp;

  if ((enemy[e].y / GRAIN) / GRID_WIDTH
   != (((enemy[e].y + sp) / GRAIN)) / GRID_WIDTH)
  {
//   enemy[e].y = (enemy[e].y / GRAIN) % GRID_WIDTH + (enemy[e].y / GRAIN) / GRID_WIDTH;
   enemy[e].attribute [ATTRIB_CRAWLER_AXIS] = 0;
   enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X] = -1;
   if (enemy[e].x < 3 * GRID_WIDTH * GRAIN
    || (enemy[e].x < arena.max_x - (2 * GRID_WIDTH * GRAIN) && grand(4) == 0))
     enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X] = 1;
  }

 return 1;
*/

}

int move_stinger(int e)
{
 if (grand(15) == 0)
 {
  accelerate_enemy(e, grand(ANGLE_FULL + 1), 1000 + grand(1000));
 }

 drag_enemy(e, 0.04);

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
 }

  if (enemy[e].attacking != ATTACK_NONE)
  {
    enemy[e].angle = track_target(e, enemy[e].attacking, 16);
    if (enemy[e].recycle == 0)
     standard_attack(e, enemy[e].angle);
  }
  
 return 1;

}




int move_bolter(int e)
{

 if (grand(15) == 0)
 {
  accelerate_enemy(e, grand(ANGLE_FULL + 1), 1000 + grand(1000));
 }

 drag_enemy(e, 0.04);

 enemy[e].attribute [ATTRIB_BOLTER_ANGLE2] += enemy[e].attribute [ATTRIB_BOLTER_SPIN];
 if (enemy[e].attribute [ATTRIB_BOLTER_ANGLE2] < 0)
  enemy[e].attribute [ATTRIB_BOLTER_ANGLE2] += ANGLE_FULL;
 if (enemy[e].attribute [ATTRIB_BOLTER_ANGLE2] < ANGLE_FULL)
  enemy[e].attribute [ATTRIB_BOLTER_ANGLE2] -= ANGLE_FULL;

 enemy[e].attribute [ATTRIB_BOLTER_SPIN] += enemy[e].attribute [ATTRIB_BOLTER_DELTA_SPIN];
 if (enemy[e].attribute [ATTRIB_BOLTER_SPIN] > 30)
  enemy[e].attribute [ATTRIB_BOLTER_SPIN] = 30;
 if (enemy[e].attribute [ATTRIB_BOLTER_SPIN] < -30)
  enemy[e].attribute [ATTRIB_BOLTER_SPIN] = -30;
  
/* if (grand(10) == 0)
 {
  enemy[e].attribute [ATTRIB_BOUNCER_DELTA_SPIN] = grand(5) - 2;
 }

 if (enemy[e].type != ENEMY_BOUNCER4)
  return 0;
*/

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(500000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].recycle == 0 && enemy[e].burst_fire == 4)
  enemy[e].attribute [ATTRIB_BOLTER_DELTA_SPIN] *= -1;

  if (enemy[e].attacking != ATTACK_NONE)
  {
//    enemy[e].attribute [ATTRIB_BOLTER_ANGLE] = track_target(e, enemy[e].attacking, 16);
//   enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y, actor[attacking].x, actor[attacking].y, enemy[e].angle, turning);
    if (enemy[e].recycle == 0)
     standard_attack(e, enemy[e].attribute [ATTRIB_BOLTER_ANGLE2]);
  }
  
 return 1;

}





int move_torper(int e)
{

 if (grand(15) == 0)
 {
  accelerate_enemy(e, grand(ANGLE_FULL + 1), 1000 + grand(1000));
 }

 drag_enemy(e, 0.03);

 enemy[e].attribute [ATTRIB_TORPER_SPRITE] ++;

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(500000, enemy[e].x, enemy[e].y);
 }

  if (enemy[e].attacking != ATTACK_NONE)
  {
    enemy[e].angle = track_target(e, enemy[e].attacking, 32);
    if (enemy[e].recycle == 0)
     standard_attack(e, enemy[e].angle);
  }
  
 return 1;

}





int move_stinger2(int e)
{
 if (grand(10) == 0)
 {
  accelerate_enemy(e, grand(ANGLE_FULL + 1), 800 + grand(800));
 }

 drag_enemy(e, 0.02);

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  if (enemy[e].burst_fire > 0)
   enemy[e].angle = track_target(e, enemy[e].attacking, 16);
    else
     enemy[e].angle = track_target(e, enemy[e].attacking, 32);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }
  
 return 1;

}

int move_stinger3(int e)
{
 if (grand(8) == 0)
 {
  accelerate_enemy(e, grand(ANGLE_FULL + 1), 600 + grand(700));
 }

 drag_enemy(e, 0.02);

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  if (enemy[e].burst_fire > 0)
   enemy[e].angle = track_target(e, enemy[e].attacking, 16);
    else
     enemy[e].angle = track_target(e, enemy[e].attacking, 32);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }
  
 return 1;

}

int move_stinger4(int e)
{
 if (grand(8) == 0)
 {
  accelerate_enemy(e, grand(ANGLE_FULL + 1), 600 + grand(700));
 }

 drag_enemy(e, 0.02);

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  if (enemy[e].burst_fire > 0)
   enemy[e].angle = track_target(e, enemy[e].attacking, 16);
    else
     enemy[e].angle = track_target(e, enemy[e].attacking, 32);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }
  
 return 1;

}


int move_swerver(int e)
{

 int tangle = enemy[e].attribute [ATTRIB_SWERVER_ANGLE];

 if (enemy[e].counter % 32 == 0)
  enemy[e].attacking = closest_target(enemy[e].x, enemy[e].y);

 enemy[e].angle = turn_towards_angle(enemy[e].angle,
        enemy[e].attribute [ATTRIB_SWERVER_ANGLE],
        enemy[e].attribute [ATTRIB_SWERVER_TURN]);

 if (enemy[e].attacking != ATTACK_NONE
     && (tangle / 100 == enemy[e].angle / 100
     || tangle / 100 == (enemy[e].angle + ANGLE_HALF) / 100
     || tangle / 100 == (enemy[e].angle - ANGLE_HALF) / 100))
 {
  enemy[e].attribute [ATTRIB_SWERVER_ANGLE] = grand(ANGLE_FULL + 1);
  if (grand(2) == 0)
  {
   enemy[e].attribute [ATTRIB_SWERVER_ANGLE] =
    radians_to_angle(atan2((actor[enemy[e].attacking].y - enemy[e].y), (actor[enemy[e].attacking].x - enemy[e].x)));
   if (enemy[e].attribute [ATTRIB_SWERVER_ANGLE] < 0)
    enemy[e].attribute [ATTRIB_SWERVER_ANGLE] += ANGLE_FULL;
   if (enemy[e].attribute [ATTRIB_SWERVER_ANGLE] > ANGLE_FULL)
    enemy[e].attribute [ATTRIB_SWERVER_ANGLE] -= ANGLE_FULL;
  }
 }
 
  enemy[e].x_speed = cos(angle_to_radians(enemy[e].angle)) * enemy[e].attribute [ATTRIB_SWERVER_SPEED];
  enemy[e].y_speed = sin(angle_to_radians(enemy[e].angle)) * enemy[e].attribute [ATTRIB_SWERVER_SPEED];

  if (enemy[e].x + enemy[e].x_speed <= enemy[e].edge_radius
      || enemy[e].x + enemy[e].x_speed >= arena.max_x - enemy[e].edge_radius - 4000)
  {
   enemy[e].x_speed *= -1;
   enemy[e].angle += ANGLE_HALF;
   enemy[e].attribute [ATTRIB_SWERVER_ANGLE] = grand(ANGLE_FULL + 1);
  }
  if (enemy[e].y + enemy[e].y_speed <= enemy[e].edge_radius
      || enemy[e].y + enemy[e].y_speed >= arena.max_y - enemy[e].edge_radius - 5000)
  {
   enemy[e].y_speed *= -1;
   enemy[e].angle -= ANGLE_HALF;
   enemy[e].attribute [ATTRIB_SWERVER_ANGLE] = grand(ANGLE_FULL + 1);
  }

enemy[e].x += enemy[e].x_speed;
enemy[e].y += enemy[e].y_speed;

// if (enemy[e].counter % 16 == 0)
// {
//  enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
// }
/*
  if (enemy[e].attacking != ATTACK_NONE)
  {
    enemy[e].attribute [ATTRIB_SWERVER3_ANGLE2] = track_target(e, enemy[e].attacking, 16);
    if (enemy[e].recycle == 0)
     standard_attack(e, enemy[e].attribute [ATTRIB_SWERVER3_ANGLE2]);
  }
*/
 return 1;

}


int move_swerver3(int e)
{

// int tangle = enemy[e].attribute [ATTRIB_SWERVER3_ANGLE2];
 int tangle = enemy[e].attribute [ATTRIB_SWERVER_ANGLE];

 if (enemy[e].counter % 32 == 0)
  enemy[e].attacking = closest_target(enemy[e].x, enemy[e].y);

 enemy[e].angle = turn_towards_angle(enemy[e].angle,
        enemy[e].attribute [ATTRIB_SWERVER_ANGLE],
        enemy[e].attribute [ATTRIB_SWERVER_TURN]);

 if (enemy[e].attacking != ATTACK_NONE
     && (tangle / 100 == enemy[e].angle / 100
     || tangle / 100 == (enemy[e].angle + ANGLE_HALF) / 100
     || tangle / 100 == (enemy[e].angle - ANGLE_HALF) / 100))
 {
  enemy[e].attribute [ATTRIB_SWERVER_ANGLE] = grand(ANGLE_FULL + 1);
  if (grand(2) == 0)
  {
   enemy[e].attribute [ATTRIB_SWERVER_ANGLE] =
    radians_to_angle(atan2((actor[enemy[e].attacking].y - enemy[e].y), (actor[enemy[e].attacking].x - enemy[e].x)));
   if (enemy[e].attribute [ATTRIB_SWERVER_ANGLE] < 0)
    enemy[e].attribute [ATTRIB_SWERVER_ANGLE] += ANGLE_FULL;
   if (enemy[e].attribute [ATTRIB_SWERVER_ANGLE] > ANGLE_FULL)
    enemy[e].attribute [ATTRIB_SWERVER_ANGLE] -= ANGLE_FULL;
  }
 }
 
  enemy[e].x_speed = cos(angle_to_radians(enemy[e].angle)) * enemy[e].attribute [ATTRIB_SWERVER_SPEED];
  enemy[e].y_speed = sin(angle_to_radians(enemy[e].angle)) * enemy[e].attribute [ATTRIB_SWERVER_SPEED];

  if (enemy[e].x + enemy[e].x_speed <= enemy[e].edge_radius
      || enemy[e].x + enemy[e].x_speed >= arena.max_x - enemy[e].edge_radius - 4000)
  {
   enemy[e].x_speed *= -1;
   enemy[e].angle += ANGLE_HALF;
   enemy[e].attribute [ATTRIB_SWERVER_ANGLE] = grand(ANGLE_FULL + 1);
  }
  if (enemy[e].y + enemy[e].y_speed <= enemy[e].edge_radius
      || enemy[e].y + enemy[e].y_speed >= arena.max_y - enemy[e].edge_radius - 5000)
  {
   enemy[e].y_speed *= -1;
   enemy[e].angle -= ANGLE_HALF;
   enemy[e].attribute [ATTRIB_SWERVER_ANGLE] = grand(ANGLE_FULL + 1);
  }

enemy[e].x += enemy[e].x_speed;
enemy[e].y += enemy[e].y_speed;

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attribute [ATTRIB_SWERVER3_ATTACKING] = nearby_target(400000, enemy[e].x, enemy[e].y);
//  exit(2);
 }

//  if (enemy[e].attacking != ATTACK_NONE)
  if (enemy[e].attribute [ATTRIB_SWERVER3_ATTACKING] != ATTACK_NONE)
  {
//  exit(3);
//    enemy[e].attribute [ATTRIB_SWERVER3_ANGLE2] = track_target(e, enemy[e].attribute [ATTRIB_SWERVER3_ATTACKING], 16);
    enemy[e].attribute [ATTRIB_SWERVER3_ANGLE2] = turn_towards_xy(enemy[e].x, enemy[e].y, actor[enemy[e].attribute [ATTRIB_SWERVER3_ATTACKING]].x, actor[enemy[e].attribute [ATTRIB_SWERVER3_ATTACKING]].y, enemy[e].attribute [ATTRIB_SWERVER3_ANGLE2], 16);
    if (enemy[e].recycle == 0)
     standard_attack(e, enemy[e].attribute [ATTRIB_SWERVER3_ANGLE2]);
  }

 return 1;

}

int move_bouncers(int e)
{

 if (enemy[e].type == ENEMY_BOUNCER4)
 {
  if (enemy[e].counter % 16 == 0)
  {
   enemy[e].attacking = nearby_target(500000, enemy[e].x, enemy[e].y);
  }

   if (enemy[e].attacking != ATTACK_NONE)
   {
//     enemy[e].angle = track_target(e, enemy[e].attacking, 4);
//     enemy[e].attribute [ATTRIB_BOUNCER_ANGLE2] = track_target(e, enemy[e].attacking, 32);
     enemy[e].attribute [ATTRIB_BOUNCER_ANGLE2] = turn_towards_xy(enemy[e].x, enemy[e].y, actor[enemy[e].attacking].x, actor[enemy[e].attacking].y, enemy[e].attribute [ATTRIB_BOUNCER_ANGLE2], 4);
//     enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y, actor[attacking].x, actor[attacking].y, enemy[e].angle, turning);
     if (enemy[e].recycle == 0)
      standard_attack(e, enemy[e].attribute [ATTRIB_BOUNCER_ANGLE2]);
   }
  return 1;
 }

 enemy[e].angle += enemy[e].attribute [ATTRIB_BOUNCER_SPIN];
 if (enemy[e].angle < 0)
  enemy[e].angle += ANGLE_FULL;
 if (enemy[e].angle < ANGLE_FULL)
  enemy[e].angle -= ANGLE_FULL;

 enemy[e].attribute [ATTRIB_BOUNCER_SPIN] += enemy[e].attribute [ATTRIB_BOUNCER_DELTA_SPIN];
 if (enemy[e].attribute [ATTRIB_BOUNCER_SPIN] > 50)
  enemy[e].attribute [ATTRIB_BOUNCER_SPIN] = 50;
 if (enemy[e].attribute [ATTRIB_BOUNCER_SPIN] < -50)
  enemy[e].attribute [ATTRIB_BOUNCER_SPIN] = -50;
  
 if (grand(10) == 0)
 {
  enemy[e].attribute [ATTRIB_BOUNCER_DELTA_SPIN] = grand(5) - 2;
 }

 return 1;

}

void move_sun(int e)
{

     int passing_colours [5] = {TRANS_LORANGE, COLOUR_ORANGE8, COLOUR_ORANGE6, COLOUR_ORANGE4, COLOUR_ORANGE2};

     if (enemy[e].attribute [ATTRIB_SUN_COUNTER]
         - enemy[e].attribute [ATTRIB_SUN_COUNT_SPEED] <= 0)
     {

       enemy[e].attribute [ATTRIB_SUN_COUNTER] = enemy[e].attribute [ATTRIB_SUN_COUNT_START];
       enemy[e].attribute [ATTRIB_SUN_COUNT_SPEED] = 0;
       standard_attack(e, grand(ANGLE_FULL));
       if (enemy[e].type == ENEMY_SUN_GREEN)
       {
        passing_colours [0] = TRANS_LGREEN;
        passing_colours [1] = COLOUR_GREEN8;
       }
       if (enemy[e].type == ENEMY_SUN_BLUE)
       {
        passing_colours [0] = TRANS_LBLUE;
        passing_colours [1] = COLOUR_BLUE8;
       }
       place_line_burst(enemy[e].x, enemy[e].y, 0, 0, 20, 15 + grand(10), 15, 1200, 300, passing_colours);
       passing_colours [0] = TRANS_DORANGE;
       passing_colours [1] = TRANS_LORANGE;
       passing_colours [2] = TRANS_YELLOW;
       if (enemy[e].type == ENEMY_SUN_GREEN)
       {
        passing_colours [0] = TRANS_DGREEN;
        passing_colours [1] = TRANS_LGREEN;
        passing_colours [2] = TRANS_YELLOW;
       }
       if (enemy[e].type == ENEMY_SUN_BLUE)
       {
        passing_colours [0] = TRANS_DBLUE;
        passing_colours [1] = TRANS_LBLUE;
        passing_colours [2] = TRANS_WHITE;
       }
       place_explosion(enemy[e].x, enemy[e].y, 0, 0, 1000 + grand(450), passing_colours);

     }
      else
      {
       enemy[e].attribute [ATTRIB_SUN_COUNTER] -= enemy[e].attribute [ATTRIB_SUN_COUNT_SPEED];
       enemy[e].attribute [ATTRIB_SUN_COUNT_SPEED] ++;
//       if (grand(enemy[e].attribute [ATTRIB_SUN_COUNTER]) < 1000)
//        place_line_burst(enemy[e].x, enemy[e].y, 0, 0, 1, 15 + grand(10), 15, 1200, 300, passing_colours);

      }

 drag_enemy(e, 0.005);
      
}

int move_drifter(int e)
{

 if (enemy[e].x / 100000 == enemy[e].attribute [ATTRIB_DRIFTER_X_DEST] / 100000
     && enemy[e].y / 100000 == enemy[e].attribute [ATTRIB_DRIFTER_Y_DEST] / 100000)
 {
  enemy[e].attribute [ATTRIB_DRIFTER_X_DEST] = grand(arena.max_x - 40000) + 20000;
  enemy[e].attribute [ATTRIB_DRIFTER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
  enemy[e].attribute [ATTRIB_DRIFTER_DELTA_SPIN] = grand(5) - 2;
 }

 float target_angle = atan2(enemy[e].attribute [ATTRIB_DRIFTER_Y_DEST] - enemy[e].y,
                            enemy[e].attribute [ATTRIB_DRIFTER_X_DEST] - enemy[e].x);

 accelerate_enemy(e, radians_to_angle(target_angle), enemy[e].attribute [ATTRIB_DRIFTER_ACCEL]);

// enemy[e].attribute [ATTRIB_DRIFTER_LUMP_ANGLE] = target_angle;
// if (enemy[e].attribute [ATTRIB_DRIFTER_LUMP_ANGLE] / 100 != radians_to_angle(target_angle) / 100)
 {
//  enemy[e].attribute [ATTRIB_DRIFTER_LUMP_ANGLE] =
//   turn_towards_angle(enemy[e].attribute [ATTRIB_DRIFTER_LUMP_ANGLE], radians_to_angle(target_angle), 32);
 }

 enemy[e].attribute [ATTRIB_DRIFTER_SPIN] += enemy[e].attribute [ATTRIB_DRIFTER_DELTA_SPIN];
 if (enemy[e].attribute [ATTRIB_DRIFTER_SPIN] > 32)
  enemy[e].attribute [ATTRIB_DRIFTER_SPIN] = 32;
 if (enemy[e].attribute [ATTRIB_DRIFTER_SPIN] < -32)
  enemy[e].attribute [ATTRIB_DRIFTER_SPIN] = -32;
 enemy[e].attribute [ATTRIB_DRIFTER_LUMP_ANGLE] += enemy[e].attribute [ATTRIB_DRIFTER_SPIN];
 
 drag_enemy(e, 0.05);

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(enemy[e].attribute [ATTRIB_DRIFTER_RANGE], enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  enemy[e].angle = track_target(e, enemy[e].attacking, 64);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

 if (enemy[e].type != ENEMY_DRIFTER3)
   return 1;

  int colours [4], ox, oy;

  enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL] += enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL_DIR];
  if (enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL] > ANGLE_FULL)
   enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL] -= ANGLE_FULL;
  if (enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL] < 0)
   enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL] += ANGLE_FULL;

  colours [0] = COLOUR_BLUE4;

  ox = enemy[e].x + cos(angle_to_radians(enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL])) * 40000;
  oy = enemy[e].y + sin(angle_to_radians(enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL])) * 40000;

  create_bullet(BULLET_ORBITAL, ox, oy,
   0, 0,
   OWNER_ENEMY, 50, 2, 100, 0,
   0, 0, colours, 1,0,0,0,0,0);


  ox = enemy[e].x + cos(angle_to_radians(enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL] + ANGLE_HALF)) * 40000;
  oy = enemy[e].y + sin(angle_to_radians(enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL] + ANGLE_HALF)) * 40000;

  create_bullet(BULLET_ORBITAL, ox, oy,
   0, 0,
   OWNER_ENEMY, 50, 2, 100, 0,
   0, 0, colours, 1,0,0,0,0,0);

//    create_cloud(CLOUD_MED_CIRCLE, ox, oy, 0, 0,
//     enemy[e].x_speed, enemy[e].y_speed, 300, 100,
//     0, 0, 300, 10,
//     2, 0, 0, 0, colours_for_cloud);
//  simple_cloud(COLOUR_BLUE6, ox, oy, enemy[e].x_speed, enemy[e].y_speed, 200);
  
 return 1;

}



int move_wanderer(int e)
{

 if (enemy[e].x / 100000 == enemy[e].attribute [ATTRIB_WANDERER_X_DEST] / 100000
     && enemy[e].y / 100000 == enemy[e].attribute [ATTRIB_WANDERER_Y_DEST] / 100000)
 {
  enemy[e].attribute [ATTRIB_WANDERER_X_DEST] = grand(arena.max_x - 40000) + 20000;
  enemy[e].attribute [ATTRIB_WANDERER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
 }

 drag_enemy(e, 0.09);

 float target_angle = atan2(enemy[e].attribute [ATTRIB_WANDERER_Y_DEST] - enemy[e].y,
                            enemy[e].attribute [ATTRIB_WANDERER_X_DEST] - enemy[e].x);

 accelerate_enemy(e, radians_to_angle(target_angle), 150);

 int colours [4] = {COLOUR_WHITE, COLOUR_WHITE,COLOUR_WHITE,COLOUR_WHITE};

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  if (enemy[e].burst_fire > 0)
   enemy[e].angle = track_target(e, enemy[e].attacking, 4);
    else
     enemy[e].angle = track_target(e, enemy[e].attacking, 32);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

 if (enemy[e].attribute [ATTRIB_WANDERER_NEXT_MINE] == 0)
 {
  create_bullet(BULLET_PREMINE, enemy[e].x, enemy[e].y,
   enemy[e].x_speed + grand(10001) - 5000, enemy[e].y_speed + grand(10001) - 5000,
   OWNER_ENEMY, 100, 50 + grand(20), 100, 0,
   0, 0, colours, 1,0,0,0,0,0);
   enemy[e].attribute [ATTRIB_WANDERER_NEXT_MINE] = 70 + grand(50);
   enemy_sound(e, ESOUND_MINE);
  simple_cloud(COLOUR_WHITE, enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed, 700);
 }
  else
   enemy[e].attribute [ATTRIB_WANDERER_NEXT_MINE] --;
 
 return 1;

}

int move_dead_wanderer(int e)
{

 int passing_colours [5] = {TRANS_DRED, TRANS_LRED, TRANS_YELLOW, TRANS_YELLOW, TRANS_YELLOW};

 drag_enemy(e, 0.01);

 int ex, ey;

 if (grand(14) == 0)
 {
  ex = enemy[e].x + grand(18001) - 9000;
  ey = enemy[e].y + grand(18001) - 9000;
   place_explosion(ex, ey, 0, 0,
    100 + crandom(550), passing_colours);
   place_burstlet_burst(ex, ey, 0, 0, 1 + grand(5), 2, 6, 1500, 1500,
    5, passing_colours);
  enemy_soundf(e, ESOUND_EXPLODE1, 1500);
 }

 hurt_enemy(e, 5, -1, 0);

 return 1;

}

int move_jelly(int e)
{

 if (enemy[e].x / 100000 == enemy[e].attribute [ATTRIB_JELLY_X_DEST] / 100000
     && enemy[e].y / 100000 == enemy[e].attribute [ATTRIB_JELLY_Y_DEST] / 100000)
 {
  enemy[e].attribute [ATTRIB_JELLY_X_DEST] = grand(arena.max_x - 40000) + 20000;
  enemy[e].attribute [ATTRIB_JELLY_Y_DEST] = grand(arena.max_y - 40000) + 20000;
 }

 float target_angle = atan2(enemy[e].attribute [ATTRIB_JELLY_Y_DEST] - enemy[e].y,
                            enemy[e].attribute [ATTRIB_JELLY_X_DEST] - enemy[e].x);

 accelerate_enemy(e, radians_to_angle(target_angle), 50);


 drag_enemy(e, 0.15);

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  enemy[e].angle = track_target(e, enemy[e].attacking, 16);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }
  
 return 1;

}

int move_superjelly(int e)
{

//ATTRIB_JELLY_PULSE,
//ATTRIB_JELLY_PULSE_SPEED

 enemy[e].attribute [ATTRIB_JELLY_PULSE] += enemy[e].attribute [ATTRIB_JELLY_PULSE_SPEED];
 if (enemy[e].attribute [ATTRIB_JELLY_PULSE] > 256)
  enemy[e].attribute [ATTRIB_JELLY_PULSE] -= 256;

 enemy[e].attribute [ATTRIB_JELLY_PULSE_SPEED] = 4;
 if (enemy[e].armour >= eclass[ENEMY_SUPERJELLY].max_armour / 4)
  enemy[e].attribute [ATTRIB_JELLY_PULSE_SPEED] = 2;
 if (enemy[e].armour >= eclass[ENEMY_SUPERJELLY].max_armour / 2)
  enemy[e].attribute [ATTRIB_JELLY_PULSE_SPEED] = 1;


 if (enemy[e].x / 100000 == enemy[e].attribute [ATTRIB_JELLY_X_DEST] / 100000
     && enemy[e].y / 100000 == enemy[e].attribute [ATTRIB_JELLY_Y_DEST] / 100000)
 {
  enemy[e].attribute [ATTRIB_JELLY_X_DEST] = grand(arena.max_x - 40000) + 20000;
  enemy[e].attribute [ATTRIB_JELLY_Y_DEST] = grand(arena.max_y - 40000) + 20000;
 }

 float target_angle = atan2(enemy[e].attribute [ATTRIB_JELLY_Y_DEST] - enemy[e].y,
                            enemy[e].attribute [ATTRIB_JELLY_X_DEST] - enemy[e].x);

 accelerate_enemy(e, radians_to_angle(target_angle), 60);


 drag_enemy(e, 0.16);

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  enemy[e].angle = track_target(e, enemy[e].attacking, 16);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }
  
 return 1;

}

int move_hurler(int e)
{

 if (enemy[e].x / 100000 == enemy[e].attribute [ATTRIB_HURLER_X_DEST] / 100000
     && enemy[e].y / 100000 == enemy[e].attribute [ATTRIB_HURLER_Y_DEST] / 100000)
 {
  enemy[e].attribute [ATTRIB_HURLER_X_DEST] = grand(arena.max_x - 40000) + 20000;
  enemy[e].attribute [ATTRIB_HURLER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
 }

 float target_angle = atan2(enemy[e].attribute [ATTRIB_HURLER_Y_DEST] - enemy[e].y,
                            enemy[e].attribute [ATTRIB_HURLER_X_DEST] - enemy[e].x);


 switch(enemy[e].type)
 {
  default:
   accelerate_enemy(e, radians_to_angle(target_angle), 100);
   drag_enemy(e, 0.15);
   break;
  case ENEMY_HURLER2:
   accelerate_enemy(e, radians_to_angle(target_angle), 110);
   drag_enemy(e, 0.10);
   break;
 }


 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = closest_target(enemy[e].x, enemy[e].y);
//  enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  enemy[e].angle = track_target(e, enemy[e].attacking, 16);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }
  
 return 1;

}


int move_cloaker(int e)
{

 if (enemy[e].x / 100000 == enemy[e].attribute [ATTRIB_CLOAKER_X_DEST] / 100000
     && enemy[e].y / 100000 == enemy[e].attribute [ATTRIB_CLOAKER_Y_DEST] / 100000)
 {
  enemy[e].attribute [ATTRIB_CLOAKER_X_DEST] = grand(arena.max_x - 40000) + 20000;
  enemy[e].attribute [ATTRIB_CLOAKER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
 }

 float target_angle = atan2(enemy[e].attribute [ATTRIB_CLOAKER_Y_DEST] - enemy[e].y,
                            enemy[e].attribute [ATTRIB_CLOAKER_X_DEST] - enemy[e].x);

 accelerate_enemy(e, radians_to_angle(target_angle), 150);


 drag_enemy(e, 0.10);

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attribute [ATTRIB_CLOAKER_PULSE] > 0)
  enemy[e].attribute [ATTRIB_CLOAKER_PULSE] --;
  
 if (enemy[e].attacking != ATTACK_NONE)
 {
  enemy[e].angle = track_target(e, enemy[e].attacking, 16);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

 return 1;

}

int move_flakker(int e)
{

 if (enemy[e].x / 100000 == enemy[e].attribute [ATTRIB_FLAKKER_X_DEST] / 100000
     && enemy[e].y / 100000 == enemy[e].attribute [ATTRIB_FLAKKER_Y_DEST] / 100000)
 {
  enemy[e].attribute [ATTRIB_FLAKKER_X_DEST] = grand(arena.max_x - 40000) + 20000;
  enemy[e].attribute [ATTRIB_FLAKKER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
 }

 float target_angle = atan2(enemy[e].attribute [ATTRIB_FLAKKER_Y_DEST] - enemy[e].y,
                            enemy[e].attribute [ATTRIB_FLAKKER_X_DEST] - enemy[e].x);

 accelerate_enemy(e, radians_to_angle(target_angle), 100);


 drag_enemy(e, 0.07);

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(300000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  enemy[e].angle = track_target(e, enemy[e].attacking, 16);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }
  
 return 1;

}

int move_minethrower(int e)
{

 if (enemy[e].x / 100000 == enemy[e].attribute [ATTRIB_MINETHROWER_X_DEST] / 100000
     && enemy[e].y / 100000 == enemy[e].attribute [ATTRIB_MINETHROWER_Y_DEST] / 100000)
 {
  enemy[e].attribute [ATTRIB_MINETHROWER_X_DEST] = grand(arena.max_x - 40000) + 20000;
  enemy[e].attribute [ATTRIB_MINETHROWER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
 }

 drag_enemy(e, 0.10);

 float target_angle = atan2(enemy[e].attribute [ATTRIB_MINETHROWER_Y_DEST] - enemy[e].y,
                            enemy[e].attribute [ATTRIB_MINETHROWER_X_DEST] - enemy[e].x);

 accelerate_enemy(e, radians_to_angle(target_angle), 100);

 int colours [4] = {COLOUR_WHITE, COLOUR_WHITE,COLOUR_WHITE,COLOUR_WHITE};

 if (enemy[e].attribute [ATTRIB_MINETHROWER_NEXT_MINE] <= 0)
 {
  create_bullet(BULLET_PREMINE, enemy[e].x, enemy[e].y,
   enemy[e].x_speed + grand(24001) - 12000, enemy[e].y_speed + grand(24001) - 12000,
   OWNER_ENEMY, 100, 50 + grand(20), 100, 0,
   0, 0, colours, 1,0,0,0,0,0);
  enemy[e].attribute [ATTRIB_MINETHROWER_NEXT_MINE] = 50 + grand(40);
  enemy_sound(e, ESOUND_MINE);
  simple_cloud_trans(TRANS_WHITE, enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed, 500);
 }
  else
   enemy[e].attribute [ATTRIB_MINETHROWER_NEXT_MINE] --;
 
 return 1;

}

int move_minethrower2(int e)
{

 if (enemy[e].x / 100000 == enemy[e].attribute [ATTRIB_MINETHROWER_X_DEST] / 100000
     && enemy[e].y / 100000 == enemy[e].attribute [ATTRIB_MINETHROWER_Y_DEST] / 100000)
 {
  enemy[e].attribute [ATTRIB_MINETHROWER_X_DEST] = grand(arena.max_x - 40000) + 20000;
  enemy[e].attribute [ATTRIB_MINETHROWER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
 }

 drag_enemy(e, 0.12);

 float target_angle = atan2(enemy[e].attribute [ATTRIB_MINETHROWER_Y_DEST] - enemy[e].y,
                            enemy[e].attribute [ATTRIB_MINETHROWER_X_DEST] - enemy[e].x);

 accelerate_enemy(e, radians_to_angle(target_angle), 80);

 int colours [4] = {COLOUR_WHITE, COLOUR_WHITE,COLOUR_WHITE,COLOUR_WHITE};

 if (enemy[e].attribute [ATTRIB_MINETHROWER_NEXT_MINE] == 0)
 {
  create_bullet(BULLET_PRESEEKMINE, enemy[e].x, enemy[e].y,
   enemy[e].x_speed + grand(20001) - 10000, enemy[e].y_speed + grand(20001) - 10000,
   OWNER_ENEMY, 100, 50 + grand(20), 100, 0,
   0, 0, colours, 1,0,0,0,0,0);
   enemy[e].attribute [ATTRIB_MINETHROWER_NEXT_MINE] = 70 + grand(40);
   enemy_sound(e, ESOUND_MINE);
  simple_cloud_trans(TRANS_WHITE, enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed, 500);
 }
  else
   enemy[e].attribute [ATTRIB_MINETHROWER_NEXT_MINE] --;
 
 return 1;

}

// NOTE: fighters 2 & 3 also use this function
int move_fighter(int e)
{

 if (enemy[e].attribute [ATTRIB_FIGHTER_MODE] == FIGHTER_MOVE)
 {
  if (enemy[e].x / 100000 == enemy[e].attribute [ATTRIB_FIGHTER_X_DEST] / 100000
     && enemy[e].y / 100000 == enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST] / 100000)
  {
//   enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
   enemy[e].attacking = closest_target(enemy[e].x, enemy[e].y);
   if (enemy[e].attacking != ATTACK_NONE)
    enemy[e].attribute [ATTRIB_FIGHTER_MODE] = FIGHTER_ATTACK;
     else
      {
       enemy[e].attribute [ATTRIB_FIGHTER_X_DEST] = grand(arena.max_x - 40000) + 20000;
       enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
       enemy[e].attacking = ATTACK_NONE;
       enemy[e].attribute [ATTRIB_FIGHTER_MODE] = FIGHTER_MOVE;
      }
  }
//  enemy[e].angle = turn_towards_xy(enemy[e].attribute [ATTRIB_FIGHTER_X_DEST], enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST], enemy[e].attribute [ATTRIB_FIGHTER_TURN]);
  enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y,
     enemy[e].attribute [ATTRIB_FIGHTER_X_DEST], enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST],
     enemy[e].angle, enemy[e].attribute [ATTRIB_FIGHTER_TURN]);
 }
  else
  {
   if (enemy[e].attacking == ATTACK_NONE
       || actor[enemy[e].attacking].in_play == 0)
       // assumes second condition not processed if first met.
   {
    enemy[e].attribute [ATTRIB_FIGHTER_X_DEST] = grand(arena.max_x - 40000) + 20000;
    enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
    enemy[e].attribute [ATTRIB_FIGHTER_MODE] = FIGHTER_MOVE;
    enemy[e].attacking = ATTACK_NONE;
    enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y, enemy[e].attribute [ATTRIB_FIGHTER_X_DEST], enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST], enemy[e].angle, enemy[e].attribute [ATTRIB_FIGHTER_TURN]);
   }
  }
 
 drag_enemy(e, 0.02);

// should accelerate only if approximately pointing in right direction.
//  should also turn faster if not accelerating, so put this at the top
//  of the function:
// accelerate_enemy(e, radians_to_angle(enemy[e].angle), );

 int ax, ay;
 int x1, y1;
 int passing_colour [5];
 
/* if ((enemy[e].attribute [ATTRIB_FIGHTER_MODE] == FIGHTER_MOVE
      && near_angle(enemy[e].angle, radians_to_angle(atan2(enemy[e].y - enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST], enemy[e].x - enemy[e].attribute [ATTRIB_FIGHTER_X_DEST]))))
    || (enemy[e].attribute [ATTRIB_FIGHTER_MODE] == FIGHTER_ATTACK
      && near_angle(enemy[e].angle, radians_to_angle(atan2(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x)))))*/
 {
  ax = cos(angle_to_radians(enemy[e].angle)) * enemy[e].attribute [ATTRIB_FIGHTER_THRUST];
  ay = sin(angle_to_radians(enemy[e].angle)) * enemy[e].attribute [ATTRIB_FIGHTER_THRUST];
 
  enemy[e].x_speed += ax;
  enemy[e].y_speed += ay;

  if (enemy[e].type == ENEMY_FIGHTER3)
  {
//    passing_colour [0] = TRANS_GREY;
//    passing_colour [1] = TRANS_DBLUE;
//    passing_colour [2] = TRANS_LBLUE;
    passing_colour [0] = TRANS_GREY;
    passing_colour [1] = TRANS_DGREEN;
    passing_colour [2] = TRANS_LGREEN;
  }
   else
   {
    passing_colour [0] = TRANS_GREY;
    passing_colour [1] = TRANS_DRED;
    passing_colour [2] = TRANS_YELLOW;
   }

  if (grand(2) != 0)
  {
   x1 = enemy[e].x + cos(angle_to_radians(enemy[e].angle + ANGLE_HALF - ANGLE_1_EIGHTH)) * 8000;
   y1 = enemy[e].y + sin(angle_to_radians(enemy[e].angle + ANGLE_HALF - ANGLE_1_EIGHTH)) * 8000;
   place_rocket_trail(x1 + grand(3001) - 1500, y1 + grand(3001) - 1500, enemy[e].x_speed - ax*10, enemy[e].y_speed - ay*10, 140, passing_colour);
  }
  if (grand(2) != 0)
  {
   x1 = enemy[e].x + cos(angle_to_radians(enemy[e].angle + ANGLE_HALF + ANGLE_1_EIGHTH)) * 8000;
   y1 = enemy[e].y + sin(angle_to_radians(enemy[e].angle + ANGLE_HALF + ANGLE_1_EIGHTH)) * 8000;
   place_rocket_trail(x1 + grand(3001) - 1500, y1 + grand(3001) - 1500, enemy[e].x_speed - ax*10, enemy[e].y_speed - ay*10, 140, passing_colour);
  }
 }
// if (enemy[e].counter % 16 == 0)
// {
//  enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
// }

  if (enemy[e].attribute [ATTRIB_FIGHTER_MODE] == FIGHTER_ATTACK
      && enemy[e].attacking != ATTACK_NONE)
  {
    enemy[e].angle = track_target(e, enemy[e].attacking, enemy[e].attribute [ATTRIB_FIGHTER_TURN]);
//    if (enemy[e].recycle == 0
//        && (enemy[e].burst_fire > 0 ||
//         near_angle(enemy[e].angle, radians_to_angle(atan2(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x)))))
    if (enemy[e].recycle == 0
//        && (enemy[e].burst_fire > 0
//            || hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x) < 50000))
//        && (enemy[e].burst_fire > 0
            && hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x) < 90000)
    {
     standard_attack(e, enemy[e].angle);
     if (enemy[e].burst_fire == 0) // experimental!
     {
      enemy[e].attribute [ATTRIB_FIGHTER_X_DEST] = grand(arena.max_x - 40000) + 20000;
      enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
      enemy[e].attribute [ATTRIB_FIGHTER_MODE] = FIGHTER_MOVE;
      enemy[e].attacking = ATTACK_NONE;
     }
    }
/*   if (enemy[e].burst_fire == 0) // experimental!
   {
    enemy[e].attribute [ATTRIB_FIGHTER_X_DEST] = grand(arena.max_x - 40000) + 20000;
    enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST] = grand(arena.max_x - 40000) + 20000;
    enemy[e].attribute [ATTRIB_FIGHTER_MODE] = FIGHTER_MOVE;
    enemy[e].attacking = ATTACK_NONE;
   }*/
  }
   else
    if (enemy[e].recycle == 0 && enemy[e].burst_fire > 0)
//        && (enemy[e].burst_fire > 0 ||
//         near_angle(enemy[e].angle, radians_to_angle(atan2(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x)))))
    {
     standard_attack(e, enemy[e].angle);
    }
  
 return 1;

}



int move_dead_fighter(int e)
{

 drag_enemy(e, 0.02);

 int ax, ay;
 int x1, y1;
 int passing_colour [5];
 
  ax = cos(angle_to_radians(enemy[e].angle)) * 300;
  ay = sin(angle_to_radians(enemy[e].angle)) * 300;
 
  enemy[e].x_speed += ax;
  enemy[e].y_speed += ay;

   passing_colour [0] = TRANS_DGREY;
   passing_colour [1] = TRANS_DRED;
   passing_colour [2] = TRANS_LORANGE;
   if (grand(3) == 0)
    passing_colour [2] = TRANS_YELLOW;

   if (enemy[e].type == ENEMY_DEAD_FIGHTER3)
   {
    passing_colour [0] = TRANS_DGREY;
    passing_colour [1] = TRANS_DGREEN;
    passing_colour [2] = TRANS_LGREEN;
   }

 drag_enemy(e, 0.01);

 int ex, ey;

 if (grand(14) == 0)
 {
  ex = enemy[e].x + grand(7001) - 3500;
  ey = enemy[e].y + grand(7001) - 3500;
  passing_colour [2] = TRANS_YELLOW;
   place_explosion(ex, ey, enemy[e].x_speed, enemy[e].y_speed,
    100 + crandom(150), passing_colour);
   place_burstlet_burst(ex, ey, enemy[e].x_speed, enemy[e].y_speed, 1 + grand(2), 2, 6, 1000, 1000,
    5, passing_colour);
  enemy_soundf(e, ESOUND_EXPLODE1, 1500 + grand(500));
 }


  if (grand(2) != 0)
  {
   x1 = enemy[e].x + cos(angle_to_radians(enemy[e].angle + ANGLE_HALF - ANGLE_1_EIGHTH)) * 8000;
   y1 = enemy[e].y + sin(angle_to_radians(enemy[e].angle + ANGLE_HALF - ANGLE_1_EIGHTH)) * 8000;
   place_rocket_trail(x1 + grand(3001) - 1500, y1 + grand(3001) - 1500, enemy[e].x_speed - ax*10, enemy[e].y_speed - ay*10, 140, passing_colour);
  }
  if (grand(2) != 0)
  {
   x1 = enemy[e].x + cos(angle_to_radians(enemy[e].angle + ANGLE_HALF + ANGLE_1_EIGHTH)) * 8000;
   y1 = enemy[e].y + sin(angle_to_radians(enemy[e].angle + ANGLE_HALF + ANGLE_1_EIGHTH)) * 8000;
   place_rocket_trail(x1 + grand(3001) - 1500, y1 + grand(3001) - 1500, enemy[e].x_speed - ax*10, enemy[e].y_speed - ay*10, 140, passing_colour);
  }

 enemy[e].angle += grand(65);
 enemy[e].angle -= 32;
 enemy[e].angle %= ANGLE_FULL;

 hurt_enemy(e, 5 + grand(5), -1, 0);
 
 return 1;

}


int move_fighter4(int e)
{

 if (enemy[e].attribute [ATTRIB_FIGHTER_MODE] == FIGHTER_MOVE)
 {
  if (enemy[e].x / 100000 == enemy[e].attribute [ATTRIB_FIGHTER_X_DEST] / 100000
     && enemy[e].y / 100000 == enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST] / 100000)
  {
//   enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
   enemy[e].attacking = closest_target(enemy[e].x, enemy[e].y);
   if (enemy[e].attacking != ATTACK_NONE)
    enemy[e].attribute [ATTRIB_FIGHTER_MODE] = FIGHTER_ATTACK;
     else
      {
       enemy[e].attribute [ATTRIB_FIGHTER_X_DEST] = grand(arena.max_x - 40000) + 20000;
       enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
       enemy[e].attacking = ATTACK_NONE;
       enemy[e].attribute [ATTRIB_FIGHTER_MODE] = FIGHTER_MOVE;
      }
  }
  enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y,
     enemy[e].attribute [ATTRIB_FIGHTER_X_DEST], enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST],
     enemy[e].angle, enemy[e].attribute [ATTRIB_FIGHTER_TURN]);
 }
  else
  {
   if (enemy[e].attacking == ATTACK_NONE
       || actor[enemy[e].attacking].in_play == 0)
       // assumes second condition not processed if first met.
   {
    enemy[e].attribute [ATTRIB_FIGHTER_X_DEST] = grand(arena.max_x - 40000) + 20000;
    enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
    enemy[e].attribute [ATTRIB_FIGHTER_MODE] = FIGHTER_MOVE;
    enemy[e].attacking = ATTACK_NONE;
    enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y, enemy[e].attribute [ATTRIB_FIGHTER_X_DEST], enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST], enemy[e].angle, enemy[e].attribute [ATTRIB_FIGHTER_TURN]);
   }
  }
 
 drag_enemy(e, 0.04);

 int ax, ay;
 int x1, y1;
 int passing_colour [5];
 
/* if ((enemy[e].attribute [ATTRIB_FIGHTER_MODE] == FIGHTER_MOVE
      && near_angle(enemy[e].angle, radians_to_angle(atan2(enemy[e].y - enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST], enemy[e].x - enemy[e].attribute [ATTRIB_FIGHTER_X_DEST]))))
    || (enemy[e].attribute [ATTRIB_FIGHTER_MODE] == FIGHTER_ATTACK
      && near_angle(enemy[e].angle, radians_to_angle(atan2(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x)))))*/
 {
  ax = cos(angle_to_radians(enemy[e].angle)) * enemy[e].attribute [ATTRIB_FIGHTER_THRUST];
  ay = sin(angle_to_radians(enemy[e].angle)) * enemy[e].attribute [ATTRIB_FIGHTER_THRUST];
 
  enemy[e].x_speed += ax;
  enemy[e].y_speed += ay;

    passing_colour [0] = TRANS_DBLUE;
    passing_colour [1] = TRANS_LBLUE;
    passing_colour [2] = TRANS_WHITE;

  if (grand(2) != 0)
  {
   x1 = enemy[e].x + cos(angle_to_radians(enemy[e].angle + ANGLE_HALF - ANGLE_1_SIXTEENTH)) * 29000;
   y1 = enemy[e].y + sin(angle_to_radians(enemy[e].angle + ANGLE_HALF - ANGLE_1_SIXTEENTH)) * 29000;
   place_rocket_trail(x1 + grand(3001) - 1500, y1 + grand(3001) - 1500, enemy[e].x_speed - ax*45, enemy[e].y_speed - ay*45, 140, passing_colour);
  }
  if (grand(2) != 0)
  {
   x1 = enemy[e].x + cos(angle_to_radians(enemy[e].angle + ANGLE_HALF + ANGLE_1_SIXTEENTH)) * 29000;
   y1 = enemy[e].y + sin(angle_to_radians(enemy[e].angle + ANGLE_HALF + ANGLE_1_SIXTEENTH)) * 29000;
   place_rocket_trail(x1 + grand(3001) - 1500, y1 + grand(3001) - 1500, enemy[e].x_speed - ax*45, enemy[e].y_speed - ay*45, 140, passing_colour);
  }
 }

  if (enemy[e].attribute [ATTRIB_FIGHTER_MODE] == FIGHTER_ATTACK
      && enemy[e].attacking != ATTACK_NONE)
  {
    enemy[e].angle = track_target(e, enemy[e].attacking, enemy[e].attribute [ATTRIB_FIGHTER_TURN]);
    if (enemy[e].recycle == 0
            && hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x) < 250000)
    {
     standard_attack(e, enemy[e].angle);
//     if (enemy[e].burst_fire == 0) // experimental!
     {
      enemy[e].attribute [ATTRIB_FIGHTER_X_DEST] = grand(arena.max_x - 40000) + 20000;
      enemy[e].attribute [ATTRIB_FIGHTER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
      enemy[e].attribute [ATTRIB_FIGHTER_MODE] = FIGHTER_MOVE;
      enemy[e].attacking = ATTACK_NONE;
     }
    }
  }
   else
    if (enemy[e].recycle == 0)// && enemy[e].burst_fire > 0)
    {
     standard_attack(e, enemy[e].angle);
    }

 move_enemy(e);
// Needs to be here so that the turrets are placed correctly

 int i = enemy[e].attribute [ATTRIB_FIGHTER4_TURRET1];
 if (i != -1)
 {
  enemy[i].x = enemy[e].x + cos(angle_to_radians(enemy[e].angle + ANGLE_HALF - ANGLE_1_EIGHTH)) * 20000;
  enemy[i].y = enemy[e].y + sin(angle_to_radians(enemy[e].angle + ANGLE_HALF - ANGLE_1_EIGHTH)) * 20000;
 }
 int j = enemy[e].attribute [ATTRIB_FIGHTER4_TURRET2];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x + cos(angle_to_radians(enemy[e].angle + ANGLE_HALF + ANGLE_1_EIGHTH)) * 20000;
  enemy[j].y = enemy[e].y + sin(angle_to_radians(enemy[e].angle + ANGLE_HALF + ANGLE_1_EIGHTH)) * 20000;
  if (i != -1)
  {
   if (enemy[i].recycle <= 1)
    enemy[j].recycle = 6; // staggered.
  }
 }

 
 return 1;

}

int move_fighter4_turret(int e)
{

 if (enemy[e].attribute [ATTRIB_FI4TURRET_MAIN_DEAD] == 1)
 {
  hurt_enemy(e, 99999, OWNER_ENEMY, 0);
  return -1;
 }

 enemy[e].x_speed = enemy[enemy[e].attribute [ATTRIB_FI4TURRET_MAIN]].x_speed;
 enemy[e].y_speed = enemy[enemy[e].attribute [ATTRIB_FI4TURRET_MAIN]].y_speed;;

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(300000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  enemy[e].angle = track_target(e, enemy[e].attacking, 16);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

 return 1;

}

int move_hive(int e)
{
 enemy[e].x_speed = 0;
 enemy[e].y_speed = 0;

 if (enemy[e].counter % 16 == 0)
 {
  if (enemy[e].attribute [ATTRIB_HIVE_ATTACK13] == ATTACK_NONE
      || enemy[e].attribute [ATTRIB_HIVE_ATTACK24] == ATTACK_NONE)
  {
   if (actor[0].in_play == 1)
   {
    if (hypot(actor[0].y - enemy[e].y, actor[0].x - enemy[e].x) < HIVE_MIN_RANGE)
    {
     enemy[e].attribute [ATTRIB_HIVE_ATTACK13] = 0;
     if (actor[1].in_play == 0)
      enemy[e].attribute [ATTRIB_HIVE_ATTACK24] = 0;
       else
       {
        if (hypot(actor[1].y - enemy[e].y, actor[1].x - enemy[e].x) < HIVE_MAX_RANGE)
         enemy[e].attribute [ATTRIB_HIVE_ATTACK24] = 1;
       }
    }
   }
   if (actor[1].in_play == 1)
   {
    if (hypot(actor[1].y - enemy[e].y, actor[1].x - enemy[e].x) < HIVE_MIN_RANGE)
    {
     enemy[e].attribute [ATTRIB_HIVE_ATTACK24] = 1;
     if (actor[0].in_play == 0)
      enemy[e].attribute [ATTRIB_HIVE_ATTACK13] = 1;
       else
       {
        if (hypot(actor[0].y - enemy[e].y, actor[0].x - enemy[e].x) < HIVE_MAX_RANGE)
         enemy[e].attribute [ATTRIB_HIVE_ATTACK13] = 0;
       }
    }
   }
  }
   else
   {
    if (actor[0].in_play == 0 && actor[1].in_play == 0)
    {
     enemy[e].attribute [ATTRIB_HIVE_ATTACK13] = ATTACK_NONE;
     enemy[e].attribute [ATTRIB_HIVE_ATTACK24] = ATTACK_NONE;
    }
     else
     {
      if (actor[0].in_play == 1)
      {
       if (hypot(actor[0].y - enemy[e].y, actor[0].x - enemy[e].x) > HIVE_MAX_RANGE)
       {
        if (enemy[e].attribute [ATTRIB_HIVE_ATTACK13] == 0)
         enemy[e].attribute [ATTRIB_HIVE_ATTACK13] = ATTACK_NONE;
        if (enemy[e].attribute [ATTRIB_HIVE_ATTACK24] == 0)
         enemy[e].attribute [ATTRIB_HIVE_ATTACK24] = ATTACK_NONE;
       }
      }
      if (actor[1].in_play == 1)
      {
       if (hypot(actor[1].y - enemy[e].y, actor[1].x - enemy[e].x) > HIVE_MAX_RANGE)
       {
        if (enemy[e].attribute [ATTRIB_HIVE_ATTACK13] == 1)
         enemy[e].attribute [ATTRIB_HIVE_ATTACK13] = ATTACK_NONE;
        if (enemy[e].attribute [ATTRIB_HIVE_ATTACK24] == 1)
         enemy[e].attribute [ATTRIB_HIVE_ATTACK24] = ATTACK_NONE;
       }
      }
     }

   }
  if (enemy[e].attribute [ATTRIB_HIVE_ATTACK13] == 1
      && actor[0].in_play == 1
      && hypot(actor[0].y - enemy[e].y, actor[0].x - enemy[e].x) < HIVE_MAX_RANGE)
         enemy[e].attribute [ATTRIB_HIVE_ATTACK13] = 0;
  if (enemy[e].attribute [ATTRIB_HIVE_ATTACK24] == 0
      && actor[1].in_play == 1
      && hypot(actor[1].y - enemy[e].y, actor[1].x - enemy[e].x) < HIVE_MAX_RANGE)
         enemy[e].attribute [ATTRIB_HIVE_ATTACK13] = 1;
 }

// enemy[e].attribute [ATTRIB_HIVE_ATTACK13] = 0;
// enemy[e].attribute [ATTRIB_HIVE_ATTACK24] = 0;


 if (counter % 128 == 0)
 {
  int i;
  int empty_bee_number [5] = {0,0,0,0,0};
  int number_of_bees = 0;
  int bee_number = 0;
  int special [10];

  for (i = 0; i < NO_ENEMIES; i ++)
  {
   if (enemy[i].type == ENEMY_BEE)
   {
    number_of_bees ++;
    empty_bee_number [enemy[i].attribute [ATTRIB_BEE_NUMBER]] = 1;
   }
  }
  
  if (number_of_bees < 5)
  {
   for (i = 0; i < 5; i ++)
   {
    if (i == 5)
     return 0; // should never happen
    if (empty_bee_number [i] == 0)
    {
      bee_number = i;
      break;
    }
   }
   special [0] = e;
   special [1] = bee_number;

   create_enemy(ENEMY_BEE, SUBTYPE_NONE, enemy[e].x, enemy[e].y,
    0, 0, grand(ANGLE_FULL), 0, special, TARGET_NO);
   simple_cloud_trans(TRANS_YELLOW, enemy[e].x, enemy[e].y, 0, 0, 500);
  }
 }

 
 return 1;
}

int move_bee(int e)
{
/*
  enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y,
     enemy[e].attribute [ATTRIB_BEE_X_DEST], enemy[e].attribute [ATTRIB_BEE_Y_DEST],
     enemy[e].angle, 16); //enemy[e].attribute [ATTRIB_BEE_TURN]);
*/

 int bee_index;

 if (enemy[e].attribute [ATTRIB_BEE_NUMBER] % 2 == 1)
     {
      bee_index = ATTRIB_HIVE_ATTACK13;
     }
      else bee_index = ATTRIB_HIVE_ATTACK24;
//  enemy[e].attribute [ATTRIB_BEE_MODE] = FIGHTER_ATTACK;

// if (enemy[e].attribute [ATTRIB_BEE_MODE] == FIGHTER_MOVE
//     || enemy[e].attribute [ATTRIB_BEE_MODE] == BEE_RESTING)
 if (TRUE)
 {
  if (enemy[e].x / 100000 == enemy[e].attribute [ATTRIB_BEE_X_DEST] / 100000
     && enemy[e].y / 100000 == enemy[e].attribute [ATTRIB_BEE_Y_DEST] / 100000)
  {
   if (enemy[e].attribute [ATTRIB_BEE_HIVE] == -1)
   {
    enemy[e].attacking = closest_target(enemy[e].x, enemy[e].y);
    if (enemy[e].attacking != ATTACK_NONE)
     enemy[e].attribute [ATTRIB_BEE_MODE] = FIGHTER_ATTACK;
   }
    else
    {
     enemy[e].attacking = enemy[enemy[e].attribute [ATTRIB_BEE_HIVE]].attribute [bee_index];
     if (enemy[e].attacking != ATTACK_NONE)
      enemy[e].attribute [ATTRIB_BEE_MODE] = FIGHTER_ATTACK;
       else
        {
//         enemy[e].attribute [ATTRIB_BEE_X_DEST] = ///grand(arena.max_x - 40000) + 20000;
//         enemy[e].attribute [ATTRIB_BEE_Y_DEST] = grand(arena.max_l - 40000) + 20000;
         enemy[e].attacking = ATTACK_NONE;
         enemy[e].attribute [ATTRIB_BEE_MODE] = BEE_RESTING;
        }

    }
// turn was here?
  }
//  enemy[e].angle = turn_towards_xy(enemy[e].attribute [ATTRIB_BEE_X_DEST], enemy[e].attribute [ATTRIB_BEE_Y_DEST], enemy[e].attribute [ATTRIB_BEE_TURN]);
 }
  else
  {
   if (enemy[e].attacking == ATTACK_NONE
       || actor[enemy[e].attacking].in_play == 0)
       // assumes second condition not processed if first met.
   {
    enemy[e].attribute [ATTRIB_BEE_MODE] = BEE_RESTING;
    enemy[e].attacking = ATTACK_NONE;
   }
  }

 if (enemy[e].attribute [ATTRIB_BEE_MODE] == BEE_RESTING)
 {
  if (enemy[e].attribute [ATTRIB_BEE_HIVE] != -1)
  {
    enemy[e].attribute [ATTRIB_BEE_X_DEST] = enemy[enemy[e].attribute [ATTRIB_BEE_HIVE]].x + cos(angle_to_radians((counter * 4) + enemy[e].attribute [ATTRIB_BEE_NUMBER] * ANGLE_QUARTER)) * 50000;
    enemy[e].attribute [ATTRIB_BEE_Y_DEST] = enemy[enemy[e].attribute [ATTRIB_BEE_HIVE]].y + sin(angle_to_radians((counter * 4) + enemy[e].attribute [ATTRIB_BEE_NUMBER] * ANGLE_QUARTER)) * 50000;
//    enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y, enemy[e].attribute [ATTRIB_BEE_X_DEST], enemy[e].attribute [ATTRIB_BEE_Y_DEST], enemy[e].angle, 16);
  }
 }

 drag_enemy(e, 0.03);

 int ax, ay;
// int x1, y1;
// int passing_colour [5];
 
/* if ((enemy[e].attribute [ATTRIB_BEE_MODE] == FIGHTER_MOVE
      && near_angle(enemy[e].angle, radians_to_angle(atan2(enemy[e].y - enemy[e].attribute [ATTRIB_BEE_Y_DEST], enemy[e].x - enemy[e].attribute [ATTRIB_BEE_X_DEST]))))
    || (enemy[e].attribute [ATTRIB_BEE_MODE] == FIGHTER_ATTACK
      && near_angle(enemy[e].angle, radians_to_angle(atan2(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x)))))*/
// {
//  ax = cos(angle_to_radians(enemy[e].angle)) * enemy[e].attribute [ATTRIB_BEE_THRUST];
//  ay = sin(angle_to_radians(enemy[e].angle)) * enemy[e].attribute [ATTRIB_BEE_THRUST];
  ax = cos(angle_to_radians(enemy[e].angle)) * 150;
  ay = sin(angle_to_radians(enemy[e].angle)) * 150;
 
  enemy[e].x_speed += ax;
  enemy[e].y_speed += ay;

  if (enemy[e].attribute [ATTRIB_BEE_MODE] == FIGHTER_ATTACK
      && enemy[e].attacking != ATTACK_NONE)
  {
//    exit(3);
    enemy[e].angle = track_target(e, enemy[e].attacking, 16);
    if (enemy[e].recycle == 0
            && hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x) < 90000)
    {
     standard_attack(e, enemy[e].angle);
     if (enemy[e].burst_fire == 0) // experimental!
     {
      enemy[e].attribute [ATTRIB_BEE_X_DEST] = actor[enemy[e].attacking].x + grand(150000) - 75000;
      enemy[e].attribute [ATTRIB_BEE_Y_DEST] = actor[enemy[e].attacking].y + grand(150000) - 75000;
//      enemy[e].attribute [ATTRIB_BEE_X_DEST] = actor[enemy[e].attacking].x + grand(1500) - 750;
//      enemy[e].attribute [ATTRIB_BEE_Y_DEST] = actor[enemy[e].attacking].y + grand(1500) - 750;
      if (enemy[e].attribute [ATTRIB_BEE_X_DEST] < 20000)
       enemy[e].attribute [ATTRIB_BEE_X_DEST] = 20000;
      if (enemy[e].attribute [ATTRIB_BEE_X_DEST] > arena.max_x - 20000)
       enemy[e].attribute [ATTRIB_BEE_X_DEST] = arena.max_x - 20000;
      if (enemy[e].attribute [ATTRIB_BEE_Y_DEST] < 20000)
       enemy[e].attribute [ATTRIB_BEE_Y_DEST] = 20000;
      if (enemy[e].attribute [ATTRIB_BEE_Y_DEST] > arena.max_y - 20000)
       enemy[e].attribute [ATTRIB_BEE_Y_DEST] = arena.max_y - 20000;
      enemy[e].attribute [ATTRIB_BEE_MODE] = FIGHTER_MOVE;
      enemy[e].attacking = ATTACK_NONE;
     }
    }
  }
   else
   {
    if (enemy[e].recycle == 0 && enemy[e].burst_fire > 0)
    {
     standard_attack(e, enemy[e].angle);
    }
     else
       enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y,
        enemy[e].attribute [ATTRIB_BEE_X_DEST], enemy[e].attribute [ATTRIB_BEE_Y_DEST],
        enemy[e].angle, 16); //enemy[e].attribute [ATTRIB_BEE_TURN]);

   }
 return 1;

}



int move_torper2(int e)
{

 if (enemy[e].x / 100000 == enemy[e].attribute [ATTRIB_TORPER2_X_DEST] / 100000
     && enemy[e].y / 100000 == enemy[e].attribute [ATTRIB_TORPER2_Y_DEST] / 100000)
 {
  enemy[e].attribute [ATTRIB_TORPER2_X_DEST] = grand(arena.max_x - 40000) + 20000;
  enemy[e].attribute [ATTRIB_TORPER2_Y_DEST] = grand(arena.max_y - 40000) + 20000;
 }

 int target_angle;// = radians_to_angle(atan2(enemy[e].attribute [ATTRIB_TORPER2_Y_DEST] - enemy[e].y,
//                            enemy[e].attribute [ATTRIB_TORPER2_X_DEST] - enemy[e].x));

// accelerate_enemy(e, radians_to_angle(target_angle), 100);

  enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y,
     enemy[e].attribute [ATTRIB_TORPER2_X_DEST], enemy[e].attribute [ATTRIB_TORPER2_Y_DEST],
     enemy[e].angle, 16);

  int ax = cos(angle_to_radians(enemy[e].angle)) * 140;
  int ay = sin(angle_to_radians(enemy[e].angle)) * 140;
 
  enemy[e].x_speed += ax;
  enemy[e].y_speed += ay;

  int passing_colour [5];

    passing_colour [0] = TRANS_DBLUE;
    passing_colour [1] = TRANS_PURPLE;
    passing_colour [2] = TRANS_WHITE;

   int x1 = enemy[e].x + cos(angle_to_radians(enemy[e].angle + ANGLE_HALF)) * 8000;
   int y1 = enemy[e].y + sin(angle_to_radians(enemy[e].angle + ANGLE_HALF)) * 8000;
   place_rocket_trail(x1 + grand(3001) - 1500, y1 + grand(3001) - 1500, enemy[e].x_speed - ax*15, enemy[e].y_speed - ay*15, 140, passing_colour);

//  if (grand(2) != 0)
//  {
//   x1 = enemy[e].x + cos(angle_to_radians(enemy[e].angle + ANGLE_HALF + ANGLE_1_EIGHTH)) * 8000;
//   y1 = enemy[e].y + sin(angle_to_radians(enemy[e].angle + ANGLE_HALF + ANGLE_1_EIGHTH)) * 8000;
//   place_rocket_trail(x1 + grand(3001) - 1500, y1 + grand(3001) - 1500, enemy[e].x_speed - ax*10, enemy[e].y_speed - ay*10, 140, passing_colour);
//  }

 drag_enemy(e, 0.04);

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
//  enemy[e].angle = track_target(e, enemy[e].attacking, 64);
  if (enemy[e].recycle == 0)
  {
   target_angle = radians_to_angle(atan2(actor[enemy[e].attacking].y - enemy[e].y,
                            actor[enemy[e].attacking].x - enemy[e].x));
   if (enemy[e].burst_fire % 2 == 0)
   {
    standard_attack(e, target_angle + 64 - grand(128));
   }
    else
     standard_attack(e, target_angle);
  }
 }

 return 1;

}

int move_hurler3(int e)
{

 if (enemy[e].x / 100000 == enemy[e].attribute [ATTRIB_HURLER3_X_DEST] / 100000
     && enemy[e].y / 100000 == enemy[e].attribute [ATTRIB_HURLER3_Y_DEST] / 100000)
 {
  enemy[e].attribute [ATTRIB_HURLER3_X_DEST] = grand(arena.max_x - 40000) + 20000;
  enemy[e].attribute [ATTRIB_HURLER3_Y_DEST] = grand(arena.max_y - 40000) + 20000;
 }

 int target_angle;// = radians_to_angle(atan2(enemy[e].attribute [ATTRIB_TORPER2_Y_DEST] - enemy[e].y,
//                            enemy[e].attribute [ATTRIB_TORPER2_X_DEST] - enemy[e].x));

// accelerate_enemy(e, radians_to_angle(target_angle), 100);

  enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y,
     enemy[e].attribute [ATTRIB_HURLER3_X_DEST], enemy[e].attribute [ATTRIB_HURLER3_Y_DEST],
     enemy[e].angle, 16);

  int ax = cos(angle_to_radians(enemy[e].angle)) * 110;
  int ay = sin(angle_to_radians(enemy[e].angle)) * 110;
 
  enemy[e].x_speed += ax;
  enemy[e].y_speed += ay;

  int passing_colour [5];

    passing_colour [0] = TRANS_DGREEN;
    passing_colour [1] = TRANS_LGREEN;
    passing_colour [2] = TRANS_YELLOW;

//   int x1 = enemy[e].x + cos(angle_to_radians(enemy[e].angle + ANGLE_HALF)) * 8000;
//   int y1 = enemy[e].y + sin(angle_to_radians(enemy[e].angle + ANGLE_HALF)) * 8000;
//   t x1 = enemy[e].x + cos(angle_to_radians(enemy[e].angle + ANGLE_HALF)) * 8000;
//   int y1 = enemy[e].y + sin(angle_to_radians(enemy[e].angle + ANGLE_HALF)) * 8000;
//   place_rocket_trail(x1 + grand(3001) - 1500, y1 + grand(3001) - 1500, enemy[e].x_speed - ax*15, enemy[e].y_speed - ay*15, 140, passing_colour);
  int x1, y1;


  if (grand(2) != 0)
  {
   x1 = enemy[e].x + cos(angle_to_radians(enemy[e].angle + 190)) * 16000;
   y1 = enemy[e].y + sin(angle_to_radians(enemy[e].angle + 190)) * 16000;
   place_rocket_trail(x1 + grand(3001) - 1500, y1 + grand(3001) - 1500, enemy[e].x_speed - ax*15, enemy[e].y_speed - ay*15, 140, passing_colour);
  }
  if (grand(2) != 0)
  {
   x1 = enemy[e].x + cos(angle_to_radians(enemy[e].angle - 190)) * 16000;
   y1 = enemy[e].y + sin(angle_to_radians(enemy[e].angle - 190)) * 16000;
   place_rocket_trail(x1 + grand(3001) - 1500, y1 + grand(3001) - 1500, enemy[e].x_speed - ax*15, enemy[e].y_speed - ay*15, 140, passing_colour);
  }
//  if (grand(2) != 0)
//  {
//   x1 = enemy[e].x + cos(angle_to_radians(enemy[e].angle + ANGLE_HALF + ANGLE_1_EIGHTH)) * 8000;
//   y1 = enemy[e].y + sin(angle_to_radians(enemy[e].angle + ANGLE_HALF + ANGLE_1_EIGHTH)) * 8000;
//   place_rocket_trail(x1 + grand(3001) - 1500, y1 + grand(3001) - 1500, enemy[e].x_speed - ax*10, enemy[e].y_speed - ay*10, 140, passing_colour);
//  }

 drag_enemy(e, 0.04);

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = closest_target(enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
//  enemy[e].angle = track_target(e, enemy[e].attacking, 64);
  if (enemy[e].recycle == 0)
  {
   target_angle = radians_to_angle(atan2(actor[enemy[e].attacking].y - enemy[e].y,
                            actor[enemy[e].attacking].x - enemy[e].x));
   target_angle += grand(128) - 64;
   standard_attack(e, target_angle);
  }
 }

 return 1;

}



int move_blaster(int e)
{

 if (grand(15) == 0)
 {
  accelerate_enemy(e, grand(ANGLE_FULL + 1), 1000 + grand(1000));
 }

 drag_enemy(e, 0.04);


 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(150000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
// enemy[e].angle = track_target(e, enemy[e].attacking, 16);
  if (enemy[e].recycle == 0)
   standard_attack(e, grand(ANGLE_FULL));
 }

 move_enemy(e);
// Needs to be here so that the turrets are placed correctly

 int j = enemy[e].attribute [ATTRIB_BLASTER_TURRET1];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x - 17*GRAIN;
  enemy[j].y = enemy[e].y - 18*GRAIN;
 }
 j = enemy[e].attribute [ATTRIB_BLASTER_TURRET2];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x + 17 * GRAIN;
  enemy[j].y = enemy[e].y - 18 * GRAIN;
 }
 
 return 1;

}

int move_blaster_turret(int e)
{

 if (enemy[e].attribute [ATTRIB_BTURRET_MAIN_DEAD] == 1)
 {
  hurt_enemy(e, 99999, OWNER_ENEMY, 0);
  return -1;
 }

 enemy[e].x_speed = enemy[enemy[e].attribute [ATTRIB_BTURRET_MAIN]].x_speed;;
 enemy[e].y_speed = enemy[enemy[e].attribute [ATTRIB_BTURRET_MAIN]].y_speed;;

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(450000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  enemy[e].angle = track_target(e, enemy[e].attacking, 16);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

 return 1;

}



int move_boss1(int e)
{

 if (counter % 64 == 0)
    enemy_sound(e, ESOUND_BOSS1);


 if (enemy[e].x / 150000 == enemy[e].attribute [ATTRIB_BOSS1_X_DEST] / 150000
     && enemy[e].y / 150000 == enemy[e].attribute [ATTRIB_BOSS1_Y_DEST] / 150000)
 {
  enemy[e].attribute [ATTRIB_BOSS1_X_DEST] = grand(arena.max_x - 300000) + 150000;
  enemy[e].attribute [ATTRIB_BOSS1_Y_DEST] = grand(arena.max_y - 300000) + 150000;
 }

 drag_enemy(e, 0.10);

 float target_angle = atan2(enemy[e].attribute [ATTRIB_BOSS1_Y_DEST] - enemy[e].y,
                            enemy[e].attribute [ATTRIB_BOSS1_X_DEST] - enemy[e].x);

 accelerate_enemy(e, radians_to_angle(target_angle), 100);


 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(500000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  if (enemy[e].recycle == 0)
   standard_attack(e, grand(ANGLE_FULL));
 }

 move_enemy(e);
// Needs to be here so that the turrets are placed correctly

 int j = enemy[e].attribute [ATTRIB_BOSS1_TURRET1];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x - 58 * GRAIN;
  enemy[j].y = enemy[e].y + 45 * GRAIN;
 }
 j = enemy[e].attribute [ATTRIB_BOSS1_TURRET2];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x + 58 * GRAIN;
  enemy[j].y = enemy[e].y + 45 * GRAIN;
 }
 j = enemy[e].attribute [ATTRIB_BOSS1_TURRET3];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x - 0 * GRAIN;
  enemy[j].y = enemy[e].y - 68 * GRAIN;
 }
/* j = enemy[e].attribute [ATTRIB_BOSS1_TURRET4];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x + 39 * GRAIN;
  enemy[j].y = enemy[e].y - 10 * GRAIN;
 }*/
/* j = enemy[e].attribute [ATTRIB_BOSS1_TURRET3];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x - 39 * GRAIN;
  enemy[j].y = enemy[e].y - 21 * GRAIN;
 }
 j = enemy[e].attribute [ATTRIB_BOSS1_TURRET4];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x + 39 * GRAIN;
  enemy[j].y = enemy[e].y - 21 * GRAIN;
 }
*/
 return 1;

}

int move_boss1_turret(int e)
{

 if (enemy[e].attribute [ATTRIB_BO1TURRET_MAIN_DEAD] == 1)
 {
  hurt_enemy(e, 99999, OWNER_ENEMY, 0);
  return -1;
 }

 enemy[e].x_speed = enemy[enemy[e].attribute [ATTRIB_BO1TURRET_MAIN]].x_speed;;
 enemy[e].y_speed = enemy[enemy[e].attribute [ATTRIB_BO1TURRET_MAIN]].y_speed;;

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(450000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  if (enemy[e].counter % 4 == 0)
   enemy[e].angle = track_target(e, enemy[e].attacking, 16);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

 return 1;

}



int move_boss2(int e)
{

 if (enemy[e].x / 150000 == enemy[e].attribute [ATTRIB_BOSS2_X_DEST] / 150000
     && enemy[e].y / 150000 == enemy[e].attribute [ATTRIB_BOSS2_Y_DEST] / 150000)
 {
  enemy[e].attribute [ATTRIB_BOSS2_X_DEST] = grand(arena.max_x - 400000) + 200000;
  enemy[e].attribute [ATTRIB_BOSS2_Y_DEST] = grand(arena.max_y - 400000) + 200000;
 }

 drag_enemy(e, 0.07);

 float target_angle = atan2(enemy[e].attribute [ATTRIB_BOSS2_Y_DEST] - enemy[e].y,
                            enemy[e].attribute [ATTRIB_BOSS2_X_DEST] - enemy[e].x);

 accelerate_enemy(e, radians_to_angle(target_angle), 100);


 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(500000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  if (enemy[e].recycle == 0)
   standard_attack(e, grand(ANGLE_FULL));
 }

 move_enemy(e);
// Needs to be here so that the turrets are placed correctly

 int spin_max = 120;
 enemy[e].attribute [ATTRIB_BOSS2_TURRETS_LEFT] = 0;

 int j = enemy[e].attribute [ATTRIB_BOSS2_TURRET1];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x + cos(angle_to_radians(enemy[e].angle)) * 90000;
  enemy[j].y = enemy[e].y + sin(angle_to_radians(enemy[e].angle)) * 90000;
  spin_max -= 10;
  enemy[e].attribute [ATTRIB_BOSS2_TURRETS_LEFT] ++;
 }
 j = enemy[e].attribute [ATTRIB_BOSS2_TURRET2];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x + cos(angle_to_radians(enemy[e].angle + ANGLE_QUARTER)) * 90000;
  enemy[j].y = enemy[e].y + sin(angle_to_radians(enemy[e].angle + ANGLE_QUARTER)) * 90000;
  spin_max -= 10;
  enemy[e].attribute [ATTRIB_BOSS2_TURRETS_LEFT] ++;
 }
 j = enemy[e].attribute [ATTRIB_BOSS2_TURRET3];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x + cos(angle_to_radians(enemy[e].angle - ANGLE_QUARTER)) * 90000;
  enemy[j].y = enemy[e].y + sin(angle_to_radians(enemy[e].angle - ANGLE_QUARTER)) * 90000;
  spin_max -= 10;
  enemy[e].attribute [ATTRIB_BOSS2_TURRETS_LEFT] ++;
 }
 j = enemy[e].attribute [ATTRIB_BOSS2_TURRET4];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x + cos(angle_to_radians(enemy[e].angle + ANGLE_HALF)) * 90000;
  enemy[j].y = enemy[e].y + sin(angle_to_radians(enemy[e].angle + ANGLE_HALF)) * 90000;
  spin_max -= 10;
  enemy[e].attribute [ATTRIB_BOSS2_TURRETS_LEFT] ++;
 }

 if (abs(enemy[e].attribute [ATTRIB_BOSS2_SPIN]) < spin_max)
  enemy[e].attribute [ATTRIB_BOSS2_SPIN] += enemy[e].attribute [ATTRIB_BOSS2_DELTA_SPIN];
 enemy[e].angle += enemy[e].attribute [ATTRIB_BOSS2_SPIN] / 10;
 

 if (counter % 64 == 0)
    enemy_soundf(e, ESOUND_BOSS2, 1000 - enemy[e].attribute [ATTRIB_BOSS2_TURRETS_LEFT] * 50);

 if (enemy[e].attribute [ATTRIB_BOSS2_TURRET_NOISE] > 0)
 {
  enemy[e].attribute [ATTRIB_BOSS2_TURRET_NOISE] --;
 } // makes sure only one turret makes noise each time they fire - otherwise
   //    it's too loud
 
 return 1;

}

int move_boss2_turret(int e)
{

 if (enemy[e].attribute [ATTRIB_BO2TURRET_MAIN_DEAD] == 1)
 {
  hurt_enemy(e, 99999, OWNER_ENEMY, 0);
  return -1;
 }

 enemy[e].x_speed = enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].x_speed;
 enemy[e].y_speed = enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].y_speed;

/* if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(450000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  if (enemy[e].counter % 4 == 0)
   enemy[e].angle = track_target(e, enemy[e].attacking, 16);*/
  if (enemy[e].recycle == 0)
  {
   enemy[e].angle = radians_to_angle(atan2(enemy[e].y - enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].y,
                  enemy[e].x - enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].x));
   standard_attack(e, enemy[e].angle);
   if (enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].attribute [ATTRIB_BOSS2_TURRET_NOISE] == 0)
   {
    enemy_soundf(e, ESOUND_PLASMA, 600);
    enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].attribute [ATTRIB_BOSS2_TURRET_NOISE] = 3;
   }
  }
// }

 return 1;

}


int move_boss3(int e)
{

// if (counter % 64 == 0)
//    enemy_sound(e, ESOUND_BOSS3);
 if (counter % 2 == 0)
//    enemy_soundf(e, ESOUND_BOSS3_1 + grand(3), 600 + grand(1500));
//     bullet_sound(b, BSOUND_ORBITAL);
  enemy_soundf(e, ESOUND_ORBITAL, 700 + grand(400));
/* if (enemy[e].x / 150000 == enemy[e].attribute [ATTRIB_BOSS2_X_DEST] / 150000
     && enemy[e].y / 150000 == enemy[e].attribute [ATTRIB_BOSS2_Y_DEST] / 150000)
 {
  enemy[e].attribute [ATTRIB_BOSS2_X_DEST] = grand(arena.max_x - 400000) + 200000;
  enemy[e].attribute [ATTRIB_BOSS2_Y_DEST] = grand(arena.max_y - 400000) + 200000;
 }*/

 if (grand(35) == 0)
 {
  accelerate_enemy(e, grand(ANGLE_FULL + 1), 400 + grand(700));
 }

 drag_enemy(e, 0.01);

// float target_angle = atan2(enemy[e].attribute [ATTRIB_BOSS2_Y_DEST] - enemy[e].y,
//                            enemy[e].attribute [ATTRIB_BOSS2_X_DEST] - enemy[e].x);

// accelerate_enemy(e, radians_to_angle(target_angle), 100);


 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(500000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
   if (enemy[e].attacking != ATTACK_NONE)
   {
    if (enemy[e].counter % 4 == 0)
     enemy[e].angle = track_target(e, enemy[e].attacking, 16);
    if (enemy[e].recycle == 0)
     standard_attack(e, enemy[e].angle);
    while(enemy[e].burst_fire != 0)
    {
     standard_attack(e, enemy[e].angle);
    }// while (enemy[e].burst_fire != 0);
   }
 }

 move_enemy(e);
// Needs to be here so that the turrets are placed correctly

 int j = enemy[e].attribute [ATTRIB_BOSS3_TURRET1];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x - 0 * GRAIN;
  enemy[j].y = enemy[e].y + 53 * GRAIN;
 }
 j = enemy[e].attribute [ATTRIB_BOSS3_TURRET2];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x + 50 * GRAIN;
  enemy[j].y = enemy[e].y - 20 * GRAIN;
 }
 j = enemy[e].attribute [ATTRIB_BOSS3_TURRET3];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x - 50 * GRAIN;
  enemy[j].y = enemy[e].y - 20 * GRAIN;
 }
 j = enemy[e].attribute [ATTRIB_BOSS3_TURRET4];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x - 43 * GRAIN;
  enemy[j].y = enemy[e].y + 30 * GRAIN;
 }
 j = enemy[e].attribute [ATTRIB_BOSS3_TURRET5];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x + 43 * GRAIN;
  enemy[j].y = enemy[e].y + 30 * GRAIN;
 }

 return 1;

}

int move_boss3_turret(int e)
{

 if (enemy[e].attribute [ATTRIB_BO3TURRET_MAIN_DEAD] == 1)
 {
  hurt_enemy(e, 99999, OWNER_ENEMY, 0);
  return -1;
 }

 enemy[e].x_speed = enemy[enemy[e].attribute [ATTRIB_BO3TURRET_MAIN]].x_speed;;
 enemy[e].y_speed = enemy[enemy[e].attribute [ATTRIB_BO3TURRET_MAIN]].y_speed;;


 switch(enemy[e].type)
 {
  case ENEMY_BOSS3_TURRET1:
//   if (enemy[e].attacking != ATTACK_NONE)
   {
//    if (enemy[e].counter % 4 == 0)
//     enemy[e].angle = track_target(e, enemy[e].attacking, 16);
    if (enemy[e].recycle == 0)
     standard_attack(e, enemy[e].angle);
//      else
//      {
//       if (ene
//      }
    enemy[e].angle = grand(ANGLE_FULL);
   }
   break;
  case ENEMY_BOSS3_TURRET2:
   if (enemy[e].counter % 16 == 0)
   {
    enemy[e].attacking = nearby_target(450000, enemy[e].x, enemy[e].y);
   }
   if (enemy[e].attacking != ATTACK_NONE)
   {
    if (enemy[e].counter % 4 == 0)
     enemy[e].angle = track_target(e, enemy[e].attacking, 16);
    if (enemy[e].recycle == 0)
     standard_attack(e, enemy[e].angle);
   }
   break;
 }


 return 1;

}


int move_boss4(int e)
{

 if (counter % 32 == 0)
//    enemy_sound(e, ESOUND_BOSS3);
// if (counter % 2 == 0)
    enemy_soundf(e, ESOUND_BOSS3_1 + grand(3), 600 + grand(1500));
//     bullet_sound(b, BSOUND_ORBITAL);
//  enemy_soundf(e, ESOUND_ORBITAL, 700 + grand(400));
 if (enemy[e].x / 150000 == enemy[e].attribute [ATTRIB_BOSS4_X_DEST] / 150000
     && enemy[e].y / 150000 == enemy[e].attribute [ATTRIB_BOSS4_Y_DEST] / 150000)
 {
  enemy[e].attribute [ATTRIB_BOSS4_X_DEST] = grand(arena.max_x - 300000) + 150000;
  enemy[e].attribute [ATTRIB_BOSS4_Y_DEST] = grand(arena.max_y - 300000) + 150000;
 }

// if (grand(35) == 0)
// {
//  accelerate_enemy(e, grand(ANGLE_FULL + 1), 400 + grand(700));
// }

 drag_enemy(e, 0.06);

 int target_angle = radians_to_angle(atan2(enemy[e].attribute [ATTRIB_BOSS2_Y_DEST] - enemy[e].y,
                            enemy[e].attribute [ATTRIB_BOSS2_X_DEST] - enemy[e].x));

 accelerate_enemy(e, target_angle, 70);


 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(500000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
   if (enemy[e].attacking != ATTACK_NONE)
   {
    if (enemy[e].counter % 4 == 0)
     enemy[e].angle = track_target(e, enemy[e].attacking, 16);
    if (enemy[e].recycle == 0)
     standard_attack(e, enemy[e].angle);
//    while(enemy[e].burst_fire != 0)
//    {
//     standard_attack(e, enemy[e].angle);
//    }// while (enemy[e].burst_fire != 0);
   }
 }

 move_enemy(e);
// Needs to be here so that the turrets are placed correctly

 int j = enemy[e].attribute [ATTRIB_BOSS4_TURRET1];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x - 51 * GRAIN;
  enemy[j].y = enemy[e].y + 16 * GRAIN;
 }
 j = enemy[e].attribute [ATTRIB_BOSS4_TURRET2];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x + 51 * GRAIN;
  enemy[j].y = enemy[e].y + 16 * GRAIN;
 }
 j = enemy[e].attribute [ATTRIB_BOSS4_TURRET3];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x - 50 * GRAIN;
  enemy[j].y = enemy[e].y - 44 * GRAIN;
 }
 j = enemy[e].attribute [ATTRIB_BOSS4_TURRET4];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x + 50 * GRAIN;
  enemy[j].y = enemy[e].y - 44 * GRAIN;
 }

 return 1;

}

int move_boss4_turret(int e)
{

 if (enemy[e].attribute [ATTRIB_BO4TURRET_MAIN_DEAD] == 1)
 {
  hurt_enemy(e, 99999, OWNER_ENEMY, 0);
  return -1;
 }

 enemy[e].x_speed = enemy[enemy[e].attribute [ATTRIB_BO4TURRET_MAIN]].x_speed;
 enemy[e].y_speed = enemy[enemy[e].attribute [ATTRIB_BO4TURRET_MAIN]].y_speed;


   if (enemy[e].counter % 16 == 0)
   {
    enemy[e].attacking = nearby_target(150000, enemy[e].x, enemy[e].y);
   }
   
   if (enemy[e].attacking != ATTACK_NONE)
   {
    if (enemy[e].counter % 4 == 0)
     enemy[e].angle = track_target(e, enemy[e].attacking, 16);
   }
    else
     enemy[e].angle = grand(ANGLE_FULL);
     
    if (enemy[e].recycle == 0)
     standard_attack(e, enemy[e].angle);

 return 1;

}


int move_firebase(int e)
{

 enemy[e].x_speed = 0;
 enemy[e].y_speed = 0;

 if (enemy[e].counter % 32 == 0)
  enemy[e].attacking = closest_target(enemy[e].x, enemy[e].y);

 if (enemy[e].attacking != ATTACK_NONE)
 {
  enemy[e].angle = track_target(e, enemy[e].attacking, 16);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

/* int j = enemy[e].attribute [ATTRIB_BLASTER_TURRET1];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x - 17*GRAIN;
  enemy[j].y = enemy[e].y - 18*GRAIN;
 }
 j = enemy[e].attribute [ATTRIB_BLASTER_TURRET2];
 if (j != -1)
 {
  enemy[j].x = enemy[e].x + 17 * GRAIN;
  enemy[j].y = enemy[e].y - 18 * GRAIN;
 }
 */
 return 1;

}

int move_firebase_turret(int e)
{

 if (enemy[e].attribute [ATTRIB_FBTURRET_MAIN_DEAD] == 1)
 {
  hurt_enemy(e, 99999, OWNER_ENEMY, 0);
  return -1;
 }

 enemy[e].x_speed = 0;
 enemy[e].y_speed = 0;

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(200000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  enemy[e].angle = track_target(e, enemy[e].attacking, 16);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

 return 1;

}


int move_firebase2(int e)
{

 enemy[e].x_speed = 0;
 enemy[e].y_speed = 0;

 if (enemy[e].counter % 32 == 0)
  enemy[e].attacking = closest_target(enemy[e].x, enemy[e].y);

 if (enemy[e].attacking != ATTACK_NONE)
 {
  enemy[e].angle = track_target(e, enemy[e].attacking, 16);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

 return 1;

}

int move_firebase2_turret1(int e)
{

 if (enemy[e].attribute [ATTRIB_FB2TURRET_MAIN_DEAD] == 1)
 {
  hurt_enemy(e, 99999, OWNER_ENEMY, 0);
  return -1;
 }

 enemy[e].x_speed = 0;
 enemy[e].y_speed = 0;

 int colours [4];

 if (enemy[e].recycle <= 0)
 {
  create_bullet(BULLET_PREMINE, enemy[e].x, enemy[e].y,
//   enemy[e].x_speed + grand(24001) - 12000, enemy[e].y_speed + grand(30001) - 12000, // oval pattern
   enemy[e].x_speed + grand(15001) - 7500, enemy[e].y_speed + grand(20001) - 7500, // oval pattern
   OWNER_ENEMY, 100, 50 + grand(20), 100, 0,
   0, 0, colours, 1,0,0,0,0,0);
  enemy[e].recycle = 22;
  enemy_sound(e, ESOUND_MINE);
  simple_cloud_trans(TRANS_WHITE, enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed, 500);
 }

 return 1;

}



int move_firebase2_turret2(int e)
{

 if (enemy[e].attribute [ATTRIB_FB2TURRET_MAIN_DEAD] == 1)
 {
  hurt_enemy(e, 99999, OWNER_ENEMY, 0);
  return -1;
 }

 enemy[e].x_speed = 0;
 enemy[e].y_speed = 0;

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(200000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  enemy[e].angle = track_target(e, enemy[e].attacking, 16);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

 return 1;

}


int move_firebase3(int e)
{

 enemy[e].x_speed = 0;
 enemy[e].y_speed = 0;

 if (enemy[e].counter % 32 == 0)
  enemy[e].attacking = closest_target(enemy[e].x, enemy[e].y);

 if (enemy[e].attacking != ATTACK_NONE)
 {
  enemy[e].angle = track_target(e, enemy[e].attacking, 16);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

 return 1;

}

int move_firebase3_turret(int e)
{

 if (enemy[e].attribute [ATTRIB_FB3TURRET_MAIN_DEAD] == 1)
 {
  hurt_enemy(e, 99999, OWNER_ENEMY, 0);
  return -1;
 }

 enemy[e].x_speed = 0;
 enemy[e].y_speed = 0;

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(200000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
  if (enemy[e].burst_fire > 0)
   enemy[e].angle = track_target(e, enemy[e].attacking, 8);
    else
     enemy[e].angle = track_target(e, enemy[e].attacking, 64);
 }

 return 1;

}



int move_curver(int e)
{

 if (enemy[e].x / 100000 == enemy[e].attribute [ATTRIB_DRIFTER_X_DEST] / 100000
     && enemy[e].y / 100000 == enemy[e].attribute [ATTRIB_DRIFTER_Y_DEST] / 100000)
 {
  enemy[e].attribute [ATTRIB_DRIFTER_X_DEST] = grand(arena.max_x - 40000) + 20000;
  enemy[e].attribute [ATTRIB_DRIFTER_Y_DEST] = grand(arena.max_y - 40000) + 20000;
 }

 float target_angle = atan2(enemy[e].attribute [ATTRIB_DRIFTER_Y_DEST] - enemy[e].y,
                            enemy[e].attribute [ATTRIB_DRIFTER_X_DEST] - enemy[e].x);

 accelerate_enemy(e, radians_to_angle(target_angle), 100);


 drag_enemy(e, 0.05);

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  enemy[e].angle = track_target(e, enemy[e].attacking, 4);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }
  
 return 1;

}


void standard_attack(int e, int angle)
{

 int timer;
 int timer_rand = 0;

 int speed = 0;
 int speed_div = 5;
 int mass;
 int seed;
 int status;
 int displaced = 0;
 int colours [4] = {COLOUR_GREEN8, COLOUR_GREEN6, COLOUR_GREEN4, COLOUR_GREEN2};
 int colours_for_cloud [5] = {COLOUR_GREEN8, COLOUR_GREEN6, COLOUR_GREEN4, COLOUR_GREEN2, COLOUR_GREEN1};
 int firing_distance = 0;
 int btype;
 int damage;
 int special1 = 0;
 int special2 = 0;
 int special3 = 0;
 int special4 = 0;
 int special5 = 0;

 int multiple = 1;
 int angle_increment = 0;

 int displace_series [4] = {0,0,0,0};

 int firing_angle;

 int t;

   if (enemy[e].burst_fire == 1)
    enemy[e].burst_fire = 0;

 switch(enemy[e].type)
 {
  case ENEMY_DRIFTER:
   btype = BULLET_YELLOW_BLOB;
   damage = 150;
   timer = 40;
   timer_rand = 20;
   mass = 10;
   speed = 4000;
/*   colours [0] = COLOUR_GREEN8;
   colours [1] = COLOUR_GREEN6;
   colours [2] = COLOUR_GREEN4;
   colours [3] = COLOUR_GREEN2;*/
   firing_distance = 3000;
   enemy[e].recycle = 60;
   speed_div = 3;
   multiple = 3;
   angle_increment = ANGLE_1_EIGHTH;
   enemy_soundf(e, ESOUND_HARD_ZAP, 900 + grand(400));
   break;
  case ENEMY_DRIFTER2:
   btype = BULLET_ORANGE_BLOB;
   damage = 150;
   timer = 40;
   timer_rand = 20;
   mass = 10;
   speed = 5000;
   firing_distance = 3000;
   enemy[e].recycle = 50;
   speed_div = 3;
   multiple = 3;
   angle_increment = ANGLE_1_EIGHTH;
   enemy_soundf(e, ESOUND_HARD_ZAP, 600 + grand(400));
   break;
  case ENEMY_DRIFTER3:
   btype = BULLET_RED_BLOB;
   damage = 150;
   timer = 40;
   timer_rand = 20;
   mass = 10;
   speed = 5000;
   firing_distance = 3000;
   enemy[e].recycle = 50;
   speed_div = 3;
   multiple = 5;
   angle_increment = ANGLE_1_SIXTEENTH;
   enemy_soundf(e, ESOUND_HARD_ZAP, 500 + grand(400));
   break;
  case ENEMY_SWERVER2:
  case ENEMY_BOUNCER2:
   switch(enemy[e].subtype)
   {
    default:
     btype = BULLET_YELLOW_BLOB;
     damage = 150;
     speed = 6000;
     break;
    case SUBTYPE_ORANGE:
    case SUBTYPE_RED:
     btype = BULLET_ORANGE_BLOB;
     damage = 200;
     speed = 7000;
     break;
    case SUBTYPE_BLUE:
    case SUBTYPE_PURPLE:
    case SUBTYPE_WHITE:
     btype = BULLET_PURPLE_BLOB;
     damage = 250;
     speed = 8000;
     break;
   }
   timer = 20;
   timer_rand = 10;
   mass = 10;
   firing_distance = 2000;
   enemy[e].recycle = 60;
   speed_div = 3;
   multiple = 8;
   angle_increment = ANGLE_1_EIGHTH;
   break;
  case ENEMY_BOUNCER3:
   btype = BULLET_SEEKER_BLOB;
   if (enemy[e].subtype > SUBTYPE_ORANGE)
    btype = BULLET_SEEKER_BLOB3; // the other blue one.
   damage = 150;
   timer = 200;
   timer_rand = 20;
   mass = 10;
   speed = 6000;
   firing_distance = 2000;
   enemy[e].recycle = 60;
   speed_div = 3;
   multiple = 4;
   angle_increment = ANGLE_QUARTER;
//   special1 = enemy[e].attacking;
   special1 = nearby_target(400000, enemy[e].x, enemy[e].y);
   break;
  case ENEMY_SWERVER3:
  case ENEMY_BOUNCER4:
   switch(enemy[e].subtype)
   {
    default:
     btype = BULLET_YELLOW_BLOB;
     damage = 150;
     speed = 6000;
     break;
    case SUBTYPE_ORANGE:
    case SUBTYPE_RED:
     btype = BULLET_ORANGE_BLOB;
     damage = 200;
     speed = 7000;
     break;
    case SUBTYPE_BLUE:
    case SUBTYPE_PURPLE:
    case SUBTYPE_WHITE:
     btype = BULLET_PURPLE_BLOB;
     damage = 250;
     speed = 8000;
     break;
   }
   timer = 25;
   timer_rand = 15;
   mass = 10;
   speed = 6000;
   firing_distance = 2000;
   enemy[e].recycle = 30;
   speed_div = 3;
   enemy_soundf(e, ESOUND_BLAT, 400);
   break;
  case ENEMY_FIGHTER4:
   btype = BULLET_YELLOW_BLOB;
   damage = 150;
   timer = 40;
   timer_rand = 20;
   mass = 10;
   speed = 6000;
   firing_distance = 9000;
   enemy[e].recycle = 50;
   speed_div = 3;
   multiple = 2;
   angle_increment = 0;
   displace_series [0] = -1;
   displace_series [1] = 1;
   displaced = 8;
   enemy_soundf(e, ESOUND_HARD_ZAP, 700 + grand(400));
   break;
  case ENEMY_BOUNCER5:
   switch(enemy[e].subtype)
   {
    default:
     btype = BULLET_YELLOW_BLOB;
     damage = 150;
     speed = 6000;
     break;
    case SUBTYPE_ORANGE:
    case SUBTYPE_RED:
     btype = BULLET_ORANGE_BLOB;
     damage = 200;
     speed = 7000;
     break;
    case SUBTYPE_BLUE:
    case SUBTYPE_PURPLE:
    case SUBTYPE_WHITE:
     btype = BULLET_PURPLE_BLOB;
     damage = 250;
     speed = 8000;
     break;
   }
   timer = 20;
   timer_rand = 10;
   mass = 10;
   firing_distance = 2000;
   enemy[e].recycle = 60;
   speed_div = 3;
   multiple = 16;
   angle_increment = ANGLE_1_SIXTEENTH;
   break;
  case ENEMY_CLOAKER:
   btype = BULLET_SEEKER_BLOB;
   damage = 200;
   timer = 399;
   timer_rand = 20;
   mass = 10;
   speed = 5000;
   colours_for_cloud [0] = TRANS_DBLUE;
   colours_for_cloud [1] = TRANS_LBLUE;
   colours_for_cloud [2] = TRANS_WHITE;
   firing_distance = 4000;
   enemy[e].recycle = 250;
   speed_div = 3;
   special1 = enemy[e].attacking;
   multiple = 3;
   angle_increment = ANGLE_QUARTER;
   enemy[e].attribute [ATTRIB_CLOAKER_PULSE] = 33;
   enemy_soundf(e, ESOUND_CROAK, 1100 + grand(400));
   break;
  case ENEMY_CLOAKER2:
   btype = BULLET_SEEKER_BLOB3;
   damage = 200;
   timer = 399;
   timer_rand = 20;
   mass = 10;
   speed = 5000;
   colours_for_cloud [0] = TRANS_DBLUE;
   colours_for_cloud [1] = TRANS_LBLUE;
   colours_for_cloud [2] = TRANS_WHITE;
   firing_distance = 4000;
   enemy[e].recycle = 250;
   speed_div = 3;
   special1 = enemy[e].attacking;
   multiple = 3;
   angle_increment = ANGLE_QUARTER;
   enemy[e].attribute [ATTRIB_CLOAKER_PULSE] = 33;
   enemy_soundf(e, ESOUND_CROAK, 700 + grand(400));
   break;
  case ENEMY_JELLY:
   btype = BULLET_SEEKER_BLOB;
   damage = 200;
   timer = 399;
   timer_rand = 20;
   mass = 10;
   speed = 5000;
   colours_for_cloud [0] = TRANS_DBLUE;
   colours_for_cloud [1] = TRANS_LBLUE;
   colours_for_cloud [2] = TRANS_WHITE;
   firing_distance = 4000;
   enemy[e].recycle = 133;
   speed_div = 3;
   special1 = enemy[e].attacking;
   break;
  case ENEMY_SUPERJELLY:
   btype = BULLET_SEEKER_BLOB3;
   damage = 250;
   timer = 399;
   timer_rand = 20;
   mass = 10;
   speed = 5000;
   colours_for_cloud [0] = TRANS_DBLUE;
   colours_for_cloud [1] = TRANS_LBLUE;
   colours_for_cloud [2] = TRANS_WHITE;
   firing_distance = 4000;
   enemy[e].recycle = 133;
   speed_div = 3;
   special1 = enemy[e].attacking;
   break;
  case ENEMY_CURVER:
   btype = BULLET_CURVE;
   damage = 250;
   timer = 30;
   timer_rand = 10;
   mass = 10;
   speed = 4000;
   colours_for_cloud [0] = TRANS_PURPLE;
   colours_for_cloud [1] = TRANS_PURPLE;
   colours_for_cloud [2] = TRANS_WHITE;
   firing_distance = 4000;
   enemy[e].recycle = 35;
   speed_div = 3;
   enemy_soundf(e, ESOUND_PLASMA, 700);
   break;
  case ENEMY_FLAKKER:
   btype = BULLET_FLAK;
   damage = 150;
   timer = 30;
   timer_rand = 3;
   if (enemy[e].attacking != ATTACK_NONE
       && actor[enemy[e].attacking].in_play == 1)
   {
    t = hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x);
    t /= 1000;
    timer = t / 7;
    if (timer < 5)
     timer = 5;
    if (timer > 45)
     timer = 45;
   }
   mass = 10;
   speed = 6000;
   colours_for_cloud [0] = TRANS_LGREY;
   colours_for_cloud [1] = TRANS_DRED;
   colours_for_cloud [2] = TRANS_PURPLE;
   firing_distance = 4000;
   enemy[e].recycle = 33;
   speed_div = 3;
//   enemy_soundf(e, ESOUND_BLAT, 400);
   enemy_soundf(e, ESOUND_MINE, 700);
   break;
  case ENEMY_FLAKKER2:
   btype = BULLET_FLAK;
   damage = 150;
   timer = 30;
   timer_rand = 3;
   if (enemy[e].attacking != ATTACK_NONE
       && actor[enemy[e].attacking].in_play == 1)
   {
    t = hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x);
    t /= 1000;
    timer = t / 7;
    if (timer < 5)
     timer = 5;
    if (timer > 45)
     timer = 45;
   }
   mass = 10;
   speed = 6000;
   colours_for_cloud [0] = TRANS_LGREY;
   colours_for_cloud [1] = TRANS_DRED;
   colours_for_cloud [2] = TRANS_PURPLE;
   firing_distance = 4000;
   enemy[e].recycle = 55;
   speed_div = 3;
   multiple = 2;
   angle_increment = ANGLE_1_EIGHTH;
   enemy_soundf(e, ESOUND_MINE, 550);
//   enemy_soundf(e, ESOUND_BLAT, 350);
   break;
  case ENEMY_STINGER:
   btype = BULLET_STING;
   damage = 150;
   timer = 40;
   timer_rand = 20;
   mass = 10;
   speed = 3000;
   colours [0] = COLOUR_GREEN8;
   colours [1] = COLOUR_GREEN6;
   colours [2] = COLOUR_GREEN4;
   colours [3] = COLOUR_GREEN2;
   firing_distance = 4000;
   enemy[e].recycle = 60;
   speed_div = 3;
   enemy_soundf(e, ESOUND_STING, 1500 + grand(400));
   break;
  case ENEMY_STINGER2:
   btype = BULLET_STING;
   damage = 150;
   timer = 30;
   timer_rand = 20;
   mass = 10;
   speed = 3000;
   colours [0] = COLOUR_YELLOW8;
   colours [1] = COLOUR_YELLOW6;
   colours [2] = COLOUR_YELLOW4;
   colours [3] = COLOUR_YELLOW2;
   firing_distance = 4000;
   speed_div = 5;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 15;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 120;
     enemy[e].burst_fire = 4 + grand(4);
    }
   enemy_soundf(e, ESOUND_STING, 1200 + grand(400));
   break;
  case ENEMY_STINGER3:
   btype = BULLET_STING;
   damage = 150;
   timer = 20;
   timer_rand = 15;
   mass = 10;
   speed = 4000;
//   colours [0] = COLOUR_RED8;
//   colours [1] = COLOUR_RED6;
//   colours [2] = COLOUR_RED4;
//   colours [3] = COLOUR_RED2;
   colours [0] = COLOUR_WHITE;
   colours [1] = COLOUR_RED8;
   colours [2] = COLOUR_RED5;
   colours [3] = COLOUR_RED3;
   firing_distance = 4000;
   speed_div = 4;
/*   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 5;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 50;
     enemy[e].burst_fire = 3 + grand(4);
    }*/
   enemy[e].recycle = 66;
   multiple = 3;
   angle_increment = ANGLE_1_SIXTEENTH;
   angle -= ANGLE_1_SIXTEENTH;
//   multiple = 5;
//   angle_increment = ANGLE_1_SIXTEENTH;
//   angle -= ANGLE_1_EIGHTH;
   enemy_soundf(e, ESOUND_STING, 900 + grand(400));
   break;
  case ENEMY_STINGER4:
   btype = BULLET_SUPERSTING;
   damage = 200;
   timer = 15;
   timer_rand = 15;
   mass = 10;
   speed = 9000;
   colours [0] = COLOUR_PURPLE8;
   colours [1] = COLOUR_PURPLE6;
   colours [2] = COLOUR_PURPLE4;
   colours [3] = COLOUR_PURPLE2;
   firing_distance = 4000;
   speed_div = 5;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 32;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 110;
     enemy[e].burst_fire = 5 + grand(4);
    }
   enemy_sound(e, ESOUND_ZAPNOTE2);
   break;
  case ENEMY_BEE:
   btype = BULLET_STING;
   damage = 100;
   timer = 10;
   timer_rand = 10;
   mass = 10;
   speed = 7000;
   colours [0] = COLOUR_YELLOW8;
   colours [1] = COLOUR_YELLOW6;
   colours [2] = COLOUR_YELLOW4;
   colours [3] = COLOUR_YELLOW2;
   firing_distance = 2000;
   enemy[e].recycle = 33;
   speed_div = 7;
   enemy_soundf(e, ESOUND_STING, 1900 + grand(400));
   break;
  case ENEMY_HURLER:
   btype = BULLET_STING;
   damage = 100;
   timer = 200 + grand(40);
//   timer_rand = 10;
   mass = 10;
   speed = 4000;
   colours [0] = COLOUR_YELLOW8;
   colours [1] = COLOUR_GREEN7;
   colours [2] = COLOUR_GREEN5;
   colours [3] = COLOUR_GREEN3;
   firing_distance = 4000;
   enemy[e].recycle = 99;
//    enemy[e].burst_fire = 3 + grand(4);
   speed_div = 9;
   multiple = 2;
   angle_increment = 0;
   displace_series [0] = -1;
   displace_series [1] = 1;
//   displace_series [2] = 1;
   displaced = 3;
   enemy_soundf(e, ESOUND_SHORTZAP2, 700);
   break;
  case ENEMY_HURLER2:
   btype = BULLET_PLASMA;
   damage = 150;
   timer = 400;
   timer_rand = 20;
   mass = 10;
   speed = 5000;
   colours [0] = TRANS_DGREEN;
   colours [1] = TRANS_LGREEN;
   colours [2] = TRANS_DGREEN; // YELLOW
   colours [3] = TRANS_LGREEN; // YELLOW
   firing_distance = 4000;
   enemy[e].recycle = 70 + grand(30);
   speed_div = 3;
   enemy_soundf(e, ESOUND_PLASMA, 700 + grand(200));
   break;
  case ENEMY_HURLER3:
   btype = BULLET_PLASMA;
   damage = 150;
   timer = 400;
   timer_rand = 20;
   mass = 10;
   speed = 6000;
   colours [0] = TRANS_DGREEN;
   colours [1] = TRANS_LGREEN;
   colours [2] = TRANS_DGREEN; // YELLOW
   colours [3] = TRANS_LGREEN; // YELLOW
   firing_distance = 4000;
   speed_div = 3;
   enemy_soundf(e, ESOUND_PLASMA, 1300 + grand(100));
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 2;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 110 + grand(30);
     enemy[e].burst_fire = 3 + grand(2);
    }
   break;
  case ENEMY_FIREBASE2:
   btype = BULLET_SEEKER_BLOB2;
   damage = 250;
   timer = 399;
   timer_rand = 20;
   mass = 10;
   speed = 2500;
   colours_for_cloud [0] = TRANS_DGREEN;
   colours_for_cloud [1] = TRANS_LGREEN;
   colours_for_cloud [2] = TRANS_WHITE;
   firing_distance = 4000;
   enemy[e].recycle = 200;
   speed_div = 3;
   special1 = enemy[e].attacking;
   enemy_sound(e, ESOUND_GREENSEEKER1);
   break;
  case ENEMY_FIREBASE2_TURRET2:
   btype = BULLET_YELLOW_BLOB;
   damage = 150;
   timer = 50;
   timer_rand = 10;
   mass = 10;
   speed = 4000;
   firing_distance = 3000;
   enemy[e].recycle = 40;
   speed_div = 3;
   enemy_soundf(e, ESOUND_BLAT, 400);
   break;
  case ENEMY_FIREBASE3:
   btype = BULLET_SEEKER_BLOB2;
   damage = 250;
   timer = 399;
   timer_rand = 20;
   mass = 10;
   speed = 2500;
   colours_for_cloud [0] = TRANS_DGREEN;
   colours_for_cloud [1] = TRANS_LGREEN;
   colours_for_cloud [2] = TRANS_WHITE;
   firing_distance = 4000;
   enemy[e].recycle = 180;
   speed_div = 3;
   special1 = enemy[e].attacking;
   enemy_sound(e, ESOUND_GREENSEEKER1);
   break;
  case ENEMY_FIREBASE3_TURRET:
   btype = BULLET_STING2;
   damage = 150;
   timer = 20;
   timer_rand = 10;
   mass = 10;
   speed = 7000;
   colours [0] = COLOUR_WHITE;
   colours [1] = COLOUR_GREY6;
   colours [2] = COLOUR_BLUE4;
   colours [3] = COLOUR_BLUE2;
   firing_distance = 4000;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 5;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 99 + grand(66);
     enemy[e].burst_fire = 3 + grand(4);
    }
   speed_div = 7;
   multiple = 2;
   angle_increment = 0;
   displace_series [0] = -1;
   displace_series [1] = 1;
   displaced = 2;
   enemy_soundf(e, ESOUND_SHORTZAP2, 1000);
   break;
  case ENEMY_FIGHTER4_TURRET:
   btype = BULLET_PRONG;
   damage = 100;
   timer = 20;
   timer_rand = 10;
   mass = 10;
   speed = 6000;
   colours [0] = COLOUR_YELLOW8;
   colours [1] = COLOUR_YELLOW6;
   colours [2] = COLOUR_YELLOW4;
   colours [3] = COLOUR_YELLOW2;
   firing_distance = 4000;
   enemy[e].recycle = 10; // if this's changed, see also move_fighter4
   special1 = grand(2);
   enemy_soundf(e, ESOUND_BLAT, 1700);
   break;
  case ENEMY_BOSS2:
   btype = BULLET_SEEKER_BLOB;
   damage = 200;
   timer = 399;
   timer_rand = 20;
   mass = 10;
   speed = 5000;
   colours_for_cloud [0] = TRANS_DBLUE;
   colours_for_cloud [1] = TRANS_LBLUE;
   colours_for_cloud [2] = TRANS_WHITE;
   firing_distance = 4000;
   enemy[e].recycle = 15 + enemy[e].attribute [ATTRIB_BOSS2_TURRETS_LEFT] * 20;
   speed_div = 3;
   special1 = enemy[e].attacking;
   if (special1 != -1)
   {
    if (hypot(enemy[e].y - actor[special1].y, enemy[e].x - actor[special1].x) < 100000)
    {
     enemy[e].recycle = 15;
     enemy_soundf(e, ESOUND_SEEKMINE, 1400 + grand(400));
    }
     else
      enemy_soundf(e, ESOUND_SEEKMINE, 1000);
   }
    else
     enemy_soundf(e, ESOUND_SEEKMINE, 1000);
   break;
  case ENEMY_BOSS2_TURRET:
   btype = BULLET_PLASMA;
   damage = 150;
   timer = 400;
   timer_rand = 20;
   mass = 10;
   speed = 11000;
   colours [0] = TRANS_DRED;
   colours [1] = TRANS_LORANGE;
   colours [2] = TRANS_DRED; // YELLOW
   colours [3] = TRANS_LRED;
   firing_distance = 4000;
   speed_div = 3;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 5;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 120 - enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].attribute [ATTRIB_BOSS2_TURRETS_LEFT] * 10;
     enemy[e].burst_fire = 7 - enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].attribute [ATTRIB_BOSS2_TURRETS_LEFT];
    }
   break;
  case ENEMY_FIREBASE:
   btype = BULLET_PLASMA;
   damage = 150;
   timer = 400;
   timer_rand = 20;
   mass = 10;
   speed = 7000;
   colours [0] = TRANS_DRED;
   colours [1] = TRANS_LORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_LRED;
   firing_distance = 4000;
   enemy[e].recycle = 80 + grand(100);
   speed_div = 3;
   enemy_soundf(e, ESOUND_PLASMA, 1200 + grand(200));
   break;
  case ENEMY_FIREBASE_TURRET:
   btype = BULLET_YELLOW_BLOB;
   damage = 150;
   timer = 30;
   timer_rand = 10;
   mass = 10;
   speed = 4000;
   firing_distance = 3000;
   enemy[e].recycle = 80;
   speed_div = 3;
   enemy_soundf(e, ESOUND_SHORTZAP2, 1000);
   break;
  case ENEMY_FIGHTER2:
   btype = BULLET_STING2;
   damage = 50;
   timer = 40;
   timer_rand = 20;
   mass = 10;
   speed = 5000;
   colours [0] = COLOUR_YELLOW8;
   colours [1] = COLOUR_YELLOW6;
   colours [2] = COLOUR_YELLOW4;
   colours [3] = COLOUR_YELLOW2;
   firing_distance = 4000;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 5;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 99 + grand(66);
     enemy[e].burst_fire = 4 + grand(4);
    }
   speed_div = 7;
   multiple = 2;
   angle_increment = 0;
   displace_series [0] = -1;
   displace_series [1] = 1;
   displaced = 2;
   enemy_soundf(e, ESOUND_SHORTZAP2, 1200);
   break;
  case ENEMY_FIGHTER:
   btype = BULLET_PLASMA;
   damage = 150;
   timer = 30;
   timer_rand = 20;
   mass = 10;
   speed = 3000;
   colours [0] = TRANS_DRED;
   colours [1] = TRANS_LORANGE;
   colours [2] = TRANS_DRED;
   colours [3] = TRANS_LRED;
   firing_distance = 4000;
   enemy[e].recycle = 30;
   speed_div = 3;
   enemy_soundf(e, ESOUND_PLASMA, 1200 + grand(200));
   break;
  case ENEMY_FIGHTER3:
   btype = BULLET_SEEKER_BLOB2;
   damage = 250;
   timer = 399;
   timer_rand = 20;
   mass = 10;
   speed = 500;
   colours_for_cloud [0] = TRANS_DGREEN;
   colours_for_cloud [1] = TRANS_LGREEN;
   colours_for_cloud [2] = TRANS_WHITE;
   firing_distance = 4000;
   enemy[e].recycle = 133;
   speed_div = 3;
   special1 = enemy[e].attacking;
   enemy_sound(e, ESOUND_GREENSEEKER2);
   break;
  case ENEMY_TORPER2:
   btype = BULLET_TORPEDO2;
   damage = 40;
   timer = 60;
   timer_rand = 0;
   mass = 10;
   speed = 5000;
   firing_distance = 4000;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 8;
    enemy[e].burst_fire -= 2;
   }
    else
    {
     enemy[e].recycle = 99 + grand(66);
     enemy[e].burst_fire = 7 + grand(8);
    }
   speed_div = 7;
   enemy_soundf(e, ESOUND_MISSILE, 2000);
   break;
  case ENEMY_WANDERER:
   btype = BULLET_STING2;
   damage = 50;
   timer = 40;
   timer_rand = 20;
   mass = 10;
   speed = 4000;
   colours [0] = COLOUR_RED8;
   colours [1] = COLOUR_RED6;
   colours [2] = COLOUR_RED4;
   colours [3] = COLOUR_RED2;
   firing_distance = 4000;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 5;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 99 + grand(66);
     enemy[e].burst_fire = 3 + grand(3);
    }
   speed_div = 3;
   multiple = 2;
   angle_increment = 0;
   displace_series [0] = -1;
   displace_series [1] = 1;
   displaced = 3;
   enemy_soundf(e, ESOUND_SHORTZAP2, 800);
   break;
  case ENEMY_TORPER:
   btype = BULLET_TORPEDO;
   damage = 250;
   timer = 40 + grand(30); // so both explode at the same time
   timer_rand = 0;
   mass = 10;
   speed = 3000;
/*   colours [0] = COLOUR_;
   colours [1] = COLOUR_RED6;
   colours [2] = COLOUR_RED4;
   colours [3] = COLOUR_RED2;*/
   firing_distance = 4000;
   enemy[e].recycle = 40 + grand(36);
   speed_div = 1;
   multiple = 2;
   angle_increment = 0;
   displace_series [0] = -1;
   displace_series [1] = 1;
   displaced = 8;
   enemy_sound(e, ESOUND_TORPEDO);
   break;
  case ENEMY_BLASTER:
   btype = BULLET_ORANGE_BLOB;
   damage = 200;
   timer = 15;
   timer_rand = 5;
   mass = 10;
   speed = 8000;
   firing_distance = 4000;
   enemy[e].recycle = 20;
   speed_div = 3;
   multiple = 4;
   angle_increment = ANGLE_QUARTER;
   enemy_soundf(e, ESOUND_HARD_ZAP, 600 + grand(400));
   break;
  case ENEMY_BLASTER_TURRET:
   btype = BULLET_LINES;
   damage = 200;
   timer = 60;
   timer_rand = 20;
   mass = 10;
   speed = 7000;
   colours [0] = COLOUR_GREY4;
   firing_distance = 4000;
   enemy[e].recycle = 30 + grand(30);
   speed_div = 3;
   break;
  case ENEMY_BOSS1:
   btype = BULLET_ORANGE_BLOB;
   damage = 150;
   timer = 50;
   timer_rand = 40;
   mass = 10;
   speed = 6000;
   firing_distance = 4000;
   enemy[e].recycle = 80;
   speed_div = 3;
   multiple = 8;
   angle_increment = ANGLE_1_EIGHTH;
   enemy_soundf(e, ESOUND_HARD_ZAP, 600 + grand(400));
   break;
  case ENEMY_BOSS1_TURRET1:
  case ENEMY_BOSS1_TURRET2:
   btype = BULLET_YELLOW_BLOB;
   damage = 150;
   timer = 40;
   timer_rand = 20;
   mass = 10;
   speed = 6000;
   firing_distance = 9000;
   enemy[e].recycle = 50 + grand(30);
   speed_div = 3;
   enemy_soundf(e, ESOUND_BLORT, 1500);//600 + grand(400));
   break;
  case ENEMY_BOSS4:
   btype = BULLET_BOSS4;
   damage = 150;
   timer = 90;
   if (enemy[e].attacking != ATTACK_NONE
       && actor[enemy[e].attacking].in_play == 1)
   {
    t = hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x);
    t /= 1000;
    timer = t / 7;
    if (timer < 5)
     timer = 5;
    if (timer > 90)
     timer = 90;
   }
   timer_rand = 20;
   mass = 10;
   speed = 6000;
   firing_distance = 15000;
   enemy[e].recycle = 60 + grand(30);
   speed_div = 3;
   enemy_soundf(e, ESOUND_PLASMA, 400 + grand(200));
//   enemy_soundf(e, ESOUND_BLORT, 200 + grand(400));
   break;
  case ENEMY_BOSS4_TURRET:
   btype = BULLET_PREMINE;
   if (grand(3) == 0)
    btype = BULLET_PRESEEKMINE;
   damage = 100;
   timer = 50;
   timer_rand = 20;
   mass = 10;
   speed = 13000;
   firing_distance = 9000;
   enemy[e].recycle = 90 + grand(20);
   speed_div = 3;
   enemy_sound(e, ESOUND_MINE);//600 + grand(400));
   break;
  case ENEMY_BOSS3:
   btype = BULLET_PLASMA;
   damage = 150;
   timer = 40;
   timer_rand = 20;
   mass = 10;
   speed = 7000;
   colours [0] = TRANS_DRED;
   colours [1] = TRANS_LRED;
   colours [2] = TRANS_LORANGE;
   colours [3] = TRANS_LORANGE;
   firing_distance = 4000;
   enemy[e].recycle = 64;
   switch(enemy[e].burst_fire)
   {
    case 0:
     enemy[e].burst_fire = 4;
//     speed = 7000;
//     colours [0] = TRANS_YELLOW;
     return;
    case 2:
     enemy[e].burst_fire = 0;
     speed = 7000;
     colours [0] = TRANS_YELLOW;
     enemy_soundf(e, ESOUND_PLASMA, 800 + grand(400));
     break;
    case 3:
     enemy[e].burst_fire --;
     speed = 5500;
     colours [0] = TRANS_LORANGE;
     break;
    case 4:
     enemy[e].burst_fire --;
     speed = 4000;
     colours [0] = TRANS_DRED;
     break;
   }
   speed_div = 3;
   break;
  case ENEMY_BOSS3_TURRET1:
   btype = BULLET_STING2;
   damage = 100;
   timer = 20;
   timer_rand = 10;
   mass = 10;
   speed = 6000;
   colours [0] = COLOUR_WHITE;
   colours [1] = COLOUR_GREY6;
   colours [2] = COLOUR_BLUE4;
   colours [3] = COLOUR_BLUE2;
   firing_distance = 8000;
   enemy[e].recycle = 16;
   speed_div = 7;
   multiple = 2;
   angle_increment = 0;
   displace_series [0] = -1;
   displace_series [1] = 1;
   displaced = 2;
   enemy_soundf(e, ESOUND_SHORTZAP2, 1300);
   break;
  case ENEMY_BOSS3_TURRET2:
   btype = BULLET_PLASMA;
   damage = 250;
   timer = 50;
   timer_rand = 20;
   mass = 10;
   speed = 6000;
   colours [0] = TRANS_DBLUE;
   colours [1] = TRANS_LBLUE;
   colours [2] = TRANS_DBLUE;
   colours [3] = TRANS_LBLUE;
   firing_distance = 9000;
   enemy[e].recycle = 64;
   speed_div = 3;
   enemy_soundf(e, ESOUND_PLASMA, 1000);
   break;
  case ENEMY_SUN:
   btype = BULLET_PLASMA;
   damage = 150;
   timer = 30;
   timer_rand = 20;
   mass = 10;
   speed = 10000;
   colours [0] = TRANS_LORANGE;
   colours [1] = TRANS_LORANGE;
   colours [2] = TRANS_DRED;
   colours [3] = TRANS_LRED;
   firing_distance = 4000;
   enemy[e].recycle = 30;
   speed_div = 3;
   multiple = 8;
   angle_increment = ANGLE_1_EIGHTH;
   enemy_soundf(e, ESOUND_SUN, 700);
   break;
  case ENEMY_SUN_GREEN:
   btype = BULLET_PLASMA;
   damage = 200;
   timer = 30;
   timer_rand = 10;
   mass = 10;
   speed = 10000;
   colours [0] = TRANS_DGREEN;
   colours [1] = TRANS_LGREEN;
   colours [2] = TRANS_DGREEN;
   colours [3] = TRANS_LGREEN;
   firing_distance = 4000;
   enemy[e].recycle = 30;
   speed_div = 3;
   multiple = 8;
   angle_increment = ANGLE_1_EIGHTH;
   enemy_soundf(e, ESOUND_SUN, 1000);
   break;
  case ENEMY_SUN_BLUE:
   btype = BULLET_PLASMA;
   damage = 250;
   timer = 20;
   timer_rand = 10;
   mass = 10;
   speed = 13000;
   colours [0] = TRANS_DBLUE;
   colours [1] = TRANS_LBLUE;
   colours [2] = TRANS_DBLUE; // WHITE
   colours [3] = TRANS_LBLUE; // WHITE
   firing_distance = 4000;
   enemy[e].recycle = 30;
   speed_div = 3;
   multiple = 8;
   angle_increment = ANGLE_1_EIGHTH;
   enemy_soundf(e, ESOUND_SUN, 1200);
   break;
  case ENEMY_BOLTER:
   btype = BULLET_PLASMA;
   damage = 150;
   timer = 60;
   timer_rand = 20;
   mass = 10;
   speed = 8000;
   colours [0] = TRANS_DRED;
   colours [1] = TRANS_LORANGE;
   colours [2] = TRANS_DRED;
   colours [3] = TRANS_LRED;
   firing_distance = 9000;
   speed_div = 3;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 5;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 60 + grand(40);
     enemy[e].burst_fire = 7 + grand(3);
    }
   enemy_soundf(e, ESOUND_PLASMA, 1000);
   break;
  case ENEMY_BOLTER2:
   btype = BULLET_PLASMA;
   damage = 200;
   timer = 70;
   timer_rand = 20;
   mass = 10;
   speed = 9000;
   colours [0] = TRANS_DGREEN;
   colours [1] = TRANS_LGREEN;
   colours [2] = TRANS_DGREEN; // YELLOW
   colours [3] = TRANS_LGREEN; // YELLOW
   firing_distance = 9000;
   speed_div = 3;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 4;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 50 + grand(40);
     enemy[e].burst_fire = 8 + grand(4);
    }
   enemy_soundf(e, ESOUND_PLASMA, 1000);
   break;
  default:
   return;
 }

  float cos_angle;
  float sin_angle;

  int xs;
  int ys;

  int x;
  int y;
  int i;

 for (i = 0; i < multiple; i ++)
 {

  firing_angle = angle + i * angle_increment;

  cos_angle = cos(angle_to_radians(firing_angle));
  sin_angle = sin(angle_to_radians(firing_angle));

  xs = enemy[e].x_speed + cos_angle * speed;
  ys = enemy[e].y_speed + sin_angle * speed;

  x = enemy[e].x + cos_angle * firing_distance;
  y = enemy[e].y + sin_angle * firing_distance;



  if (displaced != 0)
  {
//  x += cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3;
//  y += sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3;

//   x += cos(angle_to_radians(angle + (ANGLE_QUARTER * displace_series [i]))) * GRAIN * displaced;
//   y += sin(angle_to_radians(angle + (ANGLE_QUARTER * displace_series [i]))) * GRAIN * displaced;
   x += cos(angle_to_radians(firing_angle + (ANGLE_QUARTER * displace_series [i]))) * GRAIN * displaced;
   y += sin(angle_to_radians(firing_angle + (ANGLE_QUARTER * displace_series [i]))) * GRAIN * displaced;
  }

  switch(enemy[e].type)
  {
   case ENEMY_FIREBASE_TURRET:
   case ENEMY_FIREBASE2_TURRET2:
   case ENEMY_BOUNCER4:
   case ENEMY_SWERVER3:
   case ENEMY_DRIFTER:
   case ENEMY_HURLER2:
   case ENEMY_HURLER3:
    if (i == 0)
     simple_cloud(COLOUR_YELLOW8, x, y, enemy[e].x_speed, enemy[e].y_speed, 550);
    break;
   case ENEMY_DRIFTER2:
    if (i == 0)
     simple_cloud(COLOUR_ORANGE8, x, y, enemy[e].x_speed, enemy[e].y_speed, 550);
    break;
   case ENEMY_DRIFTER3:
    if (i == 0)
     simple_cloud(COLOUR_RED8, x, y, enemy[e].x_speed, enemy[e].y_speed, 550);
    break;
   case ENEMY_BLASTER:
    if (i == 0)
     simple_cloud(COLOUR_YELLOW8, x, y, enemy[e].x_speed, enemy[e].y_speed, 550);
    break;
   case ENEMY_BOSS1:
   case ENEMY_BOSS1_TURRET1:
   case ENEMY_BOSS1_TURRET2:
    if (i == 0)
     simple_cloud(COLOUR_YELLOW8, x, y, enemy[e].x_speed, enemy[e].y_speed, 550);
    break;
   case ENEMY_BOSS3_TURRET2:
    simple_cloud(COLOUR_BLUE8, x, y, enemy[e].x_speed, enemy[e].y_speed, 550);
    break;
   case ENEMY_BOSS4:
    simple_cloud(COLOUR_YELLOW8, x, y, enemy[e].x_speed, enemy[e].y_speed, 900);
    break;
   case ENEMY_FIGHTER4_TURRET:
    colours_for_cloud [0] = COLOUR_YELLOW8;
    create_cloud(CLOUD_MED_CIRCLE, x, y, 0, 0,
     enemy[e].x_speed, enemy[e].y_speed, 300, 100,
     20, 0, 0, 0, colours_for_cloud);
    break;
   case ENEMY_WANDERER:
    colours_for_cloud [0] = COLOUR_RED8;
    create_cloud(CLOUD_MED_CIRCLE, x, y, 0, 0,
     enemy[e].x_speed, enemy[e].y_speed, 500, 150,
     20, 0, 0, 0, colours_for_cloud);
    break;
   case ENEMY_FIREBASE3_TURRET:
   case ENEMY_BOSS3_TURRET1:
   case ENEMY_BOSS4_TURRET:
    colours_for_cloud [0] = COLOUR_WHITE;
    create_cloud(CLOUD_MED_CIRCLE, x, y, 0, 0,
     enemy[e].x_speed, enemy[e].y_speed, 500, 150,
     20, 0, 0, 0, colours_for_cloud);
    break;
   case ENEMY_FIGHTER:
   case ENEMY_FIGHTER2:
   case ENEMY_BOLTER:
   case ENEMY_BOLTER2:
//   case ENEMY_FIGHTER3:
    colours_for_cloud [0] = COLOUR_YELLOW8;
    create_cloud(CLOUD_MED_CIRCLE, x, y, 0, 0,
     enemy[e].x_speed, enemy[e].y_speed, 300, 150,
     20, 0, 0, 0, colours_for_cloud);
    break;
   case ENEMY_FIGHTER3:
   case ENEMY_JELLY:
   case ENEMY_CLOAKER:
   case ENEMY_CLOAKER2:
   case ENEMY_SUPERJELLY:
   case ENEMY_FLAKKER:
   case ENEMY_FLAKKER2:
   case ENEMY_BOSS2:
//      place_explosion(x, y, 0,0,
//       300 + crandom(100), colours_for_cloud);
      place_burstlet_burst(x, y, 0, 0, 3 + grand(3), 4, 2, 1000, 1500,
       4, colours_for_cloud);
   break;
   case ENEMY_CURVER:
      place_burstlet_burst(x, y, 0, 0, 3 + grand(3), 4, 2, 1000, 1500,
       4, colours_for_cloud);
   break;
   case ENEMY_FIGHTER4:
    simple_cloud(COLOUR_YELLOW6, x, y, enemy[e].x_speed, enemy[e].y_speed, 300);
    break;
   case ENEMY_BEE:
    simple_cloud(COLOUR_YELLOW6, x, y, enemy[e].x_speed, enemy[e].y_speed, 200);
    break;

  }

  switch(enemy[e].type)
  {
   case ENEMY_JELLY:
    enemy_sound(e, ESOUND_WOBBLE);
    break;
   case ENEMY_SUPERJELLY:
    enemy_sound(e, ESOUND_LONG_WOBBLE);
    break;

  }

  create_bullet(btype, x, y,
   xs, ys, OWNER_ENEMY,
   damage, timer + grand(timer_rand), mass, firing_angle,
   status, seed, colours, speed_div, special1,special2,special3,special4,special5);

 }

}


// no leading...
int track_target(int e, int attacking, int turning)
{
 return turn_towards_xy(enemy[e].x, enemy[e].y, actor[attacking].x, actor[attacking].y, enemy[e].angle, turning);
}

int near_angle(int angle1, int angle2)
{

 if (angle1 / 200 == angle2 / 200)
  return 1;

 if (angle1 < 100 && angle2 > ANGLE_FULL - 100)
  return 1;
 if (angle2 < 100 && angle1 > ANGLE_FULL - 100)
  return 1;

 return 0;
 
}



void accelerate_enemy(int e, int angle, int impulse)
{

  enemy[e].x_speed += cos(angle_to_radians(angle)) * impulse;
  enemy[e].y_speed += sin(angle_to_radians(angle)) * impulse;

}

void drag_enemy(int e, float drag_amount)
{
 enemy[e].x_speed *= (float) 1 - drag_amount;
 enemy[e].y_speed *= (float) 1 - drag_amount;
}


int metamorphosis(int e)
{
 int x = enemy[e].x;
 int y = enemy[e].y;
 int subtype = enemy[e].subtype;
 int carrying_pickup = enemy[e].carrying_pickup;
 int special [10];
 int change_to;

 switch(enemy[e].type)
 {
  case ENEMY_FALLER:
   change_to = ENEMY_BOUNCER;
   switch(enemy[e].attribute [ATTRIB_FALLER_SECTIONS])
   {
    case 0: change_to = ENEMY_BOUNCER; break;
    case 1: change_to = ENEMY_BOUNCER2; break;
    case 2: change_to = ENEMY_BOUNCER3; break;
    case 3: change_to = ENEMY_BOUNCER4; break;
    case 4: change_to = ENEMY_BOUNCER5; break;
   }
   break;
  case ENEMY_CRAWLER:
   change_to = ENEMY_SWERVER;
   break;
  case ENEMY_CRAWLER2:
   change_to = ENEMY_SWERVER2;
   break;
  case ENEMY_CRAWLER3:
   change_to = ENEMY_SWERVER3;
   break;
  case ENEMY_CRAWLER4:
   change_to = ENEMY_SWERVER;
   break;
 }
 
   if (y >= arena.max_y - GRAIN * 8)
    y = arena.max_y - GRAIN * 8;
   destroy_enemy(e);
   e = create_enemy(change_to, subtype, x, y,
     2000 - grand(4001), - 1000 - grand(1000), 0, carrying_pickup, special, TARGET_CRAWLY);

  int passing_colours2 [5];

  switch(subtype)
  {
    case SUBTYPE_GREEN:
//     passing_colours [0] = COLOUR_GREEN8;
     passing_colours2 [0] = TRANS_DGREEN;
     passing_colours2 [1] = TRANS_LGREEN;
     passing_colours2 [2] = TRANS_YELLOW;
     break;
    case SUBTYPE_YELLOW:
//     passing_colours [0] = COLOUR_YELLOW8;
     passing_colours2 [0] = TRANS_LORANGE;
     passing_colours2 [1] = TRANS_YELLOW;
     passing_colours2 [2] = TRANS_WHITE;
     break;
    case SUBTYPE_ORANGE:
//     passing_colours [0] = COLOUR_ORANGE8;
     passing_colours2 [0] = TRANS_DORANGE;
     passing_colours2 [1] = TRANS_LORANGE;
     passing_colours2 [2] = TRANS_YELLOW;
     break;
    case SUBTYPE_RED:
//     passing_colours [0] = COLOUR_RED8;
     passing_colours2 [0] = TRANS_DRED;
     passing_colours2 [1] = TRANS_LRED;
     passing_colours2 [2] = TRANS_YELLOW;
     break;
    case SUBTYPE_BLUE:
//     passing_colours [0] = COLOUR_BLUE8;
     passing_colours2 [0] = TRANS_DBLUE;
     passing_colours2 [1] = TRANS_LBLUE;
     passing_colours2 [2] = TRANS_WHITE;
     break;
    case SUBTYPE_PURPLE:
//     passing_colours [0] = COLOUR_PURPLE8;
     passing_colours2 [0] = TRANS_LBLUE;
     passing_colours2 [1] = TRANS_PURPLE;
     passing_colours2 [2] = TRANS_WHITE;
     break;
    case SUBTYPE_WHITE:
//     passing_colours [0] = COLOUR_WHITE;
     passing_colours2 [0] = TRANS_DGREY;
     passing_colours2 [1] = TRANS_LGREY;
     passing_colours2 [2] = TRANS_WHITE2;
     break;
  }
/*      create_cloud(CLOUD_SHRINKING_CIRCLE,
       location [0],
       location [1],
       0, 0,
       0, 0,
       500, -150 - eclass[type].radius / 100, 10, 0, 0, 0, passing_colours);
*/
      place_explosion(x, y, 0,0,
       600 + crandom(250), passing_colours2);

   enemy_soundf(e, ESOUND_MUTATE, 1300 + grand(300) - subtype * 100);

 calculate_ambience();

 return e;
}


int spawn_enemy(int type, int subtype, int carrying_pickup, int target)
{
 int special [10];
 int location [2];
 int passing_colours [5];
 int passing_colours2 [5];

 char standard_sound = 1;

 switch(type)
 {
  default:
   find_target_start_point(location);
   break;
  case ENEMY_CRAWLER:
  case ENEMY_CRAWLER2:
  case ENEMY_CRAWLER3:
  case ENEMY_CRAWLER4:
  case ENEMY_FALLER:
   find_crawly_start_point(location);
   break;
  case ENEMY_FIREBASE:
  case ENEMY_FIREBASE2:
  case ENEMY_FIREBASE3:
  case ENEMY_HIVE:
   find_firebase_start_point(location);
   break;
 }

// switch(type)
// {
//  default:
  switch(subtype)
  {
    case SUBTYPE_GREEN:
     passing_colours [0] = COLOUR_GREEN8;
     passing_colours2 [0] = TRANS_DGREEN;
     passing_colours2 [1] = TRANS_LGREEN;
     passing_colours2 [2] = TRANS_YELLOW;
     break;
    case SUBTYPE_YELLOW:
     passing_colours [0] = COLOUR_YELLOW8;
     passing_colours2 [0] = TRANS_LORANGE;
     passing_colours2 [1] = TRANS_YELLOW;
     passing_colours2 [2] = TRANS_WHITE;
     break;
    case SUBTYPE_ORANGE:
     passing_colours [0] = COLOUR_ORANGE8;
     passing_colours2 [0] = TRANS_DORANGE;
     passing_colours2 [1] = TRANS_LORANGE;
     passing_colours2 [2] = TRANS_YELLOW;
     break;
    case SUBTYPE_RED:
     passing_colours [0] = COLOUR_RED8;
     passing_colours2 [0] = TRANS_DRED;
     passing_colours2 [1] = TRANS_LRED;
     passing_colours2 [2] = TRANS_YELLOW;
     break;
    case SUBTYPE_BLUE:
     passing_colours [0] = COLOUR_BLUE8;
     passing_colours2 [0] = TRANS_DBLUE;
     passing_colours2 [1] = TRANS_LBLUE;
     passing_colours2 [2] = TRANS_WHITE;
     break;
    case SUBTYPE_PURPLE:
     passing_colours [0] = COLOUR_PURPLE8;
     passing_colours2 [0] = TRANS_LBLUE;
     passing_colours2 [1] = TRANS_PURPLE;
     passing_colours2 [2] = TRANS_WHITE;
     break;
    case SUBTYPE_WHITE:
     passing_colours [0] = COLOUR_WHITE;
     passing_colours2 [0] = TRANS_DGREY;
     passing_colours2 [1] = TRANS_LGREY;
     passing_colours2 [2] = TRANS_WHITE2;
     break;
    default:
     switch(type)
     {
      default:
      case ENEMY_DRIFTER:
      case ENEMY_SUN_GREEN:
      case ENEMY_FIGHTER:
      case ENEMY_BOSS1:
      case ENEMY_STINGER:
      case ENEMY_HURLER:
      passing_colours2 [0] = TRANS_DGREEN;
      passing_colours2 [1] = TRANS_LGREEN;
      passing_colours2 [2] = TRANS_YELLOW;
      passing_colours [0] = COLOUR_GREEN8;
      break;
      case ENEMY_JELLY:
      case ENEMY_WANDERER:
      case ENEMY_BEE:
      case ENEMY_SUN:
      case ENEMY_FIGHTER2:
      case ENEMY_BOSS2:
      case ENEMY_HURLER3:
      passing_colours [0] = COLOUR_YELLOW8;
      passing_colours2 [0] = TRANS_LORANGE;
      passing_colours2 [1] = TRANS_YELLOW;
      passing_colours2 [2] = TRANS_WHITE;
      break;
      case ENEMY_MINETHROWER:
      case ENEMY_STINGER2:
      case ENEMY_BOLTER:
      case ENEMY_TORPER:
      case ENEMY_HURLER2:
      passing_colours [0] = COLOUR_ORANGE8;
      passing_colours2 [0] = TRANS_DORANGE;
      passing_colours2 [1] = TRANS_LORANGE;
      passing_colours2 [2] = TRANS_YELLOW;
      break;
      case ENEMY_SUPERJELLY:
      case ENEMY_STINGER3:
      case ENEMY_DRIFTER2:
      case ENEMY_FIGHTER3:
      case ENEMY_BOLTER2:
      passing_colours [0] = COLOUR_RED8;
      passing_colours2 [0] = TRANS_DRED;
      passing_colours2 [1] = TRANS_LRED;
      passing_colours2 [2] = TRANS_YELLOW;
      break;
      case ENEMY_SUN_BLUE:
      case ENEMY_MINETHROWER2:
      case ENEMY_FIGHTER4:
      case ENEMY_BLASTER:
      case ENEMY_FLAKKER:
      case ENEMY_FLAKKER2:
      case ENEMY_BOSS3:
      passing_colours [0] = COLOUR_BLUE8;
      passing_colours2 [0] = TRANS_DBLUE;
      passing_colours2 [1] = TRANS_LBLUE;
      passing_colours2 [2] = TRANS_WHITE;
      break;
      case ENEMY_CURVER:
      case ENEMY_DRIFTER3:
      case ENEMY_CLOAKER:
      case ENEMY_CLOAKER2:
      case ENEMY_STINGER4:
      case ENEMY_BOSS4:
      case ENEMY_TORPER2:
      passing_colours [0] = COLOUR_PURPLE8;
      passing_colours2 [0] = TRANS_LBLUE;
      passing_colours2 [1] = TRANS_PURPLE;
      passing_colours2 [2] = TRANS_WHITE;
      break;
     }
     break;
  }

  
      create_cloud(CLOUD_SHRINKING_CIRCLE,
       location [0],
       location [1],
       0, 0,
       0, 0,
       500, -150 - eclass[type].radius / 100, 10, 0, 0, 0, passing_colours);
       
      place_explosion(location [0], location [1], 0,0,
       200 + crandom(250) + eclass[type].radius / 10, passing_colours2);

//  break;

// }

 if (target == TARGET_CRAWLY)
  arena.crawlies_created ++;

 int e = create_enemy(type, subtype, location [0], location [1],
  0, 0, 0, carrying_pickup, special, target);

  if (standard_sound == 1)
     enemy_soundf(e, ESOUND_WARP_IN, 800 + grand(800) - eclass[type].mass);

 calculate_ambience();
 
 return e;
// need to make sure: either that creatures made up of multiple

}

void find_target_start_point(int location [2])
{
 int timeout = 0;

 do
 {
  location [0] = grand(arena.max_x - 300000) + 150000;
  location [1] = grand(arena.max_y - 300000) + 150000;
  timeout ++;
  if (timeout >= 1000)
   break; // Oh well.
  if (actor[0].in_play == 1)
   if (location [0] > actor[0].x - 50000 && location [0] < actor[0].x + 50000
       && location [1] > actor[0].y - 50000 && location [1] < actor[0].y + 50000)
    continue;
  if (actor[1].in_play == 1)
   if (location [0] > actor[1].x - 50000 && location [0] < actor[1].x + 50000
       && location [1] > actor[1].y - 50000 && location [1] < actor[1].y + 50000)
    continue;

  break;
 }
  while (TRUE);

}

void find_crawly_start_point(int location [2])
{
 location [1] = 10000;
 int timeout = 0;

 do
 {
  location [0] = grand(arena.max_x - 100000) + 50000;
  location [0] /= GRID_WIDTH * GRAIN;
  location [0] *= GRID_WIDTH * GRAIN;
  location [0] -= 5000;
  timeout ++;
  if (timeout >= 1000)
   break; // Oh well.
  if (actor[0].in_play == 1 && actor[0].y < 100000)
   if (location [0] > actor[0].x - 50000 && location [0] < actor[0].x + 50000)
    continue;
  if (actor[1].in_play == 1 && actor[1].y < 100000)
   if (location [0] > actor[1].x - 50000 && location [0] < actor[1].x + 50000)
    continue;

  break;
 }
  while (TRUE);

}

void find_firebase_start_point(int location [2])
{
 int timeout = 0;

 do
 {
  location [0] = grand(arena.max_x - 200000) + 100000;
  location [0] /= GRID_WIDTH * GRAIN;
  location [0] *= GRID_WIDTH * GRAIN;
  location [0] -= 4000;
  location [1] = grand(arena.max_y - 200000) + 100000;
  location [1] /= GRID_WIDTH * GRAIN;
  location [1] *= GRID_WIDTH * GRAIN;
  location [1] -= 4000;
  timeout ++;
  if (timeout >= 1000)
   break; // Oh well.
/*  if (actor[0].in_play == 1//)// && actor[0].y < 100000)
       && location [0] > actor[0].x - 120000 && location [0] < actor[0].x + 120000
       && location [1] > actor[0].y - 120000 && location [1] < actor[0].y + 120000)
    continue;
  if (actor[1].in_play == 1//)// && actor[1].y < 100000)
       && location [0] > actor[1].x - 120000 && location [0] < actor[1].x + 120000
       && location [1] > actor[1].y - 120000 && location [1] < actor[1].y + 120000)
//   if (location [0] > actor[1].x - 50000 && location [0] < actor[1].x + 50000)
    continue;*/

  break;
 }
  while (TRUE);

}

void register_destroyed(int e, int cause)
{
 int i, scoring;

 for (i = 0; i < NO_BULLETS; i ++)
 {
  if (bullet[i].type == BULLET_MISSILE
      && bullet[i].special3 == e)
       bullet[i].special3 = -1;
   // clear up any locks that missiles have on the enemy
 }

 for (i = 0; i < NO_ACTORS; i ++)
 {
  if (actor[i].lock == e)
   actor[i].lock = -1;
 }

 for (i = 0; i < NO_ACTORS; i ++)
 {
  if (actor[i].turret_lock == e)
   actor[i].turret_lock = -1;
 }

 if (cause != -1)
 {
  scoring = actor[cause].controller;
  score_kill(enemy[e].type, enemy[e].subtype, scoring);
 }
  else scoring = -1;

 switch(enemy[e].type)
 {
  case ENEMY_BLASTER:
   if (enemy[e].attribute [ATTRIB_BLASTER_TURRET1] != -1)
   {
    score_kill(ENEMY_BLASTER_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BLASTER_TURRET1]].attribute [ATTRIB_BTURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_BLASTER_TURRET2] != -1)
   {
    score_kill(ENEMY_BLASTER_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BLASTER_TURRET2]].attribute [ATTRIB_BTURRET_MAIN] = -1;
   }
   break;
  case ENEMY_BLASTER_TURRET:
   if (enemy[e].attribute [ATTRIB_BTURRET_MAIN] != -1)
   {
    if (enemy[enemy[e].attribute [ATTRIB_BTURRET_MAIN]].attribute [ATTRIB_BLASTER_TURRET1] == e)
     enemy[enemy[e].attribute [ATTRIB_BTURRET_MAIN]].attribute [ATTRIB_BLASTER_TURRET1] = -1;
      else
//    if (enemy[enemy[e].attribute [ATTRIB_BTURRET_MAIN]].attribute [ATTRIB_BLASTER_TURRET2] == e)
       enemy[enemy[e].attribute [ATTRIB_BTURRET_MAIN]].attribute [ATTRIB_BLASTER_TURRET2] = -1;
   }
   break;
  case ENEMY_FIREBASE:
   if (enemy[e].attribute [ATTRIB_FIREBASE_TURRET1] != -1)
   {
    score_kill(ENEMY_FIREBASE_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_FIREBASE_TURRET1]].attribute [ATTRIB_FBTURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_FIREBASE_TURRET2] != -1)
   {
    score_kill(ENEMY_FIREBASE_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_FIREBASE_TURRET2]].attribute [ATTRIB_FBTURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_FIREBASE_TURRET3] != -1)
   {
    score_kill(ENEMY_FIREBASE_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_FIREBASE_TURRET3]].attribute [ATTRIB_FBTURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_FIREBASE_TURRET4] != -1)
   {
    score_kill(ENEMY_FIREBASE_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_FIREBASE_TURRET4]].attribute [ATTRIB_FBTURRET_MAIN] = -1;
   }
   break;
  case ENEMY_FIREBASE_TURRET:
   if (enemy[e].attribute [ATTRIB_FBTURRET_MAIN] != -1)
   {
    if (enemy[enemy[e].attribute [ATTRIB_FBTURRET_MAIN]].attribute [ATTRIB_FIREBASE_TURRET1] == e)
     enemy[enemy[e].attribute [ATTRIB_FBTURRET_MAIN]].attribute [ATTRIB_FIREBASE_TURRET1] = -1;
      else
      {
       if (enemy[enemy[e].attribute [ATTRIB_FBTURRET_MAIN]].attribute [ATTRIB_FIREBASE_TURRET2] == e)
        enemy[enemy[e].attribute [ATTRIB_FBTURRET_MAIN]].attribute [ATTRIB_FIREBASE_TURRET2] = -1;
         else
         {
          if (enemy[enemy[e].attribute [ATTRIB_FBTURRET_MAIN]].attribute [ATTRIB_FIREBASE_TURRET3] == e)
           enemy[enemy[e].attribute [ATTRIB_FBTURRET_MAIN]].attribute [ATTRIB_FIREBASE_TURRET3] = -1;
            else
            {
             if (enemy[enemy[e].attribute [ATTRIB_FBTURRET_MAIN]].attribute [ATTRIB_FIREBASE_TURRET4] == e)
              enemy[enemy[e].attribute [ATTRIB_FBTURRET_MAIN]].attribute [ATTRIB_FIREBASE_TURRET4] = -1;
            }
         }
      }
   }
//      else
//       enemy[enemy[e].attribute [ATTRIB_FBTURRET_MAIN]].attribute [ATTRIB_FIREBASE_TURRET2] = -1;
   break;
  case ENEMY_BOSS1:
   if (enemy[e].attribute [ATTRIB_BOSS1_TURRET1] != -1)
   {
    score_kill(ENEMY_BOSS1_TURRET1, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BOSS1_TURRET1]].attribute [ATTRIB_BO1TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_BOSS1_TURRET2] != -1)
   {
    score_kill(ENEMY_BOSS1_TURRET1, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BOSS1_TURRET2]].attribute [ATTRIB_BO1TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_BOSS1_TURRET3] != -1)
   {
    score_kill(ENEMY_BOSS1_TURRET1, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BOSS1_TURRET3]].attribute [ATTRIB_BO1TURRET_MAIN] = -1;
   }
//   if (enemy[e].attribute [ATTRIB_BOSS1_TURRET4] != -1)
//    enemy[enemy[e].attribute [ATTRIB_BOSS1_TURRET4]].attribute [ATTRIB_BO1TURRET_MAIN] = -1;
   break;
  case ENEMY_BOSS1_TURRET1:
  case ENEMY_BOSS1_TURRET2:
   if (enemy[e].attribute [ATTRIB_BO1TURRET_MAIN] != -1)
   {
    if (enemy[enemy[e].attribute [ATTRIB_BO1TURRET_MAIN]].attribute [ATTRIB_BOSS1_TURRET1] == e)
     enemy[enemy[e].attribute [ATTRIB_BO1TURRET_MAIN]].attribute [ATTRIB_BOSS1_TURRET1] = -1;
      else
      {
       if (enemy[enemy[e].attribute [ATTRIB_BO1TURRET_MAIN]].attribute [ATTRIB_BOSS1_TURRET2] == e)
        enemy[enemy[e].attribute [ATTRIB_BO1TURRET_MAIN]].attribute [ATTRIB_BOSS1_TURRET2] = -1;
         else
         {
          if (enemy[enemy[e].attribute [ATTRIB_BO1TURRET_MAIN]].attribute [ATTRIB_BOSS1_TURRET3] == e)
           enemy[enemy[e].attribute [ATTRIB_BO1TURRET_MAIN]].attribute [ATTRIB_BOSS1_TURRET3] = -1;
//            else
//            {
//             if (enemy[enemy[e].attribute [ATTRIB_BO1TURRET_MAIN]].attribute [ATTRIB_BOSS1_TURRET4] == e)
//              enemy[enemy[e].attribute [ATTRIB_BO1TURRET_MAIN]].attribute [ATTRIB_BOSS1_TURRET4] = -1;
//            }
         }
      }
   }
//      else
//       enemy[enemy[e].attribute [ATTRIB_FBTURRET_MAIN]].attribute [ATTRIB_FIREBASE_TURRET2] = -1;
   break;
  case ENEMY_BOSS2:
   if (enemy[e].attribute [ATTRIB_BOSS2_TURRET1] != -1)
   {
    score_kill(ENEMY_BOSS2_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BOSS2_TURRET1]].attribute [ATTRIB_BO2TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_BOSS2_TURRET2] != -1)
   {
    score_kill(ENEMY_BOSS2_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BOSS2_TURRET2]].attribute [ATTRIB_BO2TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_BOSS2_TURRET3] != -1)
   {
    score_kill(ENEMY_BOSS2_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BOSS2_TURRET3]].attribute [ATTRIB_BO2TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_BOSS2_TURRET4] != -1)
   {
    score_kill(ENEMY_BOSS2_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BOSS2_TURRET4]].attribute [ATTRIB_BO2TURRET_MAIN] = -1;
   }
   break;
  case ENEMY_BOSS2_TURRET:
   if (enemy[e].attribute [ATTRIB_BO2TURRET_MAIN] != -1)
   {
   enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].attribute [ATTRIB_BOSS2_DELTA_SPIN] *= -1;
    if (enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].attribute [ATTRIB_BOSS2_TURRET1] == e)
     enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].attribute [ATTRIB_BOSS2_TURRET1] = -1;
      else
      {
       if (enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].attribute [ATTRIB_BOSS2_TURRET2] == e)
        enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].attribute [ATTRIB_BOSS2_TURRET2] = -1;
         else
         {
          if (enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].attribute [ATTRIB_BOSS2_TURRET3] == e)
           enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].attribute [ATTRIB_BOSS2_TURRET3] = -1;
            else
            {
             if (enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].attribute [ATTRIB_BOSS2_TURRET4] == e)
              enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].attribute [ATTRIB_BOSS2_TURRET4] = -1;
            }
         }
      }
   }
//      else
//       enemy[enemy[e].attribute [ATTRIB_FBTURRET_MAIN]].attribute [ATTRIB_FIREBASE_TURRET2] = -1;
   break;
  case ENEMY_BOSS3:
   if (enemy[e].attribute [ATTRIB_BOSS3_TURRET1] != -1)
   {
    score_kill(ENEMY_BOSS3_TURRET1, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BOSS3_TURRET1]].attribute [ATTRIB_BO3TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_BOSS3_TURRET2] != -1)
   {
    score_kill(ENEMY_BOSS3_TURRET1, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BOSS3_TURRET2]].attribute [ATTRIB_BO3TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_BOSS3_TURRET3] != -1)
   {
    score_kill(ENEMY_BOSS3_TURRET1, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BOSS3_TURRET3]].attribute [ATTRIB_BO3TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_BOSS3_TURRET4] != -1)
   {
    score_kill(ENEMY_BOSS3_TURRET1, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BOSS3_TURRET4]].attribute [ATTRIB_BO3TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_BOSS3_TURRET5] != -1)
   {
    score_kill(ENEMY_BOSS3_TURRET1, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BOSS3_TURRET5]].attribute [ATTRIB_BO3TURRET_MAIN] = -1;
   }
   break;
  case ENEMY_BOSS3_TURRET1:
   if (enemy[e].attribute [ATTRIB_BO3TURRET_MAIN] != -1)
   {
//   enemy[enemy[e].attribute [ATTRIB_BO3TURRET_MAIN]].attribute [ATTRIB_BOSS3_DELTA_SPIN] *= -1;
    if (enemy[enemy[e].attribute [ATTRIB_BO3TURRET_MAIN]].attribute [ATTRIB_BOSS3_TURRET1] == e)
     enemy[enemy[e].attribute [ATTRIB_BO3TURRET_MAIN]].attribute [ATTRIB_BOSS3_TURRET1] = -1;
      else
      {
       if (enemy[enemy[e].attribute [ATTRIB_BO3TURRET_MAIN]].attribute [ATTRIB_BOSS3_TURRET2] == e)
        enemy[enemy[e].attribute [ATTRIB_BO3TURRET_MAIN]].attribute [ATTRIB_BOSS3_TURRET2] = -1;
         else
         {
          if (enemy[enemy[e].attribute [ATTRIB_BO3TURRET_MAIN]].attribute [ATTRIB_BOSS3_TURRET3] == e)
           enemy[enemy[e].attribute [ATTRIB_BO3TURRET_MAIN]].attribute [ATTRIB_BOSS3_TURRET3] = -1;
//            else
//            {
//             if (enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].attribute [ATTRIB_BOSS2_TURRET4] == e)
//              enemy[enemy[e].attribute [ATTRIB_BO2TURRET_MAIN]].attribute [ATTRIB_BOSS2_TURRET4] = -1;
//            }
         }
      }
   }
//      else
//       enemy[enemy[e].attribute [ATTRIB_FBTURRET_MAIN]].attribute [ATTRIB_FIREBASE_TURRET2] = -1;
   break;
  case ENEMY_BOSS3_TURRET2:
   if (enemy[e].attribute [ATTRIB_BO3TURRET_MAIN] != -1)
   {
    if (enemy[enemy[e].attribute [ATTRIB_BO3TURRET_MAIN]].attribute [ATTRIB_BOSS3_TURRET4] == e)
     enemy[enemy[e].attribute [ATTRIB_BO3TURRET_MAIN]].attribute [ATTRIB_BOSS3_TURRET4] = -1;
      else
      {
       if (enemy[enemy[e].attribute [ATTRIB_BO3TURRET_MAIN]].attribute [ATTRIB_BOSS3_TURRET5] == e)
        enemy[enemy[e].attribute [ATTRIB_BO3TURRET_MAIN]].attribute [ATTRIB_BOSS3_TURRET5] = -1;
      }
   }
//      else
//       enemy[enemy[e].attribute [ATTRIB_FBTURRET_MAIN]].attribute [ATTRIB_FIREBASE_TURRET2] = -1;
   break;
  case ENEMY_BOSS4:
   if (enemy[e].attribute [ATTRIB_BOSS4_TURRET1] != -1)
   {
    score_kill(ENEMY_BOSS4_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BOSS4_TURRET1]].attribute [ATTRIB_BO4TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_BOSS4_TURRET2] != -1)
   {
    score_kill(ENEMY_BOSS4_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BOSS4_TURRET2]].attribute [ATTRIB_BO4TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_BOSS4_TURRET3] != -1)
   {
    score_kill(ENEMY_BOSS4_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BOSS4_TURRET3]].attribute [ATTRIB_BO4TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_BOSS4_TURRET4] != -1)
   {
    score_kill(ENEMY_BOSS4_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_BOSS4_TURRET4]].attribute [ATTRIB_BO4TURRET_MAIN] = -1;
   }
   break;
  case ENEMY_BOSS4_TURRET:
   if (enemy[e].attribute [ATTRIB_BO4TURRET_MAIN] != -1)
   {
//   enemy[enemy[e].attribute [ATTRIB_BO3TURRET_MAIN]].attribute [ATTRIB_BOSS3_DELTA_SPIN] *= -1;
    if (enemy[enemy[e].attribute [ATTRIB_BO4TURRET_MAIN]].attribute [ATTRIB_BOSS4_TURRET1] == e)
     enemy[enemy[e].attribute [ATTRIB_BO4TURRET_MAIN]].attribute [ATTRIB_BOSS4_TURRET1] = -1;
      else
      {
       if (enemy[enemy[e].attribute [ATTRIB_BO4TURRET_MAIN]].attribute [ATTRIB_BOSS4_TURRET2] == e)
        enemy[enemy[e].attribute [ATTRIB_BO4TURRET_MAIN]].attribute [ATTRIB_BOSS4_TURRET2] = -1;
         else
         {
          if (enemy[enemy[e].attribute [ATTRIB_BO4TURRET_MAIN]].attribute [ATTRIB_BOSS4_TURRET3] == e)
           enemy[enemy[e].attribute [ATTRIB_BO4TURRET_MAIN]].attribute [ATTRIB_BOSS4_TURRET3] = -1;
            else
            {
             if (enemy[enemy[e].attribute [ATTRIB_BO4TURRET_MAIN]].attribute [ATTRIB_BOSS4_TURRET4] == e)
              enemy[enemy[e].attribute [ATTRIB_BO4TURRET_MAIN]].attribute [ATTRIB_BOSS4_TURRET4] = -1;
            }
         }
      }
   }
//      else
//       enemy[enemy[e].attribute [ATTRIB_FBTURRET_MAIN]].attribute [ATTRIB_FIREBASE_TURRET2] = -1;
   break;
  case ENEMY_FIREBASE2:
   if (enemy[e].attribute [ATTRIB_FIREBASE2_TURRET1] != -1)
   {
    score_kill(ENEMY_FIREBASE2_TURRET1, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_FIREBASE2_TURRET1]].attribute [ATTRIB_FB2TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_FIREBASE2_TURRET2] != -1)
   {
    score_kill(ENEMY_FIREBASE2_TURRET2, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_FIREBASE2_TURRET2]].attribute [ATTRIB_FB2TURRET_MAIN] = -1;
   }
   break;
  case ENEMY_FIREBASE2_TURRET1:
  case ENEMY_FIREBASE2_TURRET2:
   if (enemy[e].attribute [ATTRIB_FB2TURRET_MAIN] != -1)
   {
    if (enemy[enemy[e].attribute [ATTRIB_FB2TURRET_MAIN]].attribute [ATTRIB_FIREBASE2_TURRET1] == e)
     enemy[enemy[e].attribute [ATTRIB_FB2TURRET_MAIN]].attribute [ATTRIB_FIREBASE2_TURRET1] = -1;
      else
      {
       if (enemy[enemy[e].attribute [ATTRIB_FB2TURRET_MAIN]].attribute [ATTRIB_FIREBASE2_TURRET2] == e)
        enemy[enemy[e].attribute [ATTRIB_FB2TURRET_MAIN]].attribute [ATTRIB_FIREBASE2_TURRET2] = -1;
      }
   }
//      else
//       enemy[enemy[e].attribute [ATTRIB_FBTURRET_MAIN]].attribute [ATTRIB_FIREBASE_TURRET2] = -1;
   break;
  case ENEMY_FIREBASE3:
   if (enemy[e].attribute [ATTRIB_FIREBASE3_TURRET1] != -1)
   {
    score_kill(ENEMY_FIREBASE3_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_FIREBASE3_TURRET1]].attribute [ATTRIB_FB3TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_FIREBASE3_TURRET2] != -1)
   {
    score_kill(ENEMY_FIREBASE3_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_FIREBASE3_TURRET2]].attribute [ATTRIB_FB3TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_FIREBASE3_TURRET3] != -1)
   {
    score_kill(ENEMY_FIREBASE3_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_FIREBASE3_TURRET3]].attribute [ATTRIB_FB3TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_FIREBASE3_TURRET4] != -1)
   {
    score_kill(ENEMY_FIREBASE3_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_FIREBASE3_TURRET4]].attribute [ATTRIB_FB3TURRET_MAIN] = -1;
   }
   break;
  case ENEMY_FIREBASE3_TURRET:
   if (enemy[e].attribute [ATTRIB_FB3TURRET_MAIN] != -1)
   {
    if (enemy[enemy[e].attribute [ATTRIB_FB3TURRET_MAIN]].attribute [ATTRIB_FIREBASE3_TURRET1] == e)
     enemy[enemy[e].attribute [ATTRIB_FB3TURRET_MAIN]].attribute [ATTRIB_FIREBASE3_TURRET1] = -1;
      else
      {
       if (enemy[enemy[e].attribute [ATTRIB_FB3TURRET_MAIN]].attribute [ATTRIB_FIREBASE3_TURRET2] == e)
        enemy[enemy[e].attribute [ATTRIB_FB3TURRET_MAIN]].attribute [ATTRIB_FIREBASE3_TURRET2] = -1;
         else
         {
          if (enemy[enemy[e].attribute [ATTRIB_FB3TURRET_MAIN]].attribute [ATTRIB_FIREBASE3_TURRET3] == e)
           enemy[enemy[e].attribute [ATTRIB_FB3TURRET_MAIN]].attribute [ATTRIB_FIREBASE3_TURRET3] = -1;
            else
            {
             if (enemy[enemy[e].attribute [ATTRIB_FB3TURRET_MAIN]].attribute [ATTRIB_FIREBASE3_TURRET4] == e)
              enemy[enemy[e].attribute [ATTRIB_FB3TURRET_MAIN]].attribute [ATTRIB_FIREBASE3_TURRET4] = -1;
            }
         }
      }
   }
//      else
//       enemy[enemy[e].attribute [ATTRIB_FBTURRET_MAIN]].attribute [ATTRIB_FIREBASE_TURRET2] = -1;
   break;
  case ENEMY_FIGHTER4:
   if (enemy[e].attribute [ATTRIB_FIGHTER4_TURRET1] != -1)
   {
    score_kill(ENEMY_FIGHTER4_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_FIGHTER4_TURRET1]].attribute [ATTRIB_FI4TURRET_MAIN] = -1;
   }
   if (enemy[e].attribute [ATTRIB_FIGHTER4_TURRET2] != -1)
   {
    score_kill(ENEMY_FIGHTER4_TURRET, SUBTYPE_NONE, scoring);
    enemy[enemy[e].attribute [ATTRIB_FIGHTER4_TURRET2]].attribute [ATTRIB_FI4TURRET_MAIN] = -1;
   }
   break;
  case ENEMY_FIGHTER4_TURRET:
   if (enemy[e].attribute [ATTRIB_FI4TURRET_MAIN] != -1)
   {
    if (enemy[enemy[e].attribute [ATTRIB_FI4TURRET_MAIN]].attribute [ATTRIB_FIGHTER4_TURRET1] == e)
     enemy[enemy[e].attribute [ATTRIB_FI4TURRET_MAIN]].attribute [ATTRIB_FIGHTER4_TURRET1] = -1;
      else
//    if (enemy[enemy[e].attribute [ATTRIB_FI4TURRET_MAIN]].attribute [ATTRIB_FIGHTER4_TURRET2] == e)
       enemy[enemy[e].attribute [ATTRIB_FI4TURRET_MAIN]].attribute [ATTRIB_FIGHTER4_TURRET2] = -1;
   }
   break;
 }

 if (enemy[e].target == TARGET_PRIMARY && arena.game_over == 0)
 {
  arena.targets_left_total --;

//  if (check_for_targets() == 0)
  if (arena.targets_left_total == 0)
  {
   finish_level();
  }
 }

}


int check_for_targets(void)
{
// int i;

/* for (i = 0; i < NO_ENEMIES; i ++)
 {
  if (enemy[i].type != ENEMY_NONE && enemy[i].target == TARGET_PRIMARY)
   return 1;
 }*/

// if (arena.targets_left [0] > 0 || arena.targets_left [1] > 0 || arena.targets_left [2] > 0)
//  return 1;

 return 0;

}

/*

int detect_collision(int b, int things [2])
{

 int x2 = bullet[b].x - bullet[b].x_speed;
 int y2 = bullet[b].y - bullet[b].y_speed;
 int i;

 for (i = 0; i < NO_ACTORS; i ++)
 {
// I tried to write an accurate collision detector but failed; all my
//  ideas would've been too slow. So we get this horrible thing which
//  is woefully inaccurate at high relative speeds:
 if (actor[i].in_play == 0)
  continue;
 if (bullet[b].owner == i)// && bullet[b].left_owner < 10)
  continue;
 if (hypot(actor[i].x - bullet[b].x, actor[i].y - bullet[b].y) <= actor[i].radius * GRAIN)
 {
  things [0] = bullet[b].x;
  things [1] = bullet[b].y;
  return i;
 }
 if (hypot(actor[i].x - x2, actor[i].y - y2) <= actor[i].radius * GRAIN)
 {
  things [0] = x2;
  things [1] = y2;
  return i;
 }
 
 }

 return -1;

}
*/


void enemy_sound(int e, int sound)
{

 switch(sound)
 {
  case ESOUND_EXPLODE1:
  play_sound_pos(WAV_BANG1, 600 + grand(500), 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_WOBBLE:
  play_sound_pos(WAV_WOBBLE, 800 + grand(800), 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_LONG_WOBBLE:
  play_sound_pos(WAV_LONG_WOBBLE, 800 + grand(800), 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_EXPLODE2:
  play_sound_pos(WAV_WHINE, 700 + grand(800), 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_BOSS1:
  play_sound_pos(WAV_THRUM, 1000, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_BOSS2:
  play_sound_pos(WAV_BOSS2, 1000 + grand(100), 255, enemy[e].x, enemy[e].y);
  break;
/*  case ESOUND_STING4:
  play_sound_pos(WAV_, 800 + grand(400), 255, enemy[e].x, enemy[e].y);
  break;*/
  case ESOUND_BLAST:
  play_sound_pos(WAV_BLAST, 700 + grand(500), 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_ZAPNOTE1:
  play_sound_pos(WAV_ZAPNOTE1, 1000, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_ZAPNOTE2:
//  play_sound_pos(WAV_ZAPNOTE2, 700 + grand(500), 255, enemy[e].x, enemy[e].y);
  play_sound_pos(WAV_ZAPNOTE1, 1000, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_MINE:
  play_sound_pos(WAV_MINE, 500 + grand(300), 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_SHORTZAP:
  play_sound_pos(WAV_SHORTZAP, 1000, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_SHORTZAP2:
  play_sound_pos(WAV_SHORTZAP2, 1000, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_BLAT:
  play_sound_pos(WAV_BLAT, 1000, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_TORPEDO:
  play_sound_pos(WAV_TORPEDO, 1000, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_GREENSEEKER1:
  play_sound_pos(WAV_GREENSEEKER1, 1000, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_GREENSEEKER2:
  play_sound_pos(WAV_GREENSEEKER2, 1000, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_MINEBANG:
  play_sound_pos(WAV_MINEBANG, 800 + grand(400), 255, enemy[e].x, enemy[e].y);
  break;
 }

}

void enemy_soundf(int e, int sound, int freq)
{
 switch(sound)
 {
  case ESOUND_EXPLODE1:
  play_sound_pos(WAV_BANG1, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_STING:
  play_sound_pos(WAV_STING, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_HARD_ZAP:
  play_sound_pos(WAV_HARD_ZAP, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_BLORT:
  play_sound_pos(WAV_BLORT, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_BLAST:
  play_sound_pos(WAV_BLORT, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_ZAPNOTE1:
  play_sound_pos(WAV_ZAPNOTE1, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_ZAPNOTE2:
  play_sound_pos(WAV_ZAPNOTE2, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_SHORTZAP:
  play_sound_pos(WAV_SHORTZAP, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_SHORTZAP2:
  play_sound_pos(WAV_SHORTZAP2, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_BLAT:
  play_sound_pos(WAV_BLAT, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_SUN:
  play_sound_pos(WAV_SUN, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_CROAK:
  play_sound_pos(WAV_CROAK, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_BOSS3_1:
  play_sound_pos(WAV_BOSS3_1, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_BOSS3_2:
  play_sound_pos(WAV_BOSS3_1, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_BOSS3_3:
  play_sound_pos(WAV_BOSS3_1, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_BOSS2:
  play_sound_pos(WAV_BOSS2, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_PLASMA:
  play_sound_pos(WAV_PLASMA, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_SEEKMINE:
  play_sound_pos(WAV_SEEKMINE, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_ORBITAL:
  play_sound_pos(WAV_ORBITAL, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_MISSILE:
  play_sound_pos(WAV_TUBE, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_MINE:
  play_sound_pos(WAV_MINE, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_WARP_IN:
  play_sound_pos(WAV_WARP_IN, freq, 255, enemy[e].x, enemy[e].y);
  break;
  case ESOUND_MUTATE:
  play_sound_pos(WAV_MUTATE, freq, 255, enemy[e].x, enemy[e].y);
  break;
  
 }
}


void score_kill(int etype, int subtype, int p)
{

 if (p == -1)
  return;

 if (subtype == SUBTYPE_NONE)
 {
  gain_score(p, eclass[etype].score);
 }
  else
   gain_score(p, eclass[etype].score * subtype * subtype);
//   player[p].score += eclass[etype].score * subtype * subtype;
  
// bonus lives...

// if (player[p].score > 9999999)
//  player[p].score = 9999999;
}


