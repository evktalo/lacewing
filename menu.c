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
    
File: menu.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - the front end GUI. One of the few files other than display.c & displ_in.c
which are authorised to write directly to the screen.

*/

#include "allegro.h"

#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "globvars.h"

#include "stuff.h"

#include "palette.h"

#include "game.h"
#include "upgrades.h"
#include "display.h"
#include "sound.h"

#define NO_SCORES 15
#define SCORE_LENGTH 15

#define GRID_GRAIN 100
#define NO_BOUNCIES 16

#define NO_KEY KEY_TILDE

#define KP_WAIT_LONG 33
#define KP_WAIT_SHORT 5

extern struct optionstruct options;

enum
{
MENU_NONE,
MENU_MAIN,
MENU_OPTIONS,
MENU_SCORES,
MENU_ENTER_SCORES,
MENU_DUEL
};

enum
{
DUEL_OPTIONS,
DUEL_START,
DUEL_MODE,
DUEL_ENEMIES,
DUEL_HANDICAP_P1,
DUEL_HANDICAP_P2,
DUEL_EXIT,
DUEL_END

};

void menu_display(void);
void start_game_from_menu(int game_type);
void menu_input(void);

void menu_display_main(void);
void menu_display_options(void);
void menu_display_duel(void);
void menu_display_scores(void);
void menu_trigger(void);
void display_victory_screen(void);

int acceptable_char(int scode);
int acceptable_name_char(int ascode);
char *scancode_to_keyname(int scanc);
char scancode_to_asc(int scanc);
char get_shifted_number(char inputted);

void test_speakers(void);

BITMAP *menu_bmp;
extern volatile unsigned char ticked;
extern int slacktime;

extern FONT *font2;
extern FONT *small_font;

//BITMAP *title_in;
//BITMAP *title_out;
RLE_SPRITE *rle_title_i;
RLE_SPRITE *rle_title_o;
RLE_SPRITE *rle_title_s;

BITMAP *upgrade_box1;
BITMAP *upgrade_box2;
BITMAP *upgrade_box3;

int menu_index;
int menu_index_max;
int menu_index_min;
int keypress_wait;
int last_key;
int check_key(int scode);
int check_key_disregard_last_key(int scode);
unsigned char menu_counter;

int which_menu;

void quit_game(void);

void change_menu(int to_which);
int option_index(void);
void enter_keystroke(void);
int option_jump(int direction);
void init_config(void);
void save_config(void);

void choose_ships(void);
void display_ship_choice(int players, int p1a, int p2a, int p1c, int p2c);
void display_ship_choice2(int ship, int y, int chosen);

void jam_keys(void);

char *ship_name_long(int ship);
char *ship_name_short(int ship);
char *ship_description1(int ship);
char *ship_description2(int ship);
char *upgrade_name_long(int i);

extern RLE_SPRITE *large_ships [12] [5];

struct score_list
{
  int score;
  int ship;
  int level;
  char name [25];
};

struct score_list hs_single [NO_SCORES];
struct score_list hs_coop [NO_SCORES];

struct score_list *hscore;

void check_high_scores(void);
int enter_score_name_single(int s);
int enter_score_name_coop(int s);
void push_scores_single(int insert, int play);
void push_scores_coop(int insert, int play);
void congratulations(int player);

int entering_score;
int entering_score2;
void draw_scrolling_grid(int min_x, int min_y, int max_x, int max_y, int colour);
void show_grid(int grid_colour, int border_colour);
void make_grid_scroll(void);

int grid_x_speed, grid_y_speed, grid_x, grid_y, grid_x_accel, grid_y_accel;

struct bouncy_list
{
 int x;
 int y;
 int x_speed;
 int y_speed;
 int colour1;
 int colour2;
};

struct bouncy_list bouncy [NO_BOUNCIES];

void init_bouncies(void);
void make_bouncies_move(void);
void draw_bouncies(void);

//RGB title_colour1;
//RGB title_colour2;

void init_menus_once_only(void)
{
 text_mode(-1);
 init_config();

 menu_bmp = create_bitmap(640, 480);

 if (!menu_bmp)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to create menu bitmap! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 menu_counter = 0;
 entering_score = 0;

 grid_x_speed = grand(6001) - 3000;
 grid_y_speed = grand(6001) - 3000;
 grid_x_accel = 0;
 grid_y_accel = 0;

 RGB temp_palette [256];

 BITMAP *temp_bmp = load_bitmap("gfx\\title_i.bmp", temp_palette);

 if (!temp_bmp)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to load title_i.bmp! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 rle_title_i = get_rle_sprite(temp_bmp);

 if (!rle_title_i)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to create rle_title_i! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 destroy_bitmap(temp_bmp);

 temp_bmp = load_bitmap("gfx\\title_o.bmp", temp_palette);

 if (!temp_bmp)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to load title_o.bmp! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 rle_title_o = get_rle_sprite(temp_bmp);

 if (!rle_title_o)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to create rle_title_o! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 destroy_bitmap(temp_bmp);

 temp_bmp = load_bitmap("gfx\\title_s.bmp", temp_palette);

 if (!temp_bmp)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to load title_s.bmp! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 rle_title_s = get_rle_sprite(temp_bmp);

 if (!rle_title_s)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to create rle_title_s! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 destroy_bitmap(temp_bmp);

 upgrade_box1 = load_bitmap("gfx\\upg_box.bmp", temp_palette);

 if (!upgrade_box1)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to load upg_box.bmp! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 upgrade_box2 = load_bitmap("gfx\\upg_box2.bmp", temp_palette);

 if (!upgrade_box2)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to load upg_box2.bmp! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 upgrade_box3 = load_bitmap("gfx\\upg_box3.bmp", temp_palette);

 if (!upgrade_box3)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to load upg_box3.bmp! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 last_key = NO_KEY; // something meaningless

 game.duel_mode = 0;
 game.duel_handicap [0] = 1;
 game.duel_handicap [1] = 1;
 game.duel_crawlies = 1;
 
}

void menu_loop(void)
{

 which_menu = MENU_MAIN;
 menu_index_max = 4;
 menu_index_min = 0;
 keypress_wait = KP_WAIT_LONG;

 do
 {
  menu_counter++;

  if (check_key_disregard_last_key(last_key) == 0)
   keypress_wait = 0;

  if (keypress_wait > 0)
  {
      keypress_wait --;
  }
  
  make_grid_scroll();
//  if (which_menu == MENU_MAIN || which_menu == MENU_SCORES || which_menu == MENU_ENTER_SCORES)
  if (which_menu == MENU_SCORES || which_menu == MENU_ENTER_SCORES)
   make_bouncies_move();

  if (ticked == 0)
  {
   clear_bitmap(menu_bmp);
   menu_display();
   blit(menu_bmp, screen, 0, 0, 0, 0, 640, 480);
  }

  menu_input();

  slacktime = 0;
  do
  {
   slacktime ++;
  } while(ticked == 0);
  ticked --;

 } while(TRUE);


}


void menu_display(void)
{
 text_mode(-1);

 switch(which_menu)
 {
  case MENU_MAIN:
  menu_display_main();
  break;
  case MENU_OPTIONS:
  menu_display_main();
  menu_display_options();
  break;
  case MENU_DUEL:
  menu_display_main();
  menu_display_duel();
  break;
  case MENU_SCORES:
  menu_display_scores();
  break;
  case MENU_ENTER_SCORES:
  menu_display_scores();
  break;
 }

}


void menu_display_main(void)
{

  text_mode(-1);

  vsync();

  RGB title_colour [1];

  int mcn = menu_counter;
  int mcn2 = menu_counter / 2;
  int mcn3 = menu_counter / 4;

  if (mcn % 64 < 32)
  {
   title_colour[0].r = mcn % 64;
  }
   else
   {
    title_colour[0].r = 63 - (mcn % 64);
   }
  
  if (mcn2 % 64 < 32)
  {
   title_colour[0].g = (63 - mcn2 % 64) / 4;
  }
   else
   {
    title_colour[0].g = (mcn2 % 64) / 4;
   }
  
  if (mcn3 % 64 < 32)
  {
   title_colour[0].b = ((mcn3 + 32) % 64);// / 2;
  }
   else
   {
    title_colour[0].b = (63 - ((mcn3 + 32) % 64));// / 2;
   }
  
  set_color(252, title_colour);

  mcn = menu_counter;
  mcn2 = menu_counter / 2;
  mcn3 = menu_counter / 4;

  if (mcn % 64 >= 32)
  {
   title_colour[0].b = (mcn + 32) % 64;
  }
   else
   {
    title_colour[0].b = 63 - ((mcn + 32) % 64);
   }
  
  if (mcn2 % 64 >= 32)
  {
   title_colour[0].g = (63 - mcn2 % 64);// / 2;
  }
   else
   {
    title_colour[0].g = (mcn2 % 64);// / 2;
   }
  
  if (mcn3 % 64 >= 32)
  {
   title_colour[0].r = mcn3 % 64;
  }
   else
   {
    title_colour[0].r = 63 - (mcn3 % 64);
   }
  
  set_color(255, title_colour);
  
/*  int mcn = menu_counter;
  int mcn2 = menu_counter / 2;
  int mcn3 = menu_counter / 4;

  if (mcn % 64 < 32)
  {
   title_colour[0].r = mcn % 64;
   title_colour[0].g = (63 - mcn2 % 64) / 4;
   title_colour[0].b = ((mcn3 + 32) % 64);// / 2;
  }
   else
   {
    title_colour[0].r = 63 - (mcn % 64);
    title_colour[0].g = (mcn2 % 64) / 4;
    title_colour[0].b = (63 - ((mcn3 + 32) % 64));// / 2;
   }
  
  set_color(252, title_colour);

  mcn = menu_counter / 4;
  mcn2 = menu_counter / 8;
  mcn3 = menu_counter / 16;

  if (mcn % 64 >= 32)
  {
   title_colour[0].r = mcn3 % 64;
   title_colour[0].g = (63 - mcn2 % 64);// / 2;
   title_colour[0].b = (mcn + 32) % 64;
  }
   else
   {
    title_colour[0].r = 63 - (mcn3 % 64);
    title_colour[0].g = (mcn2 % 64);// / 2;
    title_colour[0].b = 63 - ((mcn + 32) % 64);
   }
  
  set_color(255, title_colour);*/
//  RGB title_colour1;
//RGB title_colour2;

/*  rectfill(menu_bmp, 10, 10, 630, 470, COLOUR_GREY1);
  rect(menu_bmp, 10, 10, 630, 470, COLOUR_GREEN4);
  rect(menu_bmp, 9, 9, 631, 471, COLOUR_GREEN6);
  rect(menu_bmp, 8, 8, 632, 472, COLOUR_GREEN8);

  draw_scrolling_grid(11, 11, 629, 469, COLOUR_GREEN3);
*/
  show_grid(COLOUR_GREEN3, COLOUR_GREEN8);

//  textprintf_centre(menu_bmp, font, 300, 50, COLOUR_YELLOW8 - (menu_counter / 4) % 7, "L___a___c___e___w___i___n___g}");
//  textprintf_centre(menu_bmp, font2, 300, 50, COLOUR_RED1 + (menu_counter / 4) % 7, "L___a___c___e___w___i___n___g}");


  draw_rle_sprite(menu_bmp, rle_title_i, 30, 0);
  draw_rle_sprite(menu_bmp, rle_title_o, 30, 0);
  draw_rle_sprite(menu_bmp, rle_title_s, 280, 173);

  int col1 = COLOUR_GREY3;
//  int col2 = COLOUR_GREY5;
  int scol1 = COLOUR_GREEN8 - (menu_counter / 3) % 3;
  int scol2 = COLOUR_BLUE6 + (menu_counter / 3) % 3;

  scol1 = 252;
  scol2 = 255;

  int mx = 290;
  int my = 212;
  int mys = 49;

  if (menu_index == 0 && which_menu == MENU_MAIN)
  {
   textprintf_right(menu_bmp, font2, mx, my, scol1, "Start} Game}");
   textprintf_right(menu_bmp, font, mx, my, scol2, "Start} Game}");
//   textprintf_right(menu_bmp, font2, 400, 120, COLOUR_WHITE, "Start} Game}");
//   textprintf_right(menu_bmp, font, 400, 120, COLOUR_WHITE, "Start} Game}");
  }
   else
    textprintf_right(menu_bmp, font, mx, my, col1, "Start} Game}");
  
  if (menu_index == 1 && which_menu == MENU_MAIN)
  {
   textprintf_right(menu_bmp, font2, mx, my + mys, scol1, "Duel}");
   textprintf_right(menu_bmp, font, mx, my + mys, scol2, "Duel}");
  }
   else
    textprintf_right(menu_bmp, font, mx, my + mys, col1, "Duel}");
  
  if (menu_index == 2 && which_menu == MENU_MAIN)
  {
   textprintf_right(menu_bmp, font2, mx, my + mys * 2, scol1, "Options}");
   textprintf_right(menu_bmp, font, mx, my + mys * 2, scol2, "Options}");
  }
   else
    textprintf_right(menu_bmp, font, mx, my + mys * 2, col1, "Options}");
  
  if (menu_index == 3 && which_menu == MENU_MAIN)
  {
   textprintf_right(menu_bmp, font2, mx, my + mys * 3, scol1, "High} Scores}");
   textprintf_right(menu_bmp, font, mx, my + mys * 3, scol2, "High} Scores}");
  }
   else
    textprintf_right(menu_bmp, font, mx, my + mys * 3, col1, "High} Scores}");

  if (menu_index == 4 && which_menu == MENU_MAIN)
  {
   textprintf_right(menu_bmp, font2, mx, my + mys * 4, scol1, "Exit}");
   textprintf_right(menu_bmp, font, mx, my + mys * 4, scol2, "Exit}");
  }
   else
    textprintf_right(menu_bmp, font, mx, my + mys * 4, col1, "Exit}");
  
/*  textprintf_centre(menu_bmp, font, 300, 300, COLOUR_YELLOW7, "Space} For} 1P}  Enter} For} 2P}...");
  textprintf_centre(menu_bmp, font2, 300, 300, COLOUR_RED7, "Space} For} 1P}  Enter} For} 2P}...");*/

}

