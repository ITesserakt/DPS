#pragma once

#include <game.h>

/// Command types that user used to interact with program
typedef enum { Display, MissleAt, Quit, MissleRes } CommandType;

/// Contains coordinates to missle to
typedef struct {
    int x, y;
} MissleAtAttr;

/// Contain parsed command and its attributes
typedef struct {
    union {
        MissleAtAttr missle_at;
        Result missle_res;
    };

    CommandType type;
} Command;

/// Prints greet message and all allowed command to enter
void greetings();

/// Waits for user input and parses it to command object
Command ask_user();

/// Determines whether command will block execution of app until response is
/// received
bool is_blocking(Command cmd);