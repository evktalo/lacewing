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
    
File: cmds.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - implementation of keyboard commands.
There's no reason for this stuff to be separate from input.c except that
I copied large parts of it from Captain Pork's World of Violence, which
needed a far more complex system because of its AI players. And I never
got around to changing it.

*/


#include "allegro.h"

#include <math.h>

#include "config.h"

#include "globvars.h"

#include "stuff.h"
#include "bullet.h"
#include "cloud.h"
#include "actor.h"
#include "display.h"
#include "sound.h"

#include "palette.h"

enum
{
FIRE_BOTH,
FIRE_CANNON_ONLY,
FIRE_MISSILE_ONLY
};

enum
{
ASOUND_CANNON,
ASOUND_BOMBS,
ASOUND_BLAT,
ASOUND_TUBE
};

#define BOMB_ANGLE_1 128
#define BOMB_ANGLE_2 256


const char *upgrade_large_name [MAX_UPGRADES] =
{
"No}",
"Speed}",
"Armour}",
"Power}",
"Autofire}",
"Multifire}",
"Projector}",
"Warhead}",
"Seeker}",
"Rocket}",
"Tubes}",
"Slide}",
"Bomb}",
"Launcher}",
"Loader}",
"Special}",
"Turret}",
"Sidekicks}",
"Heavy} Cannon}",
"Orbital}",
"Backfire}",
"Shield}",
"Retro}",
"Roamer}",
"Minimissiles}",
"Drift}"



};


void actor_thrust(int thractor);
void actor_slide(int thractor, int direction);
void actor_turn(int thractor, int direction);
void actor_brake(int thractor);
void actor_shoot(int sactor, int which_weapon);
void fire_cannon(int sactor);
void cannon_shot(int sactor, int btype, int damage, int timer, int mass,
 int angle, int status, int seed, int speed, int colours [4], int displaced);
void fire_missile(int sactor);
void missile_shot(int sactor, int btype, int damage, int timer, int mass,
 int angle, int status, int seed, int speed, int colours [4], int displaced,
 int special5);
void fire_bomb(int sactor);
void bomb_shot(int sactor, int btype, int damage, int timer, int mass,
 int angle, int status, int seed, int speed, int colours [4], int displaced);
void actor_upgrade(int uactor);

void exhaust_colours(int level, int passing_colours [5]);

void fire_turret(int sactor);
void fire_heavy_cannon(int sactor);
void fire_backfire(int sactor);
void fire_sidekicks(int a);

void actor_drive(int a, int level, int distance_x, int distance_y, int angle, int thrust, int doubled);

void upgrade_sidekicks(int a);
void actor_sound(int a, int sound);
void make_drive_sound(int a, int drive);

void enact_commands(void)
{
 int i,j;

 for (i = 0; i < NO_ACTORS; i ++)
 {
  if (actor[i].in_play == 0)
   continue;
  actor[i].engine_demand = 1;
  actor[i].dragging = 0;
  
  if (actor[i].actor_cmd [CMD_THRUST] == 1)
   actor[i].engine_demand ++;
  if (actor[i].actor_cmd [CMD_BRAKE] == 1 && actor[i].upgraded_system [UPG_RETRO] > 0)
   actor[i].engine_demand ++;
  if (actor[i].upgraded_system [UPG_SLIDE] > 0)
  {
   if (actor[i].actor_cmd [CMD_LEFT1] == 1)
    actor[i].engine_demand ++;
   if (actor[i].actor_cmd [CMD_RIGHT1] == 1)
    actor[i].engine_demand ++;
  }
  // this makes sure available engine capacity is split between all active thrusters
   
  for (j = 0; j < NO_CMDS; j ++)
  {
   if (actor[i].actor_cmd [j] == 1)
   {
    switch(j)
    {
     case CMD_THRUST:
     actor_thrust(i);
     break;
     case CMD_LEFT:
     actor_turn(i, -1);
     break;
     case CMD_RIGHT:
     actor_turn(i, 1);
     break;
     case CMD_LEFT1:
     actor_slide(i, -1);
     break;
     case CMD_RIGHT1:
     actor_slide(i, 1);
     break;
     case CMD_BRAKE:
     actor_brake(i);
     break;
     case CMD_FIRE1:
     if (player[actor[i].controller].link_fire == 0)
      actor_shoot(i, FIRE_CANNON_ONLY);
       else
        actor_shoot(i, FIRE_BOTH);
     break;
     case CMD_FIRE2:
     actor_shoot(i, FIRE_MISSILE_ONLY);
     break;
     case CMD_UPGRADE:
     actor_upgrade(i);
     break;
    }

/*
enum
{
CMD_THRUST,
CMD_LEFT,
CMD_RIGHT,
CMD_SYSTEM1,
CMD_SYSTEM2,
CMD_SYSTEM3,
CMD_BRAKE,
CMD_LEFT1,
CMD_RIGHT1,
CMD_LEFT2,
CMD_RIGHT2,
CMD_CHARGE_SHIELD
};
*/

   }
   actor[i].actor_cmd [j] = 0;
  }
 }

}

void actor_slide(int thractor, int direction)
{
 if (actor[thractor].upgraded_system [UPG_SLIDE] == 0)
  return;
// int x_accel = cos(angle_to_radians(actor[thractor].angle) + (PI / 2) * direction) * actor[thractor].slide;
// int y_accel = sin(angle_to_radians(actor[thractor].angle) + (PI / 2) * direction) * actor[thractor].slide;
/* int x_accel = cos(angle_to_radians(actor[thractor].angle) + PI / 4 + (PI / 2) * direction) * actor[thractor].slide;
 int y_accel = sin(angle_to_radians(actor[thractor].angle) + PI / 4 + (PI / 2) * direction) * actor[thractor].slide;
 actor[thractor].x_speed += x_accel;
 actor[thractor].y_speed += y_accel;

 int passing_colours [5];

 exhaust_colours(actor[thractor].upgraded_system [UPG_SLIDE - 1], passing_colours);

 create_cloud(CLOUD_SPECK,
 actor[thractor].x,// - (actor[thractor].x_speed - x_accel * 10),
 actor[thractor].y,// - (actor[thractor].y_speed - y_accel * 10),
  0, 0,
  actor[thractor].x_speed - x_accel * 20 + (100 - grand(201)),
  actor[thractor].y_speed - y_accel * 20 + (100 - grand(201)),
  10 + grand(11),1,0, 0, 0, 0, passing_colours);

*/
// int passing_colours [5];
 int thrust_amount = 72;
 thrust_amount += actor[thractor].upgraded_system [UPG_SLIDE] * 13;
 thrust_amount /= actor[thractor].engine_demand;

 actor_drive(thractor, actor[thractor].upgraded_system [UPG_SLIDE] - 1, actor[thractor].slide_distance_x, actor[thractor].slide_distance_y, actor[thractor].angle + ANGLE_QUARTER + ANGLE_QUARTER * direction, thrust_amount, 0);
// actor_sound(thractor, 2);
 make_drive_sound(thractor, DRIVE_SLIDE);

/*
 float xcos = cos(angle_to_radians(actor[thractor].angle) + (PI / 2) + (PI/2) * direction);
 float ysin = sin(angle_to_radians(actor[thractor].angle) + (PI / 2) + (PI/2) * direction);
 int x_accel = xcos * thrust_amount / actor[thractor].engine_demand;
 int y_accel = ysin * thrust_amount / actor[thractor].engine_demand;
 actor[thractor].x_speed -= x_accel;
 actor[thractor].y_speed -= y_accel;

 exhaust_colours(actor[thractor].upgraded_system [UPG_SLIDE] - 1, passing_colours);
 
 create_cloud(CLOUD_SPECK,
 actor[thractor].x + xcos * 5 * GRAIN,// - (actor[thractor].x_speed - x_accel * 10),
 actor[thractor].y + ysin * 3 * GRAIN,// - (actor[thractor].y_speed - y_accel * 10),
  0, 0,
//  actor[thractor].x_speed - x_accel * 20 + (100 - grand(201)),
//  actor[thractor].y_speed - y_accel * 20 + (100 - grand(201)),
  actor[thractor].x_speed + x_accel * 20 + (300 - grand(601)),
  actor[thractor].y_speed + y_accel * 20 + (300 - grand(601)),
  10 + grand(11),1,0, 0, 0, 0, passing_colours);
*/

}

