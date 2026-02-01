#include "game_logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int rows = 10, cols = 10, gen = 10;
  int use_print = 0, use_simd = 0;
  double random = 0;
  char fileName[100] = {0};
  uint8_t *game_grid = NULL;
  uint8_t *next_game_grid = NULL;

  for (int i = 1; i < argc; i++) {
    if ((strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--size") == 0) &&
        i + 2 < argc) {
      rows = atoi(argv[++i]);
      cols = atoi(argv[++i]);
    }
    if ((strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--generation") == 0) &&
        (i + 1 < argc)) {
      gen = atoi(argv[++i]);
    }
    if ((strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) &&
        (i + 1 < argc)) {
      snprintf(fileName, sizeof(fileName), "%s", argv[++i]);
    }
    if ((strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--print") == 0)) {
      use_print = 1;
    }
    if ((strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--random") == 0) &&
        (i + 1 < argc)) {
      double tmp = atof(argv[++i]);
      if (tmp >= 0 && tmp <= 1) {
        random = tmp;
      } else {
      }
    }
    if (strcmp(argv[i], "--simd") == 0) {
      use_simd = 1;
    }
  }

  if (fileName[0] != '\0') {
    game_grid = set_game_grid(rows, cols);
    read_from_file(fileName, game_grid, rows, cols);
  } else {
    game_grid = set_random_game_grid(rows, cols, random);
  }

  next_game_grid = set_game_grid(rows, cols);

  if (use_print)
    print_game_grid(game_grid, rows, cols);

  while (gen-- > 0) {
    next_step(game_grid, next_game_grid, rows, cols, use_simd);

    uint8_t *tmp = game_grid;
    game_grid = next_game_grid;
    next_game_grid = tmp;

    if (use_print)
      print_game_grid(game_grid, rows, cols);
  }

  free(game_grid);
  free(next_game_grid);

  return 0;
}
