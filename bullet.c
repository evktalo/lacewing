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
    
File: bullet.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - functions relating to bullets and dangerous projectiles

*/

#include "allegro.h"

#include <math.h>

#include "config.h"
#include "globvars.h"

#include "stuff.h"
#include "cloud.h"
#include "enemy.h"
#include "actor.h"
#include "sound.h"

#include "palette.h"

int create_bullet(int type, int x, int y,
int x_speed, int y_speed, int owner,
int damage, int timer, int mass, int angle,
int status, unsigned char seed, int colours [4], int speed_div,
int special1, int special2, int special3, int special4, int special5);


void manage_bullet(int b);
int move_bullet(int mbull, char limited);
int detect_collision(int b, int things [2]);
int detect_enemy_collision(int b, int things [2]);
void bullet_impact(int b, int k, int hit_what);

void destroy_bullet(int b);
void apply_force_to_actor(int a, int fx, int fy);
void apply_force_to_enemy(int e, int fx, int fy);

void blast(int x, int y, int radius, int damage, int force, int owner);

void drag_bullet(int b, float drag_amount);

extern int inflicteda, inflictede;

int enemy_bullet_track_target(int b, int attacking, int turning);

int closest_target(int x, int y);
int nearby_target(int range, int x, int y);

void bullet_sound(int b, int sound);
void bullet_soundf(int b, int sound, int freq);

enum
{
BSOUND_BOMB,
BSOUND_MISSILE,
BSOUND_MINE,
BSOUND_SEEKMINE,
BSOUND_ORBITAL,
BSOUND_SEEKBLOB_BANG1,
BSOUND_SEEKBLOB_BANG2,
BSOUND_TUBE,
BSOUND_MINE2,
BSOUND_HIT
};

enum
{
HIT_NOTHING,
HIT_ACTOR,
HIT_ENEMY,
HIT_EDGE
};


void init_bullets(void)
{

 int b;

 for (b = 0; b < NO_BULLETS; b++)
 {
  bullet[b].type = BULLET_NONE;
 }

}


int create_bullet(int type, int x, int y,
int x_speed, int y_speed, int owner,
int damage, int timer, int mass, int angle,
int status, unsigned char seed, int colours [4], int speed_div,
int special1, int special2, int special3, int special4, int special5)
{

   int b = 0;

   for (b = 1; b < NO_BULLETS; b++)
   {
/*    if ((bullet_type == BULLET_SPARK || bullet_type == BULLET_SPECK)
         && bcounter > MAX_BULLETS / 2)
          return -1; // these bullets just for decoration*/
    if (b == NO_BULLETS - 1) return -1;
    if (bullet[b].type == BULLET_NONE) break;
   }

   bullet[b].type = type;
//   bullet[b].x = x + (x_speed - actor[owner].x_speed);
//   bullet[b].y = y + (y_speed - actor[owner].y_speed);
   bullet[b].x = x + (x_speed - actor[owner].x_speed) / speed_div;
   bullet[b].y = y + (y_speed - actor[owner].y_speed) / speed_div;
   bullet[b].x2 = x;
   bullet[b].y2 = y;
   bullet[b].x_speed = x_speed;
   bullet[b].y_speed = y_speed;
   bullet[b].owner = owner;
   bullet[b].seed = seed;
   bullet[b].timeout = timer;
//   bullet[b].status = status;
   bullet[b].damage = damage;
   bullet[b].left_owner = 0;
   bullet[b].mass = mass;
   bullet[b].angle = angle;
   bullet[b].colours [0] = colours [0];
   bullet[b].colours [1] = colours [1];
   bullet[b].colours [2] = colours [2];
   bullet[b].colours [3] = colours [3];
   bullet[b].special1 = special1;
   bullet[b].special2 = special2;
   bullet[b].special3 = special3;
   bullet[b].special4 = special4;
   bullet[b].special5 = special5;

   switch(type)
   {
    case BULLET_CURVE:
     if (grand(2) == 0)
      bullet[b].angle = angle + ANGLE_HALF;
//       else
//        bullet[b].angle = angle - ANGLE_QUARTER;
     bullet[b].angle %= ANGLE_FULL;
     if (grand(10) == 0)
       bullet[b].angle = grand(ANGLE_FULL);
     break;
    case BULLET_TORPEDO2:
     bullet[b].x_speed = 0;
     bullet[b].y_speed = 0;
     break;

   }
   
   return b;

}



void run_bullets(void)
{

 int b;

 for (b = 1; b < NO_BULLETS; b++)
 {
  if (bullet[b].type != BULLET_NONE) manage_bullet(b);
 }

}


