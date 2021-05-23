#ifndef __GH_QUEUE_H__
#define __GH_QUEUE_H__

typedef struct Node stNode;
typedef struct Queue stQueue;

struct Node
{
    int idx;
    char name[32];
};

struct Queue
{
    int size; 
    int cur;
    stNode *buf;
};


/* 创建一个队列
 */
stQueue* queue_create(int size);

/* 销毁一个队列
 */
void queue_release(stQueue *q);


/* 向队列中插入一个数据
 */
int queue_put(stQueue *q, const char *data);

/* 向队列中获取一个数据
 */
const char *queue_get(stQueue *q);

#endif // !__GH_QUEUE_H__
