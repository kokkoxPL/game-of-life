import numpy as np

n = 10
steps = 10000
game_map = np.zeros((n, n), dtype=np.int8)

game_map[4][3] = 1
game_map[4][4] = 1
game_map[4][5] = 1
game_map[5][4] = 1

while steps:
    steps -= 1
    new_game_map = np.zeros((n, n), dtype=np.int16)

    for x in range(n):
        for y in range(n):

            live_cell = game_map[x][y] == 1
            x0, y0, xn, yn = x == 0, y == 0, x == n - 1, y == n - 1
            neigh_sum = sum(
                [
                    game_map[x - 1][y - 1] if not x0 and not y0 else 0,
                    game_map[x][y - 1] if not y0 else 0,
                    game_map[x + 1][y - 1] if not y0 and not xn else 0,
                    game_map[x - 1][y] if not x0 else 0,
                    game_map[x + 1][y] if not xn else 0,
                    game_map[x - 1][y + 1] if not x0 and not yn else 0,
                    game_map[x][y + 1] if not yn else 0,
                    game_map[x + 1][y + 1] if not xn and not yn else 0,
                ]
            )

            if neigh_sum < 2 and live_cell:
                new_game_map[x][y] = 0
            elif 2 <= neigh_sum <= 3 and live_cell:
                new_game_map[x][y] = 1
            elif neigh_sum > 3 and live_cell:
                new_game_map[x][y] = 0
            elif neigh_sum == 3 and not live_cell:
                new_game_map[x][y] = 1

    game_map = new_game_map.copy()
    print(game_map)
