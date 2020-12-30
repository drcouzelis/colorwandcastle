#pragma once

bool drc_init_text(void);
void drc_free_text(void);

/**
 * Draw the text to the screen, white with a black border.
 */
void drc_draw_text(int x, int y, const char *text);

