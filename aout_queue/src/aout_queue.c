#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

#include "aout_queue.h"

#ifndef MS
#define MS(x) ((x).tv_sec * 1000 + (x).tv_nsec / 1000000)
#endif

typedef struct _aout_info {
    int inited;
    unsigned int queue_size;
    unsigned int max_datalen;
    AOUT_BUF* aout_vec;
} AOUT_INFO;

static AOUT_INFO g_config = { 0 };

static int head = 0;
static int tail = 0;
static pthread_mutex_t aout_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t aout_notempty = PTHREAD_COND_INITIALIZER;

int aout_queue_create(AOUT_CAP cap)
{
    pthread_mutex_lock(&aout_lock);
    if (0 == g_config.inited) {
        int i = 0, len = 0;
        g_config.queue_size = cap.queue_size;
        g_config.max_datalen = cap.max_datalen;

        len = sizeof(AOUT_BUF) * cap.queue_size;
        g_config.aout_vec = malloc(len);
        memset(g_config.aout_vec, 0, len);
        for (i = 0; i < cap.queue_size; i++) {
            g_config.aout_vec[i].data = malloc(g_config.max_datalen);
        }
        g_config.inited = 1;
    }
    pthread_mutex_unlock(&aout_lock);
    return 0;
}

void aout_queue_destroy()
{
    pthread_mutex_lock(&aout_lock);
    if (1 == g_config.inited) {
        int i = 0;
        if (NULL != g_config.aout_vec) {
            for (i = 0; i < g_config.queue_size; i++) {
                if (g_config.aout_vec[i].data != NULL) {
                    free(g_config.aout_vec[i].data);
                    g_config.aout_vec[i].data = NULL;
                }
            }
            free(g_config.aout_vec);
            g_config.aout_vec = NULL;
        }
        g_config.inited = 0;
    }
    pthread_mutex_unlock(&aout_lock);
    return;
}

int aout_enqueue(AOUT_BUF s)
{
    pthread_mutex_lock(&aout_lock);
    int next;
    if (0 == g_config.inited) {
        pthread_mutex_unlock(&aout_lock);
        return -1;
    }
    next = (tail + 1) % g_config.queue_size;
    if (next == head) {
        printf("[%s %d] queue is fulled!\n", __FUNCTION__, __LINE__);
        pthread_mutex_unlock(&aout_lock);
        return -2;
    }
    if (s.datalen > g_config.max_datalen) {
        pthread_mutex_unlock(&aout_lock);
        return -3;
    }
    g_config.aout_vec[tail].type = s.type;
    g_config.aout_vec[tail].datalen = s.datalen;
    memcpy(g_config.aout_vec[tail].data, s.data, s.datalen);
    tail = next;
    pthread_cond_signal(&aout_notempty);
    pthread_mutex_unlock(&aout_lock);
    return 0;
}

int aout_dequeue(AOUT_BUF* s)
{
    pthread_mutex_lock(&aout_lock);
    if (0 == g_config.inited) {
        printf("HGH-TEST[%s %d]\n", __FUNCTION__, __LINE__);
        pthread_mutex_unlock(&aout_lock);
        return -1;
    }
    if (head == tail) {
        pthread_cond_wait(&aout_notempty, &aout_lock);
    }
    if (NULL != s) {
        memcpy(s, &g_config.aout_vec[head], sizeof(AOUT_BUF));
    }
    head = (head + 1) % g_config.queue_size;
    pthread_mutex_unlock(&aout_lock);
    return 0;
}

typedef struct _aout_file_info {
    int inited;
    unsigned int queue_size;
    AOUT_FILE_ITEM* aout_file_vec;
} AOUT_FILE_INFO;

static AOUT_FILE_INFO g_aout_file_config = { 0 };

static int aout_file_head = 0;
static int aout_file_tail = 0;
static pthread_mutex_t aout_file_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t aout_file_notempty = PTHREAD_COND_INITIALIZER;

