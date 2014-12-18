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
    
File: pickup.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - pickups (energy, repair etc)

*/

#include "allegro.h"

#include "config.h"
#include "globvars.h"

#include "stuff.h"
#include "palette.h"
#include "cloud.h"

void manage_pickup(int p);

void destroy_pickup(int p);
void pickup_explodes(int p, int taken);



void init_pickups(void)
{

 int p;

 for (p = 0; p < NO_PICKUPS; p++)
 {
  pickup[p].type = PICKUP_NONE;
 }

}

int create_pickup(int type, int x, int y, int timeout)
{

   if (arena.hostile > 0)
    return -1;

   int p;

   for (p = 0; p < NO_PICKUPS; p++)
   {
    if (p == NO_PICKUPS - 1) return -1;
    if (pickup[p].type == PICKUP_NONE) break;
   }

   pickup[p].type = type;
   pickup[p].x = x;
   pickup[p].y = y;
   pickup[p].radius = 5000; // generous
   pickup[p].timeout = timeout;
   pickup[p].counter = 256 - counter;
   
   return p;

}



void run_pickups(void)
{

 int p;

 for (p = 0; p < NO_PICKUPS; p++)
 {
  if (pickup[p].type != PICKUP_NONE)
   manage_pickup(p);
 }

}


void manage_pickup(int p)
{
   pickup[p].timeout --;
   pickup[p].counter ++;
   if (pickup[p].timeout <= 0)
   {
    pickup_explodes(p, 0);
    return;
   }
}

void pickup_explodes(int p, int taken)
{
  int passing_colours [5];
  switch(pickup[p].type)
  {
   default:
   passing_colours [3] = TRANS_WHITE;
   passing_colours [2] = TRANS_YELLOW;
   passing_colours [1] = TRANS_YELLOW;
   passing_colours [0] = TRANS_LORANGE;
    break;
   case PICKUP_UPGRADE2:
   passing_colours [3] = TRANS_YELLOW;
   passing_colours [2] = TRANS_LRED;
   passing_colours [1] = TRANS_DRED;
   passing_colours [0] = TRANS_DRED;
    break;
   case PICKUP_UPGRADE3:
   passing_colours [3] = TRANS_WHITE;
   passing_colours [2] = TRANS_PURPLE;
   passing_colours [1] = TRANS_LBLUE;
   passing_colours [0] = TRANS_DBLUE;
    break;
   case PICKUP_REPAIR:
   passing_colours [3] = TRANS_WHITE;
   passing_colours [2] = TRANS_YELLOW;
   passing_colours [1] = TRANS_LORANGE;
   passing_colours [0] = TRANS_DORANGE;
    break;
  }
      create_cloud(CLOUD_SHOCKWAVE,
       pickup[p].x,
       pickup[p].y,
       0, 0,
       0,
       0,
       150,10,0, 0, 150, 0, passing_colours);

 destroy_pickup(p);
 return;
}

void destroy_pickup(int p)
{
 pickup[p].type = PICKUP_NONE;
}


void explode_all_pickups(void)
{

 int p;

 for (p = 0; p < NO_PICKUPS; p++)
 {
  if (pickup[p].type != PICKUP_NONE)
   pickup_explodes(p, 0);
 }

}


