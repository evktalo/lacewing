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
    
File: sound.c
History:
6/10/2003 - Version 1.0 finalised

This file contains:
 - sound initialisation and playing.
enums in sound.h

*/

#include "allegro.h"

#include <string.h>

#include "sound.h"

#include "config.h"
#include "globvars.h"

#include "conio.h"
#include "math.h"
//DATAFILE *soundf;

//SAMPLE *sound_list [10];

//#define SOUND_OFF

#define BEAT 64
#define HALF_BEAT 32
#define QUARTER_BEAT 16
#define Q3_BEAT 48
#define SMALLEST_BEAT 16

enum
{
AMBI_NONE,
AMBI_BDRUM,
AMBI_SNARY,
AMBI_SOFTDRUM,
AMBI_SNARY2,
AMBI_DRONG,
AMBI_NOISY,
AMBI_HOOT,
AMBI_PULSE,
AMBI_PULSE2,
AMBI_WONK,
AMBI_SQUEAK,
AMBI_VOXBACK,
AMBI_SNARY3,
AMBI_DRUM2,
NO_AMBIS

};

#define NO_BEATS 4

extern struct optionstruct options;


int beat [NO_BEATS];
int beat_offset [NO_BEATS];
int beat_freq [NO_BEATS];
int epres [NO_ENEMY_TYPES];
int epres_subtype [NO_ENEMY_TYPES];


SAMPLE *ambi [NO_AMBIS];



SAMPLE *sounds [NO_WAVS];

char sound_active;

void check_sound(SAMPLE *samp);
void load_sample_in(int samp, const char *sfile);
void play_ambi(int samp);
void play_ambif(int samp, int freq);
void play_ambifv(int samp, int freq, int vol);
void init_ambience(void);
void load_ambi_sample_in(int samp, const char *sfile);
void calculate_ambience(void);
void assign_beat(int index, int beaty, int subtype);

