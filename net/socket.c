/* gOS Kernel — Socket Implementation (stub) */

#include "net/socket.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "gOS/errno.h"
#include "lib/string.h"

void socket_init(void) {
    klog_info("Socket layer initialized (stub)");
}

socket_t *socket_create(int domain, int type, int protocol) {
    socket_t *sock = kzalloc(sizeof(socket_t));
    if (!sock) return NULL;
    sock->domain = domain;
    sock->type = type;
    sock->protocol = protocol;
    sock->bound = false;
    sock->connected = false;
    klog_debug("Socket created: domain=%d type=%d proto=%d", domain, type, protocol);
    return sock;
}

int socket_bind(socket_t *sock, uint32_t ip, uint16_t port) {
    if (!sock) return -EINVAL;
    sock->local_ip = ip;
    sock->local_port = port;
    sock->bound = true;
    klog_debug("Socket bound to %d.%d.%d.%d:%u",
               (ip & 0xFF), (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF, port);
    return 0;
}

int socket_connect(socket_t *sock, uint32_t ip, uint16_t port) {
    if (!sock) return -EINVAL;
    sock->remote_ip = ip;
    sock->remote_port = port;
    sock->connected = true;
    klog_debug("Socket connected to %d.%d.%d.%d:%u",
               (ip & 0xFF), (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF, port);
    return 0;
}

int socket_send(socket_t *sock, const void *buf, size_t len) {
    (void)sock; (void)buf; (void)len;
    klog_warn("socket_send: stub");
    return -ENOSYS;
}

int socket_recv(socket_t *sock, void *buf, size_t len) {
    (void)sock; (void)buf; (void)len;
    klog_warn("socket_recv: stub");
    return -ENOSYS;
}

void socket_close(socket_t *sock) {
    if (!sock) return;
    kfree(sock);
}
