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
    
File: displ_in.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - bitmap loading and preparation
The bitmaps aren't organised into a datafile because I find datafiles a bit
unwieldy.

*/

#include "allegro.h"

#include <math.h>

#include "config.h"
#include "globvars.h"

#include "display.h"

#include "palette.h"

extern BITMAP *player1;
extern BITMAP *player2;

extern BITMAP *ship_bitmaps [NO_SHIP_TYPES] [17];

/*
White (lines only)
Blue
Green
Amber
Red
*/
extern RLE_SPRITE *large_ships [16] [5];

extern BITMAP *enemy1_bmp [ENEMY1_BMPS];

extern RLE_SPRITE *enemy1_rle [ENEMY1_RLES];
extern RLE_SPRITE *enemy2_rle [ENEMY2_RLES];
extern RLE_SPRITE *enemy3_rle [ENEMY3_RLES];
extern RLE_SPRITE *small1_rle [SMALL1_RLES];
extern BITMAP *small2_bmp [SMALL2_BMPS];
extern BITMAP *superjelly_bmp [2];

void bitmap_error(const char errtxt []);

void make_rle_enemy1(BITMAP *source_bmp, int which_enemy, int width, int height);
void make_rle_enemy2(BITMAP *source_bmp, int which_enemy, int width, int height);
void make_rle_enemy3(BITMAP *source_bmp, int which_enemy, int width, int height);
void make_rle_small1(BITMAP *source_bmp, int which_small, int width, int height);
void make_bmp_small2(BITMAP *source_bmp, int which_small, int width, int height);
void make_rle_large_ship(BITMAP *source_bmp, int which_ship);
void make_superjelly_bmps(void);

void make_actor_sprites(int at);

extern FONT *font2;
extern FONT *small_font;

void init_display(void)
{

 RGB temp_palette2 [1024];
 RGB temp_palette [256];
 RGB temp_palette3 [1024];

 DATAFILE *datf = load_datafile("gfx\\data.dat");
 if (datf == NULL)
 {
  set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
  allegro_message("Error: Couldn't find data.dat! \n");
  allegro_message("\n\r");
  exit(1);
 }

 font = datf[1].dat;
 font2 = datf[0].dat;
 small_font = datf[2].dat;

 text_mode(-1);

 textprintf_centre(screen, font, 320, 200, COLOUR_GREEN8, "Loading}...");
 textprintf_centre(screen, font2, 320, 200, COLOUR_YELLOW8, "Loading}...");

 BITMAP *temp_bitmap = load_bitmap("gfx\\ship8_i.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap");
 }

 BITMAP *temp_bitmap2 = create_bitmap(11, 11);
 if (temp_bitmap2 == NULL)
 {
  bitmap_error("temp_bitmap2");
 }

 blit(temp_bitmap, temp_bitmap2, 1, 1, 0, 0, 11, 11);
 blit(temp_bitmap, temp_bitmap2, 15, 1, 0, 0, 13, 13);

 int i;
 int j;


 for (i = 0; i < NO_SHIP_TYPES; i ++)
 {
/*  blit(temp_bitmap, temp_bitmap2, (i * 14) + 1, 1, 0, 0, 13, 13);
  for (j = 0; j < 17; j ++)
  {
   ship_bitmaps [i] [j] = create_bitmap(13, 13);
   if (ship_bitmaps [i] [j] == NULL)
   {
    bitmap_error("Ship bitmaps 0");
   }
   clear_bitmap(ship_bitmaps [i] [j]);
   blit(temp_bitmap, ship_bitmaps [i] [j], (j * 14) + 1, (i * 14) + 1, 0,0,13,13);*/
//   if (FALSE)//i != 11)// && i != 1 && i != 3 && i != 4 && FALSE)
//   if (i != 12 && i != 13 && i != 14 && i != 15)
   {
    for (j = 0; j < 17; j ++)
    {
     ship_bitmaps [i] [j] = create_bitmap(21, 21);
     if (ship_bitmaps [i] [j] == NULL)
     {
      bitmap_error("Ship bitmaps 0");
     }
     clear_bitmap(ship_bitmaps [i] [j]);
     blit(temp_bitmap, ship_bitmaps [i] [j], (j * 22) + 1, (i * 22) + 1, 0,0,21,21);
    }
   }
//    else
//      make_actor_sprites(i);
//   rotate_sprite(ship_bitmaps [i] [j], temp_bitmap2, 0, 0, itofix(j * 4));
 }

// save_bitmap("outsh.bmp",
 
/* blit(temp_bitmap, temp_bitmap2, 15, 1, 0, 0, 13, 13);

 for (i = 0; i < 17; i ++)
 {
  ship_bitmaps [1] [i] = create_bitmap(13, 13);
  if (ship_bitmaps [1] [i] == NULL)
  {
   bitmap_error("Ship bitmaps 0");
  }
  clear_bitmap(ship_bitmaps [1] [i]);
  rotate_sprite(ship_bitmaps [1] [i], temp_bitmap2, 0, 0, itofix(i * 4));
 }
 
 blit(temp_bitmap, temp_bitmap2, 10, 1, 0, 0, 8, 8);
*/
 destroy_bitmap(temp_bitmap);
 destroy_bitmap(temp_bitmap2);

// Let's make the output ship bitmap:

/* BITMAP *outp = create_bitmap((17 * 22) + 1, (17 * 22) + 1);
 clear_bitmap(outp);

 for (i = 0; i < NO_SHIP_TYPES; i ++)
 {
  for (j = 0; j < 18; j ++)
  {
   hline(outp, 0, i * 22, 18 * 22 + 1, COLOUR_PURPLE8);
   vline(outp, j * 22, 0, NO_SHIP_TYPES * 22 + 1, COLOUR_PURPLE8);
   if (ship_bitmaps [i] [j] != NULL)
    draw_sprite(outp, ship_bitmaps [i] [j], j * 22 + 1, i * 22 + 1);
  }
 }

 blit(outp, screen, 0,0,5,5, (16 * 22) + 1, (16 * 22) + 1);
 save_bitmap("ship8_o.bmp", outp, temp_palette);

 while (key [KEY_K] == 0)
 {
 };

 destroy_bitmap(outp);
*/

// Load in enemy bitmaps:

 temp_bitmap = load_bitmap("gfx\\enemy1.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (enemy1.bmp not loaded correctly?)");
 }

// temp_bitmap2 = create_bitmap(35, 26);
// if (temp_bitmap2 == NULL)
// {
//  bitmap_error("temp_bitmap2");
// }

 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_STINGER, 35, 26);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_DRIFTER, 27, 27);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_WANDERER, 37, 37);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_SUN, 45, 45);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_SUN_WHITE, 45, 45);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_JELLY1, 45, 33);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_JELLY2, 45, 33);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_JELLY3, 45, 33);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_JELLY4, 45, 33);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_JELLY5, 45, 33);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_JELLY6, 45, 33);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_MINETHROWER, 35, 41);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_MINETHROWER2, 49, 44);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_CURVER, 39, 36);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_STINGER2, 49, 34);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_BLASTER, 49, 36);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_STINGER3, 29, 28);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_FLAKKER, 45, 37);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_SUPERJELLY, 21, 21);
 make_superjelly_bmps();
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_HIVE, 47, 47);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_STINGER4, 49, 45);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_FLAKKER2, 49, 35);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_BOLTER, 43, 43);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_BOLTER2, 43, 43);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_TORPER1_1, 47, 45);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_TORPER1_2, 47, 45);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_TORPER1_3, 47, 45);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_TORPER1_4, 47, 45);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_HURLER, 43, 25);
 make_rle_enemy1(temp_bitmap, RLE_ENEMY1_HURLER2, 49, 36);

 destroy_bitmap(temp_bitmap);

 temp_bitmap = load_bitmap("gfx\\enemy3.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (enemy3.bmp not loaded correctly?)");
 }

 make_rle_enemy3(temp_bitmap, RLE_ENEMY3_FIREBASE, 133, 133);
 make_rle_enemy3(temp_bitmap, RLE_ENEMY3_FIREBASE2, 39, 69);
 make_rle_enemy3(temp_bitmap, RLE_ENEMY3_FIREBASE3, 91, 93);
 make_rle_enemy3(temp_bitmap, RLE_ENEMY3_BOSS1, 175, 159);
 make_rle_enemy3(temp_bitmap, RLE_ENEMY3_BOSS3, 155, 147);
 make_rle_enemy3(temp_bitmap, RLE_ENEMY3_BOSS4, 223, 138);

// blit(temp_bitmap, temp_bitmap2, 1, 1, 0, 0, 35, 26);

// enemy1_rle [RLE_ENEMY1_STINGER] = get_rle_sprite(temp_bitmap2);

// destroy_bitmap(temp_bitmap2);

 destroy_bitmap(temp_bitmap);
