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
    
File: eclass.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - the enemy structure

*/

#include "config.h"

#include "globvars.h"

struct enemy_classification eclass [NO_ENEMY_TYPES] =
{
{
0, // int max_armour;
0, // level
0, // common
0, // score
0, // int radius;
0, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
0, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Bouncer
500, // int max_armour;
0, // level
0, // common
10, // score
7000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Crawler
400, // int max_armour;
0, // level
0, // common
10, // score
7000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Faller - this armour only for 1st section - see create_enemy for others
250, // int max_armour;
0, // level
0, // common
10, // score
7000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Stinger
250, // int max_armour;
1, // level
4, // common
20, // score
7000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Swerver
350, // int max_armour;
0, // level
0, // common
10, // score
7000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Jelly
1500, // int max_armour;
7, // level
3, // common
75, // score
12000, // int radius;
13000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
150, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Minethrower
1500, // int max_armour;
8, // level
3, // common
80, // score
8000, // int radius;
8000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
150, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Firebase
5000, // int max_armour;
0, // level
0, // common
500, // score
18000, // int radius;
74000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
300, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Turret - firebase
3000, // int max_armour;
0, // level
0, // common
100, // score
8000, // int radius;
7000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
250, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Firebase2
6000, // int max_armour;
0, // level
0, // common
750, // score
15000, // int radius;
34000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
300, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Crawler2
400, // int max_armour;
0, // level
0, // common
15, // score
7000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Crawler3
400, // int max_armour;
0, // level
0, // common
25, // score
7000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Crawler4 - DOESN'T EXIST
1500, // int max_armour;
0, // level
0, // common
25, // score
7000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Swerver2
350, // int max_armour;
0, // level
0, // common
15, // score
7000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Swerver3
350, // int max_armour;
0, // level
0, // common
25, // score
7000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Bouncer-burst
500, // int max_armour;
0, // level
0, // common
10, // score
7000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Bouncer-seeker burst
500, // int max_armour;
0, // level
0, // common
10, // score
7000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Bouncer-shoots
500, // int max_armour;
0, // level
0, // common
10, // score
7000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // bouncer-mines
500, // int max_armour;
0, // level
0, // common
10, // score
7000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // superjelly
3000, // int max_armour;
10, // level
3, // common
150, // score
9000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
200, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // stinger2
1100, // int max_armour;
6, // level
3, // common
45, // score
7000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
125, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Hive
12000, // int max_armour;
0, // level
0, // common
3000, // score
13000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
500, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Bee
1200, // int max_armour;
0, // level
0, // common
100, // score
3000, // int radius;
1000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
50, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Drone
500, // int max_armour;
0, // level
0, // common
10, // score
9000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Drone-attack
500, // int max_armour;
0, // level
0, // common
10, // score
9000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Drifter
600, // int max_armour;
4, // level
4, // common
60, // score
9000, // int radius;
5000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
125, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Sun
2500, // int max_armour;
8, // level
2, // common
150, // score
10000, // int radius;
5000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
150, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Green Sun
3500, // int max_armour;
11, // level
2, // common
250, // score
10000, // int radius;
5000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
170, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Blue Sun
4500, // int max_armour;
13, // level
2, // common
500, // score
10000, // int radius;
5000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
190, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Wanderer
2000, // int max_armour;
7, // level
3, // common
95, // score
12000, // int radius;
7000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
200, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Dead Wanderer
500, // int max_armour;
0, // level
0, // common
0, // score
0, // int radius;
9000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Minethrower2
2000, // int max_armour;
13, // level
3, // common
125, // score
7000, // int radius;
8000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
150, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Fighter
1000, // int max_armour;
7, // level
3, // common
120, // score
8000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Fighter2
1300, // int max_armour;
10, // level
3, // common
200, // score
8000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Fighter3
1600, // int max_armour;
13, // level
3, // common
300, // score
8000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Curver
3500, // int max_armour;
11, // level
3, // common
300, // score
9000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
250, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Blaster
3000, // int max_armour;
14, // level
3, // common
500, // score
8000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
200, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Blaster turret
1200, // int max_armour;
0, // level
0, // common
200, // score
7000, // int radius;
3000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Dead Fighter
500, // int max_armour;
0, // level
0, // common
0, // score
8000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // stinger3
1800, // int max_armour;
9, // level
4, // common
150, // score
7000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
150, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Turret1 - firebase2
3500, // int max_armour;
0, // level
0, // common
400, // score
7000, // int radius;
7000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
250, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Turret2 - firebase2
3500, // int max_armour;
0, // level
0, // common
400, // score
7000, // int radius;
7000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
250, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Firebase3
8000, // int max_armour;
0, // level
0, // common
2000, // score
22000, // int radius;
53000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
500, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Firebase3 turret
4000, // int max_armour;
0, // level
0, // common
450, // score
7000, // int radius;
7000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
300, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Dead Fighter2
500, // int max_armour;
0, // level
0, // common
0, // score
8000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Dead Fighter3
500, // int max_armour;
0, // level
0, // common
0, // score
8000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Dead Fighter4
500, // int max_armour;
0, // level
0, // common
0, // score
16000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Fighter4
3000, // int max_armour;
15, // level
4, // common
500, // score
16000, // int radius;
14000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
250, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Fighter4 turret
1500, // int max_armour;
0, // level
0, // common
100, // score
7000, // int radius;
9000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
150, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Drifter2
1500, // int max_armour;
8, // level
4, // common
135, // score
9000, // int radius;
7000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
150, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Drifter3
2500, // int max_armour;
12, // level
4, // common
200, // score
9000, // int radius;
7000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
150, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Flakker
2000, // int max_armour;
14, // level
4, // common
600, // score
12000, // int radius;
8000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
170, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Cloaker
1000, // int max_armour;
12, // level
2, // common
450, // score
9000, // int radius;
5000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Cloaker2
1500, // int max_armour;
15, // level
2, // common
650, // score
9000, // int radius;
5000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Boss1
5500, // int max_armour;
0, // level
0, // common
500, // score
27000, // int radius;
100000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
300, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Boss1 Turret1
4000, // int max_armour;
0, // level
0, // common
200, // score
9000, // int radius;
7000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Boss1 Turret2
1500, // int max_armour;
0, // level
0, // common
200, // score
11000, // int radius;
7000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Boss2
5000, // int max_armour;
0, // level
0, // common
1500, // score
27000, // int radius;
120000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
400, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Boss2 turret
2500, // int max_armour;
0, // level
0, // common
250, // score
15000, // int radius;
7000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // stinger4
2200, // int max_armour;
12, // level
4, // common
300, // score
9000, // int radius;
6000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
170, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Bolter
3500, // int max_armour;
12, // level
4, // common
400, // score
11000, // int radius;
7000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
170, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Bolter2
5000, // int max_armour;
14, // level
3, // common
600, // score
12000, // int radius;
8000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
200, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Flakker2
2500, // int max_armour;
16, // level
3, // common
800, // score
10000, // int radius;
6000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
200, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Torper
3000, // int max_armour;
15, // level
3, // common
900, // score
9000, // int radius;
5000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
150, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Boss3
13000, // int max_armour;
0, // level
0, // common
2500, // score
23000, // int radius;
120000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
500, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Boss3 turret1
6000, // int max_armour;
0, // level
0, // common
450, // score
15000, // int radius;
7000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
200, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Boss3 turret2
6000, // int max_armour;
0, // level
0, // common
450, // score - should be same as 1 (only 1 is used if main body destroyed)
18000, // int radius;
7000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
200, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Boss4
22000, // int max_armour;
0, // level
0, // common
4000, // score
27000, // int radius;
120000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
600, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Boss4 turret
10000, // int max_armour;
0, // level
0, // common
600, // score
10000, // int radius;
7000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
200, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Torper2
1200, // int max_armour;
12, // level
3, // common
500, // score
7000, // int radius;
3000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
120, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Hurler
700, // int max_armour;
4, // level
3, // common
35, // score
8000, // int radius;
5000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
100, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Hurler2
1600, // int max_armour;
8, // level
3, // common
100, // score
9000, // int radius;
6000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
300, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
},
{ // Hurler3
1100, // int max_armour;
13, // level
3, // common
550, // score
8000, // int radius;
4000, // int edge_radius;
0, // int max_shield;
0, // char drag_affected
120, // mass
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
}

};

/*
NOTES:

- More bees per hive in coop games
- Last boss releases wasps (bigger, redder bees)

*/
