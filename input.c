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
    
File: input.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - keyboard input. Commands are passed to cmds.c - see intro to that file
for a bit more information.

*/

#include <math.h>

#include "allegro.h"

#include "config.h"

#include "globvars.h"

#include "sound.h"

//#define DEBUG_KEYS 1

#ifdef DEBUG_KEYS
#include "actor.h"
#include "enemy.h"
#include <string.h>
#include "palette.h"
extern RGB palet [256];

int scrs;
int sshot_counter;
#endif


extern int inflicteda, inflictede;

void get_input(void)
{


 int i;

 for (i = 0; i < NO_CMDS; i ++)
 {
  if (key [player[0].keys [i]])
   actor[player[0].actor_controlled].actor_cmd [i] = 1;
    else
     actor[player[0].actor_controlled].actor_cmd [i] = 0;
 }

 for (i = 0; i < NO_CMDS; i ++)
 {
  if (key [player[1].keys [i]])
   actor[player[1].actor_controlled].actor_cmd [i] = 1;
    else
     actor[player[1].actor_controlled].actor_cmd [i] = 0;
 }

 if (key [player[0].keys [CMD_LINK]])
 {
  if (player[0].link_toggle_delay == 0)
  {
   if (player[0].link_fire == 0)
    player[0].link_fire = 1;
     else
      player[0].link_fire = 0;
   player[0].link_toggle_delay = 20;
   play_sound(WAV_MENU1);
  }
 }
 
 if (key [player[1].keys [CMD_LINK]])
 {
  if (player[1].link_toggle_delay == 0)
  {
   if (player[1].link_fire == 0)
    player[1].link_fire = 1;
     else
      player[1].link_fire = 0;
   player[1].link_toggle_delay = 20;
   play_sound(WAV_MENU1);
  }
 }
 


/*
 if (key [KEY_8_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_THRUST] = 1;
 if (key [KEY_4_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_LEFT] = 1;
 if (key [KEY_6_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_RIGHT] = 1;
 if (key [KEY_0_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_FIRE1] = 1;
 if (key [KEY_ENTER_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_FIRE2] = 1;
 if (key [KEY_PLUS_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_UPGRADE] = 1;
 if (key [KEY_1_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_LEFT1] = 1;
 if (key [KEY_3_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_RIGHT1] = 1;
 if (key [KEY_2_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_BRAKE] = 1;*/
#ifdef DEBUG_KEYS
 char sfile [20];
 char istr [20];

 if (sshot_counter > 0)
  sshot_counter --;

 if (key [KEY_F1] && sshot_counter <= 0)
 {
  BITMAP *scrshot_bmp = create_bitmap(640, 480);
  blit(screen, scrshot_bmp, 0,0,0,0,640,480);

  strcpy(sfile, "sshot");
  strcat(sfile, itoa(scrs, istr, 10));
  strcat(sfile, ".bmp");
  save_bitmap(sfile, scrshot_bmp, palet);
  clear_to_color(screen, COLOUR_WHITE);
  play_sound(WAV_MENU2);
  scrs ++;
  sshot_counter = 15;
 }

 if (key [KEY_MINUS_PAD] && counter % 5 == 0)
 {
  gain_upgrade_points(player[0].actor_controlled, 1);
 }
 if (key [KEY_TILDE])
 {
  hurt_actor(0, OWNER_ENEMY, 2000);
 }
 if (key [KEY_TAB])
 {
  arena.targets_left [0] = 1;
  arena.targets_left [1] = 0;
  arena.targets_left [2] = 0;
  arena.targets_left_total = 1;
  for (i = 0; i < NO_ENEMIES; i ++)
  {
   if (enemy[i].type != ENEMY_NONE)
    hurt_enemy(i, 9999, 0,0);
  }
  
 }
 if (key [KEY_2] && counter % 15 == 0)
 {
  actor[0].type ++;
 }
 if (key [KEY_1] && counter % 15 == 0)
 {
  actor[0].type --;
 }
 if (key [KEY_E])
 {
  inflicteda ++;
 }
 if (key [KEY_R])
 {
  inflicteda --;
 }
 if (key [KEY_D])
 {
  inflictede ++;
 }
 if (key [KEY_F])
 {
  inflictede --;
 }
 if (key [KEY_7_PAD])
 {
  actor[player[0].actor_controlled].armour += 10;
  if (actor[player[0].actor_controlled].armour > actor[player[0].actor_controlled].max_armour)
   actor[player[0].actor_controlled].armour = actor[player[0].actor_controlled].max_armour;
 }

#endif
// int special [10];
// if (key [KEY_5_PAD] && counter % 10 == 0)
// {
//  create_enemy(ENEMY_BOUNCER, 10000 + random() % 50000, 10000 + random() % 50000,
//   1000 - random() % 2000, 1000 - random() % 2000, 0, special);
//  create_enemy(ENEMY_STINGER, grand(3), 10000 + random() % 300000, 90000,
//   0, 0, 0, special);
// }
/*
 if (key [KEY_UP])
  actor[player[1].actor_controlled].actor_cmd [CMD_THRUST] = 1;
 if (key [KEY_LEFT])
  actor[player[1].actor_controlled].actor_cmd [CMD_LEFT] = 1;
 if (key [KEY_RIGHT])
  actor[player[1].actor_controlled].actor_cmd [CMD_RIGHT] = 1;
 if (key [KEY_DOWN])
  actor[player[1].actor_controlled].actor_cmd [CMD_BRAKE] = 1;
 if (key [KEY_SPACE])
  actor[player[1].actor_controlled].actor_cmd [CMD_FIRE1] = 1;
 if (key [KEY_Z])
  actor[player[1].actor_controlled].actor_cmd [CMD_FIRE2] = 1;
 if (key [KEY_X])
  actor[player[1].actor_controlled].actor_cmd [CMD_UPGRADE] = 1;
*/
}





