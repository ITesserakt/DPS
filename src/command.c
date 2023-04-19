#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "command.h"

void greetings() { printf("Available commands: dsp | msl <x> <y> | exit\n"); }

static void read_line(char *s) {
    char c;
    int index = 0;
    while (scanf("%c", &c) == 1) {
        if (c == EOF || c == '\n')
            break;

        s[index++] = c;
    }

    s[index] = '\0';
}

Command ask_user() {
    printf("> ");

    char x, y;
    char cmd[32];

    read_line(cmd);

    if (strncmp(cmd, "dsp", 3) == 0) {
        Command command = {.type = Display};
        return command;
    }
    int foo = sscanf(cmd, "msl %c %c", &x, &y);
    if (foo) {
        MissleAtAttr attr = {.x = x - 'a', .y = y - 'a'};
        Command command = {.type = MissleAt, .missle_at = attr};
        return command;
    }
    if (strncmp(cmd, "exit", 4) == 0) {
        Command command = {.type = Quit};
        return command;
    }

    printf("Unknown command entered: %s\n", cmd);
    return ask_user();
}

bool is_blocking(Command cmd) {
    if (cmd.type == MissleAt)
        return true;
    return false;
}
