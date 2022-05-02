# Colorwand Castle

# Remove strcpy, strcat, strncpy, strncat
# Use malloc, free, strlen, memcpy, snprintf

## Game specs

- Resolution: 320x240 (16x12 blocks)
- Block size: 20x20 blocks
- Player hit box: 20x20
- Game play area border: 1 block, all sides
- Max vertical blocks: 10
- Enemies cannot be killed.
- Stars do not affect enemies.
- Enemies do no affect stars.
- Being hit by an enemy causes the level to restart.
- The player has an unlimited number of tries to complete a level.
- The number and position of the blocks varies from room to room.

## Bugs

- [ ] Game glitches when completing the last room
- [ ] The farground fails to scroll correctly in area 2

## TODO

- [x] Player is in a room.
- [x] There are blocks of different colors / patterns on the right.
- [x] Clearing the last block reveals a door to the next level.
- [x] Player must clear all of the blocks.
- [x] Player's wand is randomly colored to match one of the block colors.
- [x] Player waves the wand to shoot a colored star.
- [x] Stars only appear lined up with rows of blocks.
- [x] If the star hits a block of the same color, the block disappears.
- [x] If the star hits a block of a differing color, it bounces back and can kill the player.
- [x] Adaptive star colors - copy the tile into the star pattern
- [x] Door to the next level appears behind the last block
- [x] Enemies poof away when the last block disappears
- [x] Endless retries, level resets after death
- [x] Create levels in code first, then transition to DAT files
- [x] Vertical levels
- [x] Left-facing levels
- [x] Fix vertical-level-star-color-choosing glitch
- [x] Allow for clearing resources at the end of a level
- [x] New exit style: EXIT 15 9 R 1
- [x] Simplify and unify the color scheme
- [x] Two types of rooms - NORMAL and BOSS
- [ ] Preload graphics / load graphics before room transition
- [ ] Gamepad support
- [ ] Background music

## Room Theme Ideas

- [x] Bedroom x 1
- [x] Hallways x many
- [ ] Outdoor hallways x many
- [ ] Dining area x 2
- [ ] Kitchen x 3
- [ ] Tower x many
- [ ] Woods x many
- [ ] Clouds / sky x many
- [ ] Underground x many

## Room Progression

- [x] Room 1 - Movement, no blocks
- [x] Room 2 - 1 color
- [x] Room 3 - 2 colors
- [x] Room 4 - Random colors, 1 simple enemy
- [x] Boss about every 30 rooms

## Enemies

- [x] A ENEMY_HORIZONTAL (bat) flies back and forth in place
- [x] A ENEMY_VERTICAL (spider) slowly moves up and down the screen on his web
- [x] A ENEMY_DIAGONAL (ghost) bounces around the room
- [x] A ENEMY_BLOCKER (spikes) doesn't move, just guards the exit until all blocks are cleared
- [ ] A ENEMY_TRACER (blob) is attached to a surface and will "trace" the surface
- [ ] A ENEMY_SNEAK (fake block) is disguised as a block with a random texture

## Powerups

- [x] Randomize each time the room is started / restarted
- [x] Flashing star, 3 shots, clears any color block (rainbow pattern)
- [x] Laser star, 1 shot, clear all stars in path to wall
- [ ] Boom star, 5 shots, clear all adjacent blocks (constant poof behind bullet)
- [ ] Time stop, Freeze all enemies for 5 shots
- [ ] Random, 3 shots, unknown color until shot (cycle colors)
- [ ] Invincible, to stars and enemies for 5 shots
- [ ] Fragile star, doesn't bounce (see star weaken)
- [ ] An item type will not appear if it won't have any effect

## Sound Effects

- [x] Create sound effects from http://www.superflashbros.net/as3sfxr/ (old), http://sfbgames.com/chiptone (new)
- [x] Block destroyed "POP"
- [x] Bullet shot "PEW"
- [x] Bullet bounce "BOING"
- [x] Bullet destroyed "TWINKLE"
- [x] Hero hit "SAD SOUND"

## Music

- [ ] Make music from http://chirp.rezoner.net/
- [ ] Create a song for the background music
- [ ] Make a MIDI file
- [ ] freesound.org / incompetech.com/music
- [ ] Boca Ceoil / BFXR

## Ideas

- Hit-points on blocks? (No.)
- Rooms that you "come back" to, such as one hallway above another, or going down stairs that you saw previously

## David Game Library
- [x] Image management
- [x] Animated sprites
- [x] Resource management (directories)
- [x] Sound management
- [ ] Music management
- [ ] Camera control
- [ ] Maps (load, draw)
