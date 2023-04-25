#include <stdio.h>
#include <stdlib.h>

#include "game.h"

/// Tries to make cells with start at specific coord and direction filled. Also
/// checks overlaps. Returns true if cell was populated
static bool try_place_ship(Map map, Point at, int size, Direction dir) {
    for (int i = 0; i < size; i++) {
        if (dir == Up && (at.x + i > Size || map[at.x + i][at.y]))
            return false;
        if (dir == Down && (at.x - i < 0 || map[at.x - i][at.y]))
            return false;
        if (dir == Right && (at.y + i > Size || map[at.x][at.y + i]))
            return false;
        if (dir == Left && (at.y - i < 0 || map[at.x][at.y - i]))
            return false;
    }

    for (int i = 0; i < size; i++) {
        if (dir == Up)
            map[at.x + i][at.y] = Alive;
        if (dir == Down)
            map[at.x - i][at.y] = Alive;
        if (dir == Right)
            map[at.x][at.y + i] = Alive;
        if (dir == Left)
            map[at.x][at.y - i] = Alive;
    }

    return true;
}

Game *generate_map() {
    Game *g = malloc(sizeof(Game));
    int ship_sizes[] = {4, 3, 2, 2, 1, 1};

    for (int i = 0; i < sizeof(ship_sizes) / sizeof(*ship_sizes);) {
        Point p = {.x = rand() % Size, .y = rand() % Size};
        Direction dir = rand() % DSize;

        if (try_place_ship(g->map, p, ship_sizes[i], dir))
            i++;
    }
    return g;
}

void display_map(Game *game) {
    printf("  | ");
    for (int i = 0; i < Size; i++)
        printf(" %c ", 'a' + i);
    printf("\n");

    for (int i = 0; i < Size; i++) {
        printf("%c | ", 'a' + i);

        for (int j = 0; j < Size; j++) {
            if (game->map[i][j] == Alive)
                printf(" ▦ ");
            else if (game->map[i][j] == Dead)
                printf(" ▢ ");
            else
                printf("   ");
        }
        printf("\n");
    }
}

CellState check_at(Game *game, Point at) { return game->map[at.x][at.y]; }

/// Returns state of cell if coord x & y available on the map, empty cell state
/// otherwise.
static CellState checked_at(Game *game, int x, int y) {
    if (0 <= x && x < Size && 0 <= y && y <= Size)
        return game->map[x][y];
    return Empty;
}

/// Returns true if any cell on the map is populated, false otherwise
static bool ships_remained(Game *game) {
    for (int i = 0; i < Size; i++)
        for (int j = 0; j < Size; j++)
            if (game->map[i][j] == Alive)
                return true;
    return false;
}

Result try_kill(Game *game, Point at) {
    if (game->map[at.x][at.y] == Empty || game->map[at.x][at.y] == Dead)
        return Miss;

    game->map[at.x][at.y] = Dead;

    if (!ships_remained(game))
        return Loose;

    if (game->map[at.x][at.y] == Dead &&
        checked_at(game, at.x - 1, at.y - 1) != Alive &&
        checked_at(game, at.x - 1, at.y + 0) != Alive &&
        checked_at(game, at.x - 1, at.y + 1) != Alive &&
        checked_at(game, at.x + 0, at.y - 1) != Alive &&
        checked_at(game, at.x + 0, at.y + 1) != Alive &&
        checked_at(game, at.x + 1, at.y - 1) != Alive &&
        checked_at(game, at.x + 1, at.y + 0) != Alive &&
        checked_at(game, at.x + 1, at.y + 1) != Alive)
        return Full;

    return Partial;
}