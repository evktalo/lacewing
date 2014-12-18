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
    
File: palette.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - palette and transparency stuff.
Various enums are in palette.h.

*/

/*
Lacewing
Copyright (C) 2003 Captain Pork

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

    You should be able to reach Captain Pork by sending an email to
    captainpork@fastmail.fm or sending a letter to
    Captain Pork, 3/4 Kensington Rd, Rose Park, SA 5067 Australia
    but my lease is a short-term one so email is probably your best bet.

File: Palette.c
History:
1/5/03 - Started (Capt Pork)

This file contains:
The palette functions

*/

#include "allegro.h"


#include "config.h"
#include "palette.h"
#include "globvars.h"

COLOR_MAP trans_table;
int blend_function(int x, int y, RGB *rgbl);
int limit_colour(int colour_input);
int average_colours(int colour1, int colour2);

RGB other_palet [1324];
RGB palet [256];
RGB other_palet2 [2048];
// what seems to be a weird bug in Allegro forces me to put padding around
//  the palet array, or it becomes corrupted. The same thing happened in
//  World of Violence and I have no idea why.

int adjust_lit_colour(int col, int lit);
int add_light_to_colour(int col, int lit);
void pork_create_color_table(COLOR_MAP *table, AL_CONST PALETTE pal);
int get_lower_colour(int y);
int colour_illumination(int y);
int base_colour(int y);
int colour_brightness(int y);

void colour_table(const char *which_call);
//void colour_table(void);

//RGB mono_palet [256];


int base_palette [64] [3] =
{
{0, 0, 0},
{0, 0, 1},
{10, 10, 10},
{20, 20, 20},
{30, 30, 30},
{40, 40, 40},
{50, 50, 50},
{63, 63, 63}, // greys
{8, 3, 0},
{16, 6, 0},
{24, 9, 0},
{32, 12, 0},
{40, 15, 0},
{48, 18, 0},
{56, 21, 0},
{63, 24, 0}, // Oranges
{8, 8, 0},
{16, 16, 0},
{24, 24, 4},
{32, 32, 8},
{40, 40, 12},
{48, 48, 16},
{56, 56, 20},
{63, 63, 24}, // yellows
{8, 0, 0},
{16, 0, 0},
{24, 0, 0},
{32, 3, 3},
{40, 5, 5},
{48, 7, 7},
{56, 9, 9},
{63, 11, 11}, // Reds
{0, 8, 0},
{0, 16, 0},
{3, 24, 3},
{6, 32, 6},
{8, 40, 8},
{12, 48, 12},
{16, 56, 16},
{20, 63, 20}, // Greens
{0, 0, 8},
{0, 0, 16},
{4, 4, 24},
{8, 8, 32},
{12, 12, 40},
{16, 16, 48},
{20, 20, 56},
{24, 24, 63}, // Blues
{8, 0, 8},
{16, 0, 16},
{24, 0, 24},
{32, 0, 32},
{40, 0, 40},
{48, 0, 48},
{56, 0, 56},
{63, 0, 63}, // Purples
{8, 3, 8},
{16, 5, 16},
{24, 9, 24},
{32, 12, 32},
{40, 34, 40},
{48, 40, 48},
{56, 50, 56},
{63, 55, 63}, // X colours

};