void manage_bullet(int b)
{

//   int x_gain = 0;
//   int y_gain = 0;
//   char angle_change;

   int cx, cy;
   int c2x, c2y;

   int passing_colour [5];

   if (bullet[b].left_owner < 100)
    bullet[b].left_owner ++;
   if (bullet[b].timeout > 0)
   {
    bullet[b].timeout --;
    if (bullet[b].timeout <= 0)
    {
     bullet_impact(b, -1, HIT_NOTHING);
     return;
    }
   }

   switch(bullet[b].type)
   {
    case BULLET_MISSILE:
     if (bullet[b].special3 == LOCK_ACTOR0 || bullet[b].special3 == LOCK_ACTOR1)
     {
     
      if (bullet[b].special3 == LOCK_ACTOR0)
       cx = 0;
        else
         cx = 1;
      if (hypot(bullet[b].y - actor[cx].y, bullet[b].x - actor[cx].x)
           < 30000)
//          < 5000)
         bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, actor[cx].x, actor[cx].y, bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
          else
           bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, actor[cx].x + (actor[cx].x_speed * bullet[b].special5), actor[cx].y + (actor[cx].y_speed * bullet[b].special5), bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
     }
      else
       if (bullet[b].special3 >= 0)
       {
        // destroyed enemies have locks cleared in register_destroyed (enemy.c)
        if (hypot(bullet[b].y - enemy[bullet[b].special3].y, bullet[b].x - enemy[bullet[b].special3].x)
            < 30000)
//          < 5000)
         bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, enemy[bullet[b].special3].x, enemy[bullet[b].special3].y, bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
          else
           bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, enemy[bullet[b].special3].x + (enemy[bullet[b].special3].x_speed * bullet[b].special5), enemy[bullet[b].special3].y + (enemy[bullet[b].special3].y_speed * bullet[b].special5), bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
// What this does: if missile is close to enemy, heads straight for it.
//  Otherwise, leads it slightly (to an extent specified by bullet[b].special5)
//  For this reason, enemy[].x_speed must be set correctly.
       }
     cx = cos(angle_to_radians(bullet[b].angle) + ANGLE_QUARTER) * GRAIN;
     cy = sin(angle_to_radians(bullet[b].angle) + ANGLE_QUARTER) * GRAIN;
     c2x = (cx * bullet[b].special1) / GRAIN;
     c2y = (cy * bullet[b].special1) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
     drag_bullet(b, 0.03);
     if (crandom(2) != 0)
     {
      passing_colour [0] = bullet[b].colours [0];
      passing_colour [1] = bullet[b].colours [1]; // 1->1
      passing_colour [2] = bullet[b].colours [3]; // 2->3

//      place_rocket_trail(bullet[b].x, bullet[b].y, bullet[b].x_speed + cx*3, bullet[b].y_speed + cy*3, 5 + grand(5), passing_colour);
//      place_rocket_trail(bullet[b].x, bullet[b].y, bullet[b].x_speed + cx*3, bullet[b].y_speed + cy*3, 1, passing_colour);
      place_rocket_trail(bullet[b].x + grand(3001) - 1500 - cx, bullet[b].y + grand(3001) - 1500 - cy, bullet[b].x_speed - cx*2, bullet[b].y_speed - cy*2, 140, passing_colour);
     }
     break;
    case BULLET_CURVE:
     cx = cos(angle_to_radians(bullet[b].angle) + ANGLE_QUARTER) * 200;
     cy = sin(angle_to_radians(bullet[b].angle) + ANGLE_QUARTER) * 200;
     bullet[b].x_speed += cx;
     bullet[b].y_speed += cy;
     break;
    case BULLET_PLASMA:
     passing_colour [0] = bullet[b].colours [grand(4)];
     cx = bullet[b].x_speed / 3;
     cy = bullet[b].y_speed / 3;
     create_cloud(CLOUD_SMALL_TRANS_CIRCLE,
      bullet[b].x + grand(4001) - 2000,
      bullet[b].y + grand(4001) - 2000,
      cx, cy,
      0,
      0,
      250 + crandom(200),70,0, 0, 0, 0, passing_colour);
     break;
    case BULLET_SEEKER_BLOB2:
     if (counter % 6 == 0)
     {
      passing_colour [0] = COLOUR_GREEN6;
      create_cloud(CLOUD_GROWING_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       0, 0,
//       bullet[b].x_speed - cx*5, bullet[b].y_speed - cy*5,
       0, 0,
       100,3,0, 0, 100, 0, passing_colour);
      }
     if (bullet[b].special1 == ATTACK_NONE)
      break;
     bullet[b].angle = enemy_bullet_track_target(b, bullet[b].special1, 4);
     cx = cos(angle_to_radians(bullet[b].angle)) * 225;
     cy = sin(angle_to_radians(bullet[b].angle)) * 225;
     bullet[b].x_speed += cx;
     bullet[b].y_speed += cy;
     drag_bullet(b, 0.07);
     break;
    case BULLET_TORPEDO:
//     bullet[b].angle += grand(11) - 5;
     cx = cos(angle_to_radians(bullet[b].angle)) * 205;
     cy = sin(angle_to_radians(bullet[b].angle)) * 205;
     bullet[b].x_speed += cx;
     bullet[b].y_speed += cy;
     drag_bullet(b, 0.01);
     if (bullet[b].timeout % 4 != 0)
      break;
      passing_colour [0] = COLOUR_YELLOW6;
      create_cloud(CLOUD_GROWING_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       0, 0,
//       bullet[b].x_speed - cx*5, bullet[b].y_speed - cy*5,
       0, 0,
       100,3,0, 0, 100, 0, passing_colour);
     break;
    case BULLET_BOSS4:
     passing_colour [0] = TRANS_YELLOW;//bullet[b].colours [grand(4)];
     if (grand(3) == 0)
      passing_colour [0] = TRANS_WHITE;
     cx = bullet[b].x_speed / 3;
     cy = bullet[b].y_speed / 3;
     create_cloud(CLOUD_SMALL_TRANS_CIRCLE,
      bullet[b].x + grand(8001) - 4000,
      bullet[b].y + grand(8001) - 4000,
      cx, cy,
      0,
      0,
      500 + crandom(300),70,0, 0, 0, 0, passing_colour);
     create_cloud(CLOUD_SMALL_TRANS_CIRCLE,
      bullet[b].x + cos(angle_to_radians(bullet[b].timeout * 40 + bullet[b].angle)) * 30000,
      bullet[b].y + sin(angle_to_radians(bullet[b].timeout * 40 + bullet[b].angle)) * 30000,
      0, 0,
      0,
      0,
      300 + crandom(200),50,0, 0, 0, 0, passing_colour);
     create_cloud(CLOUD_SMALL_TRANS_CIRCLE,
      bullet[b].x + cos(angle_to_radians(ANGLE_FULL - (bullet[b].timeout * 40 + bullet[b].angle))) * 30000,
      bullet[b].y + sin(angle_to_radians(ANGLE_FULL - (bullet[b].timeout * 40 + bullet[b].angle))) * 30000,
      0, 0,
      0,
      0,
      300 + crandom(200),50,0, 0, 0, 0, passing_colour);
     if (bullet[b].timeout % 16 == 0)
      bullet_sound(b, BSOUND_MINE2);
     break;
    case BULLET_FLAK:
     if (counter % 3 != 0)
      break;
      passing_colour [0] = COLOUR_RED8;
      create_cloud(CLOUD_SHRINKING_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       0, 0,
       500,-50,10, 0, 0, 0, passing_colour);
     break;
    case BULLET_LINES:
     passing_colour [0] = COLOUR_GREY6;
//     cx = bullet[b].x_speed / 3;
//     cy = bullet[b].y_speed / 3;
     create_cloud(CLOUD_LINE_SHADOW,
      bullet[b].x,
      bullet[b].y,
      0, 0,
      0,
      0,
      20 + grand(60),3 + grand(7),0, bullet[b].angle, 0, 0, passing_colour);
     break;
    case BULLET_PRESEEKMINE:
//     if (grand(4) == 0)
     if (grand(3) == 0 && grand(abs(bullet[b].x_speed) + abs(bullet[b].y_speed)) > 2000)
     {
      passing_colour[0] = COLOUR_WHITE;
      passing_colour[1] = COLOUR_BLUE8;
      passing_colour[2] = COLOUR_BLUE6;
      passing_colour[3] = COLOUR_BLUE4;
      passing_colour[4] = COLOUR_BLUE2;
      create_cloud(CLOUD_SPECK, bullet[b].x + grand(6001) - 3000, bullet[b].y + grand(6001) - 3000,
       0, 0, 0, 0, 24, 1,
       0, 0, 0, 0, passing_colour);
     }
     drag_bullet(b, 0.08);
     if (abs(bullet[b].x_speed) + abs(bullet[b].y_speed) < 1000)
      bullet[b].timeout = 1;
       else
        bullet[b].timeout ++;
     break;
    case BULLET_PREMINE:
//     if (grand(4) == 0 && abs(bullet[b].x_speed) + abs(bullet[b].y_speed) > 2000)
     if (grand(3) == 0 && grand(abs(bullet[b].x_speed) + abs(bullet[b].y_speed)) > 2000)
     {
      passing_colour[0] = COLOUR_YELLOW8;
      passing_colour[1] = COLOUR_RED8;
      passing_colour[2] = COLOUR_RED6;
      passing_colour[3] = COLOUR_RED4;
      passing_colour[4] = COLOUR_RED2;
      create_cloud(CLOUD_SPECK, bullet[b].x + grand(6001) - 3000, bullet[b].y + grand(6001) - 3000,
       0, 0, 0, 0, 24, 1,
       0, 0, 0, 0, passing_colour);
     }
     drag_bullet(b, 0.08);
     if (abs(bullet[b].x_speed) + abs(bullet[b].y_speed) < 1000)
      bullet[b].timeout = 1;
       else
        bullet[b].timeout ++;
     break;
    case BULLET_SEEKMINE:
    case BULLET_MINE:
     bullet[b].x_speed = 0;
     bullet[b].y_speed = 0;
     break;
    case BULLET_SEEKER_BLOB:
     if (bullet[b].special1 == ATTACK_NONE)
      break;
     bullet[b].angle = enemy_bullet_track_target(b, bullet[b].special1, 4);
//     cx = cos(angle_to_radians(bullet[b].angle) + ANGLE_QUARTER) * 1200;
//     cy = sin(angle_to_radians(bullet[b].angle) + ANGLE_QUARTER) * 1200;
     cx = cos(angle_to_radians(bullet[b].angle)) * 1900;
     cy = sin(angle_to_radians(bullet[b].angle)) * 1900;
     bullet[b].x_speed = cx;
     bullet[b].y_speed = cy;
     break;
    case BULLET_PRONG:
     bullet[b].x_speed += grand(201) - 100;
     bullet[b].y_speed += grand(201) - 100;
     bullet[b].angle += 64;
     if (bullet[b].angle > ANGLE_FULL)
      bullet[b].angle -= ANGLE_FULL;
     break;
    case BULLET_SEEKER_BLOB3:
     passing_colour [0] = TRANS_LBLUE;
     if (grand(3) == 0)
      passing_colour [0] = TRANS_DBLUE;
//     if (grand(4) == 0)
//      passing_colour [0] = TRANS_WHITE;
     create_cloud(CLOUD_SMALL_TRANS_CIRCLE,
      bullet[b].x + grand(4001) - 2000,
      bullet[b].y + grand(4001) - 2000,
      0, 0, 0, 0, 250 + crandom(200),10,5, 0, 0, 0, passing_colour);
     if (bullet[b].special1 == ATTACK_NONE)
      break;
     bullet[b].angle = enemy_bullet_track_target(b, bullet[b].special1, 8);
     cx = cos(angle_to_radians(bullet[b].angle)) * 2300;
     cy = sin(angle_to_radians(bullet[b].angle)) * 2300;
     bullet[b].x_speed = cx;
     bullet[b].y_speed = cy;
     break;
    case BULLET_STING:
     if (bullet[b].timeout % 3 != 0)
      break;
     passing_colour[0] = bullet[b].colours [1];
     passing_colour[1] = bullet[b].colours [1];
     passing_colour[2] = bullet[b].colours [2];
     passing_colour[3] = bullet[b].colours [2];
     passing_colour[4] = bullet[b].colours [3];
//     create_cloud(CLOUD_SPECK, bullet[b].x, bullet[b].y, 0, 0,
//      0, 0, 15 + grand(15), 1,
//      1, 0, 0, 0, passing_colour);
     create_cloud(CLOUD_SPECK, bullet[b].x + grand(5001) - 2500, bullet[b].y + grand(5001) - 2500, 0, 0,
      0, 0, 15 + grand(15), 1,
      1, 0, 0, 0, passing_colour);
      break;
/*    case BULLET_PRONG:
     bullet[b].x_speed += grand(201) - 100;
     bullet[b].y_speed += grand(201) - 100;
     bullet[b].angle += 64;
     if (bullet[b].angle > ANGLE_FULL)
      bullet[b].angle -= ANGLE_FULL;
     break;*/
   }
    // other switch after move_bullet

//   if (counter % 5 != 0)
//    return;

   if (move_bullet(b, 1) == 0)
    return;

   // other switch before move_bullet
   switch(bullet[b].type)
   {
    case BULLET_HOSTILE:
      passing_colour [0] = bullet[b].colours [0];
      passing_colour [1] = bullet[b].colours [1];
      passing_colour [2] = bullet[b].colours [2];
      passing_colour [3] = bullet[b].colours [3];
       cx = bullet[b].x + grand(16001) - 8000;
       cy = bullet[b].y + grand(16001) - 8000;
      create_cloud(CLOUD_FADING_LINE,
       bullet[b].x2 - bullet[b].x_speed,
       bullet[b].y2 - bullet[b].y_speed,
       cx,
       cy,
       0,
       0,
       10, 1,0, 0, 0, 0, passing_colour);
       bullet[b].x2 = cx;
       bullet[b].y2 = cy;
     break;
    case BULLET_TORPEDO2:
      if (bullet[b].timeout < 35)
      {
       if (bullet[b].timeout == 34)
        bullet_sound(b, BSOUND_TUBE);
       cx = cos(angle_to_radians(bullet[b].angle)) * (400 + (35 - bullet[b].timeout) * 20);
       cy = sin(angle_to_radians(bullet[b].angle)) * (400 + (35 - bullet[b].timeout) * 20);
       bullet[b].x_speed += cx;
       bullet[b].y_speed += cy;
//       drag_bullet(b, 0.01);
       passing_colour [0] = COLOUR_GREY1;
       passing_colour [1] = COLOUR_GREY3;
       passing_colour [2] = COLOUR_GREY5;
       passing_colour [3] = COLOUR_WHITE;
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x2 - bullet[b].x_speed,
        bullet[b].y2 - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 1,0, 0, 0, 0, passing_colour);
        bullet[b].x2 = bullet[b].x;
        bullet[b].y2 = bullet[b].y;
      }
     break;
    case BULLET_SUPERSTING:
//     if (bullet[b].timeout % 2 == 0)
     {
      passing_colour [3] = bullet[b].colours [0];
      passing_colour [2] = bullet[b].colours [1];
      passing_colour [1] = bullet[b].colours [2];
      passing_colour [0] = bullet[b].colours [3];
       c2x = cos(angle_to_radians(bullet[b].angle + ANGLE_QUARTER)) * 3000;
       c2y = sin(angle_to_radians(bullet[b].angle + ANGLE_QUARTER)) * 3000;
      create_cloud(CLOUD_FADING_LINE,
       bullet[b].x2 - (bullet[b].x_speed * 1) + c2x,
       bullet[b].y2 - (bullet[b].y_speed * 1) + c2y,
       bullet[b].x - c2x,
       bullet[b].y - c2y,
       0,
       0,
       10, 1,0, 0, 0, 0, passing_colour);
      create_cloud(CLOUD_FADING_LINE,
       bullet[b].x - (bullet[b].x_speed * 1) - c2x,
       bullet[b].y - (bullet[b].y_speed * 1) - c2y,
       bullet[b].x + c2x,
       bullet[b].y + c2y,
       0,
       0,
       10, 1,0, 0, 0, 0, passing_colour);
     }


       
//       bullet[b].x2 = bullet[b].x + c2x;
//       bullet[b].y2 = bullet[b].y + c2y;
/*       c2x = cos(angle_to_radians(bullet[b].angle + ANGLE_QUARTER)) * 5000;
       c2y = sin(angle_to_radians(bullet[b].angle + ANGLE_QUARTER)) * 5000;
//       cx = bullet[b].x + c2x;
//       cy = bullet[b].y + c2y;
      create_cloud(CLOUD_FADING_LINE,
//       bullet[b].x2 - bullet[b].x_speed + c2x,
//       bullet[b].y2 - bullet[b].y_speed + c2y,
       bullet[b].x2 + c2x,
       bullet[b].y2 + c2y,
       bullet[b].x - c2x,
       bullet[b].y - c2y,
       0,
       0,
       10, 1,0, 0, 0, 0, passing_colour);
       bullet[b].x2 = bullet[b].x - c2x;
       bullet[b].y2 = bullet[b].y - c2y;*/
     break;

   }



   int things [2];
   int k;

   switch(bullet[b].type)
   {
    case BULLET_BOMB:
    case BULLET_BOMB_SIDE:
     return;
   }

   // Only collision detection can be from this point on, as bombs
   //  return above (they don't do collision detection)

   if (bullet[b].owner == OWNER_ENEMY || game.type == GAME_DUEL)
   {
    k = detect_collision(b, things);

    if (k != -1)
    {
     bullet[b].x = things [0];
     bullet[b].y = things [1];
// bullet_hits_actor
     hurt_actor(k, bullet[b].owner, bullet[b].damage);
     bullet_impact(b, k, HIT_ACTOR);
     return;
    }
   }
