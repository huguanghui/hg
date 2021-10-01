#include "util_file.h"

/******** get clock time **********/

#ifndef MS
#define MS(ts) (unsigned int)((ts.tv_sec * 1000) + (ts.tv_nsec / 1000000))
#endif /* !MS */

void util_gettime()
{
    struct timespec tp;
    unsigned int begin_ms = 0;
    unsigned int end_ms = 0;

    printf("[%s %d]\n", __FUNCTION__, __LINE__);
    clock_gettime(CLOCK_MONOTONIC, &tp);
    begin_ms = MS(tp);
    sleep(1);
    clock_gettime(CLOCK_MONOTONIC, &tp);
    end_ms = MS(tp);
    printf("[%s %d] time: %d\n", __FUNCTION__, __LINE__, (end_ms - begin_ms));

    return;
}

/***********************************/

/**************** hexdump ****************/
void util_hexdump(const char *prefix, void *buf, size_t len)
{
    if ((NULL == prefix) || (NULL == buf)) {
        return;
    }

    char *b = (char *)buf;
    int numRow = 0;
    int numCol = 0;
    size_t i = 0;

    printf("\nHex[0x%p:0x%x]:%s\n", buf, (unsigned int)len, prefix);
    for (i = 0; i < len; i = i + 16) {
        printf("%p|", b + i);
        numCol = (len - i >= 16) ? 16 : (len - i);
        int j = 0;
        for (j = 0; j < numCol; j++) {
            unsigned char c
                = b[i + j]; // must use unsigned char to print >128 value
            if (c < 16) {
                printf(" 0%x", c);
            } else {
                printf(" %x", c);
            }
        }
        printf("\n");
    }
    printf("\n");
    return;
}
/***********************************/

void util_file_echo()
{
    printf("[%s %d]\n", __FUNCTION__, __LINE__);
    return;
}

void utile_file_append(const char *file, char *buf, unsigned int buf_size)
{
    if (NULL != file) {
        int fd = -1;
        fd = open(file, O_CREAT | O_RDWR | O_APPEND, 0755);
        if (fd > 0) {
            write(fd, buf, buf_size);
            close(fd);
        } else {
            printf("open failed! [%d]\n", fd);
        }
    }
    return;
}
