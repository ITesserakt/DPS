#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "shared.h"
#include "socket.h"
#include "utils.h"

sock_handle serv_sock;
Game *server_game;

/// Called when SIGINT received. Closes opened socket connection and frees
/// memory
void graceful_shutdown(int _sig) {
    free(server_game);
    close(serv_sock);
    exit(0);
}

/// Creates game map on each connection and begins game loop
int do_game(sock_handle client_socket) {
    server_game = generate_map();

    greetings();

    TRY(game_loop(server_game, client_socket));

    return 0;
}

int main(int argc, char **argv) {
    srand(time(NULL));
    char *ip;
    unsigned short port;

    parse_command_args(argc, argv, &ip, &port);

    IPv4 ip_addr = {.addr = ip, .port = port};
    serv_sock = open_socket_at(ip_addr);
    TRY(serv_sock);

    signal(SIGINT, graceful_shutdown);

    printf("Started socket connection at %s:%d\n", ip, port);

    TRY(accept_connections(serv_sock, do_game));
}