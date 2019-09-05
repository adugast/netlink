#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <poll.h>

#include "netlink.h"


struct nl {
    int nl_sock;
    ssize_t buffer_size;
    char *buffer;
    nl_reader_cb_t cb;
    void *ctx;
};


static int _nl_get_socket()
{
    int netlink_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
    if (netlink_socket == -1)
        goto cleanup;

    struct sockaddr_nl addr = {
        .nl_family = AF_NETLINK,
        .nl_pid = getpid(),
        .nl_groups = -1
    };

    if (bind(netlink_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr_nl)) == -1)
        goto cleanup_socket;

    return netlink_socket;

cleanup_socket:
    close(netlink_socket);
cleanup:
    return -1;
}


static int _nl_read_socket(nl_t *hdl)
{
    struct sockaddr_nl sa;

    struct iovec iov = {
        .iov_base = hdl->buffer,
        .iov_len = hdl->buffer_size,
    };

    struct msghdr msg = {
        .msg_name = &sa,
        .msg_namelen = sizeof(sa),
        .msg_iov = &iov,
        .msg_iovlen = 1,
        .msg_control = NULL,
        .msg_controllen = 0,
        .msg_flags = 0
    };

    ssize_t msglen = recvmsg(hdl->nl_sock, &msg, 0);
    if (msglen == -1) {
        perror("recvmsg");
        return -1;
    }

    if (hdl->cb)
        hdl->cb(msglen, hdl->buffer, hdl->ctx);

    return 0;
}


nl_t *nl_init_handler(nl_reader_cb_t cb, ssize_t buffer_size, void *ctx)
{
    if (!cb)
        goto end;

    nl_t *hdl = calloc(1, sizeof(nl_t));
    if (!hdl)
        goto end;

    hdl->nl_sock = _nl_get_socket();
    if (hdl->nl_sock == -1)
        goto clean;

    hdl->buffer_size = buffer_size;
    hdl->buffer = calloc(buffer_size, sizeof(char));
    if (!hdl->buffer)
        goto clean;
    hdl->cb = cb;
    hdl->ctx = ctx;

    return hdl;

clean:
    free(hdl);
end:
    return NULL;
}


int nl_launch_listener(nl_t *hdl)
{
    struct pollfd pfd = {
        .fd = hdl->nl_sock,
        .events = POLLIN,
    };

    printf("Waiting for events ...\n");
    while (1) {

        if (poll(&pfd, 1, -1) == -1) {
            perror("poll");
            nl_deinit_handler(hdl);
            return -1;
        }

        if (pfd.revents & POLLIN)
            _nl_read_socket(hdl);
    }

    return 0;
}


void nl_deinit_handler(nl_t *hdl)
{
    if (!hdl)
        return;

    if (hdl->buffer)
        free(hdl->buffer);

    close(hdl->nl_sock);
    free(hdl);
}

