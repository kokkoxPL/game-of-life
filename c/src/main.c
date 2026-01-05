#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_game_grid(const uint8_t *game_grid, int rows, int cols) {
  char *line = (char *)malloc(cols + 1);
  line[cols] = '\n';
  for (int i = 0; i < cols; i++) {
    line[i] = '=';
  }
  fwrite(line, 1, cols + 1, stdout);

  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
      line[c] = game_grid[r * cols + c] ? 'O' : '.';
    }
    fwrite(line, 1, cols + 1, stdout);
  }
  free(line);
}

static inline int count_alive_cells(const uint8_t *game_grid, int r, int c,
                                    int rows, int cols) {
  int count = 0;
  int row = r * cols + c;

  if (r > 0) {
    int row_up = row - cols;
    if (c > 0)
      count += game_grid[row_up - 1];
    count += game_grid[row_up];
    if (c < cols - 1)
      count += game_grid[row_up + 1];
  }

  if (c > 0)
    count += game_grid[row - 1];
  if (c < cols - 1)
    count += game_grid[row + 1];

  if (r < rows - 1) {
    int row_dn = row + cols;
    if (c > 0)
      count += game_grid[row_dn - 1];
    count += game_grid[row_dn];
    if (c < cols - 1)
      count += game_grid[row_dn + 1];
  }

  return count;
}

void read_from_file(char *fileName, uint8_t *game_grid, int rows, int cols) {
  FILE *file = fopen(fileName, "r");
  int row, col;

  while (fscanf(file, "%d %d", &row, &col) != EOF) {
    if (-1 < row && row < rows && -1 < cols && col < cols) {
      game_grid[row * rows + col] = 1;
      printf("added %d:%d\n", row, col);
    }
  }
}

int main(int argc, char *argv[]) {
  int rows = 10, cols = 10, gen = 10;
  char fileName[100];

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
      strncpy(fileName, argv[++i], 99);
    }
  }
  uint8_t *game_grid = (uint8_t *)calloc(rows * cols, sizeof(uint8_t));
  uint8_t *next_game_grid = (uint8_t *)malloc(rows * cols * sizeof(uint8_t));

  read_from_file(fileName, game_grid, rows, cols);

  print_game_grid(game_grid, rows, cols);

  while (gen-- > 0) {
    for (int r = 0; r < rows; r++) {
      for (int c = 0; c < cols; c++) {
        int idx = r * cols + c;
        int neigh_sum = count_alive_cells(game_grid, r, c, rows, cols);
        uint8_t alive = game_grid[idx];

        if (alive)
          next_game_grid[idx] = (neigh_sum == 2 || neigh_sum == 3);
        else
          next_game_grid[idx] = (neigh_sum == 3);
      }
    }
    uint8_t *tmp = game_grid;
    game_grid = next_game_grid;
    next_game_grid = tmp;

    print_game_grid(game_grid, rows, cols);
  }

  free(game_grid);
  free(next_game_grid);

  return 0;
}
