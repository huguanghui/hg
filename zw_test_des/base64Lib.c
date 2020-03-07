#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "base64Lib.h"

static const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char * base64Encode( const unsigned char * bindata, char * base64, int binlength )
{
    if (!bindata || !base64)
    {
        fprintf(stderr, "NULL POINTER EXCEPTION!\n");
        return NULL;
    }

    int i = 0;
    int j = 0;
    unsigned char current = '\0';

    for ( i = 0, j = 0 ; i < binlength ; i += 3 )
    {
        current = (bindata[i] >> 2) ;
        current &= (unsigned char)0x3F;
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i] << 4 ) ) & ( (unsigned char)0x30 ) ;
        if ( i + 1 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+1] >> 4) ) & ( (unsigned char) 0x0F );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i+1] << 2) ) & ( (unsigned char)0x3C ) ;
        if ( i + 2 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+2] >> 6) ) & ( (unsigned char) 0x03 );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)bindata[i+2] ) & ( (unsigned char)0x3F ) ;
        base64[j++] = base64char[(int)current];
    }
    base64[j] = '\0';

    return base64;
}

//返回解码后数据长度
int base64Decode( const char * base64, unsigned char * bindata )
{
    if (!base64 || !bindata)
    {
        fprintf(stderr, "NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int i = 0;
    int j = 0;
    unsigned char k = '\0';
    unsigned char temp[4] = {0};

    for ( i = 0, j = 0; base64[i] != '\0' ; i += 4 )
    {
        memset( temp, 0xFF, sizeof(temp) );
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i] )
            {
                temp[0]= k;
            }
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+1] )
            {
                temp[1]= k;
            }
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+2] )
            {
                temp[2]= k;
            }
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+3] )
            {
                temp[3]= k;
            }
        }

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2))&0xFC)) |
                ((unsigned char)((unsigned char)(temp[1]>>4)&0x03));
        if ( base64[i+2] == '=' )
        {
            break;
        }

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4))&0xF0)) |
                ((unsigned char)((unsigned char)(temp[2]>>2)&0x0F));
        if ( base64[i+3] == '=' )
        {
            break;
        }

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6))&0xF0)) |
                ((unsigned char)(temp[3]&0x3F));
    }

    return j;
}

#if 0
int main(int argc, const char *argv[])
{
    char tmp_str[256] = {0};
    char *src_str = "http://192.168.2.29/tool/webapi/public/pn_raise_event&a123245";
    char *dec_str = "aHR0cDovLzE5Mi4xNjguMi4yOS90b29sL3dlYmFwaS9wdWJsaWMvcG5fcmFpc2VfZXZlbnQmYTEyMzI0NQ==";
    
    memset(tmp_str, 0, sizeof(tmp_str));
    base64_encode(src_str, tmp_str, strlen(src_str));
    
    printf("tmp_str = %s\n", tmp_str);
    
    memset(tmp_str, 0, sizeof(tmp_str));
    base64_decode(dec_str, tmp_str);
    
    printf("tmp_str = %s\n", tmp_str);

    return 0;
}
#endif

