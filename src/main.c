#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <linux/netlink.h>


static int nl_socket_save = -1;
static void signal_handler(__attribute__((unused)) int signum)
{
    printf("Netlink socket[%d]:closed\n", nl_socket_save);
    close(nl_socket_save);
    exit(EXIT_SUCCESS);
}


static int get_netlink_socket()
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


static int read_netlink_socket(int netlink_socket)
{
    char buf[4096] = {0};
    struct sockaddr_nl sa;

    struct iovec iov = {
        .iov_base = buf,
        .iov_len = sizeof(buf)
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

    ssize_t len = recvmsg(netlink_socket, &msg, 0);
    if (len == -1) {
        perror("recvmsg");
        return -1;
    }

    printf("%s\n", buf);

    return 0;
}


static int set_poll_trigger(int netlink_socket)
{
    struct pollfd pfd = {
        .fd = netlink_socket,
        .events = POLLIN,
    };

    printf("Waiting for events ...\n");
    while (1) {

        if (poll(&pfd, 1, -1) == -1) {
            perror("poll");
            close(netlink_socket);
            return -1;
        }

        if (pfd.revents & POLLIN)
            read_netlink_socket(netlink_socket);
    }

    return 0;
}


int main()
{
    struct sigaction action = {
        .sa_handler = signal_handler,
    };

    if (sigaction(SIGINT, &action, NULL) == -1) {
        perror("sigaction");
        return -1;
    }

    int netlink_socket = get_netlink_socket();
    if (netlink_socket == -1)
        return -1;
    nl_socket_save = netlink_socket;

    set_poll_trigger(netlink_socket);

    return 0;
}