void menu_display_options(void)
{

  rectfill(menu_bmp, 180, 50, 440, 400, COLOUR_GREEN1);
  rect(menu_bmp, 180, 50, 440, 400, COLOUR_GREEN6);
  rect(menu_bmp, 179, 49, 441, 401, COLOUR_GREEN7);
  rect(menu_bmp, 178, 48, 442, 402, COLOUR_GREEN8);

  int i;

  int ox = 350, oy = 9, oys = 75;
  int col;

  char ostr [50];
  char istr [10];

  for (i = 0; i < 32; i ++)
  {

   col = COLOUR_GREEN6;

   if (i == menu_index)
    col = COLOUR_YELLOW8;
   
   switch(i)
   {
    case 0:
     col = COLOUR_GREY6;
     strcpy(ostr, "General Options");
     break;
    case 1:
     strcpy(ostr, "Sound - ");
     if (options.sound_init == 0)
      strcat(ostr, "Disabled in config file");
       else
       {
        switch(options.sound_mode)
        {
         case SOUNDMODE_OFF:
          strcat(ostr, "Off");
          break;
         case SOUNDMODE_MONO:
          strcat(ostr, "On (Mono)");
          break;
         case SOUNDMODE_STEREO:
          strcat(ostr, "On (Stereo)");
          break;
         case SOUNDMODE_REVERSED:
          strcat(ostr, "On (Reverse Stereo)");
          break;
         default:
          strcat(ostr, "Buggy sound settings?");
          break;
        }
       }
     break;
    case 2:
     strcpy(ostr, "Sound Effects Volume - ");
     if (options.sound_volume == 0)
     {
      strcat(ostr, "Off");
     }
      else
       strcat(ostr, itoa(options.sound_volume, istr, 10));
     break;
    case 3:
     strcpy(ostr, "Ambience Volume - ");
     if (options.ambience_volume == 0)
     {
      strcat(ostr, "Off");
     }
      else
       strcat(ostr, itoa(options.ambience_volume, istr, 10));
     break;
    case 4:
     strcpy(ostr, "Video Sync - ");
     if (options.run_vsync == 0)
      strcat(ostr, "Off");
       else
        strcat(ostr, "On");
     break;
    case 5:
     strcpy(ostr, "Test Speakers");
     break;
    case 6:
     strcpy(ostr, "Test Keys");
     break;
    case 7:
     strcpy(ostr, "");
     break;
    case 8:
     col = COLOUR_GREY6;
     strcpy(ostr, "Player 1 Keys");
     break;
    case 9:
     strcpy(ostr, "Forwards - ");
     strcat(ostr, scancode_to_keyname(player[0].keys [CMD_THRUST]));
     break;
    case 10:
     strcpy(ostr, "Left - ");
     strcat(ostr, scancode_to_keyname(player[0].keys [CMD_LEFT]));
     break;
    case 11:
     strcpy(ostr, "Right - ");
     strcat(ostr, scancode_to_keyname(player[0].keys [CMD_RIGHT]));
     break;
    case 12:
     strcpy(ostr, "Brake - ");
     strcat(ostr, scancode_to_keyname(player[0].keys [CMD_BRAKE]));
     break;
    case 13:
     strcpy(ostr, "Fire Cannon - ");
     strcat(ostr, scancode_to_keyname(player[0].keys [CMD_FIRE1]));
     break;
    case 14:
     strcpy(ostr, "Fire Secondary - ");
     strcat(ostr, scancode_to_keyname(player[0].keys [CMD_FIRE2]));
     break;
    case 15:
     strcpy(ostr, "Upgrade - ");
     strcat(ostr, scancode_to_keyname(player[0].keys [CMD_UPGRADE]));
     break;
    case 16:
     strcpy(ostr, "Slide Left - ");
     strcat(ostr, scancode_to_keyname(player[0].keys [CMD_LEFT1]));
     break;
    case 17:
     strcpy(ostr, "Slide Right - ");
     strcat(ostr, scancode_to_keyname(player[0].keys [CMD_RIGHT1]));
     break;
    case 18:
     strcpy(ostr, "Toggle Linked Fire - ");
     strcat(ostr, scancode_to_keyname(player[0].keys [CMD_LINK]));
     break;
    case 19:
     col = COLOUR_GREY6;
     strcpy(ostr, "Player 2 Keys");
     break;
    case 20:
     strcpy(ostr, "Forwards - ");
     strcat(ostr, scancode_to_keyname(player[1].keys [CMD_THRUST]));
     break;
    case 21:
     strcpy(ostr, "Left - ");
     strcat(ostr, scancode_to_keyname(player[1].keys [CMD_LEFT]));
     break;
    case 22:
     strcpy(ostr, "Right - ");
     strcat(ostr, scancode_to_keyname(player[1].keys [CMD_RIGHT]));
     break;
    case 23:
     strcpy(ostr, "Brake - ");
     strcat(ostr, scancode_to_keyname(player[1].keys [CMD_BRAKE]));
     break;
    case 24:
     strcpy(ostr, "Fire Cannon - ");
     strcat(ostr, scancode_to_keyname(player[1].keys [CMD_FIRE1]));
     break;
    case 25:
     strcpy(ostr, "Fire Secondary - ");
     strcat(ostr, scancode_to_keyname(player[1].keys [CMD_FIRE2]));
     break;
    case 26:
     strcpy(ostr, "Upgrade - ");
     strcat(ostr, scancode_to_keyname(player[1].keys [CMD_UPGRADE]));
     break;
    case 27:
     strcpy(ostr, "Slide Left - ");
     strcat(ostr, scancode_to_keyname(player[1].keys [CMD_LEFT1]));
     break;
    case 28:
     strcpy(ostr, "Slide Right - ");
     strcat(ostr, scancode_to_keyname(player[1].keys [CMD_RIGHT1]));
     break;
    case 29:
     strcpy(ostr, "Toggle Linked Fire - ");
     strcat(ostr, scancode_to_keyname(player[1].keys [CMD_LINK]));
     break;
    case 30:
     strcpy(ostr, "");
//     strcat(ostr, scancode_to_keyname(player[1].keys [CMD_LINK])
     break;
    case 31:
     strcpy(ostr, "Exit");
     break;

   }

   textprintf_right(menu_bmp, small_font, ox, oys + oy * i, col, ostr);

  }

/*
  textprintf_centre(menu_bmp, font, 300, 200, COLOUR_YELLOW8 - (menu_counter / 4) % 7, "L___a___c___e___w___i___n___g}");
  textprintf_centre(menu_bmp, font2, 300, 200, COLOUR_RED1 + (menu_counter / 4) % 7, "L___a___c___e___w___i___n___g}");
  textprintf_centre(menu_bmp, font, 300, 300, COLOUR_YELLOW7, "Space} For} 1P}  Enter} For} 2P}...");
  textprintf_centre(menu_bmp, font2, 300, 300, COLOUR_RED7, "Space} For} 1P}  Enter} For} 2P}...");
*/
}

int option_jump(int direction)
{

 if (which_menu != MENU_OPTIONS)
  return 0;

 switch(menu_index)// + direction)
 {
//  case 5:
//  case 6:
  case 7:
  case 8:
  case 19:
  case 30:
   return 1;
 }

 return 0;

}



