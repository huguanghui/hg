#include "base/def.h"
#include "base/str.h"
#include "base/fs.h"
#include "base/flag.h"
#include "base/log.h"

#define BOARD_TYPE_MAX_LENGTH 64


static char *update_name[] = {
    "u-boot.bin",
    "uImage",
    "romfs.img",
    "web.img",
    "cfg.tar.gz",
    "custom.img",
    "onvif.img",
};

typedef struct tageImageInfo
{
    uint32  ImageType;
    uint32  ImageOffset;
    uint32  ImageLen;
    uint8   ImageCode[8];
}IMAGE_INFO;

typedef struct tagImageInfoHeader
{       
    uint8           ImageInfoMagicCode[8]; 
    uint32          ImageNum;
    IMAGE_INFO      img_inf[7];                                             
}ImageInfoHeader;

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
    log::init();

    if (v.empty() || v.size() < 1) {
        usage();
        return 0;
    }

    fastring file(v[0]);

    bool bexists = fs::exists(file.c_str());
    if (bexists) {
        char buf[32];
        ImageInfoHeader  header;

        fs::file bin_file(file.c_str(), 'r');
        // checkMagicCode
        bin_file.seek(0);
        memset(buf, 0, sizeof(buf));
        bin_file.read(buf, 8);
        COUT << buf;
        memset(buf, 0, sizeof(buf));
        bin_file.read(buf, 8);
        COUT << buf;
        while (1) {
            memset(buf, 0, sizeof(buf));
            bin_file.read(buf, 1);
            //COUT << buf;
            if (buf[0] != '[')
            {
                break;
            }
            char boardtype[72] = {0};
            for (int i = 0; i < BOARD_TYPE_MAX_LENGTH; ++i) {
                memset(buf, 0, sizeof(buf));
                bin_file.read(buf, 1);
                if (buf[0] != ']') {
                    boardtype[i] = buf[0];
                } else {
                    COUT << i;
                    break;
                }
            }
            COUT << boardtype;
        }
        char imageInfoMagicCode[9] = {0};
        imageInfoMagicCode[0] = buf[0];
        bin_file.read(&imageInfoMagicCode[1], 7);
        COUT << imageInfoMagicCode;
        memcpy(header.ImageInfoMagicCode, imageInfoMagicCode, sizeof(header.ImageInfoMagicCode));
        // getImageInfo
        uint32 imageNum = 0;
        IMAGE_INFO imageInfo;

        bin_file.read(&imageNum, 4);
        header.ImageNum = imageNum;
        COUT << imageNum;
        for (int i = 0; i < imageNum; ++i) {
           bin_file.read(&imageInfo, sizeof(imageInfo)); 
           COUT << "IDX: " << i;
           COUT << "IMAGE_TYPE: " << imageInfo.ImageType;
           HexOutput(imageInfo.ImageCode, sizeof(imageInfo.ImageCode));
           COUT << "IMAGE_LEN: " << imageInfo.ImageLen;
           COUT << "IMAGE_OFFSET: " << imageInfo.ImageOffset;
           memcpy(&header.img_inf[imageInfo.ImageType-1], &imageInfo, sizeof(IMAGE_INFO));
        }

       // char testa[1024] = {0};
       // bin_file.seek(0);
       // memset(testa, 0, sizeof(testa));
       // bin_file.read(testa, 256);
       // HexOutput(testa, 256);

       // return 0;

        // parseImageInfo
        for (int i = 0; i < imageNum; ++i) {
            // check maginc
            bin_file.seek(header.img_inf[i].ImageOffset);
            memset(buf, 0, sizeof(buf));
            bin_file.read(buf, 8);
            if (0 == strncmp((char*)header.img_inf[i].ImageCode, buf, 8))
            {
                HexOutput(header.img_inf[i].ImageCode, 8);
               COUT << i;
               fs::file out(update_name[i], 'w');
               int total_size = header.img_inf[i].ImageLen;
               int pos_size = 0;
               int read_len = 0;
               int readed_len = 0;
               int left_len = total_size;
               char big_buf[128*1024];
               
               bin_file.seek(header.img_inf[i].ImageOffset);
               while (pos_size < total_size)
               {
                   if (left_len >= sizeof(big_buf))
                   {
                       read_len = sizeof(big_buf);
                   }
                   else
                   {
                        read_len = left_len;
                   }
                   readed_len = (int)bin_file.read(big_buf, read_len); 
                   if (readed_len > 0)
                   {
                        out.write(big_buf, readed_len);
                   }
                   pos_size += readed_len;
                   left_len -= readed_len;
               }
            }
        }
        char test[1024] = {0};

        bin_file.seek(224708);
        memset(test, 0, sizeof(test));
        bin_file.read(test, 68);
        HexOutput(test, 68);

        bin_file.seek(2083619);
        memset(test, 0, sizeof(test));
        bin_file.read(test, 229);
        HexOutput(test, 229);

        bin_file.seek(7682265);
        memset(test, 0, sizeof(test));
        bin_file.read(test, 47);
        HexOutput(test, 47);

        bin_file.seek(0);
        memset(test, 0, sizeof(test));
        bin_file.read(test, 256);
        HexOutput(test, 256);
    }

    return 0;
}
