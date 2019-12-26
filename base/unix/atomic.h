#pragma once

#ifndef _WIN32

// clang or gcc 4.7+
#if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)))

#else
#error "clang or gcc 4.1+ required
#endif

#endif
