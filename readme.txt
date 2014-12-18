

LACEWING



Version 1.10
Release date: 25/11/2003

A game by Linley Henzell & Captain Pork
Copyright 2003

Produced using:
- the DJGPP compiler
- the RHIDE interface 
- the Allegro game libraries
- the GIMP (for the graphics)
- Psycle (for the sounds)

Thanks to:
Jek for playtesting and various suggestions, and anyone who's ever 
contributed to any of the software used in the production of this game.

Lacewing is distributed under the terms of the Free Software 
Foundation's General Public Licence (GPL) - see the file LICENCE.TXT. It 
comes with no warranty, and no liability is accepted for any harm it may 
do to you or your computer. In the unlikely event that it injures you, 
contact your MP or other local representative. Read the licence for more 
information.

The source code should be available from http://users.olis.net.au/zel/
If it's not there, try contacting captainpork@fastmail.fm to find out 
where it's gone.


Contents -

1. Briefing
2. Basic Piloting
3. Hints
4. Miscellaneous



**************************
1. Briefing
**************************


Welcome, pilot. 

As you would know, the situation is serious. An alien invasion force is 
slipping through the mysterious confines of Null-Z space towards Earth. 
All attempts at diplomacy have failed and we expect our outer perimeter 
defences to be under attack in a matter of hours. We are unlikely to be 
able to stand up to an assault of this magnitude without resort to 
desperate measures.

This is where you come in. Our xenotechnology experts have found a back 
door into the alien Null-Z grid which we hope can be used to sneak past 
the bulk of their forces. We can project one, or maybe two, attack craft 
into these regions of the grid, with limited reserves. It isn't much, 
but it's our best hope.

Your primary mission objective is the destruction of each of the four 
controlling intelligences of the alien invasion force. With these out of 
the picture, the whole attack will collapse. They are located deep in 
the grid, and you will need to fly through several sectors of heavy 
defences in order to reach them.

You are expected to meet two forms of resistance. The first consists of 
elements of the alien force set up to guard the intellects. These are 
critical targets, and you must take out all of them in each sector in 
order for us to be able to advance you to the next sector. Your 
Head-Up-Display (HUD) will indicate how many remain to be destroyed.

The second consists of the hostile native inhabitants of the grid. These 
may look harmless, even cute, but if you allow them to reach the bottom 
of the grid they will mutate into much more dangerous forms. Although 
these are not essential targets, you may like to zap a few of them to 
harvest the energy they sometimes carry.

You will be relieved to know that our xenotechnologists have managed to 
temporarily isolate the primary defence system in each of the sectors 
you will be required to enter. They estimate that you will have between 
two and three minutes in each sector before the local system can 
re-activate. Do try to finish up before that happens. 

The standard vehicle for a mission such as this is the Lacewing Grid 
Superiority Fighter, although if you prefer another craft we have a 
variety of interceptors, bombers, gunships, fighter-bombers, 
hunter-killers and other nasty things for you to fly. I am afraid that 
the trickiness of the Null-Z space projection required for this mission 
means that all craft will be inserted in their basic forms, but you 
should be able to find enough energy in the grid to be able to upgrade 
your vehicle as you see fit.

Good luck!



**************************
2. Basic Piloting
**************************


Lacewing can be played by one player, or by two players using the 
split-screen cooperative mode. 

Each of the craft you can choose from has its own strengths and 
weaknesses, but at first they all perform identically (and not very 
well). You can upgrade your craft's systems by harvesting energy from 
disrupted aliens. Pockets of energy look like little yellow circles, and 
when you pick them up they move your upgrade indicator (along the bottom 
of the screen) to the right.

The idea is to pick up energy until the upgrade indicator is over the 
system you want to upgrade (any overflow past the last upgrade, usually 
Shield, is wasted), then press the Upgrade command key. Each system can 
be upgraded five times.

You may from time to time come across other things to gather, like 
super-dense energy, regenerative anentropy (aka repair squares) and 
warps which allow more reserve ships to be projected. Don't leave 
anything laying around for too long or it'll dissipate and be wasted.


* Weapons and defences:

All types of craft come equipped with a basic cannon, which has 
unlimited ammunition and reloads quickly. Without upgrades, though, it 
is quite weak.

In addition, most have a secondary weapon system - either missiles 
(long-range) or bombs (short-range but very destructive). This becomes 
active as soon as you obtain an upgrade which affects it. Secondary 
weapons are usually more powerful than the cannon, but take a while to 
reload. Some craft have other weapon systems as well, like point-defence 
turrets or semi-autonomous combat drones. These also activate when 
upgraded.

All craft are protected by armour and most can be upgraded to generate an 
absorption field, aka a shield. Shields offer less protection than armour, 
but recharge.


* Commands:

