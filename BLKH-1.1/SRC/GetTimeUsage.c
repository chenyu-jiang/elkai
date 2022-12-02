#include "BLKH.h"

double GetTimeUsage(int who)
{
    struct rusage ru;
    getrusage(who, &ru);
    return ru.ru_utime.tv_sec + ru.ru_utime.tv_usec / 1000000.0;
}