void menu_display_duel(void)
{

  rectfill(menu_bmp, 180, 50, 440, 400, COLOUR_GREEN1);
  rect(menu_bmp, 180, 50, 440, 400, COLOUR_GREEN6);
  rect(menu_bmp, 179, 49, 441, 401, COLOUR_GREEN7);
  rect(menu_bmp, 178, 48, 442, 402, COLOUR_GREEN8);


  textprintf_centre(menu_bmp, font, 320, 90, COLOUR_ORANGE4 + (menu_counter / 4) % 4, "Duel}");
  textprintf_centre(menu_bmp, font2, 320, 90, COLOUR_RED8 - (menu_counter / 4) % 4, "Duel}");
//  textprintf_centre(menu_bmp, font, 320, 110, COLOUR_ORANGE8, "K_e_y_s}");
//  textprintf_centre(menu_bmp, font2, 320, 110, COLOUR_YELLOW8, "K_e_y_s}");


  int i;

  int ox = 320, oy = 11, oys = 200;
  int col;

  char ostr [50];

  for (i = 0; i < DUEL_END; i ++)
  {

   col = COLOUR_GREEN6;

   if (i == menu_index)
    col = COLOUR_YELLOW8;
   
   switch(i)
   {
    case DUEL_OPTIONS:
     col = COLOUR_GREY6;
     strcpy(ostr, "Duel Options - Space or Enter to select");
     break;
    case DUEL_MODE:
     strcpy(ostr, "Victory goes to the ");
     switch(game.duel_mode)
     {
      case DUEL_3_MINUTES:
       strcat(ostr, "winner after 3 minutes");
       break;
      case DUEL_10_MINUTES:
       strcat(ostr, "winner after 10 minutes");
       break;
      case DUEL_10_POINTS:
       strcat(ostr, "first to 10 points");
       break;
      case DUEL_30_POINTS:
       strcat(ostr, "first to 30 points");
       break;
     }
     break;
    case DUEL_ENEMIES:
     strcpy(ostr, "Enemies - ");
     switch(game.duel_crawlies)
     {
       case 0:
       strcat(ostr, "None");
       break;
       case 1:
       strcat(ostr, "A few");
       break;
       case 2:
       strcat(ostr, "Plentiful");
       break;
       case 3:
       strcat(ostr, "Swarming");
       break;
     }
     break;
    case DUEL_HANDICAP_P1:
     strcpy(ostr, "Player 1 Hull - ");
     switch(game.duel_handicap [0])
     {
       case 0:
       strcat(ostr, "75%%");
       break;
       case 1:
       strcat(ostr, "100%%");
       break;
       case 2:
       strcat(ostr, "120%%");
       break;
       case 3:
       strcat(ostr, "150%%");
       break;
     }
     break;
    case DUEL_HANDICAP_P2:
     strcpy(ostr, "Player 2 Hull - ");
     switch(game.duel_handicap [1])
     {
       case 0:
       strcat(ostr, "75%%");
       break;
       case 1:
       strcat(ostr, "100%%");
       break;
       case 2:
       strcat(ostr, "120%%");
       break;
       case 3:
       strcat(ostr, "150%%");
       break;
     }
     break;
    case DUEL_START:
     strcpy(ostr, "Start Duel");
     break;
    case DUEL_EXIT:
     strcpy(ostr, "Exit");
     break;

   }

   textprintf_centre(menu_bmp, small_font, ox, oys + oy * i, col, ostr);

  }

/*
  textprintf_centre(menu_bmp, font, 300, 200, COLOUR_YELLOW8 - (menu_counter / 4) % 7, "L___a___c___e___w___i___n___g}");
  textprintf_centre(menu_bmp, font2, 300, 200, COLOUR_RED1 + (menu_counter / 4) % 7, "L___a___c___e___w___i___n___g}");
  textprintf_centre(menu_bmp, font, 300, 300, COLOUR_YELLOW7, "Space} For} 1P}  Enter} For} 2P}...");
  textprintf_centre(menu_bmp, font2, 300, 300, COLOUR_RED7, "Space} For} 1P}  Enter} For} 2P}...");
*/
}


int check_key(int scode)
{
 if (keypress_wait == 0)
  last_key = NO_KEY;

 if (last_key == scode)
 {
  if (key [scode])
   return 0;
    else
     last_key = NO_KEY;
 }

 if (key [scode])
 {
  last_key = scode;
  return 1;
 }
 return 0;
}

int check_key_disregard_last_key(int scode)
{
 if (key [scode])
  return 1;

 return 0;
}






void menu_input(void)
{

 clear_keybuf();

 if (keypress_wait > 0)
  return;

 if (which_menu == MENU_ENTER_SCORES)
 {
  int done = 0;

  if (check_key(KEY_ESC))
  {
   save_config();
   change_menu(MENU_SCORES);
//   last_key = KEY_ESC;
   play_sound(WAV_MENU2);
   return;
  }
 
  if (game.type == GAME_SINGLE)
   done = enter_score_name_single(entering_score);//hs_single[entering_score].name);
  if (game.type == GAME_COOP)
   done = enter_score_name_coop(entering_score);//hs_coop[entering_score].name);

  if (done == 1)
  {
   save_config();
   change_menu(MENU_SCORES);
  }
  return;
 }


 if (check_key(KEY_2_PAD) || check_key(KEY_DOWN))
 {
  do
  {
   if (menu_index < menu_index_max)
    menu_index ++;
     else
      menu_index = menu_index_min;
  } while (option_jump(1) == 1);
  keypress_wait = KP_WAIT_SHORT;
  play_sound(WAV_MENU1);
 }

 if (check_key(KEY_8_PAD) || check_key(KEY_UP))
 {
  do
  {
   if (menu_index > menu_index_min)
    menu_index --;
     else
      menu_index = menu_index_max;
  } while (option_jump(-1) == 1);
  keypress_wait = KP_WAIT_SHORT;
  play_sound(WAV_MENU1);
 }

 if (check_key(KEY_ESC))
 {
  switch(which_menu)
  {
   case MENU_MAIN:
    if (keypress_wait == 0 && last_key != KEY_ESC)
    {
     play_sound(WAV_MENU2);
     quit_game();
    }
    break;
   case MENU_DUEL:
   case MENU_OPTIONS:
    change_menu(MENU_MAIN);
    keypress_wait = KP_WAIT_LONG;
    play_sound(WAV_MENU2);
    break;
   case MENU_SCORES:
    change_menu(MENU_MAIN);
    keypress_wait = KP_WAIT_LONG;
    play_sound(WAV_MENU2);
    break;
  }
 }

// this has to be after the key_esc thing or pressing escape in some
//  sub-menus will trigger it, as key_waiting won't be checked after
//  the submenus return the menu_trigger call.
// if (key [KEY_ENTER] || key [KEY_ENTER_PAD] || key [KEY_SPACE])
 if (check_key(KEY_ENTER) || check_key(KEY_ENTER_PAD) || check_key(KEY_SPACE))
 {
  menu_trigger();
 }


}


void menu_trigger(void)
{

 keypress_wait = KP_WAIT_LONG;

 switch(which_menu)
 {
  case MENU_MAIN:
   switch(menu_index)
   {
    case 0:
     play_sound(WAV_MENU2);
     choose_ships();
//     start_game_from_menu(GAME_SINGLE);
     // if it's going to be coop, this is set after each player has had a chance to join
     break;
//    case 1:
//     start_game_from_menu(GAME_COOP);
//     break;
    case 1:
     change_menu(MENU_DUEL);
     play_sound(WAV_MENU2);
//     game.type = GAME_DUEL;
//     choose_ships();
//     start_game_from_menu(GAME_DUEL);
     break;
    case 2:
     change_menu(MENU_OPTIONS);
     play_sound(WAV_MENU2);
     break;
    case 3:
     change_menu(MENU_SCORES);
     play_sound(WAV_MENU2);
//     high_score_list();
     break;
    case 4:
     play_sound(WAV_MENU2);
     quit_game();
     break;
   }
   break;
  case MENU_OPTIONS:
   if (menu_index == 31)
   {
    play_sound(WAV_MENU2);
    change_menu(MENU_MAIN);
    break;
   }
   if (menu_index < 9)
   {
    switch(menu_index)
    {
     case 1:
      if (options.sound_mode == SOUNDMODE_REVERSED)
       options.sound_mode = SOUNDMODE_OFF;
        else
         options.sound_mode++;
      play_sound(WAV_MENU1);
      return;
     case 2:
      if (options.sound_volume >= 100)
       options.sound_volume = 0;
        else
         options.sound_volume += 20;
      play_sound(WAV_MENU1);
      return;
     case 3:
      if (options.ambience_volume >= 100)
       options.ambience_volume = 0;
        else
         options.ambience_volume += 20;
      play_sound(WAV_MENU1);
      return;
     case 4:
      if (options.run_vsync == 1)
       options.run_vsync = 0;
        else
         options.run_vsync = 1;
      play_sound(WAV_MENU1);
      return;
     case 5:
      test_speakers();
      ticked = 0;
// test speakers
      return;
     case 6:
      play_sound(WAV_MENU2);
      jam_keys();
      return;

    }
   }
   if (option_index() != -1)
   {
    play_sound(WAV_MENU2);
    enter_keystroke();
    play_sound(WAV_MENU2);
   }
   break;
  case MENU_SCORES:
   play_sound(WAV_MENU2);
   change_menu(MENU_MAIN);
   break;
  case MENU_ENTER_SCORES:
   play_sound(WAV_MENU2);
   change_menu(MENU_MAIN);
   break; // return from this through enter_scores (or whatever)
  case MENU_DUEL:
   switch(menu_index)
   {
    case DUEL_START:
     game.type = GAME_DUEL;
     play_sound(WAV_MENU2);
     choose_ships();
     break;
    case DUEL_ENEMIES:
     if (game.duel_crawlies < 3)
      game.duel_crawlies ++;
       else
        game.duel_crawlies = 0;
     play_sound(WAV_MENU2);
     break;
    case DUEL_MODE:
     if (game.duel_mode < 3)
      game.duel_mode ++;
       else
        game.duel_mode = 0;
     play_sound(WAV_MENU2);
     break;
    case DUEL_HANDICAP_P1:
     if (game.duel_handicap [0] < 3)
      game.duel_handicap [0] ++;
       else
        game.duel_handicap [0] = 0;
     play_sound(WAV_MENU2);
     break;
    case DUEL_HANDICAP_P2:
     if (game.duel_handicap [1] < 3)
      game.duel_handicap [1] ++;
       else
        game.duel_handicap [1] = 0;
     play_sound(WAV_MENU2);
     break;
    case DUEL_EXIT:
     change_menu(MENU_MAIN);
     play_sound(WAV_MENU2);
     break;
   }
   break;
   
 }

}


void enter_keystroke(void)
{
 rectfill(screen, 400, 210, 550, 280, COLOUR_GREY2);
 rect(screen, 400, 210, 550, 280, COLOUR_GREY6);
 rect(screen, 401, 211, 549, 279, COLOUR_GREY5);
 rect(screen, 402, 212, 548, 278, COLOUR_GREY4);
 rect(screen, 403, 213, 547, 277, COLOUR_GREY3);
 textprintf_right(screen, small_font, 500, 240, COLOUR_WHITE, "Press a key...");
          
 clear_keybuf();
 int inputted = KEY_ESC;

 int i;

 rest(200); // to clear the enter or space key

 do
 {
  for (i = KEY_A; i < KEY_CAPSLOCK + 1; i ++)
  {
   if (check_key(i))
   {
    inputted = i;
   }
  }
 }
   while(acceptable_char(inputted) == 0);
/*
  do
  {
   inputted = readkey();
  }
   while(acceptable_char(inputted >> 8) == 0);

  inputted = inputted >> 8;
*/
  if (option_index() == -1)
   return;

  if (menu_index < 20)
   player[0].keys [option_index()] = inputted;
    else
     player[1].keys [option_index()] = inputted;

  ticked = 0;


}

int option_index(void)
{
  switch(menu_index)
  {
   case 9:
   case 20:
    return CMD_THRUST;
   case 10:
   case 21:
    return CMD_LEFT;
   case 11:
   case 22:
    return CMD_RIGHT;
   case 12:
   case 23:
    return CMD_BRAKE;
   case 13:
   case 24:
    return CMD_FIRE1;
   case 14:
   case 25:
    return CMD_FIRE2;
   case 15:
   case 26:
    return CMD_UPGRADE;
   case 16:
   case 27:
    return CMD_LEFT1;
   case 17:
   case 28:
    return CMD_RIGHT1;
   case 18:
   case 29:
    return CMD_LINK;
  }

 return -1;
 
}


void test_speakers(void)
{
 rectfill(screen, 400, 210, 550, 280, COLOUR_GREY2);
 rect(screen, 400, 210, 550, 280, COLOUR_GREY6);
 rect(screen, 401, 211, 549, 279, COLOUR_GREY5);
 rect(screen, 402, 212, 548, 278, COLOUR_GREY4);
 rect(screen, 403, 213, 547, 277, COLOUR_GREY3);
 if (options.sound_init == 0)
 {
  textprintf_right(screen, small_font, 520, 240, COLOUR_WHITE, "Sound disabled.");
  rest(1000);
  return;
 }
 textprintf_right(screen, small_font, 520, 240, COLOUR_WHITE, "Testing left speaker. . .");
 if (options.sound_mode == SOUNDMODE_REVERSED)
  play_sound2(WAV_UPGRADE, 1000, 255, 0);
   else
    play_sound2(WAV_UPGRADE, 1000, 255, 255);
 rest(1000);
 rectfill(screen, 404, 214, 546, 276, COLOUR_GREY2);
 textprintf_right(screen, small_font, 520, 240, COLOUR_WHITE, "Testing right speaker. . .");
 if (options.sound_mode == SOUNDMODE_REVERSED)
  play_sound2(WAV_UPGRADE, 1000, 255, 255);
   else
    play_sound2(WAV_UPGRADE, 1000, 255, 0);
 rest(1000);
}
          
