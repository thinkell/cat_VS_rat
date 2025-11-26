# Cat VS Rat

### A simple console Game in C

This is a simple console game. It uses the **ncurses** library.

* **Goal:** Reach the final level.
* **Characters:** You can play as Cat or a Rat
  
### How to finish a level:
* **As a Cat:** Catch and eat all the rats. Then, go to the **>** symbol
* **As a Rat:** Run away from the cats. Eat all the cheese. Then, go to the **>** symbol
  

## Requirements
* **Linux:** gcc, libncurses-dev
* **Windows:** MinGW + pdcurses

## Compilate and Run
### Linux
```terminal
gcc main.c -o main -lcurses
```
```terminal
./main
```
### Windows
```terminal
gcc main.c -o main.exe -lpdcurses
```
```terminal
./main.exe
```