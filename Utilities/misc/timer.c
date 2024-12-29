/*  -- $HeadURL: https://svn.uv-software.net/sourcemedley/library/trunk/c/timer.c $ --
 *
 *  project   :  Industrial Communication.
 *
 *  purpose   :  A high-resolution Timer.
 *
 *  copyright :  (C) 2013,2019 by UV Software, Berlin
 *
 *  compiler  :  Microsoft Visual C/C++ Compiler (Version 15.16)
 *               Apple LLVM version 10.0.0 (clang-1000.11.45.5)
 *               GCC - GNU C Compiler (Debian 6.3.0-18+deb9u1)
 *               GCC - GNU C Compiler (MinGW.org GCC-8.2.0-3)
 *
 *  export    :  (see header file)
 *
 *  includes  :  timer.h (<stdint.h>)
 *
 *  author(s) :  Uwe Vogt, UV Software
 *
 *  e-mail    :  uwe.vogt@uv-software.de
 *
 *
 *  -----------  description  -------------------------------------------
 */
/** @file        timer.c
 *
 *  @brief       A high-resolution Timer.
 *
 *  @author      $Author: haumea $
 *
 *  @version     $Rev: 478 $
 *
 *  @todo        replace `gettimeofday' by `clock_gettime' and `usleep' by `clock_nanosleep'
 *
 *  @addtogroup  timer
 *  @{
 */

/*static const char _id[] = "$Id: timer.c 478 2019-12-10 21:03:29Z haumea $";*/


/*  -----------  includes  ----------------------------------------------
 */

#include "timer.h"                      /* interface prototypes */

#include <stdio.h>                      /* standard I/O routines */
#include <errno.h>                      /* system wide error numbers */
#include <string.h>                     /* string manipulation functions */
#include <stdlib.h>                     /* commonly used library functions */
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>                    /* master include file for Windows */
#else
#include <unistd.h>
#include <sys/time.h>
#endif


/*  -----------  defines  -----------------------------------------------
 */


/*  -----------  types  -------------------------------------------------
 */


/*  -----------  prototypes  --------------------------------------------
 */


/*  -----------  variables  ---------------------------------------------
 */

static timer_obj_t gpt0 = 0;            /**< general purpose timer (GPT0) */


/*  -----------  functions  ---------------------------------------------
 */

timer_obj_t timer_new(uint32_t microseconds)
{
#if !defined(_WIN32) && !defined(_WIN64)
    struct timeval tv;
    gettimeofday(&tv, NULL);

    uint64_t llUntilStop = ((uint64_t)tv.tv_sec * (uint64_t)1000000) + (uint64_t)tv.tv_usec \
                         + ((uint64_t)microseconds);
#else
    LARGE_INTEGER largeFrequency;       // high-resolution timer frequency
    LARGE_INTEGER largeCounter;         // high-resolution performance counter

    LONGLONG llUntilStop = 0;           // counter value when to stop (time-out)

    // retrieve the frequency of the high-resolution performance counter
    if (!QueryPerformanceFrequency(&largeFrequency))
        return 0;
    // retrieve the current value of the high-resolution performance counter
    if (!QueryPerformanceCounter(&largeCounter))
        return 0;
    // calculate the counter value for the desired time-out
    llUntilStop = largeCounter.QuadPart + ((largeFrequency.QuadPart * (LONGLONG)microseconds)
                                                                    / (LONGLONG)1000000);
#endif
    return (uint64_t)llUntilStop;
}

int timer_restart(timer_obj_t *self, uint32_t microseconds)
{
#if !defined(_WIN32) && !defined(_WIN64)
    struct timeval tv;
    gettimeofday(&tv, NULL);

    uint64_t llUntilStop = ((uint64_t)tv.tv_sec * (uint64_t)1000000) + (uint64_t)tv.tv_usec \
                         + ((uint64_t)microseconds);
#else
    LARGE_INTEGER largeFrequency;       // high-resolution timer frequency
    LARGE_INTEGER largeCounter;         // high-resolution performance counter

    LONGLONG llUntilStop = 0;           // counter value when to stop (time-out)

    // retrieve the frequency of the high-resolution performance counter
    if(!QueryPerformanceFrequency(&largeFrequency))
        return 0;
    // retrieve the current value of the high-resolution performance counter
    if(!QueryPerformanceCounter(&largeCounter))
        return 0;
    // calculate the counter value for the desired time-out
    llUntilStop = largeCounter.QuadPart + ((largeFrequency.QuadPart * (LONGLONG)microseconds)
                                                                    / (LONGLONG)1000000);
#endif
    // update the timer instance or the general purpose timer (NULL)
    if (self)
        *self = (uint64_t)llUntilStop;
    else
         gpt0 = (uint64_t)llUntilStop;
    return 1;
}

int timer_timeout(timer_obj_t *self)
{
#if !defined(_WIN32) && !defined(_WIN64)
    uint64_t llNow;
    struct timeval tv;
    gettimeofday(&tv, NULL);

    uint64_t llUntilStop = self ? (uint64_t)*self : (uint64_t)gpt0;

    llNow = ((uint32_t)tv.tv_sec * (uint64_t)1000000) + (uint64_t)tv.tv_usec;

    if(llNow < llUntilStop)
        return 0;
    else
        return 1;
#else
    LARGE_INTEGER largeFrequency;       // high-resolution timer frequency
    LARGE_INTEGER largeCounter;         // high-resolution performance counter

    LONGLONG llUntilStop = self ? (LONGLONG)*self : (LONGLONG)gpt0;

    // retrieve the frequency of the high-resolution performance counter
    if (!QueryPerformanceFrequency(&largeFrequency))
        return 0;
    // retrieve the current value of the high-resolution performance counter
    if (!QueryPerformanceCounter(&largeCounter))
        return 0;
    // a time-out occurred, if the counter overruns the time-out value
    if (largeCounter.QuadPart < llUntilStop)
        return 0;
    else
        return 1;
#endif
}

int timer_delay(uint32_t microseconds)
{
#if !defined(_WIN32) && !defined(_WIN64)
    return (usleep((useconds_t)microseconds) != 0) ? 0 : 1;
#else
# ifndef TIMER_WAITABLE_TIMER
    timer_obj_t local = timer_new(microseconds);

    while (!timer_timeout(&local)) {
        Sleep(0);
    }
    return 1;
# else
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10 * (LONGLONG)microseconds); // Convert to 100 nanosecond interval, negative value indicates relative time

    if (microseconds >= 100) {  // FIXME: Who made this decision?
        if ((timer = CreateWaitableTimer(NULL, TRUE, NULL)) != NULL) {
            SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
            WaitForSingleObject(timer, INFINITE);
            CloseHandle(timer);
        }
    }
    else {
        // According to MSDN's documentation for Sleep:
        // | A value of zero causes the thread to relinquish the remainder of its time slice to any other
        // | thread that is ready to run.If there are no other threads ready to run, the function returns
        // | immediately, and the thread continues execution.
        Sleep(0);
    }
    return 1;
# endif
#endif
}

/*  -----------  local functions  ---------------------------------------
 */


/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
