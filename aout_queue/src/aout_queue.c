#include <stdio.h>
#include <pthread.h>
#include "aout_queue.h"

#define MAX_AOUT_QUEUE (10)

static AOUT_BUF aout_queue[MAX_AOUT_QUEUE];
static int head = 0;
static int tail = 0;
static pthread_mutex_t aout_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t aout_notempty = PTHREAD_COND_INITIALIZER;
static pthread_cond_t aout_notfull = PTHREAD_COND_INITIALIZER;

int aout_queue_init()
{
    return 0;
}

void aout_queue_deinit()
{
    return;
}

int aout_enqueue(AOUT_BUF s)
{
    int next;
    pthread_mutex_lock(&aout_lock);
    next =  (tail + 1) % MAX_AOUT_QUEUE;
    if (next == head) {
        pthread_cond_wait(&aout_notfull, &aout_lock);
    }
    aout_queue[tail] = s;
    tail = next;
    pthread_cond_signal(&aout_notempty);
    pthread_mutex_unlock(&aout_lock);
    return 0;
}

AOUT_BUF dequeue()
{
    AOUT_BUF s;
    pthread_mutex_lock(&aout_lock);
    if (head == tail) {
        pthread_cond_wait(&aout_notempty, &aout_lock);
    }
    s = aout_queue[head];
    head = (head+1) % MAX_AOUT_QUEUE;
    pthread_cond_signal(&aout_notfull);
    pthread_mutex_unlock(&aout_lock);
    return aout_queue[0];
}