The options menu allows you to redefine the keys you use to issue 
commands. It also has a key test functions so that you can work out 
which combinations of keys your keyboard has problems with (this is a 
hardware problem, and there's nothing I can do about it. Sorry). 

The commands are as follows:

- Forward
Activates your craft's main drive system, propelling it forwards. Speed 
upgrades increase the acceleration.

- Left/Right
Rotates your craft. Speed upgrades increase the rate of rotation.

- Brake
The grid exercises a slowing effect on most objects passing through it, a 
little like air resistance. All craft are equipped with drag field generators 
which, when active, greatly increase the rate at which your movement 
through the grid is slowed. Press the brake key to activate the generator. 
The Retro Rocket upgrade replaces the drag field.

- Slide Left/Right
If you have added the Slide upgrade to your craft, it can accelerate 
sideways with these commands.
Note that your craft's engine can only produce a certain amount of 
thrust, so if you have multiple sets of thrusters active at once (say 
you are sliding while moving forward) output is split between them, 
giving no increase in net acceleration (actually, a small decrease). 
Rotation is gyroscopic and does not affect acceleration in this way.

- Fire
Fires your cannon and, if link fire is on, your secondary weapon as 
well.

- Secondary fire
Fires your secondary weapon. A couple of red dots shows up in your 
targeting indicator when your secondary weapon is ready to fire.

- Toggle link fire
Sometimes you want to aim your secondary weapon carefully before firing, 
sometimes you want it to fire whenever it's ready.

- Upgrade
Upgrades whichever system is highlighted.

- Pause/quit
The escape key always pauses the game and lets you quit.


* Head-Up-Display (HUD)

Your HUD contains a large amount of useful information, if you know 
where to look.

Near your craft (which appears in the middle of the screen) is the 
targeting indicator, which tells you which way you're pointing and 
changes colour when your weapons are ready to fire. If your craft is 
carrying a weapon with lock-on capability, you will also see the maximum 
range of the weapon's sensors and a designator around your current 
target.

The top left shows a countdown until sector defences become active, the 
number of targets you must eliminate, and the current sector number.

The top right shows your score. You get extra reserves at 1,000, 10,000, 
and 20,000 points, and every 20,000 after that.

The bottom right shows your scanner. Your position is indicated by the 
white cross. The yellow dots indicate targets which must be eliminated, 
while orange dots are non-critical targets.

Along the bottom of the HUD is your upgrade display. The system you will 
upgrade if you press the Upgrade key is highlighted in yellow.

Finally, the bottom left holds the status display. This indicates the 
health of your armour, the strength of your shield (if you have one 
installed), the link status of your secondary weapon (if your craft is 
equipped with a secondary weapon) and the status of your drag field 
(unless it's been replaced by retro rockets). Your reserve of ships 
runs up the left side of the HUD.



**************************
3. Hints
**************************


If you're having trouble, here are some hints & tips to mastering
Lacewing:

- Try to keep moving as much as possible. The drag field decelerates
you much faster than your drives accelerate you, so it's often better
for dodging bullets. Of course, it only works if you're already moving.

- Activate your shield as early as possible. Try to get it by level 4.

- Having said that, once you have one level of shield you may be better
off upgrading armour.

- Link fire is only really useful for craft with seeker and/or
fast-loading missiles, or if you just don't care where your ordnance goes.

- The drift device (Doom Fork only) dramatically reduces the drag your
craft experiences provided that the drag is below a certain level. If drag
is above that level the device does nothing. So it helps your acceleration
but doesn't do so much for your top speed.

- Instead of flying directly at enemies, try flying past them and
strafing them instead. But that doesn't work for all of them.

- Instead of finishing a level early to get points, you may be better
off hanging around and farming non-critical targets for a while.

- Watch out for singing jellyfish!



**************************
4. Miscellaneous
**************************


* Cooperative

To enter cooperative mode, just start the game as normal and make sure 
that both players press fire when prompted.
In this mode you begin with four reserve craft which are pooled between 
the players, but you don't get bonus reserves at 1,000 points (you have 
to wait for 10,000).


* Duel

Since Lacewing contains a two-player cooperative mode, I thought it 
would be silly not to add a competitive mode as well.
Note that the game isn't really balanced for duelling, so some craft are 
much better at it than others.


* Options

As well as allowing you to redefine the keyset, the options menu has a 
few other functions which may not be so obvious. You can turn sound on 
or off and adjust the volume. You can set the sound mode to mono, or to 
stereo (which pans the sound according to the position of its source 
relative to your position, and only works in single player games), and you 
can test your speakers to see if the stereo needs to be reversed. You can 
also tell the game whether you want it to synchronise the video to the 
screen retrace; this removes some minor flickering but can slow the game 
down, so if your computer is dropping frames you might like to turn it off.


* Changes since version 1.0:

I don't remember all of the changes, but here's a rough list:
- 4 craft (the last 4) added
- some of the existing craft changed
- a few new enemies
- bombs do a little less damage and have a slightly smaller radius
- missiles with the rocket upgrade travel much further
- some of the late-game enemies are easier to kill
- repairs now give a flat 75 points regardless of your armour level
- various new effects and other things that I now forget added
- you now have more time on the later levels (this was a bug)
- one or two other minor bugs fixed


* System requirements:

 - Minimum
P150 
32MB RAM
That's the tested minimum, although it will probably work on a less 
powerful computer - I just haven't had the chance to test it on eg a 
computer with 16MB RAM. You'll probably want something better, 
though, so that you can run with a decent framerate when lots is 
happening on the screen.

- Recommended
P233 or better
32MB RAM
Some kind of sound card that Allegro can autodetect (probably most of them)
If the game refuses to load or just crashes, you could try disabling the 
sounds altogether to see if that fixes it.
Edit the lacew.cfg file and change 
Sound_init=1 
to 
Sound_init=0 
If that doesn't work, I have no idea what's going wrong. Sorry.


* Source Code

The main() function and a list of modules is in main.c.
As I use RHIDE I don't have a makefile, but building Lacewing is easy: 
just compile all the files and link them together. You must have Allegro 
properly installed to be able to do this, of course.


* Acknowledgements

Lacewing was fairly obviously inspired by a whole lot of old arcade and 
arcade-style games, most of which I had versions of on my Commodore 64 
(those were the days). Without thinking too hard about it, these 
include: Centipede, Slapfight, Spacewar, Asteroids, Juno First, X-out, 
Armalyte and others I now forget. In terms of more recent games, I 
downloaded Quadnet (great fun) and Warning Forever shortly before I 
started working on Lacewing.


* The End

Thanks for downloading Lacewing. If you like it (or if you don't like it 
and can explain why in a constructive manner), please send me an email 
at captainpork@fastmail.fm. Have fun!
