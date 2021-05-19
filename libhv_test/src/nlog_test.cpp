#include "hv/hloop.h"
#include "hv/hbase.h"
#include "hv/hlog.h"
#include "hv/nlog.h"

void timer_write_log(htimer_t* timer) 
{
    static int cnt = 0;
    hlogd("[%d] Do you recv me?", ++cnt);
    hlogi("[%d] Do you recv me?", ++cnt);
    hloge("[%d] Do you recv me?", ++cnt);
}

int main(int argc, char *argv[])
{
    hloop_t *loop = hloop_new(0);

    htimer_add(loop, timer_write_log, 1000, INFINITE);
    
    hloop_run(loop);
    hloop_free(&loop);
    return 0;
}
