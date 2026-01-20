# Game Of Life

Implementacja gry o życie napisana w językach Python i C.
Projekt powstał w celach edukacyjnych i umożliwia eksperymentowanie zarówno w wersji konsolowej, jak i z graficznym interfejsem użytkownika.

## Python

_Wymagana wersja Python 3.8+_

**Implementacja w Pythonie zawiera wersje:**

- cli - wersja konsolowa, pozwala na uruchamianie symulacji z poziomu terminala
- tk - wersja GUI napisana w tkinter
- dpg - wersja GUI napisana w dearpygui (wymaga instalacji _dearpygui_)
- original - moja pierwsza implementacja gry o życie

**Opcjonalne biblioteki**

- [numpy](https://github.com/numpy/numpy) - przyspiesza obliczenia i operacje na planszy
- [dearpygui](https://github.com/hoffstadt/DearPyGui) - potrzebna do wersji _dpg_

### cli

**lista możliwych argumentów**
|Argument|Skrót|Wartość|Opis|
|---|---|---|---|
|--size|-s|int int|wielkość planszy. Wymaga podania dwóch liczb: liczba wierszy i kolumn|
|--generation|-g|int|liczba generacji do obliczenia|
|--print|-p|brak|wyświetla planszę w konsoli (X – żywa komórka, . – martwa komórka)|
|--numpy|-n|brak|używa _numpy_ do obliczeń i przechowywania planszy|
|--time|-t|float|ustawia czas oczekiwania (w sekundach) między generacjami|
|--random|-r|float|podanie liczby (0 - 1) losowo wygenerowanych pozycji gdzie liczba oznacza procent żywych komórek|
|--benchmark|-b|brak|ustawienie tej flagi spowoduje nie zapytanie się o więcej wartości podczas uruchomienia programu|
|--file|-f|string|ścieżka do pliku z początkowym stanem żywych komórek|

> Plik powinien zawierać po jednej komórce w każdej linii: x y (wiersz, kolumna, liczba zaczyna się od zera).

przykład ustawia komórkę w rzędzie 1, kolumnie 0 oraz komórkę w rzędzie 2, kolumnie 1

```
1 0
2 1
```

| .   | .   | .   |
| --- | --- | --- |
| X   | .   | .   |
| .   | X   | .   |

przykładowe uruchomienie wersji konsolowej

```python
python cli.py --size 8 12 --generation 10 --random 0.34 --file test_val.txt --time 0.5 --print --numpy
```

### GUI

Obie wersje graficzne oferują funkcjonalność podobną do wersji konsolowej, a dodatkowo umożliwiają ręczne ustawianie stanu komórek poprzez kliknięcie myszą. W wersji dearpygui dostępna jest również możliwość wstrzymania symulacji.

uruchomienie:

```python
python tk.py
python dpg.py
```

## C

Projekt jest kompilowany przy pomocy cmake.

**Pierwsze stworzenie folderu build**

```bash
cmake -G Ninja -B build
```

**Kompilacja programu**

```bash
cmake --build build
```

## Benchmark

### W folderze znajduje się plik napisany w Go do testowania szybkości uruchamiania plików
