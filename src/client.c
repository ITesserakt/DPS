#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include "socket.h"
#include "shared.h"
#include "utils.h"
#include "game.h"

Game *client_game;
sock_handle client_socket;

void graceful_shutdown(int _sig) {
    free(client_game);
    close(client_socket);
    exit(0);
}

int main(int argc, char **argv) {
    srand(time(NULL));
    char *ip;
    unsigned short port;

    parse_command_args(argc, argv, &ip, &port);
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    TRY(client_socket);

    IPv4 ip_addr = {.addr = ip, .port = port};
    printf("Connecting to %s:%d\n", ip, port);
    TRY(open_connection(client_socket, ip_addr));

    client_game = generate_map();
    signal(SIGINT, graceful_shutdown);
    
    TRY(receive_command(client_game, client_socket));
    
    greetings();

    TRY(game_loop(client_game, client_socket));
}