void init_sound(void)
{
#ifdef SOUND_OFF
 return;
#endif

   
   sound_active = 1;

   if (options.sound_init == 0)
   {
//    cprintf("\n\r\n\rSound disabled in proj.cfg.");
//    allegro_message("\n\r\n\rSound disabled in config file.\n\r");
//    allegro_message("\n\r\n\rSound disabled in config file.\n\r");
//    rest(500);
    sound_active = 0;
    return;
   }

   reserve_voices(10, 0);
   if (install_sound (DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) == -1)
   {
//    allegro_message("\n\r\n\rSound autodetect failed.");
    sound_active = 0;
    rest(300);
    do
    {
    } while (keypressed() == 0);
   }
   set_volume(255, 0);


   load_sample_in(WAV_CANNON, "zap");
   load_sample_in(WAV_ENGINE, "engine");
   load_sample_in(WAV_WOBBLE, "wobble");
   load_sample_in(WAV_LONG_WOBBLE, "longwob");
   load_sample_in(WAV_WARP_IN, "warp_in");
   load_sample_in(WAV_BANG1, "bangs");
   load_sample_in(WAV_WHINE, "whine");
   load_sample_in(WAV_BUMP, "bump");
   load_sample_in(WAV_BOSS2, "boss2");//_2");
   load_sample_in(WAV_STING, "sting");
   load_sample_in(WAV_HARD_ZAP, "hardzap");
   load_sample_in(WAV_BLORT, "blort");
   load_sample_in(WAV_STING4, "sting4");
   load_sample_in(WAV_BOMBS, "bombs");

   load_sample_in(WAV_BLAST, "blast");
   load_sample_in(WAV_ALARM, "alarm");
   load_sample_in(WAV_MISSILE, "missile");
   load_sample_in(WAV_ZAPNOTE1, "zapnote1");
   load_sample_in(WAV_ZAPNOTE2, "zapnote2");
   load_sample_in(WAV_MINE, "mine");
   load_sample_in(WAV_SHORTZAP, "shortzap");
   load_sample_in(WAV_BLAT, "blat");
   load_sample_in(WAV_SHORTZAP2, "shortz2");
   load_sample_in(WAV_SUN, "sun");
   load_sample_in(WAV_THRUM, "thrum");
   load_sample_in(WAV_CROAK, "croak");
   load_sample_in(WAV_MINEBANG, "minebang");
   load_sample_in(WAV_GREENSEEKER1, "cools2");
   load_sample_in(WAV_GREENSEEKER2, "song2");
   load_sample_in(WAV_TORPEDO, "coolsnd");
   load_sample_in(WAV_BOSS3_1, "boss3_1");
   load_sample_in(WAV_BOSS3_2, "boss3_2");
   load_sample_in(WAV_BOSS3_3, "boss3_3");
   load_sample_in(WAV_ASPAWN, "aspawn");
   load_sample_in(WAV_GAME_OVER, "gover2");
   load_sample_in(WAV_TUBE, "tube");
   load_sample_in(WAV_UPGRADE, "upgr");
   load_sample_in(WAV_SHIELD, "shield");
   load_sample_in(WAV_REPAIR, "repair");
   load_sample_in(WAV_SEEKMINE, "seekmine");
   load_sample_in(WAV_PLASMA, "plasma");

   load_sample_in(WAV_LEVEL_END, "finish");
   load_sample_in(WAV_MENU1, "menu1");
   load_sample_in(WAV_MENU2, "menu2");
   load_sample_in(WAV_EXTRA_SHIP, "extra");
   load_sample_in(WAV_ORBITAL, "orbital");
   load_sample_in(WAV_PICKUP_UPGRADE, "pickup");
   load_sample_in(WAV_SEEKBLOB_BANG, "seekb");
   load_sample_in(WAV_BUMP2, "bump2");
   load_sample_in(WAV_HIT, "hit");
   load_sample_in(WAV_HOSTILE, "buzzzap");
   load_sample_in(WAV_MUTATE, "mutate");
//   load_sample_in(WAV_, "");

   init_ambience();

/*   sounds [WAV_CANNON] = load_sample(".\\wavs\\zap.wav");
   check_sound(sounds [WAV_CANNON]);

   sounds [WAV_ENGINE] = load_sample(".\\wavs\\engine.wav");
   check_sound(sounds [WAV_ENGINE]);

   sounds [WAV_WOBBLE] = load_sample(".\\wavs\\wobble.wav");
   check_sound(sounds [WAV_WOBBLE]);

   sounds [WAV_LONG_WOBBLE] = load_sample(".\\wavs\\longwob.wav");
   check_sound(sounds [WAV_LONG_WOBBLE]);

   sounds [WAV_WARP_IN] = load_sample(".\\wavs\\warp_in.wav");
   check_sound(sounds [WAV_WARP_IN]);

   sounds [WAV_BANG1] = load_sample(".\\wavs\\bangs.wav");
   check_sound(sounds [WAV_BANG1]);

   sounds [WAV_WHINE] = load_sample(".\\wavs\\whine.wav");
   check_sound(sounds [WAV_WHINE]);

   sounds [WAV_BUMP] = load_sample(".\\wavs\\bump.wav");
   check_sound(sounds [WAV_BUMP]);

   sounds [WAV_BOSS2] = load_sample(".\\wavs\\boss2.wav");
   check_sound(sounds [WAV_BOSS2]);

   sounds [WAV_STING] = load_sample(".\\wavs\\sting.wav");
   check_sound(sounds [WAV_STING]);

   sounds [WAV_HARD_ZAP] = load_sample(".\\wavs\\hard_zap.wav");
   check_sound(sounds [WAV_HARD_ZAP]);
*/
//   sounds [WAV_BANG2] = load_sample(".\\wavs\\test\\bg_2_1.wav");
//   sounds [WAV_BANG2] = load_sample(".\\wavs\\test\\bang4.wav");
//   sounds [WAV_BANG2] = load_sample("c:\\djgpp\\project\\sound\\grenade1.wav");
//   sounds [WAV_BANG2] = load_sample(".\\wavs\\test\\ph_1_2.wav");

/*
buzz_1_1 - electronic grinding sound
buzz_1_2 - very short bang
buzz_1_3 - longer bang
buzz_1_4 - harsh, snare-drum like bang
buzz_1_5 - like _1_4

*/

/*   soundf = load_datafile("sound.dat");

   if (soundf == NULL)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load file sound.dat");
      exit(1);
   }
*/

//   priority->sound_list[WAV_POP] = 1;

}


