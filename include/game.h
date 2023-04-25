#pragma once

#include <stdbool.h>

#define Size 10

/// Direction where ship is growing when map generated
typedef enum { Up, Down, Right, Left, DSize } Direction;

/// State of each cell on the game map
typedef enum { Empty = 0, Alive = 1, Dead = 2 } CellState;

/// Result of sending missle to the particular cell of the game map
typedef enum { Miss = 0, Partial = 1, Full = 2, Loose = 3 } Result;

/// Contains coordinates to 2D point of the map
typedef struct {
    int x, y;
} Point;

typedef CellState Map[Size][Size];

/// Contains game map
typedef struct {
    Map map;
} Game;

/// Fulfills game map with 6 ships
Game *generate_map();

/// Outputs current state of the game map to stdout
void display_map(Game *game);

/// Returns current cell state at specific point on the game map
CellState check_at(Game *game, Point at);

/// Checks if cell is alive at specific coords, makes it dead and returns a
/// result depending on its neighbours
Result try_kill(Game *game, Point at);