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
    
File: display.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - functions which put stuff onto the screen

*/

#include "allegro.h"

#include <math.h>
#include <string.h>

#include <stdlib.h>

#include "config.h"
#include "globvars.h"
#include "palette.h"

#include "stuff.h"
#include "display.h"

int pline(BITMAP *bmp, int x1, int y1, int x2, int y2, int colour);

//#define DEBUG_ENEMY_SIZE

BITMAP *player1;
BITMAP *player2;
//extern volatile int frames_per_second;
extern volatile int ticked;
extern int slacktime;

extern struct optionstruct options;

//BITMAP *ship_bitmaps [2] [65]; // externed in displ_in.c
BITMAP *ship_bitmaps [NO_SHIP_TYPES] [17];

BITMAP *enemy1_bmp [ENEMY1_BMPS];
RLE_SPRITE *enemy1_rle [ENEMY1_RLES];
RLE_SPRITE *small1_rle [SMALL1_RLES];
RLE_SPRITE *enemy2_rle [ENEMY2_RLES];
RLE_SPRITE *enemy3_rle [ENEMY3_RLES];
BITMAP *small2_bmp [SMALL2_BMPS];

BITMAP *superjelly_bmp [2];

RLE_SPRITE *large_ships [16] [5];


FONT *font2;
FONT *small_font;

void indicate_fps(BITMAP *bmp, int play);

void draw_lock(BITMAP *bmp, int dr, int x, int y, int rad);
void draw_turret_lock(BITMAP *bmp, int dr, int x, int y, int rad);
void draw_locks(BITMAP *bmp, int p);

float angle_to_radians(int angle);

int inflicteda, inflictede;

//void draw_grid(BITMAP *bmp, int xloc, int yloc, int max_x, int max_y, int colour);
void draw_stars(BITMAP *bmp, int max_x, int max_y, int player);
void draw_grid(BITMAP *bmp, int max_x, int max_y, int play, int colour, int edge_colour, int x_offset);
void draw_actors(BITMAP *bmp, int max_x, int max_y, int play);
void draw_hud(BITMAP *bmp, int x, int y, int i);

void draw_an_actor(BITMAP *bmp, int dr, int x, int y, int play);
void draw_ship_bitmap(BITMAP *bmp, int x, int y, int angle, int which_ship, int width, int height);
void draw_triangle_ship(BITMAP *bmp, int dr, int size, int pointiness, int x, int y, unsigned char colour1, unsigned char colour2);
//void draw_scan(BITMAP *bmp, int max_x, int max_y, int play, char range);
//void draw_scan_dot(BITMAP *bmp, int x, int y, int type);
void draw_status2(BITMAP *bmp, int max_x, int max_y, int play);
void draw_bullets(BITMAP *bmp, int max_x, int max_y, int play);
void draw_a_bullet(BITMAP *bmp, int dr, int x, int y, int x2, int y2, int max_x, int max_y, int play);
void draw_enemies(BITMAP *bmp, int max_x, int max_y, int play);
void draw_an_enemy(BITMAP *bmp, int dr, int x, int y);
void draw_clouds(BITMAP *bmp, int max_x, int max_y, int play);
void draw_a_cloud(BITMAP *bmp, int dr, int x, int y, int x2, int y2);
void draw_pickups(BITMAP *bmp, int max_x, int max_y, int play);
void draw_a_pickup(BITMAP *bmp, int dr, int x, int y);
void draw_crosshair(BITMAP *bmp, int dr, int x, int y);

void draw_sidekicks(BITMAP *bmp, int max_x, int max_y, int play, int a);

void draw_ship_status(BITMAP *bmp, int x, int y, int a, int style);
void draw_upgrades(BITMAP *bmp, int x, int y, int play);
char *upgrade_name(int i);

void draw_crawler_legs(BITMAP *bmp, int x, int y, int e, int col);

int set_lock_x [2];
int set_lock_y [2];
int set_lock_radius [2];
  
int set_turret_lock_x [2];
int set_turret_lock_y [2];
int set_turret_lock_radius [2];

char messages [2] [4] [70];
int message_style [2] [4];
int message_counter [2] [4];

void display_messages(BITMAP *bmp, int play, int max_x, int max_y);
void kill_message(int play, int i);

void run_display(void)
{
 static int counter;

 counter++;

 if (counter > 40000)
  counter = 0;

// int i;
  
// draw_grid(player1, 150 + counter, 150 + counter, 640, 480, 100, 2);
// draw_grid(player1, actor[0].x / GRAIN, actor[0].y / GRAIN, 640, 480, actor[0].angle, 2);
 if (game.users == 1)
 {
  set_lock_x [game.single_player] = 0;
  set_lock_y [game.single_player] = 0;
  set_lock_radius [game.single_player] = 0;
  set_turret_lock_x [game.single_player] = 0;
  set_turret_lock_y [game.single_player] = 0;
  set_turret_lock_radius [game.single_player] = 0;
//  draw_stars(player1, 640, 480, 0);
  clear_to_color(player1, arena.colour3);
  draw_grid(player1, 640, 480, game.single_player, arena.colour1, arena.colour2, 35000);
  draw_pickups(player1, 640, 480, game.single_player);
  draw_enemies(player1, 640, 480, game.single_player);
  draw_actors(player1, 640, 480, game.single_player);
  draw_bullets(player1, 640, 480, game.single_player);
  draw_clouds(player1, 640, 480, game.single_player);
//  draw_scan(player1, 50, 50, actor[player[0].actor_controlled].angle, 0, 0);
//  draw_scan(player1, 590, 50, actor[player[0].actor_controlled].angle, 0, 1);
//  draw_hud(player1, 320, 360, player[0].actor_controlled);
//  draw_upgrades(player1, 40, 420, game.single_player);
  draw_upgrades(player1, 200, 435, game.single_player);
  draw_status2(player1, 640, 480, game.single_player);
  draw_locks(player1, game.single_player);
/*  if (actor[player[game.single_player].actor_controlled].lock != -1
      && enemy[actor[player[game.single_player].actor_controlled].lock].type != ENEMY_NONE
      && set_lock_radius [game.single_player] != 0)
   draw_lock(player1, player[game.single_player].actor_controlled, set_lock_x [game.single_player], set_lock_y [game.single_player], set_lock_radius [game.single_player]);
  if (actor[player[game.single_player].actor_controlled].turret_lock != -1
      && enemy[actor[player[game.single_player].actor_controlled].turret_lock].type != ENEMY_NONE)
   draw_turret_lock(player1, player[game.single_player].actor_controlled, set_turret_lock_x [game.single_player], set_turret_lock_y [game.single_player], set_turret_lock_radius [game.single_player]);*/
  indicate_fps(player1, game.single_player);
  display_messages(player1, game.single_player, 640, 480);
/*  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
  for (i = 195; i < 206; i ++)
  {
   rectfill(player1, 10 + (i-195) * 30, 100, 30 + (i-195) * 30, 150, i);
  }
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);*/
  if (options.run_vsync > 0)
   vsync();
  blit(player1, screen, 0, 0, 0, 0, 640, 480);
 }
  else
  {
   set_lock_x [0] = 0;
   set_lock_y [0] = 0;
   set_lock_radius [0] = 0;
   set_turret_lock_x [0] = 0;
   set_turret_lock_y [0] = 0;
   set_turret_lock_radius [0] = 0;
   set_lock_x [1] = 0;
   set_lock_y [1] = 0;
   set_lock_radius [1] = 0;
   set_turret_lock_x [1] = 0;
   set_turret_lock_y [1] = 0;
   set_turret_lock_radius [1] = 0;
//   draw_stars(player1, 310, 480, 0);
   clear_to_color(player1, arena.colour3);
   clear_to_color(player2, arena.colour3);
   draw_grid(player1, 315, 480, 0, arena.colour1, arena.colour2, -2000);
   draw_pickups(player1, 315, 480, 0);
   draw_enemies(player1, 315, 480, 0);
   draw_actors(player1, 315, 480, 0);
   draw_bullets(player1, 315, 480, 0);
   draw_clouds(player1, 315, 480, 0);
//   draw_scan(player1, 50, 50, actor[player[0].actor_controlled].angle, 0, 0);
//   draw_scan(player1, 260, 50, actor[player[0].actor_controlled].angle, 0, 1);
//   draw_hud(player1, 155, 360, player[0].actor_controlled);
//   draw_upgrades(player1, 40, 420, 0);
   draw_upgrades(player1, 16, 460, 0);
   draw_status2(player1, 315, 480, 0);
   indicate_fps(player1, 0);
   display_messages(player1, 0, 310, 480);
   
//   draw_stars(player2, 310, 480, 1);
   draw_grid(player2, 315, 480, 1, arena.colour1, arena.colour2, -2000);
   draw_pickups(player2, 315, 480, 1);
   draw_enemies(player2, 315, 480, 1);
   draw_actors(player2, 315, 480, 1);
   draw_bullets(player2, 315, 480, 1);
   draw_clouds(player2, 315, 480, 1);
//   draw_scan(player2, 50, 50, actor[player[1].actor_controlled].angle, 1, 0);
//   draw_scan(player2, 260, 50, actor[player[1].actor_controlled].angle, 1, 1);
//   draw_hud(player2, 155, 360, player[1].actor_controlled);
   draw_upgrades(player2, 16, 460, 1);
   draw_status2(player2, 315, 480, 1);
   draw_locks(player1, 0);
   draw_locks(player2, 1);
/*   if (actor[player[1].actor_controlled].lock != -1
       && enemy[actor[player[1].actor_controlled].lock].type != ENEMY_NONE
       && set_lock_radius [1] != 0)
    draw_lock(player2, player[1].actor_controlled, set_lock_x [1], set_lock_y [1], set_lock_radius [1]);
   if (actor[player[1].actor_controlled].turret_lock != -1
       && enemy[actor[player[1].actor_controlled].turret_lock].type != ENEMY_NONE)
    draw_turret_lock(player2, player[1].actor_controlled, set_turret_lock_x [1], set_turret_lock_y [1], set_turret_lock_radius [1]);*/
   indicate_fps(player2, 1);
   display_messages(player2, 1, 315, 480);
   if (options.run_vsync > 0)
    vsync();
   blit(player1, screen, 0, 0, 0, 0, 315, 480);
   blit(player2, screen, 0, 0, 325, 0, 315, 480);
//   clear_bitmap(player1);
//   clear_bitmap(player2);
  }

}


void draw_locks(BITMAP *bmp, int p)
{
 int a = player[p].actor_controlled;

  if (actor[a].lock == LOCK_ACTOR0)
  {
   if (actor[0].in_play != 0)
    draw_lock(bmp, a, set_lock_x [p], set_lock_y [p], set_lock_radius [p]);
  }
   else
   {
    if (actor[a].lock == LOCK_ACTOR1)
    {
     if (actor[1].in_play != 0)
      draw_lock(bmp, a, set_lock_x [p], set_lock_y [p], set_lock_radius [p]);
    }
     else
     {
      if (actor[a].lock != -1
       && enemy[actor[a].lock].type != ENEMY_NONE
       && set_lock_radius [p] != 0)
      draw_lock(bmp, a, set_lock_x [p], set_lock_y [p], set_lock_radius [p]);
     }
   }

  if (actor[a].turret_lock == LOCK_ACTOR0)
  {
   if (actor[0].in_play != 0)
    draw_turret_lock(bmp, a, set_turret_lock_x [p], set_turret_lock_y [p], set_turret_lock_radius [p]);
  }
   else
   {
    if (actor[a].turret_lock == LOCK_ACTOR1)
    {
     if (actor[1].in_play != 0)
      draw_turret_lock(bmp, a, set_turret_lock_x [p], set_turret_lock_y [p], set_turret_lock_radius [p]);
    }
     else
     {
      if (actor[a].turret_lock != -1
       && enemy[actor[a].turret_lock].type != ENEMY_NONE
       && set_turret_lock_radius [p] != 0)
      draw_turret_lock(bmp, a, set_turret_lock_x [p], set_turret_lock_y [p], set_turret_lock_radius [p]);
     }
   }

//   if (actor[a].turret_lock != -1
//       && enemy[actor[a].turret_lock].type != ENEMY_NONE)
//    draw_turret_lock(bmp, a, set_turret_lock_x [p], set_turret_lock_y [p], set_turret_lock_radius [p]);

}


void draw_actors(BITMAP *bmp, int max_x, int max_y, int play)
{

 int i, x, y;

 for (i = 0; i < NO_ACTORS; i ++)
 {
  if (actor[i].in_play == 0)
   continue;

  if (actor[i].upgraded_system [UPG_SIDEKICK] > 0)
   draw_sidekicks(bmp, max_x, max_y, play, i);

  if (i == player[play].actor_controlled)
  {
   draw_an_actor(bmp, player[play].actor_controlled, max_x / 2, max_y / 2, play);
   draw_crosshair(bmp, player[play].actor_controlled, max_x / 2, max_y / 2);
   continue;
  }

  if (actor[i].x < actor[player[play].actor_controlled].x - (max_x / 2) * GRAIN
      || actor[i].x > actor[player[play].actor_controlled].x + (max_x / 2) * GRAIN
      || actor[i].y < actor[player[play].actor_controlled].y - (max_y / 2) * GRAIN
      || actor[i].y > actor[player[play].actor_controlled].y + (max_y / 2) * GRAIN)
       continue;

  x = (actor[i].x / GRAIN) - (actor[player[play].actor_controlled].x / GRAIN);
  y = (actor[i].y / GRAIN) - (actor[player[play].actor_controlled].y / GRAIN);
  draw_an_actor(bmp, i, x + max_x / 2, y + max_y / 2, play);
  
 }




}


void draw_an_actor(BITMAP *bmp, int dr, int x, int y, int play)
{

 int angle2 = 0;
 int angle = 0;
 if (angle < 0)
  angle += ANGLE_FULL;

 if (angle2 < 0)
  angle2 += ANGLE_FULL;

 switch(actor[dr].ship)
 {
  case SHIP_RUMSFELD:
   draw_ship_bitmap(bmp, x, y, 1, actor[dr].angle, 10, 10);
   break;
  case SHIP_LENTIL:
   draw_ship_bitmap(bmp, x, y, 2, actor[dr].angle, 10, 10);
   break;
  case SHIP_CAPYBARA:
   draw_ship_bitmap(bmp, x, y, 3, actor[dr].angle, 10, 10);
   break;
  case SHIP_HOOKWORM:
   draw_ship_bitmap(bmp, x, y, 4, actor[dr].angle, 10, 10);
   break;
  case SHIP_PORKUPINE:
   draw_ship_bitmap(bmp, x, y, 5, actor[dr].angle, 10, 10);
   break;
  case SHIP_PRONG:
   draw_ship_bitmap(bmp, x, y, 6, actor[dr].angle, 10, 10);
   break;
  case SHIP_AETHER:
   draw_ship_bitmap(bmp, x, y, 7, actor[dr].angle, 10, 10);
   break;
  case SHIP_DESPOT:
   draw_ship_bitmap(bmp, x, y, 8, actor[dr].angle, 10, 10);
   break;
  case SHIP_LACEWING:
   draw_ship_bitmap(bmp, x, y, 9, actor[dr].angle, 10, 10);
   break;
  case SHIP_TORTFEASOR:
   draw_ship_bitmap(bmp, x, y, 10, actor[dr].angle, 10, 10);
   break;
  case SHIP_SCORPION:
   draw_ship_bitmap(bmp, x, y, 11, actor[dr].angle, 10, 10);
   break;
  case SHIP_GODBOTHERER:
   draw_ship_bitmap(bmp, x, y, 12, actor[dr].angle, 10, 10);
   break;
  case SHIP_BOTULUS:
   draw_ship_bitmap(bmp, x, y, 13, actor[dr].angle, 10, 10);
   break;
  case SHIP_SLIDEWINDER:
   draw_ship_bitmap(bmp, x, y, 14, actor[dr].angle, 10, 10);
   break;
  case SHIP_DOOM:
   draw_ship_bitmap(bmp, x, y, 15, actor[dr].angle, 10, 10);
   break;

 }

 if (actor[dr].shield_pulse > 0)
 {
  int scol = COLOUR_WHITE;
  if (actor[dr].shield_pulse == 9)
   scol = COLOUR_GREY6;
  if (actor[dr].shield_pulse == 8)
   scol = COLOUR_GREY5;
  if (actor[dr].shield_pulse < 8)
   scol = COLOUR_BLUE1 + actor[dr].shield_pulse;
 
  circle(bmp, x + grand(5) - 2, y + grand(5) - 2, 10, scol);
 }
 
 int x1;//, y1;
 int gd = actor[dr].grace_period / 9;
 if (gd > 5)
  gd = 5;

 if (actor[dr].grace_period > 0)
 {
  x1 = gd + (actor[dr].grace_period / 3) % 4 + 3;
  if (((actor[dr].grace_period / 3) / 4) % 2 == 0)
   x1 = gd + 4 - (actor[dr].grace_period / 3) % 4 + 3;
  {
/*   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   circle(bmp, x, y, x1, TRANS_YELLOW);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);*/
   circle(bmp, x, y, x1, COLOUR_YELLOW3 + gd);
  }
//  circle(bmp, x + grand(3) - 1, y + grand(3) - 1, gd * 2 + grand(3), COLOUR_YELLOW1 + gd + grand(3));
 }
/* if (gd > 10)
  gd = 10;

 if (actor[dr].grace_period > 0)
 {
  x1 = x + cos(angle_to_radians(counter * 8)) * gd;
  y1 = y + sin(angle_to_radians(counter * 8)) * gd;
  putpixel(bmp, x1, y1, COLOUR_GREY2 + grand(5));
  x1 = x + cos(angle_to_radians(counter * 8) + PI / 2) * gd;
  y1 = y + sin(angle_to_radians(counter * 8) + PI / 2) * gd;
  putpixel(bmp, x1, y1, COLOUR_GREY2 + grand(5));
  x1 = x + cos(angle_to_radians(counter * 8) + PI) * gd;
  y1 = y + sin(angle_to_radians(counter * 8) + PI) * gd;
  putpixel(bmp, x1, y1, COLOUR_GREY2 + grand(5));
  x1 = x + cos(angle_to_radians(counter * 8) - PI / 2) * gd;
  y1 = y + sin(angle_to_radians(counter * 8) - PI / 2) * gd;
  putpixel(bmp, x1, y1, COLOUR_GREY2 + grand(5));
 }*/


  if (dr == 0 && actor[player[play].actor_controlled].lock == LOCK_ACTOR0)
  {
   set_lock_x [play] = x;// + (max_x / 2);
   set_lock_y [play] = y;// + (max_y / 2);
   set_lock_radius [play] = 12;
  }
  if (dr == 1 && actor[player[play].actor_controlled].lock == LOCK_ACTOR1)
  {
   set_lock_x [play] = x;// + (max_x / 2);
   set_lock_y [play] = y;// + (max_y / 2);
   set_lock_radius [play] = 12;
  }
  if (dr == 0 && actor[player[play].actor_controlled].turret_lock == LOCK_ACTOR0)
  {
   set_turret_lock_x [play] = x;// + (max_x / 2);
   set_turret_lock_y [play] = y;// + (max_y / 2);
   set_turret_lock_radius [play] = 12;
  }
  if (dr == 1 && actor[player[play].actor_controlled].turret_lock == LOCK_ACTOR1)
  {
   set_turret_lock_x [play] = x;// + (max_x / 2);
   set_turret_lock_y [play] = y;// + (max_y / 2);
   set_turret_lock_radius [play] = 12;
  }
  

}


