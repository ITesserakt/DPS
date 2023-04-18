#pragma once

typedef unsigned short port_t;
typedef int sock_handle;
typedef void (*socket_handler)(sock_handle);

typedef struct {
    const char *addr;
    port_t port;
} IPv4;

sock_handle open_socket_at(IPv4 ip);

int accept_connections(sock_handle sock, socket_handler handler);

int open_connection(sock_handle sock, IPv4 to);