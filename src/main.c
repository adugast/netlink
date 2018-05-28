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
    struct sockaddr_nl src_addr = {0};

    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();
    src_addr.nl_groups = -1;

    /* NETLINK_KOBJECT_UEVENT: Get kernel messages to user space */
    int netlink_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
    if (netlink_socket < 0) {
        perror("socket");
        return -1;
    }

    if (bind(netlink_socket, (struct sockaddr *)&src_addr, sizeof(struct sockaddr_nl)) == -1) {
        perror("bind");
        close(netlink_socket);
        return -1;
    }

    return netlink_socket;
}


static int read_netlink_socket(int netlink_socket)
{
    char buf[MSG_LEN] = {0};
    struct iovec iov = { buf, sizeof(buf) };
    struct sockaddr_nl sa;
    struct msghdr msg = { &sa, sizeof(sa), &iov, 1, NULL, 0, 0 };

    ssize_t len = recvmsg(netlink_socket, &msg, 0);

    struct nlmsghdr *nh = (struct nlmsghdr *)buf;
    while (NLMSG_OK(nh, len)) {

        /* The end of multipart message. */
        if (nh->nlmsg_type == NLMSG_DONE) {
            printf("end netlink msg\n");
            return 0;
        }

        if (nh->nlmsg_type == NLMSG_ERROR) {
            /* Do some error handling. */
            printf("error msg\n");
        }

        /* Continue here  with parsing payload. */
        /* ... */

        nh = NLMSG_NEXT(nh, len);
    }

    return 0;
}


static int set_poll_trigger(int netlink_socket)
{
    int ret = -1;
    struct pollfd pfd = {0};

    // msg and size_read variable must be removed
    char msg[MSG_LEN] = {0};
    ssize_t size_read = 0;

    // set the poll on the netlink socket
    pfd.fd = netlink_socket;
    pfd.events = POLLIN;

    printf("Waiting for events ...\n");
    while (1) {

        ret = poll(&pfd, 1, -1);
        if (ret == -1) {
            perror("poll()");
            close(netlink_socket);
            return -1;
        }

        if (pfd.revents & POLLIN) {
            // TODO: replace recv by recvmsg
//            read_netlink_socket(netlink_socket);
            size_read = recv(netlink_socket, msg, sizeof(msg), MSG_DONTWAIT);
            printf("size_read:%ld msg:%s\n", size_read, msg);
        }

    }

    return 0;
}


static int entry()
{
    // set ctrl-c signal handler to exit the loop properly
    struct sigaction action;

    action.sa_handler = signal_handler;
    if (sigaction(SIGINT, &action, NULL) == -1) {
        perror("sigaction");
        return -1;
    }

    // retrive netlink_socket
    int netlink_socket = get_netlink_socket();
    if (netlink_socket == -1)
        return -1;
    nl_socket_save = netlink_socket;

    // set poll struct to trigger the read_netlink_socket
    // function when the sockets receives data
    set_poll_trigger(netlink_socket);

    return 0;
}


int main()
{
    entry();
    return 0;
}