void draw_crosshair(BITMAP *bmp, int dr, int x, int y)
{
 int xc, yc;
 float xcos;
 float ysin;
 int col = COLOUR_YELLOW8;
 if (actor[dr].recycle1 > 0)
  col = COLOUR_GREY6;
 
 xcos = cos(angle_to_radians(actor[dr].angle + 32 - ANGLE_QUARTER));
 ysin = sin(angle_to_radians(actor[dr].angle + 32 - ANGLE_QUARTER));
 xc = x + xcos * 32;
 yc = y + ysin * 32;
 putpixel(bmp, xc, yc, col);
 
 xcos = cos(angle_to_radians(actor[dr].angle - 32 - ANGLE_QUARTER));
 ysin = sin(angle_to_radians(actor[dr].angle - 32 - ANGLE_QUARTER));
 xc = x + xcos * 32;
 yc = y + ysin * 32;
 putpixel(bmp, xc, yc, col);
 
 xcos = cos(angle_to_radians(actor[dr].angle - ANGLE_QUARTER));
 ysin = sin(angle_to_radians(actor[dr].angle - ANGLE_QUARTER));
 xc = x + xcos * 30;
 yc = y + ysin * 30;
 putpixel(bmp, xc, yc, col);
 
 xc = x + xcos * 34;
 yc = y + ysin * 34;
 putpixel(bmp, xc, yc, col);

 if (actor[dr].secondary_weapon > 0)
 {
  col = COLOUR_RED8;
  if (actor[dr].recycle2 > 0)
   col = COLOUR_GREY5;
 
  xcos = cos(angle_to_radians(actor[dr].angle + 16 - ANGLE_QUARTER));
  ysin = sin(angle_to_radians(actor[dr].angle + 16 - ANGLE_QUARTER));
  xc = x + xcos * 32;
  yc = y + ysin * 32;
  putpixel(bmp, xc, yc, col);
 
  xcos = cos(angle_to_radians(actor[dr].angle - 16 - ANGLE_QUARTER));
  ysin = sin(angle_to_radians(actor[dr].angle - 16 - ANGLE_QUARTER));
  xc = x + xcos * 32;
  yc = y + ysin * 32;
  putpixel(bmp, xc, yc, col);

 }

 if (actor[dr].upgraded_system [UPG_SEEKER] != 0)
 {
  col = COLOUR_GREY5;
 
  xcos = cos(angle_to_radians(actor[dr].angle - ANGLE_QUARTER));
  ysin = sin(angle_to_radians(actor[dr].angle - ANGLE_QUARTER));
  xc = x + xcos * 75;
  yc = y + ysin * 75;
  if (actor[dr].lock != -1)
  {
    col = COLOUR_YELLOW8 - (counter / 2) % 4;
  }
  
  circle(bmp, xc, yc, 7, col);
  if (actor[dr].recycle2 == 0)
    circle(bmp, xc, yc, 3, COLOUR_RED4 + (counter / 2) % 4);

  // col = COLOUR_YELLOW8

  int x1, y1;
  x = xc - (30 + actor[dr].upgraded_system [UPG_SEEKER] * 25);
  y = yc - (30 + actor[dr].upgraded_system [UPG_SEEKER] * 25);
  x1 = xc - (33 + actor[dr].upgraded_system [UPG_SEEKER] * 25);
  y1 = yc - (33 + actor[dr].upgraded_system [UPG_SEEKER] * 25);
  pline(bmp, x, y, x1, y1, col);
  y = yc - (40 + actor[dr].upgraded_system [UPG_SEEKER] * 30);
  y1 = yc - (43 + actor[dr].upgraded_system [UPG_SEEKER] * 30);
  pline(bmp, xc, y, xc, y1, col);
  x = xc + (30 + actor[dr].upgraded_system [UPG_SEEKER] * 25);
  y = yc + (30 + actor[dr].upgraded_system [UPG_SEEKER] * 25);
  x1 = xc + (33 + actor[dr].upgraded_system [UPG_SEEKER] * 25);
  y1 = yc + (33 + actor[dr].upgraded_system [UPG_SEEKER] * 25);
  pline(bmp, x, y, x1, y1, col);
  y = yc + (40 + actor[dr].upgraded_system [UPG_SEEKER] * 30);
  y1 = yc + (43 + actor[dr].upgraded_system [UPG_SEEKER] * 30);
  pline(bmp, xc, y, xc, y1, col);
//  pline(bmp, x, y, x1, y, COLOUR_YELLOW8);
  x = xc + (30 + actor[dr].upgraded_system [UPG_SEEKER] * 25);
  y = yc - (30 + actor[dr].upgraded_system [UPG_SEEKER] * 25);
  x1 = xc + (33 + actor[dr].upgraded_system [UPG_SEEKER] * 25);
  y1 = yc - (33 + actor[dr].upgraded_system [UPG_SEEKER] * 25);
  pline(bmp, x, y, x1, y1, col);
  x = xc + (40 + actor[dr].upgraded_system [UPG_SEEKER] * 30);
  x1 = xc + (43 + actor[dr].upgraded_system [UPG_SEEKER] * 30);
  pline(bmp, x, yc, x1, yc, col);
  x = xc - (30 + actor[dr].upgraded_system [UPG_SEEKER] * 25);
  y = yc + (30 + actor[dr].upgraded_system [UPG_SEEKER] * 25);
  x1 = xc - (33 + actor[dr].upgraded_system [UPG_SEEKER] * 25);
  y1 = yc + (33 + actor[dr].upgraded_system [UPG_SEEKER] * 25);
  pline(bmp, x, y, x1, y1, col);
  x = xc - (40 + actor[dr].upgraded_system [UPG_SEEKER] * 30);
  x1 = xc - (43 + actor[dr].upgraded_system [UPG_SEEKER] * 30);
  pline(bmp, x, yc, x1, yc, col);

 }

}



void draw_ship_bitmap(BITMAP *bmp, int x, int y, int which_ship, int angle, int width, int height)
{
/*   int draw_angle = angle / 4;

   if (draw_angle < 64)
   {
    draw_sprite(bmp, ship_bitmaps [which_ship] [draw_angle], x - width, y - height);
    return;
   }
   if (draw_angle < 128)
   {
    draw_sprite_v_flip(bmp, ship_bitmaps [which_ship] [128 - draw_angle], x - width, y - height - 1);
    return;
   }
   if (draw_angle < 192)
   {
    draw_sprite_vh_flip(bmp, ship_bitmaps [which_ship] [draw_angle - 128], x - width - 2, y - height - 1);
    return;
   }
   draw_sprite_h_flip(bmp, ship_bitmaps [which_ship] [256 - draw_angle], x - width - 2, y - height);
*/
   int draw_angle = angle / 16;

   draw_angle ++;
   if (draw_angle > 64)
    draw_angle = 0;

   if (draw_angle < 16)
   {
    draw_sprite(bmp, ship_bitmaps [which_ship] [draw_angle], x - width, y - height);
    return;
   }
   if (draw_angle < 32)
   {
    draw_sprite_v_flip(bmp, ship_bitmaps [which_ship] [32 - draw_angle], x - width, y - height);
    return;
   }
   if (draw_angle < 48)
   {
    draw_sprite_vh_flip(bmp, ship_bitmaps [which_ship] [draw_angle - 32], x - width, y - height);
    return;
   }
   draw_sprite_h_flip(bmp, ship_bitmaps [which_ship] [64 - draw_angle], x - width, y - height);

}

void draw_triangle_ship(BITMAP *bmp, int dr, int size, int pointiness, int x, int y, unsigned char colour1, unsigned char colour2)
{
 int x1,y1,x2,y2,x3,y3;

 float radangle = angle_to_radians(actor[dr].angle);
// float radangle = angle_to_radians(actor[dr].angle);

 x1 = x + cos(radangle) * (size + pointiness);
 y1 = y + sin(radangle) * (size + pointiness);
 x2 = x + cos(radangle + (PI * 2) / 3) * size;
 y2 = y + sin(radangle + (PI * 2) / 3) * size;
 x3 = x + cos(radangle - (PI * 2) / 3) * size;
 y3 = y + sin(radangle - (PI * 2) / 3) * size;

 triangle(bmp, x1, y1, x2, y2, x3, y3, colour2);
 line(bmp, x1, y1, x2, y2, colour1);
 line(bmp, x1, y1, x3, y3, colour1);
 line(bmp, x2, y2, x3, y3, colour1);

}

void draw_status2(BITMAP *bmp, int max_x, int max_y, int play)
{

 int i, x, y;
 int a = player[play].actor_controlled;

 int minutes_left = arena.time_left / 2000;
 int seconds_left = (arena.time_left / 33.3333) - (minutes_left * 60);
 int centiseconds_left = (arena.time_left) - (minutes_left * 2000) - seconds_left * 33.333;
 centiseconds_left *= 3;
 int time_colour = COLOUR_GREY6;
 int time_colour2 = COLOUR_BLACK + centiseconds_left / 20;
 int time_x = 80;
 int time_y = 20;

 if (game.type != GAME_DUEL || game.duel_mode == DUEL_3_MINUTES || game.duel_mode == DUEL_10_MINUTES)
 {
  if (arena.level_finished == 1)
  {
   textprintf_centre(bmp, font, max_x / 2, 170, COLOUR_YELLOW8 - (counter / 4) % 4, "L_e_v_e_l}  C_l_e_a_r}");
   textprintf_centre(bmp, font2, max_x / 2, 170, COLOUR_ORANGE2 + (counter / 4) % 4, "L_e_v_e_l}  C_l_e_a_r}");
   time_colour = COLOUR_YELLOW8;
   time_colour2 = COLOUR_YELLOW4;
   if (arena.time_bonus > 0)
   {
    if (arena.seconds_left_when_finished == 1)
    {
     textprintf_centre(bmp, font, max_x / 2, 280, COLOUR_ORANGE4 + (counter / 4) % 4, "%i Second} Left}", arena.seconds_left_when_finished);
     textprintf_centre(bmp, font2, max_x / 2, 280, COLOUR_RED8 - (counter / 4) % 4, "%i Second} Left}", arena.seconds_left_when_finished);
    }
     else
     {
      textprintf_centre(bmp, font, max_x / 2, 280, COLOUR_ORANGE4 + (counter / 4) % 4, "%i Seconds} Left}", arena.seconds_left_when_finished);
      textprintf_centre(bmp, font2, max_x / 2, 280, COLOUR_RED8 - (counter / 4) % 4, "%i Seconds} Left}", arena.seconds_left_when_finished);
     }
    textprintf_centre(bmp, font, max_x / 2, 320, COLOUR_ORANGE4 + (counter / 4) % 4, "Bonus}: %i", arena.time_bonus);
    textprintf_centre(bmp, font2, max_x / 2, 320, COLOUR_RED8 - (counter / 4) % 4, "Bonus}: %i", arena.time_bonus);
   }
    else
    {
     textprintf_centre(bmp, font, max_x / 2, 320, COLOUR_YELLOW4 + (counter / 4) % 4, "No} Bonus}");
     textprintf_centre(bmp, font2, max_x / 2, 320, COLOUR_GREEN8 - (counter / 4) % 4, "No} Bonus}");
    }
  }
    else
    {
    if (minutes_left == 0 && seconds_left < 10)
    {
      time_colour = COLOUR_RED8 - (counter / 4) % 4;
      time_colour2 = COLOUR_ORANGE4 + (counter / 4) % 4;
     }
    }

// textprintf_right(bmp, font, time_x, 20, time_colour, "%i : %i %i", minutes_left, seconds_left, centiseconds_left);
  textprintf_right(bmp, font, time_x - 20, time_y, time_colour, "%i :", minutes_left);
  textprintf_right(bmp, font2, time_x - 20, time_y, time_colour2, "%i :", minutes_left);
  if (seconds_left < 10)
  {
   textprintf_right(bmp, font, time_x + 30, time_y, time_colour, "0%i", seconds_left);
   textprintf_right(bmp, font2, time_x + 30, time_y, time_colour2, "0%i", seconds_left);
  }
    else
    {
     textprintf_right(bmp, font, time_x + 30, time_y, time_colour, "%i", seconds_left);
     textprintf_right(bmp, font2, time_x + 30, time_y, time_colour2, "%i", seconds_left);
    }
  if (centiseconds_left < 10)
  {
   textprintf_right(bmp, small_font, time_x + 55, time_y + 21, time_colour, "0%i", centiseconds_left);
//  textprintf_right(bmp, font2, time_x + 80, 20, time_colour2, "0%i", centiseconds_left);
  }
    else
    {
     textprintf_right(bmp, small_font, time_x + 55, time_y + 21, time_colour, "%i", centiseconds_left);
//    textprintf_right(bmp, font2, time_x + 80, 20, time_colour2, "%i", centiseconds_left);
    }
 } // end if !duel || timed duel

// for (i = 0; i <
 if (game.type != GAME_DUEL)
 {
  textprintf_right(bmp, font, max_x - 20, time_y, COLOUR_GREY4, "%i", player[play].score);
  textprintf_right(bmp, font2, max_x - 20, time_y, COLOUR_GREY6, "%i", player[play].score);
  textprintf(bmp, small_font, 20, 56, COLOUR_GREY6, "Level %i", arena.level);
 }
  else
  {
   int dcol1 = COLOUR_YELLOW4;
   int dcol2 = COLOUR_BLUE8;
   if ((play == 0 && player[0].duel_score > player[1].duel_score)
      || (play == 1 && player[1].duel_score > player[0].duel_score))
   {
    dcol1 = COLOUR_YELLOW4;
    dcol2 = COLOUR_ORANGE8;
   }
   if (player[0].duel_score == player[1].duel_score)
   {
    dcol1 = COLOUR_BLUE4;
    dcol2 = COLOUR_BLUE8;
   }
   dcol1 += (counter / 4) % 4;
   dcol2 -= (counter / 4) % 4;
   textprintf_right(bmp, font, max_x - 20, time_y, dcol1, "%i", player[play].duel_score);
   textprintf_right(bmp, font2, max_x - 20, time_y, dcol2, "%i", player[play].duel_score);
   if (game.duel_mode == DUEL_10_POINTS)
    textprintf_right(bmp, small_font, max_x - 20, 56, COLOUR_GREY6, "First to 10");
   if (game.duel_mode == DUEL_30_POINTS)
    textprintf_right(bmp, small_font, max_x - 20, 56, COLOUR_GREY6, "First to 30");
  }


  
 if (actor[a].upgraded_system [UPG_TURRET] > 0)
 {
  int x2, y2;
  int col = COLOUR_YELLOW8 - (counter / 2) % 4;
  x = max_x / 2 - (50 + actor[a].upgraded_system [UPG_TURRET] * 17) - 5;
  y = max_y / 2 - (50 + actor[a].upgraded_system [UPG_TURRET] * 17) + 5;
  x2 = max_x / 2 - (50 + actor[a].upgraded_system [UPG_TURRET] * 17) + 5;
  y2 = max_y / 2 - (50 + actor[a].upgraded_system [UPG_TURRET] * 17) - 5;
  pline(bmp, x, y, x2, y2, col);
  x = max_x / 2 + (50 + actor[a].upgraded_system [UPG_TURRET] * 17) - 5;
  y = max_y / 2 + (50 + actor[a].upgraded_system [UPG_TURRET] * 17) + 5;
  x2 = max_x / 2 + (50 + actor[a].upgraded_system [UPG_TURRET] * 17) + 5;
  y2 = max_y / 2 + (50 + actor[a].upgraded_system [UPG_TURRET] * 17) - 5;
  pline(bmp, x, y, x2, y2, col);
  x = max_x / 2 + (50 + actor[a].upgraded_system [UPG_TURRET] * 17) - 5;
  y = max_y / 2 - (50 + actor[a].upgraded_system [UPG_TURRET] * 17) - 5;
  x2 = max_x / 2 + (50 + actor[a].upgraded_system [UPG_TURRET] * 17) + 5;
  y2 = max_y / 2 - (50 + actor[a].upgraded_system [UPG_TURRET] * 17) + 5;
  pline(bmp, x, y, x2, y2, col);
  x = max_x / 2 - (50 + actor[a].upgraded_system [UPG_TURRET] * 17) - 5;
  y = max_y / 2 + (50 + actor[a].upgraded_system [UPG_TURRET] * 17) - 5;
  x2 = max_x / 2 - (50 + actor[a].upgraded_system [UPG_TURRET] * 17) + 5;
  y2 = max_y / 2 + (50 + actor[a].upgraded_system [UPG_TURRET] * 17) + 5;
  pline(bmp, x, y, x2, y2, col);
 }

// Scanner:
// NOTE: Scanner assumes at most 2 players - 0 & 1!

// int dot_colour;

 rectfill(bmp, max_x - 80, max_y - 105, max_x - 5, max_y - 35, COLOUR_GREEN1);

  if (play == 0)
  {
   if (game.users > 1)
   {
    i = player[1].actor_controlled;
    if (actor[i].in_play != 0)
    {
     x = (actor[i].x * 75) / arena.max_x + max_x - 80;
     y = (actor[i].y * 70) / arena.max_y + max_y - 105;
     hline(bmp, max_x - 80, y, max_x - 5, COLOUR_GREY3);
     vline(bmp, x, max_y - 105, max_y - 35, COLOUR_GREY3);
    }
   }
   i = player[0].actor_controlled;
   if (actor[i].in_play != 0)
   {
    x = (actor[i].x * 75) / arena.max_x + max_x - 80;
    y = (actor[i].y * 70) / arena.max_y + max_y - 105;
    hline(bmp, max_x - 80, y, max_x - 5, COLOUR_WHITE);
    vline(bmp, x, max_y - 105, max_y - 35, COLOUR_WHITE);
   }
  }
   else
   {
    if (game.users > 1)
    {
     i = player[0].actor_controlled;
     if (actor[i].in_play != 0)
     {
      x = (actor[i].x * 75) / arena.max_x + max_x - 80;
      y = (actor[i].y * 70) / arena.max_y + max_y - 105;
      hline(bmp, max_x - 80, y, max_x - 5, COLOUR_GREY3);
      vline(bmp, x, max_y - 105, max_y - 35, COLOUR_GREY3);
     }
    }
    i = player[1].actor_controlled;
    if (actor[i].in_play != 0)
    {
     x = (actor[i].x * 75) / arena.max_x + max_x - 80;
     y = (actor[i].y * 70) / arena.max_y + max_y - 105;
     hline(bmp, max_x - 80, y, max_x - 5, COLOUR_WHITE);
     vline(bmp, x, max_y - 105, max_y - 35, COLOUR_WHITE);
    }
   }

 for (i = 0; i < NO_ENEMIES; i ++)
 {
  if (enemy[i].type == ENEMY_NONE)
//      || (enemy[i].type == ENEMY_CLOAKER2 - Too hard!
//          && enemy[i].attribute [ATTRIB_CLOAKER_PULSE] == 0))
   continue;
  x = (enemy[i].x * 75) / arena.max_x + max_x - 80;
  y = (enemy[i].y * 70) / arena.max_y + max_y - 105;
  if (enemy[i].target == TARGET_PRIMARY)
   putpixel(bmp, x, y, COLOUR_YELLOW8);
    else
     putpixel(bmp, x, y, COLOUR_ORANGE8);
 }

 rect(bmp, max_x - 80, max_y - 105, max_x - 5, max_y - 35, COLOUR_GREEN8);

// draw_ship_status(bmp, 80, max_y - 150, player[play].actor_controlled, 0);
 draw_ship_status(bmp, 50, max_y - 80, player[play].actor_controlled, 0);

 if (arena.game_over != 0)
 {
  if (game.type == GAME_DUEL)
  {
   if (game.duel_winner == -1)
   {
    textprintf_centre(bmp, font, max_x / 2, 200, COLOUR_YELLOW4 + (counter / 3) % 4, "Draw}");
    textprintf_centre(bmp, font2, max_x / 2, 200, COLOUR_GREEN8 - (counter / 3) % 4, "Draw}");
   }
    else
    {
     if (game.duel_winner == play)
     {
      textprintf_centre(bmp, font, max_x / 2, 200, COLOUR_YELLOW4 + (counter / 3) % 4, "You} Win}!");
      textprintf_centre(bmp, font2, max_x / 2, 200, COLOUR_ORANGE8 - (counter / 3) % 4, "You} Win}!");
     }
      else
      {
       textprintf_centre(bmp, font, max_x / 2, 200, COLOUR_GREEN4 + (counter / 3) % 4, "You} Lose}");
       textprintf_centre(bmp, font2, max_x / 2, 200, COLOUR_GREEN8 - (counter / 3) % 4, "You} Lose}");
      }
    }
  }
   else
   {
    textprintf_centre(bmp, font, max_x / 2, 200, COLOUR_GREEN4 + (counter / 3) % 4, "G_a_m_e}  O_v_e_r}");
    textprintf_centre(bmp, font2, max_x / 2, 200, COLOUR_GREEN8 - (counter / 3) % 4, "G_a_m_e}  O_v_e_r}");
   }
 }

 if (game.type != GAME_DUEL)
 {
  if (arena.targets_left_total > 0)
  {
   char targets_left_string [30];
/// int j;

   for (i = 0; i < arena.targets_left_total; i ++)
   {
    targets_left_string [i] = 'X';
   }
   targets_left_string [i] = '\0';

   textprintf(bmp, small_font, 20, 70, COLOUR_GREY4, targets_left_string);
   textprintf(bmp, small_font, 19, 69, COLOUR_GREY6, targets_left_string);
//  textprintf_centre(bmp, small_font, max_x / 2, 60, COLOUR_GREY4, targets_left_string);
//  textprintf_centre(bmp, small_font, max_x / 2 - 1, 59, COLOUR_GREY6, targets_left_string);
//    rectfill(bmp, x + i * 10 - 3, y - 30, x + i * 10 + 3, y - 24, COLOUR_RED8);
  }


// if (arena.time_left > 195 && arena.time_left < 330 && (arena.time_left / 15) % 3 != 0)
  if (arena.time_left > 240 && arena.time_left < 330 && (arena.time_left / 10) % 3 != 0)
  {
    textprintf_centre(bmp, font, max_x / 2, 250, COLOUR_ORANGE4 + (counter / 2) % 4, "Hurry} Up}!");
    textprintf_centre(bmp, font2, max_x / 2, 250, COLOUR_RED8 - (counter / 2) % 4, "Hurry} Up}!");
  }
 
// if (arena.hostile > 0 && arena.hostile < 150 && (arena.hostile / 15) % 2 == 0)
  if (arena.hostile > 0 && arena.hostile < 90 && (arena.hostile / 10) % 3 != 0)
  {
    textprintf_centre(bmp, font, max_x / 2, 250, COLOUR_ORANGE4 + (counter / 2) % 4, "Defences} On}-Line}");
    textprintf_centre(bmp, font2, max_x / 2, 250, COLOUR_RED8 - (counter / 2) % 4, "Defences} On}-Line}");
  }
 }
 
}