void actor_brake(int thractor)
{
// int passing_colours [5];

 if (actor[thractor].upgraded_system [UPG_RETRO] > 0)
 {
//  int passing_colours [5];
  int thrust_amount = 72;
  thrust_amount += actor[thractor].upgraded_system [UPG_RETRO] * 13;
  thrust_amount /= actor[thractor].engine_demand;
  // level 1 retro gives same thrust as level 0 speed; l2->1, etc
  
  actor_drive(thractor, actor[thractor].upgraded_system [UPG_RETRO] - 1, actor[thractor].retro_distance_x, actor[thractor].retro_distance_y, actor[thractor].angle - ANGLE_QUARTER, thrust_amount, 0);

  make_drive_sound(thractor, DRIVE_RETRO);
//  actor_sound(thractor, 3);
/*
  float xcos = cos(angle_to_radians(actor[thractor].angle) - PI/2);
  float ysin = sin(angle_to_radians(actor[thractor].angle) - PI/2);
  int x_accel = xcos * thrust_amount / actor[thractor].engine_demand;
  int y_accel = ysin * thrust_amount / actor[thractor].engine_demand;
  actor[thractor].x_speed -= x_accel;
  actor[thractor].y_speed -= y_accel;

  exhaust_colours(actor[thractor].upgraded_system [UPG_RETRO] - 1, passing_colours);
 
  create_cloud(CLOUD_SPECK,
  actor[thractor].x + xcos * 5 * GRAIN,// - (actor[thractor].x_speed - x_accel * 10),
  actor[thractor].y + ysin * 3 * GRAIN,// - (actor[thractor].y_speed - y_accel * 10),
   0, 0,
   actor[thractor].x_speed + x_accel * 20 + (300 - grand(601)),
   actor[thractor].y_speed + y_accel * 20 + (300 - grand(601)),
   10 + grand(11),1,0, 0, 0, 0, passing_colours);
*/
   return;
 }
 // else must be drag field...
 actor[thractor].dragging = 1;

}



void actor_thrust(int thractor)
{
// int passing_colours [5];
 int thrust_amount = 80;// 80
 thrust_amount += actor[thractor].upgraded_system [UPG_SPEED] * 13;
 thrust_amount /= actor[thractor].engine_demand;
/*
 float xcos = cos(angle_to_radians(actor[thractor].angle) + PI/2);
 float ysin = sin(angle_to_radians(actor[thractor].angle) + PI/2);
 int x_accel = xcos * thrust_amount / actor[thractor].engine_demand;
 int y_accel = ysin * thrust_amount / actor[thractor].engine_demand;
 actor[thractor].x_speed -= x_accel;
 actor[thractor].y_speed -= y_accel;

exhaust_colours(actor[thractor].upgraded_system [UPG_SPEED], passing_colours);
 
 create_cloud(CLOUD_SPECK,
 actor[thractor].x + xcos * 9 * GRAIN,// - (actor[thractor].x_speed - x_accel * 10),
 actor[thractor].y + ysin * 7 * GRAIN,// - (actor[thractor].y_speed - y_accel * 10),
  0, 0,
//  actor[thractor].x_speed - x_accel * 20 + (100 - grand(201)),
//  actor[thractor].y_speed - y_accel * 20 + (100 - grand(201)),
  actor[thractor].x_speed + x_accel * 20 + (300 - grand(601)),
  actor[thractor].y_speed + y_accel * 20 + (300 - grand(601)),
  10 + grand(11),1,0, 0, 0, 0, passing_colours);

*/
 actor_drive(thractor, actor[thractor].upgraded_system [UPG_SPEED], actor[thractor].exhaust_distance_x, actor[thractor].exhaust_distance_y, actor[thractor].angle + ANGLE_QUARTER, thrust_amount, 1);

// actor_sound(thractor, 1);
 make_drive_sound(thractor, DRIVE_THRUST);
}

// need to already have factored engine demand into thrust
void actor_drive(int a, int level, int distance_x, int distance_y, int angle, int thrust, int doubled)
{
 int passing_colours [5];

 float xcos = cos(angle_to_radians(angle));
 float ysin = sin(angle_to_radians(angle));
// float xcos2, ysin2;
 int x_accel = xcos * thrust;
 int y_accel = ysin * thrust;
 actor[a].x_speed -= x_accel;
 actor[a].y_speed -= y_accel;

 float xcos2, ysin2;
 int displace = 0;

 switch(actor[a].ship)
 {
  case SHIP_LENTIL:
   displace = 4;
   break;
  case SHIP_CAPYBARA:
   displace = 3;
   break;
  case SHIP_GODBOTHERER:
   displace = 1;
   break;
  case SHIP_BOTULUS:
   displace = 3;
   break;
 }

 exhaust_colours(level, passing_colours);

 if (doubled == 0 || displace == 0) //FALSE) //doubled == 0 || (actor[a].ship != SHIP_LENTIL && actor[a].ship != SHIP_CAPYBARA))
 {
  create_cloud(CLOUD_SPECK,
  actor[a].x + xcos * distance_x * GRAIN,
  actor[a].y + ysin * distance_y * GRAIN,
   0, 0,
   actor[a].x_speed + x_accel * 20 + (300 - grand(601)),
   actor[a].y_speed + y_accel * 20 + (300 - grand(601)),
   10 + grand(11),1,0, 0, 0, 0, passing_colours);
 }
  else
  {
   if (counter % 2 == 0)
   {
    xcos2 = cos(angle_to_radians(angle + ANGLE_QUARTER));
    ysin2 = sin(angle_to_radians(angle + ANGLE_QUARTER));
   }
    else
    {
     xcos2 = cos(angle_to_radians(angle - ANGLE_QUARTER));
     ysin2 = sin(angle_to_radians(angle - ANGLE_QUARTER));
    }
   create_cloud(CLOUD_SPECK,
    actor[a].x + (xcos * distance_x * GRAIN) + (xcos2 * displace * GRAIN),
    actor[a].y + (ysin * distance_y * GRAIN) + (ysin2 * displace * GRAIN),
    0, 0,
    actor[a].x_speed + x_accel * 20 + (300 - grand(601)),
    actor[a].y_speed + y_accel * 20 + (300 - grand(601)),
    10 + grand(11),1,0, 0, 0, 0, passing_colours);

  }

}

void exhaust_colours(int level, int passing_colours [5])
{
 switch(level)
 {
  default:
  case 0:
   passing_colours [0] = COLOUR_YELLOW8;
   passing_colours [1] = COLOUR_RED8;
   passing_colours [2] = COLOUR_RED6;
   passing_colours [3] = COLOUR_RED4;
   passing_colours [4] = COLOUR_RED3;
   break;
  case 1:
   passing_colours [0] = COLOUR_YELLOW8;
   passing_colours [1] = COLOUR_ORANGE8;
   passing_colours [2] = COLOUR_ORANGE6;
   passing_colours [3] = COLOUR_ORANGE4;
   passing_colours [4] = COLOUR_ORANGE3;
   break;
  case 2:
   passing_colours [0] = COLOUR_YELLOW8;
   passing_colours [1] = COLOUR_YELLOW6;
   passing_colours [2] = COLOUR_YELLOW4;
   passing_colours [3] = COLOUR_YELLOW3;
   passing_colours [4] = COLOUR_YELLOW2;
   break;
  case 3:
   passing_colours [0] = COLOUR_YELLOW8;
   passing_colours [1] = COLOUR_GREEN8;
   passing_colours [2] = COLOUR_GREEN6;
   passing_colours [3] = COLOUR_GREEN4;
   passing_colours [4] = COLOUR_GREEN2;
   break;
  case 4:
   passing_colours [0] = COLOUR_BLUE8;
   passing_colours [1] = COLOUR_BLUE6;
   passing_colours [2] = COLOUR_BLUE4;
   passing_colours [3] = COLOUR_BLUE3;
   passing_colours [4] = COLOUR_BLUE2;
   break;
  case 5:
   passing_colours [0] = COLOUR_WHITE;
   passing_colours [1] = COLOUR_BLUE8;
   passing_colours [2] = COLOUR_BLUE6;
   passing_colours [3] = COLOUR_BLUE4;
   passing_colours [4] = COLOUR_BLUE3;
   break;
 }

}


