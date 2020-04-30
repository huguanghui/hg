#include <iostream>
#include "base/def.h"
#include "base/fastring.h"
#include "base/str.h"
#include "base/flag.h"
#include "base/fs.h"
#include "base/log.h"

DEF_string(type, "TYPE_ALL", "boardtype");

int main(int argc, char *argv[])
{
    auto v = flag::init(argc, argv);
    log::init();

    fastring type(FLG_type);

    COUT << "type: " << type;

    std::vector<fastring> arry_type = str::split(type, ' ');

    std::vector<fastring>::iterator it;
    for (it=arry_type.begin(); it != arry_type.end(); it++)
    {
        if (strlen((*it).c_str()) > 0)
        {
            COUT << *it;
        }
    }
    COUT << "end!";
    
    return 0;
}