void start_game_from_menu(int game_type)
{

 game.type = game_type;

 vsync();
 clear_bitmap(screen);

 begin_game();

 ticked = 0;

 game_loop();

 if (arena.level == 17)
 {
  display_victory_screen();
 }

 check_high_scores();

}


void change_menu(int to_which)
{

 which_menu = to_which;
 keypress_wait = KP_WAIT_LONG;

 switch(which_menu)
 {
  default:
  case MENU_MAIN:
    game.type = GAME_SINGLE;
    which_menu = MENU_MAIN;
    menu_index = 0;
    menu_index_max = 4;
    menu_index_min = 0;
    save_config();
    break;
  case MENU_OPTIONS:
    which_menu = MENU_OPTIONS;
    menu_index = 1;
    menu_index_max = 31;
    menu_index_min = 1;
    break;
  case MENU_DUEL:
    which_menu = MENU_DUEL;
    menu_index = 1;
    menu_index_max = DUEL_END - 1;
    menu_index_min = 1;
    break;
  case MENU_SCORES:
    init_bouncies();
    which_menu = MENU_SCORES;
    menu_index = 1;
    menu_index_max = 31;
    menu_index_min = 1;
    break;
  case MENU_ENTER_SCORES:
    init_bouncies();
    which_menu = MENU_ENTER_SCORES;
    menu_index = 1;
    menu_index_max = 31;
    menu_index_min = 1;
    break;
 }

}






void choose_ships(void)
{

 player[0].ship = 0;
 player[1].ship = 0;
 game.single_player = 0;
 game.users = 1;

 int county = 0;

 int keypress_wait1 = KP_WAIT_SHORT;
 int keypress_wait2 = KP_WAIT_SHORT;
 int waiting = 5;

 int p1_active = 0;
 int p2_active = 0;
 int p1_chosen = 0;
 int p2_chosen = 0;

 int game_type = -1;

 if (game.type == GAME_DUEL)
 {
  p1_active = 1;
  p2_active = 1;
 }

 ticked = 0;

 do
 {
  menu_counter ++;
 
  if (p1_active && !p1_chosen)// && keypress_wait1 == 0)
  {
   if (check_key(player[0].keys [CMD_LEFT]))
   {
    if (player[0].ship == 0)
     player[0].ship = 15;
      else
       player[0].ship --;
    play_sound(WAV_MENU1);
    keypress_wait1 = waiting;
   }
   if (check_key(player[0].keys [CMD_RIGHT]))
   {
    player[0].ship ++;
    if (player[0].ship == 16)
     player[0].ship = 0;
    keypress_wait1 = waiting;
    play_sound(WAV_MENU1);
   }
   if (check_key(player[0].keys [CMD_FIRE1]))
   {
    p1_chosen = 1;
    keypress_wait1 = waiting;
    play_sound(WAV_MENU2);
   }
   if (check_key(player[0].keys [CMD_THRUST]))
   {
    if (player[0].ship < 4)
     player[0].ship += 12;
      else
       player[0].ship -= 4;
    keypress_wait1 = waiting;
   play_sound(WAV_MENU1);
   }
   if (check_key(player[0].keys [CMD_BRAKE]))
   {
    if (player[0].ship > 11)
     player[0].ship -= 12;
      else
       player[0].ship += 4;
    keypress_wait1 = waiting;
    play_sound(WAV_MENU1);
   }
  }
   else
   {
    if (check_key(player[0].keys [CMD_FIRE1]))// && keypress_wait1 == 0)
    {
     p1_active = 1;
     keypress_wait1 = waiting;
     play_sound(WAV_MENU2);
   }
   }
   
  if (p2_active && !p2_chosen)// && keypress_wait2 == 0)
  {
   if (check_key(player[1].keys [CMD_LEFT]))
   {
    if (player[1].ship == 0)
     player[1].ship = 15;
      else
       player[1].ship --;
    keypress_wait2 = waiting;
    play_sound(WAV_MENU1);
   }
   if (check_key(player[1].keys [CMD_RIGHT]))
   {
    player[1].ship ++;
    if (player[1].ship == 16)
     player[1].ship = 0;
    keypress_wait2 = waiting;
    play_sound(WAV_MENU1);
   }
   if (check_key(player[1].keys [CMD_FIRE1]))
   {
    p2_chosen = 1;
    keypress_wait2 = waiting;
    play_sound(WAV_MENU2);
   }
   if (check_key(player[1].keys [CMD_THRUST]))
   {
    if (player[1].ship < 4)
     player[1].ship += 12;
      else
       player[1].ship -= 4;
    keypress_wait2 = waiting;
    play_sound(WAV_MENU1);
   }
   if (check_key(player[1].keys [CMD_BRAKE]))
   {
    if (player[1].ship > 11)
     player[1].ship -= 12;
      else
       player[1].ship += 4;
    keypress_wait2 = waiting;
    play_sound(WAV_MENU1);
   }
  }
   else
   {
    if (check_key(player[1].keys [CMD_FIRE1]))// && keypress_wait2 == 0)
    {
     p2_active = 1;
     keypress_wait2 = waiting;
     play_sound(WAV_MENU2);
    }
   }

  if (keypress_wait1 > 0)
   keypress_wait1 --;
  if (keypress_wait2 > 0)
   keypress_wait2 --;

  make_grid_scroll();

  if (ticked == 0)
  {
   display_ship_choice(0, p1_active, p2_active, p1_chosen, p2_chosen);
   blit(menu_bmp, screen, 0, 0, 0, 0, 640, 480);
  }

  do
  {
   county ++;
  } while (ticked == 0);

  ticked --;

  if (p1_chosen == 1 && p2_active == 0)
  {
   game_type = GAME_SINGLE;
   game.single_player = 0;
  }
  if (p2_chosen == 1 && p1_active == 0)
  {
   game_type = GAME_SINGLE;
   game.single_player = 1;
  }
  if (p1_chosen == 1 && p2_chosen == 1)
  {
   game_type = GAME_COOP;
   if (game.type == GAME_DUEL)
    game_type = GAME_DUEL;
  }

  if (game_type != -1)
  {
   if (player[0].ship == 15)
    player[0].ship = grand(15);
   if (player[1].ship == 15)
    player[1].ship = grand(15);
   start_game_from_menu(game_type);
   return;
  }

  if (check_key(KEY_ESC))
  {
   keypress_wait = KP_WAIT_LONG;
   last_key = KEY_ESC;
//   rest(200);
   game.type = GAME_SINGLE;
   play_sound(WAV_MENU2);
   return;
  }
 }
  while(TRUE);
}


void display_ship_choice(int players, int p1a, int p2a, int p1c, int p2c)
{

 clear_bitmap(menu_bmp);

  show_grid(COLOUR_GREEN3, COLOUR_GREEN8);

// int i, j;

 if (!p1a)
 {
  textprintf_centre(menu_bmp, font, 320, 120, COLOUR_WHITE - (menu_counter / 4) % 8, "Player} One} Fire} To} Start}");
  textprintf_centre(menu_bmp, font2, 320, 120, COLOUR_GREEN5 + (menu_counter / 8) % 4, "Player} One} Fire} To} Start}");
 }
  else
  {
   display_ship_choice2(player [0].ship, 0, p1c);
  }

 if (!p2a)
 {
  textprintf_centre(menu_bmp, font, 320, 360, COLOUR_WHITE - (menu_counter / 4) % 8, "Player} Two} Fire} To} Start}");
  textprintf_centre(menu_bmp, font2, 320, 360, COLOUR_GREEN5 + (menu_counter / 8) % 4, "Player} Two} Fire} To} Start}");
 }
  else
  {
   display_ship_choice2(player [1].ship, 225, p2c);
  }

 
}


