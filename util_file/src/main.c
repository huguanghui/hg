#include "util_file.h"
#include <stdio.h>

void test_file_append() {
  int i = 0;

  for (i = 0; i < 5; i++) {
    utile_file_append("test.log", "test", 4);
  }

  return;
}

int main(int argc, char *argv[]) {
  util_file_echo();
  test_file_append();
  return 0;
}
