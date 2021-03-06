#include "fast.h"
#include "fastring.h"
#include "lru_map.h"
#include <stdio.h>
#include <string.h>

namespace fast {

static uint16 itoh_table[256];
static uint32 itoa_table[10000];

int init_table() {
    for (int i = 0; i < 256; ++i) {
        char* b = (char*)(itoh_table + i);
        b[0] = "0123456789abcdef"[i >> 4];
        b[1] = "0123456789abcdef"[i & 0x0f];
    }       
        
    for (int i = 0; i < 10000; ++i) {
        char* b = (char*)(itoa_table + i);
        b[3] = (char)(i % 10 + '0');
        b[2] = (char)(i % 100 / 10 + '0');
        b[1] = (char)(i % 1000 / 100 + '0');
        b[0] = (char)(i / 1000); 
  
        // digits of i: (b[0] >> 4) + 1 
        if (i > 999) {
            b[0] |= (3 << 4); // 0x30
        } else if (i > 99) {
            b[0] |= (2 << 4); // 0x20
        } else if (i > 9) {
            b[0] |= (1 << 4); // 0x10
        }
    }

    return 0;
}

static int xxInitialized = init_table();

int u32toh(uint32 v, char* buf) {
    uint16 b[4], *p = b + 4;

    do {
        *--p = itoh_table[v & 0xff];
        v >>= 8;
    } while (v > 0);

    buf[0] = '0';
    buf[1] = 'x';
    int len = (int) ((char*)(b + 4) - (char*)p - (*(char*)p == '0'));
    memcpy(buf + 2, (char*)(b + 4) - len, (size_t)len);
    return len + 2;
}

int u64toh(uint64 v, char* buf) {
    uint16 b[8], *p = b + 8;

    do {
        *--p = itoh_table[v & 0xff];
        v >>= 8;
    } while (v > 0);

    buf[0] = '0';
    buf[1] = 'x';
    int len = (int) ((char*)(b + 8) - (char*)p - (*(char*)p == '0'));
    memcpy(buf + 2, (char*)(b + 8) - len, (size_t)len);
    return len + 2;
}

int u32toa(uint32 v, char* buf) {
    uint32 b[3], *p = b + 2;

    if (v > 9999) {
        b[1] = v / 10000;
        b[2] = itoa_table[v - b[1] * 10000] | 0x30303030;
        --p;
    } else {
        b[2] = itoa_table[v];
        goto u32toa_end;
    }

    if (b[1] > 9999) {
        b[0] = b[1] / 10000;
        b[1] = itoa_table[b[1] - b[0] * 10000] | 0x30303030;
        b[0] = itoa_table[b[0]];
        --p;
    } else {
        b[1] = itoa_table[b[1]];
        goto u32toa_end;
    }

  u32toa_end:
    int len = (int) (((char*)b) + 9 + ((*(char*)p) >> 4) - ((char*)p));
    memcpy(buf, ((char*)b) + 12 - len, (size_t)len);
    return len;
}

int u64toa(uint64 v, char* buf) {
    uint32 b[5], *p = b + 4;
    uint64 x;

    if (v > 9999) {
        x = v / 10000;
        b[4] = itoa_table[v - x * 10000] | 0x30303030;
        --p;
    } else {
        b[4] = itoa_table[v];
        goto u64toa_end;
    }

    if (x > 9999) {
        v = x / 10000;
        b[3] = itoa_table[x - v * 10000] | 0x30303030;
        --p;
    } else {
        b[3] = itoa_table[x];
        goto u64toa_end;
    }

    if (v > 9999) {
        x = v / 10000;
        b[2] = itoa_table[v - x * 10000] | 0x30303030;
        --p;
    } else {
        b[2] = itoa_table[v];
        goto u64toa_end;
    }

    if (x > 9999) {
        b[0] = (uint32) (x / 10000);
        b[1] = itoa_table[x - b[0] * 10000] | 0x30303030;
        b[0] = itoa_table[b[0]];
        --p;
    } else {
        b[1] = itoa_table[x];
        goto u64toa_end;
    }

  u64toa_end:
    int len = (int) (((char*)b) + 17 + ((*(char*)p) >> 4) - ((char*)p));
    memcpy(buf, ((char*)b) + 20 - len, (size_t)len);
    return len;
}

__thread LruMap<uint32, fastring>* float_cache = 0;
__thread LruMap<uint64, fastring>* double_cache = 0;

#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

int dtoa(float v, char* buf) {
    (void) xxInitialized;
    uint32 key = *(uint32*)&v;

    if (float_cache) {
        auto it = float_cache->find(key);
        if (it != float_cache->end()) {
            const fastring& s = it->second;
            memcpy(buf, s.data(), s.size());
            return (int) s.size();
        }
    } else {
        float_cache = new LruMap<uint32, fastring>(1024);
    }

  #ifdef _WIN32
    int r = _snprintf_s(buf, 24, 24, "%.7g", v);
  #else
    int r = snprintf(buf, 24, "%.7g", v);
  #endif

    if (r >= 0) {
        float_cache->insert(key, fastring(buf, r));
        return r;
    }

    return 0;
}

int dtoa(double v, char* buf) {
    uint64 key = *(uint64*)&v;

    if (double_cache) {
        auto it = double_cache->find(key);
        if (it != double_cache->end()) {
            const fastring& s = it->second;
            memcpy(buf, s.data(), s.size());
            return (int) s.size();
        }
    } else {
        double_cache = new LruMap<uint64, fastring>(1024);
    }

  #ifdef _WIN32
    int r = _snprintf_s(buf, 24, 24, "%.7g", v);
  #else
    int r = snprintf(buf, 24, "%.7g", v);
  #endif

    if (r >= 0) {
        double_cache->insert(key, fastring(buf, r));
        return r;
    }

    return 0;
}

#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
#pragma GCC diagnostic pop
#endif

}; // namespace fast 