// destroy_bitmap(temp_bitmap2);

 temp_bitmap = load_bitmap("gfx\\small1.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (small1.bmp not loaded correctly?)");
 }

 for (i = RLE_SMALL1_GREEN_BLOB_L; i < SMALL1_RLES; i ++)
 {
  make_rle_small1(temp_bitmap, i, 11, 11);
 }
 
 destroy_bitmap(temp_bitmap);


 temp_bitmap = load_bitmap("gfx\\small2.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (small2.bmp not loaded correctly?)");
 }

 for (i = BMP_SMALL2_MISSILE_1; i < BMP_SMALL2_SIDE_BOMB + 1; i ++)
 {
  make_bmp_small2(temp_bitmap, i, 7, 7);
 }

 destroy_bitmap(temp_bitmap);

 temp_bitmap = load_bitmap("gfx\\enemy2.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (enemy2.bmp not loaded correctly?)");
 }

 for (i = 0; i < 9; i ++)
 {
  make_rle_enemy2(temp_bitmap, i, 27, 27);
 }

 destroy_bitmap(temp_bitmap);
 
 temp_bitmap = load_bitmap("gfx\\lships.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (lships.bmp not loaded correctly?)");
 }

 for (i = 0; i < 16; i ++)
 {
  make_rle_large_ship(temp_bitmap, i);
 }
 
 destroy_bitmap(temp_bitmap);

}

void make_rle_enemy1(BITMAP *source_bmp, int which_enemy, int width, int height)
{
 BITMAP *temp_bmp = create_bitmap(width, height);
 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_enemy1)");
 }

 blit(source_bmp, temp_bmp, which_enemy * 51 + 1, 1, 0, 0, width, height);

 enemy1_rle [which_enemy] = get_rle_sprite(temp_bmp);

 destroy_bitmap(temp_bmp);

}

void make_rle_enemy2(BITMAP *source_bmp, int which_enemy, int width, int height)
{
 BITMAP *temp_bmp = create_bitmap(width, height);

 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_enemy2)");
 }

 clear_bitmap(temp_bmp);

 blit(source_bmp, temp_bmp, which_enemy * 41 + 1, 1, 0, 0, width, height);
 enemy2_rle [which_enemy] = get_rle_sprite(temp_bmp);

 destroy_bitmap(temp_bmp);


 init_messages();

}

void make_rle_enemy3(BITMAP *source_bmp, int which_enemy, int width, int height)
{
 BITMAP *temp_bmp = create_bitmap(width, height);
 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_enemy3)");
 }

 blit(source_bmp, temp_bmp, which_enemy * 301 + 1, 1, 0, 0, width, height);

 enemy3_rle [which_enemy] = get_rle_sprite(temp_bmp);

 destroy_bitmap(temp_bmp);

}

void make_rle_small1(BITMAP *source_bmp, int which_small, int width, int height)
{
 BITMAP *temp_bmp = create_bitmap(width, height);
 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_small1)");
 }

 blit(source_bmp, temp_bmp, which_small * 21 + 1, 1, 0, 0, width, height);

 small1_rle [which_small] = get_rle_sprite(temp_bmp);

 destroy_bitmap(temp_bmp);

}

void make_bmp_small2(BITMAP *source_bmp, int which_small, int width, int height)
{

 small2_bmp [which_small] = create_bitmap(width, height);
 
 if (small2_bmp [which_small] == NULL)
 {
  bitmap_error("temp_bmp (make_bmp_small2)");
 }

 blit(source_bmp, small2_bmp [which_small], which_small * 21 + 1, 1, 0, 0, width, height);

}

void make_rle_large_ship(BITMAP *source_bmp, int which_ship)
{
 BITMAP *temp_bmp = create_bitmap(49, 49);
 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_large_ship)");
 }

// if (which_ship > 11)
// {
//  blit(source_bmp, temp_bmp, 51 + 1, 1, 0, 0, 49, 49);
// }
//  else
   blit(source_bmp, temp_bmp, which_ship * 51 + 1, 1, 0, 0, 49, 49);

 int i, x, y, px, base_col;

 for (i = 0; i < 5; i ++)
 {
  switch(i)
  {
   case 0:
    base_col = COLOUR_BLUE1; break;
   case 1:
    base_col = COLOUR_GREEN1; break;
   case 2:
    base_col = COLOUR_YELLOW1; break;
   case 3:
    base_col = COLOUR_RED1; break;
  }
  for (x = 0; x < 50; x ++)
  {
   for (y = 0; y < 50; y ++)
   {
    px = getpixel(temp_bmp, x, y);
    if (i == 4)
    {
     if (px % 8 != 5 && px != 0)
      putpixel(temp_bmp, x, y, COLOUR_WHITE);
       else
        putpixel(temp_bmp, x, y, 0);
    }
     else
     {
      if (px != 0)
       putpixel(temp_bmp, x, y, base_col + (px % 8));
     }
   }
  }
  large_ships [which_ship] [i] = get_rle_sprite(temp_bmp);
 }
 
 destroy_bitmap(temp_bmp);

}

void make_superjelly_bmps(void)
{
 superjelly_bmp [0] = create_bitmap(21, 21);
 if (superjelly_bmp [0] == NULL)
 {
  bitmap_error("temp_bmp (make_superjelly_bmps - superjelly_bmp [0])");
 }
 clear_bitmap(superjelly_bmp [0]);

 superjelly_bmp [1] = create_bitmap(21, 21);
 if (superjelly_bmp [1] == NULL)
 {
  bitmap_error("temp_bmp (make_superjelly_bmps - superjelly_bmp [1])");
 }
 clear_bitmap(superjelly_bmp [1]);

 draw_rle_sprite(superjelly_bmp [0], enemy1_rle [RLE_ENEMY1_SUPERJELLY], 0, 0);

 rotate_sprite(superjelly_bmp [1], superjelly_bmp [0], 0, 0, itofix(64));

}


void draw_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y);

void draw_sprite_h_flip(BITMAP *bmp, BITMAP *sprite, int x, int y);

void draw_sprite_vh_flip(BITMAP *bmp, BITMAP *sprite, int x, int y);

void bitmap_error(const char errtxt [])
{
 set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
 allegro_message("Bitmap creation error  \n%s\n", allegro_error);
 allegro_message("\n\r");
 allegro_message(errtxt);
 exit(1);

}

void prepare_display(void)
{
 if (game.users == 1)
 {
  player1 = create_bitmap(640, 480);
  clear_bitmap(player1);
 }
  else
  {
   player1 = create_bitmap(315, 480);
   clear_bitmap(player1);
   player2 = create_bitmap(315, 480);
   clear_bitmap(player1);
  }

}

#ifdef DONT_COMPILE

void make_actor_sprites_old_small(int at)
{

 int x,y,x1, x2, x3, y1, y2, y3;
 int i;
 float angle;
 int a1, a2, a3;

 int points [16];

 BITMAP *tbmp = create_bitmap(21, 21);
 
 switch(at)
 {
  case 1:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 7;
    y1 = 100 + sin(angle) * 7;
    x2 = 100 + cos(angle + PI + PI / 3) * 6;
    y2 = 100 + sin(angle + PI + PI / 3) * 6;
    x3 = 100 + cos(angle + PI - PI / 3) * 6;
    y3 = 100 + sin(angle + PI - PI / 3) * 6;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 6;
    y1 = 100 + sin(angle) * 6;
    x2 = 100 + cos(angle + PI + PI / 3) * 5;
    y2 = 100 + sin(angle + PI + PI / 3) * 5;
    x3 = 100 + cos(angle + PI - PI / 3) * 5;
    y3 = 100 + sin(angle + PI - PI / 3) * 5;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 3;
    y1 = 100 + sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x1, y1, 1, COLOUR_WHITE);
//  line(ship_bitmaps [at] [i], x1, y1, x2, y2, COLOUR_GREY4);
//  line(ship_bitmaps [at] [i], x2, y2, x3, y3, COLOUR_GREY4);
//  line(ship_bitmaps [at] [i], x1, y1, x3, y3, COLOUR_GREY4);
   }
  break;
  case 2:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x2 = 100 + cos(angle + PI + PI / 3) * 5;
    y2 = 100 + sin(angle + PI + PI / 3) * 5;
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI - PI / 3) * (6 - (angle == 16));
    y2 = 100 + sin(angle + PI - PI / 3) * (6 - (angle == 16));
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY5);
    x = 10;
    y = 10;
    circlefill(ship_bitmaps [at] [i], x, y, 5, COLOUR_GREY4);
    circlefill(ship_bitmaps [at] [i], x, y, 4, COLOUR_GREY5);
//    x1 = 10;// + cos(angle) * 2;
//    y1 = 10;// + sin(angle) * 2;
//    circlefill(ship_bitmaps [at] [i], x1, y1, 1, COLOUR_WHITE);
    a1 = 64 - (i * 4 + 19);
    a2 = 64 - (i * 4 - 19);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 3, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 3, COLOUR_WHITE);
//     a3 = a1;
//     a1 = a2;
//     a2 = a3;
      }
       else
        arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 3, COLOUR_WHITE);
    }
   }
   break;
  case 3:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    circlefill(ship_bitmaps [at] [i], 10, 10, 2, COLOUR_GREY4);
    x2 = 100 + cos(angle + PI + PI / 4) * 5;
    y2 = 100 + sin(angle + PI + PI / 4) * 5;
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY4);
    circlefill(ship_bitmaps [at] [i], x2, y2, 1, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI + PI / 4) * 2;
    y2 = 100 + sin(angle + PI + PI / 4) * 2;
    circlefill(ship_bitmaps [at] [i], x2, y2, 1, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI - PI / 4) * 5;
    y2 = 100 + sin(angle + PI - PI / 4) * 5;
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY4);
    circlefill(ship_bitmaps [at] [i], x2, y2, 1, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI - PI / 4) * 2;
    y2 = 100 + sin(angle + PI - PI / 4) * 2;
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI / 4) * 5;
    y2 = 100 + sin(angle + PI / 4) * 5;
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY4);
    circlefill(ship_bitmaps [at] [i], x2, y2, 1, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI / 4) * 2;
    y2 = 100 + sin(angle + PI / 4) * 2;
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY5);
    x2 = 100 + cos(angle - PI / 4) * 5;
    y2 = 100 + sin(angle - PI / 4) * 5;
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY4);
    circlefill(ship_bitmaps [at] [i], x2, y2, 1, COLOUR_GREY5);
    x2 = 100 + cos(angle - PI / 4) * 2;
    y2 = 100 + sin(angle - PI / 4) * 2;
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY5);
    x = 10;
    y = 10;
    circlefill(ship_bitmaps [at] [i], x, y, 1, COLOUR_GREY5);
