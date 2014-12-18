#define NO_CLOUDS 1000
#define NO_ACTORS 25
#define NO_BULLETS 1000
#define NO_PLAYERS 2
#define GRAIN 1000
//#define NO_CMDS 12
#define NO_STARS 100
#define NO_PICKUPS 20

#define MAX_SCORE 9999999
#define GRID_WIDTH 50

#define NO_WEAPONS 2
#define NO_SHIP_TYPES 17

#define NO_SYSTEM_TYPES 3

#define NO_UPGRADES 9

#define FULL_UPGRADE 5

#define NO_ENEMY_ATTRIBUTES 10
#define NO_ENEMIES 40
//#define NO_ENEMY_TYPES 50

#define ANGLE_FULL_HEX 0x400
#define ANGLE_FULL 1024
#define ANGLE_HALF 512
#define ANGLE_QUARTER 256
#define ANGLE_1_EIGHTH 128
#define ANGLE_1_SIXTEENTH 64
#define ANGLE_3_EIGHTHS 384
#define ANGLE_3_SIXTEENTHS 192
#define ANGLE_TO_FIXED 4

#define OWNER_ENEMY -1

#define ATTACK_NONE -1

struct armoury
{
 char *name;
 int type;
 int speed;
 int timeout;
 int damage;
 int mass;

};

enum
{
TARGET_PRIMARY, // need to kill to advance
TARGET_NO, // turrets etc
TARGET_CRAWLY // crawlies...
};

enum
{
CMD_THRUST,
CMD_LEFT,
CMD_RIGHT,
CMD_FIRE1,
CMD_FIRE2,
CMD_BRAKE,
CMD_LEFT1,
CMD_RIGHT1,
CMD_UPGRADE,
CMD_LINK,
NO_CMDS
};

enum
{
GAME_SINGLE,
GAME_COOP,
GAME_DUEL
};

enum
{
SECONDARY_NONE,
SECONDARY_MISSILE,
SECONDARY_BOMB
};

enum
{
ACTORTYPE_NONE,
ACTORTYPE_SHIP,
ACTORTYPE_REDSHIP

};

