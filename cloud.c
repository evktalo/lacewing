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
    
File: cloud.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - functions relating to clouds and transient special effects

*/
#include "allegro.h"

#include <math.h>

#include "config.h"
#include "globvars.h"

#include "stuff.h"
#include "palette.h"

#include "sound.h"

void manage_cloud(int cl);
int move_cloud(int mc, char limited);

void destroy_cloud(int c);
void drag_cloud(int i);

void simple_cloud(int colour, int x, int y, int xs, int ys, int size);
void simple_cloud_trans(int colour, int x, int y, int xs, int ys, int size);
void place_explosion_with_smoke(int x, int y, int xs, int ys, int size, int colours [5]);



void init_clouds(void)
{

 int c;

 for (c = 0; c < NO_CLOUDS; c++)
 {
  cloud[c].type = CLOUD_NONE;
 }

}

// should delta_tickrate be unsigned?
int create_cloud(int type, int x, int y, int x2, int y2,
int x_speed, int y_speed, int timer, int tickrate,
int delta_tickrate, int angle,
int status, unsigned char seed, int colours [5])
{

   int c = 0;

   for (c = 1; c < NO_CLOUDS; c++)
   {
    if (c == NO_CLOUDS - 1) return -1;
    if (cloud[c].type == CLOUD_NONE) break;
   }

   cloud[c].type = type;
   cloud[c].x = x;// + (x_speed - actor[owner].x_speed);
   cloud[c].y = y;// + (y_speed - actor[owner].y_speed);
   cloud[c].x2 = x2;
   cloud[c].y2 = y2;
   cloud[c].x_speed = x_speed;
   cloud[c].y_speed = y_speed;
   cloud[c].seed = seed;
   cloud[c].timeout = timer;
   cloud[c].tickrate= tickrate;
   cloud[c].delta_tickrate= delta_tickrate;
   cloud[c].status = status;
   cloud[c].angle = angle;
   cloud[c].colour [0] = colours [0];
   cloud[c].colour [1] = colours [1];
   cloud[c].colour [2] = colours [2];
   cloud[c].colour [3] = colours [3];
   cloud[c].colour [4] = colours [4];
   
   return c;

}



void run_clouds(void)
{

 int c;

 for (c = 1; c < NO_CLOUDS; c++)
 {
  if (cloud[c].type != CLOUD_NONE) manage_cloud(c);
 }

}


void manage_cloud(int c)
{

 int passing_colours [5];

//   int x_gain = 0;
//   int y_gain = 0;
//   char angle_change;

//   if (cloud[c].timeout > 0)
//   {
    cloud[c].timeout -= cloud[c].tickrate;
    cloud[c].tickrate += cloud[c].delta_tickrate;
//   }

   if (cloud[c].timeout <= 0)
   {
    destroy_cloud(c);
    return;
   }

   switch(cloud[c].type)
   {
    case CLOUD_BURSTLET:
//     place_explosion(cloud[c].x, cloud[c].y, 0, 0, , cloud[c].colours);
     passing_colours [0] = cloud[c].colour [1]; // was [0]
   
     create_cloud(CLOUD_MED_TRANS_CIRCLE,
      cloud[c].x, cloud[c].y, 0, 0, 0, 0,
      50 + grand(300),0,3, 0, 0, 0, passing_colours);
//     passing_colours [0] = cloud[c].colour [1];

//     create_cloud(CLOUD_MED_TRANS_CIRCLE,
//      cloud[c].x, cloud[c].y, 0, 0, 0, 0, size,-40,15, 0, 0, 0, passing_colours);

     passing_colours [0] = cloud[c].colour [2];

     create_cloud(CLOUD_MED_TRANS_CIRCLE,
      cloud[c].x, cloud[c].y, 0, 0, 0, 0,
      50 + grand(300),0,6, 0, 0, 0, passing_colours);
     break;
    case CLOUD_SPAWNER:
     if (cloud[c].timeout % 2 == 0)
     {
       int cangle = grand(ANGLE_FULL);
       int cx = cloud[c].x + cos(angle_to_radians(cangle)) * grand(cloud[c].status * 100);
       int cy = cloud[c].y + sin(angle_to_radians(cangle)) * grand(cloud[c].status * 100);
//       create_cloud(CLOUD_MED_TRANS_CIRCLE,
//        cloud[c].x, cloud[c].y, 0, 0, 0, 0,
//        50 + grand(300),0,3, 0, 0, 0, passing_colours);
       place_explosion_with_smoke(
//        cloud[c].x - cloud[c].status * 100 + grand(cloud[c].status * 200),
//        cloud[c].y - cloud[c].status * 100 + grand(cloud[c].status * 200),
          cx, cy,
          0, 0, grand(cloud[c].status) + 250, cloud[c].colour);
       if (cloud[c].timeout % 8 == 0)
        play_sound_pos(WAV_MINEBANG, 200 + grand(200), 200, cx, cy);

     }
     break;
   }

   if (move_cloud(c, 1) == 0)
    return;

 drag_cloud(c);

}