void actor_turn(int thractor, int direction)
{
 int turned = 85;
 turned += actor[thractor].upgraded_system [UPG_SPEED] * 15;

 int target_angle = turned * direction;
 
/* if (actor[thractor].base_angle + target_angle < 0)
  actor[thractor].base_angle += 10240;
   else
   {
    if (actor[thractor].base_angle + target_angle > 10240)
     actor[thractor].base_angle -= 10240;
   }*/
  
 actor[thractor].base_angle += target_angle;
 
 if (actor[thractor].base_angle < 0)
  actor[thractor].base_angle += 10240;
   else
   {
    if (actor[thractor].base_angle > 10240)
     actor[thractor].base_angle -= 10240;
   }
   
 actor[thractor].angle = (actor[thractor].base_angle / 10) % 1024;
}


void actor_shoot(int sactor, int which_weapon)
{

 int recycled = 15;

 if (actor[sactor].recycle1 == 0 && (which_weapon == FIRE_BOTH || which_weapon == FIRE_CANNON_ONLY))
 {
  fire_cannon(sactor);

  recycled = 15;

  if (actor[sactor].upgraded_system [UPG_AUTOFIRE] > 0)
  {
   recycled = 12;
   if (actor[sactor].upgraded_system [UPG_AUTOFIRE] == 2)
    recycled = 10;
   if (actor[sactor].upgraded_system [UPG_AUTOFIRE] == 3)
    recycled = 9;
   if (actor[sactor].upgraded_system [UPG_AUTOFIRE] == 4)
    recycled = 8;
   if (actor[sactor].upgraded_system [UPG_AUTOFIRE] == 5)
    recycled = 7;
  }
  actor[sactor].recycle1 += recycled;
 }

 if (actor[sactor].secondary_weapon == SECONDARY_MISSILE && actor[sactor].recycle2 == 0 && (which_weapon == FIRE_BOTH || which_weapon == FIRE_MISSILE_ONLY))
 {
  fire_missile(sactor);

  recycled = 132;

  if (actor[sactor].upgraded_system [UPG_LOADER] > 0)
  {
   recycled = 85;
   if (actor[sactor].upgraded_system [UPG_LOADER] == 2)
    recycled = 65;
   if (actor[sactor].upgraded_system [UPG_LOADER] == 3)
    recycled = 51;
   if (actor[sactor].upgraded_system [UPG_LOADER] == 4)
    recycled = 40;
   if (actor[sactor].upgraded_system [UPG_LOADER] == 5)
    recycled = 30;
  }
  
  actor[sactor].recycle2 += recycled;
 }

 if (actor[sactor].secondary_weapon == SECONDARY_BOMB && actor[sactor].recycle2 == 0 && (which_weapon == FIRE_BOTH || which_weapon == FIRE_MISSILE_ONLY))
 {
  fire_bomb(sactor);

  recycled = 132;

/*  if (actor[sactor].upgraded_system [UPG_AUTOFIRE] > 0)
  {
   recycled = 11;
   if (actor[sactor].upgraded_system [UPG_AUTOFIRE] == 2)
    recycled = 8;
   if (actor[sactor].upgraded_system [UPG_AUTOFIRE] == 3)
    recycled = 6;
   if (actor[sactor].upgraded_system [UPG_AUTOFIRE] == 4)
    recycled = 5;
   if (actor[sactor].upgraded_system [UPG_AUTOFIRE] == 5)
    recycled = 4;
  }*/
  actor[sactor].recycle2 += recycled;
 }

 if (which_weapon == FIRE_BOTH || which_weapon == FIRE_CANNON_ONLY)
 {
 if (actor[sactor].upgraded_system [UPG_TURRET] > 0
     && actor[sactor].turret_recycle == 0)
 {
  fire_turret(sactor);

  recycled = 12;
   if (actor[sactor].upgraded_system [UPG_TURRET] == 2)
    recycled = 11;
   if (actor[sactor].upgraded_system [UPG_TURRET] == 3)
    recycled = 10;
   if (actor[sactor].upgraded_system [UPG_TURRET] == 4)
    recycled = 9;
   if (actor[sactor].upgraded_system [UPG_TURRET] == 5)
    recycled = 8;

   actor[sactor].turret_recycle += recycled;
    
 }

 if (actor[sactor].upgraded_system [UPG_HEAVY] > 0
     && actor[sactor].heavy_recycle == 0)
 {
  fire_heavy_cannon(sactor);

  recycled = 25;
   if (actor[sactor].upgraded_system [UPG_HEAVY] == 2)
    recycled = 20;
   if (actor[sactor].upgraded_system [UPG_HEAVY] == 3)
    recycled = 17;
   if (actor[sactor].upgraded_system [UPG_HEAVY] == 4)
    recycled = 14;
   if (actor[sactor].upgraded_system [UPG_HEAVY] == 5)
    recycled = 12;

   actor[sactor].heavy_recycle += recycled;
    
 }

 if (actor[sactor].upgraded_system [UPG_BACKFIRE] > 0
     && actor[sactor].backfire_recycle == 0)
 {
  fire_backfire(sactor);

  recycled = 11;
   if (actor[sactor].upgraded_system [UPG_BACKFIRE] == 2)
    recycled = 10;
   if (actor[sactor].upgraded_system [UPG_BACKFIRE] == 3)
    recycled = 9;
   if (actor[sactor].upgraded_system [UPG_BACKFIRE] == 4)
    recycled = 8;
   if (actor[sactor].upgraded_system [UPG_BACKFIRE] == 5)
    recycled = 7;

   actor[sactor].backfire_recycle += recycled;
    
 }

 if (actor[sactor].upgraded_system [UPG_SIDEKICK] > 0
     && actor[sactor].sidekick_recycle == 0)
 {
  fire_sidekicks(sactor);

  recycled = 14;
   if (actor[sactor].upgraded_system [UPG_SIDEKICK] == 2)
    recycled = 13;
   if (actor[sactor].upgraded_system [UPG_SIDEKICK] == 3)
    recycled = 12;
   if (actor[sactor].upgraded_system [UPG_SIDEKICK] == 4)
    recycled = 11;
   if (actor[sactor].upgraded_system [UPG_SIDEKICK] == 5)
    recycled = 10;

   actor[sactor].sidekick_recycle += recycled;
    
 }
 }

}



