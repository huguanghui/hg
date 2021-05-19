#include "base/def.h"
#include "base/str.h"
#include "base/fs.h"
#include "base/flag.h"
#include "base/log.h"

/* 文件头信息 */
typedef struct tagMP4NewFileHeader
{
    unsigned short  FileStartCode;          
    unsigned short  FileID;
    unsigned short  FileHeadLen;
    unsigned char   Version;
    unsigned short  DataFlag;         
    unsigned short  dwImageWidth;
    unsigned short  dwImageHight;
}__attribute__((packed)) MPEG4FILEHEADER;

typedef struct tagH264DataPacketHeader
{
    unsigned int        uDataPacketStartCode; //0xB7010000
    unsigned int        uFrameNo;
    unsigned char       byFrameType;
    unsigned char       byLoadType;
    unsigned int        uShowTime;
    unsigned long       u32RealTimeSec;
    unsigned long       u32RealTimeUSec;
    unsigned short      uReserved;
    unsigned int        uOverloadLen;
}__attribute__((packed)) H264DATAPACKETHEADER, *PH264DATAPACKETHEADER;

static void usage()
{
    return;
}

void HexOutput(void *buf, size_t len)
{
    char *b = (char*)buf;
    int numRow = 0;
    int numCol = 0;

    printf("Hen[%p]Len[%d]:\n", buf, len);
    size_t i = 0;
    for(i=0; i<len; i=i+16)
    {
        printf("%p|", b+i);
        numCol = (len-i>=16) ? 16 : (len-i);
        int j = 0;
        for (j = 0; j < numCol; j++)
        {
            unsigned char c = b[i+j]; // must use unsigned char to print >128 value
            if( c < 16)
            {
                printf(" 0%x", c);
            }
            else
            {
                printf(" %x", c);
            }
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    auto v = flag::init(argc, argv);
    size_t sz = 0;
    H264DATAPACKETHEADER *pack_head = NULL;

    log::init();

    if (v.empty() || v.size() < 1) {
        usage();
        return 0;
    }

    fastring file(v[0]);

    bool bexists = fs::exists(file.c_str());
    if (bexists) {
        char buf[1024];

        fs::file bin_file(file.c_str(), 'r');
        // checkMagicCode
        bin_file.seek(0);
        memset(buf, 0, sizeof(buf));
        bin_file.read(buf, 19);
        /* HexOutput(buf, 19); */
        while (1) {
            sz = bin_file.read(buf, sizeof(H264DATAPACKETHEADER));
            if (sz <= 0) {
                break;
            }
            pack_head = (H264DATAPACKETHEADER)pack_head;
        }
    }

    return 0;
}
