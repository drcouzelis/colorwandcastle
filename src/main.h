#pragma once

/**
 * Number of times the game will update per second.
 *
 * NOTE: This is different from the frame rate!
 * Frame rate is visual, the ticker is the game logic.
 */
#define GAME_TICKER 100

/**
 * The native resolution of the game.
 * This lets us see 16 columns and 12 rows.
 *
 * NOTE: The game window can (and probably will) be larger
 * than this, but the game logic will still treat the game
 * as this size internally.
 *
 * 320 x 240
 */
#define DISPLAY_WIDTH (16 * 20)
#define DISPLAY_HEIGHT (12 * 20)
