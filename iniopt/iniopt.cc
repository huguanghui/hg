#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "base/def.h"
#include "base/str.h"
#include "base/fs.h"
#include "base/flag.h"
#include "base/log.h"
#include "iniparser.h"

DEF_string(o, "get", "operator suport[get|set]");
DEF_string(s, "null", "session");
DEF_string(v, "null", "value");

static int iniparser_save(dictionary *d, const char *inipath);

static void usage()
{
    COUT << "usage: iniopt [-o get|set] [-s session] [-v value]";
    
    return;
}

int main(int argc, char * argv[])
{
    int     status ;
    int rc;

    // 传参解析
    auto v = flag::init(argc, argv);
    log::init();

    if (v.empty()) {
        usage();
        return 0;
    }
    
    // 有效性检测

    dictionary *dict = iniparser_load("./custommode.ini");

    const char *value = iniparser_getstring(dict, "AudioIn:AudioVol", "abc");
    printf("value: %s\n", value);
    
    rc = iniparser_set(dict, "AudioIn:AudioVol", "123");

    iniparser_save(dict, "./custommode.ini");

    iniparser_freedict(dict);

    return status ;
}

static int iniparser_save(dictionary *d, const char *inipath)
{
    int ret = 0;
    FILE *fp = NULL;

    if ((NULL == d) || (NULL == inipath))
    {
        ret = -1;
        return ret;
    }

    fp = fopen(inipath, "w"); 
    if (NULL == fp)
    {
        ret = -2;
        return ret;
    }

    iniparser_dump_ini(d, fp);

    fclose(fp);

    return 0;
}
