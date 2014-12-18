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
    
File: game.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - main game loop and a bit of miscellany

*/

#include "allegro.h"

#include "config.h"
#include "globvars.h"

#include "stuff.h"

#include "palette.h"

#include "input.h"
#include "bullet.h"
#include "cloud.h"
#include "actor.h"
#include "enemy.h"
#include "actor.h"
#include "levels.h"
#include "display.h"
#include "displ_in.h"
#include "cmds.h"
#include "ships.h"
#include "pickup.h"
#include "sound.h"

void begin_game(void);
void init_structs_etc(void);
void make_arena_hostile(void);
void run_hostile_level();
void quit_query(void);



extern volatile unsigned char ticked;
extern volatile int framecounter;
extern int slacktime;


void game_loop(void)
{
 int playing = 1;
 
 counter = 0;

 do
 {
  if (player[0].link_toggle_delay > 0)
    player[0].link_toggle_delay --;
  if (player[1].link_toggle_delay > 0)
    player[1].link_toggle_delay --;

    
  get_input();
  enact_commands();
  run_actors();
  run_bullets(); // should be between actors and enemies for orbitals to work
  run_clouds();
//  run_stars();
  run_enemies();
  run_pickups();
  play_ambience();
  if (counter % 35 == 0)
   run_level();
  if (arena.game_over > 0)
  {
   arena.game_over --;
   if (arena.game_over == 1)
    playing = 0;
   if (arena.game_over == 120)
    play_sound(WAV_GAME_OVER);
  } else
   {
    if (arena.time_left > 0)
    {
     if (game.type != GAME_DUEL
       || (game.duel_mode != DUEL_10_POINTS && game.duel_mode != DUEL_30_POINTS))
     arena.time_left --;
    }
      else
      {
       if (game.type == GAME_DUEL)
       {
        if (arena.game_over == 0)
        {
         arena.game_over = 132;
         game.duel_winner = -1;
         if (player[0].duel_score > player[1].duel_score)
          game.duel_winner = 0;
         if (player[1].duel_score > player[0].duel_score)
          game.duel_winner = 1;
        }
       }
        else
        {
         if (arena.level_finished == 1)
         {
          if (arena.level == 16)
          {
           arena.level = 17;
           play_soundf(WAV_LEVEL_END, 1500);
           playing = 0;
           break;
          }
          init_level();
          if (actor[0].in_play == 1)
           actor_new_level(0);
          if (actor[1].in_play == 1)
           actor_new_level(1);
          ticked = 0;
         }
         else
         {
          if (arena.hostile == 0)
          {
           make_arena_hostile();
          }
           else
            {
             if (arena.hostile < 10000)
              arena.hostile ++;
             run_hostile_level();
            }
         }
        }
      }
/*    if (arena.time_left == 334)
    {
     send_message(0, "Hurry} Up}!", STYLE_HURRY);
     send_message(1, "Hurry} Up}!", STYLE_HURRY);
    }*/
    if (arena.time_left == 330 && (game.type != GAME_DUEL
       || (game.duel_mode != DUEL_10_POINTS && game.duel_mode != DUEL_30_POINTS)))
     play_soundf(WAV_ALARM, 800);
    
    if (arena.hostile == 10 || arena.hostile == 40 || arena.hostile == 70)
     play_sound(WAV_ALARM);

     
   }
   
  if (ticked == 0)
  {
   run_display();
//   framecounter++;
  }

  slacktime = 0;
  
  do
  {
   slacktime ++;
  } while(ticked == 0);
  ticked --;

  counter ++;

  if (key [KEY_ESC])
   quit_query();
  
 } while(playing == 1);

}

void quit_query(void)
{

 if (arena.game_over > 0)
  return;

 display_quit_query();

 do
 {

  if (key [KEY_Y])
  {
   arena.game_over = 44;
   if (game.type == GAME_DUEL)
   {
    game.duel_winner = -1;
    if (player[0].duel_score > player[1].duel_score)
     game.duel_winner = 0;
    if (player[1].duel_score > player[0].duel_score)
     game.duel_winner = 1;
   }
   clear_quit_query();
   return;
  }

  if (key [KEY_N])
  {
   clear_quit_query();
   return;
  }

  ticked = 0;

  rest(30);
 }
  while(TRUE);
  
}