void draw_hud(BITMAP *bmp, int x, int y, int i)
{
 return;
// int viewing_angle = actor[i].moving_angle - actor[i].angle - ANGLE_QUARTER;

// draw_scan(bmp, 50, 50, 0, 0);
/*
 if (game[0].users == 2)
  draw_scan(bmp, 260, 50, 0, 1);
   else
    draw_scan(bmp, 590, 50, 0, 1);

 draw_ship_status(bmp, 50, 420, i, 1);

 if (actor[i].x_speed != 0 || actor[i].y_speed != 0)
 {
  // start off with the velocity indicator:
//  int distance1 = sqrt(hypot(actor[i].x_speed, actor[i].y_speed)) + 5 + actor[i].radius;
//  int distance2 = (sqrt(hypot(actor[i].x_speed, actor[i].y_speed)) * 4) / 5 + 5 + actor[i].radius;
/ *  int x_disp1 = cos(angle_to_radians(viewing_angle)) * distance1;
  int y_disp1 = sin(angle_to_radians(viewing_angle)) * distance1;
  int x_disp2 = cos(angle_to_radians(viewing_angle + 10)) * distance2;
  int y_disp2 = sin(angle_to_radians(viewing_angle + 10)) * distance2;
  int x_disp3 = cos(angle_to_radians(viewing_angle - 10)) * distance2;
  int y_disp3 = sin(angle_to_radians(viewing_angle - 10)) * distance2;

  line(bmp, x + x_disp1, y + y_disp1, x + x_disp2, y + y_disp2, COLOUR_GREEN4);
  line(bmp, x + x_disp1, y + y_disp1, x + x_disp3, y + y_disp3, COLOUR_GREEN4);
*/
// }
 
}

void draw_ship_status(BITMAP *bmp, int x, int y, int a, int style)
{

 int i, a1, a2;

 int shield_colour;

 char numbers [10];

 if (game.type != GAME_DUEL)
 {
  for (i = 0; i < game.ships_left; i ++)
  {
     rectfill(bmp, x - 8, y - 70 - i * 9, x - 15, y - 77 - i * 9, COLOUR_BLUE7);
  }
 }

 if (actor[a].in_play == 0)
  return;


 int armoured = (actor[a].armour * 100) / actor[a].max_armour;
 int armour_colour = RLE_LSHIP_BLUE;
// if (actor[a].in_play == 1)
// {
  if (armoured < 100)
   armour_colour = RLE_LSHIP_GREEN;
  if (armoured < 70)
   armour_colour = RLE_LSHIP_YELLOW;
  if (armoured < 30)
   armour_colour = RLE_LSHIP_RED;

  draw_rle_sprite(bmp, large_ships [actor[a].ship] [armour_colour], x - 24, y - 24);

  if (actor[a].hurt_pulse > 0)
   draw_rle_sprite(bmp, large_ships [actor[a].ship] [RLE_LSHIP_LINES], x - 24, y - 24);

  armour_colour = COLOUR_GREY4;
  int armour_colour2 = COLOUR_GREY6;

  if (armoured < 100)
  {
   armour_colour = COLOUR_GREEN4;
   armour_colour2 = COLOUR_GREEN7;
  }
  if (armoured < 70)
  {
   armour_colour = COLOUR_YELLOW4;
   armour_colour2 = COLOUR_YELLOW7;
  }
  if (armoured < 30)
  {
   armour_colour = COLOUR_RED4;
   armour_colour2 = COLOUR_RED7;
  }

  itoa(actor[a].armour / 10, numbers, 10);
  textprintf(bmp, font, x + 48, y + 15, armour_colour, numbers);
  textprintf(bmp, font2, x + 48, y + 15, armour_colour2, numbers);

  if (actor[a].upgraded_system [UPG_SHIELD] > 0)
  {
   itoa(actor[a].shield / 10, numbers, 10);
   textprintf(bmp, font, x + 55, y - 20, COLOUR_BLUE3 + grand(4), numbers);
   textprintf(bmp, font2, x + 55, y - 20, COLOUR_BLUE6 + grand(3), numbers);
  }

// }
 
// circlefill(bmp, x, y, 7, armour_colour);

 int layers = actor[a].shield / 100;
 int last_layer = actor[a].shield % 100;

 if (actor[a].in_play == 1
     && actor[a].upgraded_system [UPG_SHIELD] > 0 && actor[a].shield > 0)
 {
  for (i = 0; i < layers; i ++)
  {
   shield_colour = COLOUR_BLUE6 + grand(3);
//   a2 = 64 + (actor[a].shield * 96) / actor[a].max_shield;
//   a1 = 64 - (actor[a].shield * 96) / actor[a].max_shield;
   arc(bmp, x, y, itofix(-28), itofix(156), 35 + i * 3, shield_colour);

  }
  
   if (last_layer != 0)
   {
    shield_colour = COLOUR_BLUE6 + grand(3);
    if (actor[a].shield_pulse > 0)
    {
     shield_colour = COLOUR_GREY4 + actor[a].shield_pulse / 3;
     if (shield_colour > COLOUR_WHITE)
      shield_colour = COLOUR_WHITE;
    }
//    a2 = 64 + (last_layer * 96) / 100;
//    a1 = 64 - (last_layer * 96) / 100;
    a2 = 64 + (last_layer * 96) / 100;
    a1 = 64 - (last_layer * 96) / 100;
    arc(bmp, x, y, itofix(a1), itofix(a2), 35 + layers * 3, shield_colour);
   }
/*
 if (actor[a].upgraded_system [UPG_SHIELD] > 0 && actor[a].shield > 0)
 {
  for (i = 0; i < actor[a].upgraded_system [UPG_SHIELD] + 1; i ++)
  {
   shield_colour = COLOUR_BLUE6 + grand(3);
   if (i == actor[a].upgraded_system [UPG_SHIELD]
       && actor[a].shield_pulse > 0)
    {
     shield_colour = COLOUR_GREY4 + actor[a].shield_pulse / 3;
     if (shield_colour > COLOUR_WHITE)
      shield_colour = COLOUR_WHITE;
    }
   a2 = 64 + (actor[a].shield * 96) / actor[a].max_shield;
   a1 = 64 - (actor[a].shield * 96) / actor[a].max_shield;
   arc(bmp, x, y, itofix(a1), itofix(a2), 25 + i * 3, shield_colour);

  }
*/
 }

 if (actor[a].ship != SHIP_SCORPION)
 {
  if (player[actor[a].controller].link_fire == 1)
  {
    circlefill(bmp, x - 36, y + 43, 2, COLOUR_YELLOW2);
    textprintf_centre(bmp, small_font, x - 35, y + 37, COLOUR_GREEN8, "X");
  }
   else
   {
    circlefill(bmp, x - 36, y + 43, 2, COLOUR_RED2);
    textprintf_centre(bmp, small_font, x - 36, y + 37, COLOUR_ORANGE8, "||");
   }
 }

 if (actor[a].upgraded_system [UPG_RETRO] == 0)
 {
  if (actor[a].dragging == 1)
  {
    circlefill(bmp, x - 36, y + 33, 2, COLOUR_RED8);
  }
   else
   {
    circlefill(bmp, x - 36, y + 33, 2, COLOUR_YELLOW3);
   }
 }
 
 armour_colour = COLOUR_GREY4;
 if (actor[a].repairing > 0)
 {
  if (actor[a].repairing > 173)
   armour_colour = COLOUR_YELLOW8;
    else
     armour_colour = COLOUR_YELLOW1 + actor[a].repairing / 25;
  armour_colour2 = COLOUR_ORANGE1 + (actor[a].repairing / 4) % 8;

  itoa(actor[a].repairing / 2, numbers, 10);
  textprintf(bmp, font, x + 48, y + 45, armour_colour, numbers);
  textprintf(bmp, font2, x + 48, y + 45, armour_colour2, numbers);

  armour_colour = COLOUR_YELLOW2 + (counter / 3) % 6;

 }
// armoured = (actor[a].energy * 50) / actor[a].max_energy;
 armoured /= 2;
 rectfill(bmp, x - 25, y + 30, x + armoured - 25, y + 45, armour_colour);
 rect(bmp, x - 25, y + 30, x + 25, y + 45, COLOUR_GREY6);
// add text

// for (i = 0; i < player[actor[a].controller].ships_left; i ++)
}


void draw_upgrades(BITMAP *bmp, int x, int y, int play)
{
 text_mode(-1);
 int i, j;
 int tcol = COLOUR_GREEN4;

 for (i = 0; i < NO_UPGRADES; i ++)
 {
  if (actor[player[play].actor_controlled].upgrade_slot == i)
  {
    rectfill(bmp, x + i * 35 - 15, y - 2, x + i * 35 + 15, y + 10, COLOUR_YELLOW2);
    textprintf_centre(bmp, small_font, x + i * 35, y, COLOUR_YELLOW4, upgrade_name(actor[player[play].actor_controlled].upgrade_structure [i]));
    rect(bmp, x + i * 35 - 15, y - 2, x + i * 35 + 15, y + 10, COLOUR_YELLOW7);
  }
   else
  {
    tcol = COLOUR_GREEN4;
    if (actor[player[play].actor_controlled].just_upgraded == i
        && actor[player[play].actor_controlled].just_upgraded_timeout > 0)
        {
          tcol = COLOUR_GREEN5;
          if (actor[player[play].actor_controlled].just_upgraded_timeout > 3)
          {
           tcol = COLOUR_GREEN7;
          }
          if (actor[player[play].actor_controlled].just_upgraded_timeout > 6)
          {
           tcol = COLOUR_WHITE;
          }
        }
    textprintf_centre(bmp, small_font, x + i * 35, y, tcol, upgrade_name(actor[player[play].actor_controlled].upgrade_structure [i]));
    rect(bmp, x + i * 35 - 15, y - 2, x + i * 35 + 15, y + 10, tcol);
  }
  if (actor[player[play].actor_controlled].upgrades [i] == 5)
  {
     rectfill(bmp, x + i * 35 - 12, y + 14, x + i * 35 + 12, y + 17, COLOUR_YELLOW8);
  }
   else
    for (j = 0; j < actor[player[play].actor_controlled].upgrades [i]; j ++)
    {
     rectfill(bmp, x + i * 35 - 15 + j * 5 + 3, y + 14, x + i * 35 - 15 + j * 5 + 3 + 3, y + 17, COLOUR_BLUE7);
//   circlefill(bmp, x + i * 35 - 15 + j * 4, y + 20, 2, COLOUR_BLUE4);
    }
 }

}


char *upgrade_name(int i)
{
 switch (i)
 {
  case UPG_NONE:
   return "none";
  case UPG_SPEED:
   return "speed";
  case UPG_ARMOUR:
   return "armour";
  case UPG_POWER:
   return "power";
  case UPG_AUTOFIRE:
   return "autof";
  case UPG_MULTIFIRE:
   return "multif";
  case UPG_PROJECT:
   return "proj";
  case UPG_WARHEAD:
   return "whead";
  case UPG_ROCKET:
   return "rocket";
  case UPG_TUBES:
   return "tubes";
  case UPG_SLIDE:
   return "slide";
  case UPG_SEEKER:
   return "seek";
  case UPG_BOMB:
   return "bomb";
  case UPG_LAUNCHER:
   return "launch";
  case UPG_LOADER:
   return "load";
  case UPG_SPECIAL:
   return "special";
  case UPG_TURRET:
   return "turret";
  case UPG_SIDEKICK:
   return "sidek";
  case UPG_HEAVY:
   return "heavy";
  case UPG_ORBITAL:
   return "orbit";
  case UPG_BACKFIRE:
   return "backf";
  case UPG_SHIELD:
   return "shield";
  case UPG_RETRO:
   return "retro";
  case UPG_ROAMER:
   return "roam";
  case UPG_MINIMISSILES:
   return "minim";
  case UPG_DRIFT:
   return "drift";


   
 }

 return "ERROR!";
}



void draw_scan_dot(BITMAP *bmp, int x, int y, int type)
{

 switch(type)
 {
  case ACTORTYPE_SHIP:
   putpixel(bmp, x, y, COLOUR_BLUE4);
   break;
  case ACTORTYPE_REDSHIP:
   putpixel(bmp, x, y, COLOUR_RED4);
/*   putpixel(bmp, x + 1, y, 2);
   putpixel(bmp, x - 1, y, 2);
   putpixel(bmp, x, y + 1, 2);
   putpixel(bmp, x, y - 1, 2);*/
   break;

 }

}



void draw_bullets(BITMAP *bmp, int max_x, int max_y, int play)
{

 int i, x, y, x2, y2;

 for (i = 0; i < NO_BULLETS; i ++)
 {
  if (bullet[i].type == BULLET_NONE)
   continue;
  if (bullet[i].left_owner == 0)
   continue;
  x = (bullet[i].x / GRAIN) - (actor[player[play].actor_controlled].x / GRAIN);
  y = (bullet[i].y / GRAIN) - (actor[player[play].actor_controlled].y / GRAIN);
  if ((bullet[i].x < actor[player[play].actor_controlled].x - (max_x / 2) * GRAIN
      || bullet[i].x > actor[player[play].actor_controlled].x + (max_x / 2) * GRAIN
      || bullet[i].y < actor[player[play].actor_controlled].y - (max_y / 2) * GRAIN
      || bullet[i].y > actor[player[play].actor_controlled].y + (max_y / 2) * GRAIN)
      &&
        (bullet[i].x2 < actor[player[play].actor_controlled].x - (max_x / 2) * GRAIN
        || bullet[i].x2 > actor[player[play].actor_controlled].x + (max_x / 2) * GRAIN
        || bullet[i].y2 < actor[player[play].actor_controlled].y - (max_y / 2) * GRAIN
        || bullet[i].y2 > actor[player[play].actor_controlled].y + (max_y / 2) * GRAIN))
      continue;
  x2 = (bullet[i].x2 / GRAIN) - (actor[player[play].actor_controlled].x / GRAIN);
  y2 = (bullet[i].y2 / GRAIN) - (actor[player[play].actor_controlled].y / GRAIN);
  draw_a_bullet(bmp, i, x + (max_x / 2), y + (max_y / 2), x2 + (max_x / 2), y2 + (max_y / 2), max_x, max_y, play);
  
 }

}