void fire_cannon(int sactor)
{

 int colours [4] = {COLOUR_ORANGE8, COLOUR_ORANGE6, COLOUR_ORANGE4, COLOUR_ORANGE2};

 if (actor[sactor].upgraded_system [UPG_AUTOFIRE] > 0
     || actor[sactor].upgraded_system [UPG_MULTIFIRE] > 0)
 {
/*  if (actor[sactor].upgraded_system [UPG_MULTIFIRE] == actor[sactor].upgraded_system [UPG_AUTOFIRE])
  {
   if (actor[sactor].upgraded_system [UPG_MULTIFIRE] == 5)
   {
    colours [0] = COLOUR_PURPLE8;
    colours [1] = COLOUR_PURPLE7;
    colours [2] = COLOUR_PURPLE5;
    colours [3] = COLOUR_PURPLE3;
   }
    else
    {
     colours [0] = COLOUR_BLUE8;
     colours [1] = COLOUR_BLUE6;
     colours [2] = COLOUR_BLUE4;
     colours [3] = COLOUR_BLUE2;
    }
  }
   else
  {*/
   if (actor[sactor].upgraded_system [UPG_MULTIFIRE] > actor[sactor].upgraded_system [UPG_AUTOFIRE])
   {
    colours [0] = COLOUR_GREEN8;
    colours [1] = COLOUR_GREEN6;
    colours [2] = COLOUR_GREEN4;
    colours [3] = COLOUR_GREEN2;
   }
    else
    {
     colours [0] = COLOUR_YELLOW8;
     colours [1] = COLOUR_YELLOW6;
     colours [2] = COLOUR_YELLOW4;
     colours [3] = COLOUR_YELLOW2;
    }
//  }
 }


 int btype = BULLET_ZAP;
 int damage = 100;
 if (actor[sactor].upgraded_system [UPG_POWER] > 0)
 {
  damage += actor[sactor].upgraded_system [UPG_POWER] * 25;
  btype = BULLET_POWERED;
  switch(actor[sactor].upgraded_system [UPG_POWER])
  {
   case 1:
//    colours [0] = COLOUR_RED8;
//    colours [1] = COLOUR_RED6;
//    colours [2] = COLOUR_RED4;
//    colours [3] = TRANS_LRED;
    colours [3] = COLOUR_RED8;
    break;
   case 2:
//    colours [0] = COLOUR_YELLOW8;
//    colours [1] = TRANS_LRED;
//    colours [1] = COLOUR_YELLOW6;
//    colours [2] = COLOUR_YELLOW4;
//    colours [3] = TRANS_YELLOW;
    colours [3] = COLOUR_YELLOW8;
    break;
   case 3:
//    colours [0] = COLOUR_GREEN8;
//    colours [1] = TRANS_LGREEN;
//    colours [1] = COLOUR_GREEN6;
//    colours [2] = COLOUR_GREEN4;
//    colours [3] = TRANS_LGREEN;
    colours [3] = COLOUR_GREEN8;
    break;
   case 4:
//    colours [0] = COLOUR_BLUE8;
//    colours [1] = COLOUR_BLUE6;
//    colours [2] = COLOUR_BLUE4;
//    colours [3] = TRANS_LBLUE;
    colours [3] = COLOUR_BLUE8;
    break;
   case 5:
//    colours [0] = COLOUR_WHITE;
//    colours [1] = TRANS_LBLUE;
//    colours [1] = COLOUR_BLUE8;
//    colours [2] = COLOUR_BLUE6;
//    colours [3] = TRANS_WHITE;
    colours [3] = COLOUR_WHITE;
    break;
/*   case 1:
    colours [0] = COLOUR_RED7;
    colours [1] = COLOUR_RED6;
    colours [2] = COLOUR_RED8;
    colours [3] = COLOUR_YELLOW4;
    break;
   case 2:
    colours [0] = COLOUR_YELLOW8;
//    colours [1] = TRANS_LRED;
    colours [1] = COLOUR_RED7;
    colours [2] = COLOUR_RED8;
    colours [3] = COLOUR_YELLOW4;
    break;
   case 3:
    colours [0] = COLOUR_YELLOW8;
//    colours [1] = TRANS_LGREEN;
    colours [1] = COLOUR_GREEN7;
    colours [2] = COLOUR_GREEN8;
    colours [3] = COLOUR_YELLOW4;
    break;
   case 4:
    colours [0] = COLOUR_BLUE8;
    colours [1] = TRANS_DBLUE;
    colours [2] = COLOUR_BLUE8;
    colours [3] = COLOUR_BLUE4;
    break;
   case 5:
    colours [0] = COLOUR_WHITE;
//    colours [1] = TRANS_LBLUE;
    colours [1] = COLOUR_BLUE8;
    colours [2] = COLOUR_WHITE;
    colours [3] = COLOUR_WHITE;
    break;*/
  }
 }
 int timer = 20 - actor[sactor].upgraded_system [UPG_PROJECT] * 2;
/* if (actor[sactor].upgraded_system [UPG_PROJECT] > 1)
  timer --;
 if (actor[sactor].upgraded_system [UPG_PROJECT] > 3)
  timer --;*/
 int mass = 10;
 int status = 0;
 int speed = 16000;
 unsigned char seed = counter;

 switch(actor[sactor].upgraded_system [UPG_MULTIFIRE])
 {
  case 0:
/*   if (actor[sactor].upgraded_system [UPG_AUTOFIRE] > 0)
   {
    if (actor[sactor].cannon_status == 0)
    {
     cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + scatter,
      status, seed, speed, colours, -1);
     actor[sactor].cannon_status = 1;
    } else
     {
      cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + scatter,
       status, seed, speed, colours, 1);
      actor[sactor].cannon_status = 0;
     }
   }
    else*/
    {
     cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -1);
     cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 1);
    }
   break;
  case 1:
/*   if (actor[sactor].upgraded_system [UPG_AUTOFIRE] > 0)
   {
    if (actor[sactor].cannon_status == 0)
    {
     cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0);
     actor[sactor].cannon_status = 1;
    } else
     {
      cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
       status, seed, speed, colours, 1);
      actor[sactor].cannon_status = 0;
     }
   }
    else*/
    {
     cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0);
     cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - ANGLE_1_SIXTEENTH,
      status, seed, speed, colours, 0);
     cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + ANGLE_1_SIXTEENTH,
      status, seed, speed, colours, 0);
    }
   break;
  case 2:
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
    status, seed, speed, colours, 1);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
    status, seed, speed, colours, -1);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - ANGLE_1_SIXTEENTH,
    status, seed, speed, colours, 0);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + ANGLE_1_SIXTEENTH,
    status, seed, speed, colours, 0);
   break;
  case 3:
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
    status, seed, speed, colours, 1);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
    status, seed, speed, colours, -1);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - ANGLE_1_SIXTEENTH,
    status, seed, speed, colours, 0);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + ANGLE_1_SIXTEENTH,
    status, seed, speed, colours, 0);
   if (actor[sactor].cannon_status == 0)
   {
    cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - ANGLE_1_EIGHTH,
     status, seed, speed, colours, 0);
    actor[sactor].cannon_status = 1;
   } else
    {
     cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + ANGLE_1_EIGHTH,
      status, seed, speed, colours, 0);
     actor[sactor].cannon_status = 0;
    }
/*   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
    status, seed, speed, colours, 0);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - ANGLE_1_SIXTEENTH,
    status, seed, speed, colours, 0);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + ANGLE_1_SIXTEENTH,
    status, seed, speed, colours, 0);
   if (actor[sactor].cannon_status == 0)
   {
    cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - ANGLE_1_EIGHTH,
     status, seed, speed, colours, 0);
    actor[sactor].cannon_status = 1;
   } else
    {
     cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + ANGLE_1_EIGHTH,
      status, seed, speed, colours, 0);
     actor[sactor].cannon_status = 0;
    }*/
   break;
  case 4:
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
    status, seed, speed, colours, 1);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
    status, seed, speed, colours, -1);
//   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
//    status, seed, speed, colours, 0);
//   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - ANGLE_1_SIXTEENTH,
//    status, seed, speed, colours, 0);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + ANGLE_1_SIXTEENTH,
    status, seed, speed, colours, 0);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - ANGLE_1_SIXTEENTH,
    status, seed, speed, colours, 0);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - ANGLE_1_EIGHTH,
    status, seed, speed, colours, 0);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + ANGLE_1_EIGHTH,
    status, seed, speed, colours, 0);
   break;
  case 5:
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
    status, seed, speed, colours, -1);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
    status, seed, speed, colours, 1);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - ANGLE_1_EIGHTH,
    status, seed, speed, colours, 0);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + ANGLE_1_EIGHTH,
    status, seed, speed, colours, 0);
   if (actor[sactor].cannon_status == 0)
   {
    cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - ANGLE_1_SIXTEENTH,
     status, seed, speed, colours, -1);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - ANGLE_1_SIXTEENTH,
    status, seed, speed, colours, 1);
   cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + ANGLE_1_SIXTEENTH,
    status, seed, speed, colours, 0);
    actor[sactor].cannon_status = 1;
   } else
    {
     cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + ANGLE_1_SIXTEENTH,
      status, seed, speed, colours, -1);
     cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + ANGLE_1_SIXTEENTH,
      status, seed, speed, colours, 1);
     cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - ANGLE_1_SIXTEENTH,
      status, seed, speed, colours, 0);
     actor[sactor].cannon_status = 0;
    }
   break;
 }

 actor_sound(sactor, ASOUND_CANNON);


}