//    x1 = 10;// + cos(angle) * 2;
//    y1 = 10;// + sin(angle) * 2;
//    circlefill(ship_bitmaps [at] [i], x1, y1, 1, COLOUR_WHITE);
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
//     a3 = a1;
//     a1 = a2;
//     a2 = a3;
      }
       else
        arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
    }
   }
   break;
  case 4:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    clear_bitmap(tbmp);
    circlefill(tbmp, 10, 10, 7, COLOUR_GREY4);
    x = 100 + cos(angle) * 6;
    y = 100 + sin(angle) * 6;
    circlefill(tbmp, x, y, 7, 0);
    blit(tbmp, ship_bitmaps [at] [i], 0,0,0,0,21,21);
    clear_bitmap(tbmp);
    circlefill(tbmp, 10, 10, 6, COLOUR_GREY5);
    x = 100 + cos(angle) * 6;
    y = 100 + sin(angle) * 6;
    circlefill(tbmp, x, y, 8, 0);
    x = 10;// - cos(angle) * 3;
    y = 10;// - sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x, y, 3, COLOUR_GREY4);
    masked_blit(tbmp, ship_bitmaps [at] [i], 0,0,0,0,21,21);
    x = 10;// - cos(angle) * 3;
    y = 10;// - sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x, y, 2, COLOUR_GREY5);
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
      }
       else
        arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
     }
   }
   break;
  case 5:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    clear_bitmap(tbmp);
    circlefill(tbmp, 10, 10, 7, COLOUR_GREY4);
    circlefill(tbmp, 10, 10, 6, COLOUR_GREY5);
/*    x = 10 - cos(angle) * 4;
    y = 10 - sin(angle) * 4;
    circlefill(tbmp, x, y, 3, 0);
    x = 10 - cos(angle) * 3;
    y = 10 - sin(angle) * 3;
    circlefill(tbmp, x, y, 3, 0);
    x = 10 - cos(angle) * 2;
    y = 10 - sin(angle) * 2;
    circlefill(tbmp, x, y, 3, 0);*/
    x = 10;// ;;+ cos(angle) * 1;
    y = 10;// + sin(angle) * 1;
    circlefill(tbmp, x, y, 3, 0);
    x = 100 + cos(angle) * 2;
    y = 100 + sin(angle) * 2;
    circlefill(tbmp, x, y, 3, 0);
    x = 100 + cos(angle) * 3;
    y = 100 + sin(angle) * 3;
    circlefill(tbmp, x, y, 3, 0);
    x = 100 + cos(angle) * 4;
    y = 100 + sin(angle) * 4;
    circlefill(tbmp, x, y, 3, 0);
    x = 100 + cos(angle) * 5;
    y = 100 + sin(angle) * 5;
    circlefill(tbmp, x, y, 3, 0);
    x = 100 + cos(angle) * 6;
    y = 100 + sin(angle) * 6;
    circlefill(tbmp, x, y, 3, 0);
    x = 100 + cos(angle) * 7;
    y = 100 + sin(angle) * 7;
    circlefill(tbmp, x, y, 3, 0);
    blit(tbmp, ship_bitmaps [at] [i], 0,0,0,0,21,21);
    clear_bitmap(tbmp);
/*    circlefill(tbmp, 10, 10, 6, COLOUR_GREY5);
    x = 100 + cos(angle) * 6;
    y = 100 + sin(angle) * 6;
    circlefill(tbmp, x, y, 8, 0);
    x = 10;// - cos(angle) * 3;
    y = 10;// - sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x, y, 4, COLOUR_GREY4);
    masked_blit(tbmp, ship_bitmaps [at] [i], 0,0,0,0,21,21);*/
    x = 10;// - cos(angle) * 3;
    y = 10;// - sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x, y, 3, COLOUR_GREY5);
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
      }
       else
        arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
     }
   }
   break;
  case 6:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle + PI) * 5;
    y1 = 100 + sin(angle + PI) * 5;
    x2 = 100 + cos(angle + PI / 2) * 9;
    y2 = 100 + sin(angle + PI / 2) * 9;
    x3 = 100 + cos(angle - PI / 2) * 9;
    y3 = 100 + sin(angle - PI / 2) * 9;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 7;
    y1 = 100 + sin(angle) * 7;
    x2 = 100 + cos(angle + PI + PI / 3) * 4;
    y2 = 100 + sin(angle + PI + PI / 3) * 4;
    x3 = 100 + cos(angle + PI - PI / 3) * 4;
    y3 = 100 + sin(angle + PI - PI / 3) * 4;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 6;
    y1 = 100 + sin(angle) * 6;
    x2 = 100 + cos(angle + PI + PI / 3) * 3;
    y2 = 100 + sin(angle + PI + PI / 3) * 3;
    x3 = 100 + cos(angle + PI - PI / 3) * 3;
    y3 = 100 + sin(angle + PI - PI / 3) * 3;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle + PI) * 4;
    y1 = 100 + sin(angle + PI) * 4;
    x2 = 100 + cos(angle + PI / 2) * 8;
    y2 = 100 + sin(angle + PI / 2) * 8;
    x3 = 100 + cos(angle - PI / 2) * 8;
    y3 = 100 + sin(angle - PI / 2) * 8;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);



    
    x1 = 100 + cos(angle) * 3;
    y1 = 100 + sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x1, y1, 1, COLOUR_WHITE);
//  line(ship_bitmaps [at] [i], x1, y1, x2, y2, COLOUR_GREY4);
//  line(ship_bitmaps [at] [i], x2, y2, x3, y3, COLOUR_GREY4);
//  line(ship_bitmaps [at] [i], x1, y1, x3, y3, COLOUR_GREY4);
   }
  break;
  case 7:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    x = 100 + cos(angle) * 3;
    y = 100 + sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x, y, 2, COLOUR_GREY4);
    x1 = x + cos(angle + PI / 3) * 6;
    y1 = y + sin(angle + PI / 3) * 6;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = x + cos(angle - PI / 3) * 6;
    y1 = y + sin(angle - PI / 3) * 6;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = x + cos(angle + PI + PI / 2) * 6;
    y1 = y + sin(angle + PI + PI / 2) * 6;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = x + cos(angle + PI - PI / 2) * 7;
    y1 = y + sin(angle + PI - PI / 2) * 7;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = 100 + cos(angle + PI + PI / 3) * 6;
    y1 = 100 + sin(angle + PI + PI / 3) * 6;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = 100 + cos(angle + PI - PI / 3) * 6;
    y1 = 100 + sin(angle + PI - PI / 3) * 6;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = 100 + cos(angle + PI + PI / 6) * 7;
    y1 = 100 + sin(angle + PI + PI / 6) * 7;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = 100 + cos(angle + PI - PI / 5) * 7;
    y1 = 100 + sin(angle + PI - PI / 5) * 7;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x = 100 + cos(angle) * 3;
    y = 100 + sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x, y, 3, COLOUR_GREY5);
    
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
      }
       else
        arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
     }
   }
   break;
  case 8:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 7;
    y1 = 100 + sin(angle) * 7;
    x2 = 100 + cos(angle + PI / 2) * 4;
    y2 = 100 + sin(angle + PI / 2) * 4;
    x3 = 100 + cos(angle - PI / 2) * 4;
    y3 = 100 + sin(angle - PI / 2) * 4;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 6;
    y1 = 100 + sin(angle) * 6;
    x2 = 100 + cos(angle + PI / 2) * 3;
    y2 = 100 + sin(angle + PI / 2) * 3;
    x3 = 100 + cos(angle - PI / 2) * 3;
    y3 = 100 + sin(angle - PI / 2) * 3;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 6;
    y1 = 100 + sin(angle) * 6;
    x2 = 100 + cos(angle + PI - PI / 2) * 8;
    y2 = 100 + sin(angle + PI - PI / 2) * 8;
    x3 = 100 + cos(angle + PI - PI / 4) * 8;
    y3 = 100 + sin(angle + PI - PI / 4) * 8;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 6;
    y1 = 100 + sin(angle) * 6;
    x2 = 100 + cos(angle + PI + PI / 2) * 8;
    y2 = 100 + sin(angle + PI + PI / 2) * 8;
    x3 = 100 + cos(angle + PI + PI / 4) * 8;
    y3 = 100 + sin(angle + PI + PI / 4) * 8;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 5;
    y1 = 100 + sin(angle) * 5;
    x2 = 100 + cos(angle + PI - PI / 2) * 7;
    y2 = 100 + sin(angle + PI - PI / 2) * 7;
    x3 = 100 + cos(angle + PI - PI / 4) * 7;
    y3 = 100 + sin(angle + PI - PI / 4) * 7;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 5;
    y1 = 100 + sin(angle) * 5;
    x2 = 100 + cos(angle + PI + PI / 2) * 7;
    y2 = 100 + sin(angle + PI + PI / 2) * 7;
    x3 = 100 + cos(angle + PI + PI / 4) * 7;
    y3 = 100 + sin(angle + PI + PI / 4) * 7;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 3;
    y1 = 100 + sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x1, y1, 1, COLOUR_WHITE);
   }
  break;
  case 10:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 7;
    y1 = 100 + sin(angle) * 7;
    x2 = 100 + cos(angle + PI / 2) * 4;
    y2 = 100 + sin(angle + PI / 2) * 4;
    x3 = 100 + cos(angle - PI / 2) * 4;
    y3 = 100 + sin(angle - PI / 2) * 4;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 6;
    y1 = 100 + sin(angle) * 6;
    x2 = 100 + cos(angle + PI / 2) * 3;
    y2 = 100 + sin(angle + PI / 2) * 3;
    x3 = 100 + cos(angle - PI / 2) * 3;
    y3 = 100 + sin(angle - PI / 2) * 3;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 6;
    y1 = 100 + sin(angle) * 6;
    x2 = 100 + cos(angle + PI - PI / 4) * 8;
    y2 = 100 + sin(angle + PI - PI / 4) * 8;
    x3 = 100 + cos(angle + PI - PI / 8) * 8;
    y3 = 100 + sin(angle + PI - PI / 8) * 8;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 6;
    y1 = 100 + sin(angle) * 6;
    x2 = 100 + cos(angle + PI + PI / 4) * 8;
    y2 = 100 + sin(angle + PI + PI / 4) * 8;
    x3 = 100 + cos(angle + PI + PI / 8) * 8;
    y3 = 100 + sin(angle + PI + PI / 8) * 8;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 5;
    y1 = 100 + sin(angle) * 5;
    x2 = 100 + cos(angle + PI - PI / 4) * 7;
    y2 = 100 + sin(angle + PI - PI / 4) * 7;
    x3 = 100 + cos(angle + PI - PI / 8) * 7;
    y3 = 100 + sin(angle + PI - PI / 8) * 7;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 5;
    y1 = 100 + sin(angle) * 5;
    x2 = 100 + cos(angle + PI + PI / 4) * 7;
    y2 = 100 + sin(angle + PI + PI / 4) * 7;
    x3 = 100 + cos(angle + PI + PI / 8) * 7;
    y3 = 100 + sin(angle + PI + PI / 8) * 7;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 3;
    y1 = 100 + sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x1, y1, 1, COLOUR_WHITE);
   }
  break;
