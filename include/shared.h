#pragma once

#include "command.h"
#include "socket.h"
#include "game.h"

void help_prompt(const char *exec);

void parse_command_args(int argc, char **argv, char **ip, unsigned short *port);

int process_command(Command cmd, Game *game, sock_handle socket);

int receive_command(Game *game, sock_handle socket);

int game_loop(Game *game, sock_handle socket);