void load_sample_in(int samp, const char *sfile)
{

 char sfile_name [50];

 strcpy(sfile_name, ".\\sound\\");
 strcat(sfile_name, sfile);
 strcat(sfile_name, ".wav");

 sounds [samp] = load_sample(sfile_name);

 if (sounds [samp] == NULL)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load sound file: %s", sfile_name);
      exit(1);
 }
}
/*
void load_sample_in(SAMPLE *samp)
{
 if (samp == NULL)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load sound file");
      exit(1);
 }
}
*/
void play_sound(int sample)
{
#ifdef SOUND_OFF
 return;
#endif

 if (sound_active == 0 || options.sound_volume == 0) return;

/* stop_sample(soundf[WAV_POP].dat);
 stop_sample(soundf[WAV_RICOCHET].dat);
 if (sample == WAV_S_PULSE)
  stop_sample(soundf[WAV_S_PULSE].dat);
 play_sample(soundf[sample].dat, 250, 127, 1000, 0);
*/
// stop_sample(sounds [sample]);
 play_sample(sounds [sample], (int) (250 * options.sound_volume) / 100, 127, 1000, 0);

}

void play_sound2(int sample, int frq, int vol, int pan)
{
#ifdef SOUND_OFF
 return;
#endif

 if (sound_active == 0 || options.sound_volume == 0) return;
/*
 stop_sample(soundf[WAV_POP].dat);
 stop_sample(soundf[WAV_RICOCHET].dat);
 if (sample == WAV_S_PULSE)
  stop_sample(soundf[WAV_S_PULSE].dat);*/
// stop_sample(sounds [sample]);
 play_sample(sounds [sample], (vol * options.sound_volume) / 100, pan, frq, 0);


}

void play_soundf(int sample, int frq)
{

#ifdef SOUND_OFF
 return;
#endif

 if (sound_active == 0 || options.sound_volume == 0) return;
 play_sample(sounds [sample], (int) (255 * options.sound_volume) / 100, 127, frq, 0);


}

/*
Positional sound for stereo effects.
y2 currently unused, but may be in future for some kind of
triangulation.
*/
void play_sound_pos(int sample, int frq, int vol, int x2, int y2)
{
#ifdef SOUND_OFF
 return;
#endif

 if (sound_active == 0 || options.sound_volume == 0) return;

/* stop_sample(soundf[WAV_POP].dat);
 stop_sample(soundf[WAV_RICOCHET].dat);
 if (sample == WAV_S_PULSE)
  stop_sample(soundf[WAV_S_PULSE].dat);*/

/* if (options[0].positional_sound == 0 || game.users > 1)
 {
  play_sample(soundf[sample].dat, vol, 127, frq, 0);
 }*/


 int pan = 127;

 int vol2 = vol;
 int distance;

 int x1 = actor[player[game.single_player].actor_controlled].x;
 int y1 = actor[player[game.single_player].actor_controlled].y;

 distance = hypot(abs(x2 - x1), abs(y2 - y1)) / GRAIN;
 if (distance > 1000)
  return;
 if (distance > 300)
 {
  distance -= 300;
  distance = 1000 - distance;
  vol2 *= distance; //(800 - (distance - 300));
  vol2 /= 1000;
 }



 if (x1 == x2)
 {
  pan = 127;
 }
  else
  {
   if (x1 < x2 - (300 * GRAIN))
    pan = 0;
     else
      {
       if (x1 > x2 + (300 * GRAIN))
        pan = 255;
         else
         {
          if (x1 > x2)
          {
           pan = 127 + ((x1 - x2) * 125) / (300 * GRAIN);
          }
           else
           {
            pan = 127 - ((x2 - x1) * 125) / (300 * GRAIN);
           }
         }
      }
 }

 if (options.sound_mode == SOUNDMODE_REVERSED)
  pan = 255 - pan;
 if (options.sound_mode == SOUNDMODE_MONO || game.users == 2)
  pan = 127;


/* if (options[0].positional_sound == 2)
  pan = 255 - pan; // speakers reversed
   else
    pan = pan; // speakers reversed
*/
/*  char kill_msg [50];
  char prgk [10];
  
     strcpy(kill_msg, "Pan: ");
     strcat(kill_msg, itoa(pan, prgk, 10));
     message(-1, kill_msg, COLOUR_GREY8);
*/
// stop_sample(sounds [sample]);

 play_sample(sounds [sample], (vol2 * options.sound_volume) / 100, pan, frq, 0);

}


