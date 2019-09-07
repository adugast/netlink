#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "netlink.h"


static nl_t *nl_hdl_g = NULL;
static void signal_handler(__attribute__((unused)) int signum)
{
    nl_deinit_handler(nl_hdl_g);
    printf("Netlink handler cleaned-up\n");
    exit(EXIT_SUCCESS);
}


static void nl_reader_cb(unsigned int msglen, char *msg, void *ctx)
{
    ctx = ctx;
    printf("msglen[%d]:[%s]\n", msglen, msg);
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

    nl_t *hdl = nl_init_handler(nl_reader_cb, 4096, NULL);
    if (!hdl) {
        printf("nl_init_handler failed\n");
        return -1;
    }
    nl_hdl_g = hdl;

    // blocking call, waiting for netlink event
    nl_launch_listener(hdl);

    return 0;
}

