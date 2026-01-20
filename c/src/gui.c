#include "gui_logic.h"
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {

#ifdef _WIN32
  FreeConsole();
#endif

  uint8_t *game_grid = NULL;
  uint8_t *next_game_grid = NULL;

  if (!gui_init(1000, 800))
    return 1;

  double last_update_time = 0.0;

  while (gui_update(&game_grid, &next_game_grid)) {
  }
  gui_cleanup();

  return 0;
}