//  case 11:
   for (i = 0; i < 17; i ++)
   {
    tbmp = create_bitmap(210, 210);
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
/*
    points [0] = 100 + cos(angle + PI / 2) * 30;
    points [1] = 100 + sin(angle + PI / 2) * 30;
    points [2] = 100 + cos(angle + PI / 10) * 80;
    points [3] = 100 + sin(angle + PI / 10) * 80;
    points [4] = 100 + cos(angle + PI / 7) * 90;
    points [5] = 100 + sin(angle + PI / 7) * 90;
    points [6] = 100 + cos(angle + PI - PI / 3) * 50;
    points [7] = 100 + sin(angle + PI - PI / 3) * 50;
    points [8] = 100 + cos(angle + PI + PI / 3) * 50;
    points [9] = 100 + sin(angle + PI + PI / 3) * 50;
    points [10] = 100 + cos(angle - PI / 7) * 90;
    points [11] = 100 + sin(angle - PI / 7) * 90;
    points [12] = 100 + cos(angle - PI / 10) * 80;
    points [13] = 100 + sin(angle - PI / 10) * 80;
    points [14] = 100 + cos(angle - PI / 2) * 30;
    points [15] = 100 + sin(angle - PI / 2) * 30;
    polygon(ship_bitmaps [at] [i], 8, points, COLOUR_GREY4);
    
    x1 = 100 + cos(angle + PI) * 40;
    y1 = 100 + sin(angle + PI) * 40;
    x2 = 100 + cos(angle + PI / 2) * 70;
    y2 = 100 + sin(angle + PI / 2) * 70;
    x3 = 100 + cos(angle - PI / 2) * 70;
    y3 = 100 + sin(angle - PI / 2) * 70;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle + PI) * 30;
    y1 = 100 + sin(angle + PI) * 30;
    x2 = 100 + cos(angle + PI / 2) * 60;
    y2 = 100 + sin(angle + PI / 2) * 60;
    x3 = 100 + cos(angle - PI / 2) * 60;
    y3 = 100 + sin(angle - PI / 2) * 60;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    
    points [0] = 100 + cos(angle + PI / 2) * 20;
    points [1] = 100 + sin(angle + PI / 2) * 20;
    points [2] = 100 + cos(angle + PI / 8) * 70;
    points [3] = 100 + sin(angle + PI / 8) * 70;
    points [4] = 100 + cos(angle + PI / 5) * 80;
    points [5] = 100 + sin(angle + PI / 5) * 80;
    points [6] = 100 + cos(angle + PI - PI / 3) * 40;
    points [7] = 100 + sin(angle + PI - PI / 3) * 40;
    points [8] = 100 + cos(angle + PI + PI / 3) * 40;
    points [9] = 100 + sin(angle + PI + PI / 3) * 40;
    points [10] = 100 + cos(angle - PI / 5) * 80;
    points [11] = 100 + sin(angle - PI / 5) * 80;
    points [12] = 100 + cos(angle - PI / 8) * 70;
    points [13] = 100 + sin(angle - PI / 8) * 70;
    points [14] = 100 + cos(angle - PI / 2) * 20;
    points [15] = 100 + sin(angle - PI / 2) * 20;
    polygon(ship_bitmaps [at] [i], 8, points, COLOUR_GREY5);



    x1 = 10;// - cos(angle) * 2;
    y1 = 10;// - sin(angle) * 2;
    circlefill(ship_bitmaps [at] [i], x1, y1, 10, COLOUR_WHITE);*/
    points [0] = 100 + cos(angle + PI / 2) * 30;
    points [1] = 100 + sin(angle + PI / 2) * 30;
    points [2] = 100 + cos(angle + PI / 10) * 80;
    points [3] = 100 + sin(angle + PI / 10) * 80;
    points [4] = 100 + cos(angle + PI / 7) * 90;
    points [5] = 100 + sin(angle + PI / 7) * 90;
    points [6] = 100 + cos(angle + PI - PI / 3) * 50;
    points [7] = 100 + sin(angle + PI - PI / 3) * 50;
    points [8] = 100 + cos(angle + PI + PI / 3) * 50;
    points [9] = 100 + sin(angle + PI + PI / 3) * 50;
    points [10] = 100 + cos(angle - PI / 7) * 90;
    points [11] = 100 + sin(angle - PI / 7) * 90;
    points [12] = 100 + cos(angle - PI / 10) * 80;
    points [13] = 100 + sin(angle - PI / 10) * 80;
    points [14] = 100 + cos(angle - PI / 2) * 30;
    points [15] = 100 + sin(angle - PI / 2) * 30;
    polygon(tbmp, 8, points, COLOUR_GREY4);
    
    x1 = 100 + cos(angle + PI) * 40;
    y1 = 100 + sin(angle + PI) * 40;
    x2 = 100 + cos(angle + PI / 2) * 70;
    y2 = 100 + sin(angle + PI / 2) * 70;
    x3 = 100 + cos(angle - PI / 2) * 70;
    y3 = 100 + sin(angle - PI / 2) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle + PI) * 30;
    y1 = 100 + sin(angle + PI) * 30;
    x2 = 100 + cos(angle + PI / 2) * 60;
    y2 = 100 + sin(angle + PI / 2) * 60;
    x3 = 100 + cos(angle - PI / 2) * 60;
    y3 = 100 + sin(angle - PI / 2) * 60;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    
    points [0] = 100 + cos(angle + PI / 2) * 20;
    points [1] = 100 + sin(angle + PI / 2) * 20;
    points [2] = 100 + cos(angle + PI / 8) * 70;
    points [3] = 100 + sin(angle + PI / 8) * 70;
    points [4] = 100 + cos(angle + PI / 5) * 80;
    points [5] = 100 + sin(angle + PI / 5) * 80;
    points [6] = 100 + cos(angle + PI - PI / 3) * 40;
    points [7] = 100 + sin(angle + PI - PI / 3) * 40;
    points [8] = 100 + cos(angle + PI + PI / 3) * 40;
    points [9] = 100 + sin(angle + PI + PI / 3) * 40;
    points [10] = 100 + cos(angle - PI / 5) * 80;
    points [11] = 100 + sin(angle - PI / 5) * 80;
    points [12] = 100 + cos(angle - PI / 8) * 70;
    points [13] = 100 + sin(angle - PI / 8) * 70;
    points [14] = 100 + cos(angle - PI / 2) * 20;
    points [15] = 100 + sin(angle - PI / 2) * 20;
    polygon(tbmp, 8, points, COLOUR_GREY5);



    x1 = 100;// - cos(angle) * 2;
    y1 = 100;// - sin(angle) * 2;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }

   
  break;
