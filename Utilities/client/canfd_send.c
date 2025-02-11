//
//  canfd_send.c
//  CAN-to-Eternet Client (SocketCAN)
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#ifndef __APPLE__
#include <linux/can.h>
#else
#include "mac/can.h"
#endif
#include "ipc_client.h"

#ifndef FRAMES
#define FRAMES 2048
#endif
#ifndef FLAGS
#define FLAGS (CANFD_BRS | CANFD_FDF)
#endif
#ifndef PORT
#define PORT 60000
#endif
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define SERVER "localhost:" TOSTRING(PORT)

static void sigterm(int signo);
static volatile int running = 1;

static const char *server = SERVER;

int main() {
    int fildes = (-1);
    struct canfd_frame msg;
    uint32_t frames = 0;

    if ((signal(SIGINT, sigterm) == SIG_ERR) ||
#if !defined(_WIN32) && !defined(_WIN64)
        (signal(SIGHUP, sigterm) == SIG_ERR) ||
#endif
        (signal(SIGTERM, sigterm) == SIG_ERR)) {
          perror("+++ error");
          return errno;
    }
    if ((fildes = ipc_client_connect(server, IPC_SOCK_TCP)) < 0) {
        perror("+++ error");
        return EXIT_FAILURE;
    }
    printf("Connected to server %s\n", server);
    
    for (frames = 0; (frames < FRAMES) && running; frames++) {
        memset(&msg, 0, sizeof(msg));
        msg.can_id = (canid_t)htonl(frames);
        msg.data[0] = (uint8_t)(frames & 0xFF);
        msg.data[1] = (uint8_t)((frames >> 8) & 0xFF);
        msg.data[2] = (uint8_t)((frames >> 16) & 0xFF);
        msg.data[3] = (uint8_t)((frames >> 24) & 0xFF);
        msg.flags = (uint8_t)FLAGS;
        msg.len = (uint8_t)CANFD_MAX_DLEN;
        if (ipc_client_send(fildes, (const char*)&msg, sizeof(msg)) < 0) {
            perror("+++ error");
            break;
        }
    }
    if (ipc_client_close(fildes) < 0) {
        perror("+++ error");
        return EXIT_FAILURE;
    }
    printf("Connection closed: %u CAN FD frames sent\n", frames);
    return EXIT_SUCCESS;
}

static void sigterm(int signo) {
    running = 0;
    (void)signo;
}