void cannon_shot(int sactor, int btype, int damage, int timer, int mass,
 int angle, int status, int seed, int speed, int colours [4], int displaced)
{

 if (actor[sactor].upgraded_system [UPG_AUTOFIRE] > 0)
 {
  angle += (actor[sactor].upgraded_system [UPG_AUTOFIRE] * 5);
  angle -= grand(actor[sactor].upgraded_system [UPG_AUTOFIRE] * 10);
 }

 timer += grand(4);

 int speed_div = 7;

// if (actor[sactor].upgraded_system [UPG_PROJECT] > 0)
// {
  speed += actor[sactor].upgraded_system [UPG_PROJECT] * 5000;
  speed_div -= actor[sactor].upgraded_system [UPG_PROJECT];
// }

 int xs = actor[sactor].x_speed + cos(angle_to_radians(angle - ANGLE_QUARTER)) * speed / 3;
 int ys = actor[sactor].y_speed + sin(angle_to_radians(angle - ANGLE_QUARTER)) * speed / 3;

 int x = actor[sactor].x;
 int y = actor[sactor].y;

 if (displaced != 0)
 {
  x += cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3;
  y += sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3;
 }

 if (angle == 0)
  angle = 1;

 create_bullet(btype, x, y,
  xs, ys, sactor,
  damage, timer, mass, actor[sactor].angle,
  status, seed, colours, speed_div, 0, 0, 0, 0, 0);

 int cloud_colours [5];
 cloud_colours [0] = colours [0];

 x = actor[sactor].x + cos(angle_to_radians(angle - ANGLE_QUARTER)) * actor[sactor].flash_distance;
 y = actor[sactor].y + sin(angle_to_radians(angle - ANGLE_QUARTER)) * actor[sactor].flash_distance;

 if (displaced != 0)
 {
  x += cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3;
  y += sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3;
 }


    create_cloud(CLOUD_MED_CIRCLE, x, y, 0, 0,
     actor[sactor].x_speed, actor[sactor].y_speed, 300, 50,
     0, 0, 0, 0, cloud_colours);

}

void fire_missile(int sactor)
{

 int colours [4] = {TRANS_GREY, TRANS_DRED, TRANS_LRED, TRANS_YELLOW};

 int btype = BULLET_MISSILE;
 int damage = 20; // misleading; the explosion changes this
 if (actor[sactor].upgraded_system [UPG_WARHEAD] > 0)
 {
  damage += actor[sactor].upgraded_system [UPG_WARHEAD] * 10;
/*  switch(actor[sactor].upgraded_system [UPG_WARHEAD])
  {
   default:
   case 0:
    colours [3] = TRANS_YELLOW;
    break;
   case 1:
    colours [3] = TRANS_LRED;
    break;
   case 2:
    colours [3] = TRANS_LBLUE;
    break;
   case 3:
    colours [3] = TRANS_DBLUE;
    break;
   case 4:
    colours [3] = TRANS_LGREEN;
    break;
   case 5:
    colours [3] = TRANS_PURPLE;
    break;
  }*/
 }
/*
UPG_WARHEAD,
// increases missile damage
// special: cluster warhead
UPG_SEEKER,
// makes missiles track nearest target, then increases turn rate
// special:
//  1. Dauntless (seekers don't explode when target does; acquire new one)
//  2. Smart guidance - go for most important target
//  3. Target Analysis - send only enough missiles to destroy; prefer weaker
//      targets if they'll be destroyed
UPG_ROCKET,
// increases missile acceleration, but not range by much
// special: long range
UPG_TUBES,
// increases number of missiles
// special: extra mini-missiles, ?random directions, not seeking
*/
 int timer = 45;// - actor[sactor].upgraded_system [UPG_ROCKET];
 int mass = 10;
 int status = 0;
 int speed = 1000;
 unsigned char seed = counter;

 if (actor[sactor].angle == 0)
  actor[sactor].angle = 1; // I have no idea why this is necessary

 int specials [7] = {0,6,1,5,2,4,3};



 if (actor[sactor].bomb_status == 1)
 {
  specials [0] = 6;
  specials [1] = 0;
  specials [2] = 5;
  specials [3] = 1;
  specials [4] = 4;
  specials [5] = 2;
  specials [6] = 3;
  actor[sactor].bomb_status = 0;
 }
  else
   actor[sactor].bomb_status = 1;

 switch(actor[sactor].upgraded_system [UPG_TUBES])
 {
/*  case 1:
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0);
     break;*/
  case 0:
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 1, specials [0]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -1, specials [1]);
     break;
  case 1:
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 1, specials [0]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -1, specials [1]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0, specials [2]);
     break;
  case 2:
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 2, specials [0]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -2, specials [1]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 1, specials [2]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -1, specials [3]);
     break;
  case 3:
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0, specials [0]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + 16,
      status, seed, speed, colours, 2, specials [1]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - 16,
      status, seed, speed, colours, -2, specials [2]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 1, specials [3]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -1, specials [4]);
     break;
/*  case 4:
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0, specials [0]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + ANGLE_HALF,
      status, seed, speed, colours, 0, specials [1]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + 16,
      status, seed, speed, colours, 2, specials [2]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - 16,
      status, seed, speed, colours, -2, specials [3]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 1, specials [4]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -1, specials [5]);
     break;*/
  case 4:
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + 16,
      status, seed, speed, colours, 3, specials [0]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - 16,
      status, seed, speed, colours, -3, specials [1]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 2, specials [2]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -2, specials [3]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 1, specials [4]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -1, specials [5]);
     break;
  case 5:
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0, specials [0]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + 16,
      status, seed, speed, colours, 3, specials [1]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - 16,
      status, seed, speed, colours, -3, specials [2]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 2, specials [3]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -2, specials [4]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 1, specials [5]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -1, specials [6]);
     break;
 }
//     cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
//      status, seed, speed, colours, 1);

 actor_sound(sactor, ASOUND_TUBE);

}



void missile_shot(int sactor, int btype, int damage, int timer, int mass,
 int angle, int status, int seed, int speed, int colours [4], int displaced,
 int special5)
{

 int special1;
 int special2;
 int special3;
 int special4;
 special5 *= 2;

 timer += grand(15);

 speed += actor[sactor].upgraded_system [UPG_ROCKET] * 1000;

 special1 = 200 + actor[sactor].upgraded_system [UPG_ROCKET] * 50;

 int xs = actor[sactor].x_speed + cos(angle_to_radians(angle - ANGLE_QUARTER)) * speed / 3;
 int ys = actor[sactor].y_speed + sin(angle_to_radians(angle - ANGLE_QUARTER)) * speed / 3;

 int x = actor[sactor].x;
 int y = actor[sactor].y;

 if (displaced != 0)
 {
//  x += cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3 * displaced;
//  y += sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3 * displaced;
  x += cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER))) * GRAIN * 5 * displaced;
  y += sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER))) * GRAIN * 5 * displaced;
  xs += cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER))) * GRAIN / 8 * displaced;
  ys += sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER))) * GRAIN / 8 * displaced;
 }

 special3 = -1;

 if (actor[sactor].lock != -1)// && enemy[actor[sactor].lock].type != ENEMY_NONE)
 {
  if (actor[sactor].lock == LOCK_ACTOR0 && actor[0].in_play == 1)
   special3 = actor[sactor].lock;
  if (actor[sactor].lock == LOCK_ACTOR1 && actor[1].in_play == 1)
   special3 = actor[sactor].lock;
  if (actor[sactor].lock >= 0 && enemy[actor[sactor].lock].type != ENEMY_NONE)
   special3 = actor[sactor].lock;
 }
 
 special4 = 32 + 16 * actor[sactor].upgraded_system [UPG_SEEKER];
 
 if (angle == 0)
  angle = 1;

 create_bullet(btype, x, y,
  xs, ys, sactor,
  damage, timer, mass, angle,
  status, seed, colours, 1, special1, special2, special3, special4, special5);


}