//  case 11:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    points [0] = 100 + cos(angle) * 2;
    points [1] = 100 + sin(angle) * 2;
    points [2] = 100 + cos(angle + PI / 2) * 9;
    points [3] = 100 + sin(angle + PI / 2) * 9;
    points [4] = 100 + cos(angle + PI) * 2;
    points [5] = 100 + sin(angle + PI) * 2;
    points [6] = 100 + cos(angle - PI / 2) * 7;
    points [7] = 100 + sin(angle - PI / 2) * 7;
    polygon(ship_bitmaps [at] [i], 4, points, COLOUR_GREY4);
    circle(ship_bitmaps [at] [i], 10, 10, 7, COLOUR_GREY5);
    x = 10;// - cos(angle) * 3;
    y = 10;// - sin(angle) * 3;
//    circle(ship_bitmaps [at] [i], x, y, 7, COLOUR_GREY4);
    circle(ship_bitmaps [at] [i], x, y, 8, COLOUR_GREY5);
    x = 100 + cos(angle) * 12;
    y = 100 + sin(angle) * 12;
    circlefill(ship_bitmaps [at] [i], x, y, 9, 0);
    x = 10 - cos(angle) * 12;
    y = 10 - sin(angle) * 12;
    circlefill(ship_bitmaps [at] [i], x, y, 8, 0);
    circlefill(ship_bitmaps [at] [i], 10, 10, 3, COLOUR_GREY4);
    
    points [0] = 100 + cos(angle) * 1;
    points [1] = 100 + sin(angle) * 1;
    points [2] = 100 + cos(angle + PI / 2) * 8;
    points [3] = 100 + sin(angle + PI / 2) * 8;
    points [4] = 100 + cos(angle + PI) * 1;
    points [5] = 100 + sin(angle + PI) * 1;
    points [6] = 100 + cos(angle - PI / 2) * 6;
    points [7] = 100 + sin(angle - PI / 2) * 6;
    polygon(ship_bitmaps [at] [i], 4, points, COLOUR_GREY5);
    
    x = 10;// - cos(angle) * 3;
    y = 10;// - sin(angle) * 3;
//    circlefill(ship_bitmaps [at] [i], x, y, 3, COLOUR_GREY4);
    circlefill(ship_bitmaps [at] [i], x, y, 2, COLOUR_GREY5);

//    circlefill(ship_bitmaps [at] [i], 10, 10, 1, COLOUR_GREY6);

    x = 10;// - cos(angle) * 3;
    y = 10;// - sin(angle) * 3;
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
      }
       else
        arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
     }
   }
   break;
  case 11:
   for (i = 0; i < 17; i ++)
   {
    tbmp = create_bitmap(210, 210);
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI / 2) * 40;
    y2 = 100 + sin(angle + PI / 2) * 40;
    x3 = 100 + cos(angle - PI / 2) * 40;
    y3 = 100 + sin(angle - PI / 2) * 40;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI / 2) * 30;
    y2 = 100 + sin(angle + PI / 2) * 30;
    x3 = 100 + cos(angle - PI / 2) * 30;
    y3 = 100 + sin(angle - PI / 2) * 30;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI - PI / 4) * 80;
    y2 = 100 + sin(angle + PI - PI / 4) * 80;
    x3 = 100 + cos(angle + PI - PI / 8) * 80;
    y3 = 100 + sin(angle + PI - PI / 8) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI + PI / 4) * 80;
    y2 = 100 + sin(angle + PI + PI / 4) * 80;
    x3 = 100 + cos(angle + PI + PI / 8) * 80;
    y3 = 100 + sin(angle + PI + PI / 8) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI - PI / 4) * 70;
    y2 = 100 + sin(angle + PI - PI / 4) * 70;
    x3 = 100 + cos(angle + PI - PI / 8) * 70;
    y3 = 100 + sin(angle + PI - PI / 8) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI + PI / 4) * 70;
    y2 = 100 + sin(angle + PI + PI / 4) * 70;
    x3 = 100 + cos(angle + PI + PI / 8) * 70;
    y3 = 100 + sin(angle + PI + PI / 8) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 30;
    y1 = 100 + sin(angle) * 30;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
  break;
  
 }

 destroy_bitmap(tbmp);
}

#endif










































void make_actor_sprites(int at)
{

 int x,y,x1, x2, x3, y1, y2, y3;
 int i;
 float angle;
 int a1, a2;//, a3;

 int points [16];

 BITMAP *tbmp = create_bitmap(210, 210);
 BITMAP *tbmp2;
 
 switch(at)
 {
  case 1:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI + PI / 3) * 60;
    y2 = 100 + sin(angle + PI + PI / 3) * 60;
    x3 = 100 + cos(angle + PI - PI / 3) * 60;
    y3 = 100 + sin(angle + PI - PI / 3) * 60;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI + PI / 3) * 50;
    y2 = 100 + sin(angle + PI + PI / 3) * 50;
    x3 = 100 + cos(angle + PI - PI / 3) * 50;
    y3 = 100 + sin(angle + PI - PI / 3) * 50;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 30;
    y1 = 100 + sin(angle) * 30;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
//  line(ship_bitmaps [at] [i], x1, y1, x2, y2, COLOUR_GREY4);
//  line(ship_bitmaps [at] [i], x2, y2, x3, y3, COLOUR_GREY4);
//  line(ship_bitmaps [at] [i], x1, y1, x3, y3, COLOUR_GREY4);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
  break;
  case 2:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x2 = 100 + cos(angle + PI + PI / 3) * 50;
    y2 = 100 + sin(angle + PI + PI / 3) * 50;
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI - PI / 3) * (6 - (angle == 16));
    y2 = 100 + sin(angle + PI - PI / 3) * (6 - (angle == 16));
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY5);
    x = 100;
    y = 100;
    circlefill(tbmp, x, y, 50, COLOUR_GREY4);
    circlefill(tbmp, x, y, 40, COLOUR_GREY5);
//    x1 = 100;// + cos(angle) * 20;
//    y1 = 100;// + sin(angle) * 20;
//    circlefill(tbmp, x1, y1, 1, COLOUR_WHITE);
    a1 = 64 - (i * 4 + 19);
    a2 = 64 - (i * 4 - 19);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 30, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(tbmp, x, y, itofix(a1), itofix(a2), 30, COLOUR_WHITE);
//     a3 = a1;
//     a1 = a2;
//     a2 = a3;
      }
       else
        arc(tbmp, x, y, itofix(a1), itofix(a2), 30, COLOUR_WHITE);
    }
    
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
   
   break;
  case 3:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    circlefill(tbmp, 100, 100, 20, COLOUR_GREY4);
    x2 = 100 + cos(angle + PI + PI / 4) * 50;
    y2 = 100 + sin(angle + PI + PI / 4) * 50;
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY4);
    circlefill(tbmp, x2, y2, 10, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI + PI / 4) * 20;
    y2 = 100 + sin(angle + PI + PI / 4) * 20;
    circlefill(tbmp, x2, y2, 10, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI - PI / 4) * 50;
    y2 = 100 + sin(angle + PI - PI / 4) * 50;
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY4);
    circlefill(tbmp, x2, y2, 10, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI - PI / 4) * 20;
    y2 = 100 + sin(angle + PI - PI / 4) * 20;
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI / 4) * 50;
    y2 = 100 + sin(angle + PI / 4) * 50;
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY4);
    circlefill(tbmp, x2, y2, 10, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI / 4) * 20;
    y2 = 100 + sin(angle + PI / 4) * 20;
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY5);
    x2 = 100 + cos(angle - PI / 4) * 50;
    y2 = 100 + sin(angle - PI / 4) * 50;
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY4);
    circlefill(tbmp, x2, y2, 10, COLOUR_GREY5);
    x2 = 100 + cos(angle - PI / 4) * 20;
    y2 = 100 + sin(angle - PI / 4) * 20;
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY5);
    x = 100;
    y = 100;
    circlefill(tbmp, x, y, 10, COLOUR_GREY5);
//    x1 = 100;// + cos(angle) * 20;
//    y1 = 100;// + sin(angle) * 20;
//    circlefill(tbmp, x1, y1, 1, COLOUR_WHITE);
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
//     a3 = a1;
//     a1 = a2;
//     a2 = a3;
      }
       else
        arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
    }
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
   break;
  case 4:
  tbmp2 = create_bitmap(210, 210);
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    clear_bitmap(tbmp2);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    clear_bitmap(tbmp);
    circlefill(tbmp2, 100, 100, 70, COLOUR_GREY4);
    x = 100 + cos(angle) * 60;
    y = 100 + sin(angle) * 60;
    circlefill(tbmp2, x, y, 70, 0);
    blit(tbmp2, tbmp, 0,0,0,0,210,210);
    clear_bitmap(tbmp);
    circlefill(tbmp2, 100, 100, 60, COLOUR_GREY5);
    x = 100 + cos(angle) * 60;
    y = 100 + sin(angle) * 60;
    circlefill(tbmp2, x, y, 80, 0);
    x = 100;// - cos(angle) * 30;
    y = 100;// - sin(angle) * 30;
    circlefill(tbmp2, x, y, 30, COLOUR_GREY4);
    masked_blit(tbmp2, tbmp, 0,0,0,0,210,210);
    x = 100;// - cos(angle) * 30;
    y = 100;// - sin(angle) * 30;
    circlefill(tbmp, x, y, 20, COLOUR_GREY5);
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
      }
       else
        arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
     }
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
   destroy_bitmap(tbmp2);
   break;
  case 5:
   tbmp2 = create_bitmap(210, 210);
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    clear_bitmap(tbmp2);
    circlefill(tbmp2, 100, 100, 70, COLOUR_GREY4);
    circlefill(tbmp2, 100, 100, 60, COLOUR_GREY5);