//    else

   if (bullet[b].owner != OWNER_ENEMY) // || game.type == GAME_DUEL)
    {
     k = detect_enemy_collision(b, things);

     if (k != -1)
     {
      bullet[b].x = things [0];
      bullet[b].y = things [1];
      hurt_enemy(k, bullet[b].damage, bullet[b].owner, 1);
      bullet_soundf(b, BSOUND_HIT, 800 + grand(200) - eclass[enemy[k].type].mass);
      bullet_impact(b, k, HIT_ENEMY);
// bullet_hits_enemy
      return;
     }
    }

}


void bullet_impact(int b, int k, int hit_what)
{
 int j;
 int xs = bullet[b].x_speed;
 int ys = bullet[b].y_speed;

 int passing_colours [5];
 
 if (hit_what == HIT_ACTOR && k != -1)
 {
  xs = actor[k].x_speed;
  ys = actor[k].y_speed;
 }
  else
  {
   if (hit_what == HIT_ENEMY && k != -1)
   {
    xs = enemy[k].x_speed;
    ys = enemy[k].y_speed;
   }
    else
    {
     xs = 0;
     ys = 0;
    }
  }

 switch(bullet[b].type)
 {
  case BULLET_MISSILE:
   passing_colours [0] = bullet[b].colours [1];
   passing_colours [1] = bullet[b].colours [2];
   passing_colours [2] = bullet[b].colours [3];
//   passing_colours [3] = bullet[b].colours [3];
//   place_rocket_trail(bullet[b].x, bullet[b].y, bullet[b].x_speed, bullet[b].y_speed, 700 + grand(450), passing_colours);
   place_explosion_with_smoke(bullet[b].x, bullet[b].y, 0, 0, 700 + grand(450), passing_colours);
   blast(bullet[b].x, bullet[b].y, 30000, bullet[b].damage * 10, 3000, bullet[b].owner);
   for (j = 0; j < bullet[b].damage / 14 + 1; j ++)
   {
    xs = bullet[b].x + grand(40001) - 20000;
    ys = bullet[b].y + grand(40001) - 20000;
    place_explosion_with_smoke(xs, ys, 0, 0, 100 + grand(150), passing_colours);
    blast(xs, ys, 15000, bullet[b].damage * 2, 2000, bullet[b].owner);
   }
//   passing_colours [0] = TRANS_YELLOW;
   passing_colours [0] = TRANS_YELLOW;
   passing_colours [1] = COLOUR_YELLOW8;
   place_line_burst(bullet[b].x, bullet[b].y, 0, 0, 5 + (bullet[b].damage / 3), 15 + (bullet[b].damage / 8), 15, 1200 + (bullet[b].damage * 3), 300, passing_colours);
   bullet_sound(b, BSOUND_MISSILE);
   break;
  case BULLET_TORPEDO2:
   passing_colours [0] = TRANS_DBLUE;
   passing_colours [1] = TRANS_LBLUE;
   passing_colours [2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 700 + grand(450), passing_colours);
   blast(bullet[b].x, bullet[b].y, 30000, bullet[b].damage * 5, 3000, bullet[b].owner);
   passing_colours [0] = TRANS_LBLUE;
   passing_colours [1] = COLOUR_BLUE8;
   place_line_burst(bullet[b].x, bullet[b].y, 0, 0, 5, 15, 15, 1200, 300, passing_colours);
   bullet_sound(b, BSOUND_MISSILE);
   break;
  case BULLET_LINES:
   passing_colours [0] = TRANS_WHITE;
   passing_colours [1] = COLOUR_GREY6;
   place_line_burst(bullet[b].x, bullet[b].y, 0, 0, 5 + grand(3), 15, 15, 1200, 300, passing_colours);
   break;
  case BULLET_PRONG:
//   passing_colours [0] = TRANS_LGREEN;
//   passing_colours [1] = COLOUR_GREEN8;
//   place_line_burst(bullet[b].x, bullet[b].y, 0, 0, 5 + grand(3), 15, 15, 1200, 300, passing_colours);
   passing_colours[0] = bullet[b].colours [0];
   passing_colours[1] = bullet[b].colours [1];
   passing_colours[2] = bullet[b].colours [1];
   passing_colours[3] = bullet[b].colours [2];
   passing_colours[4] = bullet[b].colours [2];
   
   place_speck_burst(bullet[b].x, bullet[b].y, xs,
    ys, 4, 10, 21, 450, passing_colours);
   passing_colours [0] = colour_to_trans(bullet[b].colours [0]);
   
      create_cloud(CLOUD_SMALL_TRANS_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       xs, ys,
       0,
       0,
       250 + crandom(200),10,10, 0, 0, 0, passing_colours);
   break;
  case BULLET_BOMB:
   bullet_sound(b, BSOUND_BOMB);
// assumes side bombs always explode at same time
  case BULLET_BOMB_SIDE:
   passing_colours [0] = bullet[b].colours [1];
   passing_colours [1] = bullet[b].colours [2];
   passing_colours [2] = bullet[b].colours [3];
   place_explosion_with_smoke(bullet[b].x, bullet[b].y, 0, 0, 200 + grand(300) + bullet[b].damage * 10, passing_colours);
   blast(bullet[b].x, bullet[b].y, bullet[b].special1, bullet[b].damage * 10, 3000, bullet[b].owner);
   for (j = 0; j < bullet[b].damage / 12 + 1; j ++)
   {
    xs = bullet[b].x + grand(40001) - 20000;
    ys = bullet[b].y + grand(40001) - 20000;
    place_explosion_with_smoke(xs, ys, 0, 0, 100 + grand(150), passing_colours);
//    blast(xs, ys, 15000, bullet[b].damage * 5, 2000, bullet[b].owner);
   }
   passing_colours [3] = TRANS_WHITE2;
   passing_colours [2] = TRANS_LGREY;
   passing_colours [1] = TRANS_GREY;
   passing_colours [0] = TRANS_DGREY;
      create_cloud(CLOUD_SHOCKWAVE,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       0,
       0,
       200 + crandom(50) + bullet[b].special1 / 200,15,0, 0, 250 + grand(100) + bullet[b].special1 / 200, 0, passing_colours);
   break;
  case BULLET_FLAK:
   passing_colours [0] = TRANS_LGREY; //bullet[b].colours [1];
//   passing_colours [1] = TRANS_DRED; //bullet[b].colours [2];
   passing_colours [1] = TRANS_DRED; //bullet[b].colours [3];
   passing_colours [2] = TRANS_WHITE; //bullet[b].colours [3];
   place_explosion_with_smoke(bullet[b].x, bullet[b].y, 0, 0, 200 + grand(200), passing_colours);
   blast(bullet[b].x, bullet[b].y, 30000, 200, 3000, bullet[b].owner);
   passing_colours [3] = TRANS_LRED;
   passing_colours [2] = TRANS_LRED;
   passing_colours [1] = TRANS_DRED;
   passing_colours [0] = TRANS_DRED;
   create_cloud(CLOUD_SHOCKWAVE, bullet[b].x, bullet[b].y, 0, 0, 0, 0,
    200 + crandom(50) + bullet[b].special1 / 200,15,0, 0, 250 + grand(100) + bullet[b].special1 / 200, 0, passing_colours);
   bullet_sound(b, BSOUND_MINE);
/*   passing_colours [0] = TRANS_LGREY; //bullet[b].colours [1];
//   passing_colours [1] = TRANS_DRED; //bullet[b].colours [2];
   passing_colours [1] = TRANS_YELLOW; //bullet[b].colours [3];
   passing_colours [2] = TRANS_YELLOW; //bullet[b].colours [3];
   place_explosion_with_smoke(bullet[b].x, bullet[b].y, 0, 0, 200 + grand(200), passing_colours);
   blast(bullet[b].x, bullet[b].y, 30000, 200, 3000, bullet[b].owner);
   passing_colours [3] = TRANS_YELLOW;
   passing_colours [2] = TRANS_LORANGE;
   passing_colours [1] = TRANS_LRED;
   passing_colours [0] = TRANS_DRED;
   create_cloud(CLOUD_SHOCKWAVE, bullet[b].x, bullet[b].y, 0, 0, 0, 0,
    200 + crandom(50) + bullet[b].special1 / 200,15,0, 0, 250 + grand(100) + bullet[b].special1 / 200, 0, passing_colours);*/
   break;
  case BULLET_STING:
  case BULLET_STING2:
  case BULLET_ZAP:
//  for (j = 0; j < 4; j ++)
//  {
   passing_colours[0] = bullet[b].colours [0];
   passing_colours[1] = bullet[b].colours [1];
   passing_colours[2] = bullet[b].colours [1];
   passing_colours[3] = bullet[b].colours [2];
   passing_colours[4] = bullet[b].colours [2];
   
   place_speck_burst(bullet[b].x, bullet[b].y, xs,
    ys, 4, 10, 21, 450, passing_colours);
//   place_speck_burst(bullet[b].x, bullet[b].y, 0,
//    0, 4, 10, 21, 450, passing_colours);
//  }
/*   create_cloud(CLOUD_TRAIL1,
    bullet[b].x,
    bullet[b].y,
    0, 0,
    xs,
    ys,
    10 + grand(21), 0, 0, 0);*/
   break;
  case BULLET_POWERED:
   passing_colours[0] = bullet[b].colours [0];
   passing_colours[1] = bullet[b].colours [1];
   passing_colours[2] = bullet[b].colours [1];
   passing_colours[3] = bullet[b].colours [2];
   passing_colours[4] = bullet[b].colours [2];
   
   place_speck_burst(bullet[b].x, bullet[b].y, xs,
    ys, 4, 10, 21, 450, passing_colours);

   passing_colours [0] = colour_to_trans(bullet[b].colours [3]);
   
      create_cloud(CLOUD_SMALL_TRANS_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       xs, ys,
       0,
       0,
       250 + crandom(200),10,10, 0, 0, 0, passing_colours);

   break;
   case BULLET_ORBITAL:
    passing_colours[0] = bullet[b].colours [0];
    create_cloud(CLOUD_MED_CIRCLE, bullet[b].x, bullet[b].y, 0, 0,
//     enemy[e].x_speed, enemy[e].y_speed, 300, 100,
     0, 0, 300, 20,
     2, 0, 0, 0, passing_colours);
//  simple_cloud(COLOUR_BLUE6, ox, oy, enemy[e].x_speed, enemy[e].y_speed, 200);
    if (hit_what != HIT_NOTHING)
     bullet_sound(b, BSOUND_ORBITAL);
     break;
   case BULLET_NICE_ORBITAL:
    passing_colours[0] = bullet[b].colours [1];
    passing_colours[1] = bullet[b].colours [1];
    passing_colours[2] = bullet[b].colours [2];
    passing_colours[3] = bullet[b].colours [2];
    passing_colours[4] = bullet[b].colours [3];
    create_cloud(CLOUD_SPECK, bullet[b].x, bullet[b].y, 0, 0,
     0, 0, 24, 1,
     2, 0, 0, 0, passing_colours);
//    bullet_sound(b, BSOUND_ORBITAL);
    if (hit_what != HIT_NOTHING)
     bullet_sound(b, BSOUND_ORBITAL);
     break;
  case BULLET_PLASMA:
   passing_colours[0] = bullet[b].colours [0];
   passing_colours[1] = bullet[b].colours [1];
   passing_colours[2] = bullet[b].colours [2];
     place_explosion(bullet[b].x, bullet[b].y, 0, 0, 700 + grand(450), passing_colours);
     blast(bullet[b].x, bullet[b].y, 5000, bullet[b].damage, 500, bullet[b].owner);
     bullet_sound(b, BSOUND_MINE);
   break;
  case BULLET_SEEKER_BLOB:
   bullet_sound(b, BSOUND_SEEKBLOB_BANG1);
   passing_colours[0] = TRANS_DBLUE;
   passing_colours[1] = TRANS_LBLUE;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(150), passing_colours);
   break;
  case BULLET_SEEKER_BLOB3:
   bullet_sound(b, BSOUND_SEEKBLOB_BANG2);
  case BULLET_BLUE_BLOB:
   passing_colours[0] = TRANS_DBLUE;
   passing_colours[1] = TRANS_LBLUE;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(150), passing_colours);
   break;
  case BULLET_SEEKER_BLOB2:
   passing_colours[0] = TRANS_DGREEN;
   passing_colours[1] = TRANS_LGREEN;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(150), passing_colours);
   break;
  case BULLET_TORPEDO:
   passing_colours [0] = TRANS_LGREY; //bullet[b].colours [1];
   passing_colours [1] = TRANS_YELLOW; //bullet[b].colours [3];
   passing_colours [2] = TRANS_YELLOW; //bullet[b].colours [3];
   place_explosion_with_smoke(bullet[b].x, bullet[b].y, 0, 0, 200 + grand(200), passing_colours);
   blast(bullet[b].x, bullet[b].y, 30000, 200, 3000, bullet[b].owner);
   passing_colours [3] = TRANS_WHITE2;
   passing_colours [2] = TRANS_YELLOW;
   passing_colours [1] = TRANS_LGREY;
   passing_colours [0] = TRANS_DGREY;
   create_cloud(CLOUD_SHOCKWAVE, bullet[b].x, bullet[b].y, 0, 0, 0, 0,
    200 + crandom(50) + bullet[b].special1 / 200,15,0, 0, 250 + grand(100) + bullet[b].special1 / 200, 0, passing_colours);
   bullet_sound(b, BSOUND_MINE);
   break;
  case BULLET_BOSS4:
   passing_colours [0] = TRANS_LGREY;
   passing_colours [1] = TRANS_YELLOW;
   passing_colours [2] = TRANS_WHITE;
   place_explosion_with_smoke(bullet[b].x, bullet[b].y, 0, 0, 2500 + grand(200), passing_colours);
   blast(bullet[b].x, bullet[b].y, 50000, 500, 5000, bullet[b].owner);
   passing_colours [3] = TRANS_WHITE2;
   passing_colours [2] = TRANS_YELLOW;
   passing_colours [1] = TRANS_LORANGE;
   passing_colours [0] = TRANS_LRED;
   create_cloud(CLOUD_SHOCKWAVE, bullet[b].x, bullet[b].y, 0, 0, 0, 0,
    800 + crandom(50),25,0, 0, 850 + grand(100), 0, passing_colours);
