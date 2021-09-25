#include "util_file.h"

void util_file_echo() {
  printf("[%s %d]\n", __FUNCTION__, __LINE__);
  return;
}

void utile_file_append(const char *file, char *buf, unsigned int buf_size) {
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
