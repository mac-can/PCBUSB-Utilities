//
//  can_send.c
//  CAN/IPC Client
//  SocketCAN CC format (16 bytes)
//
//  ATTENTION: Make sure that the server is configured to receive CAN CC frames!
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
#include "tcp_client.h"

#ifndef FRAMES
#define FRAMES 2048U
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

int main(int argc, char *argv[]) {
    int fildes = (-1);
    struct can_frame msg;
    uint32_t i, frames = FRAMES;

    if (argc > 1) {
        frames = (uint32_t)strtoul(argv[1], NULL, 0);
    }
    if ((signal(SIGINT, sigterm) == SIG_ERR) ||
#if !defined(_WIN32) && !defined(_WIN64)
        (signal(SIGHUP, sigterm) == SIG_ERR) ||
#endif
        (signal(SIGTERM, sigterm) == SIG_ERR)) {
          perror("+++ error");
          return errno;
    }
    if ((fildes = tcp_client_connect(server)) < 0) {
        perror("+++ error");
        return EXIT_FAILURE;
    }
    printf("Connected to server %s\n", server);
    
    printf("Press ^C to abort.\n");
    for (i = 0; (i < frames) && running; i++) {
        memset(&msg, 0, sizeof(msg));
        msg.can_id = (canid_t)htonl(i & 0x7FFU);
        msg.data[0] = (uint8_t)(i & 0xFF);
        msg.data[1] = (uint8_t)((i >> 8) & 0xFF);
        msg.data[2] = (uint8_t)((i >> 16) & 0xFF);
        msg.data[3] = (uint8_t)((i >> 24) & 0xFF);
        msg.len = (uint8_t)CAN_MAX_DLEN;
        if (tcp_client_send(fildes, (const char*)&msg, sizeof(msg)) < 0) {
            perror("+++ error");
            break;
        }
    }
    if (tcp_client_close(fildes) < 0) {
        perror("+++ error");
        return EXIT_FAILURE;
    }
    printf("Connection closed: %u CAN frames sent\n", i);
    return EXIT_SUCCESS;
}

static void sigterm(int signo) {
    running = 0;
    (void)signo;
}