void init_palette(void)
{

int i;


// colour_table("First");

 for (i = 0; i < 64; i ++)
 {
    // Grey
    palet[i + 128].r = limit_colour(base_palette [i] [0] + 20);
    palet[i + 128].g = limit_colour(base_palette [i] [1] + 20);
    palet[i + 128].b = limit_colour(base_palette [i] [2] + 20);

    palet[i + 192].r = base_palette [i] [0];
    palet[i + 192].g = base_palette [i] [1];
    palet[i + 192].b = base_palette [i] [2];

    
  }

 for (i = 0; i < 8; i ++)
 {
    // White - should be same as WHITE2 (below)
//    palet[i].r = limit_colour(32 + i * 4);
//    palet[i].g = limit_colour(32 + i * 4);
//    palet[i].b = limit_colour(32 + i * 4);
    palet[i].r = limit_colour(48 + i * 2);
    palet[i].g = limit_colour(48 + i * 2);
    palet[i].b = limit_colour(48 + i * 2);

    // Purple
    palet[i + 8].r = limit_colour(32 + i * 4);
    palet[i + 8].g = limit_colour(i * 3);
    palet[i + 8].b = limit_colour(32 + i * 4);

    // LBlue
    palet[i + 16].r = limit_colour(10 + i * 3);
    palet[i + 16].g = limit_colour(10 + i * 3);
    palet[i + 16].b = limit_colour(47 + i * 2);

    // DBlue
    palet[i + 24].r = limit_colour(i * 2);
    palet[i + 24].g = limit_colour(i * 2);
    palet[i + 24].b = limit_colour(21 + i * 4);

    // Yellow
    palet[i + 32].r = limit_colour(47 + i * 3);
    palet[i + 32].g = limit_colour(47 + i * 3);
    palet[i + 32].b = limit_colour(10 + i * 3);

    // LGreen
    palet[i + 40].r = limit_colour(i * 3);
    palet[i + 40].g = limit_colour(32 + i * 4);
    palet[i + 40].b = limit_colour(i * 3);

    // DGreen
    palet[i + 48].r = limit_colour(i * 2);
    palet[i + 48].g = limit_colour(21 + i * 4);
    palet[i + 48].b = limit_colour(i * 2);

    // LOrange
    palet[i + 56].r = limit_colour(47 + i * 3);
    palet[i + 56].g = limit_colour(20 + i * 3);
    palet[i + 56].b = limit_colour(5 + i * 2);

    // DOrange
    palet[i + 64].r = limit_colour(25 + i * 4);
    palet[i + 64].g = limit_colour(12 + i * 3);
    palet[i + 64].b = limit_colour(i * 2);

    // LRed
    palet[i + 72].r = limit_colour(47 + i * 2);
    palet[i + 72].g = limit_colour(10 + i * 3);
    palet[i + 72].b = limit_colour(10 + i * 3);

    // DRed
    palet[i + 80].r = limit_colour(20 + i * 4);
    palet[i + 80].g = limit_colour(i * 2);
    palet[i + 80].b = limit_colour(i * 2);

    // White2 - should be same as WHITE (above)
//    palet[i + 88].r = limit_colour(32 + i * 4);
//    palet[i + 88].g = limit_colour(32 + i * 4);
//    palet[i + 88].b = limit_colour(32 + i * 4);
    palet[i + 88].r = limit_colour(48 + i * 2);
    palet[i + 88].g = limit_colour(48 + i * 2);
    palet[i + 88].b = limit_colour(48 + i * 2);

    // LGrey
    palet[i + 96].r = limit_colour(24 + i * 4);
    palet[i + 96].g = limit_colour(24 + i * 4);
    palet[i + 96].b = limit_colour(24 + i * 4);

    // DGrey
    palet[i + 104].r = limit_colour(12 + i * 4);
    palet[i + 104].g = limit_colour(12 + i * 4);
    palet[i + 104].b = limit_colour(12 + i * 4);


/*    // White
    palet[i].r = limit_colour(21 + i * 6);
    palet[i].g = limit_colour(21 + i * 6);
    palet[i].b = limit_colour(21 + i * 6);

    // Purple
    palet[i + 8].r = limit_colour(21 + i * 6);
    palet[i + 8].g = limit_colour(i * 3 - 12);
    palet[i + 8].b = limit_colour(21 + i * 6);

    // LBlue
    palet[i + 16].r = limit_colour(i * 4 - 12);
    palet[i + 16].g = limit_colour(i * 4 - 12);
    palet[i + 16].b = limit_colour(21 + i * 6);

    // DBlue
    palet[i + 24].r = limit_colour(i * 2 - 12);
    palet[i + 24].g = limit_colour(i * 2 - 12);
    palet[i + 24].b = limit_colour(10 + i * 5);

    // LGreen
    palet[i + 32].r = limit_colour(i * 4 - 12);
    palet[i + 32].g = limit_colour(21 + i * 6);
    palet[i + 32].b = limit_colour(i * 4 - 12);

    // DGreen
    palet[i + 40].r = limit_colour(i * 2 - 12);
    palet[i + 40].g = limit_colour(10 + i * 5);
    palet[i + 40].b = limit_colour(i * 2 - 12);

    // Yellow
    palet[i + 48].r = limit_colour(21 + i * 6);
    palet[i + 48].g = limit_colour(21 + i * 6);
    palet[i + 48].b = limit_colour(i * 4 - 12);

    // LOrange
    palet[i + 56].r = limit_colour(21 + i * 6);
    palet[i + 56].g = limit_colour(12 + i * 5);
    palet[i + 56].b = limit_colour(i * 4 - 12);

    // DOrange
    palet[i + 64].r = limit_colour(10 + i * 5);
    palet[i + 64].g = limit_colour(6 + i * 4);
    palet[i + 64].b = limit_colour(i * 4 - 12);

    // LRed
    palet[i + 72].r = limit_colour(21 + i * 6);
    palet[i + 72].g = limit_colour(i * 4 - 12);
    palet[i + 72].b = limit_colour(i * 4 - 12);

    // DRed
    palet[i + 80].r = limit_colour(10 + i * 5);
    palet[i + 80].g = limit_colour(i * 2 - 12);
    palet[i + 80].b = limit_colour(i * 2 - 12);
*/
 }

  palet[0].r = 0;
  palet[0].g = 0;
  palet[0].b = 0;


/*  for (i = 0; i < 192; i ++)
  {
   palet[i].r = 0;
   palet[i].b = 0;
   palet[i].g = 0;
  }*/


   set_palette(palet);

   pork_create_color_table(&trans_table, palet);

   color_map = &trans_table;

   set_palette(palet);
/*
   BITMAP *pbmp = create_bitmap(16, 16);

   int x, y;

   for (x = 0; x < 16; x ++)
   {
    for (y = 0; y < 16; y ++)
    {
     putpixel(pbmp, y, x, y + (x * 16));
     if (y + (x * 16) < 192)
      putpixel(pbmp, y, x, 0);
    }
   }

   save_bitmap("palbmp3.pcx", pbmp, palet);

   for (x = 192; x < 256; x ++)
   {
    rectfill(screen, (x - 192) * 6, 1, (x - 192) * 6 + 6, 10, x);
   }

   do
   {

   } while (key [KEY_U] == 0);
*/
// int j;

/* for (i = 0; i < 256; i ++)
 {
  j = limit_colour((palet[i].r + palet[i].g + palet[i].b) / 3);
  mono_palet [i].r = j;
  mono_palet [i].g = j;
  mono_palet [i].b = j;
 }

 mono_palet [222].r = 60;
 mono_palet [222].g = 40;
 mono_palet [222].b = 0;
 mono_palet [223].r = 0;
 mono_palet [223].g = 60;
 mono_palet [223].b = 10; // these colours used for 'you win'/'game over'
*/
}

