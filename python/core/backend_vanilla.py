import random as rnd


def get_empty_array(rows, cols):
    return [[0] * cols for _ in range(rows)]


def get_random_array(rows, cols, p_random):
    return [[1 if rnd.random() < p_random else 0 for _ in range(cols)] for _ in range(rows)]


def get_array_copy(game_grid):
    return [row[:] for row in game_grid]


def calculate_array(game_grid):
    rows = len(game_grid)
    cols = len(game_grid[0])

    new_game_grid = [[0] * cols for _ in range(rows)]

    for i in range(rows):
        for j in range(cols):
            total = 0
            for row_offset in (-1, 0, 1):
                for col_offset in (-1, 0, 1):
                    if row_offset == 0 and col_offset == 0:
                        continue
                    neighbor_row = i + row_offset
                    neighbor_col = j + col_offset

                    if 0 <= neighbor_row < rows and 0 <= neighbor_col < cols:
                        total += game_grid[neighbor_row][neighbor_col]

            alive = game_grid[i][j]
            if (alive and total in (2, 3)) or (not alive and total == 3):
                new_game_grid[i][j] = 1

    return new_game_grid
