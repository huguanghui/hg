#include "aout_queue.h"
#include <pthread.h>
#include <stdio.h>

typedef struct _aout_info {
  int inited;
  int head;
  int tail;
  unsigned int queue_size;
  unsigned int max_datalen;
  AOUT_BUF *aout_vec;
  pthread_mutex_t mtx;
  pthread_cond_t cond_notempty;
  pthread_cond_t cond_notfull;
} AOUT_INFO;

static AOUT_INFO g_config = {0};

static int g_inited = 0;
static AOUT_BUF *g_aout_queue = NULL;
static int head = 0;
static int tail = 0;
static pthread_mutex_t aout_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t aout_notempty = PTHREAD_COND_INITIALIZER;
static pthread_cond_t aout_notfull = PTHREAD_COND_INITIALIZER;

int aout_queue_create(AOUT_CAP cap) {
  if (0 == g_config.inited) {
    int i = 0, len = 0;

    pthread_mutex_init(&g_config.mtx, NULL);
    pthread_mutex_lock(&g_config.mtx);
    g_config.head = 0;
    g_config.tail = 0;
    g_config.queue_size = cap.queue_size;
    g_config.max_datalen = cap.max_datalen;
    pthread_cond_init(&g_config.cond_notempty, NULL);
    pthread_cond_init(&g_config.cond_notfull, NULL);

    len = sizeof(AOUT_BUF) * cap.queue_size;
    g_config.aout_vec = malloc(len);
    memset(g_config.aout_vec, 0, len);
    for (i = 0; i < cap.queue_size; i++) {
      g_config.aout_vec[0].data = malloc(g_config.max_datalen);
    }
    g_config.inited = 1;
    pthread_mutex_unlock(&g_config.mtx);
  }
  return 0;
}

void aout_queue_destroy() {
  if (1 == g_config.inited) {
    int i = 0;
  }
  return;
}

int aout_enqueue(AOUT_BUF s) {
  int next;
  pthread_mutex_lock(&aout_lock);
  next = (tail + 1) % MAX_AOUT_QUEUE;
  if (next == head) {
    pthread_cond_wait(&aout_notfull, &aout_lock);
  }
  aout_queue[tail] = s;
  tail = next;
  pthread_cond_signal(&aout_notempty);
  pthread_mutex_unlock(&aout_lock);
  return 0;
}

AOUT_BUF dequeue() {
  AOUT_BUF s;
  pthread_mutex_lock(&aout_lock);
  if (head == tail) {
    pthread_cond_wait(&aout_notempty, &aout_lock);
  }
  s = aout_queue[head];
  head = (head + 1) % MAX_AOUT_QUEUE;
  pthread_cond_signal(&aout_notfull);
  pthread_mutex_unlock(&aout_lock);
  return aout_queue[0];
}