int limit_colour(int colour_input)
{

 if (colour_input < 0) return 0;
 if (colour_input > 63) return 63;
 return colour_input;

}

int average_colours(int colour1, int colour2)
{

 int avge = colour1 + colour2 / 2;
 if (avge < 0) return 0;
 if (avge > 63) return 63;
 return avge;

}


int blend_function(int x, int y, RGB *rgbl)
{

// if (x % 8 == 0)
// {
//  return y;
// }

 if (x == TRANS_GREY)
 {
  if (y < 192)
   return y;

  return y - 64;
 }

 if (x >= TRANS_WHITE && x < TRANS_END)
 {
//  if (y / 8 > (x - TRANS_WHITE) * 8
//      && y < 128)
//       return y; // don't overwrite lower transparencies
/*  if (y / 8 > (x - TRANS_WHITE)
      && y < 128)
       return y; // don't overwrite lower transparencies
       
  return y % 8 + (x - TRANS_WHITE) * 8;*/

/*  y = 50
  x = TRANS_DRED
  x - TRANS_WHITE = 10
  if (6 < (10) || FALSE)
   return 50 % 8 + 80 = 82

  return 50;*/

  if (y / 8 < (x - TRANS_WHITE)
      && y < 128)
   return y; // don't overwrite lower transparencies


  if (y % 8 == 0)
   return y % 8 + (x - TRANS_WHITE) * 8 + 1;


   return y % 8 + (x - TRANS_WHITE) * 8;

 }

 return y;

}



int get_lower_colour(int y)
{

  if (y % 32 == COLOUR_WHITE % 32)
   return 7;
  
  if (y % 32 >= COLOUR_BLACK % 32 && y % 32 <= COLOUR_WHITE % 32)
   return (y % 32);
   
  if (y % 32 >= COLOUR_ORANGE1 % 32 && y % 32 <= COLOUR_BLUE4 % 32)
   return ((((y % 32) - (COLOUR_ORANGE1 % 32)) % 4 + 1) * 3) / 2;

 return 7;

}