enum
{
SHIP_LACEWING,
SHIP_CAPYBARA,
SHIP_DESPOT,
SHIP_HOOKWORM,
SHIP_LENTIL,
SHIP_PORKUPINE,
SHIP_PRONG,
SHIP_SCORPION,
SHIP_TORTFEASOR,
SHIP_AETHER,
SHIP_RUMSFELD,
SHIP_GODBOTHERER,
SHIP_BOTULUS,
SHIP_SLIDEWINDER,
SHIP_DOOM

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

Godbotherer - fat triangle - heavy fighter - multi, autof, heavy cannons, tubes?
Botulus - backwards curve - fast rocketed missiles - rocket, loader, multi, power,
Slidewinder - three triangles - fast seeker missiles - seek, loader, auto, slide
Doom Fork - two prongs forwards - drifter - auto, power, rocket, whead
 
*/

/*
Aether Squid
Lentil
*/

enum
{
ENEMY_NONE,
ENEMY_BOUNCER,
ENEMY_CRAWLER,
ENEMY_FALLER,
ENEMY_STINGER,
ENEMY_SWERVER,
ENEMY_JELLY,
ENEMY_MINETHROWER,
ENEMY_FIREBASE,
ENEMY_FIREBASE_TURRET,
ENEMY_FIREBASE2,
ENEMY_CRAWLER2,
ENEMY_CRAWLER3,
ENEMY_CRAWLER4,
ENEMY_SWERVER2,
ENEMY_SWERVER3,
ENEMY_BOUNCER2,
ENEMY_BOUNCER3,
ENEMY_BOUNCER4,
ENEMY_BOUNCER5,
ENEMY_SUPERJELLY,
ENEMY_STINGER2,
ENEMY_HIVE,
ENEMY_BEE,
// have bees orbit hive in order determined by their index in attributes
ENEMY_DRONE,
ENEMY_DRONE_ATTACK,
ENEMY_DRIFTER,
ENEMY_SUN,
ENEMY_SUN_GREEN,
ENEMY_SUN_BLUE,
ENEMY_WANDERER,
ENEMY_DEAD_WANDERER,
ENEMY_MINETHROWER2,
ENEMY_FIGHTER,
ENEMY_FIGHTER2,
ENEMY_FIGHTER3,
ENEMY_CURVER,
ENEMY_BLASTER,
ENEMY_BLASTER_TURRET,
ENEMY_DEAD_FIGHTER,
ENEMY_STINGER3,
ENEMY_FIREBASE2_TURRET1,
ENEMY_FIREBASE2_TURRET2,
ENEMY_FIREBASE3,
ENEMY_FIREBASE3_TURRET,
ENEMY_DEAD_FIGHTER2,
ENEMY_DEAD_FIGHTER3,
ENEMY_DEAD_FIGHTER4,
ENEMY_FIGHTER4,
ENEMY_FIGHTER4_TURRET,
ENEMY_DRIFTER2,
ENEMY_DRIFTER3,
ENEMY_FLAKKER,
ENEMY_CLOAKER,
ENEMY_CLOAKER2,
ENEMY_BOSS1,
ENEMY_BOSS1_TURRET1,
ENEMY_BOSS1_TURRET2,
ENEMY_BOSS2,
ENEMY_BOSS2_TURRET,
ENEMY_STINGER4,
ENEMY_BOLTER,
ENEMY_BOLTER2,
ENEMY_FLAKKER2,
ENEMY_TORPER,
ENEMY_BOSS3,
ENEMY_BOSS3_TURRET1,
ENEMY_BOSS3_TURRET2,
ENEMY_BOSS4,
ENEMY_BOSS4_TURRET,
ENEMY_TORPER2,
ENEMY_HURLER,
ENEMY_HURLER2,
ENEMY_HURLER3,
NO_ENEMY_TYPES
};

/*
Bosses:
All have main circle in centre

// Note in briefing: must destroy central brain. Other parts help but not
//  essential

Basic:
Centre fires seekers
Four turrets fire random zaps

Wheel:
Circle fires zaps at actors
Surrounded by 3-4 turrets which fire series of radiating bolts
Turrets change turning direction when one is destroyed, and speed up

Horseshoe wreath:
Circle fires bursts of radiating blobs
turrets are shielded and fire random zaps

Final flea-thing
has hideous moving legs and turrets around central brain like spider-eyes
centre fires streams of bolts
2 kinds of turrets (which may be shielded):
- random zaps
- a special kind of bee

*/

enum
{
PICKUP_NONE,
PICKUP_REPAIR,
PICKUP_GRACE,
PICKUP_SHIP,
PICKUP_UPGRADE1,
PICKUP_UPGRADE2,
PICKUP_UPGRADE3
};

enum
{
SUBTYPE_NONE, // for creepy-crawlies
SUBTYPE_GREEN,
SUBTYPE_YELLOW,
SUBTYPE_ORANGE,
SUBTYPE_RED,
SUBTYPE_BLUE,
SUBTYPE_PURPLE,
SUBTYPE_WHITE
};

enum
{
ATTRIB_CRAWLER_DIRECTION_X,
ATTRIB_CRAWLER_AXIS,
ATTRIB_CRAWLER_SPEED,
ATTRIB_CRAWLER_ARMOUR
};

enum
{
ATTRIB_FALLER_SPEED,
ATTRIB_FALLER_SECTIONS,
ATTRIB_FALLER_ARMOUR
};

enum
{
ATTRIB_SWERVER_ANGLE,
ATTRIB_SWERVER_TURN,
ATTRIB_SWERVER_SPEED,
ATTRIB_SWERVER3_ANGLE2,
ATTRIB_SWERVER3_ATTACKING
};

enum
{
ATTRIB_BOUNCER_SPIN,
ATTRIB_BOUNCER_DELTA_SPIN,
ATTRIB_BOUNCER_ANGLE2 // only the BOUNCER5 enemy uses this
};

enum
{
ATTRIB_BOLTER_SPIN,
ATTRIB_BOLTER_DELTA_SPIN,
ATTRIB_BOLTER_ANGLE2 // only the BOUNCER5 enemy uses this
};

enum
{
ATTRIB_SUN_COUNTER,
ATTRIB_SUN_COUNT_SPEED,
ATTRIB_SUN_COUNT_START
};

enum
{
ATTRIB_DRIFTER_X_DEST,
ATTRIB_DRIFTER_Y_DEST,
ATTRIB_DRIFTER_ACCEL,
ATTRIB_DRIFTER_RANGE,
ATTRIB_DRIFTER_LUMP_ANGLE,
ATTRIB_DRIFTER_SPIN,
ATTRIB_DRIFTER_DELTA_SPIN,
ATTRIB_DRIFTER3_ORBITAL,
ATTRIB_DRIFTER3_ORBITAL_DIR
};

enum
{
ATTRIB_WANDERER_X_DEST,
ATTRIB_WANDERER_Y_DEST,
ATTRIB_WANDERER_NEXT_MINE
};

enum
{
ATTRIB_DEAD_WANDERER_TIMEOUT
};

enum
{
ATTRIB_MINETHROWER_X_DEST,
ATTRIB_MINETHROWER_Y_DEST,
ATTRIB_MINETHROWER_NEXT_MINE
};

enum
{
ATTRIB_JELLY_X_DEST,
ATTRIB_JELLY_Y_DEST,
ATTRIB_JELLY_PULSE,
ATTRIB_JELLY_PULSE_SPEED
};

enum
{
ATTRIB_FLAKKER_X_DEST,
ATTRIB_FLAKKER_Y_DEST
};

enum
{
ATTRIB_FIGHTER_X_DEST,
ATTRIB_FIGHTER_Y_DEST,
ATTRIB_FIGHTER_MODE,
ATTRIB_FIGHTER_THRUST,
ATTRIB_FIGHTER_TURN,
ATTRIB_FIGHTER4_TURRET1,
ATTRIB_FIGHTER4_TURRET2,
ATTRIB_FIGHTER_NEW_DEST // set when it runs into an actor

};

enum
{
ATTRIB_TORPER2_X_DEST,
ATTRIB_TORPER2_Y_DEST
};

enum
{
ATTRIB_HURLER_X_DEST,
ATTRIB_HURLER_Y_DEST
};

enum
{
ATTRIB_HURLER2_X_DEST,
ATTRIB_HURLER2_Y_DEST
};

enum
{
ATTRIB_HURLER3_X_DEST,
ATTRIB_HURLER3_Y_DEST
};

enum
{
ATTRIB_BLASTER_TURRET1,
ATTRIB_BLASTER_TURRET2
};

enum
{
ATTRIB_BTURRET_MAIN,
ATTRIB_BTURRET_MAIN_DEAD
};

enum
{
ATTRIB_FIREBASE_TURRET1,
ATTRIB_FIREBASE_TURRET2,
ATTRIB_FIREBASE_TURRET3,
ATTRIB_FIREBASE_TURRET4
};

enum
{
ATTRIB_FBTURRET_MAIN,
ATTRIB_FBTURRET_MAIN_DEAD
};

enum
{
ATTRIB_FIREBASE2_TURRET1,
ATTRIB_FIREBASE2_TURRET2
};

enum
{
ATTRIB_FB2TURRET_MAIN,
ATTRIB_FB2TURRET_MAIN_DEAD
};

enum
{
ATTRIB_FIREBASE3_TURRET1,
ATTRIB_FIREBASE3_TURRET2,
ATTRIB_FIREBASE3_TURRET3,
ATTRIB_FIREBASE3_TURRET4
};

enum
{
ATTRIB_FB3TURRET_MAIN,
ATTRIB_FB3TURRET_MAIN_DEAD
};


enum
{
ATTRIB_FI4TURRET_MAIN,
ATTRIB_FI4TURRET_MAIN_DEAD
};

enum
{
ATTRIB_CLOAKER_X_DEST,
ATTRIB_CLOAKER_Y_DEST,
ATTRIB_CLOAKER_CLOAKED,
ATTRIB_CLOAKER_PULSE,
ATTRIB_CLOAKER_ACCEL
};

enum
{
ATTRIB_HIVE_ATTACK13,
ATTRIB_HIVE_ATTACK24,
ATTRIB_HIVE_ANGRY
/*ATTRIB_HIVE_BEE2,
ATTRIB_HIVE_BEE3,
ATTRIB_HIVE_BEE4*/
};

/*enum
{
ATTRIB_BEE_HIVE,
ATTRIB_BEE_X_DEST,
ATTRIB_BEE_Y_DEST,
ATTRIB_BEE_MODE,
ATTRIB_BEE_NUMBER
};*/

enum
{
ATTRIB_BEE_HIVE,
ATTRIB_BEE_X_DEST,
ATTRIB_BEE_Y_DEST,
ATTRIB_BEE_MODE,
ATTRIB_BEE_NUMBER,
ATTRIB_BEE_RANGE
};

enum
{
ATTRIB_BOSS1_X_DEST,
ATTRIB_BOSS1_Y_DEST,
ATTRIB_BOSS1_TURRET1,
ATTRIB_BOSS1_TURRET2,
ATTRIB_BOSS1_TURRET3,
ATTRIB_BOSS1_TURRET4
};

enum
{
ATTRIB_BO1TURRET_MAIN,
ATTRIB_BO1TURRET_MAIN_DEAD
};

enum
{
ATTRIB_BOSS2_X_DEST,
ATTRIB_BOSS2_Y_DEST,
ATTRIB_BOSS2_TURRET1,
ATTRIB_BOSS2_TURRET2,
ATTRIB_BOSS2_TURRET3,
ATTRIB_BOSS2_TURRET4,
ATTRIB_BOSS2_SPIN,
ATTRIB_BOSS2_DELTA_SPIN,
ATTRIB_BOSS2_TURRETS_LEFT,
ATTRIB_BOSS2_TURRET_NOISE
};

enum
{
ATTRIB_BO2TURRET_MAIN,
ATTRIB_BO2TURRET_MAIN_DEAD
};

enum
{
//ATTRIB_BOSS3_X_DEST,
//ATTRIB_BOSS3_Y_DEST,
ATTRIB_BOSS3_TURRET1,
ATTRIB_BOSS3_TURRET2,
ATTRIB_BOSS3_TURRET3,
ATTRIB_BOSS3_TURRET4,
ATTRIB_BOSS3_TURRET5
};

enum
{
ATTRIB_BO3TURRET_MAIN,
ATTRIB_BO3TURRET_MAIN_DEAD
};

enum
{
ATTRIB_BOSS4_X_DEST,
ATTRIB_BOSS4_Y_DEST,
ATTRIB_BOSS4_TURRET1,
ATTRIB_BOSS4_TURRET2,
ATTRIB_BOSS4_TURRET3,
ATTRIB_BOSS4_TURRET4
};

enum
{
ATTRIB_BO4TURRET_MAIN,
ATTRIB_BO4TURRET_MAIN_DEAD
};

enum
{
ATTRIB_TORPER_SPRITE
};

enum
{
FIGHTER_ATTACK,
FIGHTER_MOVE,
BEE_RESTING
};

enum
{
BULLET_NONE,
BULLET_ZAP,
BULLET_POWERED,
BULLET_MISSILE,
BULLET_MISSILE_MINI,
BULLET_BOMB,
BULLET_BOMB_SIDE,
BULLET_BOMBLET, // also used for cluster missiles
BULLET_STING,
BULLET_HOSTILE,
BULLET_PREMINE,
BULLET_MINE,
BULLET_BLUE_BLOB,
BULLET_RED_BLOB,
BULLET_YELLOW_BLOB,
BULLET_ORANGE_BLOB,
BULLET_SEEKER_BLOB,
BULLET_PLASMA,
BULLET_PRESEEKMINE,
BULLET_SEEKMINE,
BULLET_SEEKER_BLOB2,
BULLET_LINES,
BULLET_CURVE,
BULLET_PRONG,
BULLET_ORBITAL,
BULLET_FLAK,
BULLET_SEEKER_BLOB3,
BULLET_BOLT,
BULLET_TORPEDO,
BULLET_SUPERSTING,
BULLET_NICE_ORBITAL,
BULLET_STING2,
BULLET_PURPLE_BLOB,
BULLET_BOSS4,
BULLET_TORPEDO2



};


enum
{
WPN_NONE,
WPN_ZAPPER
};

enum
{
SYST_NONE,
SYST_WPN_ZAPPER
};

enum
{
BRAKES_NONE,
BRAKES_RETRO,
BRAKES_DRAG
};

enum
{
SHIELD_NONE,
SHIELD_LOWFREQ,
SHIELD_MEDIUMFREQ,
SHIELD_HIGHFREQ
};

enum
{
SLIDE_NO,
SLIDE_YES
};

enum
{
CLOUD_NONE,
CLOUD_SPECK,
CLOUD_TRAIL1,
CLOUD_SMALL_CIRCLE,
CLOUD_SMALL_TRANS_CIRCLE,
CLOUD_MED_CIRCLE,
CLOUD_MED_TRANS_CIRCLE,
CLOUD_LARGE_CIRCLE,
CLOUD_LARGE_TRANS_CIRCLE,
CLOUD_FADING_LINE,
CLOUD_SHRINKING_LINE,
CLOUD_SHOCKWAVE,
CLOUD_BURSTLET,
CLOUD_GROWING_CIRCLE,
CLOUD_LINE_SHADOW,
CLOUD_SHRINKING_CIRCLE,
CLOUD_SPAWNER
};

enum
{
UPG_NONE,
UPG_SPEED,
// increases speed + turning
// special: aerodynamics (may not be a benefit, really)
UPG_ARMOUR,
// increases armour
// special: shield, then increase shield
UPG_POWER,
// increases cannon damage
// special: plasma bolt
UPG_AUTOFIRE,
// increases cannon fire rate + scatter
// special: sidekick
UPG_MULTIFIRE,
// increases: cannon shot spread
// special: backwards fire
UPG_PROJECT,
// increases cannon speed + range
// special: lightning bolt
UPG_WARHEAD,
// increases missile damage
// special: cluster warhead
UPG_SEEKER,
// makes missiles track nearest target, then increases turn rate
// special:
//  1. Dauntless (seekers don't explode when target does; acquire new one)
//  2. Smart guidance - go for most important target
//  3. Target Analysis - send only enough missiles to destroy; prefer weaker
//      targets if they'll be destroyed
UPG_ROCKET,
// increases missile acceleration, but not range by much
// special: long range
UPG_TUBES,
// increases number of missiles
// special: extra mini-missiles, ?random directions, not seeking
UPG_SLIDE,
// gives, then increases, slide rockets
// special: retro rockets (replaces drag field)
UPG_BOMB,
// increases bomb damage
// special: dispersion (explosion size)
UPG_LAUNCHER,
// adds more bombs (side-bombs are smaller)
// special: cluster bombs (main one only?)
UPG_LOADER,
// increases missile firing rate

UPG_SPECIAL,

UPG_TURRET,

UPG_SIDEKICK,

UPG_HEAVY,

UPG_ORBITAL,

UPG_BACKFIRE,

UPG_SHIELD,

UPG_RETRO,

UPG_ROAMER,

UPG_MINIMISSILES,

UPG_DRIFT,

/*
REMEMBER: When adding something which upgrades a secondary weapon,
must make sure that the secondary weapon is switched on when the upgrade
is gained.
*/

MAX_UPGRADES
};


enum
{
DRIVE_THRUST,
DRIVE_SLIDE,
DRIVE_RETRO

};

enum
{
DUEL_10_POINTS,
DUEL_30_POINTS,
DUEL_3_MINUTES,
DUEL_10_MINUTES
};

#define LOCK_ACTOR0 -3
#define LOCK_ACTOR1 -2

/*
enum
{
SPECIAL_SHIELD,
SPECIAL_
};
*/
struct optionstruct
{
 int sound_init; // if 0, sound isn't initialised at all. Changed in config file only.
 int sound_mode; // mono, stereo, reversed, off
 int run_vsync; // on or off
 int sound_volume; // sound fx volume; if 0 sound effects not played
 int ambience_volume; // if 0 ambience not played
};

struct starstruct
{
 int colour [2];
 int x [2];
 int y [2];
 char distance [2];

};

struct gamestruct
{
 int users;
 int type;
 int duel_mode;
 int duel_crawlies;
 int duel_handicap [2];
 int duel_winner;
 int single_player;
 int ships_left;
 float drag;
};

struct arenastruct
{
  int max_x;
  int max_y;
  int level;
  int colour1;
  int colour2;
  int colour3;
  int max_targets_in_level;
  int next_target;
  int total_targets;
  int targets_left_total;
  int target [3];
  int targets_left [3];
  int next_target_upgrade;