//   place_line_burst(bullet[b].x, bullet[b].y, 0, 0, 5), 15 + (bullet[b].damage / 8), 15, 1200 + (bullet[b].damage * 3), 300, passing_colours);
   bullet_sound(b, BSOUND_BOMB);
   break;
  case BULLET_RED_BLOB:
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(150), passing_colours);
   break;
  case BULLET_CURVE:
  case BULLET_PURPLE_BLOB:
  case BULLET_SUPERSTING:
   passing_colours[0] = TRANS_DBLUE;
   passing_colours[1] = TRANS_PURPLE;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(150), passing_colours);
   break;
  case BULLET_ORANGE_BLOB:
   passing_colours[0] = TRANS_DORANGE;
   passing_colours[1] = TRANS_LORANGE;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(150), passing_colours);
   break;
  case BULLET_YELLOW_BLOB:
   passing_colours[0] = TRANS_LORANGE;
   passing_colours[1] = TRANS_YELLOW;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(150), passing_colours);
   break;
  case BULLET_PREMINE:
   bullet[b].type = BULLET_MINE;
   bullet[b].timeout = 333 + grand(100);
   bullet[b].x_speed = 0;
   bullet[b].y_speed = 0;
   return;
// NOTE: not break!
  case BULLET_PRESEEKMINE:
   bullet[b].type = BULLET_SEEKMINE;
   bullet[b].timeout = 333 + grand(100);
   bullet[b].x_speed = 0;
   bullet[b].y_speed = 0;
   return;
