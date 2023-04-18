#include <sys/socket.h>

#include "socket.h"
#include "shared.h"
#include "utils.h"

int main(int argc, char **argv) {
    char *ip;
    unsigned short port;

    parse_command_args(argc, argv, &ip, &port);
    sock_handle sock = socket(AF_INET, SOCK_STREAM, 0);

    IPv4 ip_addr = {.addr = ip, .port = port};
    TRY(open_connection(sock, ip_addr));
}