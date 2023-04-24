#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <signal.h>

#include "shared.h"
#include "utils.h"

void help_prompt(const char *exec) {
    printf("Usage: ./%s [IP] [PORT]\n", exec);
    printf("\tDefault: IP 127.0.0.1\n");
    printf("\t\t PORT 5001\n");
}

void parse_command_args(int argc, char **argv, char **ip,
                        unsigned short *port) {
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

static int sendall(sock_handle socket, char *buf, int len, int flags) {
    int total = 0;
    int n;

    while (total < len) {
        n = send(socket, buf + total, len - total, flags);
        if (n == -1)
            break;

        total += n;
    }

    return (n == -1 ? -1 : total);
}

static int recvall(sock_handle socket, char *buf, int len, int flags) {
    int total = 0;
    int n;

    while (total < len) {
        n = recv(socket, buf + total, len - total, flags);
        if (n == -1)
            break;
        if (n == 0)
            break;
        total += n;
    }

    return (n == -1 ? -1 : total);
}

int receive_command(Game *game, sock_handle socket) {
    char buf[32];

    TRY(recvall(socket, buf, sizeof(buf), 0));

    int x, y;
    Result in_res;
    if (sscanf(buf, "msl %d %d", &x, &y) > 0) {
        Point p = {.x = x, .y = y};
        Result r = try_kill(game, p);
        Command c = {.type = MissleRes, .missle_res = r};

        TRY(process_command(c, game, socket));
    } else if (sscanf(buf, "rst %d", &in_res) > 0) {
        if (in_res == Miss)
            printf("Missed\n");
        else if (in_res == Partial)
            printf("Partial\n");
        else if (in_res == Full)
            printf("Full\n");
        else if (in_res == Loose) {
            printf("You win\n");
            shutdown(socket, SHUT_RDWR);
            return -2;
        }
    } else {
        printf("Unknown payload received: %s\n", buf);
        return -1;
    }

    return 0;
}

int process_command(Command cmd, Game *game, sock_handle socket) {
    char buf[32];

    if (cmd.type == Display) {
        display_map(game);
    } else if (cmd.type == MissleAt) {
        int size = sprintf(buf, "msl %d %d", cmd.missle_at.x, cmd.missle_at.y);
        TRY(size);
        TRY(sendall(socket, buf, sizeof(buf), 0));
        TRY(receive_command(game, socket));
    } else if (cmd.type == Quit) {
        shutdown(socket, SHUT_RDWR);
        raise(SIGINT);
    } else if (cmd.type == MissleRes) {
        int size = sprintf(buf, "rst %d", cmd.missle_res);
        TRY(size);
        TRY(sendall(socket, buf, sizeof(buf), 0));
    }

    return 0;
}

int game_loop(Game *game, sock_handle socket) { 
    while(true) {
        Command cmd = ask_user();
        TRY(process_command(cmd, game, socket));

        if (is_blocking(cmd)) {
            printf("Waiting for opponent...\n");
            TRY(receive_command(game, socket));
        }
    }
}