int aout_file_queue_create(AOUT_FILE_QUEUE_CAP cap)
{
    pthread_mutex_lock(&aout_file_lock);
    if (0 == g_aout_file_config.inited) {
        int i = 0, len = 0;
        g_aout_file_config.queue_size = cap.queue_size;

        len = sizeof(AOUT_FILE_ITEM) * cap.queue_size;
        g_aout_file_config.aout_file_vec = malloc(len);
        memset(g_aout_file_config.aout_file_vec, 0, len);
        g_aout_file_config.inited = 1;
    }
    pthread_mutex_unlock(&aout_file_lock);
    return 0;
}

void aout_file_queue_destroy()
{
    pthread_mutex_lock(&aout_file_lock);
    if (1 == g_aout_file_config.inited) {
        int i = 0;
        if (NULL != g_aout_file_config.aout_file_vec) {
            free(g_aout_file_config.aout_file_vec);
            g_aout_file_config.aout_file_vec = NULL;
        }
        g_aout_file_config.inited = 0;
    }
    pthread_mutex_unlock(&aout_file_lock);
    return;
}

int aout_file_enqueue(AOUT_FILE_ITEM s)
{
    pthread_mutex_lock(&aout_file_lock);
    int next;
    if (0 == g_aout_file_config.inited) {
        pthread_mutex_unlock(&aout_file_lock);
        return -1;
    }
    next = (aout_file_tail + 1) % g_aout_file_config.queue_size;
    if (next == aout_file_head) {
        printf("[%s %d] queue is fulled!\n", __FUNCTION__, __LINE__);
        pthread_mutex_unlock(&aout_file_lock);
        return -2;
    }
    g_aout_file_config.aout_file_vec[aout_file_tail] = s;
    aout_file_tail = next;
    pthread_cond_signal(&aout_file_notempty);
    pthread_mutex_unlock(&aout_file_lock);
    return 0;
}

int aout_file_dequeue(AOUT_FILE_ITEM* s)
{
    pthread_mutex_lock(&aout_file_lock);
    if (0 == g_aout_file_config.inited) {
        printf("HGH-TEST[%s %d]\n", __FUNCTION__, __LINE__);
        pthread_mutex_unlock(&aout_file_lock);
        return -1;
    }
    if (aout_file_head == aout_file_tail) {
        pthread_cond_wait(&aout_file_notempty, &aout_file_lock);
    }
    if (NULL != s) {
        memcpy(s, &g_aout_file_config.aout_file_vec[aout_file_head], sizeof(AOUT_FILE_ITEM));
    }
    aout_file_head = (aout_file_head + 1) % g_aout_file_config.queue_size;
    pthread_mutex_unlock(&aout_file_lock);
    return 0;
}

static int g_lasttime_talkdata_ms = 0;
static int g_lasttime_speechdata_ms = 0;
static pthread_mutex_t g_aout_status_lock = PTHREAD_MUTEX_INITIALIZER;

void aout_status_talk_refresh()
{
    pthread_mutex_lock(&g_aout_status_lock);
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    g_lasttime_talkdata_ms = MS(tp);
    pthread_mutex_unlock(&g_aout_status_lock);
    return;
}

void aout_status_speech_refresh()
{
    pthread_mutex_lock(&g_aout_status_lock);
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    g_lasttime_speechdata_ms = MS(tp);
    pthread_mutex_unlock(&g_aout_status_lock);
    return;
}

void aout_status_get(int* in_talk, int* in_speech)
{
    struct timespec tp;
    int status = 0;
    int delta = 0;
    int cur_time_ms = 0;

    pthread_mutex_lock(&g_aout_status_lock);
    clock_gettime(CLOCK_MONOTONIC, &tp);
    cur_time_ms = MS(tp);
    if (NULL != in_talk) {
        status = 0;
        delta = cur_time_ms - g_lasttime_talkdata_ms;
        if (delta < 1000) {
            status = 1;
        }
        *in_talk = status;
    }
    if (NULL != in_speech) {
        status = 0;
        delta = cur_time_ms - g_lasttime_speechdata_ms;
        if (delta < 1000) {
            status = 1;
        }
        *in_speech = status;
    }
    pthread_mutex_unlock(&g_aout_status_lock);
    return;
}
