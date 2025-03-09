/* -- $HeadURL: https://svn.uv-software.net/MacCAN/PCANUSB/Utilities/can_port/trunk/main.c $ --
 *
 *  project   :  CAN - Controller Area Network
 *
 *  purpose   :  CAN/IPC Server (SocketCAN Frame Format)
 *
 *  copyright :  (c) 2008,2012 by UV Software, Friedrichshafen
 *               (c) 2013-2025 by UV Software, Berlin
 *
 *  revision  :  $Rev: 2088 $ of $Date: 2025-03-01 18:25:58 +0100 (Sat, 01 Mar 2025) $
 * 
 *  author(s) :  Uwe Vogt, UV Software
 *
 *  e-mail    :  uwe.vogt@uv-software.de
 *
 *
 *  -----------  description  --------------------------------------------
 *
 *  CAN/IPC Server (based on macOS Library for PCAN USB Interfaces)
 *
 *  (1) Standalone version with compiled PCBUSB sources
 *
 *  This program is freeware without any warranty or support!
 *  Please note the copyright and license agreements.
 *
 *  Note: This version does not require the libPCBUSB to be installed.
 *
 *  (2) Open-source version with libPCBUSB support
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, see <https://www.gnu.org/licenses/>.
 *
 *  Note: The libPCBUSB is licensed under a freeware license without any
 *  warranty or support.  The libPCBUSB is not part of this program.
 *  It can be downloaded from <https://www.mac-can.com/>.
 *
 */
/*  -----------  version  ------------------------------------------------
 */
#include "build_no.h"
#if (OPTION_PCBUSB_STANDALONE != 0)
#define VERSION_MAJOR     0
#define VERSION_MINOR     0
#define VERSION_PATCH     99
#else
#define VERSION_MAJOR     0
#define VERSION_MINOR     0
#define VERSION_PATCH     99
#endif
#define VERSION_BUILD     BUILD_NO
#define VERSION_STRING    TOSTRING(VERSION_MAJOR) "." TOSTRING(VERSION_MINOR) "." TOSTRING(VERSION_PATCH) " (" TOSTRING(BUILD_NO) ")"
#if defined(__APPLE__)
#define PLATFORM    "macOS"
#elif defined(__linux__)
#define PLATFORM    "Linux"
#else
#error Unsupported platform
#endif
#if defined(__APPLE__)
static const char APPLICATION[] = "CAN/IPC Server for PEAK-System PCAN USB Interfaces, Version " VERSION_STRING;
static const char COPYRIGHT[]   = "Copyright (c) 2008,2012-2025 by Uwe Vogt, UV Software, Berlin";
#if (OPTION_PCBUSB_STANDALONE != 0)
static const char WARRANTY[]    = "This program is freeware without any warranty or support!";
static const char LICENSE[]     = "This program is freeware without any warranty or support!\n\n" \
                                  "Note: This program does not require the libPCBUSB to be installed.";
#else
static const char WARRANTY[]    = "This program comes with ABSOLUTELY NO WARRANTY!\n\n" \
                                  "This is free software, and you are welcome to redistribute it\n" \
                                  "under certain conditions; type `can_port --version' for details.";
static const char LICENSE[]     = "This program is free software; you can redistribute it and/or modify\n" \
                                  "it under the terms of the GNU General Public License as published by\n" \
                                  "the Free Software Foundation; either version 2 of the License, or\n" \
                                  "(at your option) any later version.\n\n" \
                                  "This program is distributed in the hope that it will be useful,\n" \
                                  "but WITHOUT ANY WARRANTY; without even the implied warranty of\n" \
                                  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" \
                                  "GNU General Public License for more details.\n\n" \
                                  "You should have received a copy of the GNU General Public License along\n" \
                                  "with this program; if not, see <https://www.gnu.org/licenses/>.\n\n" \
                                  "Note: The libPCBUSB is licensed under a freeware license without any\n" \
                                  "warranty or support.  The libPCBUSB is not part of this program.\n" \
                                  "It can be downloaded from <https://www.mac-can.com/>.";
#endif
#else
static const char APPLICATION[] = "CAN/IPC Server for PEAK-System PCAN USB Interfaces, Version " VERSION_STRING;
static const char COPYRIGHT[]   = "Copyright (c) 2008,2012-2025 by Uwe Vogt, UV Software, Berlin";
static const char WARRANTY[]    = "This program comes with ABSOLUTELY NO WARRANTY!\n\n" \
                                  "This is free software, and you are welcome to redistribute it\n" \
                                  "under certain conditions; type `can_port --version' for details.";
static const char LICENSE[]     = "This program is free software; you can redistribute it and/or modify\n" \
                                  "it under the terms of the GNU General Public License as published by\n" \
                                  "the Free Software Foundation; either version 2 of the License, or\n" \
                                  "(at your option) any later version.\n\n" \
                                  "This program is distributed in the hope that it will be useful,\n" \
                                  "but WITHOUT ANY WARRANTY; without even the implied warranty of\n" \
                                  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" \
                                  "GNU General Public License for more details.";
#endif
static const char SECURITY[]    = "This program will open a network socket for CAN/IPC communication.\n" \
                                  "This may expose your computer to security vulnerabilities, unauthorized\n" \
                                  "access, data interception, denial of service attacks, and resource\n" \
                                  "exhaustion.\n\n" \
                                  "It is strongly recommended to implement appropriate security measures\n" \
                                  "to minimize these risks.";
static const char QUESTION[]    = "Enter 'Y' to accept these risks and continue, or any other key to exit: ";
static const char ACCEPTED[]    = "Security risks accepted. Continue with caution!";


/*  -----------  includes  -----------------------------------------------
 */
#if (OPTION_PCBUSB_STANDALONE != 0)
#include "pcan_api.h"
#else
#if defined(__APPLE__)
#include "PCBUSB.h"
#else
#include "PCANBasic.h"
#endif
#endif
#include "bitrates.h"
#include "printmsg.h"
#include "timer.h"

#include "tcp_server.h"
#ifndef __APPLE__
#include <linux/can.h>
#else
#include "include/mac/can.h"
#endif
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <libgen.h>
#include <getopt.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <inttypes.h>
#include <assert.h>


/*  -----------  options  ---------------------------------------------------
 */
#define BLOCKING_READ


/*  -----------  defines  ---------------------------------------------------
 */
#define MAX_ID        2048

#define TIME_ZERO    0
#define TIME_ABS     1
#define TIME_REL     2

#define MODE_HEX     0
#define MODE_DEC     1
#define MODE_OCT     2

#define ASCII_OFF    0
#define ASCII_ON     1
#ifndef BLOCKING_READ
#define RxTIMEOUT    0    /* return immediately */
#else
#define RxTIMEOUT    65535    /* blocking read */
#endif
#define CODE_11BIT   0x000
#define MASK_11BIT   0x7FF
#define CODE_29BIT   0x00000000
#define MASK_29BIT   0x1FFFFFFF


