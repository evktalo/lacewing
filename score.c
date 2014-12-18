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

#define NO_SCORES 20
// also #def'd in menu.c


struct score_list
{
  int score;
  int ship;
  int level;
  char name [20];
};

extern struct score_list hs_single [NO_SCORES];
extern struct score_list hs_coop [NO_SCORES];

extern struct score_list *hscore;