// angle is the angle it's being viewed at, not its direction
void draw_a_bullet(BITMAP *bmp, int dr, int x, int y, int x2, int y2, int max_x, int max_y, int play)
{
 int xa, ya, xb, yb, xc, yc, cs;

 switch(bullet[dr].type)
 {
  case BULLET_TORPEDO2:
   cs = angle_to_radians(bullet[dr].angle);
   xa = (x) + ((float) cos(cs) * 3);
   ya = (y) + ((float) sin(cs) * 3);
   xb = (x) + ((float) cos(cs + (PI * 2) / 3) * 4);
   yb = (y) + ((float) sin(cs + (PI * 2) / 3) * 4);
   xc = (x) + ((float) cos(cs - (PI * 2) / 3) * 4);
   yc = (y) + ((float) sin(cs - (PI * 2) / 3) * 4);
/*   xa = (x) + ((float) cos(radangle + PI) * 3);
   ya = (y) + ((float) sin(radangle + PI) * 3);
   xb = (x) + ((float) cos(radangle + PI + (PI * 2) / 3) * 4);
   yb = (y) + ((float) sin(radangle + PI + (PI * 2) / 3) * 4);
   xc = (x) + ((float) cos(radangle + PI - (PI * 2) / 3) * 4);
   yc = (y) + ((float) sin(radangle + PI - (PI * 2) / 3) * 4);*/
   triangle(bmp, xa, ya, xb, yb, xc, yc, COLOUR_GREY5);
   line(bmp, xa, ya, xb, yb, COLOUR_BLUE7);
   line(bmp, xa, ya, xc, yc, COLOUR_BLUE7);
   line(bmp, xb, yb, xc, yc, COLOUR_BLUE7);
   break;
  case BULLET_HOSTILE:
   break;
  case BULLET_STING:
  case BULLET_STING2:
//   xa = x2 - x;
//   ya = y2 - y;
   x = (bullet[dr].x) - (actor[player[play].actor_controlled].x) + (max_x / 2) * GRAIN;
   y = (bullet[dr].y) - (actor[player[play].actor_controlled].y) + (max_y / 2) * GRAIN;
   x2 = (bullet[dr].x2) - (actor[player[play].actor_controlled].x) + (max_x / 2) * GRAIN;
   y2 = (bullet[dr].y2) - (actor[player[play].actor_controlled].y) + (max_y / 2) * GRAIN;
   xa = x2 - x;
   ya = y2 - y;
   pline(bmp, x / GRAIN, y / GRAIN, (x + 3 * xa) / GRAIN, (y + 3 * ya) / GRAIN, bullet[dr].colours [0]);
//   putpixel(bmp, x / GRAIN, y / GRAIN, bullet[dr].colours [0]);
   break;
  case BULLET_POWERED:
  case BULLET_ZAP:
/*  x2 = bullet[dr].x_speed;
  x2 /= GRAIN;
  x2 += x;
  
  y2 = bullet[dr].y_speed;
  y2 /= GRAIN;
  y2 += y;
  
  if (x2 < -100 || x2 > 800 || y2 < -100 || y2 > 600)
   break;*/
//  xa = x2 - x;
//  ya = y2 - y;
  x = (bullet[dr].x) - (actor[player[play].actor_controlled].x);
  y = (bullet[dr].y) - (actor[player[play].actor_controlled].y);
  x2 = (bullet[dr].x2) - (actor[player[play].actor_controlled].x);
  y2 = (bullet[dr].y2) - (actor[player[play].actor_controlled].y);
  xa = x2 - x;
  ya = y2 - y;
  pline(bmp, (x) / GRAIN + (max_x / 2), y / GRAIN + (max_y / 2), (x + xa) / GRAIN + (max_x / 2), (y + ya) / GRAIN + (max_y / 2), bullet[dr].colours [0]);
  pline(bmp, (x + xa) / GRAIN + (max_x / 2), (y + ya) / GRAIN + (max_y / 2), (x + 2 * xa) / GRAIN + (max_x / 2), (y + 2 * ya) / GRAIN + (max_y / 2), bullet[dr].colours [1]);
  pline(bmp, (x + 2 * xa) / GRAIN + (max_x / 2), (y + 2 * ya) / GRAIN + (max_y / 2), (x + 3 * xa) / GRAIN + (max_x / 2), (y + 3 * ya) / GRAIN + (max_y / 2), bullet[dr].colours [2]);
//  pline(bmp, (x) / GRAIN + max_x / 2, y / GRAIN, (x + xa) / GRAIN, (y + ya) / GRAIN, bullet[dr].colours [0]);
//  pline(bmp, (x + xa) / GRAIN, (y + ya) / GRAIN, (x + 2 * xa) / GRAIN, (y + 2 * ya) / GRAIN, bullet[dr].colours [1]);
//  pline(bmp, (x + 2 * xa) / GRAIN, (y + 2 * ya) / GRAIN, (x + 3 * xa) / GRAIN, (y + 3 * ya) / GRAIN, bullet[dr].colours [2]);
/*  pline(bmp, (x), y, x + xa, y + ya, COLOUR_RED4);
  pline(bmp, x + xa, y + ya, x + 2 * xa, y + 2 * ya, COLOUR_RED3);
  pline(bmp, x + 2 * xa, y + 2 * ya, x + 3 * xa, y + 3 * ya, COLOUR_RED2);*/
//  pline(bmp, x, y, x2, y2, COLOUR_RED4);
//  circlefill(bmp, x2, y2, 3, COLOUR_RED4);
//  circlefill(bmp, x, y, 1, COLOUR_GREEN4);
   break;
//  case BULLET_POWERED:
//  circlefill(bmp, (x), y, 1, bullet[dr].colours [random() % 4]);
  if (crandom(3) == 0)
   circlefill(bmp, (x), y, 1, bullet[dr].colours [2]);
    else
     circlefill(bmp, (x), y, 1, bullet[dr].colours [0]);
//  circlefill(bmp, (x), y, 1 + random() % 2, bullet[dr].colours [0]);
   break;
  case BULLET_PLASMA:
/*  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
  if (crandom(4) == 0)
   circlefill(bmp, (x), y, 3, bullet[dr].colours [grand(4)]);
    else
     circlefill(bmp, (x), y, 4, bullet[dr].colours [2]);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);*/
   switch(bullet[dr].colours [0])
   {
    case TRANS_DRED:
     xa = RLE_SMALL1_RED_BLOB_L;
     break;
    default:
    case TRANS_DBLUE:
     xa = RLE_SMALL1_BLUE_BLOB_L;
     break;
    case TRANS_DGREEN:
     xa = RLE_SMALL1_GREEN_BLOB_L;
     break;
    case TRANS_LORANGE:
     xa = RLE_SMALL1_ORANGE_BLOB_L;
     break;
    case TRANS_YELLOW:
     xa = RLE_SMALL1_YELLOW_BLOB_L;
     break;
   }
     if ((counter / 3) % 4 == 1)
      draw_rle_sprite(bmp, small1_rle [xa], (x) - 5, (y) - 5);
     if ((counter / 3) % 2 == 0)
      draw_rle_sprite(bmp, small1_rle [xa + 1], (x) - 5, (y) - 5);
     if ((counter / 3) % 4 == 3)
      draw_rle_sprite(bmp, small1_rle [xa + 2], (x) - 5, (y) - 5);
//  circlefill(bmp, (x), y, 1 + random() % 2, bullet[dr].colours [0]);
   break;

  case BULLET_BOMB:
     draw_sprite(bmp, small2_bmp [BMP_SMALL2_BOMB], (x) - 3, (y) - 3);
     break;
  case BULLET_BOMBLET:
  case BULLET_BOMB_SIDE:
     draw_sprite(bmp, small2_bmp [BMP_SMALL2_SIDE_BOMB], (x) - 3, (y) - 3);
     break;
  case BULLET_MISSILE:
     switch((bullet[dr].angle % ANGLE_FULL) / ANGLE_QUARTER)
     {
       case 0:
       xa = (bullet[dr].angle % ANGLE_FULL) / 42;
       if (xa > 6) xa = 6;
       if (xa < 0) xa = 0;
       draw_sprite(bmp, small2_bmp [BMP_SMALL2_MISSILE_7 - xa], (x) - 3, (y) - 3);
       break;
       case 1:
       xa = (bullet[dr].angle - ANGLE_QUARTER) / 42;
       if (xa > 6) xa = 6;
       if (xa < 0) xa = 0;
       draw_sprite_v_flip(bmp, small2_bmp [BMP_SMALL2_MISSILE_1 + xa], (x) - 3, (y) - 3);
       break;
       case 2:
       xa = (bullet[dr].angle - ANGLE_HALF) / 42;
       if (xa > 6) xa = 6;
       if (xa < 0) xa = 0;
       draw_sprite_vh_flip(bmp, small2_bmp [BMP_SMALL2_MISSILE_7 - xa], (x) - 3, (y) - 3);
       break;
       case 3:
       xa = (bullet[dr].angle - ANGLE_HALF - ANGLE_QUARTER) / 42;
       if (xa > 6) xa = 6;
       if (xa < 0) xa = 0;
       draw_sprite_h_flip(bmp, small2_bmp [BMP_SMALL2_MISSILE_1 + xa], (x) - 3, (y) - 3);
       break;

     }
//     circlefill(bmp, (x), y, 2, COLOUR_GREY6);
     break;
  case BULLET_PREMINE:
     draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_PREMINE], (x) - 3, (y) - 3);
     break;
  case BULLET_MINE:
//     if (dr % 2 == 0) // as it's purely for display, this horrible thing is okay
//      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_MINE1 + (bullet[dr].timeout / 16) % 3], (x) - 5, (y) - 5);
//       else
        draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_MINE3 - (bullet[dr].timeout / (16)) % 3], (x) - 3, (y) - 3);
     break;
  case BULLET_PRESEEKMINE:
     draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_PRESEEKMINE], (x) - 3, (y) - 3);
     break;
  case BULLET_SEEKMINE:
     draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_SEEKMINE3 - (bullet[dr].timeout / (12)) % 3], (x) - 3, (y) - 3);
     break;
  case BULLET_MISSILE_MINI:
     circlefill(bmp, (x), y, 1, COLOUR_GREY5);
     break;
  case BULLET_NICE_ORBITAL:
     circlefill(bmp, x, y, 1, bullet[dr].colours [0]);
     break;
  case BULLET_BLUE_BLOB:
     circlefill(bmp, (x), y, 3, COLOUR_BLUE8);
     break;
//     circlefill(bmp, (x), y, 5, bullet[dr].colours [0]);
  case BULLET_FLAK:
  case BULLET_RED_BLOB:
     if ((counter / 3) % 4 == 1)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_RED_BLOB_S], (x) - 5, (y) - 5);
     if ((counter / 3) % 2 == 0)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_RED_BLOB_M], (x) - 5, (y) - 5);
     if ((counter / 3) % 4 == 3)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_RED_BLOB_L], (x) - 5, (y) - 5);
     break;
  case BULLET_YELLOW_BLOB:
  case BULLET_TORPEDO:
//     circlefill(bmp, (x), y, 3, COLOUR_YELLOW8);
     if ((counter / 3) % 4 == 1)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_YELLOW_BLOB_S], (x) - 5, (y) - 5);
     if ((counter / 3) % 2 == 0)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_YELLOW_BLOB_M], (x) - 5, (y) - 5);
     if ((counter / 3) % 4 == 3)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_YELLOW_BLOB_L], (x) - 5, (y) - 5);
     break;
  case BULLET_ORBITAL:
  case BULLET_CURVE:
  case BULLET_PURPLE_BLOB:
     if ((counter / 3) % 4 == 1)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_PURPLE_BLOB_S], (x) - 5, (y) - 5);
     if ((counter / 3) % 2 == 0)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_PURPLE_BLOB_M], (x) - 5, (y) - 5);
     if ((counter / 3) % 4 == 3)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_PURPLE_BLOB_L], (x) - 5, (y) - 5);
     break;
  case BULLET_ORANGE_BLOB:
     if ((counter / 3) % 4 == 1)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_ORANGE_BLOB_S], (x) - 5, (y) - 5);
     if ((counter / 3) % 2 == 0)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_ORANGE_BLOB_M], (x) - 5, (y) - 5);
     if ((counter / 3) % 4 == 3)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_ORANGE_BLOB_L], (x) - 5, (y) - 5);
     break;
  case BULLET_SEEKER_BLOB3:
  case BULLET_SEEKER_BLOB:
     if ((counter / 3) % 4 == 1)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_BLUE_BLOB_S], (x) - 5, (y) - 5);
     if ((counter / 3) % 2 == 0)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_BLUE_BLOB_M], (x) - 5, (y) - 5);
     if ((counter / 3) % 4 == 3)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_BLUE_BLOB_L], (x) - 5, (y) - 5);
     break;
  case BULLET_SEEKER_BLOB2:
     if ((counter / 3) % 4 == 1)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_GREEN_BLOB_S], (x) - 5, (y) - 5);
     if ((counter / 3) % 2 == 0)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_GREEN_BLOB_M], (x) - 5, (y) - 5);
     if ((counter / 3) % 4 == 3)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_GREEN_BLOB_L], (x) - 5, (y) - 5);
     break;
  case BULLET_PRONG:
   cs = bullet[dr].angle;
   if (bullet[dr].special1 == 1)
    cs = ANGLE_FULL - bullet[dr].angle;
   xa = x + cos(angle_to_radians(cs)) * (2);
   ya = y + sin(angle_to_radians(cs)) * (2);
   xb = x - cos(angle_to_radians(cs)) * (2);
   yb = y - sin(angle_to_radians(cs)) * (2);
   pline(bmp, xa, ya, xb, yb, bullet[dr].colours [0]);
   break;


 }

}


