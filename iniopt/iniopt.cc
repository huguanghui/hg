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

static int iniparser_save(dictionary *d, const char *inipath);
static int handler_get(std::vector<fastring> v);
static int handler_set(std::vector<fastring> v);
static int handler_check(std::vector<fastring> v);
static int handler_merge(std::vector<fastring> v);
static int handler_dbg(std::vector<fastring> v);

typedef int (*opt_callback)(std::vector<fastring> v);

struct OptHandler
{
    char opt[8];
    opt_callback handler;
};
static void usage()
{
    COUT << "usage:\n"
         << "    " << "iniopt [file] get [session]               get the value of [session] from [file]\n"
         << "    " << "iniopt [file] set [session] [value]       get the value of [session] from [file]\n"
         << "    " << "iniopt [file] check                       check the valid of ini[file]\n"
         << "    " << "iniopt [file] merge [subfile]             merge [subfile] to [file]";
    
    return;
}

int main(int argc, char** argv)
{
    int status ;
    int rc;

    // 传参解析
    auto v = flag::init(argc, argv);
    log::init();

    if (v.empty() || v.size() < 2) {
        usage();
        return 0;
    }
    
    // 有效性检测
    //for (size_t i = 0; i < v.size(); ++i)
    //{
    //    COUT << "Idx[" << i << "]: " << v[i].c_str();
    //}
    
    // v[0] - file
    fastring file(v[0]);
    // v[1] - operator
    fastring opt(v[1]);
    
    if (!fs::exists(file.c_str()))
    {
        COUT << "file not exists! file: " << file;
        return 0;
    }

    struct OptHandler optmap[] = {
        {"get",     handler_get},
        {"set",     handler_set},
        {"check",   handler_check},
        {"merge",   handler_merge},
        //{"dbg",     handler_dbg},
    };

    for (size_t i = 0; i < sizeof(optmap)/sizeof(optmap[0]); ++i)
    {
        if (opt == optmap[i].opt)
        {
            optmap[i].handler(v);
        }
    }

    

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

int handler_get(std::vector<fastring> v)
{
    fastring file(v[0]);
    fastring session(v[2]);

    dictionary *dict = iniparser_load(file.c_str());
    if (NULL == dict)
    {
        return 0;
    }
    const char *val = iniparser_getstring(dict, session.c_str(), "");
    COUT << val;
    iniparser_freedict(dict);
   
    return 0;
}

int handler_set(std::vector<fastring> v)
{
    fastring file(v[0]);
    fastring session(v[2]);
    fastring value(v[3]);

    dictionary *dict = iniparser_load(file.c_str());
    if (NULL == dict)
    {
        return 0;
    }
    int rc = iniparser_set(dict, session.c_str(), value.c_str());
    iniparser_save(dict, file.c_str());
    iniparser_freedict(dict);
    return 0;
}

int handler_check(std::vector<fastring> v)
{
    fastring file(v[0]);
    
    dictionary *dict = iniparser_load(file.c_str());
    if (NULL == dict)
    {
        return 0;
    }
    iniparser_freedict(dict);

    return 0;
}

int handler_merge(std::vector<fastring> v)
{
    fastring file(v[0]);
    fastring subfile(v[2]);

    dictionary *dict = iniparser_load(file.c_str());
    if (NULL == dict)
    {
        return 0;
    }

    dictionary *subdict = iniparser_load(subfile.c_str());
    if (NULL == subdict)
    {
        return 0;
    }
    const char *session = NULL;
    int nsec = 0;
    int nkey = 0;
    const char **arry_key = NULL;
    const char **str_rc = NULL;
    int int_rc = 0;
    const char *str_value = NULL;

    nsec = iniparser_getnsec(subdict);
    for (int i = 0; i < nsec; ++i)
    {
        session = iniparser_getsecname(subdict, i);
        if (session != NULL)
        {
            //COUT << "session: " << session << "\n";
            nkey = iniparser_getsecnkeys(subdict, session);
            arry_key = (const char**)malloc(sizeof(char*) * nkey);
            str_rc = iniparser_getseckeys(subdict, session, arry_key);  
            if (str_rc == arry_key)
            {
                for (int k = 0; k < nkey; ++k)
                {
                    //COUT << "key: " << arry_key[k] << "\n";
                    int_rc = iniparser_find_entry(dict, arry_key[k]);
                    if (1 == int_rc)
                    {
                       str_value = iniparser_getstring(subdict, arry_key[k], "null-1"); 
                       //COUT << "rc[" << int_rc << "], key[" << arry_key[k] << "] value[" << str_value << "]\n";
                       iniparser_set(dict, arry_key[k], str_value);
                    }
                    else
                    {

                       //COUT << "rc[" << int_rc << "], key[" << arry_key[k] << "]\n";
                    }
                }
            }
        }
    }

    iniparser_freedict(subdict);

    iniparser_save(dict, file.c_str());
    iniparser_freedict(dict);

    return 0;
}

int handler_dbg(std::vector<fastring> v)
{
    
    fastring file(v[0]);

    dictionary *dict = iniparser_load(file.c_str());
    if (NULL == dict)
    {
        return 0;
    }
    iniparser_save(dict, file.c_str());
    iniparser_freedict(dict);

    return 0;
}