void display_ship_choice2(int ship, int y, int chosen)
{
 int i,j;

 int bx, by;
 int col, sprite_drawn = 0;

//  show_grid(COLOUR_GREEN3, COLOUR_GREEN8);

 for (i = 0; i < 4; i ++)
 {
  for (j = 0; j < 4; j ++)
  {
    bx = 20 + i * 54;
    by = y + 20 + j * 54;
    col = COLOUR_BLUE2;
    sprite_drawn = RLE_LSHIP_BLUE;
    if (i + j * 4 == ship)
    {
     col = COLOUR_RED3;
     sprite_drawn = RLE_LSHIP_YELLOW;
     if (chosen)
     {
      col = COLOUR_PURPLE3;
      sprite_drawn = RLE_LSHIP_RED;
     }
    }
    rectfill(menu_bmp, bx, by, bx + 49, by + 49, col);
//    if (i + j * 3 < 11)
    draw_rle_sprite(menu_bmp, large_ships [i + j * 4] [sprite_drawn], bx, by);
  }
 }

 if (chosen)
 {
  textprintf_centre(menu_bmp, font, 436, y + 20, COLOUR_YELLOW8 - (menu_counter / 2) % 4, ship_name_long(ship));
  textprintf_centre(menu_bmp, font2, 436, y + 20, COLOUR_ORANGE4 + (menu_counter / 4) % 4, ship_name_long(ship));
 }
  else
  {
   textprintf_centre(menu_bmp, font, 436, y + 20, COLOUR_GREEN8 - (menu_counter / 2) % 4, ship_name_long(ship));
   textprintf_centre(menu_bmp, font2, 436, y + 20, COLOUR_BLUE4 + (menu_counter / 4) % 4, ship_name_long(ship));
  }
// textprintf(menu_bmp, font2, 280, y + 20, COLOUR_WHITE, "%i", ship);

   textprintf_centre(menu_bmp, small_font, 436, y + 60, COLOUR_YELLOW7, ship_description1(ship));
   textprintf_centre(menu_bmp, small_font, 436, y + 70, COLOUR_YELLOW7, ship_description2(ship));

 int line_colour = 255;
 int box_colour = 255;

 RGB lcolour [1];
 int mcn = menu_counter * 2;

 if (mcn % 128 < 64)
 {
  lcolour[0].r = 8 + (63 - mcn % 64) / 7;
//  lcolour[0].g = (63 - mcn % 64);
  lcolour[0].g = 8 + (63 - mcn % 64) / 4;
  lcolour[0].b = 0;//((mcn + 32) % 64);// / 2;
 }
  else
  {
   lcolour[0].r = 8 + (mcn % 64) / 7;
//   lcolour[0].g = (mcn % 64);
   lcolour[0].g = 8 + (mcn % 64) / 4;
   lcolour[0].b = 0;//(63 - ((mcn + 32) % 64));// / 2;
  }
  
// vsync(); MAY NEED TO REPLACE THIS!!!

 set_color(255, lcolour);

 if (ship != 15)
 {
  line(menu_bmp, 431, y + 110, 431, y + 125, line_colour);
  line(menu_bmp, 439, y + 110, 439, y + 125, line_colour);
  line(menu_bmp, 431, y + 125, 439, y + 125, line_colour);
  line(menu_bmp, 435, y + 125, 435, y + 135, line_colour);
  line(menu_bmp, 435, y + 135, 250, y + 135, line_colour);
  line(menu_bmp, 250, y + 135, 250, y + 160, line_colour);
  line(menu_bmp, 250, y + 160, 615, y + 160, line_colour);
  line(menu_bmp, 615, y + 160, 615, y + 185, line_colour);
  line(menu_bmp, 615, y + 185, 250, y + 185, line_colour);
  line(menu_bmp, 250, y + 185, 250, y + 210, line_colour);
  line(menu_bmp, 250, y + 210, 590, y + 210, line_colour);

  rectfill(menu_bmp, 397, y + 103, 472, y + 117, box_colour);
  draw_sprite(menu_bmp, upgrade_box3, 395, y + 93);

  for (i = 0; i < 4; i ++)
  {
   rectfill(menu_bmp, 263 + i * 90, y + 153, 336 + i * 90, y + 168, box_colour);
   rectfill(menu_bmp, 263 + i * 90, y + 203, 336 + i * 90, y + 218, box_colour);
   draw_sprite(menu_bmp, upgrade_box1, 260 + i * 90, y + 143);
   if (i == 3)
    draw_sprite(menu_bmp, upgrade_box2, 260 + i * 90, y + 193);
     else
      draw_sprite(menu_bmp, upgrade_box1, 260 + i * 90, y + 193);
  }

  textprintf_centre(menu_bmp, small_font, 326 + 110, y + 104, COLOUR_YELLOW8, "Upgrades");

  for (i = 0; i < 4; i ++)
  {
   textprintf_centre(menu_bmp, small_font, 300 + i * 90, y + 155, COLOUR_YELLOW7, upgrade_name_long(upgrades [ship] [i + 1]));
   textprintf_centre(menu_bmp, small_font, 300 + i * 90, y + 205, COLOUR_YELLOW7, upgrade_name_long(upgrades [ship] [i + 5]));
  }
/*  line(menu_bmp, 396, y + 110, 396, y + 125, line_colour);
  line(menu_bmp, 404, y + 110, 404, y + 125, line_colour);
  line(menu_bmp, 396, y + 125, 404, y + 125, line_colour);
  line(menu_bmp, 400, y + 125, 400, y + 135, line_colour);
  line(menu_bmp, 400, y + 135, 200, y + 135, line_colour);
  line(menu_bmp, 200, y + 135, 200, y + 160, line_colour);
  line(menu_bmp, 200, y + 160, 600, y + 160, line_colour);
  line(menu_bmp, 600, y + 160, 600, y + 185, line_colour);
  line(menu_bmp, 600, y + 185, 200, y + 185, line_colour);
  line(menu_bmp, 200, y + 185, 200, y + 210, line_colour);
  line(menu_bmp, 200, y + 210, 580, y + 210, line_colour);

  rectfill(menu_bmp, 362, y + 103, 437, y + 117, box_colour);
  draw_sprite(menu_bmp, upgrade_box3, 360, y + 93);

  for (i = 0; i < 4; i ++)
  {
   rectfill(menu_bmp, 213 + i * 100, y + 153, 286 + i * 100, y + 168, box_colour);
   rectfill(menu_bmp, 213 + i * 100, y + 203, 286 + i * 100, y + 218, box_colour);
   draw_sprite(menu_bmp, upgrade_box1, 210 + i * 100, y + 143);
   if (i == 3)
    draw_sprite(menu_bmp, upgrade_box2, 210 + i * 100, y + 193);
     else
      draw_sprite(menu_bmp, upgrade_box1, 210 + i * 100, y + 193);
  }

  textprintf_centre(menu_bmp, small_font, 281 + 120, y + 104, COLOUR_YELLOW8, "Upgrades");

  for (i = 0; i < 4; i ++)
  {
   textprintf_centre(menu_bmp, small_font, 250 + i * 100, y + 155, COLOUR_YELLOW7, upgrade_name_long(upgrades [ship] [i + 1]));
   textprintf_centre(menu_bmp, small_font, 250 + i * 100, y + 205, COLOUR_YELLOW7, upgrade_name_long(upgrades [ship] [i + 5]));
  }
 */
 }
 
}


void show_grid(int grid_colour, int border_colour)
{

  rectfill(menu_bmp, 10, 10, 630, 470, COLOUR_GREY1);
  rect(menu_bmp, 10, 10, 630, 470, border_colour - 4);
  rect(menu_bmp, 9, 9, 631, 471, border_colour - 2);
  rect(menu_bmp, 8, 8, 632, 472, border_colour);

  draw_scrolling_grid(11, 11, 629, 469, grid_colour);

}


void menu_display_scores(void)
{

/*  rectfill(menu_bmp, 10, 50, 540, 400, COLOUR_GREEN1);
  rect(menu_bmp, 10, 50, 540, 400, COLOUR_GREEN4);
  rect(menu_bmp, 9, 49, 541, 401, COLOUR_GREEN6);
  rect(menu_bmp, 8, 48, 542, 402, COLOUR_GREEN8);*/

  show_grid(COLOUR_YELLOW3, COLOUR_YELLOW8);
  draw_bouncies();


  textprintf_centre(menu_bmp, font, 320, 20, COLOUR_YELLOW4 + (menu_counter / 2) % 4, "High} Scores}");
  textprintf_centre(menu_bmp, font2, 320, 20, COLOUR_ORANGE8 - (menu_counter / 2) % 4, "High} Scores}");



  int i;

  int ox = 205, oy = 10, oys = 100;
  int col;

  textprintf_centre(menu_bmp, small_font, 320, oys - 30, COLOUR_ORANGE8, "Single Player");

  textprintf(menu_bmp, small_font, ox, oys - 10, COLOUR_YELLOW8, "Name");
  textprintf_right(menu_bmp, small_font, ox + 90, oys - 10, COLOUR_YELLOW8, "Level");
  textprintf_right(menu_bmp, small_font, ox + 190, oys - 10, COLOUR_YELLOW8, "Craft");
  textprintf_right(menu_bmp, small_font, ox + 240, oys - 10, COLOUR_YELLOW8, "Score");


  char sstr [50];
  char istr [20];

  for (i = 0; i < NO_SCORES; i ++)
  {

   col = COLOUR_GREEN5;

   if (which_menu == MENU_ENTER_SCORES)
    col = COLOUR_GREEN3;

   if (game.type == GAME_SINGLE && i == entering_score && which_menu == MENU_ENTER_SCORES)
    col = COLOUR_YELLOW4 + (menu_counter / 4) % 4;

   strcpy(sstr, hs_single[i].name);
   textprintf(menu_bmp, small_font, ox, oys + oy * i, col, sstr);

   if (hs_single[i].level == 17)
    strcpy(sstr, "<V>");
     else
      strcpy(sstr, itoa(hs_single[i].level, istr, 10));
   textprintf_right(menu_bmp, small_font, ox + 90, oys + oy * i, col, sstr);

   strcpy(sstr, ship_name_short(hs_single[i].ship));
   textprintf_right(menu_bmp, small_font, ox + 190, oys + oy * i, col, sstr);

   strcpy(sstr, itoa(hs_single[i].score, istr, 10));
   textprintf_right(menu_bmp, small_font, ox + 240, oys + oy * i, col, sstr);

  }

  oys = 300;

  textprintf_centre(menu_bmp, small_font, 320, oys - 30, COLOUR_ORANGE8, "Cooperative - best player only");

  textprintf(menu_bmp, small_font, ox, oys - 10, COLOUR_YELLOW8, "Name");
  textprintf_right(menu_bmp, small_font, ox + 90, oys - 10, COLOUR_YELLOW8, "Level");
  textprintf_right(menu_bmp, small_font, ox + 190, oys - 10, COLOUR_YELLOW8, "Craft");
  textprintf_right(menu_bmp, small_font, ox + 240, oys - 10, COLOUR_YELLOW8, "Score");

  for (i = 0; i < NO_SCORES; i ++)
  {

   col = COLOUR_GREEN5;

   if (which_menu == MENU_ENTER_SCORES)
    col = COLOUR_GREEN3;

   if (game.type == GAME_COOP && i == entering_score && which_menu == MENU_ENTER_SCORES)
    col = COLOUR_YELLOW4 + (menu_counter / 2) % 4;

   strcpy(sstr, hs_coop[i].name);
   textprintf(menu_bmp, small_font, ox, oys + oy * i, col, sstr);

   if (hs_coop[i].level == 17)
    strcpy(sstr, "<V>");
     else
      strcpy(sstr, itoa(hs_coop[i].level, istr, 10));
   textprintf_right(menu_bmp, small_font, ox + 90, oys + oy * i, col, sstr);

   strcpy(sstr, ship_name_short(hs_coop[i].ship));
   textprintf_right(menu_bmp, small_font, ox + 190, oys + oy * i, col, sstr);

   strcpy(sstr, itoa(hs_coop[i].score, istr, 10));
   textprintf_right(menu_bmp, small_font, ox + 240, oys + oy * i, col, sstr);

  }



/*
  textprintf_centre(menu_bmp, font, 300, 200, COLOUR_YELLOW8 - (menu_counter / 4) % 7, "L___a___c___e___w___i___n___g}");
  textprintf_centre(menu_bmp, font2, 300, 200, COLOUR_RED1 + (menu_counter / 4) % 7, "L___a___c___e___w___i___n___g}");
  textprintf_centre(menu_bmp, font, 300, 300, COLOUR_YELLOW7, "Space} For} 1P}  Enter} For} 2P}...");
  textprintf_centre(menu_bmp, font2, 300, 300, COLOUR_RED7, "Space} For} 1P}  Enter} For} 2P}...");
*/
}



void draw_scrolling_grid(int min_x, int min_y, int max_x, int max_y, int colour)
{

 int i, j, x1, y1;

 int x_offset = (grid_x / GRAIN) % 50;
 int y_offset = (grid_y / GRAIN) % 50;

 for (i = 0; i < 15; i ++)
 {
   x1 = i * 50 + x_offset;
   if (x1 < min_x || x1 > max_x)
    continue;
   vline(menu_bmp, x1, min_y, max_y, colour);
 }
 for (j = 0; j < 12; j ++)
 {
   y1 = j * 50 + y_offset;
   if (y1 < min_y || y1 > max_y)
    continue;
   hline(menu_bmp, min_x, y1, max_x, colour);
 }

}


void make_grid_scroll(void)
{

 grid_x += grid_x_speed;
 grid_y += grid_y_speed;

 grid_x %= 50000;
 grid_y %= 50000;

 grid_x_speed += grid_x_accel;
 if (grid_x_speed > 3000) grid_x_speed = 3000;
 if (grid_x_speed < -3000) grid_x_speed = -3000;
 grid_y_speed += grid_y_accel;
  if (grid_y_speed > 3000) grid_y_speed = 3000;
  if (grid_y_speed < -3000) grid_y_speed = -3000;

 if (menu_counter % 32 == 0)
 {
  grid_x_accel = grand(301) - 150;
//  if (grid_x_speed > 3000) grid_x_speed = 3000;
//  if (grid_x_speed < -3000) grid_x_speed = -3000;

  grid_y_accel = grand(301) - 150;
//  if (grid_y_speed > 3000) grid_y_speed = 3000;
//  if (grid_y_speed < -3000) grid_y_speed = -3000;
 }

}