void draw_sidekicks(BITMAP *bmp, int max_x, int max_y, int play, int a)
{
 int i, x, y, x1, x2,x3, y1, y2, y3;
 float radangle;

 for (i = 0; i < actor[a].sidekicks; i ++)
 {
  if (actor[a].sidekick_x [i] + 2000 < actor[player[play].actor_controlled].x - (max_x / 2) * GRAIN
      || actor[a].sidekick_x [i] - 2000 > actor[player[play].actor_controlled].x + (max_x / 2) * GRAIN
      || actor[a].sidekick_y [i] + 2000 < actor[player[play].actor_controlled].y - (max_y / 2) * GRAIN
      || actor[a].sidekick_y [i] - 2000 > actor[player[play].actor_controlled].y + (max_y / 2) * GRAIN)
      continue;
      
  x = ((actor[a].sidekick_x [i] / GRAIN) - (actor[player[play].actor_controlled].x / GRAIN)) + max_x / 2;
  y = ((actor[a].sidekick_y [i] / GRAIN) - (actor[player[play].actor_controlled].y / GRAIN)) + max_y / 2;

//  circle(bmp, x + max_x / 2, y + max_y / 2, 3, COLOUR_GREEN7);
   radangle = angle_to_radians(actor[a].sidekick_angle [i]);
   x1 = (x) + ((float) cos(radangle + PI) * 3);
   y1 = (y) + ((float) sin(radangle + PI) * 3);
   x2 = (x) + ((float) cos(radangle + PI + (PI * 2) / 3) * 4);
   y2 = (y) + ((float) sin(radangle + PI + (PI * 2) / 3) * 4);
   x3 = (x) + ((float) cos(radangle + PI - (PI * 2) / 3) * 4);
   y3 = (y) + ((float) sin(radangle + PI - (PI * 2) / 3) * 4);
   triangle(bmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
   line(bmp, x1, y1, x2, y2, COLOUR_GREY4);
   line(bmp, x1, y1, x3, y3, COLOUR_GREY4);
   line(bmp, x2, y2, x3, y3, COLOUR_GREY4);

 }


}





void draw_enemies(BITMAP *bmp, int max_x, int max_y, int play)
{

 int e, x, y;

 for (e = 0; e < NO_ENEMIES; e ++)
 {
  if (enemy[e].type == ENEMY_NONE)
   continue;
//  x = (enemy[e].x - actor[player[play].actor_controlled].x);// / GRAIN;
//  y = (enemy[e].y - actor[player[play].actor_controlled].y);// / GRAIN;
/*  if (enemy[e].x < actor[player[play].actor_controlled].x - (max_x / 2) * GRAIN
      || enemy[e].x > actor[player[play].actor_controlled].x + (max_x / 2) * GRAIN
      || enemy[e].y < actor[player[play].actor_controlled].y - (max_y / 2) * GRAIN
      || enemy[e].y > actor[player[play].actor_controlled].y + (max_y / 2) * GRAIN)
      continue;*/
  if (enemy[e].x + enemy[e].edge_radius + 9000 < actor[player[play].actor_controlled].x - (max_x / 2) * GRAIN
      || enemy[e].x - enemy[e].edge_radius - 9000 > actor[player[play].actor_controlled].x + (max_x / 2) * GRAIN
      || enemy[e].y + enemy[e].edge_radius + 9000 < actor[player[play].actor_controlled].y - (max_y / 2) * GRAIN
      || enemy[e].y - enemy[e].edge_radius - 9000 > actor[player[play].actor_controlled].y + (max_y / 2) * GRAIN)
      continue;
      
  x = ((enemy[e].x / GRAIN) - (actor[player[play].actor_controlled].x / GRAIN));// / GRAIN;
  y = ((enemy[e].y / GRAIN) - (actor[player[play].actor_controlled].y / GRAIN));// / GRAIN;

  draw_an_enemy(bmp, e, (x) + (max_x / 2), (y) + (max_y / 2));

  if (actor[player[play].actor_controlled].lock == e)
  {
   set_lock_x [play] = x + (max_x / 2);
   set_lock_y [play] = y + (max_y / 2);
   set_lock_radius [play] = enemy[e].radius / GRAIN + 7;
  }
  if (actor[player[play].actor_controlled].turret_lock == e)
  {
   set_turret_lock_x [play] = x + (max_x / 2);
   set_turret_lock_y [play] = y + (max_y / 2);
   set_turret_lock_radius [play] = enemy[e].radius / GRAIN + 7;
  }
  
 }

}

void draw_lock(BITMAP *bmp, int dr, int x, int y, int rad)
{
// if (enemy[dr].type == ENEMY_NONE)
//  return;
 int lock_colour = COLOUR_YELLOW8 - (counter / 2) % 4;

 int angle = (counter * 8) % 256;
 arc(bmp, x, y, itofix(angle), itofix(angle + 64), rad, lock_colour);
 arc(bmp, x, y, itofix(angle + 128), itofix(angle + 192), rad, lock_colour);
 float fangle = angle_to_radians(ANGLE_FULL - (( - counter) * 8) % ANGLE_FULL);
 int x1 = x + cos(fangle) * (rad + 1);
 int x2 = x + cos(fangle) * (rad + 7);
 int y1 = y + sin(fangle) * (rad + 1);
 int y2 = y + sin(fangle) * (rad + 7);
 pline(bmp, x1, y1, x2, y2, lock_colour);
 x1 = x + cos(fangle + PI / 2) * (rad + 1);
 x2 = x + cos(fangle + PI / 2) * (rad + 7);
 y1 = y + sin(fangle + PI / 2) * (rad + 1);
 y2 = y + sin(fangle + PI / 2) * (rad + 7);
 pline(bmp, x1, y1, x2, y2, lock_colour);
 x1 = x + cos(fangle + PI) * (rad + 1);
 x2 = x + cos(fangle + PI) * (rad + 7);
 y1 = y + sin(fangle + PI) * (rad + 1);
 y2 = y + sin(fangle + PI) * (rad + 7);
 pline(bmp, x1, y1, x2, y2, lock_colour);
 x1 = x + cos(fangle - PI / 2) * (rad + 1);
 x2 = x + cos(fangle - PI / 2) * (rad + 7);
 y1 = y + sin(fangle - PI / 2) * (rad + 1);
 y2 = y + sin(fangle - PI / 2) * (rad + 7);
 pline(bmp, x1, y1, x2, y2, lock_colour);
 x1 = x + cos(fangle + PI / 4) * (rad + 1);
 x2 = x + cos(fangle + PI / 4) * (rad + 4);
 y1 = y + sin(fangle + PI / 4) * (rad + 1);
 y2 = y + sin(fangle + PI / 4) * (rad + 4);
 pline(bmp, x1, y1, x2, y2, lock_colour);
 x1 = x + cos(fangle - PI / 4) * (rad + 1);
 x2 = x + cos(fangle - PI / 4) * (rad + 4);
 y1 = y + sin(fangle - PI / 4) * (rad + 1);
 y2 = y + sin(fangle - PI / 4) * (rad + 4);
 pline(bmp, x1, y1, x2, y2, lock_colour);
 x1 = x + cos(fangle + PI - PI / 4) * (rad + 1);
 x2 = x + cos(fangle + PI - PI / 4) * (rad + 4);
 y1 = y + sin(fangle + PI - PI / 4) * (rad + 1);
 y2 = y + sin(fangle + PI - PI / 4) * (rad + 4);
 pline(bmp, x1, y1, x2, y2, lock_colour);
 x1 = x + cos(fangle + PI + PI / 4) * (rad + 1);
 x2 = x + cos(fangle + PI + PI / 4) * (rad + 4);
 y1 = y + sin(fangle + PI + PI / 4) * (rad + 1);
 y2 = y + sin(fangle + PI + PI / 4) * (rad + 4);
 pline(bmp, x1, y1, x2, y2, lock_colour);
}


void draw_turret_lock(BITMAP *bmp, int dr, int x, int y, int rad)
{
// float fangle = angle_to_radians(ANGLE_FULL - (( - counter) * 8) % ANGLE_FULL);
 int col = COLOUR_YELLOW8 - (counter / 2) % 4;
 float fangle = angle_to_radians((counter * 6) % ANGLE_FULL);
 int x1 = x + cos(fangle + PI / 8) * (rad + 1);
 int y1 = y + sin(fangle + PI / 8) * (rad + 1);
 int x2 = x + cos(fangle - PI / 8) * (rad + 1);
 int y2 = y + sin(fangle - PI / 8) * (rad + 1);
 pline(bmp, x1, y1, x2, y2, col);
 x1 = x + cos(fangle + PI / 2 + PI / 8) * (rad + 1);
 y1 = y + sin(fangle + PI / 2 + PI / 8) * (rad + 1);
 x2 = x + cos(fangle + PI / 2 - PI / 8) * (rad + 1);
 y2 = y + sin(fangle + PI / 2 - PI / 8) * (rad + 1);
 pline(bmp, x1, y1, x2, y2, col);
 x1 = x + cos(fangle + PI + PI / 8) * (rad + 1);
 y1 = y + sin(fangle + PI + PI / 8) * (rad + 1);
 x2 = x + cos(fangle + PI - PI / 8) * (rad + 1);
 y2 = y + sin(fangle + PI - PI / 8) * (rad + 1);
 pline(bmp, x1, y1, x2, y2, col);
 x1 = x + cos(fangle - PI / 2 + PI / 8) * (rad + 1);
 y1 = y + sin(fangle - PI / 2 + PI / 8) * (rad + 1);
 x2 = x + cos(fangle - PI / 2 - PI / 8) * (rad + 1);
 y2 = y + sin(fangle - PI / 2 - PI / 8) * (rad + 1);
 pline(bmp, x1, y1, x2, y2, col);

}



void draw_an_enemy(BITMAP *bmp, int dr, int x, int y)
{
// x += 500;
// y += 500;

// int xa, ya;
 int col1, col2, col3, col4, i, dy, rad, count, thing;

 int x1,y1,x2,y2,x3,y3;

 float radangle;

 int rle_to_draw;

 col1 = enemy[dr].colours [1];
 col2 = enemy[dr].colours [0];
 if (enemy[dr].hurt_pulse > 0)
 {
  col1 = enemy[dr].hurt_pulse_colour2 + enemy[dr].hurt_pulse;
  col2 = enemy[dr].hurt_pulse_colour1 + enemy[dr].hurt_pulse / 2;
  if (enemy[dr].hurt_pulse >= 6)
  {
   col1 = COLOUR_WHITE;
   col2 = COLOUR_WHITE;
  }
 }
#ifdef DEBUG_ENEMY_SIZE
 int store_x = x;
 int store_y = y;
#endif
 
 switch(enemy[dr].type)
 {
  default:
   textprintf(bmp, font, x,y, COLOUR_WHITE, "ERROR: %i %i", dr, enemy[dr].type);
   break;
  case ENEMY_CRAWLER:
   draw_crawler_legs(bmp, x, y, dr, col2 - 2);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_CRAWLER2:
   draw_crawler_legs(bmp, x, y, dr, col2 - 2);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   rle_to_draw = 0;
   if (enemy[dr].attribute [ATTRIB_CRAWLER_AXIS] == 0)
   {
    if (enemy[dr].attribute [ATTRIB_CRAWLER_DIRECTION_X] == -1)
     draw_rle_sprite(bmp, enemy2_rle [1], x - 11, y - 12);
      else
       draw_rle_sprite(bmp, enemy2_rle [2], x - 10, y - 12);
   }
    else
     draw_rle_sprite(bmp, enemy2_rle [0], x - 12, y - 9);
   break;
  case ENEMY_CRAWLER3:
   draw_crawler_legs(bmp, x, y, dr, col2 - 2);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   if (enemy[dr].attribute [ATTRIB_CRAWLER_AXIS] == 0)
   {
    if (enemy[dr].attribute [ATTRIB_CRAWLER_DIRECTION_X] == -1)
     draw_rle_sprite(bmp, enemy2_rle [4], x - 11, y - 11);
      else
       draw_rle_sprite(bmp, enemy2_rle [5], x - 9, y - 11);
   }
    else
     draw_rle_sprite(bmp, enemy2_rle [3], x - 11, y - 9);
   break;
/*  case ENEMY_CRAWLER4:
   draw_crawler_legs(bmp, x / GRAIN, y / GRAIN, dr, col2 - 2);
   circlefill(bmp, (x) / GRAIN, y / GRAIN, 7, col1);
   circle(bmp, (x) / GRAIN, y / GRAIN, 7, col2);
   if (enemy[dr].attribute [ATTRIB_CRAWLER_AXIS] == 0)
   {
    if (enemy[dr].attribute [ATTRIB_CRAWLER_DIRECTION_X] == -1)
     draw_rle_sprite(bmp, enemy2_rle [7], (x/GRAIN) - 11, (y/GRAIN) - 10);
      else
       draw_rle_sprite(bmp, enemy2_rle [8], (x/GRAIN) - 10, (y/GRAIN) - 12);
   }
    else
     draw_rle_sprite(bmp, enemy2_rle [6], (x/GRAIN) - 8, (y/GRAIN) - 13);
   break;*/
  case ENEMY_FALLER:
   col3 = enemy[dr].colours [1];
   col4 = enemy[dr].colours [0] - 2;
   for (i = enemy[dr].attribute [ATTRIB_FALLER_SECTIONS] - 1; i >= 0; i --)
   {
    dy = y - (8 * i) - 7;
    circlefill(bmp, (x), dy, 5, col3);
    circle(bmp, (x), dy, 5, col4);
   }
  case ENEMY_BOUNCER:
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_BOUNCER2:
   col4 = enemy[dr].colours [0] - 2;
   x1 = x + cos(angle_to_radians(enemy[dr].angle)) * 12;
   y1 = y + sin(angle_to_radians(enemy[dr].angle)) * 12;
   x2 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF)) * 12;
   y2 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF)) * 12;
   pline(bmp, x1, y1, x2, y2, col4);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_BOUNCER3:
   col4 = enemy[dr].colours [0] - 2;
   x1 = x + cos(angle_to_radians(enemy[dr].angle)) * 12;
   y1 = y + sin(angle_to_radians(enemy[dr].angle)) * 12;
   x2 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF)) * 12;
   y2 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF)) * 12;
   pline(bmp, x1, y1, x2, y2, col2);
   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_QUARTER)) * 13;
   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_QUARTER)) * 13;
   x2 = x + cos(angle_to_radians(enemy[dr].angle - ANGLE_QUARTER)) * 13;
   y2 = y + sin(angle_to_radians(enemy[dr].angle - ANGLE_QUARTER)) * 13;
   pline(bmp, x1, y1, x2, y2, col4);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_BOUNCER4:
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   radangle = angle_to_radians(enemy[dr].attribute [ATTRIB_BOUNCER_ANGLE2]);
   x = (x) + cos(radangle) * (7);
   y = (y) + sin(radangle) * (7);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_BOUNCER5:
   col4 = enemy[dr].colours [0] - 2;
   radangle = enemy[dr].angle;
   x1 = x + cos(angle_to_radians(radangle)) * 12;
   y1 = y + sin(angle_to_radians(radangle)) * 12;
   x2 = x + cos(angle_to_radians(radangle + ANGLE_HALF)) * 12;
   y2 = y + sin(angle_to_radians(radangle + ANGLE_HALF)) * 12;
   pline(bmp, x1, y1, x2, y2, col4);
   x1 = x + cos(angle_to_radians(radangle + ANGLE_QUARTER)) * 13;
   y1 = y + sin(angle_to_radians(radangle + ANGLE_QUARTER)) * 13;
   x2 = x + cos(angle_to_radians(radangle - ANGLE_QUARTER)) * 13;
   y2 = y + sin(angle_to_radians(radangle - ANGLE_QUARTER)) * 13;
   pline(bmp, x1, y1, x2, y2, col4);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   x1 = x + cos(angle_to_radians(ANGLE_FULL - radangle + ANGLE_QUARTER)) * 13;
   y1 = y + sin(angle_to_radians(ANGLE_FULL - radangle + ANGLE_QUARTER)) * 13;
   x2 = x + cos(angle_to_radians(ANGLE_FULL - radangle - ANGLE_QUARTER)) * 13;
   y2 = y + sin(angle_to_radians(ANGLE_FULL - radangle - ANGLE_QUARTER)) * 13;
   pline(bmp, x1, y1, x2, y2, col2);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
/*   radangle = angle_to_radians(enemy[dr].angle);
   x = (x) + cos(radangle) * (7);
   y = (y) + sin(radangle) * (7);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);*/
   break;
  case ENEMY_FIGHTER:
  case ENEMY_DEAD_FIGHTER:
  case ENEMY_DEAD_FIGHTER2:
  case ENEMY_DEAD_FIGHTER3:
  case ENEMY_FIGHTER2:
  case ENEMY_FIGHTER3:
   circlefill(bmp, (x), y, 8, col1);
   circle(bmp, (x), y, 8, col2);
   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH)) * 6;
   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH)) * 6;
   circlefill(bmp, (x1), y1, 3, col1);
   circle(bmp, (x1), y1, 3, col2);
   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF + ANGLE_1_EIGHTH)) * 6;
   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF + ANGLE_1_EIGHTH)) * 6;
   circlefill(bmp, (x1), y1, 3, col1);
   circle(bmp, (x1), y1, 3, col2);
   break;
  case ENEMY_STINGER:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_STINGER], (x) - 17, (y) - 11);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   radangle = angle_to_radians(enemy[dr].angle);
   x = (x) + cos(radangle) * (3);
   y = (y) + sin(radangle) * (3);
//   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, (x), y, 2, col2);
   break;
  case ENEMY_STINGER2:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_STINGER2], (x) - 24, (y) - 18);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   radangle = angle_to_radians(enemy[dr].angle);
   x = (x) + cos(radangle) * (3);
   y = (y) + sin(radangle) * (3);
//   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, (x), y, 2, col2);
   break;
  case ENEMY_STINGER3:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_STINGER3], (x) - 14, (y) - 15);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   radangle = angle_to_radians(enemy[dr].angle);
   x = (x) + cos(radangle) * (3);
   y = (y) + sin(radangle) * (3);
//   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, (x), y, 2, col2);
   break;
  case ENEMY_HURLER:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_HURLER], (x) - 21, (y) - 9);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
//   radangle = angle_to_radians(enemy[dr].angle);
//   x = (x) + cos(radangle) * (3);
//   y = (y) + sin(radangle) * (3);
//   circle(bmp, (x), y, 2, col2);
   break;
  case ENEMY_HURLER2:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_HURLER2], (x) - 24, (y) - 14);
   circlefill(bmp, (x), y, 8, col1);
   circle(bmp, (x), y, 8, col2);
//   radangle = angle_to_radians(enemy[dr].angle);
//   x = (x) + cos(radangle) * (3);
//   y = (y) + sin(radangle) * (3);
//   circle(bmp, (x), y, 2, col2);
   break;
  case ENEMY_CURVER:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_CURVER], (x) - 19, (y) - 12);
   circlefill(bmp, (x), y, 9, col1);
   circle(bmp, (x), y, 9, col2);
   break;
  case ENEMY_BLASTER:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_BLASTER], (x) - 24, (y) - 25);
   circlefill(bmp, (x), y, 8, col1);
   circle(bmp, (x), y, 8, col2);
   break;
  case ENEMY_BLASTER_TURRET:
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
//   radangle = angle_to_radians(enemy[dr].angle);
//   x = (x) + cos(radangle) * (3);
//   y = (y) + sin(radangle) * (3);
//   circle(bmp, (x), y, 2, col2);
//   arc(bmp, x, y, itofix((enemy[dr].angle - ANGLE_1_EIGHTH) / 4), itofix((enemy[dr].angle + ANGLE_1_EIGHTH) / 4), 4, COLOUR_YELLOW8);
   radangle = angle_to_radians(enemy[dr].angle);
   x = (x) + cos(radangle) * (3);
   y = (y) + sin(radangle) * (3);
//   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, (x), y, 2, col2);
   break;
  case ENEMY_BOSS1:
   draw_rle_sprite(bmp, enemy3_rle [RLE_ENEMY3_BOSS1], (x) - 87, (y) - 88);
   circlefill(bmp, (x), y, 27, col1);
   circle(bmp, (x), y, 27, col2);
   break;
  case ENEMY_BOSS1_TURRET1:
   circlefill(bmp, (x), y, 8, col1);
   circle(bmp, (x), y, 8, col2);
   radangle = angle_to_radians(enemy[dr].angle);
   x = (x) + cos(radangle) * (7);
   y = (y) + sin(radangle) * (7);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_BOSS1_TURRET2:
   circlefill(bmp, (x), y, 11, col1);
   circle(bmp, (x), y, 11, col2);
   radangle = angle_to_radians(enemy[dr].angle);
   x = (x) + cos(radangle) * (10);
   y = (y) + sin(radangle) * (10);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_BOSS2:
   col3 = enemy[dr].colours [1];
   col4 = enemy[dr].colours [0] - 2;
   circlefill(bmp, (x), y, 90, col3);
   circle(bmp, (x), y, 90, col4);
   x1 = cos(angle_to_radians(enemy[dr].angle)) * (90);
   y1 = sin(angle_to_radians(enemy[dr].angle)) * (90);
   line(bmp, x + x1, y + y1, x - x1, y - y1, col4);
   x1 = cos(angle_to_radians(enemy[dr].angle + ANGLE_QUARTER)) * (90);
   y1 = sin(angle_to_radians(enemy[dr].angle + ANGLE_QUARTER)) * (90);
   line(bmp, x + x1, y + y1, x - x1, y - y1, col4);
/*   x1 = cos(angle_to_radians(enemy[dr].angle + ANGLE_1_EIGHTH)) * (90);
   y1 = sin(angle_to_radians(enemy[dr].angle + ANGLE_1_EIGHTH)) * (90);
   pline(bmp, x + x1, y + y1, x - x1, y - y1, col4);
   x1 = cos(angle_to_radians(enemy[dr].angle + ANGLE_QUARTER + ANGLE_1_EIGHTH)) * (90);
   y1 = sin(angle_to_radians(enemy[dr].angle + ANGLE_QUARTER + ANGLE_1_EIGHTH)) * (90);
   pline(bmp, x + x1, y + y1, x - x1, y - y1, col4);*/
/*   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_QUARTER)) * (90);
   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_QUARTER)) * (90);
   circlefill(bmp, x1, y1, 20, col3);
   circle(bmp, x1, y1, 20, col4);
   x1 = x + cos(angle_to_radians(enemy[dr].angle - ANGLE_QUARTER)) * (90);
   y1 = y + sin(angle_to_radians(enemy[dr].angle - ANGLE_QUARTER)) * (90);
   circlefill(bmp, x1, y1, 20, col3);
   circle(bmp, x1, y1, 20, col4);
   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF)) * (90);
   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF)) * (90);
   circlefill(bmp, x1, y1, 20, col3);
   circle(bmp, x1, y1, 20, col4);
   x1 = x + cos(angle_to_radians(enemy[dr].angle)) * (90);
   y1 = y + sin(angle_to_radians(enemy[dr].angle)) * (90);
   circlefill(bmp, x1, y1, 20, col3);
   circle(bmp, x1, y1, 20, col4);*/

   circlefill(bmp, (x), y, 27, col1);
   circle(bmp, (x), y, 27, col2);
   break;
  case ENEMY_BOSS2_TURRET:
   if (enemy[dr].recycle < 20 && enemy[dr].recycle % 4 < 2)// && enemy[dr].burst_fire <= 0)
    col1 += 3;
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   break;
  case ENEMY_BOSS3:
   draw_rle_sprite(bmp, enemy3_rle [RLE_ENEMY3_BOSS3], (x) - 77, (y) - 70);
   circlefill(bmp, (x), y, 23, col1);
   circle(bmp, (x), y, 23, col2);
   break;
  case ENEMY_BOSS3_TURRET1:
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   break;
  case ENEMY_BOSS3_TURRET2:
   circlefill(bmp, (x), y, 18, col1);
   circle(bmp, (x), y, 18, col2);
   break;
  case ENEMY_BOSS4:
   draw_rle_sprite(bmp, enemy3_rle [RLE_ENEMY3_BOSS4], (x) - 111, (y) - 75);
   circlefill(bmp, (x), y, 27, col1);
   circle(bmp, (x), y, 27, col2);
   break;
  case ENEMY_BOSS4_TURRET:
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   break;
  case ENEMY_FIGHTER4:
  case ENEMY_DEAD_FIGHTER4:
   col3 = enemy[dr].colours [1] + 1;
   col4 = enemy[dr].colours [0] - 1;
   x1 = (x) - cos(angle_to_radians(enemy[dr].angle)) * (10);
   y1 = (y) - sin(angle_to_radians(enemy[dr].angle)) * (10);
   circlefill(bmp, (x1), y1, 15, col3);
   circle(bmp, (x1), y1, 15, col4);
   circlefill(bmp, (x), y, 16, col1);
   circle(bmp, (x), y, 16, col2);
   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF - ANGLE_1_SIXTEENTH)) * 25;
   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF - ANGLE_1_SIXTEENTH)) * 25;
   circlefill(bmp, (x1), y1, 3, col1);
   circle(bmp, (x1), y1, 3, col2);
   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF + ANGLE_1_SIXTEENTH)) * 25;
   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF + ANGLE_1_SIXTEENTH)) * 25;
   circlefill(bmp, (x1), y1, 3, col1);
   circle(bmp, (x1), y1, 3, col2);
   break;
  case ENEMY_FIGHTER4_TURRET:
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_SWERVER:
   col3 = enemy[dr].colours [1];
   col4 = enemy[dr].colours [0] - 2;
   radangle = angle_to_radians(enemy[dr].angle);
   x1 = (x) + cos(radangle) * (13);
   y1 = (y) + sin(radangle) * (13);
   x2 = (x) + cos(radangle + (PI * 2) / 3) * 11;
   y2 = (y) + sin(radangle + (PI * 2) / 3) * 11;
   x3 = (x) + cos(radangle - (PI * 2) / 3) * 11;
   y3 = (y) + sin(radangle - (PI * 2) / 3) * 11;
   triangle(bmp, x1, y1, x2, y2, x3, y3, col3);
   line(bmp, x1, y1, x2, y2, col4);
   line(bmp, x1, y1, x3, y3, col4);
   line(bmp, x2, y2, x3, y3, col4);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_SWERVER2:
   col3 = enemy[dr].colours [1];
   col4 = enemy[dr].colours [0] - 2;
   radangle = angle_to_radians(enemy[dr].angle);
   x1 = (x) + cos(radangle + PI) * (11);
   y1 = (y) + sin(radangle + PI) * (11);
   x2 = (x) + cos(radangle + PI + (PI * 2) / 3) * 14;
   y2 = (y) + sin(radangle + PI + (PI * 2) / 3) * 14;
   x3 = (x) + cos(radangle + PI - (PI * 2) / 3) * 14;
   y3 = (y) + sin(radangle + PI - (PI * 2) / 3) * 14;
   triangle(bmp, x1, y1, x2, y2, x3, y3, col3);
   line(bmp, x1, y1, x2, y2, col4);
   line(bmp, x1, y1, x3, y3, col4);
   line(bmp, x2, y2, x3, y3, col4);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_SWERVER3:
   col3 = enemy[dr].colours [1];
   col4 = enemy[dr].colours [0] - 2;
   radangle = angle_to_radians(enemy[dr].angle);
   x1 = (x) + cos(radangle + PI) * (10);
   y1 = (y) + sin(radangle + PI) * (10);
   x2 = (x) + cos(radangle + PI + (PI * 2) / 3) * 16;
   y2 = (y) + sin(radangle + PI + (PI * 2) / 3) * 16;
   x3 = (x) + cos(radangle + PI - (PI * 2) / 3) * 16;
   y3 = (y) + sin(radangle + PI - (PI * 2) / 3) * 16;
   triangle(bmp, x1, y1, x2, y2, x3, y3, col3);
   line(bmp, x1, y1, x2, y2, col4);
   line(bmp, x1, y1, x3, y3, col4);
   line(bmp, x2, y2, x3, y3, col4);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   radangle = angle_to_radians(enemy[dr].attribute [ATTRIB_SWERVER3_ANGLE2]);
   x = (x) + cos(radangle) * (6);
   y = (y) + sin(radangle) * (6);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_TORPER2:
   col3 = COLOUR_RED3;//enemy[dr].colours [1];
   col4 = COLOUR_RED5;//enemy[dr].colours [0] - 2;
   radangle = angle_to_radians(enemy[dr].angle);
   x1 = (x) + cos(radangle + PI) * (12);
   y1 = (y) + sin(radangle + PI) * (12);
   x2 = (x) + cos(radangle + PI + (PI * 2) / 3) * 17;
   y2 = (y) + sin(radangle + PI + (PI * 2) / 3) * 17;
   x3 = (x) + cos(radangle + PI - (PI * 2) / 3) * 17;
   y3 = (y) + sin(radangle + PI - (PI * 2) / 3) * 17;
   triangle(bmp, x1, y1, x2, y2, x3, y3, col3);
   line(bmp, x1, y1, x2, y2, col4);
   line(bmp, x1, y1, x3, y3, col4);
   line(bmp, x2, y2, x3, y3, col4);
   circlefill(bmp, (x1), y1, 3, col1);
   circle(bmp, (x1), y1, 3, col2);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