void set_engine_sound(int a, int drive, int setting)
{

#ifdef SOUND_OFF
 return;
#endif

}

void kill_drive_sounds(void)
{
#ifdef SOUND_OFF
 return;
#endif

 if (sound_active == 0)
  return;

// if (actor[a].drive_sound [DRIVE_THRUST] > 0)
 {
  stop_sample(sounds [WAV_ENGINE]);
 }
}










/*
************************

Ambience functions

************************
*/



void play_ambience(void)
{

 if (sound_active == 0 || options.ambience_volume == 0)
  return;
 if (options.ambience_volume == 0)
  return;
  
 int tempo = 2;

// if (arena.time_left < 33.333 * 60)
//  tempo = 2;
// if (arena.time_left < 33.333 * 10)
//  tempo = 4;

// if (arena.level_finished > 0)
//  tempo = 1;

 if (counter % (SMALLEST_BEAT / tempo) != 0)
  return;

// calculate_ambience();

 int i;
 
 for (i = 0; i < NO_BEATS; i ++)
 {
  if (beat [i] != AMBI_NONE)
  {
   if (counter % (BEAT / tempo) == beat_offset [i] / tempo)
//    play_ambi(beat [i]);
    play_ambif(beat [i], beat_freq [i]);
  }
 }
/*
 if (counter % (32 / tempo) == 0)
  play_ambi(AMBI_BDRUM);

 if (counter % (32 / tempo) == (32 / tempo) / 2)
  play_ambi(AMBI_SNARY);

 if (counter % (128 / tempo) == (128 / tempo) / 2)
  play_ambi(AMBI_SOFTDRUM);
*/


}




void calculate_ambience(void)
{

 if (sound_active == 0)
  return;
 if (options.ambience_volume == 0)
  return;

 int i;

 for (i = 0; i < NO_ENEMY_TYPES; i ++)
 {
   epres [i] = 0;
   epres_subtype [i] = SUBTYPE_NONE;
 }


 for (i = 0; i < NO_ENEMIES; i ++)
 {
  if (enemy[i].type != ENEMY_NONE)
  {
   epres [enemy[i].type] = 1;
   if (enemy[i].subtype > epres_subtype [enemy[i].type])
    epres_subtype [enemy[i].type] = enemy[i].subtype;
  }
 }

 beat [0] = AMBI_NONE;
 beat [1] = AMBI_NONE;
 beat [2] = AMBI_NONE;
 beat [3] = AMBI_NONE;

 beat_offset [0] = 0;
 beat_offset [1] = HALF_BEAT;
 beat_offset [2] = QUARTER_BEAT;
 beat_offset [3] = Q3_BEAT;

 if (epres [ENEMY_CRAWLER] > 0)
  assign_beat(0, AMBI_DRUM2, epres_subtype [ENEMY_CRAWLER]);
 if (epres [ENEMY_CRAWLER2] > 0)
  assign_beat(0, AMBI_SNARY3, epres_subtype [ENEMY_CRAWLER2]);
 if (epres [ENEMY_CRAWLER3] > 0) // 2 & 3 reversed
  assign_beat(0, AMBI_SNARY2, epres_subtype [ENEMY_CRAWLER3]);

 if (epres [ENEMY_FALLER] > 0)
  assign_beat(1, AMBI_SOFTDRUM, epres_subtype [ENEMY_FALLER]);

 if (epres [ENEMY_SWERVER] > 0)
  assign_beat(2, AMBI_DRONG, epres_subtype [ENEMY_SWERVER]);
 if (epres [ENEMY_SWERVER2] > 0)
  assign_beat(2, AMBI_PULSE2, epres_subtype [ENEMY_SWERVER2]);
 if (epres [ENEMY_SWERVER3] > 0)
  assign_beat(2, AMBI_PULSE, epres_subtype [ENEMY_SWERVER3]);

 if (epres [ENEMY_BOUNCER] > 0)
  assign_beat(3, AMBI_NOISY, epres_subtype [ENEMY_BOUNCER]);
 if (epres [ENEMY_BOUNCER2] > 0)
  assign_beat(3, AMBI_SQUEAK, epres_subtype [ENEMY_BOUNCER2]);
 if (epres [ENEMY_BOUNCER3] > 0)
  assign_beat(3, AMBI_HOOT, epres_subtype [ENEMY_BOUNCER3]);
 if (epres [ENEMY_BOUNCER4] > 0)
  assign_beat(3, AMBI_WONK, epres_subtype [ENEMY_BOUNCER4]);
 if (epres [ENEMY_BOUNCER5] > 0)
  assign_beat(3, AMBI_VOXBACK, epres_subtype [ENEMY_BOUNCER5]);

}

