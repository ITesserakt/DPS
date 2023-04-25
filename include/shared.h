#pragma once

#include "command.h"
#include "game.h"
#include "socket.h"

/// Prints how to use this app to stdout
void help_prompt(const char *exec);

/// Parses ip address and port to connect to from command line args
void parse_command_args(int argc, char **argv, char **ip, unsigned short *port);

/// Applies specific command to the current game state
int process_command(Command cmd, Game *game, sock_handle socket);

/// Blocks until next response received and processes it
int receive_command(Game *game, sock_handle socket);

/// Greets the user, reads next command and processes it in infinite loop
int game_loop(Game *game, sock_handle socket);