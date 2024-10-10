#include <sys/time.h>
#include "tx_api.h"
extern "C" int gettimeofday(struct timeval *tv, void *tz)
{
    const auto t = tx_time_get();
    if(tv)
    {
        tv->tv_sec = t/TX_TICK_RATE_HZ;
        tv->tv_usec = t%TX_TICK_RATE_HZ * (1000000/TX_TICK_RATE_HZ);
    }
    
    return 0;
}