void assign_beat(int index, int beaty, int subtype)
{
 beat [index] = beaty;
 beat_freq [index] = 800 + subtype * 100;

 if (arena.time_left < 33.333 * 10)
  beat_freq [index] += 400;
}


void play_ambi(int samp)
{

 play_sample(ambi [samp], (255 * options.sound_volume) / 100, 127, 1000, 0);
}

void play_ambif(int samp, int freq)
{

 play_sample(ambi [samp], (255 * options.sound_volume) / 100, 127, freq, 0);
}

void play_ambifv(int samp, int freq, int vol)
{

 play_sample(ambi [samp], (vol * options.sound_volume) / 100, 127, freq, 0);
}









void init_ambience(void)
{

 if (sound_active == 0)
  return;

 load_ambi_sample_in(AMBI_BDRUM, "bdrum");
 load_ambi_sample_in(AMBI_SNARY, "snary");
 load_ambi_sample_in(AMBI_SOFTDRUM, "drummy");
 load_ambi_sample_in(AMBI_SNARY2, "snary2");
 load_ambi_sample_in(AMBI_NOISY, "noisy");
 load_ambi_sample_in(AMBI_DRONG, "drong");
 load_ambi_sample_in(AMBI_HOOT, "hoot");
 load_ambi_sample_in(AMBI_PULSE, "pulse");
 load_ambi_sample_in(AMBI_PULSE2, "pulse2");
 load_ambi_sample_in(AMBI_WONK, "wonk");
 load_ambi_sample_in(AMBI_SQUEAK, "squeak");
 load_ambi_sample_in(AMBI_VOXBACK, "voxback");
 load_ambi_sample_in(AMBI_SNARY3, "snary3");
 load_ambi_sample_in(AMBI_DRUM2, "drum2");

// load_ambi_sample_in(AMBI_, "");
 int i;

 for (i = 0; i < NO_ENEMY_TYPES; i ++)
 {
   epres [i] = 0;
 }

 beat [0] = AMBI_NONE;
 beat [1] = AMBI_NONE;
 beat [2] = AMBI_NONE;
 beat [3] = AMBI_NONE;

 beat_offset [0] = 0;
 beat_offset [1] = HALF_BEAT;
 beat_offset [2] = QUARTER_BEAT;
 beat_offset [3] = Q3_BEAT;
}




void load_ambi_sample_in(int samp, const char *sfile)
{

 char sfile_name [50];

 strcpy(sfile_name, ".\\sound\\ambi\\");
 strcat(sfile_name, sfile);
 strcat(sfile_name, ".wav");

 ambi [samp] = load_sample(sfile_name);

 if (ambi [samp] == NULL)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load sound file: %s", sfile_name);
      exit(1);
 }
}

