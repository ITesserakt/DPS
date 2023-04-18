#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "utils.h"
#include "socket.h"
#include "shared.h"

typedef enum { SocketFailed = -1, Ok = 0 } Error;

void nothing(sock_handle _h) {
    
}

int main(int argc, char **argv) {
    char *ip;
    unsigned short port;

    parse_command_args(argc, argv, &ip, &port);

    IPv4 ip_addr = {.addr = ip, .port = port};
    sock_handle socket = open_socket_at(ip_addr);
    TRY(socket);

    printf("Started socket connection at %s:%d\n", ip, port);

    TRY(accept_connections(socket, nothing));
}