#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shared.h"

void help_prompt(const char* exec) {
    printf("Usage: ./%s [IP] [PORT]\n", exec);
    printf("\tDefault: IP 127.0.0.1\n");
    printf("\t\t PORT 5001\n");
}

void parse_command_args(int argc, char **argv, char **ip, unsigned short *port) {
    *ip = "127.0.0.1";
    *port = 5001;

    if (argc > 1 && strcmp(argv[1], "-h") == 0) {
        help_prompt(argv[0]);
        exit(0);
    } else if (argc > 1) {
        *ip = argv[1];
    }
    if (argc > 2) {
        *port = atoi(argv[2]);
    }
}