/*    x = 10 - cos(angle) * 40;
    y = 10 - sin(angle) * 40;
    circlefill(tbmp, x, y, 3, 0);
    x = 10 - cos(angle) * 30;
    y = 10 - sin(angle) * 30;
    circlefill(tbmp, x, y, 3, 0);
    x = 10 - cos(angle) * 20;
    y = 10 - sin(angle) * 20;
    circlefill(tbmp, x, y, 3, 0);*/
    x = 100;// ;;+ cos(angle) * 1;
    y = 100;// + sin(angle) * 1;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 + cos(angle) * 20;
    y = 100 + sin(angle) * 20;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 + cos(angle) * 30;
    y = 100 + sin(angle) * 30;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 + cos(angle) * 40;
    y = 100 + sin(angle) * 40;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 + cos(angle) * 50;
    y = 100 + sin(angle) * 50;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 + cos(angle) * 60;
    y = 100 + sin(angle) * 60;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 + cos(angle) * 70;
    y = 100 + sin(angle) * 70;
    circlefill(tbmp2, x, y, 30, 0);
    blit(tbmp2, tbmp, 0,0,0,0,210,210);
    clear_bitmap(tbmp2);
/*    circlefill(tbmp, 10, 10, 6, COLOUR_GREY5);
    x = 100 + cos(angle) * 60;
    y = 100 + sin(angle) * 60;
    circlefill(tbmp, x, y, 8, 0);
    x = 100;// - cos(angle) * 30;
    y = 100;// - sin(angle) * 30;
    circlefill(tbmp, x, y, 4, COLOUR_GREY4);
    masked_blit(tbmp, ship_bitmaps [at] [i], 0,0,0,0,21,21);*/
    x = 100;// - cos(angle) * 30;
    y = 100;// - sin(angle) * 30;
    circlefill(tbmp, x, y, 30, COLOUR_GREY5);
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
      }
       else
        arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
     }
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
   destroy_bitmap(tbmp2);
   break;
  case 6:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle + PI) * 50;
    y1 = 100 + sin(angle + PI) * 50;
    x2 = 100 + cos(angle + PI / 2) * 90;
    y2 = 100 + sin(angle + PI / 2) * 90;
    x3 = 100 + cos(angle - PI / 2) * 90;
    y3 = 100 + sin(angle - PI / 2) * 90;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI + PI / 3) * 40;
    y2 = 100 + sin(angle + PI + PI / 3) * 40;
    x3 = 100 + cos(angle + PI - PI / 3) * 40;
    y3 = 100 + sin(angle + PI - PI / 3) * 40;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI + PI / 3) * 30;
    y2 = 100 + sin(angle + PI + PI / 3) * 30;
    x3 = 100 + cos(angle + PI - PI / 3) * 30;
    y3 = 100 + sin(angle + PI - PI / 3) * 30;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle + PI) * 40;
    y1 = 100 + sin(angle + PI) * 40;
    x2 = 100 + cos(angle + PI / 2) * 80;
    y2 = 100 + sin(angle + PI / 2) * 80;
    x3 = 100 + cos(angle - PI / 2) * 80;
    y3 = 100 + sin(angle - PI / 2) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);



    
    x1 = 100 + cos(angle) * 30;
    y1 = 100 + sin(angle) * 30;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
//  line(ship_bitmaps [at] [i], x1, y1, x2, y2, COLOUR_GREY4);
//  line(ship_bitmaps [at] [i], x2, y2, x3, y3, COLOUR_GREY4);
//  line(ship_bitmaps [at] [i], x1, y1, x3, y3, COLOUR_GREY4);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
  break;
  case 7:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    x = 100 + cos(angle) * 30;
    y = 100 + sin(angle) * 30;
    circlefill(tbmp, x, y, 20, COLOUR_GREY4);
    x1 = x + cos(angle + PI / 3) * 60;
    y1 = y + sin(angle + PI / 3) * 60;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = x + cos(angle - PI / 3) * 60;
    y1 = y + sin(angle - PI / 3) * 60;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = x + cos(angle + PI + PI / 2) * 60;
    y1 = y + sin(angle + PI + PI / 2) * 60;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = x + cos(angle + PI - PI / 2) * 70;
    y1 = y + sin(angle + PI - PI / 2) * 70;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = 100 + cos(angle + PI + PI / 3) * 60;
    y1 = 100 + sin(angle + PI + PI / 3) * 60;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = 100 + cos(angle + PI - PI / 3) * 60;
    y1 = 100 + sin(angle + PI - PI / 3) * 60;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = 100 + cos(angle + PI + PI / 6) * 70;
    y1 = 100 + sin(angle + PI + PI / 6) * 70;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = 100 + cos(angle + PI - PI / 5) * 70;
    y1 = 100 + sin(angle + PI - PI / 5) * 70;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x = 100 + cos(angle) * 30;
    y = 100 + sin(angle) * 30;
    circlefill(tbmp, x, y, 30, COLOUR_GREY5);
    
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
      }
       else
        arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
     }
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
   break;
  case 8:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI / 2) * 50;
    y2 = 100 + sin(angle + PI / 2) * 50;
    x3 = 100 + cos(angle - PI / 2) * 50;
    y3 = 100 + sin(angle - PI / 2) * 50;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI / 2) * 40;
    y2 = 100 + sin(angle + PI / 2) * 40;
    x3 = 100 + cos(angle - PI / 2) * 40;
    y3 = 100 + sin(angle - PI / 2) * 40;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI - PI / 2) * 80;
    y2 = 100 + sin(angle + PI - PI / 2) * 80;
    x3 = 100 + cos(angle + PI - PI / 4) * 80;
    y3 = 100 + sin(angle + PI - PI / 4) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI + PI / 2) * 80;
    y2 = 100 + sin(angle + PI + PI / 2) * 80;
    x3 = 100 + cos(angle + PI + PI / 4) * 80;
    y3 = 100 + sin(angle + PI + PI / 4) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI - PI / 2) * 70;
    y2 = 100 + sin(angle + PI - PI / 2) * 70;
    x3 = 100 + cos(angle + PI - PI / 4) * 70;
    y3 = 100 + sin(angle + PI - PI / 4) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI + PI / 2) * 70;
    y2 = 100 + sin(angle + PI + PI / 2) * 70;
    x3 = 100 + cos(angle + PI + PI / 4) * 70;
    y3 = 100 + sin(angle + PI + PI / 4) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 30;
    y1 = 100 + sin(angle) * 30;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
/*    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI / 2) * 40;
    y2 = 100 + sin(angle + PI / 2) * 40;
    x3 = 100 + cos(angle - PI / 2) * 40;
    y3 = 100 + sin(angle - PI / 2) * 40;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI / 2) * 30;
    y2 = 100 + sin(angle + PI / 2) * 30;
    x3 = 100 + cos(angle - PI / 2) * 30;
    y3 = 100 + sin(angle - PI / 2) * 30;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI - PI / 2) * 80;
    y2 = 100 + sin(angle + PI - PI / 2) * 80;
    x3 = 100 + cos(angle + PI - PI / 4) * 80;
    y3 = 100 + sin(angle + PI - PI / 4) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI + PI / 2) * 80;
    y2 = 100 + sin(angle + PI + PI / 2) * 80;
    x3 = 100 + cos(angle + PI + PI / 4) * 80;
    y3 = 100 + sin(angle + PI + PI / 4) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI - PI / 2) * 70;
    y2 = 100 + sin(angle + PI - PI / 2) * 70;
    x3 = 100 + cos(angle + PI - PI / 4) * 70;
    y3 = 100 + sin(angle + PI - PI / 4) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI + PI / 2) * 70;
    y2 = 100 + sin(angle + PI + PI / 2) * 70;
    x3 = 100 + cos(angle + PI + PI / 4) * 70;
    y3 = 100 + sin(angle + PI + PI / 4) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 30;
    y1 = 100 + sin(angle) * 30;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);*/
   }
  break;
  case 9:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI / 2) * 30;
    y2 = 100 + sin(angle + PI / 2) * 30;
    x3 = 100 + cos(angle - PI / 2) * 30;
    y3 = 100 + sin(angle - PI / 2) * 30;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI / 2) * 20;
    y2 = 100 + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle - PI / 2) * 20;
    y3 = 100 + sin(angle - PI / 2) * 20;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI - PI / 4) * 80;
    y2 = 100 + sin(angle + PI - PI / 4) * 80;
    x3 = 100 + cos(angle + PI - PI / 8) * 80;
    y3 = 100 + sin(angle + PI - PI / 8) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI + PI / 4) * 80;
    y2 = 100 + sin(angle + PI + PI / 4) * 80;
    x3 = 100 + cos(angle + PI + PI / 8) * 80;
    y3 = 100 + sin(angle + PI + PI / 8) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI - PI / 4) * 70;
    y2 = 100 + sin(angle + PI - PI / 4) * 70;
    x3 = 100 + cos(angle + PI - PI / 8) * 70;
    y3 = 100 + sin(angle + PI - PI / 8) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI + PI / 4) * 70;
    y2 = 100 + sin(angle + PI + PI / 4) * 70;
    x3 = 100 + cos(angle + PI + PI / 8) * 70;
    y3 = 100 + sin(angle + PI + PI / 8) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 30;
    y1 = 100 + sin(angle) * 30;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
  break;
  case 11:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