void init_bouncies(void)
{
 int i;

 for (i = 0; i < NO_BOUNCIES; i ++)
 {

  bouncy[i].x = (grand(500) + 20) * GRAIN;
  bouncy[i].y = (grand(400) + 20) * GRAIN;
  bouncy[i].x_speed = (900 + grand(800));
  if (grand(2) == 0)
   bouncy[i].x_speed *= -1;
  bouncy[i].y_speed = (900 + grand(800));
  if (grand(2) == 0)
   bouncy[i].y_speed *= -1;
  bouncy[i].colour1 = COLOUR_GREEN1;
  bouncy[i].colour2 = COLOUR_GREEN6;
  if (grand(25) == 0)
  {
   bouncy[i].colour1 = COLOUR_GREY1;
   bouncy[i].colour2 = COLOUR_GREY6;
  }
  if (grand(15) == 0)
  {
   bouncy[i].colour1 = COLOUR_PURPLE1;
   bouncy[i].colour2 = COLOUR_PURPLE6;
  }
  if (grand(8) == 0)
  {
   bouncy[i].colour1 = COLOUR_BLUE1;
   bouncy[i].colour2 = COLOUR_BLUE6;
  }
  if (grand(5) == 0)
  {
   bouncy[i].colour1 = COLOUR_RED1;
   bouncy[i].colour2 = COLOUR_RED6;
  }
  if (grand(5) == 0)
  {
   bouncy[i].colour1 = COLOUR_ORANGE1;
   bouncy[i].colour2 = COLOUR_ORANGE6;
  }
  if (grand(5) == 0)
  {
   bouncy[i].colour1 = COLOUR_YELLOW1;
   bouncy[i].colour2 = COLOUR_YELLOW6;
  }
 }


}

void make_bouncies_move(void)
{
 int i;
// int j;

 for (i = 0; i < NO_BOUNCIES; i ++)
 {
  if (bouncy[i].x_speed == 0 && bouncy[i].y_speed == 0) return;

  if (bouncy[i].x + bouncy[i].x_speed <= 18 * GRAIN
      || bouncy[i].x + bouncy[i].x_speed >= 622 * GRAIN)
  {
   bouncy[i].x_speed *= -1;
  }
  if (bouncy[i].y + bouncy[i].y_speed <= 18 * GRAIN
      || bouncy[i].y + bouncy[i].y_speed >= 462 * GRAIN)
  {
   bouncy[i].y_speed *= -1;
  }


  bouncy[i].x += bouncy[i].x_speed;
  bouncy[i].y += bouncy[i].y_speed;

 }

}


void draw_bouncies(void)
{

 int i;

 for (i = 0; i < NO_BOUNCIES; i ++)
 {
  circlefill(menu_bmp, bouncy[i].x / GRAIN, bouncy[i].y / GRAIN, 7, bouncy[i].colour1);
  circle(menu_bmp, bouncy[i].x / GRAIN, bouncy[i].y / GRAIN, 7, bouncy[i].colour2);
 }

}




void check_high_scores(void)
{

 int i;
 int entry = -1;
 int better_player = 0;

 if (game.type == GAME_SINGLE)
 {
  for (i = 0; i < NO_SCORES; i ++)
  {
   if (player[game.single_player].score > hs_single[i].score)
   {
    entry = i;
    break;
   }
  }
  if (entry != -1)
  {
   push_scores_single(entry, game.single_player);
   entering_score = entry;
//   exit(88);
//    change_menu(MENU_SCORES);
//   congratulations(-1);
   change_menu(MENU_ENTER_SCORES);
  }
   else
    change_menu(MENU_SCORES);
 }

 if (game.type == GAME_COOP)
 {
  if (player[0].score > player[1].score) better_player = 0;
   else better_player = 1;
 
  for (i = 0; i < NO_SCORES; i ++)
  {
   if (player[better_player].score > hs_coop[i].score)
   {
    entry = i;
    break;
   }
  }
  for (i = 0; i < NO_SCORES; i ++)
  {
   if (player[better_player].score > hs_coop[i].score)
   {
    entry = i;
    break;
   }
  }
  if (entry != -1)
  {
   push_scores_coop(entry, better_player);
   entering_score = entry;
//   exit(88);
//    change_menu(MENU_SCORES);
//   entering_score2 = better_player;
   change_menu(MENU_ENTER_SCORES);
  }
   else
    change_menu(MENU_SCORES);

/* if (game.type == GAME_COOP)
 {
  for (i = 0; i < NO_SCORES; i ++)
  {
   if (player[0].score > hs_single[i].score)
   {
    entry = i;
    break;
   }
  }
  for (i = 0; i < NO_SCORES; i ++)
  {
   if (player[1].score > hs_single[i].score)
   {
    entry = i;
    break;
   }
  }
  if (entry != -1 || entry2 != -1)
  {
   push_scores_coop(entry, game.single_player);
   entering_score = entry;
//   exit(88);
//    change_menu(MENU_SCORES);
   change_menu(MENU_ENTER_SCORES);
  }
   else
    change_menu(MENU_SCORES);
*/

 }


}


void congratulations(int player)
{

 rectfill(screen, 140, 150, 500, 280, COLOUR_ORANGE1);
 rect(screen, 140, 150, 500, 280, COLOUR_ORANGE2);
 rect(screen, 139, 149, 501, 281, COLOUR_ORANGE3);
 rect(screen, 138, 148, 502, 282, COLOUR_ORANGE4);
 rect(screen, 137, 147, 503, 283, COLOUR_ORANGE5);
 rect(screen, 136, 146, 504, 284, COLOUR_ORANGE6);




 if (player == -1)
 {
  textprintf_centre(screen, font, 320, 180, COLOUR_YELLOW8, "Congratulations}!");
  textprintf_centre(screen, font2, 320, 180, COLOUR_ORANGE6, "Congratulations}!");
  textprintf_centre(screen, font, 320, 230, COLOUR_YELLOW6, "You} Got} A} High} Score}!");
  textprintf_centre(screen, font2, 320, 230, COLOUR_ORANGE8, "You} Got} A} High} Score}!");
// textprintf_right(screen, small_font, 500, 240, COLOUR_WHITE, "Press a key...");
 }
   else
   {
    textprintf_centre(screen, font, 320, 180, COLOUR_YELLOW8, "Congratulations}!");
    textprintf_centre(screen, font2, 320, 180, COLOUR_ORANGE6, "Congratulations}!");
    textprintf_centre(screen, font, 320, 230, COLOUR_YELLOW6, "Player} %i Got} A} High} Score}!", player + 1);
    textprintf_centre(screen, font2, 320, 230, COLOUR_ORANGE8, "Player} %i Got} A} High} Score}!", player + 1);
   }

 rest(200);

 rect(screen, 140, 150, 500, 280, COLOUR_YELLOW2);
 rect(screen, 139, 149, 501, 281, COLOUR_YELLOW3);
 rect(screen, 138, 148, 502, 282, COLOUR_YELLOW4);
 rect(screen, 137, 147, 503, 283, COLOUR_YELLOW5);
 rect(screen, 136, 146, 504, 284, COLOUR_YELLOW6);

 clear_keybuf();

 readkey();

 keypress_wait = KP_WAIT_SHORT;

}

void push_scores_single(int insert, int play)
{

 int i;

 for (i = NO_SCORES - 2; i > insert - 1; i --)
 {
  hs_single[i + 1].score = hs_single[i].score;
  hs_single[i + 1].level = hs_single[i].level;
  hs_single[i + 1].ship = hs_single[i].ship;
  strcpy(hs_single[i + 1].name, hs_single[i].name);
 }

 i ++;

 hs_single[i].score = player[play].score;
 hs_single[i].level = arena.level;
 hs_single[i].ship = actor[player[play].actor_controlled].ship;
 strcpy(hs_single[i].name, "_");

}

// my attempt to use one push_scores function with pointers failed.
void push_scores_coop(int insert, int play)
{

 int i;

 for (i = NO_SCORES - 2; i > insert - 1; i --)
 {
  hs_coop[i + 1].score = hs_coop[i].score;
  hs_coop[i + 1].level = hs_coop[i].level;
  hs_coop[i + 1].ship = hs_coop[i].ship;
  strcpy(hs_coop[i + 1].name, hs_coop[i].name);
 }

 i ++;

 char istr [10];

 hs_coop[i].score = player[play].score;
 hs_coop[i].level = arena.level;
 hs_coop[i].ship = actor[player[play].actor_controlled].ship;
 strcpy(hs_coop[i].name, "P");
 strcat(hs_coop[i].name, itoa(play + 1, istr, 10));
 strcat(hs_coop[i].name, "_");

}



int enter_score_name_single(int s)
{
/* rectfill(screen, 400, 210, 550, 280, COLOUR_GREY2);
 rect(screen, 400, 210, 550, 280, COLOUR_GREY6);
 rect(screen, 401, 211, 549, 279, COLOUR_GREY5);
 rect(screen, 402, 212, 548, 278, COLOUR_GREY4);
 rect(screen, 403, 213, 547, 277, COLOUR_GREY3);
 textprintf_right(screen, small_font, 500, 240, COLOUR_WHITE, "Press a key...");
*/


// clear_keybuf();

 char inputted; // = KEY_ESC;
 int shifter = key [KEY_LSHIFT] + key [KEY_RSHIFT];

// int i;

// rest(200); // to clear the enter or space key

// if (keypressed() != TRUE)
//  return 0;

 int i;

  for (i = KEY_A; i < KEY_CAPSLOCK + 1; i ++)
  {
   if (check_key(i))
   {
    break;
   }
   if (i == KEY_CAPSLOCK)
    return 0;
  }

 if (i == KEY_ESC || i == KEY_ENTER || i == KEY_ENTER_PAD) // escape, enter
 {
  hs_single[s].name [strlen(hs_single[s].name) - 1] = '\0'; // remove the _
  keypress_wait = KP_WAIT_SHORT;
  play_sound(WAV_MENU2);
  return 1;
 }

 if (i == KEY_BACKSPACE) // backspace
 {
  if (strlen(hs_single[s].name) > 1)
  {
   hs_single[s].name [strlen(hs_single[s].name) - 2] = '\0';
   strcat(hs_single[s].name, "_");
   keypress_wait = KP_WAIT_SHORT;
   play_sound(WAV_MENU1);
   return 0;
  }
 }

// inputted = readkey() & 0xff;

 inputted = scancode_to_asc(i);

 if (inputted == '\0')
  return 0;

// if (acceptable_name_char(inputted) == 0)
//  return 0;

 if (strlen(hs_single[s].name) > SCORE_LENGTH || text_length(small_font, hs_single[s].name) > 75)
  return 0;

 if (shifter != 0)
 {
  if (inputted >= 'a' && inputted <= 'z')
   inputted = 'A' + inputted - 'a';
  if (inputted >= '0' && inputted <= '9')
   inputted = get_shifted_number(inputted);
 }

 int slen = strlen(hs_single[s].name);

 hs_single[s].name [slen - 1] = inputted;
 hs_single[s].name [slen] = '\0';

 strcat(hs_single[s].name, "_");

 keypress_wait = KP_WAIT_SHORT;
 play_sound(WAV_MENU1);

 return 0;
 
}




int enter_score_name_coop(int s)
{

 char inputted;
 int shifter = key [KEY_LSHIFT] + key [KEY_RSHIFT];

 int i;

  for (i = KEY_A; i < KEY_CAPSLOCK + 1; i ++)
  {
   if (check_key(i))
   {
    break;
   }
   if (i == KEY_CAPSLOCK)
    return 0;
  }

 if (i == KEY_ESC || i == KEY_ENTER || i == KEY_ENTER_PAD) // escape, enter
 {
  hs_coop[s].name [strlen(hs_coop[s].name) - 1] = '\0'; // remove the _
  keypress_wait = KP_WAIT_LONG;
  play_sound(WAV_MENU2);
  return 1;
 }

 if (i == KEY_BACKSPACE) // backspace
 {
  if (strlen(hs_coop[s].name) > 1)
  {
   hs_coop[s].name [strlen(hs_coop[s].name) - 2] = '\0';
   strcat(hs_coop[s].name, "_");
   keypress_wait = KP_WAIT_SHORT;
   play_sound(WAV_MENU1);
   return 0;
  }
 }

 inputted = scancode_to_asc(i);

 if (inputted == '\0')
  return 0;

 if (strlen(hs_coop[s].name) > SCORE_LENGTH || text_length(small_font, hs_coop[s].name) > 75)
  return 0;

 if (shifter != 0)
 {
  if (inputted >= 'a' && inputted <= 'z')
   inputted = 'A' + inputted - 'a';
  if (inputted >= '0' && inputted <= '9')
   inputted = get_shifted_number(inputted);
//  if (inputted >= '0' && inputted <= '9')
//   inputted -= 15;
 }

 int slen = strlen(hs_coop[s].name);

 hs_coop[s].name [slen - 1] = inputted;
 hs_coop[s].name [slen] = '\0';

 strcat(hs_coop[s].name, "_");

 keypress_wait = KP_WAIT_SHORT;
 play_sound(WAV_MENU1);

 return 0;
 
}