void destroy_cloud(int c)
{
 cloud[c].type = CLOUD_NONE;
}


int move_cloud(int c, char limited)
{


if (cloud[c].x_speed == 0 && cloud[c].y_speed == 0) return 1;


cloud[c].x2 += cloud[c].x_speed;
cloud[c].y2 += cloud[c].y_speed;

cloud[c].x += cloud[c].x_speed;
cloud[c].y += cloud[c].y_speed;

return 1;

}




void drag_cloud(int i)
{

 cloud[i].x_speed *= 1 - game.drag;
 cloud[i].y_speed *= 1 - game.drag;

}

void simple_cloud(int colour, int x, int y, int xs, int ys, int size)
{

   int passing_colours [5];

   passing_colours [0] = colour;
   
   create_cloud(CLOUD_MED_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-30,10, 0, 0, 0, passing_colours);


}

void simple_cloud_trans(int colour, int x, int y, int xs, int ys, int size)
{

   int passing_colours [5];

   passing_colours [0] = colour;
   
   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-30,10, 0, 0, 0, passing_colours);


}


void place_explosion(int x, int y, int xs, int ys, int size, int colours [5])
{

   int passing_colours [5];


   passing_colours [0] = colours [0];
   
   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-100,10, 0, 0, 0, passing_colours);

   passing_colours [0] = colours [1];

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-120,15, 0, 0, 0, passing_colours);

   passing_colours [0] = colours [2];

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-140,20, 0, 0, 0, passing_colours);

}

void place_explosion_with_smoke(int x, int y, int xs, int ys, int size, int colours [5])
{

   int passing_colours [5];

   passing_colours [0] = TRANS_GREY;

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-30,1, 0, 0, 0, passing_colours);


   passing_colours [0] = colours [0];
   
   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-100,10, 0, 0, 0, passing_colours);

   passing_colours [0] = colours [1];

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-105,15, 0, 0, 0, passing_colours);

   passing_colours [0] = colours [2];

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-110,20, 0, 0, 0, passing_colours);

}

void place_rocket_trail(int x, int y, int xs, int ys, int size, int colours [5])
{

   int passing_colours [5];


   passing_colours [0] = colours [0];
   
   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-6,1, 0, 0, 0, passing_colours);

   passing_colours [0] = colours [1];

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-15,6, 0, 0, 0, passing_colours);

   passing_colours [0] = colours [2];

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-20,15, 0, 0, 0, passing_colours);

}


void place_speck_burst(int x, int y, int xs, int ys, int specks, int min_timeout, int random_timeout, int scatter, int colours [5])
{

  int i;

  for (i = 0; i < specks; i ++)
  {
   create_cloud(CLOUD_SPECK,
    x,
    y,
    0, 0,
    xs + grand(scatter * 2 + 1) - scatter,
    ys + grand(scatter * 2 + 1) - scatter,
    min_timeout + grand(random_timeout), 1,0,0, 0, 0, colours);
  }

}

void place_line_burst(int x, int y, int xs, int ys, int specks, int min_timeout, int random_timeout, int burst, int random_burst, int colours [5])
{

  int i, angle, xsp, ysp, burst_speed;

  for (i = 0; i < specks; i ++)
  {
   angle = grand(ANGLE_FULL);
   burst_speed = burst + grand(burst);
   xsp = xs + cos(angle_to_radians(angle)) * burst_speed;
   ysp = ys + sin(angle_to_radians(angle)) * burst_speed;
   x += xsp;
   y += ysp;
   create_cloud(CLOUD_SHRINKING_LINE,
    x,
    y,
    0, 0,
    xsp,
    ysp,
    (min_timeout + grand(random_timeout)) * 10, 10 + grand(6),0,angle, 0, 0, colours);
  }

}


void place_burstlet_burst(int x, int y, int xs, int ys, int burstlets, int min_timeout, int random_timeout, int burst, int random_burst, int pretravel, int colours [5])
{

int i, xs2, ys2, x2, y2;

  for (i = 0; i < burstlets; i ++)
  {
   xs2 = burst + grand(random_burst);
   ys2 = burst + grand(random_burst);
   if (grand(2) == 0)
    xs2 *= -1;
   if (grand(2) == 0)
    ys2 *= -1;
   xs2 += xs;
   ys2 += ys;
   x2 = x + xs2 * pretravel; // or it will just be masked by the main
   y2 = y + ys2 * pretravel; //  explosion

   create_cloud(CLOUD_BURSTLET,
    x2, y2, 0,0, xs2, ys2,
    min_timeout + random_timeout,1,0, 0, 0, 0, colours);
  }


}