  int time_left;
  int level_finished;
  int seconds_left_when_finished;
  int time_bonus;
  int game_over;
  
  int max_crawlies_in_level;
  int next_crawly;
  int next_crawly_upgrade;
  int crawlies_created;

  int between_target_upgrades;
  int between_crawly_upgrades;

  int hostile;
};


struct playerstruct
{
 int actor_controlled;
 int user;
// int ships_left;
 char player_cmd [NO_CMDS];
 char link_fire;
 int link_toggle_delay;

 int ship;
 int score;

 int duel_score;

 int keys [NO_CMDS];
};


struct actorstruct
{
 int x, y, x_speed, y_speed, angle, base_angle;
 int moving_angle; // the angle at which it's moving. Set in drag_actor,
 // so may not be very reliable but is good enough for friction burn and
 // ai routines etc.

 int type;
 int ship;
 int controller;
 char actor_cmd [NO_CMDS];
 int drag_amount; // amount that actor is accelerated by drag
 int radius;
 int edge_radius;
 char in_play;

 int turning;
 int thrust;
 int slide;
 int mass;
 int recharge;

 int armour;
 int max_armour;

 int upgrade_slot;
 int upgrades [NO_UPGRADES]; // referred to by slot
 int upgrade_specials [NO_UPGRADES];
 int upgrade_structure [NO_UPGRADES];
 int upgraded_system [MAX_UPGRADES]; // referred to by UPG_ index
 int just_upgraded;
 int just_upgraded_timeout;
 int shield_pulse;
 int recycle1;
 int recycle2;
 int cannon_status;
 int bomb_status;
 int shield;
 int max_shield;
/* int shield_type;
 int shield_strength1;
 int shield_strength2;
 int shield_pulse;
 int shield_angle;
 int system [3];
 int system_status1 [3];
 int system_status2 [3];
 int max_energy;
 int energy;*/
 int brakes;
 int brake_strength;
 char dragging; // is 1 if drag field active
 int just_collided;
 int spawn_delay;
 int grace_period;
 int repairing;
 int secondary_weapon;