//    tbmp = create_bitmap(210, 210);
//    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    points [0] = 100 + cos(angle + PI / 2) * 30;
    points [1] = 100 + sin(angle + PI / 2) * 30;
    points [2] = 100 + cos(angle + PI / 10) * 80;
    points [3] = 100 + sin(angle + PI / 10) * 80;
    points [4] = 100 + cos(angle + PI / 7) * 90;
    points [5] = 100 + sin(angle + PI / 7) * 90;
    points [6] = 100 + cos(angle + PI - PI / 3) * 50;
    points [7] = 100 + sin(angle + PI - PI / 3) * 50;
    points [8] = 100 + cos(angle + PI + PI / 3) * 50;
    points [9] = 100 + sin(angle + PI + PI / 3) * 50;
    points [10] = 100 + cos(angle - PI / 7) * 90;
    points [11] = 100 + sin(angle - PI / 7) * 90;
    points [12] = 100 + cos(angle - PI / 10) * 80;
    points [13] = 100 + sin(angle - PI / 10) * 80;
    points [14] = 100 + cos(angle - PI / 2) * 30;
    points [15] = 100 + sin(angle - PI / 2) * 30;
    polygon(tbmp, 8, points, COLOUR_GREY4);
    
    x1 = 100 + cos(angle + PI) * 40;
    y1 = 100 + sin(angle + PI) * 40;
    x2 = 100 + cos(angle + PI / 2) * 70;
    y2 = 100 + sin(angle + PI / 2) * 70;
    x3 = 100 + cos(angle - PI / 2) * 70;
    y3 = 100 + sin(angle - PI / 2) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle + PI) * 30;
    y1 = 100 + sin(angle + PI) * 30;
    x2 = 100 + cos(angle + PI / 2) * 60;
    y2 = 100 + sin(angle + PI / 2) * 60;
    x3 = 100 + cos(angle - PI / 2) * 60;
    y3 = 100 + sin(angle - PI / 2) * 60;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    
    points [0] = 100 + cos(angle + PI / 2) * 20;
    points [1] = 100 + sin(angle + PI / 2) * 20;
    points [2] = 100 + cos(angle + PI / 8) * 70;
    points [3] = 100 + sin(angle + PI / 8) * 70;
    points [4] = 100 + cos(angle + PI / 5) * 80;
    points [5] = 100 + sin(angle + PI / 5) * 80;
    points [6] = 100 + cos(angle + PI - PI / 3) * 40;
    points [7] = 100 + sin(angle + PI - PI / 3) * 40;
    points [8] = 100 + cos(angle + PI + PI / 3) * 40;
    points [9] = 100 + sin(angle + PI + PI / 3) * 40;
    points [10] = 100 + cos(angle - PI / 5) * 80;
    points [11] = 100 + sin(angle - PI / 5) * 80;
    points [12] = 100 + cos(angle - PI / 8) * 70;
    points [13] = 100 + sin(angle - PI / 8) * 70;
    points [14] = 100 + cos(angle - PI / 2) * 20;
    points [15] = 100 + sin(angle - PI / 2) * 20;
    polygon(tbmp, 8, points, COLOUR_GREY5);



    x1 = 100;// - cos(angle) * 20;
    y1 = 100;// - sin(angle) * 20;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }

   
  break;
/*  case 12:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    points [0] = 100 + cos(angle) * 20;
    points [1] = 100 + sin(angle) * 20;
    points [2] = 100 + cos(angle + PI / 2) * 90;
    points [3] = 100 + sin(angle + PI / 2) * 90;
    points [4] = 100 + cos(angle + PI) * 20;
    points [5] = 100 + sin(angle + PI) * 20;
    points [6] = 100 + cos(angle - PI / 2) * 70;
    points [7] = 100 + sin(angle - PI / 2) * 70;
    polygon(ship_bitmaps [at] [i], 4, points, COLOUR_GREY4);
    circle(tbmp, 100, 100, 70, COLOUR_GREY5);
    x = 100;// - cos(angle) * 30;
    y = 100;// - sin(angle) * 30;
//    circle(tbmp, x, y, 7, COLOUR_GREY4);
    circle(tbmp, x, y, 80, COLOUR_GREY5);
    x = 100 + cos(angle) * 12;
    y = 100 + sin(angle) * 12;
    circlefill(tbmp, x, y, 90, 0);
    x = 10 - cos(angle) * 12;
    y = 10 - sin(angle) * 12;
    circlefill(tbmp, x, y, 80, 0);
    circlefill(tbmp, 100, 100, 30, COLOUR_GREY4);

    points [0] = 100 + cos(angle) * 1;
    points [1] = 100 + sin(angle) * 1;
    points [2] = 100 + cos(angle + PI / 2) * 80;
    points [3] = 100 + sin(angle + PI / 2) * 80;
    points [4] = 100 + cos(angle + PI) * 1;
    points [5] = 100 + sin(angle + PI) * 1;
    points [6] = 100 + cos(angle - PI / 2) * 60;
    points [7] = 100 + sin(angle - PI / 2) * 60;
    polygon(ship_bitmaps [at] [i], 4, points, COLOUR_GREY5);
    
    x = 100;// - cos(angle) * 30;
    y = 100;// - sin(angle) * 30;
//    circlefill(tbmp, x, y, 3, COLOUR_GREY4);
    circlefill(tbmp, x, y, 20, COLOUR_GREY5);

//    circlefill(tbmp, 10, 10, 1, COLOUR_GREY6);

    x = 100;// - cos(angle) * 30;
    y = 100;// - sin(angle) * 30;
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
      }
       else
        arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
     }
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
   break;*/
/*  case 13:
   for (i = 0; i < 17; i ++)
   {
//.    tbmp = create_bitmap(210, 210);
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI / 2) * 40;
    y2 = 100 + sin(angle + PI / 2) * 40;
    x3 = 100 + cos(angle - PI / 2) * 40;
    y3 = 100 + sin(angle - PI / 2) * 40;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI / 2) * 30;
    y2 = 100 + sin(angle + PI / 2) * 30;
    x3 = 100 + cos(angle - PI / 2) * 30;
    y3 = 100 + sin(angle - PI / 2) * 30;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI - PI / 4) * 80;
    y2 = 100 + sin(angle + PI - PI / 4) * 80;
    x3 = 100 + cos(angle + PI - PI / 8) * 80;
    y3 = 100 + sin(angle + PI - PI / 8) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI + PI / 4) * 80;
    y2 = 100 + sin(angle + PI + PI / 4) * 80;
    x3 = 100 + cos(angle + PI + PI / 8) * 80;
    y3 = 100 + sin(angle + PI + PI / 8) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI - PI / 4) * 70;
    y2 = 100 + sin(angle + PI - PI / 4) * 70;
    x3 = 100 + cos(angle + PI - PI / 8) * 70;
    y3 = 100 + sin(angle + PI - PI / 8) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI + PI / 4) * 70;
    y2 = 100 + sin(angle + PI + PI / 4) * 70;
    x3 = 100 + cos(angle + PI + PI / 8) * 70;
    y3 = 100 + sin(angle + PI + PI / 8) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 30;
    y1 = 100 + sin(angle) * 30;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
  break;*/
  case 12:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 90;
    y1 = 100 + sin(angle) * 90;
    x2 = 100 + cos(angle + PI / 2) * 40;
    y2 = 100 + sin(angle + PI / 2) * 40;
    x3 = 100 + cos(angle - PI / 2) * 40;
    y3 = 100 + sin(angle - PI / 2) * 40;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle + PI - PI / 3) * 90;
    y1 = 100 + sin(angle + PI - PI / 3) * 90;
    x2 = 100 + cos(angle + PI / 4) * 40;
    y2 = 100 + sin(angle + PI / 4) * 40;
    x3 = 100 + cos(angle + PI) * 20;
    y3 = 100 + sin(angle + PI) * 20;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle + PI + PI / 3) * 90;
    y1 = 100 + sin(angle + PI + PI / 3) * 90;
    x2 = 100 + cos(angle - PI / 4) * 40;
    y2 = 100 + sin(angle - PI / 4) * 40;
    x3 = 100 + cos(angle + PI) * 20;
    y3 = 100 + sin(angle + PI) * 20;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100;
    y1 = 100;
    circlefill(tbmp, x1, y1, 30, COLOUR_GREY4);

    x1 = 100 + cos(angle) * 80;
    y1 = 100 + sin(angle) * 80;
    x2 = 100 + cos(angle + PI / 2) * 30;
    y2 = 100 + sin(angle + PI / 2) * 30;
    x3 = 100 + cos(angle - PI / 2) * 30;
    y3 = 100 + sin(angle - PI / 2) * 30;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle + PI - PI / 3) * 80;
    y1 = 100 + sin(angle + PI - PI / 3) * 80;
    x2 = 100 + cos(angle + PI / 4) * 30;
    y2 = 100 + sin(angle + PI / 4) * 30;
    x3 = 100 + cos(angle + PI) * 10;
    y3 = 100 + sin(angle + PI) * 10;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle + PI + PI / 3) * 80;
    y1 = 100 + sin(angle + PI + PI / 3) * 80;
    x2 = 100 + cos(angle - PI / 4) * 30;
    y2 = 100 + sin(angle - PI / 4) * 30;
    x3 = 100 + cos(angle + PI) * 10;
    y3 = 100 + sin(angle + PI) * 10;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100;
    y1 = 100;
    circlefill(tbmp, x1, y1, 20, COLOUR_GREY5);
