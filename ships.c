#include "config.h"

#include "ships.h"

#include "globvars.h"

struct ship_list ships [NO_SHIP_TYPES] =
{
{
"No Ship",
0, // radius
0, // turning
0, // thrust
0, // mass
0, // recharge
0, // energy
0, // shield type
0, // shield strength1
0, // shield strength2
0, // system 1
0, // system 2
0, // system 3
0, // system 1 status 1
0, // system 2 status 1
0, // system 3 status 1
0, // system 1 status 2
0, // system 2 status 2
0, // system 3 status 2
0, // brake type
0, // brake strength
0 // armour
}
,
{
"Fighter 1",
6000, // radius
0, // edge_radius
70, // turning
100, // thrust
50, // slide
100, // mass
50, // recharge
10000, // energy
SHIELD_MEDIUMFREQ, // shield type
10, // shield strength1
10, // shield strength2
SYST_WPN_ZAPPER, // system 1
SYST_NONE, // system 2
SYST_NONE, // system 3
0, // system 1 status 1
0, // system 2 status 1
0, // system 3 status 1
0, // system 1 status 2
0, // system 2 status 2
0, // system 3 status 2
BRAKES_DRAG, // brake type
5, // brake strength
1000 // armour                   },
{
"Arrow",
6000, // radius
6000, // edge_radius
50, // turning
70, // thrust
0, // slide
200, // mass
150, // recharge
15000, // energy
SHIELD_HIGHFREQ, // shield type
20, // shield strength1
20, // shield strength2
SYST_WPN_ZAPPER, // system 1
SYST_NONE, // system 2
SYST_NONE, // system 3
0, // system 1 status 1
0, // system 2 status 1
0, // system 3 status 1
0, // system 1 status 2
0, // system 2 status 2
0, // system 3 status 2
BRAKES_RETRO, // brake type
50, // brake strength
1500 // armour
}


};




void init_actor_as_ship(int i, int ship)
{
 int j;

 actor[i].type = ship;
 actor[i].radius = ships[ship].radius;
 actor[i].edge_radius = ships[ship].edge_radius;
 actor[i].turning = ships[ship].turning;
 actor[i].thrust = ships[ship].thrust;
 actor[i].slide = ships[ship].slide;
 actor[i].mass = ships[ship].mass;
 actor[i].recharge = ships[ship].recharge;
 actor[i].just_upgraded = 0;
 actor[i].just_upgraded_timeout = 0;
// actor[i].max_energy = ships[ship].energy;
// actor[i].energy = ships[ship].energy;
 actor[i].shield_pulse = 0;
 actor[i].recycle1 = 0;
 actor[i].recycle2 = 0;
 actor[i].cannon_status = 0;
/* actor[i].shield_type = ships[ship].shield_type;
 actor[i].shield_strength1 = ships[ship].shield_strength1;
 actor[i].shield_strength2 = ships[ship].shield_strength2;
 actor[i].shield_pulse = 0;
 actor[i].system [0] = ships[ship].system1;
 actor[i].system [1] = ships[ship].system2;
 actor[i].system [2] = ships[ship].system3;
 actor[i].system_status1 [0] = ships[ship].system1_status1;
 actor[i].system_status1 [1] = ships[ship].system2_status1;
 actor[i].system_status1 [2] = ships[ship].system3_status1;
 actor[i].system_status2 [0] = ships[ship].system1_status2;
 actor[i].system_status2 [1] = ships[ship].system2_status2;
 actor[i].system_status2 [2] = ships[ship].system3_status2;*/
 actor[i].upgrade_slot = 0;
 for (j = 0; j < NO_UPGRADES; j ++)
 {
  actor[i].upgrades [j] = 0;
  actor[i].upgrade_specials [j] = 0;
 }
 actor[i].upgrade_structure [0] = UPG_NONE;
 actor[i].upgrade_structure [1] = UPG_SPEED;
 actor[i].upgrade_structure [2] = UPG_POWER;
 actor[i].upgrade_structure [3] = UPG_AUTOFIRE;
 actor[i].upgrade_structure [4] = UPG_MULTIFIRE;
 actor[i].upgrade_structure [5] = UPG_ARMOUR;
 actor[i].upgrade_structure [6] = UPG_PROJECT;
 actor[i].upgrade_structure [7] = UPG_TUBES;
 actor[i].upgrade_structure [8] = UPG_SPECIAL;
 actor[i].brakes = BRAKES_DRAG; //ships[ship].brake_type;
 actor[i].brake_strength = 5; //ships[ship].brake_strength;
 actor[i].armour = 1000; //ships[ship].armour1;
 actor[i].max_armour = 1000; //ships[ship].armour1;
 

}