 int seek_x;
 int seek_y;
 int lock;
 int new_lock;
 
 int sidekick_x [5];
 int sidekick_y [5];
 int sidekick_x_speed [5];
 int sidekick_y_speed [5];
 int sidekick_angle [5];
 int sidekick_recycle;
 int sidekicks;

 int turret_lock;
 int turret_recycle;

 int heavy_recycle;

 int backfire_recycle;

 int exhaust_distance_x;
 int exhaust_distance_y;
// int exhaust_displacement;
 int retro_distance_x;
 int retro_distance_y;
// int retro_displacement;
 int slide_distance_x;
 int slide_distance_y;
// int slide_displacement;
 int flash_distance;

 int engine_demand;

 int orbital_angle;
 int orbital_spin;

 int hurt_pulse;

 int drive_sound [3];
 
};

struct bulletstruct
{
 int x, y, x_speed, y_speed;
 int x2, y2;
 int type;
 int owner;
 int timeout;
 unsigned char seed;
 int damage;
 int left_owner;
 int mass;
 int angle;
 int colours [4];
 int special1;
 // missiles: acceleration
 // bombs: explosion size
 // enemy seeker blobs: actor target
 // enemy prong: spin direction (applied in display.c)
 int special2;
 // missiles: number of clusters
 // bombs: number of clusters
 int special3;
 // missiles: seeker target
 int special4;
 // missiles: seeker turn rate
 int special5;
 // missiles: seeker guidance system

};


struct cloudstruct
{
 int x, y, x_speed, y_speed;
 int x2, y2;
 int type;
 unsigned char seed;
 int angle;
 int timeout;
 int tickrate;
 int delta_tickrate;
 int status;
 int colour [5];
};

struct pickupstruct
{
 int x, y, x_speed, y_speed;
 int counter;
 int type;
 int timeout;
 int radius;
};

struct enemystruct
{

 int x, y, x_speed, y_speed, angle, base_angle;
 int moving_angle;
 int type;
 int subtype;
 int attacking; // which actor is it attacking (-1 for none)
 int drag_amount; // amount that enemy is accelerated by drag
 char drag_affected; // is it affected by drag?
 int radius;
 int edge_radius;
 int recycle;
// char in_play;
 int armour;
 int hurt_pulse;
 int shield;
 int max_shield;
 int attribute [NO_ENEMY_ATTRIBUTES];
 int colours [3];
 int hurt_pulse_colour1;
 int hurt_pulse_colour2;
 unsigned char counter;
 int just_collided;
 int mass;
 int carrying_pickup;
 int burst_fire;
 int target;
};

struct enemy_classification
{
 int max_armour;
 int level;
 int common;
 int score;
 int radius; // for hit calculation
 int edge_radius; // for edge collision
 int max_shield;
 char drag_affected;
 int mass;
 int default_attributes [NO_ENEMY_ATTRIBUTES];

};



