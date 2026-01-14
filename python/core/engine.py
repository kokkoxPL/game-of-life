from core import backend_vanilla

# Żaden moduł nie ma adnotacji typów, co obniża czytelność i narzędzia nie pomogą w wykrywaniu błędów.

BACKENDS = ["vanilla", "numpy"]
b = backend_vanilla  # Global zmieniany w trakcie działania; brak izolacji między równoległymi grami.


def get_backend(backend):
    global b

    match backend:
        case "numpy":
            from core import backend_numpy # Importy w środku kodu. To zła praktyka. Dynamiczne importowanie uniemożliwia statyczną analizę kodu 
 
            b = backend_numpy
        case "vanilla":
            from core import backend_vanilla # Importy w środku kodu. To zła praktyka.

            b = backend_vanilla
        case _:
            raise ValueError(f"Unknown backend: {backend}")


def set_game_grid(rows, cols, backend="vanilla", p_random=None):
    get_backend(backend)

    if p_random is not None:
        game_grid = b.get_random_array(rows, cols, p_random)
    else:
        game_grid = b.get_empty_array(rows, cols)

    return game_grid


def get_game_grid_copy(game_grid):
    return b.get_array_copy(game_grid)


def calculate_step(game_grid):
    return b.calculate_array(game_grid)
