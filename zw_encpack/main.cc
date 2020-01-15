#include "base/def.h"
#include "base/str.h"
#include "base/fs.h"
#include "base/flag.h"
#include "base/log.h"

DEF_string(type, "TYPE_ALL", "boardtype");
DEF_string(uboot, "null", "uboot path");
DEF_string(kernel, "null", "kernel path");
DEF_string(app, "null", "app path");
DEF_string(web, "null", "web path");
DEF_string(conf, "null", "config path");
DEF_string(custom, "null", "custom path");
DEF_string(onvif, "null", "onvif path");
DEF_string(output, "null", "output path");

#define MAX_IMAGE_SIZE (7)
// 做包设备的 256 字节对齐
#define PACK_ALIGN (256)

static unsigned char MAGIC_BIN[8] = {'B', 'I', 'N', 'M', 'A', 'G', 'I', 'C'};
static unsigned char MAGIC_INFO[8] = {'I', 'N', 'F', 'M', 'A', 'G', 'I', 'C'};
static unsigned char MAGIC_IMG[8] = {'I', 'M', 'G', 'M', 'A', 'G', 'I', 'C'};

typedef struct tageImageInfo
{
    uint32  ImageType;
    uint32  ImageOffset;
    uint32  ImageLen;
    uint8   ImageCode[8];
}IMAGE_INFO;

static void usage()
{
    return ;
}

static void HexOutput(void *buf, size_t len)
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

static int parse_imageinfo_byfile(const char *file, int type, IMAGE_INFO *arry, uint32 *pos)
{
    if (NULL == file || *pos >= MAX_IMAGE_SIZE)
    {
        return -1;
    }

    if (fs::exists(file) && fs::fsize(file) > 8) {
        fs::file fo(file, 'r');
        uint32 cur_pos = *pos;
        uint32 image_paddlen = 0;
        uint32 image_offset = 0;

        fo.read(arry[cur_pos].ImageCode, 8);
        arry[cur_pos].ImageType = type;
        if (0 == cur_pos) {
            image_offset = PACK_ALIGN;
        } else {
            image_paddlen = arry[cur_pos-1].ImageLen % PACK_ALIGN;
            if (image_paddlen > 0) {
                image_paddlen = PACK_ALIGN - image_paddlen;
            }
            image_offset = arry[cur_pos-1].ImageOffset + arry[cur_pos-1].ImageLen + image_paddlen;
        }
        arry[cur_pos].ImageLen = fo.size();
        arry[cur_pos].ImageOffset = image_offset;
        cur_pos++;
        *pos = cur_pos;
    }
    return 0;
}

// 本文件使用256字节对齐
int main(int argc, char *argv[])
{
    uint32 image_num = 0;
    auto v = flag::init(argc, argv);
    log::init();

    fastring type(FLG_type);
    fastring output(FLG_output);
    fastring in_uboot(FLG_uboot);
    fastring in_kernel(FLG_kernel);
    fastring in_app(FLG_app);
    fastring in_web(FLG_web);
    fastring in_conf(FLG_conf);
    fastring in_custom(FLG_custom);
    fastring in_onvif(FLG_onvif);
    fs::file out_file;
    IMAGE_INFO image_info[MAX_IMAGE_SIZE];

    COUT << "type: " << type;
    COUT << "uboot: " << in_uboot;
    COUT << "kernel: " << in_kernel;
    COUT << "app: " << in_app;
    COUT << "web: " << in_web;
    COUT << "conf: " << in_conf;
    COUT << "custom: " << in_custom;
    COUT << "onvif: " << in_onvif;
    COUT << "output: " << output;

    if (in_uboot != "null") {
        parse_imageinfo_byfile(in_uboot.c_str(), 1, image_info, &image_num);
    }

    if (in_kernel != "null") {
        parse_imageinfo_byfile(in_kernel.c_str(), 2, image_info, &image_num);
    }

    if (in_app != "null") {
        parse_imageinfo_byfile(in_app.c_str(), 3, image_info, &image_num);
    }

    if (in_web != "null") {
        parse_imageinfo_byfile(in_web.c_str(), 4, image_info, &image_num);
    }

    if (in_custom != "null") {
        parse_imageinfo_byfile(in_custom.c_str(), 6, image_info, &image_num);
    }

    if (in_conf != "null") {
        parse_imageinfo_byfile(in_conf.c_str(), 5, image_info, &image_num);
    }

    if (in_onvif != "null") {
        parse_imageinfo_byfile(in_onvif.c_str(), 7, image_info, &image_num);
    }

    if (image_num <= 0) {
        usage();
        return 0;
    }

    out_file.open(output.c_str(), 'w');

    // header
    char buf_header[209] = {0};
    char less[256] = {0};
    fastring header(256);
    char buf[66] = {0};
    int padd_len = 0;
    int total_size = 0;
    int want_size = 0;
    int readed_size = 0;
    int left_size = 0;
    bool bwrite_flag = true;
    fastring write_file_name;
    fs::file write_file;

    memset(buf_header, 0, sizeof(buf_header));
    header.append(MAGIC_BIN, sizeof(MAGIC_BIN));
    header.append(MAGIC_INFO, sizeof(MAGIC_INFO));
    snprintf(buf, sizeof(buf), "[%s]", type.c_str());
    header.append(buf, strlen(buf));
    header.append(MAGIC_IMG, sizeof(MAGIC_IMG));
    header.append(&image_num, 4);
    for (int i = 0; i < image_num; ++i) {
        header.append(&image_info[i], sizeof(IMAGE_INFO));
    }
    memcpy(buf_header, header.data(), header.size());
    out_file.write(buf_header, sizeof(buf_header));
    // 补上对齐
    total_size = sizeof(buf_header);
    padd_len = total_size % PACK_ALIGN;
    if (padd_len > 0) {
        padd_len = PACK_ALIGN - padd_len;
        memset(less, 0xff, sizeof(less));
        out_file.write(less, padd_len);
    }

    for (int i = 0; i < image_num; ++i) {
        bwrite_flag = true; 
        switch (image_info[i].ImageType) {
            case 1:
                write_file_name = in_uboot;
                break;
            case 2:
                write_file_name = in_kernel;
                break;
            case 3:
                write_file_name = in_app;
                break;
            case 4:
                write_file_name = in_web;
                break;
            case 5:
                write_file_name = in_conf;
                break;
            case 6:
                write_file_name = in_custom;
                break;
            case 7:
                write_file_name = in_onvif;
                break;
            default:
                {
                    COUT << "Unknow Type!";            
                    bwrite_flag = false;
                }
        }
        if (bwrite_flag) {
            write_file.open(write_file_name.c_str(), 'r');
    
            total_size = image_info[i].ImageLen;
            left_size = total_size;
            while (left_size > 0) {
               if (left_size >= sizeof(less)) {
                    want_size = sizeof(less);
               } else {
                    want_size = left_size;
               }
               readed_size = write_file.read(less, want_size);
               left_size -= readed_size;
               out_file.write(less, readed_size);
            }
            // 补齐
            padd_len = total_size % PACK_ALIGN;
            if (padd_len > 0) {
                padd_len = PACK_ALIGN - padd_len;
                memset(less, 0xff, sizeof(less));
                out_file.write(less, padd_len);
            }
        }
    }
    
    return 0;
}
