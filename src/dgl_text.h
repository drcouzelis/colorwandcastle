#pragma once

bool dgl_init_text(void);
void dgl_free_text(void);

/**
 * Draw the text to the screen, white with a black border.
 */
void dgl_draw_text(int x, int y, const char *text);