int colour_to_trans(int y)
{

 switch(y)
 {
  case COLOUR_GREY1:
  case COLOUR_GREY2:
  case COLOUR_GREY3:
  case COLOUR_GREY4:
   return TRANS_GREY;
  case COLOUR_GREY5:
  case COLOUR_GREY6:
  case COLOUR_WHITE:
   return TRANS_WHITE;
  case COLOUR_ORANGE1:
  case COLOUR_ORANGE2:
  case COLOUR_ORANGE3:
  case COLOUR_ORANGE4:
   return TRANS_DORANGE;
  case COLOUR_ORANGE5:
  case COLOUR_ORANGE6:
  case COLOUR_ORANGE7:
  case COLOUR_ORANGE8:
   return TRANS_LORANGE;
  case COLOUR_BLUE1:
  case COLOUR_BLUE2:
  case COLOUR_BLUE3:
  case COLOUR_BLUE4:
   return TRANS_DBLUE;
  case COLOUR_BLUE5:
  case COLOUR_BLUE6:
  case COLOUR_BLUE7:
  case COLOUR_BLUE8:
   return TRANS_LBLUE;
  case COLOUR_RED1:
  case COLOUR_RED2:
  case COLOUR_RED3:
  case COLOUR_RED4:
   return TRANS_DRED;
  case COLOUR_RED5:
  case COLOUR_RED6:
  case COLOUR_RED7:
  case COLOUR_RED8:
   return TRANS_LRED;
  case COLOUR_YELLOW1:
  case COLOUR_YELLOW2:
  case COLOUR_YELLOW3:
  case COLOUR_YELLOW4:
   return TRANS_YELLOW;
  case COLOUR_YELLOW5:
  case COLOUR_YELLOW6:
  case COLOUR_YELLOW7:
  case COLOUR_YELLOW8:
   return TRANS_YELLOW;
  case COLOUR_GREEN1:
  case COLOUR_GREEN2:
  case COLOUR_GREEN3:
  case COLOUR_GREEN4:
   return TRANS_DGREEN;
  case COLOUR_GREEN5:
  case COLOUR_GREEN6:
  case COLOUR_GREEN7:
  case COLOUR_GREEN8:
   return TRANS_LGREEN;
  case COLOUR_PURPLE1:
  case COLOUR_PURPLE2:
  case COLOUR_PURPLE3:
  case COLOUR_PURPLE4:
  case COLOUR_PURPLE5:
  case COLOUR_PURPLE6:
  case COLOUR_PURPLE7:
  case COLOUR_PURPLE8:
   return TRANS_PURPLE;

 default:
  return TRANS_WHITE;

 }

}


int base_colour(int y)
{
 switch(y % 32)
 {
  case COLOUR_GREY1:
  case COLOUR_GREY2:
  case COLOUR_GREY3:
  case COLOUR_GREY4:
  case COLOUR_GREY5:
  case COLOUR_GREY6:
  case COLOUR_WHITE:
   return COLOUR_GREY1;
  case COLOUR_ORANGE1:
  case COLOUR_ORANGE2:
  case COLOUR_ORANGE3:
  case COLOUR_ORANGE4:
   return COLOUR_ORANGE1;
  case COLOUR_BLUE1:
  case COLOUR_BLUE2:
  case COLOUR_BLUE3:
  case COLOUR_BLUE4:
   return COLOUR_BLUE1;
  case COLOUR_RED1:
  case COLOUR_RED2:
  case COLOUR_RED3:
  case COLOUR_RED4:
   return COLOUR_RED1;
  case COLOUR_YELLOW1:
  case COLOUR_YELLOW2:
  case COLOUR_YELLOW3:
  case COLOUR_YELLOW4:
   return COLOUR_YELLOW1;
  case COLOUR_GREEN1:
  case COLOUR_GREEN2:
  case COLOUR_GREEN3:
  case COLOUR_GREEN4:
   return COLOUR_GREEN1;
 }

 return COLOUR_BLACK;

}


/*
This function had to be modified from the allegro create_color_table
because the allegro version used bestfit_color, whereas we need
specific color values (eg so that there can be multiple blacks to carry
information in areas of the screen that haven't been lightsourced yet)
*/
void pork_create_color_table(COLOR_MAP *table, AL_CONST PALETTE pal)
//void pork_create_color_table(COLOR_MAP *table, AL_CONST PALETTE pal, (int *)(blend)(AL_CONST PALETTE pal, int x, int y, RGB *rgb), void (*callback)(int pos))
{
   int x, y, z;
   RGB c;

   for (x=0; x<PAL_SIZE; x++) {
      for (y=0; y<PAL_SIZE; y++) {
   z = blend_function(x, y, &c);

//   if (rgb_map)
      table->data[x][y] = z;//rgb_map->data[c.r>>1][c.g>>1][c.b>>1];
//   else
//      table->data[x][y] = bestfit_color(pal, c.r, c.g, c.b);
      }

   }
}


