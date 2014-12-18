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
    
File: Main.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - main()
 - various initialisation functions
 - miscellaneous stuff


*/

/*

Your guide to source files:
(for more information visit the files themselves)

actor.c
 - functions for dealing with actors (ie players' ships)
bullet.c
 - creation, movement and collision of bullets
cloud.c
 - clouds - ie non-bullet special effects
cmds.c
 - makes actors implement commands issued in input.c
displ_in.c
 - initialises the display and loads bitmaps
display.c
 - shows things on the screen
eclass.c
 - big struct containing enemy data
enemy.c
 - horrible code for all enemies
game.c
 - the game loop and a few special functions
input.c
 - takes keyboard input
levels.c
 - initialises and manages the levels, enemy spawning etc
main.c
 - this file
menu.c
 - the front-end GUI
palette.c
 - the palette and transparency functions
pickup.c
 - pickups are energy/repair/etc
sound.c
 - initialises and plays sounds
stuff.c
 - small, very generic functions (like grand)
upgrades.c
 - struct containing ship upgrade data


Most .h files just contain function definitions. But some are special:

config.h
 - structs, enums and #defines
globvar.h
 - extern declarations of global variables
sound.h
 - the WAV_xxx sound enum
palette.h
 - the COLOUR_xxx and TRANS_xxx enums
display.h
 - all the graphics enums

*/



#include "allegro.h"

#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <pc.h>
#include <math.h>

#include "config.h"

#include "palette.h"

#include "displ_in.h"
#include "display.h"
#include "cmds.h"
#include "input.h"
#include "actor.h"
//#include "stars.h"
#include "bullet.h"
#include "cloud.h"
//#include "ships.h"
#include "enemy.h"
#include "levels.h"
#include "menu.h"
#include "sound.h"

// Global variables:

struct gamestruct game;

struct playerstruct player [NO_PLAYERS];

struct actorstruct actor [NO_ACTORS];

struct bulletstruct bullet [NO_BULLETS];

struct cloudstruct cloud [NO_CLOUDS];

struct starstruct star [NO_STARS];

struct arenastruct arena;

struct enemystruct enemy [NO_ENEMIES];

unsigned char counter;

struct pickupstruct pickup [NO_PICKUPS];

//   --- end global variables

// timer interupt functions and variables:
//void framecount(void);

//volatile int framecounter;
//volatile int frames_per_second;

volatile int inputcounter = 0;
volatile int inputs_per_second = 0;

volatile int turncounter = 0;
volatile int turns_per_second = 0;

void tickover(void);

volatile unsigned char ticked;
//volatile unsigned char tick_counter;
int slacktime;
// --- end timer interupt


// init functions
void init_at_startup(void);
void begin_game(void);
// --- end init functions


void game_loop(void);

struct optionstruct options;

/*
void framecount(void)
{
   frames_per_second = framecounter;
   framecounter = 0;
//   turns_per_second = turncounter;
//   turncounter = 0;
//   inputs_per_second = inputcounter;
//   inputcounter = 0;
//  arena.time_left --;
}
END_OF_FUNCTION (framecount);
*/

void tickover(void)
{
 ticked ++;
// tick_counter++; // assumes it'll wrap at 256
}
END_OF_FUNCTION (tickover);



int main(void)
{

 cprintf("\n\rHello and welcome to Lacewing version 1.10!");
 cprintf("\n\rCopyright 2003 by Linley Henzell & Captain Pork.\n\r");
 cprintf("\n\rLacewing is free software published under the");
 cprintf("\n\rGeneral Public Licence. It can be modified and");
 cprintf("\n\rredistributed under certain conditions and comes");
 cprintf("\n\rwith no warranties of any kind.");
 cprintf("\n\rPlease read the LICENCE.TXT file for more information.\n\r");
 cprintf("\n\rPress Q to Quit, or any other key to continue...\n\r");

 char rkey;

 int randcount; // doesn't matter if it's not initialised - better, actually

 do
 {
   randcount ++;
 }
  while(kbhit() == 0);
  
 rkey = getch();
 if (rkey == 'q' || rkey == 'Q')
  return 0;
 if (rkey == 0)
  getch();

 srandom(randcount);

 init_at_startup();

 menu_loop();
 
 return 0;

}

END_OF_MAIN();


void init_at_startup(void)
{

   allegro_init();

   set_config_file("lacew.cfg");
   
   install_keyboard();
   install_timer();

   three_finger_flag = 0;
   key_led_flag = 0;

//   LOCK_FUNCTION (framecount);
   LOCK_FUNCTION (tickover);
   LOCK_VARIABLE (ticked);
//   LOCK_VARIABLE (tick_counter);
//   LOCK_VARIABLE (frames_per_second);
//   LOCK_VARIABLE (framecounter);
   LOCK_VARIABLE (turns_per_second);
   LOCK_VARIABLE (turncounter);
//   LOCK_VARIABLE (inputs_per_second);
//   LOCK_VARIABLE (inputcounter);

//   install_int (framecount, 1000);
   install_int (tickover, 30);

   set_color_depth(8);

   if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0) != 0)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to set 640x480 mode\n%s\n", allegro_error);
      exit(1);
   }

   init_palette();

   init_display();
   init_menus_once_only();

   init_sound(); // must come after init_menus_once_only, as that's where
    // options.sound_enabled is set.

}



