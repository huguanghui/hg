#include "shared_buff.h"

SharedBuffer::SharedBuffer(SharedBufferType type, SharedBufferMode mode):
    m_shmAddr(nullptr),
    m_rdOffset(sizeof(SharedBufferHeadSt)),
    m_wrOffset(sizeof(SharedBufferHeadSt)),
    m_shmId(-1),
    m_semId(-1),
    m_shmKey(-1),
    m_semKey(-1),
    m_shmType(type),
    m_shmMode(mode)
{    
    m_shmKey = ftok("/tmp", SHM_PROJ_ID);
    if(m_shmKey < 0){
        printf("SharedBuffer: constructor: m_shmKey failed!\n");
        return;
    }
    printf("m_shmKey[%x]\n", m_shmKey);

    m_semKey = ftok("/tmp", SEM_PROJ_ID);
    if(m_semKey < 0){
        printf("SharedBuffer: constructor: m_semKey failed!\n");
        return;
    }
    printf("m_semKey[%x]\n", m_semKey);

    int ret = shmAttach();
    if(ret < 0){
        printf("SharedBuffer: constructor: shmAttach failed!\n");
        return;
    }
}

SharedBuffer::~SharedBuffer()
{
    int ret = shmDetach();
    if(ret < 0){
        printf("SharedBuffer: destructor: shmDetach failed!\n");
        return;
    }
}

int SharedBuffer::shmAttach()
{
    int shmFlag = 0;
    shmFlag = IPC_CREAT | 0660;   
    int len = sizeof(SharedBufferHeadSt) + SHM_PACK_LEN*SHM_SIZE;
    m_shmId = shmget(m_shmKey, len, shmFlag);
    if(m_shmId < 0){
        perror("SharedBuffer::shmAttach: shmget");
        return -1;
    }

    m_shmAddr = shmat(m_shmId, NULL, 0);
    if(m_shmAddr == (void *)-1){
        perror("SharedBuffer::shmAttach: shmat");
        return -1;
    }

    if(m_shmType == TypeProducer){
        ((SharedBufferHeadSt *)(m_shmAddr))->offset = sizeof(SharedBufferHeadSt);
        ((SharedBufferHeadSt *)(m_shmAddr))->queueLen = 0;
    }

    m_semId = semget(m_semKey, 1, IPC_CREAT | 0660);
    if(m_semId < 0){
        perror("SharedBuffer::shmAttach: semget");
        return -1;
    }

    if(m_shmType == TypeProducer){
        int ret = semctl(m_semId, 0, SETVAL, 1);
        if(ret < 0){
            perror("SharedBuffer::shmAttach: semctl");
            return -1;
        }
    }
    return 0;
}

int SharedBuffer::shmDetach()
{
    int ret = 0;
    if(m_shmType == TypeProducer){
        ret = semctl(m_semId, 1, IPC_RMID);
        if(ret < 0){
            perror("SharedBuffer::shmDetach: semctl");
            return -1;
        }
    }

    ret = shmdt(m_shmAddr);
    m_shmAddr = nullptr;
    if(ret < 0){
        perror("SharedBuffer::shmDetach: shmdt");
        return -1;
    }

    if(m_shmType == TypeProducer){
        ret = shmctl(m_shmId, IPC_RMID, NULL);
        if(ret < 0){
            perror("SharedBuffer::shmDetach: shmctl");
            return -1;
        }
    }

    return 0;
}

int SharedBuffer::sem_p()
{
    short flag = 0;
    if(m_shmMode == ModeUnblock)
        flag = IPC_NOWAIT | SEM_UNDO;
    else
        flag = SEM_UNDO;

    struct sembuf buf = {0, -1, flag};
    int ret = semop(m_semId, &buf, 1);
    if(ret < 0){
        if(errno != EAGAIN)
            perror("semop");
        return -1;
    }
    return 0;
}

int SharedBuffer::sem_v()
{
    short flag = 0;
    if(m_shmMode == ModeUnblock)
        flag = IPC_NOWAIT | SEM_UNDO;
    else
        flag = SEM_UNDO;

    struct sembuf buf = {0, 1, flag};
    int ret = semop(m_semId, &buf, 1);
    if(ret < 0){
        if(errno != EAGAIN)
            perror("semop");
        return -1;
    }
    return 0;
}

int SharedBuffer::put(const void *buf, const uint len)
{
    if(m_shmType != TypeProducer){
        printf("SharedBuffer::put: type invalid!\n");
        return -1;
    }

    if(len > SHM_PACK_LEN || buf == nullptr){
        printf("SharedBuffer::put: invalid parameter!\n");
        return -1;
    }

    int ret = sem_p();
    if(ret < 0)
        return -1;

    m_wrOffset = ((SharedBufferHeadSt *)(m_shmAddr))->offset;
    char *wrPtr = (char*)m_shmAddr + m_wrOffset;
    bzero(wrPtr, SHM_PACK_LEN);
    memcpy(wrPtr, buf, len);
    if(m_wrOffset + SHM_PACK_LEN == (sizeof(SharedBufferHeadSt) + SHM_PACK_LEN*SHM_SIZE)){
        m_wrOffset = sizeof(SharedBufferHeadSt);
        //printf("SharedBuffer::put: overload!\n");
    }else{
        m_wrOffset += SHM_PACK_LEN;
    }
    ((SharedBufferHeadSt *)(m_shmAddr))->offset = m_wrOffset;

    ret = sem_v();
    if(ret < 0)
        return -1;

    return len;
}

int SharedBuffer::get(void *buf, const uint len)
{
    if(m_shmType != TypeConsumer){
        printf("SharedBuffer::get: type invalid!\n");
        return -1;
    }

    if(len < SHM_PACK_LEN || buf == nullptr){
        printf("SharedBuffer::get: invalid parameter!\n");
        return -1;
    }

    int ret = 0;
    if(sem_p() < 0)
        return -1;

    if(m_rdOffset == ((SharedBufferHeadSt *)(m_shmAddr))->offset){
        //printf("SharedBuffer::get: read empty!\n");
        ret = 0;
    }else{
        char *rdPtr = (char*)m_shmAddr + m_rdOffset;
        memcpy(buf, rdPtr, SHM_PACK_LEN);
        ret = SHM_PACK_LEN;
        if(m_rdOffset + SHM_PACK_LEN == (sizeof(SharedBufferHeadSt) + SHM_PACK_LEN*SHM_SIZE)){
            m_rdOffset = sizeof(SharedBufferHeadSt);
        }else{
            m_rdOffset += SHM_PACK_LEN;
        }
    }

    if(sem_v() < 0)
        return -1;

    return ret;
}
