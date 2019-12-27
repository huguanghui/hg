#include <stdio.h>
#include "base/random.h"

int main(int argc, char *argv[])
{
    uint32_t val = 0;

    Random rd(14);    
    
    for (int i = 0; i < 10; i++)
    {
        val = rd.next();
        printf("[%s %d] idx[%d] val[%d]\n", __FUNCTION__, __LINE__, i, val);
    }
    
    return 0;
}
