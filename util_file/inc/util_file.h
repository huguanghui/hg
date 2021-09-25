#ifndef __UTIL_FILE_H__
#define __UTIL_FILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void util_file_echo();

void utile_file_append(const char *file, char *buf, unsigned int buf_size);

#ifdef __cplusplus
}
#endif

#endif // !__UTIL_FILE_H__
