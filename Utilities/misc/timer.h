/*  -- $HeadURL: https://svn.uv-software.net/sourcemedley/library/trunk/c/timer.h $ --
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
 *  export    :  timer_obj_t timer_new(uint32_t timeout);
 *               int timer_restart(timer_obj_t *self, uint32_t timeout);
 *               int timer_timeout(timer_obj_t *self);
 *               int timer_delay(uint32_t delay);
 *
 *  includes  :  <stdio.h>, <stdint.h>
 *
 *  author(s) :  Uwe Vogt, UV Software
 *
 *  e-mail    :  uwe.vogt@uv-software.de
 *
 *
 *  -----------  description  -------------------------------------------
 */
/** @file        timer.h
 *
 *  @brief       A high-resolution Timer.
 *
 *               Including a general purpose timer (GPT0).
 *
 *  @remark      The name 'timer_t' is used in stdlih.h on Linux. Even
 *               if this name is not POSIX compliant, I renamed it.
 *
 *  @author      $Author: haumea $
 *
 *  @version     $Rev: 478 $
 *
 *  @defgroup    timer A high-resolution Timer
 *  @{
 */
#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

/*  -----------  includes  ----------------------------------------------
 */

#include <stdio.h>
#include <stdint.h>


/*  -----------  options  -----------------------------------------------
 */

#if defined(_WIN32) || defined(_WIN64)
#define TIMER_WAITABLE_TIMER            /**< Windows alternative for usleep() */
#endif

/*  -----------  defines  -----------------------------------------------
 */

#define TIMER_GPT0          NULL        /**< general purpose timer */

#define TIMER_USEC(x)       (uint32_t)((uint32_t)(x) * (uint32_t)1)
#define TIMER_MSEC(x)       (uint32_t)((uint32_t)(x) * (uint32_t)1000)
#define TIMER_SEC(x)        (uint32_t)((uint32_t)(x) * (uint32_t)1000000)
#define TIMER_MIN(x)        (uint32_t)((uint32_t)(x) * (uint32_t)60000000)


/*  -----------  types  -------------------------------------------------
 */

typedef uint64_t timer_obj_t;           /**< timer object */


/*  -----------  variables  ---------------------------------------------
 */


/*  -----------  prototypes  --------------------------------------------
 */

/** @brief       creates and starts a new timer object.
 *
 *  @param[in]   microseconds  in [usec]
 *
 *  @returns     a timer object on success, or zero otherwise
 */
timer_obj_t timer_new(uint32_t microseconds);


/** @brief       restarts an expired or running timer object.
 *
 *  @param[in]   self  pointer to a timer object, or NULL (GPT0)
 *  @param[in]   microseconds  in [usec]
 *
 *  @returns     none-zero value on success, or zero otherwise
 */
int timer_restart(timer_obj_t *self, uint32_t microseconds);


/** @brief       returns True when the given timer object has expired.
 *
 *  @param[in]   self  pointer to a timer object, or NULL (GPT0)
 *
 *  @returns     none-zero value when timed out, or zero otherwise
 */
int timer_timeout(timer_obj_t *self);


/** @brief       suspends the calling thread for the given time period.
 *
 *  @param[in]   microseconds  in [usec]
 *
 *  @returns     none-zero value on success, or zero otherwise
 */
int timer_delay(uint32_t microseconds);


#endif /* TIMER_H_INCLUDED */
/** @}
 */
 /* ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
