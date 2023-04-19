#include <arpa/inet.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "socket.h"
#include "utils.h"

sock_handle open_socket_at(IPv4 ip) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    TRY(sock);

    struct sockaddr_in sock_addr = {.sin_family = AF_INET,
                                    .sin_port = htons(ip.port),
                                    .sin_addr = {.s_addr = inet_addr(ip.addr)}};

    TRY(bind(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr)));

    return sock;
}

int accept_connections(sock_handle sock, socket_handler handler) {
    TRY(listen(sock, 1));

    while (true) {
        struct sockaddr_in incoming;
        unsigned int incoming_size;
        sock_handle incoming_handle =
            accept(sock, (struct sockaddr *)&incoming, &incoming_size);

        TRY(incoming_handle);

        printf("Received new connection from %s:%d\n",
               inet_ntoa(incoming.sin_addr), ntohs(incoming.sin_port));
        fflush(stdout);

        handler(incoming_handle);

        printf("Client disconnected\n");
    }
}

int open_connection(sock_handle sock, IPv4 to) {
    struct sockaddr_in sock_addr = {.sin_family = AF_INET,
                                    .sin_port = htons(to.port),
                                    .sin_addr = {.s_addr = inet_addr(to.addr)}};

    TRY(connect(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr)));

    return 0;
}
