#define _POSIX_C_SOURCE 200809L //kept getting errors for CLOCK_REALTIME

#include <time.h>
#include "utils.h"

long long getTimeInMs() {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000 + nanoSeconds / 1000000;
    return milliSeconds;
}

void sleepForMs(long long delayInMs) {
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    long timeSec = delayNs / NS_PER_SECOND;
    long timeNs = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {timeSec, timeNs};
    nanosleep(&reqDelay, (struct timespec *)NULL);
}