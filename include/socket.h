#pragma once

typedef unsigned short port_t;
typedef int sock_handle;
typedef int (*socket_handler)(sock_handle);

/// Packed version of ip and port
typedef struct {
    const char *addr;
    port_t port;
} IPv4;

/// Tries to open a socket connection and bind it to specific ip and port
sock_handle open_socket_at(IPv4 ip);

/// Starts to accept socket connections. On each connection, handler is called
int accept_connections(sock_handle sock, socket_handler handler);

/// Tries to connect to a socket at specific ip and port
int open_connection(sock_handle sock, IPv4 to);