void fire_bomb(int sactor)
{

 int colours [4] = {TRANS_GREY, TRANS_DRED, TRANS_LRED, TRANS_YELLOW};


// int btype = BULLET_BOMB;
 int damage = 50; // misleading; the explosion changes this
 if (actor[sactor].upgraded_system [UPG_BOMB] > 0)
 {
  damage += actor[sactor].upgraded_system [UPG_BOMB] * 25;
 }
/*
UPG_BOMB,
// increases bomb damage
// special: dispersion (explosion size)
UPG_LAUNCHER,
// adds more bombs (side-bombs are smaller)
// special: cluster bombs (main one only?)
UPG_LOADER,
// increases bomb firing rate
// special: long range
*/
 int timer = 30;// + actor[sactor].upgraded_system [UPG_ROCKET];
 int mass = 10;
 int status = 0;
 int speed = 6500;
 unsigned char seed = counter;

 switch(actor[sactor].upgraded_system [UPG_LAUNCHER])
 {
  case 0:
     bomb_shot(sactor, BULLET_BOMB, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0);
     break;
  case 1:
     bomb_shot(sactor, BULLET_BOMB, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0);
     if (actor[sactor].bomb_status == 0)
     {
      bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle - BOMB_ANGLE_1,
       status, seed, speed, colours, 0);
      actor[sactor].bomb_status = 1;
     } else
      {
       bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + BOMB_ANGLE_1,
        status, seed, speed, colours, 0);
       actor[sactor].bomb_status = 0;
      }
     break;
  case 2:
     bomb_shot(sactor, BULLET_BOMB, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle - BOMB_ANGLE_1,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + BOMB_ANGLE_1,
      status, seed, speed, colours, 0);
     break;
  case 3:
     bomb_shot(sactor, BULLET_BOMB, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle - BOMB_ANGLE_1,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + BOMB_ANGLE_1,
      status, seed, speed, colours, 0);
     if (actor[sactor].bomb_status == 0)
     {
      bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle - BOMB_ANGLE_2,
       status, seed, speed, colours, 0);
      actor[sactor].bomb_status = 1;
     } else
      {
       bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + BOMB_ANGLE_2,
        status, seed, speed, colours, 0);
       actor[sactor].bomb_status = 0;
      }


     break;
  case 4:
     bomb_shot(sactor, BULLET_BOMB, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle - BOMB_ANGLE_1,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + BOMB_ANGLE_1,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle - BOMB_ANGLE_2,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + BOMB_ANGLE_2,
      status, seed, speed, colours, 0);
     break;
  case 5:
     bomb_shot(sactor, BULLET_BOMB, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle - BOMB_ANGLE_1,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + BOMB_ANGLE_1,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle - BOMB_ANGLE_2,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + BOMB_ANGLE_2,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + ANGLE_HALF,
      status, seed, speed, colours, 0);
     break;
 }

 actor_sound(sactor, ASOUND_BOMBS);


}



void bomb_shot(int sactor, int btype, int damage, int timer, int mass,
 int angle, int status, int seed, int speed, int colours [4], int displaced)
{
 int special1;
 int special2;
 int special3;
 int special4;
 int special5;

 timer += grand(4);

//  speed += actor[sactor].upgraded_system [UPG_ROCKET] * 1000;

/*
UPG_BOMB,
// increases bomb damage
// special: dispersion (explosion size)
UPG_LAUNCHER,
// adds more bombs (side-bombs are smaller)
// special: cluster bombs (main one only?)
UPG_LOADER,
// increases bomb firing rate
// special: long range
*/

 special1 = 30000 + actor[sactor].upgraded_system [UPG_BOMB] * 5000;
 // size of bomb's explosion
 if (btype == BULLET_BOMB_SIDE)
 {
  damage /= 2;
  special1 /= 2;
 }
     
 special2 = 0;
// int special1;
 // missiles: acceleration
 // bombs: explosion size
// int special2;
 // missiles: number of clusters
 // bombs: number of clusters


 int xs = actor[sactor].x_speed + cos(angle_to_radians(angle - ANGLE_QUARTER)) * speed / 3;
 int ys = actor[sactor].y_speed + sin(angle_to_radians(angle - ANGLE_QUARTER)) * speed / 3;

 int x = actor[sactor].x;
 int y = actor[sactor].y;

/* if (displaced != 0)
 {
//  x += cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3 * displaced;
//  y += sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3 * displaced;
  x += cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER))) * GRAIN * 5 * displaced;
  y += sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER))) * GRAIN * 5 * displaced;
  xs += cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER))) * GRAIN / 8 * displaced;
  ys += sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER))) * GRAIN / 8 * displaced;
 }*/

 if (angle == 0)
  angle = 1;

 create_bullet(btype, x, y,
  xs, ys, sactor,
  damage, timer, mass, angle,
  status, seed, colours, 1, special1, special2, special3, special4, special5);


}




void fire_turret(int sactor)
{

 int colours [4] = {COLOUR_RED8, COLOUR_RED6, COLOUR_RED4, COLOUR_RED2};

 int damage = 100;
 int timer = 11 + actor[sactor].upgraded_system [UPG_TURRET] * 2;
 int mass = 10;
 int status = 0;
 int speed = 22000;
 unsigned char seed = counter;

// int target = closest_enemy(actor[sactor].x, actor[sactor].y, 100000 + actor[sactor].upgraded_system [UPG_TURRET], 1);
 int target = actor[sactor].turret_lock;
 int at;

 int angle;
 int lead = 5;

 int target_x;
 int target_y;
 int source_x;
 int source_y;


 if (target == LOCK_ACTOR0 || target == LOCK_ACTOR1)
 {
  if (target == LOCK_ACTOR0)
   at = 0;
    else
     at = 1;

  lead = 5;

  target_x = actor[at].x + actor[at].x_speed * lead;
  target_y = actor[at].y + actor[at].y_speed * lead;
  source_x = actor[sactor].x + actor[sactor].x_speed * lead;
  source_y = actor[sactor].y + actor[sactor].y_speed * lead;

  angle = radians_to_angle(atan2(target_y - source_y,
          target_x - source_x));

 }
  else
  {

   if (target == -1 || enemy[target].type == ENEMY_NONE)
   {
    angle = actor[sactor].angle - ANGLE_QUARTER;
   }
     else
     {

      lead = 5;

      target_x = enemy[target].x + enemy[target].x_speed * lead;
      target_y = enemy[target].y + enemy[target].y_speed * lead;
      source_x = actor[sactor].x + actor[sactor].x_speed * lead;
      source_y = actor[sactor].y + actor[sactor].y_speed * lead;

// int angle = radians_to_angle(atan2(enemy[target].y - actor[sactor].y,
//               enemy[target].x - actor[sactor].x));
      angle = radians_to_angle(atan2(target_y - source_y,
               target_x - source_x));
     }
  }
  
// angle += (actor[sactor].upgraded_system [UPG_TURRET] * 3);
// angle -= grand(actor[sactor].upgraded_system [UPG_TURRET] * 6);
 angle += 20;
 angle -= grand(40);

 timer += grand(4);

 int speed_div = 4;

 speed += actor[sactor].upgraded_system [UPG_TURRET] * 800;
 speed_div -= actor[sactor].upgraded_system [UPG_TURRET] / 2;

 int xs = actor[sactor].x_speed + cos(angle_to_radians(angle)) * speed / 3;
 int ys = actor[sactor].y_speed + sin(angle_to_radians(angle)) * speed / 3;

 int x = actor[sactor].x;
 int y = actor[sactor].y;

 if (angle == 0)
  angle = 1;

 create_bullet(BULLET_ZAP, x, y,
  xs, ys, sactor,
  damage, timer, mass, angle,
  status, seed, colours, speed_div, 0, 0, 0, 0, 0);

  play_sound2(WAV_BLAT, 600, 155, 127);

 int cloud_colours [5];
 cloud_colours [0] = colours [0];

 x = actor[sactor].x + cos(angle_to_radians(angle)) * 2000;
 y = actor[sactor].y + sin(angle_to_radians(angle)) * 2000;

// if (displaced != 0)
// {
//  x += cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3;
//  y += sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3;
// }


    create_cloud(CLOUD_MED_CIRCLE, x, y, 0, 0,
     actor[sactor].x_speed, actor[sactor].y_speed, 300, 50,
     0, 0, 0, 0, cloud_colours);


}


