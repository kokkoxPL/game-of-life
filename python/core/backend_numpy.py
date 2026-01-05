import numpy as np


def get_empty_array(rows, cols):
    return np.zeros((rows, cols), dtype=np.int8)


def get_random_array(rows, cols, p_random):
    return (np.random.random((rows, cols)) < p_random).astype(np.int8)


def get_array_copy(game_grid):
    return game_grid.copy()


def calculate_array(game_grid):
    padded_grid = np.pad(game_grid, pad_width=1, mode="constant", constant_values=0)

    neigh_sum = (
        padded_grid[:-2, :-2]
        + padded_grid[:-2, 1:-1]
        + padded_grid[:-2, 2:]
        + padded_grid[1:-1, :-2]
        + padded_grid[1:-1, 2:]
        + padded_grid[2:, :-2]
        + padded_grid[2:, 1:-1]
        + padded_grid[2:, 2:]
    )
    return ((neigh_sum == 3) | ((game_grid == 1) & (neigh_sum == 2))).astype(np.int8)
