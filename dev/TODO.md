# Colorwand Castle

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
- [ ] Vertical levels
- [ ] Left-facing levels

## Enemies

- [x] A bat flies back and forth in place
- [x] A spider slowly moves up and down the screen on his web
- [ ] A snake walks around the border of the play area
- [ ] A fireball bounces around the room

## Powerups

- [ ] Mega star, clears any two complete rows of blocks
- [ ] Flashing star, clears any color block
- [ ] Rapid star, shoot multiple stars at once
- [ ] Speed star, shoots fast
- [ ] Slow star, shoots slow
- [ ] Fragile star, doesn't bounce

## Ideas

- Hit-points on blocks? (No.)
