//
//  canfd_recv.c
//  CAN/IPC Client
//  SocketCAN FD format (72 bytes)
//
//  ATTENTION: Make sure that the server is configured to send CAN FD frames!
//
#include <stdio.h>
#include <stdint.h>
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
#include <arpa/inet.h>

#ifndef PORT
#define PORT 60000
#endif
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define SERVER "127.0.0.1:" TOSTRING(PORT)

static void sigterm(int signo);
static volatile int running = 1;

static const char *server = SERVER;

int main() {
    int fildes = (-1);
    struct canfd_frame msg;
    uint32_t frames = 0;
    ssize_t n = 0;

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
    while (running) {
        memset(&msg, 0, sizeof(msg));
        if ((n = tcp_client_recv(fildes, (void*)&msg, sizeof(msg), TCP_WAIT_FOREVER)) < 0) {
            if (errno != ENODATA)
                perror("+++ error");
            if (errno == EBADMSG)
                break;
            continue;
        }
        if (n != (ssize_t)sizeof(msg)) {
            fprintf(stderr, "+++ error: message with wrong frame size received\n");
            break;
        }
        msg.can_id = ntohl(msg.can_id);
        printf("))) %u\t", frames++);
        //printf("%7li.%04li\t", (long)msg.timestamp.tv_sec, msg.timestamp.tv_nsec / 100000);
        printf("%03X\t", (msg.can_id & ((msg.can_id & CAN_EFF_FLAG) ? CAN_EFF_MASK : CAN_SFF_MASK)));
        if (!(msg.can_id & CAN_ERR_FLAG)) {
            putchar((msg.can_id & CAN_EFF_FLAG) ? 'X' : 'S');
            putchar((msg.flags & CANFD_FDF) ? 'F' : '-');
            putchar((msg.flags & CANFD_BRS) ? 'B' : '-');
            putchar((msg.flags & CANFD_ESI) ? 'E' : '-');
            putchar((msg.can_id & CAN_RTR_FLAG) ? 'R' : '-');
        } else {
            printf("Error");
        }
        printf(" [%u]", msg.len);
        for (uint8_t i = 0; (i < msg.len) && (i < CANFD_MAX_DLEN); i++) {
            printf(" %02X", msg.data[i]);
        }
        printf("\n");
    }
    if (tcp_client_close(fildes) < 0) {
        perror("+++ error");
        return EXIT_FAILURE;
    }
    printf("Connection closed: %u CAN FD frames received\n", frames);
    return EXIT_SUCCESS;
}

static void sigterm(int signo) {
    running = 0;
    (void)signo;
}
