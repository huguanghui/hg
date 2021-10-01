#include <stdio.h>

#include "util_file.h"
#include "util_log.h"
#include "util_timeout_task.h"

void test_file_append()
{
    int i = 0;

    for (i = 0; i < 5; i++) {
        utile_file_append("test.log", "test", 4);
    }

    return;
}

bool timer_test(void *user_data)
{
    dbg_log("");
    return IPC_TIMEOUT_TASK_CONTINUE;
}

int main(int argc, char *argv[])
{
    char data[4] = { 0x11, 0xFE };
    util_hexdump("Test Hexdump", data, sizeof(data));
    util_gettime();
    util_file_echo();
    test_file_append();
    ipc_timeout_task_init();

    ipc_timeout_task_add(1000, timer_test, NULL);
    while (1) {
        sleep(1);
    }

    ipc_timeout_task_deinit();
    return 0;
}
