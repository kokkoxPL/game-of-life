#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdint.h>

void read_from_file(char *fileName, uint8_t *game_grid, int rows, int cols);
uint8_t *set_game_grid(int rows, int cols);
uint8_t *set_random_game_grid(int rows, int cols, float p_alive);
void next_step(uint8_t *game_grid, uint8_t *next_game_grid, int rows, int cols,
               int simd);
void print_game_grid(const uint8_t *game_grid, int rows, int cols);

#endif