//   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF)) * 6;
//   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF)) * 6;
   break;
  case ENEMY_HURLER3:
   col3 = COLOUR_ORANGE2;
   col4 = COLOUR_ORANGE5;
   radangle = angle_to_radians(enemy[dr].angle);
   x1 = (x) + cos(radangle + PI) * (12);
   y1 = (y) + sin(radangle + PI) * (12);
   x2 = (x) + cos(radangle + PI + (PI * 2) / 3) * 21;
   y2 = (y) + sin(radangle + PI + (PI * 2) / 3) * 21;
   x3 = (x) + cos(radangle + PI - (PI * 2) / 3) * 21;
   y3 = (y) + sin(radangle + PI - (PI * 2) / 3) * 21;
   triangle(bmp, x1, y1, x2, y2, x3, y3, col3);
   line(bmp, x1, y1, x2, y2, col4);
   line(bmp, x1, y1, x3, y3, col4);
   line(bmp, x2, y2, x3, y3, col4);
   x1 = (x) + cos(radangle + PI + (PI * 2) / 3) * 16;
   y1 = (y) + sin(radangle + PI + (PI * 2) / 3) * 16;
   circlefill(bmp, (x1), y1, 3, COLOUR_RED1);
   circle(bmp, (x1), y1, 3, COLOUR_RED6);
   x1 = (x) + cos(radangle + PI - (PI * 2) / 3) * 16;
   y1 = (y) + sin(radangle + PI - (PI * 2) / 3) * 16;
   circlefill(bmp, (x1), y1, 3, COLOUR_RED1);
   circle(bmp, (x1), y1, 3, COLOUR_RED6);
   circlefill(bmp, (x), y, 8, col1);
   circle(bmp, (x), y, 8, col2);
//   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF)) * 6;
//   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF)) * 6;
   break;
  case ENEMY_CLOAKER:
   col1 = COLOUR_BLACK;
   col2 = COLOUR_BLACK;
   col3 = COLOUR_BLACK;
   col4 = COLOUR_BLACK;
   if (enemy[dr].attribute [ATTRIB_CLOAKER_PULSE] > 0)
   {
    col1 = enemy[dr].colours [0] + enemy[dr].attribute [ATTRIB_CLOAKER_PULSE] / 12;
    col2 = enemy[dr].colours [1] + enemy[dr].attribute [ATTRIB_CLOAKER_PULSE] / 6;
    if (col1 > enemy[dr].colours [0] + 7)
     col1 = enemy[dr].colours [0] + 7;
    if (col2 > enemy[dr].colours [1] + 7)
     col2 = enemy[dr].colours [1] + 7;
    col3 = enemy[dr].colours [0] + enemy[dr].attribute [ATTRIB_CLOAKER_PULSE] / 6;
    col4 = enemy[dr].colours [1] + enemy[dr].attribute [ATTRIB_CLOAKER_PULSE] / 3;
    if (col3 > enemy[dr].colours [0] + 7)
     col3 = enemy[dr].colours [0] + 7;
    if (col4 > enemy[dr].colours [1] + 7)
     col4 = enemy[dr].colours [1] + 7;
   }
   triangle(bmp, x - 2, y - 2, x - 30, y - 2, x - 2, y - 15, col1);
   pline(bmp, x - 2, y - 2, x - 30, y - 2, col2);
   pline(bmp, x - 2, y - 2, x - 2, y - 15, col2);
   pline(bmp, x - 2, y - 15, x - 30, y - 2, col2);
   triangle(bmp, x + 2, y - 2, x + 30, y - 2, x + 2, y - 15, col1);
   pline(bmp, x + 2, y - 2, x + 30, y - 2, col2);
   pline(bmp, x + 2, y - 2, x + 2, y - 15, col2);
   pline(bmp, x + 2, y - 15, x + 30, y - 2, col2);
   triangle(bmp, x + 2, y + 2, x + 30, y + 2, x + 2, y + 15, col1);
   pline(bmp, x + 2, y + 2, x + 30, y + 2, col2);
   pline(bmp, x + 2, y + 2, x + 2, y + 15, col2);
   pline(bmp, x + 2, y + 15, x + 30, y + 2, col2);
   triangle(bmp, x - 2, y + 2, x - 30, y + 2, x - 2, y + 15, col1);
   pline(bmp, x - 2, y + 2, x - 30, y + 2, col2);
   pline(bmp, x - 2, y + 2, x - 2, y + 15, col2);
   pline(bmp, x - 2, y + 15, x - 30, y + 2, col2);
   circlefill(bmp, (x), y, 9, col3);
   circle(bmp, (x), y, 9, col4);
   break;
  case ENEMY_CLOAKER2:
   col1 = COLOUR_BLACK;
   col2 = COLOUR_BLACK;
   col3 = COLOUR_BLACK;
   col4 = COLOUR_BLACK;
   if (enemy[dr].attribute [ATTRIB_CLOAKER_PULSE] > 0)
   {
    col1 = enemy[dr].colours [0] + enemy[dr].attribute [ATTRIB_CLOAKER_PULSE] / 12;
    col2 = enemy[dr].colours [1] + enemy[dr].attribute [ATTRIB_CLOAKER_PULSE] / 6;
    if (col1 > enemy[dr].colours [0] + 7)
     col1 = enemy[dr].colours [0] + 7;
    if (col2 > enemy[dr].colours [1] + 7)
     col2 = enemy[dr].colours [1] + 7;
    col3 = enemy[dr].colours [0] + enemy[dr].attribute [ATTRIB_CLOAKER_PULSE] / 6;
    col4 = enemy[dr].colours [1] + enemy[dr].attribute [ATTRIB_CLOAKER_PULSE] / 3;
    if (col3 > enemy[dr].colours [0] + 7)
     col3 = enemy[dr].colours [0] + 7;
    if (col4 > enemy[dr].colours [1] + 7)
     col4 = enemy[dr].colours [1] + 7;
   }
   triangle(bmp, x - 2, y - 2, x - 20, y - 2, x - 2, y - 15, col1);
   pline(bmp, x - 2, y - 2, x - 20, y - 2, col2);
   pline(bmp, x - 2, y - 2, x - 2, y - 15, col2);
   pline(bmp, x - 2, y - 15, x - 20, y - 2, col2);
   triangle(bmp, x + 2, y - 2, x + 20, y - 2, x + 2, y - 15, col1);
   pline(bmp, x + 2, y - 2, x + 20, y - 2, col2);
   pline(bmp, x + 2, y - 2, x + 2, y - 15, col2);
   pline(bmp, x + 2, y - 15, x + 20, y - 2, col2);
   triangle(bmp, x + 2, y + 2, x + 20, y + 2, x + 2, y + 15, col1);
   pline(bmp, x + 2, y + 2, x + 20, y + 2, col2);
   pline(bmp, x + 2, y + 2, x + 2, y + 15, col2);
   pline(bmp, x + 2, y + 15, x + 20, y + 2, col2);
   triangle(bmp, x - 2, y + 2, x - 20, y + 2, x - 2, y + 15, col1);
   pline(bmp, x - 2, y + 2, x - 20, y + 2, col2);
   pline(bmp, x - 2, y + 2, x - 2, y + 15, col2);
   pline(bmp, x - 2, y + 15, x - 20, y + 2, col2);
   circlefill(bmp, (x), y, 9, col3);
   circle(bmp, (x), y, 9, col4);
   break;
  case ENEMY_DRIFTER:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_DRIFTER], (x) - 12, (y) - 12);
   circlefill(bmp, (x), y, 9, col1);
   circle(bmp, (x), y, 9, col2);
   break;
  case ENEMY_FLAKKER:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_FLAKKER], (x) - 22, (y) - 18);
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   break;
  case ENEMY_SUPERJELLY:
//   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_SUPERJELLY], (x) - 24, (y) - 24);
   x1 = (enemy[dr].attribute [ATTRIB_JELLY_PULSE] / 4) % 8;
   if (x1 >= 4)
    x1 = 8 - x1;
   draw_sprite(bmp, superjelly_bmp [0], (x) - 10, (y) - 24 - x1);
   draw_sprite_v_flip(bmp, superjelly_bmp [0], (x) - 10, (y) + 4 + x1);
   draw_sprite(bmp, superjelly_bmp [1], (x) + 4 + x1, (y) - 10);
   draw_sprite_h_flip(bmp, superjelly_bmp [1], (x) - 24 - x1, (y) - 10);
   circlefill(bmp, (x), y, 9, col1);
   circle(bmp, (x), y, 9, col2);
   break;
  case ENEMY_HIVE:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_HIVE], (x) - 23, (y) - 23);
   circlefill(bmp, (x), y, 13, col1);
   circle(bmp, (x), y, 13, col2);
   break;

  case ENEMY_STINGER4:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_STINGER4], (x) - 24, (y) - 22);
   circlefill(bmp, (x), y, 9, col1);
   circle(bmp, (x), y, 9, col2);
   radangle = angle_to_radians(enemy[dr].angle);
   x = (x) + cos(radangle) * (6);
   y = (y) + sin(radangle) * (6);
   circle(bmp, (x), y, 2, col2);
   break;
  case ENEMY_FLAKKER2:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_FLAKKER2], (x) - 24, (y) - 17);
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   break;
  case ENEMY_BOLTER:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_BOLTER], (x) - 21, (y) - 21);
   circlefill(bmp, (x), y, 11, col1);
   circle(bmp, (x), y, 11, col2);
   radangle = angle_to_radians(enemy[dr].attribute [ATTRIB_BOLTER_ANGLE2]);
   x = (x) + cos(radangle) * (10);
   y = (y) + sin(radangle) * (10);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_BOLTER2:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_BOLTER2], (x) - 21, (y) - 21);
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   radangle = angle_to_radians(enemy[dr].attribute [ATTRIB_BOLTER_ANGLE2]);
   x = (x) + cos(radangle) * (11);
   y = (y) + sin(radangle) * (11);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_TORPER:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_TORPER1_1 + (enemy[dr].attribute [ATTRIB_TORPER_SPRITE] / 4) % 4], (x) - 23, (y) - 17);
   circlefill(bmp, (x), y, 9, col1);
   circle(bmp, (x), y, 9, col2);
   break;

  case ENEMY_BEE:
//   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_HIVE], (x) - 23, (y) - 23);
   thing = grand(ANGLE_QUARTER);
   col4 = COLOUR_GREY4;
   radangle = angle_to_radians(enemy[dr].angle);
   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF - thing)) * 9;
   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF - thing)) * 9;
   line(bmp, x, y, x1, y1, col4);
   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF + thing)) * 9;
   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF + thing)) * 9;
   line(bmp, x, y, x1, y1, col4);
   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF)) * 4;
   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF)) * 4;
   col3 = enemy[dr].colours [1];
   col4 = enemy[dr].colours [0] - 2;
   circlefill(bmp, (x1), y1, 3, col3);
   circle(bmp, (x1), y1, 3, col4);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_DRIFTER3:
   col3 = COLOUR_BLUE3; //enemy[dr].colours [1];
   col4 = COLOUR_BLUE6; //enemy[dr].colours [0] - 2;
   thing = ANGLE_FULL - enemy[dr].attribute [ATTRIB_DRIFTER_LUMP_ANGLE];
   x1 = (x) - cos(angle_to_radians(thing)) * (10);
   y1 = (y) - sin(angle_to_radians(thing)) * (10);
   circlefill(bmp, (x1), y1, 3, col3);
   circle(bmp, (x1), y1, 3, col4);
   x1 = (x) - cos(angle_to_radians(thing + ANGLE_HALF)) * (10);
   y1 = (y) - sin(angle_to_radians(thing + ANGLE_HALF)) * (10);
   circlefill(bmp, (x1), y1, 3, col3);
   circle(bmp, (x1), y1, 3, col4);
// intentional fall-through
  case ENEMY_DRIFTER2:
   if (enemy[dr].type == ENEMY_DRIFTER2)
   {
     col3 = COLOUR_ORANGE2; //enemy[dr].colours [1];
     col4 = COLOUR_ORANGE6; //enemy[dr].colours [0] - 2;
   }
   thing = enemy[dr].attribute [ATTRIB_DRIFTER_LUMP_ANGLE];
   x1 = (x) - cos(angle_to_radians(thing)) * (10);
   y1 = (y) - sin(angle_to_radians(thing)) * (10);
   circlefill(bmp, (x1), y1, 3, col3);
   circle(bmp, (x1), y1, 3, col4);
   x1 = (x) - cos(angle_to_radians(thing + ANGLE_QUARTER)) * (10);
   y1 = (y) - sin(angle_to_radians(thing + ANGLE_QUARTER)) * (10);
   circlefill(bmp, (x1), y1, 3, col3);
   circle(bmp, (x1), y1, 3, col4);
   x1 = (x) - cos(angle_to_radians(thing - ANGLE_QUARTER)) * (10);
   y1 = (y) - sin(angle_to_radians(thing - ANGLE_QUARTER)) * (10);
   circlefill(bmp, (x1), y1, 3, col3);
   circle(bmp, (x1), y1, 3, col4);
   x1 = (x) - cos(angle_to_radians(thing + ANGLE_HALF)) * (10);
   y1 = (y) - sin(angle_to_radians(thing + ANGLE_HALF)) * (10);
   circlefill(bmp, (x1), y1, 3, col3);
   circle(bmp, (x1), y1, 3, col4);
   circlefill(bmp, (x), y, 9, col1);
   circle(bmp, (x), y, 9, col2);
   break;
  case ENEMY_JELLY:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_JELLY1 + (enemy[dr].counter / 3) % 6], (x) - 22, (y) - 8); // 11
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   break;
  case ENEMY_MINETHROWER:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_MINETHROWER], (x) - 17, (y) - 17);
   circlefill(bmp, (x), y, 8, col1);
   circle(bmp, (x), y, 8, col2);
   break;
  case ENEMY_MINETHROWER2:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_MINETHROWER2], (x) - 24, (y) - 13);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_DEAD_WANDERER:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_WANDERER], (x) - 17, (y) - 17);
   break;
  case ENEMY_WANDERER:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_WANDERER], (x) - 17, (y) - 17);
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   break;
  case ENEMY_FIREBASE:
   draw_rle_sprite(bmp, enemy3_rle [RLE_ENEMY3_FIREBASE], (x) - 66, (y) - 66);
   circlefill(bmp, (x), y, 18, col1);
   circle(bmp, (x), y, 18, col2);
   break;
  case ENEMY_FIREBASE_TURRET:
   circlefill(bmp, (x), y, 8, col1);
   circle(bmp, (x), y, 8, col2);
   radangle = angle_to_radians(enemy[dr].angle);
   x = (x) + cos(radangle) * (7);
   y = (y) + sin(radangle) * (7);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_FIREBASE2:
   draw_rle_sprite(bmp, enemy3_rle [RLE_ENEMY3_FIREBASE2], (x) - 19, (y) - 34);
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   break;
  case ENEMY_FIREBASE2_TURRET1:
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_FIREBASE2_TURRET2:
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   radangle = angle_to_radians(enemy[dr].angle);
   x = (x) + cos(radangle) * (6);
   y = (y) + sin(radangle) * (6);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_FIREBASE3:
   draw_rle_sprite(bmp, enemy3_rle [RLE_ENEMY3_FIREBASE3], (x) - 45, (y) - 46);
   circlefill(bmp, (x), y, 22, col1);
   circle(bmp, (x), y, 22, col2);
   break;
  case ENEMY_FIREBASE3_TURRET:
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
/*   radangle = angle_to_radians(enemy[dr].angle);
   x = (x) + cos(radangle) * (7);
   y = (y) + sin(radangle) * (7);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);*/
   break;
  case ENEMY_SUN:
  case ENEMY_SUN_GREEN:
  case ENEMY_SUN_BLUE:
   thing = COLOUR_ORANGE1;
   rle_to_draw = RLE_ENEMY1_SUN;
   if (enemy[dr].type == ENEMY_SUN_GREEN)
    thing = COLOUR_GREEN1;
   if (enemy[dr].type == ENEMY_SUN_BLUE)
   {
    thing = COLOUR_BLUE1;
    rle_to_draw = RLE_ENEMY1_SUN_WHITE;
   }
   count = enemy[dr].attribute [ATTRIB_SUN_COUNTER] / 100;
   rad = (count / 2) % 16;
   if (rad > 8)
    rad = 16 - rad;
   rad += 12;
   col4 = (count / 2) % 8;
   if (col4 > 4)
    col4 = 8 - col4;
   circlefill(bmp, (x), y, rad, thing + col4);
   circle(bmp, (x), y, rad, thing + col4 + 3);
   draw_rle_sprite(bmp, enemy1_rle [rle_to_draw], (x) - 21, (y) - 21);
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
//   circle(bmp, (x) / GRAIN - 4, y / GRAIN - 4, 3, col2);
//   circle(bmp, (x) / GRAIN + 4, y / GRAIN - 4, 3, col2);
   break;
   
 }
