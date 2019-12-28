#include "pthread.h"
#include "time.h"
#include <vector>

#ifndef _WIN32
#ifdef __linux__
#include <unistd.h>         // for syscall()
#include <sys/syscall.h>    // for SYS_xxx definitions
#include <time.h>           // for clock_gettime
#else
#include <sys/time.h>       // for gettimeofday
#endif

namespace xx {

#ifdef __linux__
#ifndef SYS_gettid
#define SYS_gettid __NR_gettid
#endif

unsigned int gettid() {
    return syscall(SYS_gettid);
}

#else /* for mac, bsd */

unsigned int gettid() {
    uint64 x;
    pthread_threadid_np(0, &x);
    return (unsigned int) x;
}

#endif

} // namespace xx

#endif
