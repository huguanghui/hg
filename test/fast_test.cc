#include <iostream>
#include "base/def.h"
#include "base/fast.h"

int main(int argc, char *argv[])
{
    char tmp[512];
    int rc = 0;

    rc = fast::dtoa(1.12, tmp);
    printf("[%s %d]Tmp[%s]\n", __FUNCTION__, __LINE__, tmp);

    uint32 nm = 23;
    rc = fast::u32toh(nm, tmp);
    printf("[%s %d]Tmp[%s]\n", __FUNCTION__, __LINE__, tmp);

    int32 ab = 1234;
    rc = fast::i32toa(ab, tmp);
    printf("[%s %d]Tmp[%s]\n", __FUNCTION__, __LINE__, tmp);
    
    return 0;
}