/*    x1 = 100 + cos(angle + PI / 2) * 80;
    y1 = 100 + sin(angle + PI / 2) * 80;
    x2 = 100;// + cos(angle + PI / 2) * 20;
    y2 = 100;// + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle + PI / 2 + PI / 5) * 80;
    y3 = 100 + sin(angle + PI / 2 + PI / 5) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle - PI / 2) * 80;
    y1 = 100 + sin(angle - PI / 2) * 80;
    x2 = 100;// + cos(angle + PI / 2) * 20;
    y2 = 100;// + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle - PI / 2 - PI / 5) * 80;
    y3 = 100 + sin(angle - PI / 2 - PI / 5) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI / 2) * 35;
    y2 = 100 + sin(angle + PI / 2) * 35;
    x3 = 100 + cos(angle - PI / 2) * 35;
    y3 = 100 + sin(angle - PI / 2) * 35;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle + PI / 2) * 70;
    y1 = 100 + sin(angle + PI / 2) * 70;
    x2 = 100;// + cos(angle + PI / 2) * 20;
    y2 = 100;// + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle + PI / 2 + PI / 5) * 70;
    y3 = 100 + sin(angle + PI / 2 + PI / 5) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle - PI / 2) * 70;
    y1 = 100 + sin(angle - PI / 2) * 70;
    x2 = 100;// + cos(angle + PI / 2) * 20;
    y2 = 100;// + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle - PI / 2 - PI / 5) * 70;
    y3 = 100 + sin(angle - PI / 2 - PI / 5) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);*/
    x1 = 100 + cos(angle) * 40;
    y1 = 100 + sin(angle) * 40;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
  break;
  case 13:
   tbmp2 = create_bitmap(210, 210);
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    clear_bitmap(tbmp2);
    x1 = 100 - cos(angle) * 20;
    y1 = 100 - sin(angle) * 20;
    circlefill(tbmp2, x1, y1, 80, COLOUR_GREY4);
    circlefill(tbmp2, x1, y1, 70, COLOUR_GREY5);
//    x = 100;// ;;+ cos(angle) * 1;
//    y = 100;// + sin(angle) * 1;
//    circlefill(tbmp2, x, y, 30, 0);
    x = 100 - cos(angle) * 70;
    y = 100 - sin(angle) * 70;
    circlefill(tbmp2, x, y, 75, 0);
/*    x = 100 - cos(angle) * 30;
    y = 100 - sin(angle) * 30;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 - cos(angle) * 40;
    y = 100 - sin(angle) * 40;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 - cos(angle) * 50;
    y = 100 - sin(angle) * 50;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 - cos(angle) * 60;
    y = 100 - sin(angle) * 60;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 - cos(angle) * 70;
    y = 100 - sin(angle) * 70;
    circlefill(tbmp2, x, y, 30, 0);*/
    blit(tbmp2, tbmp, 0,0,0,0,210,210);
    clear_bitmap(tbmp2);
    x = x1 + cos(angle + PI / 4 + PI / 6) * 40;
    y = y1 + sin(angle + PI / 4 + PI / 6) * 40;
    circlefill(tbmp, x, y, 20, COLOUR_GREY4);
    circlefill(tbmp, x, y, 10, COLOUR_GREY5);
    x = x1 + cos(angle - PI / 4 - PI / 6) * 40;
    y = y1 + sin(angle - PI / 4 - PI / 6) * 40;
    circlefill(tbmp, x, y, 20, COLOUR_GREY4);
    circlefill(tbmp, x, y, 10, COLOUR_GREY5);
    x = 100 - cos(angle) * 20;
    y = 100 - sin(angle) * 20;
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
    }
       arc(tbmp, x, y, itofix(a1), itofix(a2), 50, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 49, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 48, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 47, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 46, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 45, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 44, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 43, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 42, COLOUR_WHITE);
/*    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 50, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 49, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 48, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 47, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 46, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(tbmp, x, y, itofix(a1), itofix(a2), 50, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 49, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 48, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 47, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 46, COLOUR_WHITE);
      }
       else
       {
        arc(tbmp, x, y, itofix(a1), itofix(a2), 50, COLOUR_WHITE);
        arc(tbmp, x, y, itofix(a1), itofix(a2), 49, COLOUR_WHITE);
        arc(tbmp, x, y, itofix(a1), itofix(a2), 48, COLOUR_WHITE);
        arc(tbmp, x, y, itofix(a1), itofix(a2), 47, COLOUR_WHITE);
        arc(tbmp, x, y, itofix(a1), itofix(a2), 46, COLOUR_WHITE);
       }
     }*/
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
   destroy_bitmap(tbmp2);
   break;
  case 14:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 80;
    y1 = 100 + sin(angle) * 80;
    x2 = 100 + cos(angle + PI / 2) * 40;
    y2 = 100 + sin(angle + PI / 2) * 40;
    x3 = 100 + cos(angle - PI / 2) * 40;
    y3 = 100 + sin(angle - PI / 2) * 40;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle + PI / 2) * 80;
    y1 = 100 + sin(angle + PI / 2) * 80;
    x2 = 100;// + cos(angle + PI / 2) * 20;
    y2 = 100;// + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle + PI / 2 + PI / 5) * 80;
    y3 = 100 + sin(angle + PI / 2 + PI / 5) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle - PI / 2) * 80;
    y1 = 100 + sin(angle - PI / 2) * 80;
    x2 = 100;// + cos(angle + PI / 2) * 20;
    y2 = 100;// + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle - PI / 2 - PI / 5) * 80;
    y3 = 100 + sin(angle - PI / 2 - PI / 5) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI / 2) * 35;
    y2 = 100 + sin(angle + PI / 2) * 35;
    x3 = 100 + cos(angle - PI / 2) * 35;
    y3 = 100 + sin(angle - PI / 2) * 35;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle + PI / 2) * 70;
    y1 = 100 + sin(angle + PI / 2) * 70;
    x2 = 100;// + cos(angle + PI / 2) * 20;
    y2 = 100;// + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle + PI / 2 + PI / 5) * 70;
    y3 = 100 + sin(angle + PI / 2 + PI / 5) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle - PI / 2) * 70;
    y1 = 100 + sin(angle - PI / 2) * 70;
    x2 = 100;// + cos(angle + PI / 2) * 20;
    y2 = 100;// + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle - PI / 2 - PI / 5) * 70;
    y3 = 100 + sin(angle - PI / 2 - PI / 5) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 40;
    y1 = 100 + sin(angle) * 40;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
  break;
  case 15:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle + PI / 26) * 90;
    y1 = 100 + sin(angle + PI / 26) * 90;
    x2 = 100 + cos(angle + PI / 2) * 60;
    y2 = 100 + sin(angle + PI / 2) * 60;
    x3 = 100 + cos(angle + PI - PI / 26) * 60;
    y3 = 100 + sin(angle + PI - PI / 26) * 60;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle - PI / 26) * 90;
    y1 = 100 + sin(angle - PI / 26) * 90;
    x2 = 100 + cos(angle - PI / 2) * 60;
    y2 = 100 + sin(angle - PI / 2) * 60;
    x3 = 100 + cos(angle + PI + PI / 26) * 60;
    y3 = 100 + sin(angle + PI + PI / 26) * 60;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
/*    x1 = 100 + cos(angle + PI - PI / 4) * 50;
    y1 = 100 + sin(angle + PI - PI / 4) * 50;
    circlefill(tbmp, x1, y1, 35, COLOUR_GREY4);
    x1 = 100 + cos(angle + PI + PI / 4) * 50;
    y1 = 100 + sin(angle + PI + PI / 4) * 50;
    circlefill(tbmp, x1, y1, 35, COLOUR_GREY4);*/
    x1 = 100 - cos(angle) * 30;
    y1 = 100 - sin(angle) * 30;
    circlefill(tbmp, x1, y1, 40, COLOUR_GREY4);
    x1 = 100 + cos(angle + PI / 26) * 80;
    y1 = 100 + sin(angle + PI / 26) * 80;
    x2 = 100 + cos(angle + PI / 2) * 50;
    y2 = 100 + sin(angle + PI / 2) * 50;
    x3 = 100 + cos(angle + PI - PI / 26) * 50;
    y3 = 100 + sin(angle + PI - PI / 26) * 50;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle - PI / 26) * 80;
    y1 = 100 + sin(angle - PI / 26) * 80;
    x2 = 100 + cos(angle - PI / 2) * 50;
    y2 = 100 + sin(angle - PI / 2) * 50;
    x3 = 100 + cos(angle + PI + PI / 26) * 50;
    y3 = 100 + sin(angle + PI + PI / 26) * 50;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 - cos(angle) * 30;
    y1 = 100 - sin(angle) * 30;
    circlefill(tbmp, x1, y1, 30, COLOUR_GREY5);
    x1 = 100 - cos(angle) * 20;
    y1 = 100 - sin(angle) * 20;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
  break;


  
 }

 destroy_bitmap(tbmp);
}


