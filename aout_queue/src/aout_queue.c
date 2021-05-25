#include "aout_queue.h"
#include <pthread.h>
#include <stdio.h>

typedef struct _aout_info {
  int inited;
  unsigned int queue_size;
  unsigned int max_datalen;
  AOUT_BUF *aout_vec;
} AOUT_INFO;

static AOUT_INFO g_config = {0};

static int head = 0;
static int tail = 0;
static pthread_mutex_t aout_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t aout_notempty = PTHREAD_COND_INITIALIZER;

int aout_queue_create(AOUT_CAP cap) {
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

void aout_queue_destroy() {
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

int aout_enqueue(AOUT_BUF s) {
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

int aout_dequeue(AOUT_BUF *s)
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
