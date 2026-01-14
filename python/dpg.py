from core.engine import BACKENDS, calculate_step, get_game_grid_copy, set_game_grid
import dearpygui.dearpygui as dpg
import time

# Cała konfiguracja GUI odpala się przy imporcie; Takie rzeczy powinno się opakować w funkcję main / klasę / funkcje. 

CELL_SIZE = 10
ALIVE_COLOR = (0, 0, 0, 255)
DEAD_COLOR = (255, 255, 255, 255)
BORDER_COLOR = (150, 150, 150, 255)

game_grid = []
game_grid_rects = []
running = False

dpg.create_context()

with dpg.value_registry():
    dpg.add_int_value(default_value=20, tag="rows_value")
    dpg.add_int_value(default_value=20, tag="cols_value")
    dpg.add_int_value(default_value=10, tag="gen_value")
    dpg.add_double_value(default_value=0.4, tag="random_value")
    dpg.add_double_value(default_value=0.1, tag="time_value")
    dpg.add_bool_value(default_value=True, tag="use_random_value")
    dpg.add_string_value(default_value="vanilla", tag="backend_value")


def set_grid():
    global game_grid

    rows = dpg.get_value("rows_value")
    cols = dpg.get_value("cols_value")
    backend = dpg.get_value("backend_value")
    p_random = dpg.get_value("random_value") if dpg.get_value("use_random_value") else None
    game_grid = set_game_grid(rows, cols, backend=backend, p_random=p_random)


def on_click():
    global game_grid_rects

    dpg.configure_item("simulation", show=True)
    dpg.configure_item("settings", show=False)
    dpg.set_primary_window("simulation", True)

    set_grid()
    rows, cols = dpg.get_values(("rows_value", "cols_value"))

    dpg.delete_item("game_grid") 

    game_grid_rects = [[None for _ in range(cols)] for _ in range(rows)]
    dpg.delete_item("game_grid")

    with dpg.drawlist(
        tag="game_grid",
        parent="simulation_drawlist_container",
        callback=change_cell,
        width=cols * CELL_SIZE,
        height=rows * CELL_SIZE,
    ):
        for r in range(rows):
            for c in range(cols):
                x1, y1 = c * CELL_SIZE, r * CELL_SIZE
                x2, y2 = x1 + CELL_SIZE, y1 + CELL_SIZE

                fill = ALIVE_COLOR if game_grid[r][c] else DEAD_COLOR

                game_grid_rects[r][c] = dpg.draw_rectangle(
                    (x1, y1), (x2, y2), tag=f"grid[{r}][{c}]", fill=fill, color=BORDER_COLOR
                )


def update_canvas(prev_grid):
    rows, cols = dpg.get_values(("rows_value", "cols_value"))

    for r in range(rows):
        for c in range(cols):
            if game_grid[r][c] != prev_grid[r][c]:
                fill = ALIVE_COLOR if game_grid[r][c] else DEAD_COLOR
                dpg.configure_item(game_grid_rects[r][c], fill=fill)


def next_step():
    global game_grid

    prev_grid = get_game_grid_copy(game_grid)
    game_grid = calculate_step(game_grid)
    update_canvas(prev_grid)


def schedule_steps(steps, interval):
    start_time = time.time()
    remaining = steps

    def step_callback():
        global running
        nonlocal start_time, remaining

        if not running:
            return

        now = time.time()

        if now - start_time >= interval:
            next_step()
            remaining -= 1
            start_time = now

        if remaining > 0:
            dpg.set_frame_callback(dpg.get_frame_count() + 1, step_callback)
        else:
            running = False

    step_callback()


def run_iterations():
    global running

    if running:
        return
    running = True # Co robi running?

    gen, interval = dpg.get_values(("gen_value", "time_value"))
    if interval > 0:
        schedule_steps(gen, interval)
    else:
        for _ in range(gen):
            next_step()
            running = False # Co ten running robi?

def stop_simulation():
    global running
    running = False