/*  -----------  types  -----------------------------------------------------
 */


/*  -----------  prototypes  ------------------------------------------------
 */
static void sigterm(int signo);
static void usage(FILE *stream, const char *program);
static void version(FILE *stream, const char *program);

static int list_bitrates(BYTE op_mode);
static int list_interfaces(void);
static int test_interfaces(void);

static uint64_t receive(TPCANHandle channel, tcp_server_t server);
static uint64_t receive_fd(TPCANHandle channel, tcp_server_t server);

static int transmit(const void *date, size_t size, void *hint);
static int transmit_fd(const void *date, size_t size, void *hint);

static uint8_t dlc2len(uint8_t dlc);
static uint8_t len2dlc(uint8_t len);


/*  -----------  variables  ----------------------------------------------
 */
static const unsigned char dlc_table[16] = {
    0,1,2,3,4,5,6,7,8,12,16,20,24,32,48,64
};
static volatile int running = 1;

/*  - - - - - -  from CAN API Defs - - - - - - - - - - - - - - - - - - - -
 */
#define PCAN_BOARDS     (8)
static struct _can_board_t
{
  unsigned long type;                  /**< board type */
  char* name;                          /**< board name */
} can_board[PCAN_BOARDS] = {
  {PCAN_USBBUS1,                           "PCAN-USB1"},
  {PCAN_USBBUS2,                           "PCAN-USB2"},
  {PCAN_USBBUS3,                           "PCAN-USB3"},
  {PCAN_USBBUS4,                           "PCAN-USB4"},
  {PCAN_USBBUS5,                           "PCAN-USB5"},
  {PCAN_USBBUS6,                           "PCAN-USB6"},
  {PCAN_USBBUS7,                           "PCAN-USB7"},
  {PCAN_USBBUS8,                           "PCAN-USB8"},
};

/*  -----------  main  ---------------------------------------------------
 */
