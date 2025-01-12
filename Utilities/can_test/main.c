/*  -- $HeadURL: https://uv-software.net/MacCAN/PCANUSB/Utilities/can_test/trunk/main.c $ --
 *
 *  project   :  CAN - Controller Area Network
 *
 *  purpose   :  CAN Tester goes macOS aka OS X
 *
 *  copyright :  (c) 2005-2012 by UV Software, Friedrichshafen
 *               (c) 2013-2025 by UV Software, Berlin
 *
 *  revision  :  $Rev: 2065 $ of $Date: 2024-12-30 16:48:19 +0100 (Mo, 30 Dez 2024) $
 * 
 *  author(s) :  Uwe Vogt, UV Software
 *
 *  e-mail    :  uwe.vogt@uv-software.de
 *
 *
 *  -----------  description  --------------------------------------------
 *
 *  CAN Tester (based on macOS Library for PCAN USB Interfaces)
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
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *  Note:  The libPCBUSB is licensed under a freeware license without any
 *  warranty or support.  The libPCBUSB is not part of this program.
 *  It can be downloaded from <https://www.mac-can.com/>.
 *
 */
/*  -----------  version  ------------------------------------------------
 */
#include "build_no.h"
#if (OPTION_PCBUSB_STANDALONE != 0)
#define VERSION_MAJOR     0
#define VERSION_MINOR     6
#define VERSION_PATCH     1
#else
#define VERSION_MAJOR     1
#define VERSION_MINOR     0
#define VERSION_PATCH     99
#endif
#define VERSION_BUILD     BUILD_NO
#define VERSION_STRING    TOSTRING(VERSION_MAJOR)"." TOSTRING(VERSION_MINOR) "." TOSTRING(VERSION_PATCH) " (" TOSTRING(BUILD_NO) ")"
#if defined(__APPLE__)
#define PLATFORM    "macOS"
#elif defined(__linux__)
#define PLATFORM    "Linux"
#else
#error Unsupported platform
#endif
#if defined(__APPLE__)
static const char APPLICATION[] = "CAN Tester for PEAK-System PCAN USB Interfaces, Version "VERSION_STRING;
#else
static const char APPLICATION[] = "CAN Tester for PEAK-System PCAN Interfaces, Version "VERSION_STRING;
#endif
static const char COPYRIGHT[]   = "Copyright (c) 2007,2012-2025 by Uwe Vogt, UV Software, Berlin";
#if (OPTION_PCBUSB_STANDALONE != 0)
static const char WARRANTY[]    = "This program is freeware without any warranty or support!";
static const char LICENSE[]     = "This program is freeware without any warranty or support!\n\n" \
                                  "Note: This program does not require the libPCBUSB to be installed.";
#else
static const char WARRANTY[]    = "CAN Tester comes with ABSOLUTELY NO WARRANTY; for details type `--version'.\n\n" \
                                  "This is free software, and you are welcome to redistribute it\n" \
                                  "under certain conditions; type `--version' for details.";
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
#include "timer.h"

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

#include <inttypes.h>


/*  -----------  options  ---------------------------------------------------
 */
#define BLOCKING_READ


/*  -----------  defines  ---------------------------------------------------
 */
#define RxMODE    (0)
#define TxMODE    (1)
#define TxFRAMES  (2)
#define TxRANDOM  (3)


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

static uint64_t tx_random(TPCANHandle channel, BYTE mode, uint32_t can_id, uint8_t dlc, uint32_t delay, uint64_t number, uint64_t offset);
static uint64_t tx_frames(TPCANHandle channel, BYTE mode, uint32_t can_id, uint8_t dlc, uint32_t delay, uint64_t number, uint64_t offset);
static uint64_t tx_test(TPCANHandle channel, BYTE mode, uint32_t can_id, uint8_t dlc, uint32_t delay, time_t duration, uint64_t offset);
static uint64_t rx_test(TPCANHandle channel, int check, uint64_t offset, int stop_on_error);

static uint64_t tx_random_fd(TPCANHandle channel, BYTE mode, uint32_t can_id, uint8_t dlc, uint32_t delay, uint64_t number, uint64_t offset);
static uint64_t tx_frames_fd(TPCANHandle channel, BYTE mode, uint32_t can_id, uint8_t dlc, uint32_t delay, uint64_t number, uint64_t offset);
static uint64_t tx_test_fd(TPCANHandle channel, BYTE mode, uint32_t can_id, uint8_t dlc, uint32_t delay, time_t duration, uint64_t offset);
static uint64_t rx_test_fd(TPCANHandle channel, int check, uint64_t offset, int stop_on_error);


/*  -----------  variables  ----------------------------------------------
 */
