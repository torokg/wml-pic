/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/**   ThreadX Component                                                   */
/**                                                                       */
/**   POSIX Compliancy Wrapper (POSIX)                                    */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/**************************************************************************/
/*                                                                        */
/*  EKP DEFINITIONS                                        RELEASE        */
/*                                                                        */
/*    tx_px_time.h                                        PORTABLE C      */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the constants, structures, etc. needed to         */
/*    implement time related functionality for the Evacuation Kit         */
/*    for POSIX Users (POSIX)                                             */
/*                                                                        */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  06-02-2021     William E. Lamie         Initial Version 6.1.7         */
/*                                                                        */
/**************************************************************************/

#ifndef _TX_PX_TIME_H
#define _TX_PX_TIME_H
#include <tx_port.h>
//#include_next <time.h>


#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _TIME_T
#define _TIME_T
typedef  ULONG  time_t;
#endif

typedef unsigned long clock_t;

struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
	long __tm_gmtoff;
	const char *__tm_zone;
};


typedef INT clockid_t;

struct timespec 
{
    time_t    tv_sec;                /* time in terms of seconds */
    ULONG     tv_nsec;               /* remaining time in terms of nano seconds*/
};


struct itimerspec
{
    struct timespec  it_interval ;         /* Timer period. */
    struct timespec  it_value;             /* Timer expiration. */ 
};

#define CLOCK_REALTIME 0
#define TIMER_ABSTIME   1
//#define CLOCK_MONOTONIC 2
#define CLOCK_REALTIME_COARSE    5
#define CLOCK_REALTIME_ALARM     8

clock_t clock (void);

double difftime (time_t, time_t);
time_t mktime (struct tm *);
time_t time (time_t *);
char *asctime (const struct tm *);
char *ctime (const time_t *);
struct tm *gmtime (const time_t *);
struct tm *localtime (const time_t *);
size_t strftime (char *__restrict, size_t, const char *__restrict, const struct tm *__restrict);

//INT clock_settime(clockid_t, const struct timespec *);
//INT clock_gettime(clockid_t, struct timespec *);
//INT clock_getres(clockid_t, struct timespec *);


INT nanosleep(struct timespec *req, struct timespec *rem);
UINT sleep(ULONG seconds);

#ifdef __cplusplus
}
#endif

#endif


