#include "base/flag.h"
#include "base/log.h"

DEF_bool(boo, false, "bool flag");
DEF_string(s, "hello world", "string");

int main(int argc, char** argv)
{
    auto args = flag::init(argc, argv);

    COUT << "boo: " << FLG_boo;
    COUT << "s: " << FLG_s;
    
    return 0;
}
