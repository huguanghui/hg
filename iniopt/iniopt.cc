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

DEF_bool(boo, false, "bool flag");
DEF_string(abc, "abc", "aabbcc");

DEF_string(ao, "get", "operator suport[get|set]");
DEF_string(as, "null", "session");
DEF_string(av, "null", "value");

static int iniparser_save(dictionary *d, const char *inipath);

static void usage()
{
    COUT << "usage: iniopt [-o get|set] [-s session] [-v value]";
    
    return;
}

int main(int argc, char** argv)
{
    int status ;
    int rc;

    // 传参解析
    printf("%s:%d\n", __FUNCTION__, __LINE__);
    auto args = flag::init(argc, argv);

    COUT << "boo: " << FLG_boo;
    COUT << "abc: " << FLG_abc;
    
    return 0;
    log::init();

    printf("%s:%d\n", __FUNCTION__, __LINE__);

    if (args.empty()) {
        usage();
        return 0;
    }
    printf("%s:%d\n", __FUNCTION__, __LINE__);
    
    // 有效性检测
    COUT << "o: " << FLG_ao;
    COUT << "s: " << FLG_as;
    COUT << "v: " << FLG_av;
    printf("%s:%d\n", __FUNCTION__, __LINE__);

    return 0;

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