void fire_heavy_cannon(int sactor)
{

 int colours [4] = {COLOUR_WHITE, COLOUR_GREY6, COLOUR_GREY4, COLOUR_GREY2};
  switch(actor[sactor].upgraded_system [UPG_HEAVY])
  {
   case 1:
    colours [3] = COLOUR_RED8;
    break;
   case 2:
    colours [3] = COLOUR_YELLOW8;
    break;
   case 3:
    colours [3] = COLOUR_GREEN8;
    break;
   case 4:
    colours [3] = COLOUR_BLUE8;
    break;
   case 5:
    colours [3] = COLOUR_WHITE;
    break;
  }


 int damage = 100 + 25 * actor[sactor].upgraded_system [UPG_HEAVY];
 int timer = 13;// + actor[sactor].upgraded_system [UPG_HEAVY];
 int mass = 10;
 int status = 0;
 int speed = 25000 + actor[sactor].upgraded_system [UPG_HEAVY] * 1000;
 unsigned char seed = counter;


 int angle = actor[sactor].angle - ANGLE_QUARTER;

 timer += grand(3);

 int speed_div = 4;

// speed += actor[sactor].upgraded_system [UPG_HEAVY] * 1000;
 speed_div -= actor[sactor].upgraded_system [UPG_HEAVY] / 2;

 int xs = actor[sactor].x_speed + cos(angle_to_radians(angle)) * speed / 3;
 int ys = actor[sactor].y_speed + sin(angle_to_radians(angle)) * speed / 3;

 int x, y;

 if (angle == 0)
  angle = 1;

 int cloud_colours [5];
 cloud_colours [0] = colours [0];

// x = actor[sactor].x + cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER))) * GRAIN * 30;
// y = actor[sactor].y + sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER))) * GRAIN * 30;
 x = actor[sactor].x + cos(angle_to_radians(angle - ANGLE_QUARTER)) * GRAIN * 5;
 y = actor[sactor].y + sin(angle_to_radians(angle - ANGLE_QUARTER)) * GRAIN * 5;

 create_bullet(BULLET_POWERED, x, y,
  xs, ys, sactor,
  damage, timer, mass, angle,
  status, seed, colours, speed_div, 0, 0, 0, 0, 0);

 x = actor[sactor].x + cos(angle_to_radians(angle)) * 3000;
 y = actor[sactor].y + sin(angle_to_radians(angle)) * 3000;
 x += cos(angle_to_radians(angle - ANGLE_QUARTER)) * GRAIN * 3;
 y += sin(angle_to_radians(angle - ANGLE_QUARTER)) * GRAIN * 3;
 create_cloud(CLOUD_MED_CIRCLE, x, y, 0, 0,
  actor[sactor].x_speed, actor[sactor].y_speed, 300, 50,
  0, 0, 0, 0, cloud_colours);

 x = actor[sactor].x + cos(angle_to_radians(angle + ANGLE_QUARTER)) * GRAIN * 5; // * must be twice above because we need to counteract it
 y = actor[sactor].y + sin(angle_to_radians(angle + ANGLE_QUARTER)) * GRAIN * 5;

 create_bullet(BULLET_POWERED, x, y,
  xs, ys, sactor,
  damage, timer, mass, angle,
  status, seed, colours, speed_div, 0, 0, 0, 0, 0);

 actor_sound(sactor, ASOUND_BLAT);

 x = actor[sactor].x + cos(angle_to_radians(angle)) * 3000;
 y = actor[sactor].y + sin(angle_to_radians(angle)) * 3000;
 x += cos(angle_to_radians(angle + ANGLE_QUARTER)) * GRAIN * 3;
 y += sin(angle_to_radians(angle + ANGLE_QUARTER)) * GRAIN * 3;
 create_cloud(CLOUD_MED_CIRCLE, x, y, 0, 0,
  actor[sactor].x_speed, actor[sactor].y_speed, 300, 50,
  0, 0, 0, 0, cloud_colours);


}

void fire_backfire(int sactor)
{

 int colours [4] = {COLOUR_GREEN8, COLOUR_GREEN6, COLOUR_GREEN4, COLOUR_GREEN2};

 int damage = 100;
 int timer = 14;
 int mass = 10;
 int status = 0;
 int speed = 29000;
 unsigned char seed = counter;

 int angle = actor[sactor].angle + ANGLE_QUARTER;

 timer += grand(4);

 int speed_div = 4;

 int xs;// = actor[sactor].x_speed + cos(angle_to_radians(angle)) * speed / 3;
 int ys;// = actor[sactor].y_speed + sin(angle_to_radians(angle)) * speed / 3;

 int x = actor[sactor].x;
 int y = actor[sactor].y;

 if (angle == 0)
  angle = 1;

// x = actor[sactor].x + cos(angle_to_radians(angle - ANGLE_QUARTER)) * GRAIN * 5;
// y = actor[sactor].y + sin(angle_to_radians(angle - ANGLE_QUARTER)) * GRAIN * 5;

 int i;
 int multiple = 1 + actor[sactor].upgraded_system [UPG_BACKFIRE];

 int angle_inc = ANGLE_1_SIXTEENTH;

 angle -= angle_inc * (multiple / 2) + 1;

 if (multiple % 2 == 0)
  angle += ANGLE_1_SIXTEENTH / 2;

 int cloud_colours [5];
 cloud_colours [0] = colours [0];

 int x2, y2;

 for (i = 0; i < multiple; i ++)
 {
//  angle = grand(1000);

 xs = actor[sactor].x_speed + cos(angle_to_radians(angle)) * speed / 3;
 ys = actor[sactor].y_speed + sin(angle_to_radians(angle)) * speed / 3;
 
  create_bullet(BULLET_ZAP, x, y,
   xs, ys, sactor,
   damage, timer, mass, angle,
   status, seed, colours, speed_div, 0, 0, 0, 0, 0);
   
  x2 = actor[sactor].x + cos(angle_to_radians(angle)) * 2000;
  y2 = actor[sactor].y + sin(angle_to_radians(angle)) * 2000;

//  x2 += cos(angle_to_radians(angle)) * GRAIN * 3;
//  y2 += sin(angle_to_radians(angle)) * GRAIN * 3;

  create_cloud(CLOUD_MED_CIRCLE, x2, y2, 0, 0,
   actor[sactor].x_speed, actor[sactor].y_speed, 300, 50,
   0, 0, 0, 0, cloud_colours);

  angle += angle_inc;

 }
/*
 x = actor[sactor].x + cos(angle_to_radians(angle + ANGLE_QUARTER)) * GRAIN * 5; // * must be twice above because we need to counteract it
 y = actor[sactor].y + sin(angle_to_radians(angle + ANGLE_QUARTER)) * GRAIN * 5;

 create_bullet(BULLET_POWERED, x, y,
  xs, ys, sactor,
  damage, timer, mass, angle,
  status, seed, colours, speed_div, 0, 0, 0, 0, 0);
*/
  play_sound2(WAV_BLAT, 400, 155, 127);

}

