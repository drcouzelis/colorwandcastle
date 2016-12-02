#ifndef MAIN_HEADER
#define MAIN_HEADER

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
 * NOTE: Levels can be bigger than this. This is just how
 * much can be seen at a time.
 *
 * NOTE: The game window can (and probably will) be larger
 * than this, but the game logic will still treat the game
 * as this size internally.
 */
#define DISPLAY_WIDTH (16 * 20)
#define DISPLAY_HEIGHT (12 * 20)

#endif
