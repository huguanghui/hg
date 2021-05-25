#ifndef __AOUT_QUEUE_H__
#define __AOUT_QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  AENC_PCM,
  AENC_G711u,
  AENC_G726,
} E_AENC_TYPE;

typedef struct _AOUT_CAP {
  unsigned int queue_size;
  unsigned int max_datalen;
} AOUT_CAP;

typedef struct _AOUT_BUF {
  E_AENC_TYPE type;
  unsigned int datalen;
  char *data;
} AOUT_BUF;

int aout_queue_create(AOUT_CAP cap);

void aout_queue_destroy();

int aout_enqueue(AOUT_BUF s);

int aout_dequeue(AOUT_BUF *s);

#ifdef __cplusplus
}
#endif

#endif // !__AOUT_QUEUE_H__
