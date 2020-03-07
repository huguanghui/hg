#include <stdio.h>
#include <string.h>
#include "base64Lib.h"
#include "stdDes.h"

int main(int argc, char *argv[])
{
    char in[256] = {0};
    char out[256] = {0};
    char key[64] = {0};
    int nBinDataLen = 0;

    if (2 == argc)
    {
        char *param = argv[1];
		
        snprintf(key, sizeof(key), "123456789");
        printf("paran[%s]\n", param);
        nBinDataLen = base64Decode(param, (unsigned char*)in);
        
       int rc = StdDesEncryption((unsigned char*)in, (unsigned char*)out, ((nBinDataLen + 7)&0xfffffff8), (unsigned char*)key, DECRYPTION_MODE);  
		
        printf("rc [%d] out-[%s]\n", rc, out);
    }


    return 0;
}
