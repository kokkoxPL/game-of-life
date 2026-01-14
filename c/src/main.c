#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// brakuje kodu gui!

void print_game_grid(const uint8_t *game_grid, int rows, int cols) {
  char *line = (char *)malloc(cols + 1);
  // Brak sprawdzenia malloc – przy dużych planszach możemy polecieć na NULL
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

  // Ręczne liczenie sąsiadów bez modulo – działa, ale łatwo o literówkę.
  // Ogólnie te warunki są dość nieczytelne. Masz wiele zagniezdzonych if-ów i indeksujesz ręcznie.
  // Do tego tutaj jest wiele magicznych zmiennych. Co dokładnie znaczy r i c? czym się różni od r różni od row (przeciez ma badzo podobną nazwę: _r_ow i row)
  // Patrząc na ten kod nie wiem czy on działa poprawnie bez zagłębiania się w dokładny sposób liczenia indeksów.
  // wersja 'original.c' jest pod tym względem bardziej czytelna. 
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
  // Uwaga: brak sprawdzenia fopen, a fileName może być śmieciem jeśli nie podano -f.
  int row, col;

  while (fscanf(file, "%d %d", &row, &col) != EOF) {
    // Literówka w warunku: sprawdzamy "cols" zamiast "col" i można wpisać kolumnę -5.
    // Ogólnie ten warunek jest ekstremapnie skomplikowany. Lepsza czytelność byłaby z użyciem dwóch osobnych if-ów i sortując te wyrażenia w rozsądny sposób. np niepisanym warunkiem jest że stała znajduje się po prawej stronie a zmienna po lewej. 
    if (-1 < row && row < rows && -1 < cols && col < cols) {
      // Tu jest coś nie tak z indeksowaniem - aby to miało sens, powinno być row*number_of_columns + col a nie row*rows + col
      game_grid[row * rows + col] = 1;
      printf("added %d:%d\n", row, col);
    }
  }
}

int main(int argc, char *argv[]) {
  int rows = 10, cols = 10, gen = 10;
  char fileName[100];

  for (int i = 1; i < argc; i++) {
    // argv[++i] w warunkach psuje czytelność i sprzyja off-by-one
    // Słynny przykład:
    // 
    // ++a + a++
    // 
    // Jaka jest wartość wyrażenia?
    // 
    // Prawda jest taka że a) nie chce wiedzieć bo tak się nie pisze. b) zależy od kompilatora!
    // Co prawda to jest hiperbola, ale zagnieżdżanie wyrażeń w warunkach zwiększa ryzyko błędów i utrudnia czytanie kodu.
    // W nowoczesnym c lepiej unikać takich konstrukcji.

    // do tego do samego arg parsowania mogła by się przydać jakas abstrakcja/funkcja - po co śmiecić w mainie? struct args {...} parse_args(int argc, char *argv[], struct args *out_args);
    // wtedy możesz zrobić duży handling i nie oszdzędzać linijek kodu. 
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
        // Ten kod jest semantycznie poprawny, ale też ma problem z czytelnością
        // Aktualnie używasz tutaj dwóch abstrakcji do indeksowania planszy:
        // - ręczne liczenie indeksu: r * cols + c
        // - przekazywanie r i c
        // to też utrudnia czytanie kodu bo trzeba się zastanowić która abstrakcja jest używana w danym miejscu
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