int main(int argc, char *argv[])
{
    TPCANHandle channel = PCAN_USBBUS1;
    TPCANStatus status;
    UINT64 intarg;
    int   opt, i;
    long  board = -1; int b;
    BYTE  op_mode = PCAN_MESSAGE_STANDARD; int op = 0;
    BYTE  listenonly = PCAN_PARAMETER_OFF; int lo = 0;
    BYTE  allow_sts = PCAN_PARAMETER_ON; int sf = 0;
    BYTE  allow_rtr = PCAN_PARAMETER_ON; int rf = 0;
    DWORD std_code = CODE_11BIT; int sc = 0;
    DWORD std_mask = MASK_11BIT; int sm = 0;
    DWORD xtd_code = CODE_29BIT; int xc = 0;
    DWORD xtd_mask = MASK_29BIT; int xm = 0;
    BYTE  trace = PCAN_PARAMETER_OFF; int ts = 0;
    long  baudrate = PCAN_BAUD_250K; int bd = 0;
    int   logging = 0; int ll = 0;
    int   risks = 0; int sr = 0;
    int   show_version = 0;
    int   verbose = 0;
    int   num_boards = 0;
    BYTE  buffer[256]; /* char *device, *firmware, *software, *library; */
    DWORD device_id = 8472;
    UINT64 filter = 0U;
    /* Default Bit-rate */
    TPCANBitrateFD bitrate = "f_clock_mhz=80,nom_brp=2,nom_tseg1=127,nom_tseg2=32,nom_sjw=32,data_brp=2,data_tseg1=15,data_tseg2=4,data_sjw=4";
    long unsigned freq = 0; struct btr_bit_timing slow, fast;
    /* CAN/IPC server */
    tcp_server_t server = NULL;
    char *port = NULL;

    struct option long_options[] = {
        {"baudrate", required_argument, 0, 'b'},
        {"bitrate", required_argument, 0, 'B'},
        {"verbose", no_argument, 0, 'v'},
        {"mode", required_argument, 0, 'm'},
        {"listen-only", no_argument, 0, 'M'},
        {"no-status-frames", no_argument, 0, 'E'},
        {"no-remote-frames", no_argument, 0, 'R'},
        {"code", required_argument, 0, '1'},
        {"mask", required_argument, 0, '2'},
        {"xtd-code", required_argument, 0, '3'},
        {"xtd-mask", required_argument, 0, '4'},
        {"trace", required_argument, 0, 'Y'},
        {"logging", required_argument, 0, 'G'},
        {"security-risks", required_argument, 0, 's'},
        {"list-bitrates", optional_argument, 0, 'l'},
        {"list-boards", no_argument, 0, 'L'},
        {"test-boards", no_argument, 0, 'T'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, &show_version, 1},
        {0, 0, 0, 0}
    };

    /* signal handler */
    if ((signal(SIGINT, sigterm) == SIG_ERR) ||
#if !defined(_WIN32) && !defined(_WIN64)
       (signal(SIGHUP, sigterm) == SIG_ERR) ||
#endif
       (signal(SIGTERM, sigterm) == SIG_ERR)) {
        perror("+++ error");
        return errno;
    }
    /* scan command-line */
    while ((opt = getopt_long(argc, (char * const *)argv, "b:vm:lLTh", long_options, &i)) != -1) {
        switch (opt) {
        /* option '--baudrate=<baudrate>' (-b) */
        case 'b':
            if (bd++) {
                fprintf(stderr, "%s: duplicated option `--baudrate' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if (sscanf(optarg, "%li", &baudrate) != 1) {
                fprintf(stderr, "%s: illegal argument for option `--baudrate' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            switch (baudrate) {
                case 0: case 1000: case 1000000: baudrate = PCAN_BAUD_1M; break;
                case 1: case 800:  case 800000:  baudrate = PCAN_BAUD_800K; break;
                case 2: case 500:  case 500000:  baudrate = PCAN_BAUD_500K; break;
                case 3: case 250:  case 250000:  baudrate = PCAN_BAUD_250K; break;
                case 4: case 125:  case 125000:  baudrate = PCAN_BAUD_125K; break;
                case 5: case 100:  case 100000:  baudrate = PCAN_BAUD_100K; break;
                case 6: case 50:   case 50000:   baudrate = PCAN_BAUD_50K; break;
                case 7: case 20:   case 20000:   baudrate = PCAN_BAUD_20K; break;
                case 8: case 10:   case 10000:   baudrate = PCAN_BAUD_10K; break;
                default:
                    fprintf(stderr, "%s: illegal argument for option `--baudrate' (%c)\n", basename(argv[0]), opt);
                    return 1;
            }
            bitrate = NULL;
            break;
        /* option '--bitrate=<bit-rate>' as string */
        case 'B':
            if (bd++) {
                fprintf(stderr, "%s: duplicated option `--bitrate'\n", basename(argv[0]));
                return 1;
            }
            if (!btr_string_to_bit_timing(optarg, &freq, &slow, &fast)) {
                fprintf(stderr, "%s: illegal argument for option `--bitrate'\n", basename(argv[0]));
                return 1;
            }
            bitrate = optarg;
            baudrate = 0;
            break;
        /* option '--verbose' (-v) */
        case 'v':
            if (verbose) {
                fprintf(stderr, "%s: duplicated option `--verbose' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            verbose = 1;
            break;
        /* option '--mode=(2.0|FDF[+BRS])' (-m) */
        case 'm':
            if (op++) {
                fprintf(stderr, "%s: duplicated option `--mode' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if (!strcasecmp(optarg, "DEFAULT") || !strcasecmp(optarg, "CLASSIC") || !strcasecmp(optarg, "CLASSICAL") ||
                !strcasecmp(optarg, "CAN2.0") || !strcasecmp(optarg, "CAN20") || !strcasecmp(optarg, "2.0") ||
                !strcasecmp(optarg, "CANCC") || !strcasecmp(optarg, "CCF") || !strcasecmp(optarg, "CC"))
                op_mode = PCAN_MESSAGE_STANDARD;
            else if (!strcasecmp(optarg, "CANFD") || !strcasecmp(optarg, "FDF") || !strcasecmp(optarg, "FD"))
                op_mode = PCAN_MESSAGE_FD;
            else if (!strcasecmp(optarg, "CANFD+BRS") || !strcasecmp(optarg, "FDF+BRS") || !strcasecmp(optarg, "FD+BRS"))
                op_mode = PCAN_MESSAGE_FD | PCAN_MESSAGE_BRS;
            else {
                fprintf(stderr, "%s: illegal argument for option `--mode' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            break;
        /* option '--listen-only' */
        case 'M':
            if (lo++) {
                fprintf(stderr, "%s: duplicated option `--listen-only'\n", basename(argv[0]));
                return 1;
            }
            listenonly = PCAN_PARAMETER_ON;
            break;
        /* option '--no-status-frames' */
        case 'E':
            if (sf++) {
                fprintf(stderr, "%s: duplicated option `--no-status-frames'\n", basename(argv[0]));
                return 1;
            }
            allow_sts = PCAN_PARAMETER_OFF;
            break;
        /* option '--no-remote-frames' */
        case 'R':
            if (rf++) {
                fprintf(stderr, "%s: duplicated option `--no-remote-frames'\n", basename(argv[0]));
                return 1;
            }
            allow_rtr = PCAN_PARAMETER_OFF;
            break;
        /* option '--code=<11-bit-code>' */
        case '1':
            if (sc++) {
                fprintf(stderr, "%s: duplicated option `--code'\n", basename(argv[0]));
                return 1;
            }
            if (sscanf(optarg, "%" SCNx64, &intarg) != 1) {
                fprintf(stderr, "%s: illegal argument for option `--code'\n", basename(argv[0]));
                return 1;
            }
            if ((intarg & ~0x7FF) != 0) {
                fprintf(stderr, "%s: illegal argument for option --code'\n", basename(argv[0]));
                return 1;
            }
            std_code = (DWORD)intarg;
            break;
        /* option '--mask=<11-bit-mask>' */
        case '2':
            if (sm++) {
                fprintf(stderr, "%s: duplicated option `--mask'\n", basename(argv[0]));
                return 1;
            }
            if (sscanf(optarg, "%" SCNx64, &intarg) != 1) {
                fprintf(stderr, "%s: illegal argument for option --mask'\n", basename(argv[0]));
                return 1;
            }
            if ((intarg & ~0x7FF) != 0) {
                fprintf(stderr, "%s: illegal argument for option --mask'\n", basename(argv[0]));
                return 1;
            }
            std_mask = (DWORD)intarg;
            break;
        /* option '--xtd-code=<29-bit-code>' */
        case '3':
            if (xc++) {
                fprintf(stderr, "%s: duplicated option `--xtd-code'\n", basename(argv[0]));
                return 1;
            }
            if (sscanf(optarg, "%" SCNx64, &intarg) != 1) {
                fprintf(stderr, "%s: illegal argument for option --xtd-code'\n", basename(argv[0]));
                return 1;
            }
            if ((intarg & ~0x1FFFFFFF) != 0) {
                fprintf(stderr, "%s: illegal argument for option --xtd-code'\n", basename(argv[0]));
                return 1;
            }
            xtd_code = (DWORD)intarg;
            break;
        /* option '--xtd-mask=<29-bit-mask>' */
        case '4':
            if (xm++) {
                fprintf(stderr, "%s: duplicated option `--xtd-mask'\n", basename(argv[0]));
                return 1;
            }
            if (sscanf(optarg, "%" SCNx64, &intarg) != 1) {
                fprintf(stderr, "%s: illegal argument for option --xtd-mask'\n", basename(argv[0]));
                return 1;
            }
            if ((intarg & ~0x1FFFFFFF) != 0) {
                fprintf(stderr, "%s: illegal argument for option --xtd-mask'\n", basename(argv[0]));
                return 1;
            }
            xtd_mask = (DWORD)intarg;
            break;
        /* option '--trace=(ON|OFF)' */
        case 'Y':
            if (ts++) {
                fprintf(stderr, "%s: duplicated option `--trace'\n", basename(argv[0]));
                return 1;
            }
            if (!strcasecmp(optarg, "OFF") || !strcasecmp(optarg, "NO") || !strcasecmp(optarg, "n") || !strcasecmp(optarg, "0"))
                trace = PCAN_PARAMETER_OFF;
            else if (!strcasecmp(optarg, "ON") || !strcasecmp(optarg, "YES") || !strcasecmp(optarg, "y") || !strcasecmp(optarg, "1"))
                trace = PCAN_PARAMETER_ON;
            else {
                fprintf(stderr, "%s: illegal argument for option `--trace'\n", basename(argv[0]));
                return 1;
            }
            break;
        /* option '--logging=<level> */
        case 'G':
            if (ll++) {
                fprintf(stderr, "%s: duplicated option `--logging'\n", basename(argv[0]));
                return 1;
            }
            if (sscanf(optarg, "%" SCNx64, &intarg) != 1) {
                fprintf(stderr, "%s: illegal argument for option `--logging'\n", basename(argv[0]));
                return 1;
            }
            // if ((intarg < 0) || (3 < intarg)) {
            //     fprintf(stderr, "%s: illegal argument for option --logging'\n", basename(argv[0]));
            //     return 1;
            // }
            logging = (int)intarg;
            break;
        /* option '--security-risks="I ACCEPT"' */
        case 's':
            if (sr++) {
                fprintf(stderr, "%s: duplicated option `--security-risks'\n", basename(argv[0]));
                return 1;
            }
            if (!strcmp(optarg, "I ACCEPT"))
                risks = 1;
            break;
        /* option '--list-bitrates[=(2.0|FDF[+BRS])]' */
        case 'l':
            fprintf(stdout, "%s\n%s\n\n%s\n\n", APPLICATION, COPYRIGHT, WARRANTY);
            /* list bit-rates (depending on operation mode) */
            if (optarg != NULL) {
                if (op++) {
                    fprintf(stderr, "%s: option `--list-bitrates' - operation mode already set'\n", basename(argv[0]));
                    return 1;
                }
                if (!strcasecmp(optarg, "DEFAULT") || !strcasecmp(optarg, "CLASSIC") || !strcasecmp(optarg, "CLASSICAL") ||
                    !strcasecmp(optarg, "CAN2.0") || !strcasecmp(optarg, "CAN20") || !strcasecmp(optarg, "2.0") ||
                    !strcasecmp(optarg, "CANCC") || !strcasecmp(optarg, "CCF") || !strcasecmp(optarg, "CC"))
                    op_mode = PCAN_MESSAGE_STANDARD;
                else if (!strcasecmp(optarg, "CANFD") || !strcasecmp(optarg, "FDF") || !strcasecmp(optarg, "FD"))
                    op_mode = PCAN_MESSAGE_FD;
                else if (!strcasecmp(optarg, "CANFD+BRS") || !strcasecmp(optarg, "FDF+BRS") || !strcasecmp(optarg, "FD+BRS"))
                    op_mode = PCAN_MESSAGE_FD | PCAN_MESSAGE_BRS;
                else {
                    fprintf(stderr, "%s: illegal argument for option `--list-bitrates'\n", basename(argv[0]));
                    return 1;
                }
            }
            (void)list_bitrates(op_mode);
            return 1;
        /* option '--list-boards' (-L) */
        case 'L':
            fprintf(stdout, "%s\n%s\n\n%s\n\n", APPLICATION, COPYRIGHT, WARRANTY);
            /* list all supported interfaces */
            num_boards = list_interfaces();
            fprintf(stdout, "Number of supported CAN interfaces=%i\n", num_boards);
            return (num_boards >= 0) ? 0 : 1;
        /* option '--test-boards' (-T) */
        case 'T':
            fprintf(stdout, "%s\n%s\n\n%s\n\n", APPLICATION, COPYRIGHT, WARRANTY);
            /* list all available interfaces */
            num_boards = test_interfaces();
            fprintf(stdout, "Number of present CAN interfaces=%i\n", num_boards);
            return (num_boards >= 0) ? 0 : 1;
        /* option '--help' (-h) */
        case 'h':
            usage(stdout, basename(argv[0]));
            return 0;
        case '?':
            if (!opterr)
                usage(stderr, basename(argv[0]));
            return 1;
        default:
            if (show_version) {
                version(stdout, basename(argv[0]));
                return 0;
            }
            else {
                usage(stderr, basename(argv[0]));
                return 1;
            }
        }
    }
    /* - check if one and only one <interface>@<port> is given */
    if (optind + 1 != argc) {
        if (optind == argc)
            fprintf(stderr, "%s: no interface given\n", basename(argv[0]));
        else
            fprintf(stderr, "%s: too many arguments given\n", basename(argv[0]));
        return 1;
    }
    /* - now extract <port> from <interface>@<port> */
    port = strchr(argv[optind], '@');
    if (port == NULL) {
        fprintf(stderr, "%s: illegal argument for <interface>@<port> (`%s')\n", basename(argv[0]), argv[optind]);
        return 1;
    }
    *port++ = '\0';
    /* - first search the <interface> by its name in the device list */
    for (b = 0; b < PCAN_BOARDS; b++) {
        if (strcasecmp(argv[optind], can_board[b].name) == 0) {
            board = can_board[b].type;
            break;
        }
    }
    /* if not found, search the <interface> by its channel handle in the device list */
    if (b >= PCAN_BOARDS) {
        if (sscanf(argv[optind], "%li", &board) != 1) {
            fprintf(stderr, "%s: illegal argument\n", basename(argv[0]));
            return 1;
        }
        for (b = 0; b < PCAN_BOARDS; b++) {
            if (board == (long)can_board[b].type)
                break;
        }
        if (b >= PCAN_BOARDS) {
            fprintf(stderr, "%s: illegal argument\n", basename(argv[0]));
            return 1;
        }
    }
    /* - check if a bit-rate string is given for CAN FD mode */
    if ((op_mode & PCAN_MESSAGE_FD) && (bitrate == NULL)) {
        fprintf(stderr, "%s: illegal combination of options `--mode' (m) and `--baudrate' (b)\n", basename(argv[0]));
        return 1;
    }
    /* - check is a BTR0BTR1 value is given for CAN 2.0 mode */
    if (!(op_mode & PCAN_MESSAGE_FD) && (baudrate == 0)) {
        fprintf(stderr, "%s: illegal combination of options `--mode' (m) and `--bitrate'\n", basename(argv[0]));
        return 1;
    }
    /* CAN Monitor for PCAN-USB Interfaces */
    fprintf(stdout, "%s\n%s\n\n%s\n\n", APPLICATION, COPYRIGHT, WARRANTY);
    /* - show operation mode and bit-rate settings */
    if (verbose) {
        fprintf(stdout, "Op.-Mode=%s", (op_mode & PCAN_MESSAGE_FD) ? "CANFD" : "CAN2.0");
        if ((op_mode & PCAN_MESSAGE_BRS)) fprintf(stdout, "+BRS");
        fprintf(stdout, " (op_mode=%02Xh)\n", op_mode);
        if ((op_mode & PCAN_MESSAGE_FD)) {
            (void)btr_string_to_bit_timing(bitrate, &freq, &slow, &fast);
            fprintf(stdout, "Bit-rate=%lukbps@%.1f%%",
                btr_calc_bit_rate_nominal(&slow, freq),
                btr_calc_sample_point_nominal(&slow) * 100.);
            if ((op_mode & PCAN_MESSAGE_BRS))
                fprintf(stdout, ":%lukbps@%.1f%%",
                    btr_calc_bit_rate_data(&fast, freq),
                    btr_calc_sample_point_data(&fast) * 100.);
            fprintf(stdout, " (%s)\n\n", bitrate);
        }
        else {
            fprintf(stdout, "Baudrate=%skbps (BTR0BTR1=%04lx)\n\n",
                    baudrate == PCAN_BAUD_1M ? "1000" :
                    baudrate == PCAN_BAUD_800K ?  "800" :
                    baudrate == PCAN_BAUD_500K ?  "500" :
                    baudrate == PCAN_BAUD_250K ?  "250" :
                    baudrate == PCAN_BAUD_125K ?  "125" :
                    baudrate == PCAN_BAUD_100K ?  "100" :
                    baudrate == PCAN_BAUD_50K  ?   "50" :
                    baudrate == PCAN_BAUD_20K  ?   "20" :
                    baudrate == PCAN_BAUD_10K  ?   "10" : "?", baudrate);
        }
    }
    /* - initialization */
    fprintf(stdout, "Hardware=%s...", can_board[b].name);
    fflush (stdout);
    channel = (TPCANHandle)can_board[b].type;
    if ((status = CAN_SetValue(channel, PCAN_LISTEN_ONLY, (void*)&listenonly, sizeof(listenonly))) != PCAN_ERROR_OK) {
        fprintf(stdout, "FAILED!\n");
        fprintf(stderr, "+++ error: CAN_SetValue PCAN-USB%u (PCAN_LISTEN_ONLY=%u) returned 0x%X\n", (channel - 0x50), listenonly, status);
        return (int)status;;
    }
    if (!(op_mode & PCAN_MESSAGE_FD)) {
        if ((status = CAN_Initialize(channel, (TPCANBaudrate)baudrate, PCAN_USB, 0, 0)) != PCAN_ERROR_OK) {
            fprintf(stdout, "FAILED!\n");
            fprintf(stderr, "+++ error: CAN_Initialize PCAN-USB%u returned 0x%X\n", (channel - 0x50), status);
            return (int)status;;
        }
        fprintf(stdout, "OK!\nBaudrate=%lukbps...",
            btr_calc_bit_rate_sja1000((unsigned short)baudrate) / 1000);
    }
    else {
        if (!btr_string_to_bit_timing(bitrate, &freq, &slow, &fast)) {
            fprintf(stdout, "FAILED!\n");
            fprintf(stderr, "+++ error: CAN_InitializeFD PCAN-USB%u with wrong bit-rate\n", (channel - 0x50));
            return (int)-1;
        }
        if ((status = CAN_InitializeFD(channel, bitrate)) != PCAN_ERROR_OK) {
            fprintf(stdout, "FAILED!\n");
            fprintf(stderr, "+++ error: CAN_InitializeFD PCAN-USB%u returned 0x%X\n", (channel - 0x50), status);
            return (int)status;;
        }
        fprintf(stdout, "OK!\nBit-rate=%lukbps",
            btr_calc_bit_rate_nominal(&slow, freq) / 1000);
        if ((op_mode & PCAN_MESSAGE_BRS))
            fprintf(stdout, ":%lukbps",
                btr_calc_bit_rate_data(&fast, freq) / 1000);
        fprintf(stdout, "...");
    }
    if ((status = CAN_SetValue(channel, PCAN_ALLOW_STATUS_FRAMES, (void*)&allow_sts, sizeof(allow_sts))) != PCAN_ERROR_OK) {
        fprintf(stdout, "FAILED!\n");
        fprintf(stderr, "+++ error: CAN_SetValue PCAN-USB%u (PCAN_ALLOW_STATUS_FRAMES=%u) returned 0x%X\n", (channel - 0x50), allow_sts, status);
        (void)CAN_Uninitialize(channel);
        return (int)status;;
    }
    if ((status = CAN_SetValue(channel, PCAN_ALLOW_RTR_FRAMES, (void*)&allow_rtr, sizeof(allow_rtr))) != PCAN_ERROR_OK) {
        fprintf(stdout, "FAILED!\n");
        fprintf(stderr, "+++ error: CAN_SetValue PCAN-USB%u (PCAN_ALLOW_RTR_FRAMES=%u) returned 0x%X\n", (channel - 0x50), allow_rtr, status);
        (void)CAN_Uninitialize(channel);
        return (int)status;;
    }
    if ((std_code != CODE_11BIT) || (std_mask != MASK_11BIT)) {
        filter = ((UINT64)std_code << 32) | (UINT64)std_mask;
        if ((status = CAN_SetValue(channel, PCAN_ACCEPTANCE_FILTER_11BIT, (void*)&filter, sizeof(filter))) != PCAN_ERROR_OK) {
            fprintf(stdout, "FAILED!\n");
            fprintf(stderr, "+++ error: CAN_SetValue PCAN-USB%u (PCAN_ACCEPTANCE_FILTER_11BIT=%"PRIX64") returned 0x%X\n", (channel - 0x50), filter, status);
            (void)CAN_Uninitialize(channel);
            return (int)status;;
        }
    }
    if ((xtd_code != CODE_29BIT) || (xtd_mask != MASK_29BIT)) {
        filter = ((UINT64)xtd_code << 32) | (UINT64)xtd_mask;
        if ((status = CAN_SetValue(channel, PCAN_ACCEPTANCE_FILTER_29BIT, (void*)&filter, sizeof(filter))) != PCAN_ERROR_OK) {
            fprintf(stdout, "FAILED!\n");
            fprintf(stderr, "+++ error: CAN_SetValue PCAN-USB%u (PCAN_ACCEPTANCE_FILTER_29BIT=%"PRIX64") returned 0x%X\n", (channel - 0x50), filter, status);
            (void)CAN_Uninitialize(channel);
            return (int)status;;
        }
    }
    if (trace != PCAN_PARAMETER_OFF) {
        if (getcwd((char*)buffer, 256) == NULL) {
            fprintf(stdout, "FAILED!\n");
            perror("+++ error");
            (void)CAN_Uninitialize(channel);
            return (int)status;;
        }
        if ((status = CAN_SetValue(channel, PCAN_TRACE_LOCATION, (void*)buffer, 256U)) != PCAN_ERROR_OK) {
            fprintf(stdout, "FAILED!\n");
            fprintf(stderr, "+++ error: CAN_SetValue PCAN-USB%u (PCAN_TRACE_LOCATION=%s) returned 0x%X\n", (channel - 0x50), (char*)buffer, status);
            (void)CAN_Uninitialize(channel);
            return (int)status;;
        }
        buffer[0] = TRACE_FILE_SINGLE | TRACE_FILE_DATE | TRACE_FILE_TIME | TRACE_FILE_OVERWRITE;
        if ((status = CAN_SetValue(channel, PCAN_TRACE_CONFIGURE, (void*)&buffer[0], sizeof(BYTE))) != PCAN_ERROR_OK) {
            fprintf(stdout, "FAILED!\n");
            fprintf(stderr, "+++ error: CAN_SetValue PCAN-USB%u (PCAN_TRACE_CONFIGURE=%X) returned 0x%X\n", (channel - 0x50), buffer[0], status);
            (void)CAN_Uninitialize(channel);
            return (int)status;;
        }
        if ((status = CAN_SetValue(channel, PCAN_TRACE_STATUS, (void*)&trace, sizeof(trace))) != PCAN_ERROR_OK) {
            fprintf(stdout, "FAILED!\n");
            fprintf(stderr, "+++ error: CAN_SetValue PCAN-USB%u (PCAN_TRACE_STATUS=%X) returned 0x%u\n", (channel - 0x50), trace, status);
            (void)CAN_Uninitialize(channel);
            return (int)status;;
        }
    }
    fprintf(stdout, "OK!\n");
    /* - security risks */
    fprintf(stdout, "\n%s\n", SECURITY);
    if (!risks) {
        /* -- accept it or leave */
        fprintf(stdout, "\n%s", QUESTION);
        fflush(stdout);
        if (getchar() != 'Y') {
            (void)CAN_Uninitialize(channel);
            return (-999);
        }
    }
    fprintf(stdout, "\n%s\n\n", ACCEPTED);
    /* - start CAN/IPC server */
    fprintf(stdout, "CAN/IPC server on port %s...", port);
    fflush(stdout);
    if (!(op_mode & PCAN_MESSAGE_FD))
        server = tcp_server_start(port, sizeof(struct can_frame), transmit, (void*)&channel, logging);
    else
        server = tcp_server_start(port, sizeof(struct canfd_frame), transmit_fd, (void*)&channel, logging);
    if (server == NULL) {
        fprintf(stdout, "FAILED!\n");
        fprintf(stderr, "+++ error: CAN/IPC server could not be started\n");
        if (errno) perror("    cause");
        (void)CAN_Uninitialize(channel);
        return errno;
    }
    fprintf(stdout, "\b\b\b started!\n");
    /* - reception loop */
    fprintf(stderr, "\nPress ^C to abort.\n\n");
    if (!(op_mode & PCAN_MESSAGE_FD))
        (void)receive(channel, server);
    else
        (void)receive_fd(channel, server);
    /* - stop CAN/IPC server */
    if (tcp_server_stop(server) < 0) {
        fprintf(stderr, "+++ error: CAN/IPC server could not be stopped\n");
        if (errno) perror("    cause");
        (void)CAN_Uninitialize(channel);
        return errno;
    }
    fprintf(stdout, "CAN/IPC server on port %s stopped\n\n", port);
    /* - teardown */
    if (CAN_GetValue(channel, PCAN_HARDWARE_NAME, buffer, 256) == PCAN_ERROR_OK) {
        fprintf(stdout, "Hardware: %s", buffer);
        if (CAN_GetValue(channel, PCAN_DEVICE_ID, (void*)&device_id, sizeof(DWORD)) == PCAN_ERROR_OK)
            fprintf(stdout, " (DeviceId=0x%02"PRIx32")", device_id);
        fprintf(stdout, "\n");
    }
    if (CAN_GetValue(channel, PCAN_CHANNEL_VERSION, buffer, 256) == PCAN_ERROR_OK) {
        fprintf(stdout, "Firmware: %s\n", buffer);
    }
    if (CAN_GetValue(channel, PCAN_API_VERSION, buffer, 256) == PCAN_ERROR_OK) {
        fprintf(stdout, "Software: API Version %s\n", buffer);
    }
    if ((status = CAN_Uninitialize(channel)) != PCAN_ERROR_OK) {
        fprintf(stderr, "+++ error: CAN_Uninitialize PCAN-USB%u returned 0x%X\n", (channel - 0x50), status);
        return (int)status;;
    }
    fprintf(stdout, "%s\n", COPYRIGHT);
    return 0;
}

/*  -----------  local functions  ----------------------------------------
 */
static int list_interfaces(void)
{
    int i;

    fprintf(stdout, "Supported hardware:\n");
    for (i = 0; i < PCAN_BOARDS; i++) {
        fprintf(stdout, "\"%s\" (VendorName=\"PEAK-System\", LibraryId=400, BoardType=%lxh)\n", can_board[i].name, can_board[i].type);
    }
    return i;
}

static int test_interfaces(void)
{
    TPCANStatus status;
    int i, n = 0;
    BYTE test;
    DWORD device_id;

    for (i = 0; i < PCAN_BOARDS; i++) {
        if ((status = CAN_GetValue(PCAN_USBBUS1+i, PCAN_CHANNEL_CONDITION, &test, sizeof(BYTE))) == PCAN_ERROR_OK) {
            if (test > PCAN_CHANNEL_UNAVAILABLE) {
                fprintf(stdout, "Hardware=%s...", can_board[i].name);
                fflush(stdout);
                timer_delay(TIMER_MSEC(333));
                fprintf(stdout, "%s", (test >= PCAN_CHANNEL_OCCUPIED) ? "occupied" : "available");
                if (CAN_GetValue(PCAN_USBBUS1+i, PCAN_DEVICE_ID, (void*)&device_id, sizeof(DWORD)) == PCAN_ERROR_OK)
                    fprintf(stdout, " (DeviceId=0x%02"PRIx32")", device_id);
                fprintf(stdout, "\n");
                n++;
            }
        }
        else {
            fprintf(stderr, "+++ error: CAN_GetStatus PCAN_CHANNEL_CONDITION of PCAN-USB%i returned 0x%X\n", i+1, status);
            return (-1);
        }
    }
    if (n == 0) {
        fprintf(stdout, "No hardware found!\n");
    }
    return n;
}

static int list_bitrates(BYTE op_mode)
{
    if (op_mode & PCAN_MESSAGE_FD) {
        if (op_mode & PCAN_MESSAGE_BRS) {
            fprintf(stdout, "Bitrates - CAN FD with Bit-rate Switching (BRS):\n");
            fprintf(stdout, "  1000kbps@80.0%%:8000kbps@80.0%%=\"f_clock=80000000,nom_brp=2,nom_tseg1=31,nom_tseg2=8,nom_sjw=8,data_brp=2,data_tseg1=3,data_tseg2=1,data_sjw=1\"\n");
            fprintf(stdout, "   500kbps@80.0%%:4000kbps@80.0%%=\"f_clock=80000000,nom_brp=2,nom_tseg1=63,nom_tseg2=16,nom_sjw=16,data_brp=2,data_tseg1=7,data_tseg2=2,data_sjw=2\"\n");
            fprintf(stdout, "   250kbps@80.0%%:2000kbps@80.0%%=\"f_clock=80000000,nom_brp=2,nom_tseg1=127,nom_tseg2=32,nom_sjw=32,data_brp=2,data_tseg1=15,data_tseg2=4,data_sjw=4\"\n");
            fprintf(stdout, "   125kbps@80.0%%:1000kbps@80.0%%=\"f_clock=80000000,nom_brp=2,nom_tseg1=255,nom_tseg2=64,nom_sjw=64,data_brp=2,data_tseg1=31,data_tseg2=8,data_sjw=8\"\n");
            return 4;
        }
        else {
            fprintf(stdout, "Bitrates - CAN FD without Bit-rate Switching (BRS):\n");
            fprintf(stdout, "  1000kbps@80.0%%=\"f_clock=80000000,nom_brp=2,nom_tseg1=31,nom_tseg2=8,nom_sjw=8\"\n");
            fprintf(stdout, "   500kbps@80.0%%=\"f_clock=80000000,nom_brp=2,nom_tseg1=63,nom_tseg2=16,nom_sjw=16\"\n");
            fprintf(stdout, "   250kbps@80.0%%=\"f_clock=80000000,nom_brp=2,nom_tseg1=127,nom_tseg2=32,nom_sjw=32\"\n");
            fprintf(stdout, "   125kbps@80.0%%=\"f_clock=80000000,nom_brp=2,nom_tseg1=255,nom_tseg2=64,nom_sjw=64\"\n");
            return 4;
        }
    }
    else {
        fprintf(stdout, "Bitrates - CAN Classic (SJA1000 bit-timing register):\n");
        fprintf(stdout, "  1000kbps@75.0%%=\"BTR0BTR1=0x%04x\"\n", PCAN_BAUD_1M);
        fprintf(stdout, "   800kbps@80.0%%=\"BTR0BTR1=0x%04x\"\n", PCAN_BAUD_800K);
        fprintf(stdout, "   500kbps@87.5%%=\"BTR0BTR1=0x%04x\"\n", PCAN_BAUD_500K);
        fprintf(stdout, "   250kbps@87.5%%=\"BTR0BTR1=0x%04x\"\n", PCAN_BAUD_250K);
        fprintf(stdout, "   125kbps@87.5%%=\"BTR0BTR1=0x%04x\"\n", PCAN_BAUD_125K);
        fprintf(stdout, "   100kbps@85.0%%=\"BTR0BTR1=0x%04x\"\n", PCAN_BAUD_100K);
        fprintf(stdout, "    50kbps@85.0%%=\"BTR0BTR1=0x%04x\"\n", PCAN_BAUD_50K);
        fprintf(stdout, "    20kbps@85.0%%=\"BTR0BTR1=0x%04x\"\n", PCAN_BAUD_20K);
        fprintf(stdout, "    10kbps@85.0%%=\"BTR0BTR1=0x%04x\"\n", PCAN_BAUD_10K);
        fprintf(stdout, "     5kbps@68.0%%=\"BTR0BTR1=0x%04x\"\n", PCAN_BAUD_5K);
        return 10;
    }
}

static uint64_t receive(TPCANHandle channel, tcp_server_t server)
{
    TPCANStatus status;
    TPCANMsg message;
    TPCANTimestamp timestamp;

    struct can_frame packet;
    uint64_t frames = 0;
    assert(server);

#ifdef BLOCKING_READ
    int fdes = -1;

    if ((status = CAN_GetValue(channel, PCAN_RECEIVE_EVENT, &fdes, sizeof(int))) != PCAN_ERROR_OK) {
        fprintf(stderr, "+++ error: CAN_GetValue PCAN_RECEIVE_EVENT returned 0x%X\n", status);
        return 0;
    }
    fd_set rdfs;
    FD_ZERO(&rdfs);
    FD_SET(fdes, &rdfs);
#endif
    while (running) {
        if ((status = CAN_Read(channel, &message, &timestamp)) == PCAN_ERROR_OK) {
            memset(&packet, 0, sizeof(packet));
            // map PCAN message to SocketCAN frame
            packet.can_id = (__u32)message.ID;
            packet.can_id |= (message.MSGTYPE & PCAN_MESSAGE_EXTENDED) ? CAN_EFF_FLAG : 0U;
            packet.can_id |= (message.MSGTYPE & PCAN_MESSAGE_RTR) ? CAN_RTR_FLAG : 0U;
            packet.can_id |= (message.MSGTYPE & PCAN_MESSAGE_STATUS) ? CAN_ERR_FLAG : 0U;
            packet.len = (message.LEN < CAN_MAX_DLEN) ? message.LEN : CAN_MAX_DLEN;
            memcpy(packet.data, message.DATA, CAN_MAX_DLEN);
            // convert CAN ID to network byte order
            packet.can_id = htonl(packet.can_id);
            // sent it to all connected clients
            (void)tcp_server_send(server, (const void*)&packet, sizeof(packet));
        }
        else if (status == PCAN_ERROR_QRCVEMPTY)
#ifdef BLOCKING_READ
            select(fdes+1, &rdfs, NULL, NULL, NULL);
#else
            timer_delay(1);
#endif
    }
    fprintf(stdout, "\n");
    return frames;
}

static uint64_t receive_fd(TPCANHandle channel, tcp_server_t server)
{
    TPCANStatus status;
    TPCANMsgFD message;
    TPCANTimestampFD timestamp;
    
    struct canfd_frame packet;
    uint64_t frames = 0;
    assert(server);

#ifdef BLOCKING_READ
    int fdes = -1;

    if ((status = CAN_GetValue(channel, PCAN_RECEIVE_EVENT, &fdes, sizeof(int))) != PCAN_ERROR_OK) {
        fprintf(stderr, "+++ error: CAN_GetValue PCAN_RECEIVE_EVENT returned 0x%X\n", status);
        return 0;
    }
    fd_set rdfs;
    FD_ZERO(&rdfs);
    FD_SET(fdes, &rdfs);
#endif
    while (running) {
        if ((status = CAN_ReadFD(channel, &message, &timestamp)) == PCAN_ERROR_OK) {
            memset(&packet, 0, sizeof(packet));
            // map PCAN FD message to SocketCAN FD frame
            packet.can_id = (__u32)message.ID;
            packet.can_id |= (message.MSGTYPE & PCAN_MESSAGE_EXTENDED) ? CAN_EFF_FLAG : 0U;
            packet.can_id |= (message.MSGTYPE & PCAN_MESSAGE_RTR) ? CAN_RTR_FLAG : 0U;
            packet.can_id |= (message.MSGTYPE & PCAN_MESSAGE_STATUS) ? CAN_ERR_FLAG : 0U;
            packet.flags |= (message.MSGTYPE & PCAN_MESSAGE_FD) ? CANFD_FDF : 0U;
            packet.flags |= (message.MSGTYPE & PCAN_MESSAGE_BRS) ? CANFD_BRS : 0U;
            packet.flags |= (message.MSGTYPE & PCAN_MESSAGE_ESI) ? CANFD_ESI : 0U;
            packet.len = dlc2len(message.DLC);
            memcpy(packet.data, message.DATA, CANFD_MAX_DLEN);
            // convert CAN ID to network byte order
            packet.can_id = htonl(packet.can_id);
            // send it to all connected clients
            (void)tcp_server_send(server, (const void*)&packet, sizeof(packet));
        }
        else if (status == PCAN_ERROR_QRCVEMPTY)
#ifdef BLOCKING_READ
            select(fdes+1, &rdfs, NULL, NULL, NULL);
#else
            timer_delay(1);
#endif
    }
    fprintf(stdout, "\n");
    return frames;
}

static int transmit(const void *data, size_t size, void *hint)
{
    struct can_frame *packet = (struct can_frame*)data;
    TPCANHandle channel = *(TPCANHandle*)hint;
    TPCANStatus status;
    TPCANMsg message;

    if ((data == NULL) || (hint == NULL))
        return -1;
    if (size != sizeof(struct can_frame))
        return -2;
    // convert CAN ID to host byte order
    packet->can_id = ntohl(packet->can_id);
    // map Socket CAN frame to PCAN message
    memset(&message, 0, sizeof(message));
    message.ID = packet->can_id & CAN_EFF_MASK;
    message.MSGTYPE |= (packet->can_id & CAN_EFF_FLAG) ? PCAN_MESSAGE_EXTENDED : PCAN_MESSAGE_STANDARD;
    message.MSGTYPE |= (packet->can_id & CAN_RTR_FLAG) ? PCAN_MESSAGE_RTR : 0U;
    message.MSGTYPE |= (packet->can_id & CAN_ERR_FLAG) ? PCAN_MESSAGE_ERRFRAME : 0U;
    message.LEN = (packet->len < CAN_MAX_DLEN) ? packet->len : CAN_MAX_DLEN;
    memcpy(message.DATA, packet->data, CAN_MAX_DLEN);
    // send it on the CAN bus (retry if busy (w/ timeout))
    timer_restart(TIMER_GPT0, TIMER_MSEC(55U));
    do {
        status = CAN_Write(channel, &message);
    } while ((status & (PCAN_ERROR_QXMTFULL | PCAN_ERROR_XMTFULL)) && !timer_timeout(TIMER_GPT0));
    // return error code
    return (int)status;
}

static int transmit_fd(const void *data, size_t size, void *hint)
{
    struct canfd_frame *packet = (struct canfd_frame*)data;
    TPCANHandle channel = *(TPCANHandle*)hint;
    TPCANStatus status;
    TPCANMsgFD message;

    if ((data == NULL) || (hint == NULL))
        return -1;
    if (size != sizeof(struct canfd_frame))
        return -2;
    // convert CAN ID to host byte order
    packet->can_id = ntohl(packet->can_id);
    // map Socket CAN FD frame to PCAN FD message
    memset(&message, 0, sizeof(message));
    message.ID = packet->can_id & CAN_EFF_MASK;
    message.MSGTYPE |= (packet->can_id & CAN_EFF_FLAG) ? PCAN_MESSAGE_EXTENDED : PCAN_MESSAGE_STANDARD;
    message.MSGTYPE |= (packet->can_id & CAN_RTR_FLAG) ? PCAN_MESSAGE_RTR : 0U;
    message.MSGTYPE |= (packet->can_id & CAN_ERR_FLAG) ? PCAN_MESSAGE_ERRFRAME : 0U;
    message.MSGTYPE |= (packet->flags & CANFD_FDF) ? PCAN_MESSAGE_FD : 0U;
    message.MSGTYPE |= (packet->flags & CANFD_BRS) ? PCAN_MESSAGE_BRS : 0U;
    message.MSGTYPE |= (packet->flags & CANFD_ESI) ? PCAN_MESSAGE_ESI : 0U;
    message.DLC = len2dlc(packet->len);
    memcpy(message.DATA, packet->data, CANFD_MAX_DLEN);
    // send it on the CAN bus (retry if busy (w/ timeout))
    timer_restart(TIMER_GPT0, TIMER_MSEC(55U));
    do {
        status = CAN_WriteFD(channel, &message);
    } while ((status & (PCAN_ERROR_QXMTFULL | PCAN_ERROR_XMTFULL)) && !timer_timeout(TIMER_GPT0));
    // return error code
    return (int)status;
}

static uint8_t dlc2len(uint8_t dlc)
{
    return dlc_table[(dlc < 16U) ? dlc : 15U];
}

static uint8_t len2dlc(uint8_t dlen)
{
    if(dlen > 48U) return 0x0FU;
    if(dlen > 32U) return 0x0EU;
    if(dlen > 24U) return 0x0DU;
    if(dlen > 20U) return 0x0CU;
    if(dlen > 16U) return 0x0BU;
    if(dlen > 12U) return 0x0AU;
    if(dlen > 8U) return 0x09U;
    return dlen;
}

static void sigterm(int signo)
{
    //fprintf(stderr, "%s: got signal %d\n", __FILE__, signo);
    running = 0;
    (void)signo;
}

static void usage(FILE *stream, const char *program)
{
    fprintf(stdout, "%s\n%s\n\n%s\n\n", APPLICATION, COPYRIGHT, WARRANTY);
    fprintf(stream, "Usage: %s <interface>@<port> [<option>...]\n", program);
    fprintf(stream, "Options:\n");
    fprintf(stream, " -m, --mode=(CCF|FDF[+BRS])       CAN operation mode: CAN CC or CAN FD\n");
    fprintf(stream, "     --listen-only                monitor mode (listen-only mode)\n");
    fprintf(stream, "     --no-status-frames           suppress reception of status frames\n");
    fprintf(stream, "     --no-remote-frames           suppress reception of remote frames\n");
    fprintf(stream, "     --code=<id>                  acceptance code for 11-bit IDs (default=0x%03X)\n", CODE_11BIT);
    fprintf(stream, "     --mask=<id>                  acceptance mask for 11-bit IDs (default=0x%03X)\n", MASK_11BIT);
    fprintf(stream, "     --xtd-code=<id>              acceptance code for 29-bit IDs (default=0x%08X)\n", CODE_29BIT);
    fprintf(stream, "     --xtd-mask=<id>              acceptance mask for 29-bit IDs (default=0x%08X)\n", MASK_29BIT);
    fprintf(stream, " -b, --baudrate=<baudrate>        CAN bit-timing in kbps (default=250)\n");
    fprintf(stream, "     --bitrate=<bit-rate>         CAN bit-rate settings (as key/value list)\n");
    fprintf(stream, " -v, --verbose                    show detailed bit-rate settings\n");
    fprintf(stream, "     --trace=(ON|OFF)             write a trace file (default=OFF)\n");
    fprintf(stream, "     --logging=<level>            set CAN/IPC logging level (default=0)\n");
    fprintf(stream, "     --security-risks=\"I ACCEPT\"  accept security risks (skip interactive input)\n");
    fprintf(stream, "     --list-bitrates[=<mode>]     list standard bit-rate settings and exit\n");
    fprintf(stream, " -L, --list-boards                list all supported CAN interfaces and exit\n");
    fprintf(stream, " -T, --test-boards                list all available CAN interfaces and exit\n");
    fprintf(stream, " -h, --help                       display this help screen and exit\n");
    fprintf(stream, "     --version                    show version information and exit\n");
    fprintf(stream, "Hazard note:\n");
    fprintf(stream, "  If you connect your CAN device to a real CAN network when using this program,\n");
    fprintf(stream, "  you might damage your application.\n");
}

static void version(FILE *stream, const char *program)
{
    fprintf(stdout, "%s\n%s\n\n%s\n\n", APPLICATION, COPYRIGHT, LICENSE);
    (void)program;
    fprintf(stream, "Written by Uwe Vogt, UV Software, Berlin <https://www.mac-can.com/>\n");
}

/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
