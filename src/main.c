#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <linux/netlink.h>


#define MSG_LEN 512


static int nl_socket_save = -1;
static void signal_handler(__attribute__((unused)) int signum)
{
    printf("\nnetlink_socket[%d]:closed\n", nl_socket_save);
    close(nl_socket_save);
    exit(EXIT_SUCCESS);
}


static int get_netlink_socket()
{
    int ret = -1;
    int nl_socket = -1;
    struct sockaddr_nl src_addr = {0};

    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();
    src_addr.nl_groups = -1;

    nl_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
    if (nl_socket < 0) {
        perror("socket()");
        return -1;
    }

    ret = bind(nl_socket, (struct sockaddr *)&src_addr, sizeof(struct sockaddr_nl));
    if (ret == -1) {
        perror("bind()");
        close(nl_socket);
        return -1;
    }

    return nl_socket;
}


static int receive_netlink_msg(int nl_socket)
{
    int ret = -1;
    struct pollfd fds[1] = {0};
    char msg[MSG_LEN] = {0};
    ssize_t size_read = 0;

    // set the poll on the netlink socket
    fds[0].fd = nl_socket;
    fds[0].events = POLLIN;

    printf("Waiting for events ...\n");
    while (1) {

        ret = poll(fds, 1, -1);
        if (ret == -1) {
            perror("poll()");
            close(nl_socket);
            return -1;
        }

        if (fds[0].revents & POLLIN) {
            // TODO: replace recv by recvmsg
            size_read = recv(nl_socket, msg, sizeof(msg), MSG_DONTWAIT);
            printf("size_read:%ld msg:%s\n", size_read, msg);
        }

    }

    return 0;
}


static int entry()
{
    int ret = -1;
    struct sigaction action = {0};
    int nl_socket = -1;

    action.sa_handler = signal_handler;
    ret = sigaction(SIGINT, &action, NULL);
    if (ret == -1) {
        perror("sigaction()");
        return -1;
    }

    nl_socket = get_netlink_socket();
    if (nl_socket == -1)
        return -1;
    nl_socket_save = nl_socket;

    receive_netlink_msg(nl_socket);

    return 0;
}


int main()
{
    entry();
    return 0;
}