// NOTE: not break!
  case BULLET_SEEKMINE:
   bullet[b].type = BULLET_SEEKER_BLOB;
   bullet[b].timeout = 333 + grand(100);
   bullet[b].special1 = closest_target(bullet[b].x, bullet[b].y);
   passing_colours [1] = TRANS_LBLUE;
   passing_colours [2] = TRANS_WHITE;
   place_burstlet_burst(bullet[b].x, bullet[b].y, 0, 0, 3 + grand(3),
    4, 2, 1000, 1500, 4, passing_colours);
   return;
// NOTE: not break!
  case BULLET_MINE:
   passing_colours [0] = TRANS_DRED;
   passing_colours [1] = TRANS_LRED;
   passing_colours [2] = TRANS_YELLOW;
   place_explosion_with_smoke(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(300), passing_colours);
   blast(bullet[b].x, bullet[b].y, 30000, 200, 9000, bullet[b].owner);
   passing_colours [3] = TRANS_WHITE2;
   passing_colours [2] = TRANS_LGREY;
   passing_colours [1] = TRANS_GREY;
   passing_colours [0] = TRANS_DGREY;
      create_cloud(CLOUD_SHOCKWAVE,
       bullet[b].x, bullet[b].y,
       0, 0, 0, 0,
       200 + crandom(50),15,0, 0, 250 + grand(100), 0, passing_colours);
   bullet_sound(b, BSOUND_MINE);
   break;
 }

