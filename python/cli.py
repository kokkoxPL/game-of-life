# E401 [*] Multiple imports on one line
import argparse, time
from core.engine import set_game_grid, calculate_step, BACKENDS

# Brak type hintów. Typehinty w pythonie to dziś abligatoryjny standard, ułatwiają czytelność i korzystanie z IDE i ogólnie ułatwia życie wszystkim.
# Dzięki typom python staje się całkiem przyjemnym językiem do pisania każdego rodzaju projektu.

# Brak jakiejkolwiek deklaracji środowiska (pyproject/requirements/uv/conda) – to też dziś wymagane. Sprawdź jak to działa w innych projektach.

# Uwaga: cały moduł uruchamia się przy imporcie (brak main guard) co utrudnia testy/ponowne użycie.

# Sprawdź warningi generowane przez narzędzie 'ruff'
# python -m ruff check  

parser = argparse.ArgumentParser()
parser.add_argument("-p", "--print", action="store_true")
parser.add_argument("-b", "--backend", choices=BACKENDS)
parser.add_argument("-i", "--skip-inputs", action="store_true")
parser.add_argument("-s", "--size", type=int, nargs=2)
parser.add_argument("-g", "--generation", type=int)
parser.add_argument("-f", "--file")
parser.add_argument("-t", "--time", type=float)
parser.add_argument("-r", "--random", type=float)
args = parser.parse_args()

if args.size:
    rows, cols = args.size
else:
    rows = int(input("Podaj ilość rzędów: "))
    cols = int(input("Podaj ilość kolumn: "))

generation = args.generation or int(input("Podaj ilość kroków gry: "))
sleep_time = args.time
backend = args.backend or "vanilla"
p_random = args.random

if not args.skip_inputs:
    choice = input(f"Wybierz backend {', '.join(BACKENDS)}: ")
    if choice in BACKENDS:
        backend = choice

    if p_random is None:
        try:
            p_random = float(input("Podaj liczbę 0 - 1 jeśli chcesz losowo wygenerować planszę: "))
            if not (0 <= p_random <= 1):
                print("Liczba nie jest pomiędzy 0 a 1")
                p_random = None
        except ValueError:
            print("Nieprawidłowa liczba")
            p_random = None

game_grid = set_game_grid(rows, cols, backend=backend, p_random=p_random)

filename = args.file
if filename:
    with open(filename, "r") as f:
        for line in f:
            try:
                x, y = map(int, line.strip().split())
                if 0 <= x < rows and 0 <= y < cols:
                    game_grid[x][y] = 1
            except ValueError:
                continue


if filename is None and p_random is None:
    print("Podaj pozycje: początkowych żywych komórek w formacie 'x y' (pozostaw puste, aby zakończyć): ")
    while line := input().strip():
        # Pusta linia jest już odfiltrowana przez strip() powyżej; ten warunek nigdy nie wykona się.
        if not line:
            break
        x, y = map(int, line.split())  # Brak obsługi błędów - invalid input wywali program.

        if 0 <= x < rows and 0 <= y < cols:
            game_grid[x][y] = 1

# To nie jest konwencjonalny sposób nazywania funkcji w Pythonie - używamy snake_case zamiast camelCase.
def printMap():
    for row in game_grid:
        print(" ".join("X" if cell else "." for cell in row))


if args.print:
    print("Początkowa wersja: ")
    print_space = "-" * (cols * 2)
    printMap()

for step in range(generation):
    if sleep_time:
        time.sleep(sleep_time)

    game_grid = calculate_step(game_grid)

    if args.print:
        print_space = "-" * (cols * 2)
        print(f"{print_space}\nKrok {step + 1}:")
        printMap()
