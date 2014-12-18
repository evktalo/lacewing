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
    
File: stuff.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - a few common functions

*/

#include "allegro.h"

#include <math.h>
#include <stdlib.h>

#include "config.h"

int turn_towards_angle(int angle, int tangle, int turning);

float angle_to_radians(int angle)
{
 return ((float) angle / ANGLE_FULL) * PI * 2;
}

int radians_to_angle(float angle)
{
 return (angle * ANGLE_FULL) / (PI * 2);
}

fixed angle_to_fixed(int angle)
{
 return itofix(angle / ANGLE_TO_FIXED);
}

int grand(int number)
{
 if (number == 0)
  return 0;
 return random() % number;
}

int crandom(int number)
{
 if (number == 0)
  return 0;
 return random() % number;
}

int turn_towards_angle(int angle, int tangle, int turning)
{

 if ((angle < tangle && tangle > angle + ANGLE_HALF)
     || (angle > tangle && tangle > angle - ANGLE_HALF))
 {
  angle -= turning;
  if (angle < 0)
   angle += ANGLE_FULL;
 }
  else
  {
   angle += turning;
   if (angle > ANGLE_FULL)
    angle -= ANGLE_FULL;
  }
  
 return angle;
 
}


int turn_towards_xy(int x1, int y1, int x2, int y2, int angle, int turning)
{

  int tangle =
    radians_to_angle(atan2((y2 - y1), (x2 - x1)));
   if (tangle < 0)
    tangle += ANGLE_FULL;
   if (tangle > ANGLE_FULL)
    tangle -= ANGLE_FULL;

  return turn_towards_angle(angle, tangle, turning);

}