// Note: not all bullets get this far (eg mines)

 destroy_bullet(b);

}


void destroy_bullet(int b)
{
 bullet[b].type = BULLET_NONE;
}


int move_bullet(int mbull, char limited)
{


if (bullet[mbull].x_speed == 0 && bullet[mbull].y_speed == 0) return 1;
/*
 if (bullet[mbull].x + bullet[mbull].x_speed <= 0
      || bullet[mbull].x + bullet[mbull].x_speed >= arena.max_x - 3000)
 {
  bullet_impact(mbull, -1);
  return 0;
 }
 if (bullet[mbull].y + bullet[mbull].y_speed <= 0
     || bullet[mbull].y + bullet[mbull].y_speed >= arena.max_y - 4000)
 {
  bullet_impact(mbull, -1);
  return 0;
 }
*/

 if (bullet[mbull].x <= 0
      || bullet[mbull].x >= arena.max_x - 3000)
 {
  bullet_impact(mbull, -1, HIT_EDGE);
  return 0;
 }
 if (bullet[mbull].y <= 0
     || bullet[mbull].y >= arena.max_y - 4000)
 {
  bullet_impact(mbull, -1, HIT_EDGE);
  return 0;
 }

bullet[mbull].x2 += bullet[mbull].x_speed;
bullet[mbull].y2 += bullet[mbull].y_speed;

bullet[mbull].x += bullet[mbull].x_speed;
bullet[mbull].y += bullet[mbull].y_speed;

return 1;

char dir_x = 0, dir_y = 0;

int move_x = bullet[mbull].x_speed;
int move_y = bullet[mbull].y_speed;

int timeout = 0;

if (move_x > 0) dir_x = 10;
if (move_x < 0) dir_x = -10;
if (move_y > 0) dir_y = 10;
if (move_y < 0) dir_y = -10;


int step_x = move_x;
int step_y = move_y;

int oldx = bullet[mbull].x;
int oldy = bullet[mbull].y;


if (step_x == 0)
{
 if (step_y > GRAIN)
  step_y = GRAIN;
 if (step_y < -GRAIN)
  step_y = -GRAIN;
}
 else
if (step_y == 0)
{
 if (step_x > GRAIN)
  step_x = GRAIN;
 if (step_x < -GRAIN)
  step_x = -GRAIN;
}
 else
 if (abs(step_x) > GRAIN || abs(step_y) > GRAIN)
 {
  if (abs(step_x) >= abs(step_y))
  {
   step_y = (step_y * GRAIN) / abs(step_x);
   step_x = (GRAIN * step_x) / abs(step_x);
  } else
  if (abs(step_y) > abs(step_x))
  {
   step_x = (step_x * GRAIN) / abs(step_y);
   step_y = (GRAIN * step_y) / abs(step_y);
  }
 }

// if (move_x == 10 && move_y == 0 && bullet[mbull].damage != -1)
// {
//  if (actor_collision(mbull) == 1) return 0;
// }


while (move_x != 0 || move_y != 0)
{
 timeout ++;
 if (timeout > 1000)
  break;
//  if (actor_collision(mbull) == 1) return 0;



 if (step_x == 0 && step_y == 0) return 1;

 if (step_x == 0) move_x = 0;
 if (step_y == 0) move_y = 0;



 if (move_y != 0 && (abs(move_y + step_y) == move_y + step_y) != (abs(step_y) == step_y))
 {
//  move_y = 0;
  step_y = move_y;
  if (move_x == 0) break;
 }
 
 if (move_x != 0 && (abs(move_x + step_x) == move_x + step_x) != (abs(step_x) == step_x))
 {
//  move_x = 0;
  step_x = move_x;
  if (move_y == 0) break;
 }

 if ((move_x > 0) != (step_x > 0))// && (move_x < 0) != (step_x < 0))
 {
//  move_x = 0;
  step_x = move_x;
  if (move_y == 0) break;
 }

 if ((move_y > 0) != (step_y > 0))// && (move_y < 0) != (step_y < 0))
 {
//  move_y = 0;
  step_y = move_y;
  if (move_x == 0) break;
 }


 if (limited == 1)
 {
  move_x -= step_x;
  move_y -= step_y;
 }

 bullet[mbull].x += step_x;
 bullet[mbull].y += step_y;


} // end of while loop

bullet[mbull].x2 += bullet[mbull].x_speed;
bullet[mbull].y2 += bullet[mbull].y_speed;


if (oldx == bullet[mbull].x && oldy == bullet[mbull].y)
{
 bullet[mbull].x_speed = 0;
 bullet[mbull].y_speed = 0;
}



return 1;

}



