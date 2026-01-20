#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static uint32_t state = 2463534244;

uint32_t xorshift32() {
  uint32_t x = state;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  state = x;
  return x;
}

float random_bit() { return (xorshift32() & 0xFFFFFF) / (float)0x1000000; }

void read_from_file(char *fileName, uint8_t *game_grid, int rows, int cols) {
  FILE *file = fopen(fileName, "r");
  if (!file) {
    perror("Error opening file");
    return;
  }

  int pitch = cols + 2;
  int row, col;
  while (fscanf(file, "%d %d", &row, &col) != EOF) {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
      game_grid[(row + 1) * pitch + (col + 1)] = 1;
    }
  }
  fclose(file);
}

int count_alive_cells(const uint8_t *game_grid, int r, int c, int pitch) {
  int idx = r * pitch + c;

  int count = game_grid[idx - pitch - 1] + game_grid[idx - pitch] +
              game_grid[idx - pitch + 1] + game_grid[idx - 1] +
              game_grid[idx + 1] + game_grid[idx + pitch - 1] +
              game_grid[idx + pitch] + game_grid[idx + pitch + 1];

  return count;
}

uint8_t *set_game_grid(int rows, int cols) {
  return (uint8_t *)calloc((rows + 2) * (cols + 2), sizeof(uint8_t));
}

uint8_t *set_random_game_grid(int rows, int cols, float p_alive) {
  uint8_t *game_grid = set_game_grid(rows, cols);
  state = time(NULL);

  int pitch = cols + 2;
  for (int r = 1; r <= rows; r++) {
    for (int c = 1; c <= cols; c++) {
      game_grid[r * pitch + c] = (random_bit() < p_alive);
    }
  }
  return game_grid;
}

void next_step(const uint8_t *game_grid, uint8_t *next_game_grid, int rows,
               int cols, int simd) {
  int pitch = cols + 2;
  if (simd) {
    for (int r = 1; r <= rows; r++) {
      int c = 1;
      int simd_end = cols - 31;

      for (; c <= simd_end; c += 32) {
        const uint8_t *row_up = &game_grid[(r - 1) * pitch + c];
        const uint8_t *row_mid = &game_grid[r * pitch + c];
        const uint8_t *row_down = &game_grid[(r + 1) * pitch + c];

        __m256i top_l = _mm256_loadu_si256((__m256i *)(row_up - 1));
        __m256i top_m = _mm256_loadu_si256((__m256i *)row_up);
        __m256i top_r = _mm256_loadu_si256((__m256i *)(row_up + 1));
        __m256i mid_l = _mm256_loadu_si256((__m256i *)(row_mid - 1));
        __m256i mid_m = _mm256_loadu_si256((__m256i *)row_mid);
        __m256i mid_r = _mm256_loadu_si256((__m256i *)(row_mid + 1));
        __m256i bot_l = _mm256_loadu_si256((__m256i *)(row_down - 1));
        __m256i bot_m = _mm256_loadu_si256((__m256i *)row_down);
        __m256i bot_r = _mm256_loadu_si256((__m256i *)(row_down + 1));

        __m256i count = _mm256_setzero_si256();
        count = _mm256_add_epi8(count, top_l);
        count = _mm256_add_epi8(count, top_m);
        count = _mm256_add_epi8(count, top_r);
        count = _mm256_add_epi8(count, mid_l);
        count = _mm256_add_epi8(count, mid_r);
        count = _mm256_add_epi8(count, bot_l);
        count = _mm256_add_epi8(count, bot_m);
        count = _mm256_add_epi8(count, bot_r);

        __m256i is_three = _mm256_cmpeq_epi8(count, _mm256_set1_epi8(3));
        __m256i is_two = _mm256_cmpeq_epi8(count, _mm256_set1_epi8(2));

        __m256i next_state =
            _mm256_or_si256(is_three, _mm256_and_si256(mid_m, is_two));

        next_state = _mm256_and_si256(next_state, _mm256_set1_epi8(1));
        _mm256_storeu_si256((__m256i *)&next_game_grid[r * pitch + c],
                            next_state);
      }

      for (; c <= cols; c++) {
        int idx = r * pitch + c;
        int neigh_sum = count_alive_cells(game_grid, r, c, pitch);
        uint8_t alive = game_grid[idx];
        next_game_grid[idx] =
            alive ? (neigh_sum == 2 || neigh_sum == 3) : (neigh_sum == 3);
      }
    }
  } else {
    int pitch = cols + 2;
    for (int r = 1; r <= rows; r++) {
      for (int c = 1; c <= cols; c++) {
        int idx = r * pitch + c;
        int neigh_sum = count_alive_cells(game_grid, r, c, pitch);
        uint8_t alive = game_grid[idx];
        next_game_grid[idx] =
            alive ? (neigh_sum == 2 || neigh_sum == 3) : (neigh_sum == 3);
      }
    }
  }
}

void print_game_grid(const uint8_t *game_grid, int rows, int cols) {
  size_t line_len = cols + 1;
  size_t pitch = cols + 2;
  size_t total_size = (rows + 1) * line_len + 1;
  char *buffer = (char *)malloc(total_size);

  char *ptr = buffer;

  for (int i = 0; i < cols; i++)
    ptr[i] = '=';
  ptr[cols] = '\n';
  ptr += line_len;

  for (int r = 0; r < rows; r++) {
    const uint8_t *data_row = &game_grid[(r + 1) * pitch + 1];

    for (int c = 0; c < cols; c++) {
      ptr[c] = data_row[c] ? 'X' : '.';
    }
    ptr[cols] = '\n';
    ptr += line_len;
  }
  fwrite(buffer, 1, total_size - 1, stdout);
  free(buffer);
}