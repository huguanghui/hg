#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "aout_queue.h"

static void* do_data(void* param)
{
    int rc;
    AOUT_BUF s;
    pthread_detach(pthread_self());

    while (1) {
        memset(&s, 0, sizeof(AOUT_BUF));
        rc = aout_dequeue(&s);
        printf("HGH-TEST[%s %d]rc: %d\n", __FUNCTION__, __LINE__, rc);
        if ((0 == rc) && (s.data != NULL)) {
            printf("HGH-TEST[%s %d] Data: %s\n", __FUNCTION__, __LINE__,
                s.data);
        }
        usleep(50 * 1000);
    }

    return NULL;
}

static void* do_file_data(void* param)
{
    int rc;
    AOUT_FILE_ITEM s;
    pthread_detach(pthread_self());

    sleep(10);
    while (1) {
        memset(&s, 0, sizeof(AOUT_FILE_ITEM));
        rc = aout_file_dequeue(&s);
        aout_status_speech_refresh();
        printf("HGH-TEST[%s %d]rc: %d\n", __FUNCTION__, __LINE__, rc);
        if (0 == rc) {
            printf("HGH-TEST[%s %d] FileName: %s\n", __FUNCTION__, __LINE__,
                s.filename);
        }
        usleep(400 * 1000);
    }

    return NULL;
}

int main(int argc, char** argv)
{
    int i;
    char tmp[32] = { 0 };
    pthread_t tid;
    AOUT_BUF buf = { 0 };
    AOUT_CAP cap = { 0 };
    cap.max_datalen = 320;
    cap.queue_size = 10;
    pthread_t file_tid;
    AOUT_FILE_QUEUE_CAP file_cap = { 0 };
    AOUT_FILE_ITEM file_buf = { 0 };

    aout_queue_create(cap);
    file_cap.queue_size = 10;
    aout_file_queue_create(file_cap);
    pthread_create(&tid, NULL, do_data, NULL);
    pthread_create(&file_tid, NULL, do_file_data, NULL);

    for (i = 0; i < 15; i++) {
        buf.type = AENC_G711u;
        snprintf(tmp, sizeof(tmp), "%d", i);
        buf.datalen = strlen(tmp) + 1;
        buf.data = tmp;
        aout_enqueue(buf);
        aout_status_talk_refresh();
        printf("HGH-TEST[%s %d]\n", __FUNCTION__, __LINE__);
    }
    for (i = 0; i < 10; i++) {
        snprintf(file_buf.filename, sizeof(file_buf.filename),
            "/test/audio-%d", i);
        aout_file_enqueue(file_buf);
        printf("HGH-TEST[%s %d]\n", __FUNCTION__, __LINE__);
    }
    while (1) {
        int speech_status = 0;
        int talk_status = 0;
        aout_status_get(&talk_status, &speech_status);
        printf("HGH-TEST[%s %d] talk: %d speech: %d\n",
            __FUNCTION__, __LINE__, talk_status, speech_status);
        usleep(100 * 1000);
    }
    aout_queue_destroy();

    return 0;
}
