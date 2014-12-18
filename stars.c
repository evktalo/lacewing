#include "allegro.h"

#include <stdlib.h>

#include "config.h"
#include "globvars.h"
#include "palette.h"

#include "stuff.h"

void replace_star(int i, int j, int k);
void place_star(int i, int j, int x, int y);


void init_stars(void)
{
 int i = 0;
 int j = 0;

 for (j = 0; j < 2; j ++)
 {
  for (i = 0; i < NO_STARS; i ++)
  {
   place_star(i, j, grand(1200 * GRAIN), grand(1200 * GRAIN));
  }
 }


}

void place_star(int i, int j, int x, int y)
{
   star[i].x [j] = x;
   star[i].y [j] = y;
   star[i].colour [j] = COLOUR_GREY5 + grand(2);
   star[i].distance [j] = 1 + grand(3);
   if (grand(5) == 0)
   {
    star[i].colour [j] = COLOUR_GREY6 + grand(2);
    star[i].distance [j] = 3 + grand(3);
   }
   if (grand(15) == 0)
   {
    star[i].colour [j] = COLOUR_WHITE;
    star[i].distance [j] = 6 + grand(3);
   }

//   if (grand(7) == 0)
//    star[i].distance [j] = grand(3) + 1;
}


void run_stars(void)
{
 int i = 0;
 int j = 0;
 int k = 1;

 if (game[0].users == 2)
  k = 2;

 for (j = 0; j < k; j ++)
 {
  for (i = 0; i < NO_STARS; i ++)
  {
   star[i].x [j] -= (actor[player[j].actor_controlled].x_speed * star[i].distance [j]) / 8;
   star[i].y [j] -= (actor[player[j].actor_controlled].y_speed * star[i].distance [j]) / 8;
   if (star[i].x [j] < 0
       || star[i].x [j] > 1200 * GRAIN
       || star[i].y [j] < 0
       || star[i].y [j] > 1200 * GRAIN)
       {
        replace_star(i, j, player[j].actor_controlled);
       }

  }
 }

}


void replace_star(int i, int j, int k)
{

// star[i].x [j] = grand(1500) * GRAIN;
// star[i].y [j] = grand(1500) * GRAIN;
 int x, y;

 if (grand(abs(actor[k].x_speed) + abs(actor[k].y_speed)) < abs(actor[k].x_speed))
 {
  y = grand(1200 * GRAIN);
  if (actor[k].x_speed > 0)
   x = 1150 * GRAIN;
    else
     x = 50;
 }
  else
  {
   x = grand(1200 * GRAIN);
   if (actor[k].y_speed > 0)
    y = 1150 * GRAIN;
     else
      y = 50;
  }

  place_star(i, j, x, y);

}