#ifdef DEBUG_ENEMY_SIZE
 circle(bmp, store_x, store_y, eclass[enemy[dr].type].radius / GRAIN, COLOUR_YELLOW2 + counter % 6);
#endif

}

void draw_crawler_legs(BITMAP *bmp, int x, int y, int e, int col)
{

   int i;
   int x2, y2;
   int x3, y3;
   int length = 11;
   float dir = 1;

   float angle = (float) ((PI * (counter % 8)) / 8) / 8;
   float angle2 = angle;

   if (enemy[e].attribute [ATTRIB_CRAWLER_AXIS] == 1)
   {
    angle2 = PI - angle;
//    angle = angle2 - PI / 2 + PI + PI / 2;
    angle = angle2 + PI;
   }
    else
    {
     angle += PI;
     if (enemy[e].attribute [ATTRIB_CRAWLER_DIRECTION_X] == -1)
     {
      angle = -angle; //PI - angle;
      dir = -1;
     }
       else
        angle -= PI;
     angle2 = angle;
    }

   for (i = 0; i < 6; i ++)
   {
/*    if (i == 0)
     length = 9;
    if (i == 1)
     length = 10;
    if (i == 2)
     length = 11;
    if (i == 3)
     length = 10;
    if (i == 4)
     length = 9;
    if (i == 5)
     length = 8;*/
    if (i == 0)
     length = 9;
    if (i == 1)
     length = 10;
    if (i == 2)
     length = 11;
    if (i == 3)
     length = 12;
    if (i == 4)
     length = 11;
    if (i == 5)
     length = 9;
    x2 = x + cos(angle + (float) (i * PI * dir) / 8) * length;
    y2 = y + sin(angle + (float) (i * PI * dir) / 8) * length;
    x3 = x - cos(PI - (angle2 + (float) (i * PI * dir) / 8)) * length;
    y3 = y - sin(PI - (angle2 + (float) (i * PI * dir) / 8)) * length;
    pline(bmp, x, y, x2, y2, col);
    pline(bmp, x, y, x3, y3, col);
   }

}

/*
void draw_clouds(BITMAP *bmp, int max_x, int max_y, int play)
{

 int i, x, y, x2 = 0, y2 = 0;

 for (i = 0; i < NO_CLOUDS; i ++)
 {
  if (cloud[i].type == CLOUD_NONE)
   continue;
  if (cloud[i].x < actor[player[play].actor_controlled].x - (max_x / 2) * GRAIN
      || cloud[i].x > actor[player[play].actor_controlled].x + (max_x / 2) * GRAIN
      || cloud[i].y < actor[player[play].actor_controlled].y - (max_y / 2) * GRAIN
      || cloud[i].y > actor[player[play].actor_controlled].y + (max_y / 2) * GRAIN)
      continue;
  x = (cloud[i].x - actor[player[play].actor_controlled].x);// / GRAIN;
  y = (cloud[i].y - actor[player[play].actor_controlled].y);// / GRAIN;
  x2 = (cloud[i].x2 - actor[player[play].actor_controlled].x);// / GRAIN;
  y2 = (cloud[i].y2 - actor[player[play].actor_controlled].y);// / GRAIN;
  draw_a_cloud(bmp, i, x + (max_x / 2) * GRAIN, y + (max_y / 2) * GRAIN, x2 + (max_x / 2) * GRAIN, y2 + (max_y / 2) * GRAIN);
  
 }

}
*/
void draw_clouds(BITMAP *bmp, int max_x, int max_y, int play)
{

 int i, x, y, x2 = 0, y2 = 0;

 for (i = 0; i < NO_CLOUDS; i ++)
 {
  if (cloud[i].type == CLOUD_NONE)
   continue;
  if (cloud[i].x + 12000 < actor[player[play].actor_controlled].x - (max_x / 2) * GRAIN
      || cloud[i].x - 12000 > actor[player[play].actor_controlled].x + (max_x / 2) * GRAIN
      || cloud[i].y + 12000 < actor[player[play].actor_controlled].y - (max_y / 2) * GRAIN
      || cloud[i].y - 12000 > actor[player[play].actor_controlled].y + (max_y / 2) * GRAIN)
      continue;
  x = (cloud[i].x / GRAIN) - (actor[player[play].actor_controlled].x / GRAIN);
  y = (cloud[i].y / GRAIN)  - (actor[player[play].actor_controlled].y / GRAIN);
  x2 = (cloud[i].x2 / GRAIN)  - (actor[player[play].actor_controlled].x / GRAIN);
  y2 = (cloud[i].y2 / GRAIN)  - (actor[player[play].actor_controlled].y / GRAIN);
  draw_a_cloud(bmp, i, x + (max_x / 2), y + (max_y / 2), x2 + (max_x / 2), y2 + (max_y / 2));
  
 }

}



void draw_a_cloud(BITMAP *bmp, int dr, int x, int y, int x2, int y2)
{
 int xa, ya, xb, yb, cs, c2;

 switch(cloud[dr].type)
 {
  case CLOUD_TRAIL1:
  circlefill(bmp, x, y, cloud[dr].timeout / 20 + 1, COLOUR_RED8);
   break;
  case CLOUD_FADING_LINE:
   if ((cloud[dr].timeout / 2) > 3)
    xa = cloud[dr].colour [3];
     else
      xa = cloud[dr].colour [(cloud[dr].timeout) / 2 - 1];
//   if (cloud[dr].timeout > 4)
//    xa = cloud[dr].colour [0];
//     else
//      xa = cloud[dr].colour [4 - cloud[dr].timeout];
   pline(bmp, x, y, x2, y2, xa);
   break;
  case CLOUD_SHRINKING_CIRCLE:
   cs = cloud[dr].timeout / 100;
   if (cs > 7)
    cs = 7;
   xa = cloud[dr].colour [0] - 7 + cs;
   circle(bmp, x, y, cloud[dr].timeout / 100, xa);
   break;
  case CLOUD_SHRINKING_LINE:
   // colour [0] must be transparent, [1] non-transparent
   cs = cloud[dr].colour [0];
   if (cloud[dr].timeout < 140)
    {
     cs = cloud[dr].colour [1];
     cs -= 6;
     cs += cloud[dr].timeout / 20;
    }
     else
      drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   xa = x + cos(angle_to_radians(cloud[dr].angle)) * (cloud[dr].timeout / 10);
   ya = y + sin(angle_to_radians(cloud[dr].angle)) * (cloud[dr].timeout / 10);
//   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   pline(bmp, x, y, xa, ya, cs);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_LINE_SHADOW:
   cs = cloud[dr].angle + ANGLE_QUARTER;
   c2 = cloud[dr].timeout / 10;
   xa = x + cos(angle_to_radians(cs)) * (c2);
   ya = y + sin(angle_to_radians(cs)) * (c2);
   xb = x - cos(angle_to_radians(cs)) * (c2);
   yb = y - sin(angle_to_radians(cs)) * (c2);
//   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   pline(bmp, xa, ya, xb, yb, cloud[dr].colour [0]);
//   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;

  case CLOUD_SPECK:
   xa = cloud[dr].colour [0];
   if (cloud[dr].timeout < 14)
    xa = cloud[dr].colour [1];
   if (cloud[dr].timeout < 11)
    xa = cloud[dr].colour[2];
   if (cloud[dr].timeout < 8)
    xa = cloud[dr].colour[3];
   if (cloud[dr].timeout < 4)
    xa = cloud[dr].colour[4];
   putpixel(bmp, x, y, xa);
   break;
  case CLOUD_SMALL_TRANS_CIRCLE:
  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
  circlefill(bmp, x, y, cloud[dr].timeout / 100, cloud[dr].colour [0]);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_SMALL_CIRCLE:
  circlefill(bmp, x, y, cloud[dr].timeout / 100, cloud[dr].colour [0]);
   break;
  case CLOUD_MED_TRANS_CIRCLE:
  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
  circlefill(bmp, x, y, cloud[dr].timeout / 70, cloud[dr].colour [0]);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_MED_CIRCLE:
  circlefill(bmp, x, y, cloud[dr].timeout / 70, cloud[dr].colour [0]);
   break;
  case CLOUD_LARGE_TRANS_CIRCLE:
  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
  circlefill(bmp, x, y, cloud[dr].timeout / 40, cloud[dr].colour [0]);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_LARGE_CIRCLE:
  circlefill(bmp, x, y, cloud[dr].timeout / 40, cloud[dr].colour [0]);
   break;
  case CLOUD_SHOCKWAVE:
  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
//  if (cloud[dr].timeout > 80)
//   circle(bmp, x, y, (cloud[dr].status - cloud[dr].timeout) / 10, cloud[dr].colour [4]);
  if (cloud[dr].timeout > 90)
   circle(bmp, x, y, (cloud[dr].status - cloud[dr].timeout) / 10 + 4, cloud[dr].colour [3]);
  if (cloud[dr].timeout > 60)
   circle(bmp, x, y, (cloud[dr].status - cloud[dr].timeout) / 10 + 3, cloud[dr].colour [2]);
  if (cloud[dr].timeout > 30)
   circle(bmp, x, y, (cloud[dr].status - cloud[dr].timeout) / 10 + 2, cloud[dr].colour [1]);
  circle(bmp, x, y, (cloud[dr].status - cloud[dr].timeout) / 10 + 1, cloud[dr].colour [0]);
//  text_mode(COLOUR_RED8);
//  textprintf(bmp, font, x, y, COLOUR_BLUE4, "dr %i to %i ", dr, cloud[dr].timeout);
//  text_mode(-1);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_GROWING_CIRCLE:
   cs = cloud[dr].colour [0];
   c2 = (cloud[dr].timeout - cloud[dr].status) / 10;
   if (c2 < -5)
    c2 = -5;
   cs += c2;
   circle(bmp, x, y, (cloud[dr].status - cloud[dr].timeout) / 10 + 4, cs);
   break;
   
//  case CLOUD_DEAD_WANDERER:
//   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_WANDERER], (x/GRAIN) - 17, (y/GRAIN) - 17);
//   break;

 }

}



void draw_pickups(BITMAP *bmp, int max_x, int max_y, int play)
{

 int i, x, y;

 for (i = 0; i < NO_PICKUPS; i ++)
 {
  if (pickup[i].type == PICKUP_NONE)
   continue;
  if (pickup[i].x + 3000 < actor[player[play].actor_controlled].x - (max_x / 2) * GRAIN
      || pickup[i].x - 3000 > actor[player[play].actor_controlled].x + (max_x / 2) * GRAIN
      || pickup[i].y + 3000 < actor[player[play].actor_controlled].y - (max_y / 2) * GRAIN
      || pickup[i].y - 3000 > actor[player[play].actor_controlled].y + (max_y / 2) * GRAIN)
      continue;
  x = (pickup[i].x / GRAIN) - (actor[player[play].actor_controlled].x / GRAIN);
  y = (pickup[i].y / GRAIN) - (actor[player[play].actor_controlled].y / GRAIN);
  
  draw_a_pickup(bmp, i, x + (max_x / 2), y + (max_y / 2));
  
 }

}


void draw_a_pickup(BITMAP *bmp, int dr, int x, int y)
{
 int x1, y1;

 int col = COLOUR_YELLOW8;

 switch(pickup[dr].type)
 {
  default:
  case PICKUP_UPGRADE3:
   col = COLOUR_PURPLE8;
  case PICKUP_UPGRADE2:
   if (pickup[dr].type == PICKUP_UPGRADE2)
   col = COLOUR_RED8;
  case PICKUP_UPGRADE1:
//  circlefill(bmp, x, y, 4, COLOUR_YELLOW4 + (pickup[dr].counter / 8) % 4);
//  circlefill(bmp, x, y, 1 + (pickup[dr].counter / 4) % 5, COLOUR_YELLOW6 + (pickup[dr].counter / 8) % 3);
  circlefill(bmp, x, y, 2, col - (pickup[dr].counter / 8) % 2);
  x1 = cos(angle_to_radians(pickup[dr].counter * 9)) * 7;
  y1 = sin(angle_to_radians(pickup[dr].counter * 9)) * 7;
//  circlefill(bmp, x + x1, y + y1, 1, COLOUR_YELLOW8 - (pickup[dr].counter / 8) % 3);
//  circlefill(bmp, x - x1, y - y1, 1, COLOUR_YELLOW8 - (pickup[dr].counter / 8) % 3);
  putpixel(bmp, x + x1, y + y1, col - (pickup[dr].counter / 8) % 3);
  putpixel(bmp, x - x1, y - y1, col - (pickup[dr].counter / 8) % 3);
  x1 = cos(angle_to_radians(pickup[dr].counter * 9) - PI / 4) * 7;
  y1 = sin(angle_to_radians(pickup[dr].counter * 9) - PI / 4) * 7;
  putpixel(bmp, x + x1, y + y1, col - 4 - (pickup[dr].counter / 8) % 3);
  putpixel(bmp, x - x1, y - y1, col - 4 - (pickup[dr].counter / 8) % 3);
  x1 = cos(angle_to_radians(pickup[dr].counter * 9) - PI / 8) * 7;
  y1 = sin(angle_to_radians(pickup[dr].counter * 9) - PI / 8) * 7;
  putpixel(bmp, x + x1, y + y1, col - 2 - (pickup[dr].counter / 8) % 3);
  putpixel(bmp, x - x1, y - y1, col - 2 - (pickup[dr].counter / 8) % 3);
   break;
  case PICKUP_REPAIR:
  col = COLOUR_ORANGE8;
  x1 = cos(angle_to_radians(pickup[dr].counter * 9)) * 8;
  y1 = sin(angle_to_radians(pickup[dr].counter * 9)) * 8;
  putpixel(bmp, x + x1, y + y1, col - (pickup[dr].counter / 8) % 3);
  putpixel(bmp, x - x1, y - y1, col - (pickup[dr].counter / 8) % 3);
  x1 = cos(angle_to_radians(pickup[dr].counter * 9) - PI / 4) * 8;
  y1 = sin(angle_to_radians(pickup[dr].counter * 9) - PI / 4) * 8;
  putpixel(bmp, x + x1, y + y1, col - 4 - (pickup[dr].counter / 8) % 3);
  putpixel(bmp, x - x1, y - y1, col - 4 - (pickup[dr].counter / 8) % 3);
  x1 = cos(angle_to_radians(pickup[dr].counter * 9) - PI / 8) * 8;
  y1 = sin(angle_to_radians(pickup[dr].counter * 9) - PI / 8) * 8;
  putpixel(bmp, x + x1, y + y1, col - 2 - (pickup[dr].counter / 8) % 3);
  putpixel(bmp, x - x1, y - y1, col - 2 - (pickup[dr].counter / 8) % 3);

//   circlefill(bmp, x, y, 4 + (pickup[dr].counter / 4) % 3, COLOUR_GREY1);
//   circle(bmp, x, y, 4 + (pickup[dr].counter / 4) % 3, COLOUR_GREY4);
//   circlefill(bmp, x, y, 4 + (pickup[dr].counter / 4) % 3, COLOUR_GREY1);
//   circle(bmp, x, y, 4 + (pickup[dr].counter / 4) % 3, COLOUR_GREY4);
   rectfill(bmp, x - 4, y - 4, x + 4, y + 5, COLOUR_GREY1);
   rect(bmp, x - 4, y - 4, x + 4, y + 5, COLOUR_GREY6);
//   rectfill(bmp, x - sin((float) pickup[dr].counter / 32) * 8, y - sin((float) pickup[dr].counter / 32) * 8, x + sin((float) pickup[dr].counter / 32) * 8, y + sin((float) pickup[dr].counter / 32) * 8, COLOUR_WHITE);
//   rectfill(bmp, x - 4, y - sin((float) pickup[dr].counter / 32) * 8, x + 4, y + sin((float) pickup[dr].counter / 32) * 8, COLOUR_WHITE);
//   rectfill(bmp, x - (8 - sin((float) pickup[dr].counter / 32) * 8), y - 4, x + (8 - sin((float) pickup[dr].counter / 32) * 8), y + 4, COLOUR_WHITE);
   textprintf(bmp, small_font, x - 2, y - 5, col - (pickup[dr].counter / 8) % 4, "R");
   break;
  case PICKUP_SHIP:
   col = COLOUR_BLUE8;
   x1 = cos(angle_to_radians(pickup[dr].counter * 9)) * 8;
   y1 = sin(angle_to_radians(pickup[dr].counter * 9)) * 8;
   putpixel(bmp, x + x1, y + y1, col - (pickup[dr].counter / 8) % 3);
   putpixel(bmp, x - x1, y - y1, col - (pickup[dr].counter / 8) % 3);
   x1 = cos(angle_to_radians(pickup[dr].counter * 9) - PI / 4) * 8;
   y1 = sin(angle_to_radians(pickup[dr].counter * 9) - PI / 4) * 8;
   putpixel(bmp, x + x1, y + y1, col - 4 - (pickup[dr].counter / 8) % 3);
   putpixel(bmp, x - x1, y - y1, col - 4 - (pickup[dr].counter / 8) % 3);
   x1 = cos(angle_to_radians(pickup[dr].counter * 9) - PI / 8) * 8;
   y1 = sin(angle_to_radians(pickup[dr].counter * 9) - PI / 8) * 8;
   putpixel(bmp, x + x1, y + y1, col - 2 - (pickup[dr].counter / 8) % 3);
   putpixel(bmp, x - x1, y - y1, col - 2 - (pickup[dr].counter / 8) % 3);
   rectfill(bmp, x - 4, y - 4, x + 4, y + 5, COLOUR_PURPLE1);
   rect(bmp, x - 4, y - 4, x + 4, y + 5, COLOUR_PURPLE6);
   textprintf(bmp, small_font, x - 2, y - 5, col - (pickup[dr].counter / 8) % 4, "X");
   break;
 }

}


/*
void draw_stars(BITMAP *bmp, int max_x, int max_y, int player)
{
 int i, x, y;

 float star_angle;
 int distance;

 for (i = 0; i < NO_STARS; i ++)
 {
  x = star[i].x [player] / GRAIN;
  if (x == 0)
   continue;
  y = star[i].y [player] / GRAIN;
  x -= 600;
  y -= 600;
  distance = hypot(x, y);
  if (x == 0 || y == 0)
   continue;
//  star_angle = atan((float) ((float) (y - 750) / (float) (x - 750)));
//  star_angle = atan((float) ((float) y / (float) x));
  star_angle = atan2((float) y, (float) x);
//  if (x > y)
//   star_angle = asin(y / x);
//    else
//     star_angle = acos(x / y);
  star_angle -= PI / 2;
  x = cos(star_angle) * distance;// - 750;
  y = sin(star_angle) * distance;// - 750;

//  putpixel(bmp, x, y, star[i].colour [player]);
//  circlefill(bmp, x + max_x / 2, y + max_y / 2, 2, 3);
  putpixel(bmp, x + max_x / 2, y + (max_y / 4) * 3, star[i].colour [player]);
 }

//  circlefill(bmp, max_x / 2, (max_y / 4) * 3, 2, 5);


}
*/

