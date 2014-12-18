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
    
File: upgrades.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - the upgrade structure for all ships

*/

#include "config.h"


int upgrades [18] [9] =
{
{
UPG_NONE,
UPG_SPEED,
UPG_PROJECT,
UPG_AUTOFIRE,
UPG_ARMOUR,
UPG_WARHEAD,
UPG_POWER,
UPG_SIDEKICK,
UPG_SHIELD,
}, // Lacewing - fighter
{
UPG_NONE,
UPG_AUTOFIRE,
UPG_ARMOUR,
UPG_SPEED,
UPG_BOMB,
UPG_PROJECT,
UPG_LAUNCHER,
UPG_ORBITAL,
UPG_SHIELD,
}, // Capybara - medium bomber
{
UPG_NONE,
UPG_ROCKET,
UPG_SPEED,
UPG_WARHEAD,
UPG_SEEKER,
UPG_ARMOUR,
UPG_AUTOFIRE,
UPG_SIDEKICK,
UPG_SHIELD,
}, // Despot - medium missiles
{
UPG_NONE,
UPG_AUTOFIRE,
UPG_SPEED,
UPG_TUBES,
UPG_SEEKER,
UPG_ARMOUR,
UPG_ROCKET,
UPG_ORBITAL,
UPG_SHIELD,
}, // Hookworn - lots of homing missiles
{
UPG_NONE,
UPG_ARMOUR,
UPG_LAUNCHER,
UPG_POWER,
UPG_BOMB,
UPG_SPEED,
UPG_PROJECT,
UPG_SHIELD,
UPG_TURRET,
}, // Lentil
{
UPG_NONE,
UPG_ARMOUR,
UPG_ROCKET,
UPG_POWER,
UPG_TUBES,
UPG_SPEED,
UPG_WARHEAD,
UPG_TURRET,
UPG_SHIELD,
}, // Porkupine - lots of non-homing missiles
{
UPG_NONE,
UPG_POWER,
UPG_SPEED,
UPG_MULTIFIRE,
UPG_PROJECT,
UPG_WARHEAD,
UPG_ARMOUR,
UPG_ORBITAL,
UPG_SHIELD,
}, // Prong - super cannons
{
UPG_NONE,
UPG_SPEED,
UPG_SLIDE,
UPG_AUTOFIRE,
UPG_RETRO,
UPG_ARMOUR,
UPG_POWER,
UPG_HEAVY,
UPG_SHIELD,
}, // False Scorpion
{
UPG_NONE,
UPG_AUTOFIRE,
UPG_SLIDE,
UPG_SPEED,
UPG_BOMB,
UPG_ARMOUR,
UPG_POWER,
UPG_SIDEKICK,
UPG_SHIELD,
}, // Tortfeasor
{
UPG_NONE,
UPG_MULTIFIRE,
UPG_SPEED,
UPG_PROJECT,
UPG_AUTOFIRE,
UPG_ROCKET,
UPG_ARMOUR,
UPG_BACKFIRE,
UPG_SHIELD,
}, // Aether Squid
{
UPG_NONE,
UPG_ARMOUR,
UPG_SLIDE,
UPG_MULTIFIRE,
UPG_PROJECT,
UPG_BOMB,
UPG_SPEED,
UPG_RETRO,
UPG_BACKFIRE,
}, // Rumsfeld
{
UPG_NONE,
UPG_ROCKET,
UPG_ARMOUR,
UPG_AUTOFIRE,
UPG_TUBES,
UPG_MULTIFIRE,
UPG_SPEED,
UPG_HEAVY,
UPG_SHIELD,
/*UPG_NONE,
UPG_ROCKET,
UPG_ARMOUR,
UPG_POWER,
UPG_TUBES,
UPG_MULTIFIRE,
UPG_AUTOFIRE,
UPG_SPEED,
UPG_SHIELD,*/

}, // Godbotherer
{
UPG_NONE,
UPG_ROCKET,
UPG_SPEED,
UPG_LOADER,
UPG_POWER,
UPG_ARMOUR,
UPG_PROJECT,
UPG_ORBITAL,
UPG_SHIELD,
}, // Botulus
{
UPG_NONE,
UPG_SPEED,
UPG_AUTOFIRE,
UPG_LOADER,
UPG_SEEKER,
UPG_SLIDE,
UPG_ARMOUR,
UPG_SIDEKICK,
UPG_SHIELD,
}, // Slidewinder
{
UPG_NONE,
UPG_SPEED,
UPG_AUTOFIRE,
UPG_POWER,
UPG_ARMOUR,
UPG_BOMB,
UPG_PROJECT,
UPG_ORBITAL,
UPG_DRIFT,
} // Doom Fork
};


/*

Ships:

Lacewing - pointy: fighter
Aether Squid - spiny: multifire
Lentil - rounded: heavy bomber
Rumsfeld's Delight - triangle: silly
Hookworm - small round wing: many homing missiles
Tortfeasor - 2 prongs: light bomber (+ slide)
False Scorpion: - bracketed: agile
Porkupine - large round wing: multi missiles

Despot - wide lacewing - just 2 homing missiles
Capybara - blobby - medium bomber
Prong - pointy2 - super cannons

*/