int detect_collision(int b, int things [2])
{

 int x2 = bullet[b].x - bullet[b].x_speed;
 int y2 = bullet[b].y - bullet[b].y_speed;
 int i;
/* int xa, ya;
 float bullet_angle;
 float bullet_slope;
 int side_disp;
 int x1 =*/

 for (i = 0; i < NO_ACTORS; i ++)
 {
// I tried to write an accurate collision detector but failed; all my
//  ideas would've been too slow. So we get this horrible thing which
//  is woefully inaccurate at high relative speeds:
 if (actor[i].in_play == 0)
  continue;
 if (actor[i].grace_period > 0)
  continue;
 if (bullet[b].owner == i)
  continue; // for duels; actors' bullets in non-duel games don't get this far.
// if (bullet[b].owner == i)// && bullet[b].left_owner < 10)
// if (bullet[b].owner != OWNER_ENEMY)// && bullet[b].left_owner < 10)
//  continue;
 if (hypot(actor[i].x - bullet[b].x, actor[i].y - bullet[b].y) <= actor[i].radius)
 {
  things [0] = bullet[b].x;
  things [1] = bullet[b].y;
  return i;
 }
 if (hypot(actor[i].x - x2, actor[i].y - y2) <= actor[i].radius)
 {
  things [0] = x2;
  things [1] = y2;
  return i;
 }
 
 
/*  if (actor[i].type == ACTORTYPE_NONE)
   continue;
  if (bullet[b].owner == i && bullet[b].left_owner < 10)
   continue;
  if (actor[i].x < bullet[b].x - 100 * GRAIN
      || actor[i].x > bullet[b].x + 100 * GRAIN
      || actor[i].y < bullet[b].y - 100 * GRAIN
      || actor[i].y > bullet[b].y + 100 * GRAIN)
       continue;
  if (hypot(actor[i].x - bullet[b].x, actor[i].y - bullet[b].y)
      <= actor[i].radius)
       return i;
  if (hypot(actor[i].x - x2, actor[i].y - y2)
      <= actor[i].radius)
       return i;
  if (actor[i].x
       
  if (bullet[b].x_speed == 0)
   bullet_slope = 0;
    else
     bullet_slope = bullet[b].y_speed / bullet[b].x_speed;
  bullet_angle = atan2(bullet[b].y_speed, bullet[b].x_speed);

//  if (

//   else
//   {
//     side_disp = abs(actor[i].y - (bullet[b].y - bullet[b].y_speed)
//        / actor[i].x - (bullet[b].x - bullet[b].x_speed)
//   }

//  bullet_angle += PI / 2;
  x2 = cos(bullet_angle) * actor[i].radius * GRAIN;
  y2 = sin(bullet_angle) * actor[i].radius * GRAIN;
  xa = actor[i].x + x2;
  ya = actor[i].y + y2;
*/
 }

 return -1;

}



int detect_enemy_collision(int b, int things [2])
{

 int x2 = bullet[b].x - bullet[b].x_speed;
 int y2 = bullet[b].y - bullet[b].y_speed;
 int e;

 for (e = 0; e < NO_ENEMIES; e ++)
 {
  if (enemy[e].type == ENEMY_NONE)
   continue;
  if (enemy[e].radius == 0) // dead wanderer etc
   continue;
//  if (bullet[b].owner == OWNER_ENEMY)
//   continue;
  if (hypot(enemy[e].x - bullet[b].x, enemy[e].y - bullet[b].y) <= enemy[e].radius)
  {
   things [0] = bullet[b].x;
   things [1] = bullet[b].y;
   return e;
  }
  if (hypot(enemy[e].x - x2, enemy[e].y - y2) <= enemy[e].radius)
  {
   things [0] = x2;
   things [1] = y2;
   return e;
  }
  x2 = bullet[b].x - bullet[b].x_speed / 2;
  y2 = bullet[b].y - bullet[b].y_speed / 2;
  if (hypot(enemy[e].x - x2, enemy[e].y - y2) <= enemy[e].radius)
  {
   things [0] = x2;
   things [1] = y2;
   return e;
  }
 
 }

 return -1;

}





