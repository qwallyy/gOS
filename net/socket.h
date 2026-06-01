/* gOS Kernel — Socket Abstraction (stub)
 * Reference: https://wiki.osdev.org/Berkeley_Sockets
 */

#ifndef GOS_SOCKET_H
#define GOS_SOCKET_H

#include "gOS/types.h"

#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3

#define AF_INET         2
#define AF_INET6        10
#define AF_PACKET       17

typedef struct socket {
    int domain;
    int type;
    int protocol;
    bool bound;
    bool connected;
    uint32_t local_ip;
    uint32_t remote_ip;
    uint16_t local_port;
    uint16_t remote_port;
    void *private_data;
} socket_t;

void socket_init(void);
socket_t *socket_create(int domain, int type, int protocol);
int socket_bind(socket_t *sock, uint32_t ip, uint16_t port);
int socket_connect(socket_t *sock, uint32_t ip, uint16_t port);
int socket_send(socket_t *sock, const void *buf, size_t len);
int socket_recv(socket_t *sock, void *buf, size_t len);
void socket_close(socket_t *sock);

#endif /* GOS_SOCKET_H */