static char *prompt[4] = {"-\b", "/\b", "|\b", "\\\b"};
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

    int opt;
    int mode = RxMODE, m = 0;
    time_t txtime = 0; long txframes = 0;
    long  board = -1; int b;
    BYTE  op_mode = PCAN_MESSAGE_STANDARD; int op = 0;
    BYTE  listenonly = PCAN_PARAMETER_OFF; int lo = 0;
    BYTE  allow_sts = PCAN_PARAMETER_ON; int sf = 0;
    BYTE  allow_rtr = PCAN_PARAMETER_ON; int rf = 0;
    BYTE  trace = PCAN_PARAMETER_OFF; int ts = 0;
    long  baudrate = PCAN_BAUD_250K; int bd = 0;
    long  delay = 0; int t = 0;
    long  data = 8; int d = 0;
    long  can_id = 0x100; int c = 0;
    long  number = 0; int n = 0;
    int   stop_on_error = 0;
    int   show_version = 0;
    int   verbose = 0;
    int   num_boards = 0;
    BYTE  buffer[256]; /* char *device, *firmware, *software, *library; */
    DWORD device_id = 8472;
    /* Default Bit-rate */
    TPCANBitrateFD bitrate = "f_clock_mhz=80,nom_brp=2,nom_tseg1=127,nom_tseg2=32,nom_sjw=32,data_brp=2,data_tseg1=15,data_tseg2=4,data_sjw=4";
    long unsigned freq = 0; struct btr_bit_timing slow, fast;

    struct option long_options[] = {
        {"baudrate", required_argument, 0, 'b'},
        {"bitrate", required_argument, 0, 'B'},
        {"verbose", no_argument, 0, 'v'},
        {"mode", required_argument, 0, 'm'},
        {"shared", no_argument, 0, 'S'},
        {"listen-only", no_argument, 0, 'M'},
        {"no-status-frames", no_argument, 0, 'S'},
        {"no-remote-frames", no_argument, 0, 'R'},
        // {"code", required_argument, 0, '1'},
        // {"mask", required_argument, 0, '2'},
        // {"xtd-code", required_argument, 0, '3'},
        // {"xtd-mask", required_argument, 0, '4'},
        {"receive", no_argument, 0, 'r'},
        {"number", required_argument, 0, 'n'},
        {"stop", no_argument, 0, 's'},
        {"transmit", required_argument, 0, 't'},
        {"frames", required_argument, 0, 'f'},
        {"random", required_argument, 0, 'F'},
        {"cycle", required_argument, 0, 'c'},
        {"usec", required_argument, 0, 'u'},
        {"data", required_argument, 0, 'd'},
        {"dlc", required_argument, 0, 'd'},
        {"id", required_argument, 0, 'i'},
        {"trace", required_argument, 0, 'y'},
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
    while ((opt = getopt_long(argc, (char * const *)argv, "b:vm:rn:st:f:R:c:u:d:i:y:laLTvh", long_options, NULL)) != -1) {
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
        /* option '--mode=(2.0|FDF[+BRS])' (-m)*/
        case 'm':
            if (op++) {
                fprintf(stderr, "%s: duplicated option `--mode' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if (!strcasecmp(optarg, "default") || !strcasecmp(optarg, "classic") ||
                !strcasecmp(optarg, "CAN2.0") || !strcasecmp(optarg, "CAN20") || !strcasecmp(optarg, "2.0"))
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
        case 'S':
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
        case 'r':  /* option '--receive' (-r) */
            if (m++) {
                fprintf(stderr, "%s: duplicated option `--receive' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            mode = RxMODE;
            break;
        case 'n':  /* option '--number=<offset>' (-n) */
            if (n++) {
                fprintf(stderr, "%s: duplicated option `--number' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if (sscanf(optarg, "%li", &number) != 1) {
                fprintf(stderr, "%s: illegal argument for option `--number' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if (number < 0) {
                fprintf(stderr, "%s: illegal argument for option `--number' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            break;
        case 's':  /* option '--stop' (-s) */
            if (stop_on_error) {
                fprintf(stderr, "%s: duplicated option `--stop' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            stop_on_error = 1;
            break;
        case 't':  /* option '--transmit=<duration>' (-t) in [s] */
            if (m++) {
                fprintf(stderr, "%s: duplicated option `--transmit' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if (sscanf(optarg, "%li", &txtime) != 1) {
                fprintf(stderr, "%s: illegal argument for option `--transmit' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if (txtime < 0) {
                fprintf(stderr, "%s: illegal argument for option `--transmit' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            mode = TxMODE;
            break;
        case 'f':  /* option '--frames=<frames>' (-f) */
            if (m++) {
                fprintf(stderr, "%s: duplicated option `--frames' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if (sscanf(optarg, "%li", &txframes) != 1) {
                fprintf(stderr, "%s: illegal argument for option `--frames' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if (txframes < 0) {
                fprintf(stderr, "%s: illegal argument for option `--frames' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            mode = TxFRAMES;
            break;
        case 'F':  /* option '--random=<frames>' */
            if (m++) {
                fprintf(stderr, "%s: duplicated option `--random'\n", basename(argv[0]));
                return 1;
            }
            if (sscanf(optarg, "%li", &txframes) != 1) {
                fprintf(stderr, "%s: illegal argument for option `--random'\n", basename(argv[0]));
                return 1;
            }
            if (txframes < 0) {
                fprintf(stderr, "%s: illegal argument for option `--random' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if (!d) /* let the tester generate messages of arbitrary length */
                data = 0;
            mode = TxRANDOM;
            break;
        case 'c':  /* option '--cycle=<msec>' (-c) */
            if (t++) {
                fprintf(stderr, "%s: duplicated option `--cycle' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if (sscanf(optarg, "%li", &delay) != 1) {
                fprintf(stderr, "%s: illegal argument for option `--cycle' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if ((delay < 0) || (delay > (LONG_MAX / 1000l))) {
                fprintf(stderr, "%s: illegal argument for option `--cycle' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            delay *= 1000l;
            break;
        case 'u':  /* option '--usec=<usec>' (-u) */
            if (t++) {
                fprintf(stderr, "%s: duplicated option `--usec' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if (sscanf(optarg, "%li", &delay) != 1) {
                fprintf(stderr, "%s: illegal argument for option `--usec' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if (delay < 0) {
                fprintf(stderr, "%s: illegal argument for option `--usec' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            break;
        case 'd':  /* option '--dlc=<length>' (-d) */
            if (d++) {
                fprintf(stderr, "%s: duplicated option `--dlc' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if (sscanf(optarg, "%li", &data) != 1) {
                fprintf(stderr, "%s: illegal argument for option `--dlc' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if ((data < 0) || (64 < data)) {
                fprintf(stderr, "%s: illegal argument for option `--dlc' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            break;
        case 'i':  /* option '--id=<identifier>' (-i) */
            if (c++) {
                fprintf(stderr, "%s: duplicated option `--id' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if (sscanf(optarg, "%li", &can_id) != 1) {
                fprintf(stderr, "%s: illegal argument for option `--id' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            if ((can_id < 0x000) || (0x1FFFFFFF < can_id)) { // TODO: to be checked with --mode=NXTD
                fprintf(stderr, "%s: illegal argument for option `--id' (%c)\n", basename(argv[0]), opt);
                return 1;
            }
            break;
        /* option '--trace=(ON|OFF)' (-y) */
        case 'y':
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
        /* option '--list-bitrates' */
        case 'l':
            fprintf(stdout, "%s\n%s\n\n%s\n\n", APPLICATION, COPYRIGHT, WARRANTY);
            /* list bit-rates (depending on operation mode) */
            if (optarg != NULL) {
                if (op++) {
                    fprintf(stderr, "%s: option `--list-bitrates' - operation mode already set'\n", basename(argv[0]));
                    return 1;
                }
                if (!strcasecmp(optarg, "default") || !strcasecmp(optarg, "classic") ||
                    !strcasecmp(optarg, "CAN2.0") || !strcasecmp(optarg, "CAN20") || !strcasecmp(optarg, "2.0"))
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
        case 'a':  /* option '--list-boards' (-a, deprecated) */
        case 'L':  /* option '--list-boards' (-L) */
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
    /* - check if one and only one <interface> is given */
    if (optind + 1 != argc) {
        if (optind == argc)
            fprintf(stderr, "%s: no interface given\n", basename(argv[0]));
        else
            fprintf(stderr, "%s: too many arguments given\n", basename(argv[0]));
        return 1;
    }
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
    /* - check if DLC is less or equal to 8 for CAN 2.0 mode */
    if (!(op_mode & PCAN_MESSAGE_FD) && (data > 8)) {
        fprintf(stderr, "%s: illegal combination of options `--mode' (m) and `--dlc' (d)\n", basename(argv[0]));
        return 1;
    }
    else {
        if (data > 48) data = 0xF;
        else if (data > 32) data = 0xE;
        else if (data > 24) data = 0xD;
        else if (data > 20) data = 0xC;
        else if (data > 16) data = 0xB;
        else if (data > 12) data = 0xA;
        else if (data > 8) data = 0x9;
    }
    /* - check if monitor mode is not set for transmitter tests (we want to say something) */
    if ((mode != RxMODE) && (listenonly)) {
        fprintf(stderr, "%s: illegal option `--listen-only' for transmitter test\n", basename(argv[0]));
        return 1;
    }
    /* CAN Tester for PCAN-USB interfaces */
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
    /* - do your job well: */
    switch (mode) {
    case TxMODE:    /* transmitter test (duration) */
        if (!(op_mode & PCAN_MESSAGE_FD))
            tx_test(channel, op_mode, (uint32_t)can_id, (uint8_t)data, (uint32_t)delay, (time_t)txtime, (uint64_t)number);
        else
            tx_test_fd(channel, op_mode, (uint32_t)can_id, (uint8_t)data, (uint32_t)delay, (time_t)txtime, (uint64_t)number);
        break;
    case TxFRAMES:  /* transmitter test (frames) */
        if (!(op_mode & PCAN_MESSAGE_FD))
            tx_frames(channel, op_mode, (uint32_t)can_id, (uint8_t)data, (uint32_t)delay, (uint64_t)txframes, (uint64_t)number);
        else
            tx_frames_fd(channel, op_mode, (uint32_t)can_id, (uint8_t)data, (uint32_t)delay, (uint64_t)txframes, (uint64_t)number);
        break;
    case TxRANDOM:  /* transmitter test (random) */
        if (!(op_mode & PCAN_MESSAGE_FD))
            tx_random(channel, op_mode, (uint32_t)can_id, (uint8_t)data, (uint32_t)delay, (uint64_t)txframes, (uint64_t)number);
        else
            tx_random_fd(channel, op_mode, (uint32_t)can_id, (uint8_t)data, (uint32_t)delay, (uint64_t)txframes, (uint64_t)number);
        break;
    default:        /* receiver test (abort with Ctrl+C) */
        if (!(op_mode & PCAN_MESSAGE_FD))
            rx_test(channel, n, (uint64_t)number, stop_on_error);
        else
            rx_test_fd(channel, n, (uint64_t)number, stop_on_error);
        break;
    }
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

static uint64_t tx_random(TPCANHandle channel, BYTE mode, uint32_t can_id, uint8_t dlc, uint32_t delay, uint64_t number, uint64_t offset)
{
    time_t start = time(NULL);

    TPCANStatus status;
    TPCANMsg message;

    uint64_t frames = 0;
    uint64_t errors = 0;
    uint64_t calls = 0;

    uint8_t  random_dlc = dlc;
    uint32_t random_delay = delay;
    const struct {
        uint32_t delay;
        int counter;
    } random_mess[13] = {
        { TIMER_USEC(100), (385 * 10) },
        { TIMER_USEC(250), (238 * 10) },
        { TIMER_USEC(500), (147 * 10) },
        { TIMER_USEC(714),    (91 * 10) },
        { TIMER_USEC(1000), (56 * 10) },
        { TIMER_USEC(1667), (35 * 10) },
        { TIMER_USEC(2500), (21 * 10) },
        { TIMER_USEC(5000), (13 * 10) },
        { TIMER_USEC(7143),     (8 * 10) },
        { TIMER_USEC(10000), (5 * 10) },
        { TIMER_USEC(16667), (3 * 10) },
        { TIMER_USEC(25000), (2 * 10) },
        { TIMER_USEC(50000), (1 * 10) }
    };
    int index = 6, counter = 13;
    srand((unsigned int)time(NULL));

    fprintf(stderr, "\nPress ^C to abort.\n");
    message.ID  = (DWORD)can_id;
    message.LEN = (BYTE)dlc;
    message.MSGTYPE = (TPCANMessageType)mode;
    fprintf(stdout, "\nTransmitting message(s)...");
    fflush (stdout);
    while (frames < number) {
        message.DATA[0] = (BYTE)((frames + offset) >> 0);
        message.DATA[1] = (BYTE)((frames + offset) >> 8);
        message.DATA[2] = (BYTE)((frames + offset) >> 16);
        message.DATA[3] = (BYTE)((frames + offset) >> 24);
        message.DATA[4] = (BYTE)((frames + offset) >> 32);
        message.DATA[5] = (BYTE)((frames + offset) >> 40);
        message.DATA[6] = (BYTE)((frames + offset) >> 48);
        message.DATA[7] = (BYTE)((frames + offset) >> 56);
        //memset(&message.DATA[8], 0, 8 - 8);
        message.LEN = (BYTE)(random_dlc < dlc) ? dlc : random_dlc;
        /* transmit message (repeat when busy) */
retry_tx_random:
        calls++;
        if ((status = CAN_Write(channel, &message)) == PCAN_ERROR_OK)
            fprintf(stderr, "%s", prompt[(frames++ % 4)]);
        else if ((status == PCAN_ERROR_QXMTFULL) && running)
            goto retry_tx_random;
        else
            errors++;
        /* pause between two messages, as you please */
        if (random_delay < TIMER_MSEC(10)) {
            timer_restart(TIMER_GPT0, random_delay);
            while (!timer_timeout(TIMER_GPT0)) {
                if (!running) {
                    fprintf(stderr, "\b");
                    fprintf(stdout, "STOP!\n\n");
                    fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
                    fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
                    fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
                    fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);
                    return frames;
                }
            }
        }
        else /* suspend our thread! */
            timer_delay(random_delay);
        if (!running) {
            fprintf(stderr, "\b");
            fprintf(stdout, "STOP!\n\n");
            fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
            fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
            fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
            fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);
            return frames;
        }
        /* ramdom delay and length */
        if (!counter--) {
            index = rand() % 13; // index {0..12}
            counter = random_mess[index].counter;
        }
        random_delay = (random_mess[index].delay < delay) ? delay : random_mess[index].delay;
        random_dlc = (BYTE)(rand() % (8 + 1));
    }
    fprintf(stderr, "\b");
    fprintf(stdout, "OK!\n\n");
    fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
    fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
    fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
    fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);

    timer_delay(TIMER_SEC(1));    // afterburner
    return frames;
}

static uint64_t tx_frames(TPCANHandle channel, BYTE mode, uint32_t can_id, uint8_t dlc, uint32_t delay, uint64_t number, uint64_t offset)
{
    time_t start = time(NULL);

    TPCANStatus status;
    TPCANMsg message;

    uint64_t frames = 0;
    uint64_t errors = 0;
    uint64_t calls = 0;

    fprintf(stderr, "\nPress ^C to abort.\n");
    message.ID  = (DWORD)can_id;
    message.LEN = (BYTE)dlc;
    message.MSGTYPE = (TPCANMessageType)mode;
    fprintf(stdout, "\nTransmitting message(s)...");
    fflush (stdout);
    while (frames < number) {
        message.DATA[0] = (BYTE)((frames + offset) >> 0);
        message.DATA[1] = (BYTE)((frames + offset) >> 8);
        message.DATA[2] = (BYTE)((frames + offset) >> 16);
        message.DATA[3] = (BYTE)((frames + offset) >> 24);
        message.DATA[4] = (BYTE)((frames + offset) >> 32);
        message.DATA[5] = (BYTE)((frames + offset) >> 40);
        message.DATA[6] = (BYTE)((frames + offset) >> 48);
        message.DATA[7] = (BYTE)((frames + offset) >> 56);
        //memset(&message.DATA[8], 0, 8 - 8);
        /* transmit message (repeat when busy) */
retry_tx_frames:
        calls++;
        if ((status = CAN_Write(channel, &message)) == PCAN_ERROR_OK)
            fprintf(stderr, "%s", prompt[(frames++ % 4)]);
        else if ((status == PCAN_ERROR_QXMTFULL) && running)
            goto retry_tx_frames;
        else
            errors++;
        /* pause between two messages, as you please */
        timer_delay(TIMER_USEC(delay));
        if (!running) {
            fprintf(stderr, "\b");
            fprintf(stdout, "STOP!\n\n");
            fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
            fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
            fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
            fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);
            return frames;
        }
    }
    fprintf(stderr, "\b");
    fprintf(stdout, "OK!\n\n");
    fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
    fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
    fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
    fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);

    timer_delay(TIMER_SEC(1));    // afterburner
    return frames;
}

static uint64_t tx_test(TPCANHandle channel, BYTE mode, uint32_t can_id, uint8_t dlc, uint32_t delay, time_t duration, uint64_t offset)
{
    time_t start = time(NULL);

    TPCANStatus status;
    TPCANMsg message;

    uint64_t frames = 0;
    uint64_t errors = 0;
    uint64_t calls = 0;

    fprintf(stderr, "\nPress ^C to abort.\n");
    message.ID  = (DWORD)can_id;
    message.LEN = (BYTE)dlc;
    message.MSGTYPE = (TPCANMessageType)mode;
    fprintf(stdout, "\nTransmitting message(s)...");
    fflush (stdout);
    while (time(NULL) < (start + duration)) {
        message.DATA[0] = (BYTE)((frames + offset) >> 0);
        message.DATA[1] = (BYTE)((frames + offset) >> 8);
        message.DATA[2] = (BYTE)((frames + offset) >> 16);
        message.DATA[3] = (BYTE)((frames + offset) >> 24);
        message.DATA[4] = (BYTE)((frames + offset) >> 32);
        message.DATA[5] = (BYTE)((frames + offset) >> 40);
        message.DATA[6] = (BYTE)((frames + offset) >> 48);
        message.DATA[7] = (BYTE)((frames + offset) >> 56);
        //memset(&message.DATA[8], 0, 8 - 8);
        /* transmit message (repeat when busy) */
retry_tx_test:
        calls++;
        if ((status = CAN_Write(channel, &message)) == PCAN_ERROR_OK)
            fprintf(stderr, "%s", prompt[(frames++ % 4)]);
        else if ((status == PCAN_ERROR_QXMTFULL) && running)
            goto retry_tx_test;
        else
            errors++;
        /* pause between two messages, as you please */
        timer_delay(TIMER_USEC(delay));
        if (!running) {
            fprintf(stderr, "\b");
            fprintf(stdout, "STOP!\n\n");
            fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
            fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
            fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
            fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);
            return frames;
        }
    }
    fprintf(stderr, "\b");
    fprintf(stdout, "OK!\n\n");
    fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
    fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
    fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
    fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);

    timer_delay(TIMER_SEC(1));    // afterburner
    return frames;
}

static uint64_t rx_test(TPCANHandle channel, int check, uint64_t number, int stop_on_error)
{
    time_t start = time(NULL);

    TPCANStatus status;
    TPCANMsg message;
    TPCANTimestamp timestamp;

    uint64_t frames = 0;
    uint64_t errors = 0;
    uint64_t calls = 0;
    uint64_t data;

#ifdef BLOCKING_READ
    int fdes = -1;

    if ((status = CAN_GetValue(channel, PCAN_RECEIVE_EVENT, &fdes, sizeof(int))) != PCAN_ERROR_OK) {
        fprintf(stderr, "+++ error: CAN_GetValue PCAN_RECEIVE_EVENT returned 0x%X\n", status);
        return 0;;
    }
    fd_set rdfs;
    FD_ZERO(&rdfs);
    FD_SET(fdes, &rdfs);
#endif
    fprintf(stderr, "\nPress ^C to abort.\n");
    fprintf(stdout, "\nReceiving message(s)...");
    fflush (stdout);
    for (;;) {
        if ((status = CAN_Read(channel, &message, &timestamp)) == PCAN_ERROR_OK) {
            if (!(message.MSGTYPE & PCAN_MESSAGE_STATUS)) {
               if (check) {
                    data = 0;
                    if (message.LEN > 0)
                        data |= (uint64_t)message.DATA[0] << 0;
                    if (message.LEN > 1)
                        data |= (uint64_t)message.DATA[1] << 8;
                    if (message.LEN > 2)
                        data |= (uint64_t)message.DATA[2] << 16;
                    if (message.LEN > 3)
                        data |= (uint64_t)message.DATA[3] << 24;
                    if (message.LEN > 4)
                        data |= (uint64_t)message.DATA[4] << 32;
                    if (message.LEN > 5)
                        data |= (uint64_t)message.DATA[5] << 40;
                    if (message.LEN > 6)
                        data |= (uint64_t)message.DATA[6] << 48;
                    if (message.LEN > 7)
                        data |= (uint64_t)message.DATA[7] << 56;
                    if (data != number) {
                        fprintf(stderr, "\b");
                        fprintf(stdout, "FAILED!\n\n");
                        fprintf(stderr, "+++ error: data inconsistent - %"PRIu64" received / %"PRIu64" expected\n", data, number);
                        if (stop_on_error) {
                            fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
                            fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
                            fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
                            fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);
                            return frames+1;
                        }
                        else {
                            fprintf(stderr, "... ");
                            number = data;
                        }
                    }
                    number++;    // depending on DLC received data may wrap around while number is counting up!
                }
                fprintf(stderr, "%s", prompt[(frames++ % 4)]);
            }
            else
                errors++;
        }
        else if (status != PCAN_ERROR_QRCVEMPTY)
            errors++;
        else
#ifdef BLOCKING_READ
            select(fdes+1, &rdfs, NULL, NULL, NULL);
#else
            timer_delay(1);
#endif
        calls++;
        if (!running) {
            fprintf(stderr, "\b");
            fprintf(stdout, "OK!\n\n");
            fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
            fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
            fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
            fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);
            return frames;
        }
    }
    return frames;
}

static uint64_t tx_random_fd(TPCANHandle channel, BYTE mode, uint32_t can_id, uint8_t dlc, uint32_t delay, uint64_t number, uint64_t offset)
{
    time_t start = time(NULL);

    TPCANStatus status;
    TPCANMsgFD message;

    uint64_t frames = 0;
    uint64_t errors = 0;
    uint64_t calls = 0;

    uint8_t  random_dlc = dlc;
    uint32_t random_delay = delay;
    const struct {
        uint32_t delay;
        int counter;
    } random_mess[13] = {
        { TIMER_USEC(100), (385 * 10) },
        { TIMER_USEC(250), (238 * 10) },
        { TIMER_USEC(500), (147 * 10) },
        { TIMER_USEC(714),    (91 * 10) },
        { TIMER_USEC(1000), (56 * 10) },
        { TIMER_USEC(1667), (35 * 10) },
        { TIMER_USEC(2500), (21 * 10) },
        { TIMER_USEC(5000), (13 * 10) },
        { TIMER_USEC(7143),     (8 * 10) },
        { TIMER_USEC(10000), (5 * 10) },
        { TIMER_USEC(16667), (3 * 10) },
        { TIMER_USEC(25000), (2 * 10) },
        { TIMER_USEC(50000), (1 * 10) }
    };
    int index = 6, counter = 13;
    srand((unsigned int)time(NULL));

    fprintf(stderr, "\nPress ^C to abort.\n");
    message.ID  = (DWORD)can_id;
    message.DLC = (BYTE)dlc;
    message.MSGTYPE = (TPCANMessageType)mode;
    fprintf(stdout, "\nTransmitting message(s)...");
    fflush (stdout);
    while (frames < number) {
        message.DATA[0] = (BYTE)((frames + offset) >> 0);
        message.DATA[1] = (BYTE)((frames + offset) >> 8);
        message.DATA[2] = (BYTE)((frames + offset) >> 16);
        message.DATA[3] = (BYTE)((frames + offset) >> 24);
        message.DATA[4] = (BYTE)((frames + offset) >> 32);
        message.DATA[5] = (BYTE)((frames + offset) >> 40);
        message.DATA[6] = (BYTE)((frames + offset) >> 48);
        message.DATA[7] = (BYTE)((frames + offset) >> 56);
        memset(&message.DATA[8], (frames & 1) ? 0x55 : 0xAA, 64 - 8);
        message.DLC = (random_dlc < dlc) ? dlc : random_dlc;
        /* transmit message (repeat when busy) */
retry_tx_random_fd:
        calls++;
        if ((status = CAN_WriteFD(channel, &message)) == PCAN_ERROR_OK)
            fprintf(stderr, "%s", prompt[(frames++ % 4)]);
        else if ((status == PCAN_ERROR_QXMTFULL) && running)
            goto retry_tx_random_fd;
        else
            errors++;
        /* pause between two messages, as you please */
        if (random_delay < TIMER_MSEC(10)) {
            timer_restart(TIMER_GPT0, random_delay);
            while (!timer_timeout(TIMER_GPT0)) {
                if (!running) {
                    fprintf(stderr, "\b");
                    fprintf(stdout, "STOP!\n\n");
                    fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
                    fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
                    fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
                    fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);
                    return frames;
                }
            }
        }
        else /* suspend our thread! */
            timer_delay(random_delay);
        if (!running) {
            fprintf(stderr, "\b");
            fprintf(stdout, "STOP!\n\n");
            fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
            fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
            fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
            fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);
            return frames;
        }
        /* ramdom delay and length */
        if (!counter--) {
            index = rand() % 13; // index {0..12}
            counter = random_mess[index].counter;
        }
        random_delay = (random_mess[index].delay < delay) ? delay : random_mess[index].delay;
        random_dlc = (BYTE)(rand() % (15 + 1));
    }
    fprintf(stderr, "\b");
    fprintf(stdout, "OK!\n\n");
    fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
    fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
    fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
    fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);

    timer_delay(TIMER_SEC(1));    // afterburner
    return frames;
}

static uint64_t tx_frames_fd(TPCANHandle channel, BYTE mode, uint32_t can_id, uint8_t dlc, uint32_t delay, uint64_t number, uint64_t offset)
{
    time_t start = time(NULL);

    TPCANStatus status;
    TPCANMsgFD message;

    uint64_t frames = 0;
    uint64_t errors = 0;
    uint64_t calls = 0;

    fprintf(stderr, "\nPress ^C to abort.\n");
    message.ID  = (DWORD)can_id;
    message.DLC = (BYTE)dlc;
    message.MSGTYPE = (TPCANMessageType)mode;
    fprintf(stdout, "\nTransmitting message(s)...");
    fflush (stdout);
    while (frames < number) {
        message.DATA[0] = (BYTE)((frames + offset) >> 0);
        message.DATA[1] = (BYTE)((frames + offset) >> 8);
        message.DATA[2] = (BYTE)((frames + offset) >> 16);
        message.DATA[3] = (BYTE)((frames + offset) >> 24);
        message.DATA[4] = (BYTE)((frames + offset) >> 32);
        message.DATA[5] = (BYTE)((frames + offset) >> 40);
        message.DATA[6] = (BYTE)((frames + offset) >> 48);
        message.DATA[7] = (BYTE)((frames + offset) >> 56);
        memset(&message.DATA[8], 0, 64 - 8);
        /* transmit message (repeat when busy) */
retry_tx_frames_fd:
        calls++;
        if ((status = CAN_WriteFD(channel, &message)) == PCAN_ERROR_OK)
            fprintf(stderr, "%s", prompt[(frames++ % 4)]);
        else if ((status == PCAN_ERROR_QXMTFULL) && running)
            goto retry_tx_frames_fd;
        else
            errors++;
        /* pause between two messages, as you please */
        timer_delay(TIMER_USEC(delay));
        if (!running) {
            fprintf(stderr, "\b");
            fprintf(stdout, "STOP!\n\n");
            fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
            fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
            fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
            fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);
            return frames;
        }
    }
    fprintf(stderr, "\b");
    fprintf(stdout, "OK!\n\n");
    fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
    fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
    fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
    fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);

    timer_delay(TIMER_SEC(1));    // afterburner
    return frames;
}

static uint64_t tx_test_fd(TPCANHandle channel, BYTE mode, uint32_t can_id, uint8_t dlc, uint32_t delay, time_t duration, uint64_t offset)
{
    time_t start = time(NULL);

    TPCANStatus status;
    TPCANMsgFD message;

    uint64_t frames = 0;
    uint64_t errors = 0;
    uint64_t calls = 0;

    fprintf(stderr, "\nPress ^C to abort.\n");
    message.ID  = (DWORD)can_id;
    message.DLC = (BYTE)dlc;
    message.MSGTYPE = (TPCANMessageType)mode;
    fprintf(stdout, "\nTransmitting message(s)...");
    fflush (stdout);
    while (time(NULL) < (start + duration)) {
        message.DATA[0] = (BYTE)((frames + offset) >> 0);
        message.DATA[1] = (BYTE)((frames + offset) >> 8);
        message.DATA[2] = (BYTE)((frames + offset) >> 16);
        message.DATA[3] = (BYTE)((frames + offset) >> 24);
        message.DATA[4] = (BYTE)((frames + offset) >> 32);
        message.DATA[5] = (BYTE)((frames + offset) >> 40);
        message.DATA[6] = (BYTE)((frames + offset) >> 48);
        message.DATA[7] = (BYTE)((frames + offset) >> 56);
        memset(&message.DATA[8], 0, 64 - 8);
        /* transmit message (repeat when busy) */
retry_tx_test_fd:
        calls++;
        if ((status = CAN_WriteFD(channel, &message)) == PCAN_ERROR_OK)
            fprintf(stderr, "%s", prompt[(frames++ % 4)]);
        else if ((status == PCAN_ERROR_QXMTFULL) && running)
            goto retry_tx_test_fd;
        else
            errors++;
        /* pause between two messages, as you please */
        timer_delay(TIMER_USEC(delay));
        if (!running) {
            fprintf(stderr, "\b");
            fprintf(stdout, "STOP!\n\n");
            fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
            fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
            fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
            fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);
            return frames;
        }
    }
    fprintf(stderr, "\b");
    fprintf(stdout, "OK!\n\n");
    fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
    fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
    fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
    fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);

    timer_delay(TIMER_SEC(1));    // afterburner
    return frames;
}

static uint64_t rx_test_fd(TPCANHandle channel, int check, uint64_t number, int stop_on_error)
{
    time_t start = time(NULL);

    TPCANStatus status;
    TPCANMsgFD message;
    TPCANTimestampFD timestamp;

    uint64_t frames = 0;
    uint64_t errors = 0;
    uint64_t calls = 0;
    uint64_t data;

#ifdef BLOCKING_READ
    int fdes = -1;

    if ((status = CAN_GetValue(channel, PCAN_RECEIVE_EVENT, &fdes, sizeof(int))) != PCAN_ERROR_OK) {
        fprintf(stderr, "+++ error: CAN_GetValue PCAN_RECEIVE_EVENT returned 0x%X\n", status);
        return 0;;
    }
    fd_set rdfs;
    FD_ZERO(&rdfs);
    FD_SET(fdes, &rdfs);
#endif
    fprintf(stderr, "\nPress ^C to abort.\n");
    fprintf(stdout, "\nReceiving message(s)...");
    fflush (stdout);
    for (;;) {
        if ((status = CAN_ReadFD(channel, &message, &timestamp)) == PCAN_ERROR_OK) {
            if (!(message.MSGTYPE & PCAN_MESSAGE_STATUS)) {
               if (check) {
                    data = 0;
                    if (message.DLC > 0)
                        data |= (uint64_t)message.DATA[0] << 0;
                    if (message.DLC > 1)
                        data |= (uint64_t)message.DATA[1] << 8;
                    if (message.DLC > 2)
                        data |= (uint64_t)message.DATA[2] << 16;
                    if (message.DLC > 3)
                        data |= (uint64_t)message.DATA[3] << 24;
                    if (message.DLC > 4)
                        data |= (uint64_t)message.DATA[4] << 32;
                    if (message.DLC > 5)
                        data |= (uint64_t)message.DATA[5] << 40;
                    if (message.DLC > 6)
                        data |= (uint64_t)message.DATA[6] << 48;
                    if (message.DLC > 7)
                        data |= (uint64_t)message.DATA[7] << 56;
                    if (data != number) {
                        fprintf(stderr, "\b");
                        fprintf(stdout, "FAILED!\n\n");
                        fprintf(stderr, "+++ error: data inconsistent - %"PRIu64" received / %"PRIu64" expected\n", data, number);
                        if (stop_on_error) {
                            fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
                            fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
                            fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
                            fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);
                            return frames+1;
                        }
                        else {
                            fprintf(stderr, "... ");
                            number = data;
                        }
                    }
                    number++;    // depending on DLC received data may wrap around while number is counting up!
                }
                fprintf(stderr, "%s", prompt[(frames++ % 4)]);
            }
            else
                errors++;
        }
        else if (status != PCAN_ERROR_QRCVEMPTY)
            errors++;
        else
#ifdef BLOCKING_READ
            select(fdes+1, &rdfs, NULL, NULL, NULL);
#else
            timer_delay(1);
#endif
        calls++;
        if (!running) {
            fprintf(stderr, "\b");
            fprintf(stdout, "OK!\n\n");
            fprintf(stdout, "Message(s)=%"PRIu64"\n", frames);
            fprintf(stdout, "Error(s)=%"PRIu64"\n", errors);
            fprintf(stdout, "Call(s)=%"PRIu64"\n", calls);
            fprintf(stdout, "Time=%lisec\n\n", time(NULL) - start);
            return frames;
        }
    }
    return frames;
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
    fprintf(stream, "Usage: %s <interface> [<option>...]\n", program);
    fprintf(stream, "Options for receiver test (default):\n");
    fprintf(stream, " -r, --receive                 count received messages until ^C is pressed\n");
    fprintf(stream, " -n, --number=<number>         check up-counting numbers starting with <number>\n");
    fprintf(stream, " -s, --stop                    stop on error (with option --number)\n");
    fprintf(stream, " -m, --mode=(2.0|FDF[+BRS])    CAN operation mode: CAN 2.0 or CAN FD format\n");
    fprintf(stream, "     --listen-only             monitor mode (transmitter is off)\n");
    fprintf(stream, "     --no-status-frames        suppress reception of status frames\n");
    fprintf(stream, "     --no-remote-frames        suppress reception of remote frames\n");
    // fprintf(stream, "     --code=<id>               acceptance code for 11-bit IDs (default=0x%03X)\n", CODE_11BIT);
    // fprintf(stream, "     --mask=<id>               acceptance mask for 11-bit IDs (default=0x%03X)\n", MASK_11BIT);
    // fprintf(stream, "     --xtd-code=<id>           acceptance code for 29-bit IDs (default=0x%08X)\n", CODE_29BIT);
    // fprintf(stream, "     --xtd-mask=<id>           acceptance mask for 29-bit IDs (default=0x%08X)\n", MASK_29BIT);
    fprintf(stream, " -b, --baudrate=<baudrate>     CAN 2.0 bit timing in kbps (default=250)\n");
    fprintf(stream, "     --bitrate=<bit-rate>      CAN FD bit rate (as a string)\n");
    fprintf(stream, " -v, --verbose                 show detailed bit rate settings\n");
    fprintf(stream, " -y, --trace=(ON|OFF)          write a trace file (default=OFF)\n");
    fprintf(stream, "Options for transmitter test:\n");
    fprintf(stream, " -t, --transmit=<time>         send messages for the given time in seconds, or\n");
    fprintf(stream, " -f, --frames=<number>,        alternatively send the given number of messages, or\n");
    fprintf(stream, "     --random=<number>         optionally with random cycle time and data length\n");
    fprintf(stream, " -c, --cycle=<cycle>           cycle time in milliseconds (default=0) or\n");
    fprintf(stream, " -u, --usec=<cycle>            cycle time in microseconds (default=0)\n");
    fprintf(stream, " -d, --dlc=<length>            send messages of given length (default=8)\n");
    fprintf(stream, " -i, --id=<can-id>             use given identifier (default=100h)\n");
    fprintf(stream, " -n, --number=<number>         set first up-counting number (default=0)\n");
    fprintf(stream, " -m, --mode=(2.0|FDF[+BRS])    CAN operation mode: CAN 2.0 or CAN FD format\n");
    fprintf(stream, " -b, --baudrate=<baudrate>     CAN 2.0 bit timing in kbps (default=250)\n");
    fprintf(stream, "     --bitrate=<bit-rate>      CAN FD bit rate (as a string)\n");
    fprintf(stream, " -v, --verbose                 show detailed bit rate settings\n");
    fprintf(stream, " -y, --trace=(ON|OFF)          write a trace file (default=OFF)\n");
    fprintf(stream, "Other options:\n");
    fprintf(stream, "     --list-bitrates[=<mode>]  list standard bit-rate settings and exit\n");
    fprintf(stream, " -L, --list-boards             list all supported CAN interfaces and exit\n");
    fprintf(stream, " -T, --test-boards             list all available CAN interfaces and exit\n");
    fprintf(stream, " -h, --help                    display this help screen and exit\n");
    fprintf(stream, "     --version                 show version information and exit\n");
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