def change_cell():
    if running:
        return

    x, y = dpg.get_drawing_mouse_pos()
    rows, cols = dpg.get_values(("rows_value", "cols_value"))
    row = int(y // CELL_SIZE)
    col = int(x // CELL_SIZE)

    if 0 <= row < rows and 0 <= col < cols:
        next_cell = not game_grid[row][col]
        game_grid[row][col] = 1 if next_cell else 0
        dpg.configure_item(f"grid[{row}][{col}]", fill=ALIVE_COLOR if next_cell else DEAD_COLOR)


def on_click_back():
    dpg.configure_item("simulation", show=False)
    dpg.configure_item("settings", show=True)
    dpg.set_primary_window("settings", True)


def on_click_reset():
    global game_grid, running
    running = False
    prev_game_grid = get_game_grid_copy(game_grid)
    set_grid()
    update_canvas(prev_game_grid)


def get_from_file(_, app_data):
    global game_grid
    file_path = app_data["file_path_name"]
    rows, cols, backend = dpg.get_values(("rows_value", "cols_value", "backend_value"))
    prev_game_grid = get_game_grid_copy(game_grid)
    game_grid = set_game_grid(rows, cols, backend=backend)

    with open(file_path) as file:
        for line in file:
            try:
                x, y = map(int, line.strip().split())
                if 0 <= x < rows and 0 <= y < cols:
                    game_grid[x][y] = 1
            except ValueError:
                continue

    update_canvas(prev_game_grid)


# SETTINGS WINDOW
with dpg.window(tag="settings", label="Ustawienia", autosize=False):
    dpg.add_text("Wielkosc planszy: ")

    dpg.add_input_int(label="Rzedy", source="rows_value")
    dpg.add_input_int(label="Kolumny", source="cols_value")
    dpg.add_combo(items=BACKENDS, source="backend_value", default_value="vanilla")
    dpg.add_checkbox(
        label="Uzyc wartosci losowej",
        source="use_random_value",
        callback=lambda: dpg.configure_item("random_input", enabled=dpg.get_value("use_random_value")),
    )
    dpg.add_input_double(
        tag="random_input",
        label="Losowa wartosc",
        source="random_value",
        enabled=False,
        min_value=0.0,
        max_value=1.0,
        min_clamped=True,
        max_clamped=True,
    )

    dpg.add_button(label="Symulacja", callback=on_click)

# SIMULATION WINDOW
with dpg.window(tag="simulation", label="Symulacja", autosize=False, show=False):
    with dpg.file_dialog(
        directory_selector=False, show=False, callback=get_from_file, tag="file_dialog", width=700, height=400
    ):
        dpg.add_file_extension("", color=(220, 220, 220, 255))
        dpg.add_file_extension(".txt", color=(0, 255, 0, 255), custom_text="[txt]")

    with dpg.group(horizontal=True):
        dpg.add_button(label="Powrot", callback=on_click_back)
        dpg.add_button(label="Reset", callback=on_click_reset)
        dpg.add_button(label="+1", callback=next_step)
        dpg.add_button(arrow=True, direction=dpg.mvDir_Right, callback=run_iterations)
        dpg.add_button(label="stop", callback=stop_simulation)
        dpg.add_button(label="Pobierz z pliku", callback=lambda: dpg.show_item("file_dialog"))

    with dpg.group(horizontal=True):
        dpg.add_input_int(label="Obliczane generacje", source="gen_value", min_value=1, min_clamped=True)

    with dpg.group(horizontal=True):
        dpg.add_input_double(label="Czas pomiedzy generacjami", source="time_value", min_value=0, min_clamped=True)

    dpg.add_child_window(tag="simulation_drawlist_container", horizontal_scrollbar=True, width=-1, height=-1)

dpg.set_global_font_scale(1.25)
dpg.set_primary_window("settings", True)
dpg.create_viewport(title="Game Of Life", width=1000, height=700)
dpg.setup_dearpygui()
dpg.show_viewport()
dpg.start_dearpygui()
dpg.destroy_context()
