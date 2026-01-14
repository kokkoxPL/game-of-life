import os
# F403 `from tkinter import *` used; unable to detect undefined names
from tkinter import *  # Dziki import utrudnia śledzenie symboli.
from tkinter import messagebox, filedialog
# F403 `from tkinter.ttk import *` used; unable to detect undefined names
from tkinter.ttk import *
from core.engine import calculate_step, set_game_grid, get_game_grid_copy, BACKENDS


class GameOfLifeTK:
    CELL_SIZE = 20

    def __init__(self):
        self.root = Tk()  # F405 `Tk` may be undefined, or defined from star imports
        self.root.title("Gra o Życie")

        self.rows_var = IntVar(value=10)  # F405 `IntVar` may be undefined, or defined from star imports
        self.cols_var = IntVar(value=10)  # F405 `IntVar` may be undefined, or defined from star imports
        self.backend_var = StringVar(value="vanilla")  # F405 `StringVar` may be undefined, or defined from star imports
        self.random_var = BooleanVar(value=False)  # F405 `BooleanVar` may be undefined, or defined from star imports

        self.game_grid = []
        self.prev_game_grid = []
        self.rows = 0
        self.cols = 0

        self.build_main_window()
        self.root.mainloop()

    def build_main_window(self):
        frame = Frame(self.root, padding=10)  # F405 `Frame` may be undefined, or defined from star imports
        frame.pack()

        Label(frame, text="Wielkość planszy (rzędy, kolumny)").grid(column=0, columnspan=2, row=0)  # F405 `Label` may be undefined, or defined from star imports
        Entry(frame, textvariable=self.rows_var).grid(column=0, row=1, pady=10)  # F405 `Entry` may be undefined, or defined from star imports
        Entry(frame, textvariable=self.cols_var).grid(column=1, row=1, pady=10)  # F405 `Entry` may be undefined, or defined from star imports

        Label(frame, text="Wybierz backend").grid(column=0, row=2)  # F405 `Label` may be undefined, or defined from star imports
        Combobox(frame, textvariable=self.backend_var, values=BACKENDS).grid(column=1, row=2)  # F405 `Combobox` may be undefined, or defined from star imports

        Label(frame, text="Losowa wartość").grid(column=0, row=3, pady=5)  # F405 `Label` may be undefined, or defined from star imports
        Checkbutton(frame, variable=self.random_var, command=self.toggle_random).grid(column=1, row=3, pady=5)  # F405 `Checkbutton` may be undefined, or defined from star imports

        Label(frame, text="Wartość losowości (0 - 1)").grid(column=0, row=4)  # F405 `Label` may be undefined, or defined from star imports
        self.random_entry = Entry(frame, state=DISABLED)  # F405 `Entry` may be undefined, or defined from star imports, F405 `DISABLED` may be undefined, or defined from star imports
        self.random_entry.grid(column=1, row=4)

        Button(frame, text="Symulacja", width=30, command=self.run_sim).grid(  # F405 `Button` may be undefined, or defined from star imports
            column=0, columnspan=2, row=5, pady=(10, 0)
        )

    def get_random_value(self):
        if not self.random_var.get():
            return None
        try:
            val = float(self.random_entry.get())
            if 0 <= val <= 1:
                return val
        except:  # E722 Do not use bare `except`
            pass # pass w except to antywzorzec
        return None

    def toggle_random(self):
        if self.random_var.get():
            self.random_entry.config(state=NORMAL)  # F405 `NORMAL` may be undefined, or defined from star imports
        else:
            self.random_entry.delete(0, END)  # F405 `END` may be undefined, or defined from star imports
            self.random_entry.config(state=DISABLED)  # F405 `DISABLED` may be undefined, or defined from star imports

    def run_sim(self):
        self.rows = self.rows_var.get()
        self.cols = self.cols_var.get()

        if self.rows < 1 or self.cols < 1:
            messagebox.showerror("Błąd", "Wielkość planszy musi być wieksza niż 0")
            return

        p_random = self.get_random_value()
        if self.random_var.get() and p_random is None:
            messagebox.showerror("Błąd", "Źle ustawiona wartość losowa")
            return

        self.game_grid = set_game_grid(self.rows, self.cols, backend=self.backend_var.get(), p_random=p_random)
        self.prev_game_grid = get_game_grid_copy(self.game_grid)

        if self.game_grid is None:
            messagebox.showerror(
                "Błąd", "Problem z uruchomieniem symulacji (najprawdopodobniej nie zaimportowano numpy)"
            )
            return

        self.build_sim_window()

    def build_sim_window(self):
        self.time_var = DoubleVar()  # F405 `DoubleVar` may be undefined, or defined from star imports
        self.gen_var = IntVar(value=1)  # F405 `IntVar` may be undefined, or defined from star imports

        self.sim_window = Toplevel(self.root)  # F405 `Toplevel` may be undefined, or defined from star imports
        self.sim_window.title("Symulacja gry o życie")
        self.sim_window.grid_rowconfigure(0, weight=1)
        self.sim_window.grid_columnconfigure(1, weight=1)

        self.config_frame = Frame(self.sim_window)  # F405 `Frame` may be undefined, or defined from star imports
        self.config_frame.grid(row=0, column=0, sticky=NS)  # F405 `NS` may be undefined, or defined from star imports

        self.grid_frame = Frame(self.sim_window)  # F405 `Frame` may be undefined, or defined from star imports
        self.grid_frame.grid(row=0, column=1, sticky=NSEW)  # F405 `NSEW` may be undefined, or defined from star imports
        self.grid_frame.grid_rowconfigure(0, weight=1)
        self.grid_frame.grid_columnconfigure(0, weight=1)

        self.canvas_frame = Frame(self.grid_frame)  # F405 `Frame` may be undefined, or defined from star imports

        self.canvas_frame.grid(row=0, column=0, sticky=NSEW)  # F405 `NSEW` may be undefined, or defined from star imports
        self.canvas_frame.grid_rowconfigure(0, weight=1)
        self.canvas_frame.grid_columnconfigure(0, weight=1)

        self.v_scroll = Scrollbar(self.canvas_frame, orient=VERTICAL)  # F405 `Scrollbar` may be undefined, or defined from star imports, F405 `VERTICAL` may be undefined, or defined from star imports
        self.h_scroll = Scrollbar(self.canvas_frame, orient=HORIZONTAL)  # F405 `Scrollbar` may be undefined, or defined from star imports, F405 `HORIZONTAL` may be undefined, or defined from star imports

        self.canvas = Canvas(  # F405 `Canvas` may be undefined, or defined from star imports
            self.canvas_frame,
            width=min(self.cols * self.CELL_SIZE, 600),
            height=min(self.rows * self.CELL_SIZE, 600),
            yscrollcommand=self.v_scroll.set,
            xscrollcommand=self.h_scroll.set,
            scrollregion=(0, 0, self.cols * self.CELL_SIZE, self.rows * self.CELL_SIZE),
        )

        self.v_scroll.config(command=self.canvas.yview)
        self.h_scroll.config(command=self.canvas.xview)

        self.v_scroll.grid(row=0, column=1, sticky=NS)  # F405 `NS` may be undefined, or defined from star imports
        self.h_scroll.grid(row=1, column=0, sticky=EW)  # F405 `EW` may be undefined, or defined from star imports
        self.canvas.grid(row=0, column=0, sticky=NSEW)  # F405 `NSEW` may be undefined, or defined from star imports

        self.grid_cells = [
            [
                self.canvas.create_rectangle(
                    j * self.CELL_SIZE,
                    i * self.CELL_SIZE,
                    (j + 1) * self.CELL_SIZE,
                    (i + 1) * self.CELL_SIZE,
                    fill="black" if self.game_grid[i][j] else "white",
                    outline="gray",
                )
                for j in range(self.cols)
            ]
            for i in range(self.rows)
        ]

        self.canvas.bind("<Button-1>", self.change_cell_state)

        Button(self.config_frame, text="Reset", command=self.reset).grid(row=0, column=0, sticky=EW, padx=5, pady=2)  # F405 `Button` may be undefined, or defined from star imports, F405 `EW` may be undefined, or defined from star imports
        Button(
            self.config_frame,
            text="Pobierz wartości z pliku",
            command=self.get_from_file,
        ).grid(row=1, column=0, sticky=EW, padx=5, pady=2)  # F405 `Button` may be undefined, or defined from star imports, F405 `EW` may be undefined, or defined from star imports

        Label(self.config_frame, text="Czas pomiędzy generacjami").grid(row=2, column=0, sticky=W, padx=5, pady=2)  # F405 `Label` may be undefined, or defined from star imports, F405 `W` may be undefined, or defined from star imports
        Entry(self.config_frame, textvariable=self.time_var).grid(row=3, column=0, sticky=EW, padx=5, pady=2)  # F405 `Entry` may be undefined, or defined from star imports, F405 `EW` may be undefined, or defined from star imports

        Label(self.config_frame, text="Ilość generacji").grid(row=4, column=0, sticky=W, padx=5, pady=2)  # F405 `Label` may be undefined, or defined from star imports, F405 `W` may be undefined, or defined from star imports
        Entry(self.config_frame, textvariable=self.gen_var).grid(row=5, column=0, sticky=EW, padx=5, pady=2)  # F405 `Entry` may be undefined, or defined from star imports, F405 `EW` may be undefined, or defined from star imports

        Button(self.config_frame, text="Start", command=self.run_iterations).grid(  # F405 `Button` may be undefined, or defined from star imports
            row=6, column=0, sticky=EW, padx=5, pady=2  # F405 `EW` may be undefined, or defined from star imports
        )
        Button(self.config_frame, text="+1", command=self.next_step).grid(row=7, column=0, sticky=EW, padx=5, pady=2)  # F405 `Button` may be undefined, or defined from star imports, F405 `EW` may be undefined, or defined from star imports

    def update_canvas(self):
        for i in range(self.rows):
            for j in range(self.cols):
                if self.game_grid[i][j] != self.prev_game_grid[i][j]:
                    color = "black" if self.game_grid[i][j] else "white"
                    self.canvas.itemconfig(self.grid_cells[i][j], fill=color)

    def next_step(self):
        self.prev_game_grid = get_game_grid_copy(self.game_grid)
        self.game_grid = calculate_step(self.game_grid)
        self.update_canvas()

    def run_iterations(self):
        self.remaining = self.gen_var.get()
        self.delay = int(self.time_var.get() * 1000)
        self.run_loop()

    def run_loop(self):
        if self.remaining <= 0:
            return

        self.next_step()
        self.remaining -= 1
        if self.delay == 0:
            self.sim_window.after_idle(self.run_loop)
        else:
            self.sim_window.after(self.delay, self.run_loop)

    def change_cell_state(self, event):
        try:
            y = self.canvas.canvasy(event.y)
            x = self.canvas.canvasx(event.x)
        except TclError:  # F405 `TclError` may be undefined, or defined from star imports
            return

        row = int(y) // self.CELL_SIZE
        col = int(x) // self.CELL_SIZE

        if 0 <= row < self.rows and 0 <= col < self.cols:
            self.game_grid[row][col] = not self.game_grid[row][col]
            color = "black" if self.game_grid[row][col] else "white"
            self.canvas.itemconfig(self.grid_cells[row][col], fill=color)

    def reset(self):
        p_random = self.get_random_value()
        self.prev_game_grid = get_game_grid_copy(self.game_grid)
        self.game_grid = set_game_grid(self.rows, self.cols, backend=self.backend_var.get(), p_random=p_random)
        self.update_canvas()

    def get_from_file(self):
        try:
            f = filedialog.askopenfile(
                initialdir=os.getcwd(),
                title="Wybierz plik z wartościami",
                filetypes=[("Text files", "*.txt")],
            )
            if not f:
                return

            for line in f:
                x, y = map(int, line.strip().split())
                if 0 <= x < self.rows and 0 <= y < self.cols:
                    self.game_grid[x][y] = 1

            self.game_grid = set_game_grid(self.rows, self.cols)  # Nadpisuje wczytaną planszę (bug).
            self.update_canvas()

        except:  # E722 Do not use bare `except`
            messagebox.showerror("Błąd", "Błąd w odczytywaniu pliku")


if __name__ == "__main__":
    GameOfLifeTK()