void make_arena_hostile(void)
{
  explode_all_pickups();
  arena.hostile = 1;
  arena.colour3 = COLOUR_RED1;
//  send_message(0, "Defences} On}-Line}!", STYLE_HURRY);
//  send_message(1, "Defences} On}-Line}!", STYLE_HURRY);
}



void run_hostile_level()
{
  int hostile_attack, i;

  if (counter % 8 != 0 || grand(3) != 0)
   return;

  hostile_attack = grand(5) + arena.hostile / 100;

  if (hostile_attack > 5)
   hostile_attack = 5;

  int x, y, xs, ys;

//  int colours [4] = {COLOUR_WHITE, COLOUR_BLUE6, COLOUR_BLUE4, COLOUR_BLUE2};
  int colours [4] = {COLOUR_BLUE3, COLOUR_BLUE5, COLOUR_BLUE7, COLOUR_WHITE};
//  int colours [4] = {COLOUR_WHITE, COLOUR_BLUE7, COLOUR_BLUE5, COLOUR_BLUE3};

  play_sound(WAV_HOSTILE);

  for (i = 0; i < hostile_attack; i ++)
  {

   if (grand(2) == 0)
   {
    x = 1000;
    xs = 10000;
    if (grand(2) == 0)
    {
     x = arena.max_x - 1000;
     xs = -10000;
    }
    y = grand(arena.max_y - 20000) + 10000;
    y /= GRID_WIDTH * GRAIN;
    y *= GRID_WIDTH * GRAIN;
    y -= 4000;
    ys = 0;
   }
    else
    {
     y = 2000;
     ys = 10000;
     if (grand(2) == 0)
     {
      y = arena.max_y - 2000;
      ys = -10000;
     }
     x = grand(arena.max_x - 20000) + 10000;
     x /= GRID_WIDTH * GRAIN;
     x *= GRID_WIDTH * GRAIN;
     x -= 4000;
     xs = 0;
    }

   create_bullet(BULLET_HOSTILE, x, y,
    xs, ys, OWNER_ENEMY,
    400, 2000, 400, 0,
    0, 0, colours, 3,0,0,0,0,0);

  }

}


void begin_game(void)
{
 int i;

 counter = 0;

 init_structs_etc();

 for (i = 0; i < NO_ACTORS; i ++)
 {
  actor[i].in_play = 0;
 }

 player[0].score = 0;
 player[1].score = 0;

 if (game.type == GAME_SINGLE)
 {
  game.users = 1;

  actor[0].in_play = 0;
  player[game.single_player].actor_controlled = 0;
//  player[0].ships_left = 3;
  game.ships_left = 3;
  actor[0].controller = game.single_player;
  init_actor(0, player[game.single_player].ship); //ACTORTYPE_SHIP);
 }
  else
   {
    game.users = 2;

    actor[0].in_play = 0;
    player[0].actor_controlled = 0;
//    player[0].ships_left = 3;
    actor[0].controller = 0;
    init_actor(0, player[0].ship); //ACTORTYPE_SHIP);
 
    actor[1].in_play = 0;
    player[1].actor_controlled = 1;
//    player[1].ships_left = 3;
    actor[1].controller = 1;
    init_actor(1, player[1].ship); //ACTORTYPE_SHIP);
    game.ships_left = 6;
   }
   
 init_bullets();
 init_clouds();
// init_stars();
 init_enemies();
 prepare_display();

 game.drag = 0.013;//955;//9999;
/*
 arena.max_x = 894000;
 arena.max_y = 895000;
*/

 arena.level = 0;

 init_level();


}

void init_structs_etc(void)
{

 int i, j;

 for (i = 0; i < NO_ACTORS; i ++)
 {
  actor[i].type = ACTORTYPE_NONE;
  actor[i].controller = -1;
  actor[i].in_play = 0;
  for (j = 0; j < NO_CMDS; j ++)
  {
   actor[i].actor_cmd [j] = 0;
  }
 }
 
 for (i = 0; i < NO_PLAYERS; i ++)
 {
  player[i].actor_controlled = -1;
  player[i].user = -1;
  player[i].link_fire = 1;
  player[i].link_toggle_delay = 0;
//  for (j = 0; j < NO_CMDS; j ++)
//  {
//   actor[i].player_cmd [j] = 0;
//  }
 }

 for (i = 0; i < NO_BULLETS; i ++)
 {
  bullet[i].type = BULLET_NONE;
 }
 
 for (i = 0; i < NO_CLOUDS; i ++)
 {
  cloud[i].type = CLOUD_NONE;
 }

 init_pickups();
 
}


