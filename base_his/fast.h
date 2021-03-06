#pragma once

#include "def.h"

namespace fast {

// convert float point numbers to string
int dtoa(float v, char* buf);
int dtoa(double v, char* buf);

// convert integer to HEX string
int u32toh(uint32 v, char* buf);
int u64toh(uint64 v, char* buf);

// convert integer to ASCII string
int u32toa(uint32 v, char* buf);
int u64toa(uint64 v, char* buf);

inline int i32toa(int32 v, char* buf) {
    if (v >= 0) return u32toa((uint32)v, buf);
    *buf = '-';
    return u32toa((uint32)(-v), buf + 1) + 1;
}

inline int i64toa(int64 v, char* buf) {
    if (v >=0 ) return u64toa((uint64)v, buf);
    *buf = '-';
    return u64toa((uint64)(-v), buf + 1) + 1;
}

} // namespace fast