void quit_game(void)
{

      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
//      clrscr();
      allegro_message("\n\rBye bye!                           ");
//      free(palet);
      exit(0);

}

char get_shifted_number(char inputted)
{

  switch(inputted)
  {
   case '1': return '!';
   case '2': return '@';
   case '3': return '#';
   case '4': return '$';
   case '5': return '$';
   case '6': return '^';
   case '7': return '&';
   case '8': return '*';
   case '9': return '(';
   case '0': return ')';
  }

  return inputted;

}

void jam_keys(void)
{

 do
 {

  rectfill(menu_bmp, 180, 50, 440, 400, COLOUR_GREEN1);
  rect(menu_bmp, 180, 50, 440, 400, COLOUR_GREEN4);
  rect(menu_bmp, 179, 49, 441, 401, COLOUR_GREEN6);
  rect(menu_bmp, 178, 48, 442, 402, COLOUR_GREEN8);

  textprintf_centre(menu_bmp, font, 320, 90, COLOUR_ORANGE8, "T_e_s_t}");
  textprintf_centre(menu_bmp, font2, 320, 90, COLOUR_YELLOW8, "T_e_s_t}");
  textprintf_centre(menu_bmp, font, 320, 130, COLOUR_ORANGE8, "K_e_y_s}");
  textprintf_centre(menu_bmp, font2, 320, 130, COLOUR_YELLOW8, "K_e_y_s}");

  textprintf(menu_bmp, small_font, 200, 210, COLOUR_YELLOW8, "Most keyboards refuse to register certain");
  textprintf(menu_bmp, small_font, 200, 220, COLOUR_YELLOW8, "combinations of keys.  Try pressing the keys you want");
  textprintf(menu_bmp, small_font, 200, 230, COLOUR_YELLOW8, "to use to test for conflicts.  Press Escape to exit.");

  int i;
  int row = 0;

  for (i = 0; i < KEY_CAPSLOCK + 1; i ++)
  {
   if (key [i] != 0)
   {
    textprintf(menu_bmp, small_font, 200, 250 + row * 10, COLOUR_ORANGE8, scancode_to_keyname(i));
    row ++;
   }
  }

  blit(menu_bmp, screen, 0, 0, 0, 0, 640, 480);
 }
  while(!key [KEY_ESC]);

 keypress_wait = KP_WAIT_LONG;
 last_key = KEY_ESC;

 ticked = 0;

}




int acceptable_char(int scode)
{
// if (ascode >= 32 && ascode <= 126) return 1;
 switch(scode)
 {
  case KEY_ESC:
   keypress_wait = KP_WAIT_SHORT;
   return 0;
 }
 return 1;
}

int acceptable_name_char(int ascode)
{
 if (ascode >= 32 && ascode <= 126) return 1;

 return 0;
}


char *scancode_to_keyname(int scanc)
{

switch(scanc)
{
case KEY_A: return " a";
case KEY_B: return " b";
case KEY_C: return " c";
case KEY_D: return " d";
case KEY_E: return " e";
case KEY_F: return " f";
case KEY_G: return " g";
case KEY_H: return " h";
case KEY_I: return " i";
case KEY_J: return " j";
case KEY_K: return " k";
case KEY_L: return " l";
case KEY_M: return " m";
case KEY_N: return " n";
case KEY_O: return " o";
case KEY_P: return " p";
case KEY_Q: return " q";
case KEY_R: return " r";
case KEY_S: return " s";
case KEY_T: return " t";
case KEY_U: return " u";
case KEY_V: return " v";
case KEY_W: return " w";
case KEY_X: return " x";
case KEY_Y: return " y";
case KEY_Z: return " z";
case KEY_0: return " 0";
case KEY_1: return " 1";
case KEY_2: return " 2";
case KEY_3: return " 3";
case KEY_4: return " 4";
case KEY_5: return " 5";
case KEY_6: return " 6";
case KEY_7: return " 7";
case KEY_8: return " 8";
case KEY_9: return " 9";
case KEY_0_PAD: return " Numpad Ins";
case KEY_1_PAD: return " Numpad 1";
case KEY_2_PAD: return " Numpad 2";
case KEY_3_PAD: return " Numpad 3";
case KEY_4_PAD: return " Numpad 4";
case KEY_5_PAD: return " Numpad 5";
case KEY_6_PAD: return " Numpad 6";
case KEY_7_PAD: return " Numpad 7";
case KEY_8_PAD: return " Numpad 8";
case KEY_9_PAD: return " Numpad 9";
case KEY_F1: return " F1";
case KEY_F2: return " F2";
case KEY_F3: return " F3";
case KEY_F4: return " F4";
case KEY_F5: return " F5";
case KEY_F6: return " F6";
case KEY_F7: return " F7";
case KEY_F8: return " F8";
case KEY_F9: return " F9";
case KEY_F10: return " F10";
case KEY_F11: return " F11";
case KEY_F12: return " F12";
case KEY_ESC: return " Escape"; // invalid!
case KEY_TILDE: return " `";
case KEY_MINUS: return " -";
case KEY_EQUALS: return " =";
case KEY_BACKSPACE: return " Backspace";
case KEY_TAB: return " Tab";
case KEY_OPENBRACE: return " {";
case KEY_CLOSEBRACE: return " }";
case KEY_ENTER: return " Enter";
case KEY_COLON: return " ;";
case KEY_QUOTE: return " '";
case KEY_BACKSLASH: return " \\";
case KEY_BACKSLASH2: return " \\"; // ????
case KEY_COMMA: return " ,";
case KEY_STOP: return " .";
case KEY_SLASH: return " /";
case KEY_SPACE: return " Space";
case KEY_INSERT: return " Insert";
case KEY_DEL: return " Delete";
case KEY_HOME: return " Home";
case KEY_END: return " End";
case KEY_PGUP: return " Page Up";
case KEY_PGDN: return " Page Down";
case KEY_LEFT: return " Left";
case KEY_RIGHT: return " Right";
case KEY_UP: return " Up";
case KEY_DOWN: return " Down";
case KEY_SLASH_PAD: return " Keypad /";
case KEY_ASTERISK: return " Keypad *";
case KEY_MINUS_PAD: return " Keypad -";
case KEY_PLUS_PAD: return " Keypad +";
case KEY_DEL_PAD: return " Keypad .";
case KEY_ENTER_PAD: return " Keypad Enter";
case KEY_PRTSCR: return " Print Screen";
case KEY_PAUSE: return " Pause";
case KEY_ABNT_C1: return " Weird Foreign Key"; // may as well put these in
case KEY_YEN: return " Yen";
case KEY_KANA: return " Kana";
case KEY_CONVERT: return " Convert";
case KEY_NOCONVERT: return " NOCONVERT";
case KEY_AT: return " At";
case KEY_CIRCUMFLEX: return " Circumflex";
case KEY_COLON2: return " 2nd Colon";
case KEY_KANJI: return " Kanji";
case KEY_LSHIFT: return " Left Shift";
case KEY_RSHIFT: return " Right Shift";
case KEY_LCONTROL: return " Left Control";
case KEY_RCONTROL: return " Right Control";
case KEY_ALT: return " Left Alt";
case KEY_ALTGR: return " Right Alt";
case KEY_LWIN: return " Left Win";
case KEY_RWIN: return " Right Win";
case KEY_MENU: return " Menu";
case KEY_SCRLOCK: return " Scroll Lock";
case KEY_NUMLOCK: return " Num Lock";
case KEY_CAPSLOCK: return " Caps Lock";
//case KEY_: return " ";

default: return " unknown key";

/*

          KEY_ESC, KEY_TILDE, KEY_MINUS, KEY_EQUALS,
          KEY_BACKSPACE, KEY_TAB, KEY_OPENBRACE, KEY_CLOSEBRACE,
          KEY_ENTER, KEY_COLON, KEY_QUOTE, KEY_BACKSLASH,
          KEY_BACKSLASH2, KEY_COMMA, KEY_STOP, KEY_SLASH,
          KEY_SPACE,

*/

}


}



char scancode_to_asc(int scanc)
{

switch(scanc)
{
case KEY_A: return 'a';
case KEY_B: return 'b';
case KEY_C: return 'c';
case KEY_D: return 'd';
case KEY_E: return 'e';
case KEY_F: return 'f';
case KEY_G: return 'g';
case KEY_H: return 'h';
case KEY_I: return 'i';
case KEY_J: return 'j';
case KEY_K: return 'k';
case KEY_L: return 'l';
case KEY_M: return 'm';
case KEY_N: return 'n';
case KEY_O: return 'o';
case KEY_P: return 'p';
case KEY_Q: return 'q';
case KEY_R: return 'r';
case KEY_S: return 's';
case KEY_T: return 't';
case KEY_U: return 'u';
case KEY_V: return 'v';
case KEY_W: return 'w';
case KEY_X: return 'x';
case KEY_Y: return 'y';
case KEY_Z: return 'z';
case KEY_0: return '0';
case KEY_1: return '1';
case KEY_2: return '2';
case KEY_3: return '3';
case KEY_4: return '4';
case KEY_5: return '5';
case KEY_6: return '6';
case KEY_7: return '7';
case KEY_8: return '8';
case KEY_9: return '9';
case KEY_TILDE: return '`';
case KEY_MINUS: return '-';
case KEY_EQUALS: return '=';
case KEY_OPENBRACE: return '{';
case KEY_CLOSEBRACE: return '}';
case KEY_COLON: return ';';
case KEY_QUOTE: return '\'';
case KEY_BACKSLASH: return '\\';
case KEY_BACKSLASH2: return '\\'; // ????
case KEY_COMMA: return ',';
case KEY_STOP: return '.';
case KEY_SLASH: return '/';
case KEY_SPACE: return ' ';
case KEY_SLASH_PAD: return '/';
case KEY_ASTERISK: return '*';
case KEY_MINUS_PAD: return '-';
case KEY_PLUS_PAD: return '+';

default: return '\0';

/*

          KEY_ESC, KEY_TILDE, KEY_MINUS, KEY_EQUALS,
          KEY_BACKSPACE, KEY_TAB, KEY_OPENBRACE, KEY_CLOSEBRACE,
          KEY_ENTER, KEY_COLON, KEY_QUOTE, KEY_BACKSLASH,
          KEY_BACKSLASH2, KEY_COMMA, KEY_STOP, KEY_SLASH,
          KEY_SPACE,

*/

}

 return '\0';


}

