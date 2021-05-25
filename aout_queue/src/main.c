#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "aout_queue.h"

static void *do_data(void* param)
{
    int rc;
    AOUT_BUF s;
    pthread_detach(pthread_self());

    while(1) {
        memset(&s, 0, sizeof(AOUT_BUF));
        rc = aout_dequeue(&s);
        printf("HGH-TEST[%s %d]rc: %d\n", __FUNCTION__, __LINE__, rc);
        if ((0 == rc) && (s.data != NULL)) {
            printf("HGH-TEST[%s %d] Data: %s\n", __FUNCTION__, __LINE__,
                        s.data);
        }
        sleep(1);
    }

    return NULL;
}

int main(int argc, char **argv) {
    int i;
    char tmp[32] = {0};
    pthread_t tid;
    AOUT_BUF buf = {0};
    AOUT_CAP cap = {0};
    cap.max_datalen = 320;
    cap.queue_size  = 10;

    aout_queue_create(cap);
    pthread_create(&tid, NULL, do_data, NULL);

    for (i=0; i<15; i++){
        buf.type = AENC_G711u;
        snprintf(tmp, sizeof(tmp), "%d", i);
        buf.datalen = strlen(tmp)+1;
        buf.data = tmp;
        aout_enqueue(buf);
        printf("HGH-TEST[%s %d]\n", __FUNCTION__, __LINE__);
    }
    while(1) {
        printf("HGH-TEST[%s %d]\n", __FUNCTION__, __LINE__);
        sleep(5);
    }
    aout_queue_destroy();

    return 0;
}
