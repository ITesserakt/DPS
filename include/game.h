#pragma once

#include <stdbool.h>

#define Size 10

typedef enum {
    Up, Down, Right, Left, DSize
} Direction;

typedef enum {
    Empty = 0, Alive = 1, Dead = 2
} CellState;

typedef enum {
    Miss = 0, Partial = 1, Full = 2
} Result;

typedef struct {
    int x, y;
} Point;

typedef CellState Map[Size][Size];

typedef struct {
    Map map;
} Game;

Game *generate_map();

void display_map(Game *game);

CellState check_at(Game *game, Point at);

Result try_kill(Game *game, Point at);

bool ships_remained(Game *game);