void init_config(void)
{

set_config_file("lacew.cfg");


 char miscstring [40];
 char wstring [40];
 char itstring [40];
 int i;


 options.sound_init = get_config_int("Options", "Sound_init", 1);
 options.sound_mode = get_config_int("Options", "Sound_mode", SOUNDMODE_STEREO);
 options.run_vsync = get_config_int("Options", "Run_vsync", 0);
 options.sound_volume = get_config_int("Options", "Sound_volume", 100);
 options.ambience_volume = get_config_int("Options", "Ambience_volume", 100);

 for (i = 0; i < NO_CMDS; i ++)
 {
  strcpy(wstring, "Player1Keys");
  strcpy(miscstring, "Key");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  player[0].keys [i] = get_config_int(wstring, miscstring, KEY_X);
 }

 for (i = 0; i < NO_CMDS; i ++)
 {
  strcpy(wstring, "Player2Keys");
  strcpy(miscstring, "Key");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  player[1].keys [i] = get_config_int(wstring, miscstring, KEY_SPACE);
 }

 for (i = 0; i < NO_SCORES; i ++)
 {
  strcpy(wstring, "Highscores_single");
  strcpy(miscstring, "Score");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  hs_single[i].score = get_config_int(wstring, miscstring, 100);
  strcpy(miscstring, "Level");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  hs_single[i].level = get_config_int(wstring, miscstring, 1);
  strcpy(miscstring, "Ship");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  hs_single[i].ship = get_config_int(wstring, miscstring, SHIP_LACEWING);
  strcpy(miscstring, "Name");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  strcpy(hs_single[i].name, get_config_string(wstring, miscstring, "CaptainP"));
  strcpy(wstring, "Highscores_Coop");
  strcpy(miscstring, "Score");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  hs_coop[i].score = get_config_int(wstring, miscstring, 100);
  strcpy(miscstring, "Level");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  hs_coop[i].level = get_config_int(wstring, miscstring, 1);
  strcpy(miscstring, "Ship");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  hs_coop[i].ship = get_config_int(wstring, miscstring, SHIP_LACEWING);
  strcpy(miscstring, "Name");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  strcpy(hs_coop[i].name, get_config_string(wstring, miscstring, "CaptainP"));
 }

}

void save_config(void)
{
 char miscstring [40];
 char wstring [40];
 char itstring [40];
 int i;

// options.sound_init = get_config_int("Options", "Sound_init", 1);
 set_config_int("Options", "Sound_mode", options.sound_mode);
 set_config_int("Options", "Run_vsync", options.run_vsync);
 set_config_int("Options", "Sound_volume", options.sound_volume);
 set_config_int("Options", "Ambience_volume", options.ambience_volume);

 for (i = 0; i < NO_CMDS; i ++)
 {
  strcpy(wstring, "Player1Keys");
  strcpy(miscstring, "Key");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  set_config_int(wstring, miscstring, player[0].keys [i]);
 }

 for (i = 0; i < NO_CMDS; i ++)
 {
  strcpy(wstring, "Player2Keys");
  strcpy(miscstring, "Key");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  set_config_int(wstring, miscstring, player[1].keys [i]);
 }


 for (i = 0; i < NO_SCORES; i ++)
 {
  strcpy(wstring, "Highscores_single");
  strcpy(miscstring, "Score");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  set_config_int(wstring, miscstring, hs_single[i].score);
  strcpy(miscstring, "Level");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  set_config_int(wstring, miscstring, hs_single[i].level);
  strcpy(miscstring, "Ship");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  set_config_int(wstring, miscstring, hs_single[i].ship);
  strcpy(miscstring, "Name");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  set_config_string(wstring, miscstring, hs_single[i].name);
  
  strcpy(wstring, "Highscores_Coop");
  strcpy(miscstring, "Score");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  set_config_int(wstring, miscstring, hs_coop[i].score);
  strcpy(miscstring, "Level");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  set_config_int(wstring, miscstring, hs_coop[i].level);
  strcpy(miscstring, "Ship");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  set_config_int(wstring, miscstring, hs_coop[i].ship);
  strcpy(miscstring, "Name");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  set_config_string(wstring, miscstring, hs_coop[i].name);
 }


}


char *ship_name_long(int ship)
{

 switch(ship)
 {
  case SHIP_LACEWING: return "Lacewing}";
  case SHIP_CAPYBARA: return "Capybara}";
  case SHIP_DESPOT: return "Despot}";
  case SHIP_HOOKWORM: return "Hookworm}";
  case SHIP_LENTIL: return "Lentil}";
  case SHIP_PORKUPINE: return "Porkupine}";
  case SHIP_PRONG: return "Prong}";
  case SHIP_SCORPION: return "False} Scorpion}";
  case SHIP_TORTFEASOR: return "Tortfeasor}";
  case SHIP_AETHER: return "Aether} Squid}";
  case SHIP_RUMSFELD: return "Rumsfeld's} Delight}";
  case SHIP_GODBOTHERER: return "Godbotherer}";
  case SHIP_BOTULUS: return "Botulus}";
  case SHIP_SLIDEWINDER: return "Slidewinder}";
  case SHIP_DOOM: return "Doom} Fork}";
  default: return "Random}";
 }
}

char *ship_name_short(int ship)
{

 switch(ship)
 {
  case SHIP_LACEWING: return "Lacewing";
  case SHIP_CAPYBARA: return "Capybara";
  case SHIP_DESPOT: return "Despot";
  case SHIP_HOOKWORM: return "Hookworm";
  case SHIP_LENTIL: return "Lentil";
  case SHIP_PORKUPINE: return "Porkupine";
  case SHIP_PRONG: return "Prong";
  case SHIP_SCORPION: return "False Scorpion";
  case SHIP_TORTFEASOR: return "Tortfeasor";
  case SHIP_AETHER: return "Aether Squid";
  case SHIP_RUMSFELD: return "Rumsfeld's Delight";
  case SHIP_GODBOTHERER: return "Godbotherer";
  case SHIP_BOTULUS: return "Botulus";
  case SHIP_SLIDEWINDER: return "Slidewinder";
  case SHIP_DOOM: return "Doom Fork";
  default: return "Unknown";
 }
}

char *ship_description1(int ship)
{

 switch(ship)
 {
  case SHIP_LACEWING: return "The basic fighter of our fleet.";
  case SHIP_CAPYBARA: return "A bomber named after one of";
  case SHIP_DESPOT: return "A dangerous fighter equipped with homing missiles.";
  case SHIP_HOOKWORM: return "Fires lots of little seeker missiles.";
  case SHIP_LENTIL: return "A heavy bomber with enormous firepower.";
  case SHIP_PORKUPINE: return "A heavy missile launcher.";
  case SHIP_PRONG: return "A pointy fighter.";
  case SHIP_SCORPION: return "The most agile craft we have, with an extra couple";
  case SHIP_TORTFEASOR: return "A medium bomber with slide capability.";
  case SHIP_AETHER: return "An unusual design, but very effective.";
  case SHIP_RUMSFELD: return "Built by military contractors to honour";
  case SHIP_GODBOTHERER: return "A heavy gunship capable";
  case SHIP_BOTULUS: return "A quick missile launcher.";
  case SHIP_SLIDEWINDER: return "A slippery dogfighter.";
  case SHIP_DOOM: return "Equipped with an experimental anti-drag device.";
  default: return "Fly a random craft.";
 }
}

char *ship_description2(int ship)
{

 switch(ship)
 {
  case SHIP_LACEWING: return "It is fast and is equipped with decent firepower.";
  case SHIP_CAPYBARA: return "the Earth's most graceful rodents.";
  case SHIP_DESPOT: return "";
  case SHIP_HOOKWORM: return "";
  case SHIP_LENTIL: return "";
  case SHIP_PORKUPINE: return "";
  case SHIP_PRONG: return "";
  case SHIP_SCORPION: return "of cannons to make up for its lack of missiles or bombs.";
  case SHIP_TORTFEASOR: return "";
  case SHIP_AETHER: return "Smart enemies stay well away from it.";
  case SHIP_RUMSFELD: return "one of the early 21st century's great warmongers.";
  case SHIP_GODBOTHERER: return "of destroying anything it can catch.";
  case SHIP_BOTULUS: return "";
  case SHIP_SLIDEWINDER: return "";
  case SHIP_DOOM: return "Unfortunately this precludes the use of a shield.";
  default: return "";
 }
}


char *upgrade_name_long(int i)
{
 switch (i)
 {
  case UPG_NONE:
   return "none";
  case UPG_SPEED:
   return "Speed";
  case UPG_ARMOUR:
   return "Armour";
  case UPG_POWER:
   return "Cannon Power";
  case UPG_AUTOFIRE:
   return "Autofire";
  case UPG_MULTIFIRE:
   return "Multifire";
  case UPG_PROJECT:
   return "Projector";
  case UPG_WARHEAD:
   return "Warhead";
  case UPG_ROCKET:
   return "Rocket";
  case UPG_TUBES:
   return "Missile Tubes";
  case UPG_SLIDE:
   return "Slide Drive";
  case UPG_SEEKER:
   return "Seeker Missiles";
  case UPG_BOMB:
   return "Bomb Payload";
  case UPG_LAUNCHER:
   return "Bomb Launchers";
  case UPG_LOADER:
   return "Missile Loader";
  case UPG_SPECIAL:
   return "special";
  case UPG_TURRET:
   return "PD Turret";
  case UPG_SIDEKICK:
   return "Sidekicks";
  case UPG_HEAVY:
   return "Heavy Cannon";
  case UPG_ORBITAL:
   return "Orbital";
  case UPG_BACKFIRE:
   return "Backfire";
  case UPG_SHIELD:
   return "Shield";
  case UPG_RETRO:
   return "Retro Drive";
  case UPG_ROAMER:
   return "roam";
  case UPG_MINIMISSILES:
   return "minim";
  case UPG_DRIFT:
   return "Drift";


   
 }

 return "ERROR!";
}



/*
enum
{
SHIP_LACEWING,
SHIP_CAPYBARA,
SHIP_DESPOT,
SHIP_HOOKWORM,
SHIP_LENTIL,
SHIP_PORKUPINE,
SHIP_PRONG,
SHIP_SCORPION,
SHIP_TORTFEASOR,
SHIP_AETHER,
SHIP_RUMSFELD

};
*/






void display_victory_screen(void)
{

 int county = 0;
 int county2 = 0;

 do
 {
  menu_counter ++;
  make_grid_scroll();

  if (ticked == 0)
  {
   clear_bitmap(menu_bmp);
   show_grid(COLOUR_PURPLE3, COLOUR_PURPLE8);
   textprintf_centre(menu_bmp, font, 320, 120, COLOUR_YELLOW8 - (menu_counter / 2) % 8, "Victory}!");
   textprintf_centre(menu_bmp, font2, 320, 120, COLOUR_GREY4 + (menu_counter / 4) % 4, "Victory}!");
   textprintf_centre(menu_bmp, small_font, 320, 200, COLOUR_YELLOW8, "You have defeated all four controlling");
   textprintf_centre(menu_bmp, small_font, 320, 215, COLOUR_YELLOW8, "intellects of the alien invasion force");
   textprintf_centre(menu_bmp, small_font, 320, 230, COLOUR_YELLOW8, "and saved Earth from a terrible threat.");
   textprintf_centre(menu_bmp, small_font, 320, 260, COLOUR_YELLOW8, "Congratulations!");
//   textprintf_centre(menu_bmp, small_font, 320, 290, COLOUR_YELLOW8, "Now you can do something useful with");
//   textprintf_centre(menu_bmp, small_font, 320, 305, COLOUR_YELLOW8, "your time, like ");
   if (county2 > 99)
    textprintf_centre(menu_bmp, small_font, 320, 380, COLOUR_YELLOW5, "Press Space to continue.");
   vsync();
   blit(menu_bmp, screen, 0, 0, 0, 0, 640, 480);
  }

  do
  {
   county ++;
  } while (ticked == 0);

  ticked --;

  county2 ++;

  if (county2 > 99 && key [KEY_SPACE] != 0)
  {
   last_key = KEY_SPACE;
   break;
  }

 }
  while(TRUE);
}



