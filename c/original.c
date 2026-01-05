#include <stdio.h>
#include <string.h>

int main() {
  enum { n = 10 };
  int steps = 10;
  int game_map[10][10] = {0};

  game_map[4][3] = 1;
  game_map[4][4] = 1;
  game_map[4][5] = 1;
  game_map[5][4] = 1;

  while (steps-- > 0) {
    int new_game_map[n][n] = {0};

    for (int x = 0; x < n; x++) {
      for (int y = 0; y < n; y++) {
        char live_cell = game_map[x][y] == 1;
        char x0 = x == 0;
        char y0 = y == 0;
        char xn = x == n - 1;
        char yn = y == n - 1;
        int neigh_sum = 0;

        if (!x0 && !y0)
          neigh_sum += game_map[x - 1][y - 1];
        if (!y0)
          neigh_sum += game_map[x][y - 1];
        if (!xn && !y0)
          neigh_sum += game_map[x + 1][y - 1];

        if (!x0)
          neigh_sum += game_map[x - 1][y];
        if (!xn)
          neigh_sum += game_map[x + 1][y];

        if (!x0 && !yn)
          neigh_sum += game_map[x - 1][y + 1];
        if (!yn)
          neigh_sum += game_map[x][y + 1];
        if (!xn && !yn)
          neigh_sum += game_map[x + 1][y + 1];

        if (neigh_sum < 2 && live_cell)
          new_game_map[x][y] = 0;
        else if ((neigh_sum == 2 || neigh_sum == 3) && live_cell)
          new_game_map[x][y] = 1;
        else if (neigh_sum > 3 && live_cell)
          new_game_map[x][y] = 0;
        else if (neigh_sum == 3 && !live_cell)
          new_game_map[x][y] = 1;
      }
    }
    memcpy(game_map, new_game_map, sizeof(game_map));
    for (int x = 0; x < n; x++) {
      for (int y = 0; y < n; y++) {
        printf("%d ", game_map[x][y]);
      }
      printf("\n");
    }
  }
  return 0;
}