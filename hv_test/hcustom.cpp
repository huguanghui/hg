#include "hv/hloop.h"
#include "hv/hbase.h"
#include "hv/hlog.h"
#include "hv/nlog.h"

#include "shared_buff.h"

static SharedBuffer *gSharedBuff = NULL;

void mylogger(int loglevel, const char* buf, int len) {
    //if (loglevel >= LOG_LEVEL_ERROR) {
        stderr_logger(loglevel, buf, len);
    //}

    //if (loglevel >= LOG_LEVEL_INFO) {
        file_logger(loglevel, buf, len);
    //}

    network_logger(loglevel, buf, len);
}

void timer_product_read(htimer_t* timer) {
    char buf[4096] = {0};
    int readlen = gSharedBuff->get(buf, sizeof(buf));
    hlogi("custom tmp: %s len: %d", buf, readlen);
}

int main() {
    hloop_t* loop = hloop_new(0);

    gSharedBuff = new SharedBuffer(TypeConsumer, ModeBlock);

    hlog_set_level(LOG_LEVEL_DEBUG);
    logger_set_handler(hlog, mylogger);
    // htimer_add(loop, timer_write_log, 1000, INFINITE);
    htimer_add(loop, timer_product_read, 3000, INFINITE);
    nlog_listen(loop, 12345);
    hloop_run(loop);

    delete gSharedBuff;
    hloop_free(&loop);
    return 0;
}
