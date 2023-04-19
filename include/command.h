#pragma once

#include <game.h>

typedef enum { Display, MissleAt, Quit, MissleRes } CommandType;

typedef struct {
    int x, y;
} MissleAtAttr;

typedef struct {
    union {
        MissleAtAttr missle_at;
        Result missle_res;
    };

    CommandType type;
} Command;

void greetings();

Command ask_user();

bool is_blocking(Command cmd);