/*
Warning - this function uses a lot of really horrible magic numbers.
*/
/*void draw_grid(BITMAP *bmp, int max_x, int max_y, int play, int colour, int edge_colour, int x_offset)
{
 int i, x, y;

 int no_x = max_x / GRID_WIDTH + 2;
 int no_y = max_y / GRID_WIDTH + 2;

 int ax = actor[player[play].actor_controlled].x;// + inflicteda;
 int ay = actor[player[play].actor_controlled].y;// + inflictede;

 int min_x = 0;
 int min_y = 0;
 int dmax_x = max_x;
 int dmax_y = max_y;

 char highlight = 0;

 if (ax / GRAIN < max_x / 2)
 {
  min_x = (max_x / 2) - ax / GRAIN;
 }
 if (ax / GRAIN > arena.max_x / GRAIN - max_x / 2)
 {
  dmax_x = (max_x / 2) + arena.max_x / GRAIN - ax / GRAIN;
 }
 if (ay / GRAIN < max_y / 2)
 {
  min_y = (max_y / 2) - ay / GRAIN;
 }
 if (ay / GRAIN > arena.max_y / GRAIN - max_y / 2)
 {
  dmax_y = (max_y / 2) + arena.max_y / GRAIN - ay / GRAIN;
 }
 

// float star_angle;
// int distance;

 for (i = 0; i < no_x; i ++)
 {
  if ((i + 1) * GRID_WIDTH + ax / GRAIN > arena.max_x / GRAIN + max_x / 2 + 50)
  {
   highlight = 1;
   if (i * GRID_WIDTH + ax / GRAIN > arena.max_x / GRAIN + max_x / 2 + 50)
    break;
  }
  if (i * GRID_WIDTH <= min_x - 6)
  {
   highlight = 1;
   continue;
  }
  x = i * GRID_WIDTH - ((ax + x_offset) / GRAIN) % GRID_WIDTH;
//  x = i * GRID_WIDTH - (actor[player[play].actor_controlled].y / GRAIN) % GRID_WIDTH;
  if (highlight == 1)
  {
   pline(bmp, x, min_y - 4, x, dmax_y, edge_colour);
   highlight = 0;
  }
   else
    pline(bmp, x, min_y - 4, x, dmax_y, colour);
 }
 
 for (i = 0; i < no_y; i ++)
 {
  if ((i + 1) * GRID_WIDTH + ay / GRAIN > arena.max_y / GRAIN + max_y / 2 + 64)
  {
   highlight = 1;
   if (i * GRID_WIDTH + ay / GRAIN > arena.max_y / GRAIN + max_y / 2 + 64)
    break;
  }
  if (min_y > 0 && i * GRID_WIDTH <= min_y + 9)
  {
   highlight = 1;
   continue;
  }
  y = i * GRID_WIDTH - (ay / GRAIN) % GRID_WIDTH;
//  y = i * GRID_WIDTH + (actor[player[play].actor_controlled].x / GRAIN) % GRID_WIDTH;
  if (highlight == 1)
  {
   pline(bmp, min_x - 4, y - 15, dmax_x, y - 15, edge_colour);
   highlight = 0;
  }
   else
    pline(bmp, min_x - 4, y - 15, dmax_x, y - 15, colour);
 }


}
*/

void draw_grid(BITMAP *bmp, int max_x, int max_y, int play, int colour, int edge_colour, int x_offset)
{
 int i, x, y;

 int no_x = max_x / GRID_WIDTH + 2;
 int no_y = max_y / GRID_WIDTH + 2;

 int ax = actor[player[play].actor_controlled].x;// + inflicteda;
 int ay = actor[player[play].actor_controlled].y;// + inflictede;

 int min_x = 0;
 int min_y = 0;
 int dmax_x = max_x;
 int dmax_y = max_y;

 char highlight = 0;

 if (ax / GRAIN < max_x / 2)
 {
  min_x = (max_x / 2) - ax / GRAIN;
 }
 if (ax / GRAIN > arena.max_x / GRAIN - max_x / 2)
 {
  dmax_x = (max_x / 2) + arena.max_x / GRAIN - ax / GRAIN;
 }
 if (ay / GRAIN < max_y / 2)
 {
  min_y = (max_y / 2) - ay / GRAIN;
 }
 if (ay / GRAIN > arena.max_y / GRAIN - max_y / 2)
 {
  dmax_y = (max_y / 2) + arena.max_y / GRAIN - ay / GRAIN;
 }
 

// float star_angle;
// int distance;

 for (i = 0; i < no_x; i ++)
 {
  if ((i + 1) * GRID_WIDTH + ax / GRAIN > arena.max_x / GRAIN + max_x / 2 + 50)
  {
   highlight = 1;
   if (i * GRID_WIDTH + ax / GRAIN > arena.max_x / GRAIN + max_x / 2 + 50)
    break;
  }
  if (i * GRID_WIDTH <= min_x - 6)
  {
   highlight = 1;
   continue;
  }
  x = i * GRID_WIDTH - ((ax + x_offset) / GRAIN) % GRID_WIDTH;
//  x = i * GRID_WIDTH - (actor[player[play].actor_controlled].y / GRAIN) % GRID_WIDTH;
  if (highlight == 1)
  {
   pline(bmp, x, min_y - 4, x, dmax_y, edge_colour);
   highlight = 0;
  }
   else
    pline(bmp, x, min_y - 4, x, dmax_y, colour);
 }
 
 for (i = 0; i < no_y; i ++)
 {
  if ((i + 1) * GRID_WIDTH + ay / GRAIN > arena.max_y / GRAIN + max_y / 2 + 64)
  {
   highlight = 1;
   if (i * GRID_WIDTH + ay / GRAIN > arena.max_y / GRAIN + max_y / 2 + 64)
    break;
  }
  if (min_y > 0 && i * GRID_WIDTH <= min_y + 9)
  {
   highlight = 1;
   continue;
  }
  y = i * GRID_WIDTH - (ay / GRAIN) % GRID_WIDTH;
//  y = i * GRID_WIDTH + (actor[player[play].actor_controlled].x / GRAIN) % GRID_WIDTH;
  if (highlight == 1)
  {
   pline(bmp, min_x - 4, y - 15, dmax_x, y - 15, edge_colour);
   highlight = 0;
  }
   else
    pline(bmp, min_x - 4, y - 15, dmax_x, y - 15, colour);
 }


}


int pline(BITMAP *bmp, int x1, int y1, int x2, int y2, int colour)
{
 if (x1 < -100 || y1 < -100 || x1 > 750 || y1 > 600
     || x2 < -100 || y2 < -100 || x2 > 750 || y2 > 600)
      return 0;
 line(bmp, x1, y1, x2, y2, colour);
 return 1;
}

void indicate_fps(BITMAP *bmp, int play)
{
// textprintf(bmp, small_font, 20, 110, 5, "Xs %i  Ys %i  TS %f  DS %f  Drag %i",
//  actor[player[play].actor_controlled].x_speed, actor[player[play].actor_controlled].y_speed, hypot(actor[player[play].actor_controlled].x_speed, actor[player[play].actor_controlled].y_speed), hypot(actor[player[play].actor_controlled].x_speed, actor[player[play].actor_controlled].y_speed) * game.drag, actor[player[play].actor_controlled].drag_amount);
// textprintf(bmp, small_font, 20, 60, 5, "Stack %i", stackavail());
// textprintf(bmp, small_font, 20, 90, 5, "Slack %i", slacktime);
// textprintf(bmp, small_font, 20, 100, 5, "Lock %i", actor[0].lock);
//textprintf(bmp, smallfont, 20, 50, 5, "Fps} %i", frames_per_second);
/* textprintf(bmp, font, 20, 50, 5, "Fps} %i", frames_per_second);
 textprintf(bmp, font2, 20, 50, 5, "Fps} %i", frames_per_second);
 textprintf(bmp, font, 20, 80, 5, "Slack} %i", slacktime);
 textprintf(bmp, font, 20, 110, 5, "Xs} %i  Ys} %i  Drag} %i",
  actor[player[play].actor_controlled].x_speed, actor[player[play].actor_controlled].y_speed, actor[player[play].actor_controlled].drag_amount);
 textprintf(bmp, font, 20, 140, 5, "X} %i  Y} %i ",
  actor[player[play].actor_controlled].x, actor[player[play].actor_controlled].y);
 textprintf(bmp, font, 20, 170, 5, "Angle} %i  Wants} %i Att} %i Rec} %i ",
  enemy[0].angle, enemy[0].attribute [ATTRIB_SWERVER_ANGLE], enemy[0].attacking, enemy[0].recycle);
 textprintf(bmp, font, 20, 200, 5, "Lock} %i ", actor[0].lock);*/
/* textprintf(bmp, small_font, 20, 50, 5, "FPS %i", frames_per_second);
 textprintf(bmp, small_font, 20, 60, 5, "SLACK %i", slacktime);
 textprintf(bmp, small_font, 20, 70, 5, "XS %i  YS %i  DRAG %i",
  actor[player[play].actor_controlled].x_speed, actor[player[play].actor_controlled].y_speed, actor[player[play].actor_controlled].drag_amount);
 textprintf(bmp, small_font, 20, 80, 5, "X %i  Y %i ",
  actor[player[play].actor_controlled].x, actor[player[play].actor_controlled].y);
 textprintf(bmp, small_font, 20, 90, 5, "ANGLE %i  WANTS %i ATT %i REC %i ",
  enemy[0].angle, enemy[0].attribute [ATTRIB_SWERVER_ANGLE], enemy[0].attacking, enemy[0].recycle);
 textprintf(bmp, small_font, 20, 100, 5, "LOCK %i ", actor[0].lock);
 textprintf(bmp, small_font, 20, 110, 5, "Angle %i ", actor[0].angle / 16);*/
// textprintf(bmp, font, 20, 160, 5, "inflicteda %i inflictede %i ", inflicteda, inflictede);
// textprintf(bmp, font, 20, 170, 5, "angle %i wants %i mode %i attacking %i ",
//   enemy[0].angle, 0, enemy[0].attribute [ATTRIB_FIGHTER_MODE], enemy[0].attacking);
/* textprintf(bmp, font, 20, 180, 5, "hive %i mode %i attack %i hive at13 %i hive at24 %i xd %i yd %i",
   enemy[1].attribute [ATTRIB_BEE_HIVE], enemy[1].attribute [ATTRIB_BEE_MODE], enemy[1].attacking,
   enemy[enemy[1].attribute [ATTRIB_BEE_HIVE]].attribute [ATTRIB_HIVE_ATTACK13], enemy[enemy[1].attribute [ATTRIB_BEE_HIVE]].attribute [ATTRIB_HIVE_ATTACK24],
   enemy[1].attribute [ATTRIB_BEE_X_DEST], enemy[1].attribute [ATTRIB_BEE_Y_DEST]);*/
}


void level_announcement(void)
{

 clear_bitmap(screen);

 int waiting = 30;

 if (arena.level == 1)
  waiting = 60;

// waiting = 2;

 ticked = 0;

 do
 {
  while(ticked > 0)
  {
   waiting --;
   ticked --;
  };
  if ((waiting / 5) % 2 == 0)
  {
   textprintf_centre(screen, font, 320, 240, COLOUR_GREEN4 + (waiting / 2) % 4, "Get}  Ready}");
   textprintf_centre(screen, font2, 320, 240, COLOUR_YELLOW8 - (waiting / 2) % 4, "Get}  Ready}");
  }
    else
    {
     textprintf_centre(screen, font, 320, 240, COLOUR_BLACK, "Get}  Ready}");
     textprintf_centre(screen, font2, 320, 240, COLOUR_BLACK, "Get}  Ready}");
    }
 textprintf_centre(screen, font, 320, 180, COLOUR_YELLOW4 + (waiting / 3) % 4, "Level} %i", arena.level);
 textprintf_centre(screen, font2, 320, 180, COLOUR_ORANGE8 - (waiting / 3) % 4, "Level} %i", arena.level);

    
 } while (waiting > 0);

 clear_bitmap(screen);

}



void display_messages(BITMAP *bmp, int play, int max_x, int max_y)
{

// return;

 int i;
 int col1, col2;
 int count;
 int count_x;
 int k;
 int y_loc;
 int x_offset;

 FONT *msgfont1;
 FONT *msgfont2;

 for (i = 0; i < 4; i ++)
 {
  if (strlen(messages [play] [i]) == 0)
   continue;
  if (message_counter [play] [i] > 100)
  {
   kill_message(play, i);
   continue;
  } // can be killed later also
  
  message_counter [play] [i] ++;
  count = message_counter [play] [i];
  y_loc = 200 + i * 40;
  x_offset = 0;
  msgfont1 = font;
  msgfont2 = font2;
  
  switch(message_style [play] [i])
  {
   case STYLE_UPGRADE1:
    col1 = COLOUR_BLACK;
    col2 = COLOUR_BLACK;
    break;
   case STYLE_UPGRADE2:
    col1 = COLOUR_GREEN1;
    col2 = COLOUR_GREEN1;
    msgfont1 = font2;
    msgfont2 = font;
    break;
   case STYLE_UPGRADE3:
    col1 = COLOUR_YELLOW1;
    col2 = COLOUR_GREEN1;
    break;
   case STYLE_UPGRADE4:
    col1 = COLOUR_ORANGE1;
    col2 = COLOUR_RED1;
    msgfont1 = font2;
    msgfont2 = font;
    break;
   case STYLE_UPGRADE5:
    col1 = COLOUR_BLUE1;
    col2 = COLOUR_PURPLE1;
    break;
  
 }

  switch(message_style [play] [i])
  {
   default:
   case STYLE_UPGRADE1:
   case STYLE_UPGRADE2:
   case STYLE_UPGRADE3:
   case STYLE_UPGRADE4:
   case STYLE_UPGRADE5:
    x_offset = -30;
    if (message_counter [play] [i] > 60)
    {
     kill_message(play, i);
     continue;
    }
    count_x = count * 15;
    k = 700;
/*    if (max_x < 500)
    {
     k = 350;
     count_x /= 2;
    }
    should set counter according to screen width.
    */
    if (count_x >= 351)
    {
      if (count_x < 400)
      {
       col1 = COLOUR_WHITE;
       col2 = COLOUR_WHITE;
      }
       else
       {
//        col1 += (1500 - count_x) / 165;
//        col2 += (1500 - count_x) / 165;
        col1 += (900 - count_x) / 72;
        col2 += (900 - count_x) / 72;
       }
//      col1 += (count / 3) % 4;
//      col2 += (count / 3) % 4;
//      textprintf_centre(bmp, msgfont1, x_offset + 351, y_loc, col1, messages [play] [i]);
//      textprintf_centre(bmp, msgfont2, x_offset + 351, y_loc, col2, messages [play] [i]);
      textprintf_centre(bmp, msgfont1, x_offset + max_x / 2 + 31, y_loc, col1, messages [play] [i]);
      textprintf_centre(bmp, msgfont2, x_offset + max_x / 2 + 31, y_loc, col2, messages [play] [i]);
    }
     else
     {
      col1 += 1 + (count / 3) % 5;
      col2 += 3 + (count / 3) % 5;
//      col1 += 2;
//      col2 += 2;
//      textprintf_centre(bmp, msgfont1, x_offset + count_x, y_loc, col1, messages [play] [i]);
//      textprintf_centre(bmp, msgfont2, x_offset + 2 + k - count_x, y_loc, col2, messages [play] [i]);
      textprintf_centre(bmp, msgfont1, x_offset + count_x + max_x / 2 - 320, y_loc, col1, messages [play] [i]);
      textprintf_centre(bmp, msgfont2, x_offset + 2 + k - count_x + max_x / 2 - 320, y_loc, col2, messages [play] [i]);
     }
    break;
/*   case STYLE_HURRY:
    col1 = COLOUR_RED1;
    col2 = COLOUR_ORANGE1;
    x_offset = -30;
//    if (counter % 2 == 0)
//     message_counter [play] [i] --; // slows it down
    if (message_counter [play] [i] > 120)
    {
     kill_message(play, i);
     continue;
    }
//    count_x = count * 15;
    count_x = count * 7;
    k = 700;
//    col1 += (900 - count_x) / 72;
//    col2 += (900 - count_x) / 72;
    col1 += 1 + (count / 3) % 5;
    col2 += 3 + (count / 3) % 5;
//    textprintf_centre(bmp, msgfont1, x_offset + count_x + max_x / 2 - 320, y_loc, col1, messages [play] [i]);
    textprintf_centre(bmp, msgfont1, x_offset + 2 + k - count_x + max_x / 2 - 320, y_loc, col1, messages [play] [i]);
    textprintf_centre(bmp, msgfont2, x_offset + 2 + k - count_x + max_x / 2 - 320, y_loc, col2, messages [play] [i]);
    break;*/
  }

 } // end loop
}

void kill_message(int play, int i)
{
// return;

 messages [play] [i] [0] = '\0';
 
}


void init_messages(void)
{
// return;

 int i;

 for (i = 0; i < 4; i ++)
 {
  strcpy(messages [0] [i], "");
  strcpy(messages [1] [i], "");
  message_style [0] [i] = STYLE_NONE;
  message_style [1] [i] = STYLE_NONE;
  message_counter [0] [i] = 0;
  message_counter [1] [i] = 0;
 }

}

// It's okay to send a message to a non-existent player
void send_message(int play, const char *msg, int style)
{

// return;
// return;

 int i;

 for (i = 0; i < 4; i ++)
 {
  if (messages [play] [i] [0] == 0 || i == 3)
   break;
 }
 // slot 3 will be overwritten if there're no gaps

 strcpy(messages [play] [i], msg);
 message_style [play] [i] = style;
 message_counter [play] [i] = 0;
 if (game.users > 1)
 {
  message_counter [play] [i] = 10;
 }

}


void display_quit_query(void)
{

// drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
// rectfill(screen, 140, 150, 500, 280, TRANS_DGREY);
// drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
 
 rect(screen, 140, 150, 500, 280, COLOUR_BLUE2);
 rect(screen, 139, 149, 501, 281, COLOUR_BLUE3);
 rect(screen, 138, 148, 502, 282, COLOUR_BLUE4);
 rect(screen, 137, 147, 503, 283, COLOUR_BLUE5);
 rect(screen, 136, 146, 504, 284, COLOUR_BLUE6);

 textprintf_centre(screen, font, 320, 180, COLOUR_BLUE8, "Really} Quit}?");
 textprintf_centre(screen, font2, 320, 180, COLOUR_GREEN6, "Really} Quit}?");
 textprintf_centre(screen, font, 320, 230, COLOUR_GREEN6, "Y} Or} N}");
 textprintf_centre(screen, font2, 320, 230, COLOUR_BLUE8, "Y} Or} N}");

}

void clear_quit_query(void)
{

// vsync();
// clear_bitmap(screen);
 rectfill(screen, 140, 140, 510, 290, 0);
 
}