void blast(int x, int y, int radius, int damage, int force, int owner)
{

 int i, damage_done, distance;
 float bangle, proportion;

 if (owner == OWNER_ENEMY || game.type == GAME_DUEL)
 {
  for (i = 0; i < NO_ACTORS; i ++)
  {
   if (actor[i].in_play == 0)
    continue;
   if (actor[i].grace_period > 0)
    continue;
   if (i == owner)
    continue;

   distance = hypot(actor[i].x - x, actor[i].y - y);

   if (distance <= actor[i].radius + radius)
   {
    bangle = atan2(actor[i].y - y, actor[i].x - x);
    proportion = ((radius + actor[i].radius) * 50) / (distance + actor[i].radius);
    if (proportion > 100)
     proportion = 100;
    actor[i].x_speed += cos(bangle) * (float) force * proportion / actor[i].mass;
    actor[i].y_speed += sin(bangle) * (float) force * proportion / actor[i].mass;
    damage_done = (float) damage * proportion;
    damage_done /= 100;
//    inflicteda = damage_done;
//    inflictede = proportion;
    hurt_actor(i, owner, damage_done);
   }

   
  }
  if (owner == OWNER_ENEMY)
   return;
 }

 int e;

 for (e = 0; e < NO_ENEMIES; e ++)
 {
  if (enemy[e].type == ENEMY_NONE)
   continue;

   distance = hypot(enemy[e].x - x, enemy[e].y - y);

   if (distance <= enemy[e].radius + radius)
   {
    bangle = atan2(enemy[e].y - y, enemy[e].x - x);
//    proportion = (float) (((distance + enemy[e].radius) * 100) / (float) radius + enemy[e].radius);
    proportion = ((radius + enemy[e].radius) * 50) / (distance + enemy[e].radius);
//    (((distance + enemy[e].radius) * 100) / (float) radius + enemy[e].radius);
    if (proportion > 100)
     proportion = 100;
    enemy[e].x_speed += cos(bangle) * (float) force * proportion / enemy[e].mass;
    enemy[e].y_speed += sin(bangle) * (float) force * proportion / enemy[e].mass;
    damage_done = (float) damage * proportion;
    damage_done /= 100;
//    inflicteda = damage_done;
//    inflictede = proportion;
    hurt_enemy(e, damage_done, owner, 1);
   }
//  if (distance <= enemy[e].radius + radius)
//  {
//  }
 }

}


void drag_bullet(int b, float drag_amount)
{
 bullet[b].x_speed *= (float) 1 - drag_amount;
 bullet[b].y_speed *= (float) 1 - drag_amount;
}


int enemy_bullet_track_target(int b, int attacking, int turning)
{

 bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, actor[attacking].x, actor[attacking].y, bullet[b].angle, turning);
 return bullet[b].angle;
}


// returns the nearest actor - assumes max 2 actors
// returns either 1 or 0, or -1 if no actors in place
int closest_target(int x, int y)
{

 if (actor[0].in_play == 0 && actor[1].in_play == 0)
  return ATTACK_NONE;

 if (actor[0].in_play == 0)
  return 1;
 if (actor[1].in_play == 0)
  return 0;

 if (hypot(x - actor[0].x, y - actor[0].y)
      < hypot(x - actor[1].x, y - actor[1].y))
       return 0;
        else
         return 1;

}

// returns a random actor within range. Assumes max 2 actors
// returns 1, 0, or ATTACK_NONE if none within range
// range calculation is quick & dirty
int nearby_target(int range, int x, int y)
{

 if (actor[0].in_play == 0 && actor[1].in_play == 0)
  return ATTACK_NONE;

 if (actor[0].in_play == 1)
 {
  if (abs(actor[0].x - x) + abs(actor[0].y - y) < range)
  {
   if (actor[1].in_play == 1
        && abs(actor[1].x - x) + abs(actor[1].y - y) < range)
   {
    return grand(2);
   }
    else
     return 0;
  }
 }

 if (actor[1].in_play == 1
     && abs(actor[1].x - x) + abs(actor[1].y - y) < range)
  return 1;

 return ATTACK_NONE;

}



/*

void apply_force_to_actor(int a, float rangle, int total_force)
{

   float rangle = atan2(fx, fy);

   int total_force = hypot(fx, fy);

   actor[a].x_speed += cos(angle) * total_force;
   actor[a].y_speed += sin(angle) * total_force;

}


void apply_force_to_enemy(int e, int fx, int fy)
{

   float rangle = atan2(fx, fy);

   int total_force = hypot(fx, fy);

   enemy[e].x_speed += cos(angle) * total_force;
   enemy[e].y_speed += sin(angle) * total_force;


}


*/


void bullet_sound(int b, int sound)
{

 switch(sound)
 {
  case BSOUND_MISSILE:
  play_sound_pos(WAV_MISSILE, 800 + grand(400), 255, bullet[b].x, bullet[b].y);
  break;
  case BSOUND_BOMB:
//  play_sound_pos(WAV_BLAST, 200 + grand(2000), 255, bullet[b].x, bullet[b].y);
  play_sound_pos(WAV_BLAST, 300 + grand(200), 255, bullet[b].x, bullet[b].y);
  break;
  case BSOUND_MINE:
  play_sound_pos(WAV_MINEBANG, 700 + grand(200), 200, bullet[b].x, bullet[b].y);
  break;
  case BSOUND_SEEKMINE:
  play_sound_pos(WAV_SEEKMINE, 700 + grand(200), 200, bullet[b].x, bullet[b].y);
  break;
  case BSOUND_ORBITAL:
  play_sound_pos(WAV_ORBITAL, 1200, 200, bullet[b].x, bullet[b].y);
  break;
  case BSOUND_SEEKBLOB_BANG1:
  play_sound_pos(WAV_SEEKBLOB_BANG, 1200 + grand(200), 200, bullet[b].x, bullet[b].y);
  break;
  case BSOUND_SEEKBLOB_BANG2:
  play_sound_pos(WAV_SEEKBLOB_BANG, 800 + grand(200), 200, bullet[b].x, bullet[b].y);
  break;
  case BSOUND_TUBE:
  play_sound_pos(WAV_TUBE, 2700, 200, bullet[b].x, bullet[b].y);
  break;
  case BSOUND_MINE2:
  play_sound_pos(WAV_MINE, 600, 200, bullet[b].x, bullet[b].y);
  break;
 }

}


void bullet_soundf(int b, int sound, int freq)
{

 switch(sound)
 {
  case BSOUND_MISSILE:
  play_sound_pos(WAV_MISSILE, freq, 255, bullet[b].x, bullet[b].y);
  break;
  case BSOUND_HIT:
  play_sound_pos(WAV_HIT, freq, 200, bullet[b].x, bullet[b].y);
  break;
 }

}