void fire_sidekicks(int a)
{

 if (actor[a].sidekick_recycle > 0)
  return;

 int colours [4] = {COLOUR_RED8, COLOUR_RED6, COLOUR_RED4, COLOUR_RED2};
// int colours [4] = {COLOUR_YELLOW8, COLOUR_YELLOW6, COLOUR_YELLOW4, COLOUR_YELLOW2};
 int damage = 100;
 int timer = 14;
 int mass = 10;
 int status = 0;
 int speed = 29000;
 unsigned char seed = counter;
 int i;
 int angle;
 int speed_div = 2;
 int xs;// = actor[sactor].x_speed + cos(angle_to_radians(angle)) * speed / 3;
 int ys;// = actor[sactor].y_speed + sin(angle_to_radians(angle)) * speed / 3;
 int x;// = actor[a].sidekick_x [i];
 int y;// = actor[a].sidekick_y [i];
 int btype = BULLET_ZAP;
 timer += grand(4);

 switch(actor[a].upgraded_system [UPG_SIDEKICK])
 {
  case 2:
  case 3:
   damage = 125;
   btype = BULLET_POWERED;
   colours [3] = COLOUR_RED8;
   break;
  case 4:
   damage = 150;
   btype = BULLET_POWERED;
   colours [3] = COLOUR_YELLOW8;
   break;
  case 5:
   damage = 175;
   btype = BULLET_POWERED;
   colours [3] = COLOUR_GREEN8;
   break;
 }

 int cloud_colours [5];

 cloud_colours [0] = COLOUR_ORANGE8;

 for (i = 0; i < actor[a].sidekicks; i ++)
 {

 x = actor[a].sidekick_x [i];
 y = actor[a].sidekick_y [i];

 angle = actor[a].sidekick_angle [i];

 if (angle == 0)
  angle = 1;

  xs = actor[a].sidekick_x_speed [i] + cos(angle_to_radians(angle)) * speed / 3;
  ys = actor[a].sidekick_y_speed [i] + sin(angle_to_radians(angle)) * speed / 3;
 
  create_bullet(btype, x, y,
   xs, ys, a,
   damage, timer, mass, angle,
   status, seed, colours, speed_div, 0, 0, 0, 0, 0);
   
  x = x + cos(angle_to_radians(angle)) * 4000;
  y = y + sin(angle_to_radians(angle)) * 4000;

  create_cloud(CLOUD_MED_CIRCLE, x, y, 0, 0,
   actor[a].sidekick_x_speed [i], actor[a].sidekick_y_speed [i], 300, 50,
   0, 0, 0, 0, cloud_colours);

 }

  play_sound2(WAV_BLAT, 500, 155, 127);


}


void actor_upgrade(int uactor)
{

 if (actor[uactor].upgrade_slot == 0)
  return;

/* if (actor[uactor].upgrade_structure [actor[uactor].upgrade_slot] == UPG_SPECIAL)
 {
  return;
 }*/

 if (actor[uactor].upgrades [actor[uactor].upgrade_slot] >= FULL_UPGRADE)
  return;

 actor[uactor].upgrades [actor[uactor].upgrade_slot] ++;
 actor[uactor].upgraded_system [actor[uactor].upgrade_structure [actor[uactor].upgrade_slot]] ++;
 actor[uactor].just_upgraded = actor[uactor].upgrade_slot;
 actor[uactor].just_upgraded_timeout = 9;

 switch(actor[uactor].upgrade_structure [actor[uactor].upgrade_slot])
 {
  case UPG_WARHEAD:
  case UPG_TUBES:
  case UPG_SEEKER:
  case UPG_ROCKET:
  case UPG_LOADER:
   actor[uactor].secondary_weapon = SECONDARY_MISSILE;
   break;
  case UPG_BOMB:
  case UPG_LAUNCHER:
   actor[uactor].secondary_weapon = SECONDARY_BOMB;
   break;
  case UPG_SIDEKICK:
   upgrade_sidekicks(uactor);
   break;
  case UPG_ARMOUR:
   actor[uactor].max_armour = 1000 + 250 * actor[uactor].upgraded_system [UPG_ARMOUR];
   if (game.type == GAME_DUEL)
   {
    switch(game.duel_handicap [actor[uactor].controller])
    {
     case 0:
      actor[uactor].max_armour *= 75;
      actor[uactor].max_armour /= 100;
      actor[uactor].armour += 188;
      break;
     case 1:
      break;
     case 2:
      actor[uactor].max_armour *= 120;
      actor[uactor].max_armour /= 100;
      actor[uactor].armour += 300;
      break;
     case 3:
      actor[uactor].max_armour *= 150;
      actor[uactor].max_armour /= 100;
      actor[uactor].armour += 375;
      break;
    }
   }
    else
     actor[uactor].armour += 250;
   if (actor[uactor].armour > actor[uactor].max_armour)
    actor[uactor].armour = actor[uactor].max_armour;
   break;
 }

 send_message(actor[uactor].controller, upgrade_large_name [actor[uactor].upgrade_structure [actor[uactor].upgrade_slot]], STYLE_UPGRADE1 + actor[uactor].upgrades [actor[uactor].upgrade_slot] - 1);

// play_sound2(WAV_UPGRADE, 1000 + actor[uactor].upgrades [actor[uactor].upgrade_slot] * 100, 255, 127);
 play_sound2(WAV_UPGRADE, 1000 + actor[uactor].upgrade_slot * 100, 255, 127);

 actor[uactor].upgrade_slot = 0;

 
}


void upgrade_sidekicks(int a)
{
 int old_number = actor[a].sidekicks;

 switch(actor[a].upgraded_system [UPG_SIDEKICK])
 {
  case 1:
  case 2:
   actor[a].sidekicks = 2;
   break;
  default:
   actor[a].sidekicks = 4;
   break;
 }

 int i;

 if (actor[a].sidekicks > old_number)
 {
  simple_cloud_trans(TRANS_LBLUE, actor[a].x, actor[a].y, 0, 0, 500);
  for (i = old_number; i < actor[a].sidekicks; i ++)
  {
   actor[a].sidekick_x [i] = actor[a].x;
   actor[a].sidekick_y [i] = actor[a].y;
   actor[a].sidekick_x_speed [i] = actor[a].x_speed;
   actor[a].sidekick_y_speed [i] = actor[a].y_speed;
   actor[a].sidekick_angle [i] = actor[a].angle;
  }
 }

}


void actor_sound(int a, int sound)
{

 switch(sound)
 {
  case ASOUND_CANNON:
  play_sound2(WAV_CANNON, 1000 + actor[a].upgraded_system [UPG_AUTOFIRE] * 200, 155 + actor[a].upgraded_system [UPG_POWER] * 20, 127);
  break;
  case ASOUND_BOMBS:
  play_sound2(WAV_BOMBS, 1000 - actor[a].upgraded_system [UPG_BOMB] * 70, 155 + actor[a].upgraded_system [UPG_LAUNCHER] * 20, 127);
  break;
  case ASOUND_BLAT:
  play_sound2(WAV_BLAT, 400 - actor[a].upgraded_system [UPG_HEAVY] * 20, 155, 127);
  break;
  case ASOUND_TUBE:
  play_sound2(WAV_TUBE, 1000 + actor[a].upgraded_system [UPG_ROCKET] * 100 + actor[a].upgraded_system [UPG_LOADER] * 50, 155, 127);
  break;
// play_sound(sample);
/*  case 1:
  play_sound2(1, 1000 + actor[a].upgraded_system [UPG_SPEED] * 200, 255, 127);
  break;
  case 2:
  play_sound2(1, 1000 + actor[a].upgraded_system [UPG_SLIDE] * 200, 255, 127);
  break;
  case 3:
  play_sound2(1, 1000 + actor[a].upgraded_system [UPG_RETRO] * 200, 255, 127);
  break;*/
}

}




void make_drive_sound(int a, int drive)
{

 if (actor[a].drive_sound [drive] == 0)
 {
  switch(drive)
  {
   case DRIVE_THRUST:
    play_sound2(WAV_ENGINE, 1000 + actor[a].upgraded_system [UPG_SPEED] * 50, 100, 127);
    actor[a].drive_sound [DRIVE_THRUST] = 10 - actor[a].upgraded_system [UPG_SPEED] - (actor[a].upgraded_system [UPG_SPEED] == 5);
    break;
   case DRIVE_SLIDE:
    play_sound2(WAV_ENGINE, 1000 + actor[a].upgraded_system [UPG_SLIDE] * 50, 100, 127);
    actor[a].drive_sound [DRIVE_SLIDE] = 10 - actor[a].upgraded_system [UPG_SLIDE];
    break;
   case DRIVE_RETRO:
    play_sound2(WAV_ENGINE, 1000 + actor[a].upgraded_system [UPG_RETRO] * 50, 100, 127);
    actor[a].drive_sound [DRIVE_RETRO] = 10 - actor[a].upgraded_system [UPG_RETRO];
    break;
  }
 }

}


