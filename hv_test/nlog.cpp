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

void timer_write_log(htimer_t* timer) {
    static int cnt = 0;
    hlogd("[%d] Do you recv me?", ++cnt);
    hlogi("[%d] Do you recv me?", ++cnt);
    hloge("[%d] Do you recv me?", ++cnt);
}

void timer_product_write(htimer_t* timer) {
    static int cnt = 0;
    hlogi("product len: %d", cnt);
    char buf[4096] = {0};
    snprintf(buf, sizeof(buf), "tmp: %d", cnt);
    gSharedBuff->put(buf, sizeof(buf));
    cnt++;
}

int main() {
    hloop_t* loop = hloop_new(0);

    gSharedBuff = new SharedBuffer(TypeProducer, ModeBlock);

    hlog_set_level(LOG_LEVEL_DEBUG);
    logger_set_handler(hlog, mylogger);
    // htimer_add(loop, timer_write_log, 1000, INFINITE);
    htimer_add(loop, timer_product_write, 3000, INFINITE);
    nlog_listen(loop, DEFAULT_LOG_PORT);
    hloop_run(loop);

    delete gSharedBuff;
    hloop_free(&loop);
    return 0;
}
