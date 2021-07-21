#ifndef __AOUT_QUEUE_H__
#define __AOUT_QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 喇叭播报队列管理(单例) */

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
    char* data;
} AOUT_BUF;

int aout_queue_create(AOUT_CAP cap);

void aout_queue_destroy();

int aout_enqueue(AOUT_BUF s);

int aout_dequeue(AOUT_BUF* s);

/********************************************************************/
/* 语音播报队列(单例)管理 */

typedef struct _AOUT_FILE_ITEM {
    char filename[128];
} AOUT_FILE_ITEM;

typedef struct _AOUT_FILE_U_CAP {
    unsigned int queue_size;
} AOUT_FILE_QUEUE_CAP;

/**
 * @brief 管理语音播放文件的队列
 *
 * @Param cap
 *
 * @Returns
 */
int aout_file_queue_create(AOUT_FILE_QUEUE_CAP cap);

/**
 * @brief 销毁语音播报文件队列
 *
 * @Param handle
 */
void aout_file_queue_destroy();

/**
 * @brief 插入到播报队列
 *
 * @Param s
 *
 * @Returns
 */
int aout_file_enqueue(AOUT_FILE_ITEM s);

/**
 * @brief 从播报队列中取出播报文件
 *
 * @Param s
 *
 * @Returns
 */
int aout_file_dequeue(AOUT_FILE_ITEM* s);

/********************************************************************/
/* 对讲和语音播报状态维护 */
void aout_status_talk_refresh();
void aout_status_speech_refresh();
void aout_status_get(int* in_talk, int* in_speech);

#ifdef __cplusplus
}
#endif

#endif // !__AOUT_QUEUE_H__
