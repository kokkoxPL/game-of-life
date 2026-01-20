#ifndef GUI_H
#define GUI_H

#include <stdint.h>

int gui_init(int width, int height);

int gui_update(uint8_t **game_grid, uint8_t **next_game_grid);

void gui_cleanup(void);

#endif